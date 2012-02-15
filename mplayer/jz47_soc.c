
#include <mplaylib.h>
#include <mplaylib.h>
#include <assert.h>
#include <time.h>
#include "config.h"
#include "mp_msg.h"
#include "sys/stat.h"
#include "sys/fcntl.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include <mplaylib.h>

#include "libavcodec/avcodec.h"
#include "libmpcodecs/img_format.h"
#include "stream/stream.h"
#include "libmpdemux/demuxer.h"
#include "libmpdemux/stheader.h"

#include "libmpcodecs/img_format.h"
#include "libmpcodecs/mp_image.h"
#include "libmpcodecs/vf.h"
#include "libswscale/swscale.h"
#include "libswscale/swscale_internal.h"
#include "libmpcodecs/vf_scale.h"

#include "jzsoc/linux/jz4750_ipu_regops.h"
#include "jzsoc/linux/jz47_iputype.h"
extern void noah_kprintf(const char *format, ... );
extern void noah_open_osd(int w, int y);

typedef struct {
    AVCodecContext *avctx;
    AVFrame *pic;
    enum PixelFormat pix_fmt;
    float last_aspect;
    int do_slices;
    int do_dr1;
    int vo_inited;
    int best_csp;
    int b_age;
    int ip_age[2];
    int qp_stat[32];
    double qp_sum;
    double inv_qp_sum;
    int ip_count;
    int b_count;
} vd_ffmpeg_ctx;

struct vf_priv_s {
    int w,h;
    int v_chr_drop;
    double param[2];
    unsigned int fmt;
    struct SwsContext *ctx;
    struct SwsContext *ctx2; //for interlaced slices only
    unsigned char* palette;
    int interlaced;
    int noup;
    int accurate_rnd;
    int query_format_cache[64];
};

/* ================================================================================ */
#define IPU_OUT_FB        0
#define IPU_OUT_LCD       1
#define IPU_OUT_PAL_TV    2
#define IPU_OUT_NTSC_TV   3
#define IPU_OUT_MEM       8

#define DEBUG_LEVEL  1  /* 1 dump resize message, 2 dump register for every frame.  */

#define CPU_TYPE 4755
#define OUTPUT_MODE IPU_OUT_FB

int noah_get_ipu_status();
extern void __dcache_writeback_all(void);

/* ================================================================================ */
static int xpos = -1, ypos = -1;
static int scale_outW=0;  // -1, calculate the w,h base the panel w,h; 0, use the input w,h
static int scale_outH=0;

/* Flag to indicate the module init status */
int jz47_ipu_module_init = 0; 

int ipu_image_completed = 0;

/* CPU type: 4740, 4750, 4755, 4760 */
int jz47_cpu_type = CPU_TYPE;

/* flush the dcache.  */
//unsigned int dcache[4096];

unsigned char *jz47_ipu_output_mem_ptr = NULL;

/* ================================================================================ */
extern int lcd_get_width (), lcd_get_height (), lcd_get_line_length ();
extern void* lcd_get_frame ();
extern int jz47_calc_resize_para ();
/* ================================================================================ */
#define get_phy_addr(x) ((unsigned int )(x) & 0x1fffffff)

/* ipu virtual address.   */
volatile unsigned char *ipu_vbase = (unsigned char *)(IPU__OFFSET | 0xa0000000);

/* struct IPU module to recorde some info */
struct JZ47_IPU_MOD jz47_ipu_module = {
    .output_mode = OUTPUT_MODE, /* Use the frame for the default */ 
    .rsize_algorithm = 0,      /* 0: liner, 1: bicube, 2: biliner.  */
    .rsize_bicube_level = -2,     /*-2, -1, -0.75, -0.5 */
 };

/* ================================================================================ */
static char *ipu_regs_name[] = {
    "REG_CTRL",         /* 0x0 */ 
    "REG_STATUS",       /* 0x4 */ 
    "REG_D_FMT",        /* 0x8 */ 
    "REG_Y_ADDR",       /* 0xc */ 
    "REG_U_ADDR",       /* 0x10 */ 
    "REG_V_ADDR",       /* 0x14 */ 
    "REG_IN_FM_GS",     /* 0x18 */ 
    "REG_Y_STRIDE",     /* 0x1c */ 
    "REG_UV_STRIDE",    /* 0x20 */ 
    "REG_OUT_ADDR",     /* 0x24 */ 
    "REG_OUT_GS",       /* 0x28 */ 
    "REG_OUT_STRIDE",   /* 0x2c */ 
    "RSZ_COEF_INDEX",   /* 0x30 */ 
    "REG_CSC_C0_COEF",  /* 0x34 */ 
    "REG_CSC_C1_COEF",  /* 0x38 */ 
    "REG_CSC_C2_COEF",  /* 0x3c */ 
    "REG_CSC_C3_COEF",  /* 0x40 */ 
    "REG_CSC_C4_COEF",  /* 0x44 */
    "REG_H_LUT",        /* 0x48 */ 
    "REG_V_LUT",        /* 0x4c */ 
    "REG_CSC_OFFPARA",  /* 0x50 */
};

static int jz47_dump_ipu_regs(int num)
{
  int i, total;
  if (num >= 0)
  {
    noah_kprintf ("ipu_reg: %s: 0x%x\n", ipu_regs_name[num >> 2], REG32(ipu_vbase + num));
  	return 1;
  }
  if (num == -1)
  {
    total = sizeof (ipu_regs_name) / sizeof (char *);
    for (i = 0; i < total; i++)
      noah_kprintf ("ipu_reg: %s: 0x%x\n", ipu_regs_name[i], REG32(ipu_vbase + (i << 2)));
  }
  return 1;
}

/* ================================================================================ */

static int jz47_get_output_panel_info (void)
{
  int output_mode = jz47_ipu_module.output_mode;

  /* For JZ4740 cpu, We haven't TV output and IPU through */
  if (jz47_cpu_type == 4740 && output_mode != IPU_OUT_FB && output_mode != IPU_OUT_MEM)
    jz47_ipu_module.output_mode = output_mode = IPU_OUT_FB;

  switch (output_mode)
  {
    case IPU_OUT_MEM:
      jz47_ipu_module.out_panel.w = 480;
      jz47_ipu_module.out_panel.h = 272;
      jz47_ipu_module.out_panel.bpp_byte = 4;
      jz47_ipu_module.out_panel.bytes_per_line = 480 * 4;
      jz47_ipu_module.out_panel.output_phy = get_phy_addr ((unsigned int)jz47_ipu_output_mem_ptr);
      break;

    case IPU_OUT_FB:
    default:
      /* set the output panel info */
      jz47_ipu_module.out_panel.w = lcd_get_width ();
      jz47_ipu_module.out_panel.h = lcd_get_height ();
      jz47_ipu_module.out_panel.bytes_per_line = lcd_get_line_length ();
      jz47_ipu_module.out_panel.bpp_byte = lcd_get_line_length () / lcd_get_width ();
      jz47_ipu_module.out_panel.output_phy = get_phy_addr ((unsigned int)lcd_get_frame ());
      
      noah_kprintf ("+++ w = %d, h = %d, line = %d, bpp = %d, phy=0x%08x\n",
                jz47_ipu_module.out_panel.w, jz47_ipu_module.out_panel.h, 
                jz47_ipu_module.out_panel.bytes_per_line, jz47_ipu_module.out_panel.bpp_byte,
                jz47_ipu_module.out_panel.output_phy);
      
      break;
  }
#if 0  
  {
  	int i;
  	unsigned char *p = lcd_get_frame ();
  	for (i = 0; i < lcd_get_line_length () * lcd_get_height () * 2; i+=4)
  	{
  	  *(p + i + 0) = 0xff;
  	  *(p + i + 1) = 0x0;
  	  *(p + i + 2) = 0x0;
  	  *(p + i + 3) = 0x0;
  	}
  	__dcache_writeback_all();
  	  /* flush dcache if need.  */
//      memset (dcache, 0x2, 0x4000);
  }
  noah_kprintf ("+++ return ++++\n");
  while (1);
#endif  
  return 1;
}

/* ================================================================================ */
/*
  x = -1, y = -1 is center display
  w = -1, h = -1 is orignal w,h
  w = -2, h = -2 is auto fit
  other: specify  by user
*/

static int jz47_calc_ipu_outsize_and_position (int x, int y, int w, int h)
{
  int dispscr_w, dispscr_h;
  int orignal_w = jz47_ipu_module.srcW;
  int orignal_h = jz47_ipu_module.srcH;

  /* record the orignal setting */
  jz47_ipu_module.out_x = x;
  jz47_ipu_module.out_y = y;
  jz47_ipu_module.out_w = w;
  jz47_ipu_module.out_h = h;

  // The MAX display area which can be used by ipu 
  dispscr_w = (x < 0) ? jz47_ipu_module.out_panel.w : (jz47_ipu_module.out_panel.w - x);
  dispscr_h = (y < 0) ? jz47_ipu_module.out_panel.h : (jz47_ipu_module.out_panel.h - y);

  // Orignal size playing or auto fit screen playing mode
  if ((w == -1 && h == -1 && (orignal_w > dispscr_w ||  orignal_h > dispscr_h)) || (w == -2 || h == -2))
  {
    float scale_h = (float)orignal_h / dispscr_h;
    float scale_w = (float)orignal_w / dispscr_w;
    if (scale_w > scale_h)
    {
      w = dispscr_w;
      h = (dispscr_w * orignal_h) / orignal_w;
    }
    else
    {
      h = dispscr_h;
      w = (dispscr_h * orignal_w) / orignal_h;
    }
  }
  
  // w,h is orignal w,h
  w = (w == -1)? orignal_w : w;
  h = (h == -1)? orignal_h : h;

  // w,h must < dispscr_w,dispscr_h
  w = (w > dispscr_w)? dispscr_w : w;
  h = (h > dispscr_h)? dispscr_h : h;

  // w,h must <= 2*(orignal_w, orignal_h)
  w = (w > 2 * orignal_w) ? (2 * orignal_w) : w;
  h = (h > 2 * orignal_h) ? (2 * orignal_h) : h;

  // calc output position out_x, out_y
  jz47_ipu_module.act_x = (x == -1) ? ((jz47_ipu_module.out_panel.w - w) / 2) : x;
  jz47_ipu_module.act_y = (y == -1) ? ((jz47_ipu_module.out_panel.h - h) / 2) : y;

  // set the resize_w, resize_h
  jz47_ipu_module.act_w = w;
  jz47_ipu_module.act_h = h;

  jz47_ipu_module.need_config_resize = 1;
  jz47_ipu_module.need_config_outputpara = 1;
  return 1;
}

/* ================================================================================ */
static int jz47_config_stop_ipu ()
{
  int switch_mode, runing_mode;

  /* Get the runing mode class.  */
  if (jz47_cpu_type == 4740
      || jz47_ipu_module.ipu_working_mode == IPU_OUT_FB
      || jz47_ipu_module.ipu_working_mode == IPU_OUT_MEM)
    runing_mode = 'A';
  else 
    runing_mode = 'B';
  
  /* Get the switch mode class.  */
  if (jz47_cpu_type == 4740
      || jz47_ipu_module.output_mode == IPU_OUT_FB
      || jz47_ipu_module.output_mode == IPU_OUT_MEM)
    switch_mode = 'A';
  else 
    switch_mode = 'B';

  /* Base on the runing_mode and switch_mode, disable lcd or stop ipu.  */
  if (runing_mode == 'A' && switch_mode == 'A')
  {
    while (ipu_is_enable(ipu_vbase) && (!polling_end_flag(ipu_vbase)))
      ;
    stop_ipu(ipu_vbase);
    clear_end_flag(ipu_vbase);
  }
  return 1;
}

/* ================================================================================ */
static void jz47_config_ipu_input_para (SwsContext *c, mp_image_t *mpi)
{
  unsigned int in_fmt;
  unsigned int srcFormat = c->srcFormat;

  in_fmt = INFMT_YCbCr420; // default value
  if (jz47_ipu_module.need_config_inputpara)
  {
    /* Set input Data format.  */
    switch (srcFormat)
    {
      case PIX_FMT_YUV420P:
        in_fmt = INFMT_YCbCr420;
      break;

      case PIX_FMT_YUV422P:
        in_fmt = INFMT_YCbCr422;
      break;

      case PIX_FMT_YUV444P:
        in_fmt = INFMT_YCbCr444;
      break;

      case PIX_FMT_YUV411P:
        in_fmt = INFMT_YCbCr411;
      break;
    }
    REG32 (ipu_vbase + REG_D_FMT) &= ~(IN_FMT_MASK);
    REG32 (ipu_vbase + REG_D_FMT) |= in_fmt;

    /* Set input width and height.  */
    REG32(ipu_vbase + REG_IN_FM_GS) = IN_FM_W(jz47_ipu_module.srcW) | IN_FM_H (jz47_ipu_module.srcH & ~1);

    /* Set the CSC COEF */
    REG32(ipu_vbase + REG_CSC_C0_COEF) = YUV_CSC_C0;
    REG32(ipu_vbase + REG_CSC_C1_COEF) = YUV_CSC_C1;
    REG32(ipu_vbase + REG_CSC_C2_COEF) = YUV_CSC_C2;
    REG32(ipu_vbase + REG_CSC_C3_COEF) = YUV_CSC_C3;
    REG32(ipu_vbase + REG_CSC_C4_COEF) = YUV_CSC_C4;

    if (jz47_cpu_type != 4740)
      REG32(ipu_vbase + REG_CSC_OFFPARA) = YUV_CSC_OFFPARA;

    /* Configure the stride for YUV.  */
    REG32(ipu_vbase + REG_Y_STRIDE) = mpi->stride[0];
    REG32(ipu_vbase + REG_UV_STRIDE) = U_STRIDE(mpi->stride[1]) | V_STRIDE(mpi->stride[2]);
  }

  /* Set the YUV addr.  */
  REG32(ipu_vbase + REG_Y_ADDR) = get_phy_addr ((unsigned int)mpi->planes[0]);
  REG32(ipu_vbase + REG_U_ADDR) = get_phy_addr ((unsigned int)mpi->planes[1]);
  REG32(ipu_vbase + REG_V_ADDR) = get_phy_addr ((unsigned int)mpi->planes[2]);

}

/* ================================================================================ */
static void jz47_config_ipu_resize_para ()
{
	int i, width_resize_enable, height_resize_enable;
	int width_up, height_up, width_lut_size, height_lut_size;

	width_resize_enable  = jz47_ipu_module.resize_para.width_resize_enable;
	height_resize_enable = jz47_ipu_module.resize_para.height_resize_enable;
	width_up  =  jz47_ipu_module.resize_para.width_up;
	height_up =  jz47_ipu_module.resize_para.height_up;
	width_lut_size  = jz47_ipu_module.resize_para.width_lut_size;
	height_lut_size = jz47_ipu_module.resize_para.height_lut_size;

	/* Enable the rsize configure.  */
	disable_rsize (ipu_vbase);

	if (width_resize_enable)
	enable_hrsize (ipu_vbase);

	if (height_resize_enable)
	enable_vrsize (ipu_vbase);

	/* select the resize algorithm.  */
#ifdef JZ4750_OPT
	if (jz47_cpu_type == 4760 && jz47_ipu_module.rsize_algorithm)     /* 0: liner, 1: bicube, 2: biliner.  */
		enable_ipu_bicubic(ipu_vbase);
	else
		disable_ipu_bicubic(ipu_vbase);
#endif

	/* Enable the scale configure.  */
	REG32 (ipu_vbase + REG_CTRL) &= ~((1 << V_SCALE_SHIFT) | (1 << H_SCALE_SHIFT));
	REG32 (ipu_vbase + REG_CTRL) |= (height_up << V_SCALE_SHIFT) | (width_up << H_SCALE_SHIFT);

	/* Set the LUT index.  */
	REG32 (ipu_vbase + REG_RSZ_COEF_INDEX) = (((height_lut_size - 1) << VE_IDX_SFT) 
											| ((width_lut_size - 1) << HE_IDX_SFT));

	/* set lut. */
	if (jz47_cpu_type == 4740)
	{
		if (height_resize_enable)
		{
		  for (i = 0; i < height_lut_size; i++)
			REG32 (ipu_vbase + VRSZ_LUT_BASE + i * 4) = jz47_ipu_module.resize_para.height_lut_coef[i];
		}
		else
		  REG32 (ipu_vbase + VRSZ_LUT_BASE) = ((128 << 2)| 0x3);

		if (width_resize_enable)
		{
		  for (i = 0; i < width_lut_size; i++)
			REG32 (ipu_vbase + HRSZ_LUT_BASE + i * 4) = jz47_ipu_module.resize_para.width_lut_coef[i];
		}
		else
		  REG32 (ipu_vbase + HRSZ_LUT_BASE) = ((128 << 2)| 0x3);
	}
	else
	{
		REG32 (ipu_vbase + VRSZ_LUT_BASE) = (1 << START_N_SFT);
		for (i = 0; i < height_lut_size; i++)
		  if (jz47_cpu_type == 4760 && jz47_ipu_module.rsize_algorithm)     /* 0: liner, 1: bicube, 2: biliner.  */
		  {
			REG32 (ipu_vbase + VRSZ_LUT_BASE) = jz47_ipu_module.resize_para.height_bicube_lut_coef[2*i + 0];
			REG32 (ipu_vbase + VRSZ_LUT_BASE) = jz47_ipu_module.resize_para.height_bicube_lut_coef[2*i + 1];
		  }
		  else
			REG32 (ipu_vbase + VRSZ_LUT_BASE) = jz47_ipu_module.resize_para.height_lut_coef[i];

		REG32 (ipu_vbase + HRSZ_LUT_BASE) = (1 << START_N_SFT);
		for (i = 0; i < width_lut_size; i++)
		  if (jz47_cpu_type == 4760 && jz47_ipu_module.rsize_algorithm)     /* 0: liner, 1: bicube, 2: biliner.  */
		  {
			REG32 (ipu_vbase + HRSZ_LUT_BASE) = jz47_ipu_module.resize_para.width_bicube_lut_coef[2*i + 0];
			REG32 (ipu_vbase + HRSZ_LUT_BASE) = jz47_ipu_module.resize_para.width_bicube_lut_coef[2*i + 1];
		  }
		  else
			REG32 (ipu_vbase + HRSZ_LUT_BASE) = jz47_ipu_module.resize_para.width_lut_coef[i];
	}

	/* dump the resize messages.  */
	if (DEBUG_LEVEL > 0)
	{
		noah_kprintf ("panel_w = %d, panel_h = %d, srcW = %d, srcH = %d\n",
				jz47_ipu_module.out_panel.w, jz47_ipu_module.out_panel.h, 
				jz47_ipu_module.srcW, jz47_ipu_module.srcH);
		noah_kprintf ("out_x = %d, out_y = %d, out_w = %d, out_h = %d\n", 
				jz47_ipu_module.out_x, jz47_ipu_module.out_y, 
				jz47_ipu_module.out_w, jz47_ipu_module.out_h);
		noah_kprintf ("act_x = %d, act_y = %d, act_w = %d, act_h = %d, outW = %d, outH = %d\n", 
				jz47_ipu_module.act_x, jz47_ipu_module.act_y, 
				jz47_ipu_module.act_w, jz47_ipu_module.act_h,
				jz47_ipu_module.resize_para.outW, jz47_ipu_module.resize_para.outH);
	}
	
	noah_open_osd(jz47_ipu_module.resize_para.outW,jz47_ipu_module.resize_para.outH);
}

extern int noah_get_out_format(void);
/* ================================================================================ */
static void  jz47_config_ipu_output_para (SwsContext *c)
{
  int frame_offset, out_x, out_y;
  int rsize_w, rsize_h, outW, outH;
  int output_mode = jz47_ipu_module.output_mode;
  unsigned int out_fmt, dstFormat = c->dstFormat;
   
  /* Get the output parameter from struct.  */
  outW = jz47_ipu_module.resize_para.outW;
  outH = jz47_ipu_module.resize_para.outH;
  out_x = jz47_ipu_module.act_x;
  out_y = jz47_ipu_module.act_y;
  rsize_w = jz47_ipu_module.act_w;
  rsize_h = jz47_ipu_module.act_h;

  /* outW must < resize_w and outH must < resize_h.  */
  outW = (outW <= rsize_w) ? outW : rsize_w;
  outH = (outH <= rsize_h) ? outH : rsize_h;

  /* calc the offset for output.  */
  frame_offset = (out_x + out_y * jz47_ipu_module.out_panel.w) * jz47_ipu_module.out_panel.bpp_byte;
  out_fmt = OUTFMT_RGB565;  // default value
  dstFormat = noah_get_out_format();	//PIX_FMT_RGBA32;

  /* clear some output control bits.  */
  disable_ipu_direct(ipu_vbase);

  switch (dstFormat)
  {
    case PIX_FMT_RGBA32:
      out_fmt = OUTFMT_RGB888;
      outW = outW << 2;
      break;

    case PIX_FMT_RGB555:
      out_fmt = OUTFMT_RGB555;
      outW = outW << 1;
      break;

    case PIX_FMT_RGB565:
    case PIX_FMT_BGR565:
      out_fmt = OUTFMT_RGB565;
      outW = outW << 1;
      break;
  }

  /* clear the OUT_DATA_FMT control bits.  */
  REG32 (ipu_vbase + REG_D_FMT) &= ~(OUT_FMT_MASK);

  switch (output_mode)
  {
    case IPU_OUT_FB:
    default:
      REG32(ipu_vbase + REG_OUT_ADDR) = jz47_ipu_module.out_panel.output_phy + frame_offset;
      REG32(ipu_vbase + REG_OUT_STRIDE) = jz47_ipu_module.out_panel.bytes_per_line;
      break;
  }

  REG32(ipu_vbase + REG_OUT_GS) = OUT_FM_W (outW) | OUT_FM_H (outH);
  REG32 (ipu_vbase + REG_D_FMT) |= out_fmt;
  REG32 (ipu_vbase + REG_CTRL) |= CSC_EN;
}

/* ================================================================================ */

static int  jz47_config_run_ipu ()
{
  int output_mode = jz47_ipu_module.output_mode;

  /* set the ipu working mode.  */
  jz47_ipu_module.ipu_working_mode = output_mode;

  if (jz47_cpu_type == 4740)
  {
    run_ipu (ipu_vbase);
    return 1;
  }

  /* run ipu for different output mode.  */
  switch (output_mode)
  {
    case IPU_OUT_FB:
    case IPU_OUT_MEM:
    default:
      run_ipu (ipu_vbase);
      break;
  }

  return 1;
}

/* ================================================================================ */
int jz47_put_image_with_ipu (struct vf_instance_s *vf, mp_image_t *mpi, double pts)
{
  SwsContext *c = vf->priv->ctx;

//  kprintf("jz47_put_image_with_ipu\n");
  /* Get the output panel information, calc the output position and shapes */
  if (!jz47_ipu_module_init)
  {
    if (! jz47_get_output_panel_info ())
      return 0;
    /* input size */
    jz47_ipu_module.srcW = c->srcW;
    jz47_ipu_module.srcH = c->srcH;
    /* output size */
    scale_outW = (!scale_outW) ? c->dstW : scale_outW;
    scale_outH = (!scale_outH) ? c->dstH : scale_outH;
    /* calculate input/output size. */
    jz47_calc_ipu_outsize_and_position (xpos, ypos, scale_outW, scale_outH);

    jz47_ipu_module.need_config_resize = 1;
    jz47_ipu_module.need_config_inputpara = 1;
    jz47_ipu_module.need_config_outputpara = 1;
    jz47_ipu_module_init = 1;
    reset_ipu (ipu_vbase);
  }

  if( noah_get_ipu_status() )
  {
	/* set some flag for normal.  */
	jz47_ipu_module.need_config_resize = 0;
	jz47_ipu_module.need_config_inputpara = 0;
	jz47_ipu_module.need_config_outputpara = 0;
	ipu_image_completed = 1;
	return 1;
  }
  
	
  /* calculate resize parameter.  */
  if (jz47_ipu_module.need_config_resize)
    jz47_calc_resize_para ();

  /* Following codes is used to configure IPU, so we need stop the ipu.  */
  jz47_config_stop_ipu ();

  /* configure the input parameter.  */
  jz47_config_ipu_input_para (c, mpi);

  /* configure the resize parameter.  */
  if (jz47_ipu_module.need_config_resize)
    jz47_config_ipu_resize_para ();

  /* configure the output parameter.  */
  if (jz47_ipu_module.need_config_outputpara)
    jz47_config_ipu_output_para (c);

  /* flush dcache if need.  */
  {
  	__dcache_writeback_all();
//    unsigned int img_area = c->srcW * c->srcH;
//    if (img_area <= 320 * 240)
//      memset (dcache, 0x2, 0x4000);
  }

  /* run ipu */
  jz47_config_run_ipu ();

  if (DEBUG_LEVEL > 1)
    jz47_dump_ipu_regs(-1);

  /* set some flag for normal.  */
  jz47_ipu_module.need_config_resize = 0;
  jz47_ipu_module.need_config_inputpara = 0;
  jz47_ipu_module.need_config_outputpara = 0;
  ipu_image_completed = 1;
//  kprintf("ipu end\n");
  return 1;
}

void ipu_polling_end ()
{
  while(ipu_is_enable(ipu_vbase) && (!polling_end_flag(ipu_vbase)));
}
/* Following function is the interface.  */
/* ================================================================================ */


void  ipu_image_start()
{
  ipu_image_completed = 0;
  jz47_ipu_module_init = 0;
}

static int visual = 0;
void drop_image ()
{
  visual = 0;
}

int jz47_put_image (struct vf_instance_s *vf, mp_image_t *mpi, double pts)
{
  mp_image_t *dmpi=mpi->priv;

  if (!(mpi->flags & MP_IMGFLAG_DRAW_CALLBACK && dmpi))
  {
    if (mpi->pict_type == 1)  /* P_type */
      visual = 1;
    if (visual || (mpi->pict_type == 0)) /* I_type */
      jz47_put_image_with_ipu (vf, mpi, pts);
  }
//while (1);
  return 1;
}

void  IpuOutmodeChanged(int x,int y,int w, int h)
{
  /* caculate the x, y, w, h */
  jz47_calc_ipu_outsize_and_position(x, y,  w, h);
  jz47_ipu_module.need_config_resize = 1;
  jz47_ipu_module.need_config_outputpara = 1;
}

void ipu_init_lcd_size(int x,int y,int w, int h)
{
	xpos = x;
	ypos = y;
	scale_outW = w;  // -1, calculate the w,h base the panel w,h; 0, use the input w,h
	scale_outH = h;
}


