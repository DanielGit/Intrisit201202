#include <mplaylib.h>
#include "mplaylib.h"

//#include <assert.h>

#include "mplayertm.h"


#include "config.h"
#include "mp_msg.h"
//#include "sys/stat.h"
//#include "sys/fcntl.h"
//#include "sys/mman.h"
//#include "fcntl.h"
//#include <linux/fb.h>


//#include "mp4_stream.h"
#include "avcodec.h"
#include "img_format.h"
//#include "stream.h"
//#include "demuxer.h"
//#include "stheader.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"
#include "libswscale/swscale.h"
#include "libswscale/swscale_internal.h"
#include "vf_scale.h"

#include "jz4740_ipu.h"
#include <uclib.h>
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

#define PAGE_SIZE (4096)
#define MEM_ALLOC_DEV_NUM 4

//static unsigned int dcache[4096];

#define ipu_vbase  (IPU__OFFSET | 0xa0000000)

static int ipu_inited = 0, ipu_size_cfg = 0, get_fbinfo_already = 0;
static int ipu_rtable_init = 0, ipu_rtable_len;
static int  ipu_maped = 0, ipu_image_completed = 0;
static volatile unsigned char *img_ptr=NULL, *frame_buffer=NULL;
static unsigned int fb_w, fb_h, phy_fb = 0, fbmemlen, fb_line_len;
static unsigned int rsize_w = 0, rsize_h = 0;
static unsigned int out_x,out_y,out_w,out_h,frame_offset = 0;

#define IPU_LUT_LEN 20
struct Ration2m ipu_ratio_table[(IPU_LUT_LEN) * (IPU_LUT_LEN)];
rsz_lut h_lut[IPU_LUT_LEN];
rsz_lut v_lut[IPU_LUT_LEN];


//modify  by jliu  2009.2.10
#define FRAMEBUF_16BIT 0
/*
#define DISPLAY_DIRECT 1
#define DISPLAY_TV_OUT 2
#define DISPLAY_ALL    3
*/
#define DISPLAY_BUFFER     0x00
#define DECODE_BUFFER  	   0x01
static int display_mode = 0;
//modify end
#define MP_IMAGE_TYOE_SIZE_FINISH 0
#define MP_IMAGE_TYPE_CUSTORM     1
#define MP_IMAGE_TYPE_INITSIZE    2
#define MP_IMAGE_TYPE_EQSIZE      3
#define MP_IMAGE_TYPE_FULLSCREEN  4

static unsigned int cur_frame_buf_address = 0;
/*
#define LCD_BPP 32
#define LCD_SHOWMODE PIX_FMT_RGBA32 
*/
#define LCD_BPP 16
#define LCD_SHOWMODE PIX_FMT_RGB565

#define FB_LCD_WIDTH lcd_get_width()
#define FB_LCD_HEIGHT lcd_get_height()

//PIX_FMT_RGB565


#undef printf
#if 1
static unsigned int frame_buf[12] = {0};

void add_frame_buf(unsigned int frame)
{
	static int count = 0;
	int i;
	memcpy(frame_buf,&frame_buf[1],11 * 4);
	frame_buf[11] = frame;
	count++;
	if((frame_buf[8] != frame_buf[11]) && (frame_buf[0] != 0))
	{
			printf("---------------%d-------------\n",count);
			for(i = 0;i < 12 / 4 ;i++)
				printf("0:%x 1:%x 2:%x 3:%x\n",frame_buf[i*4 + 0],frame_buf[i*4 + 1],frame_buf[i*4 + 2],frame_buf[i*4 + 3]);
	}

}
#endif
#define LCD_BPP_BYTE (LCD_BPP / 8)
void get_fbaddr_info ()
{

  fb_w = FB_LCD_WIDTH;
  fb_h = FB_LCD_HEIGHT;
	phy_fb= (lcd_get_frame() & 0x1fffffff);
  fb_line_len = FB_LCD_WIDTH * LCD_BPP_BYTE;
  fbmemlen = FB_LCD_WIDTH * LCD_BPP_BYTE *FB_LCD_HEIGHT;

  frame_buffer = (uint8_t *)lcd_get_frame();

  printf ("fb_w=%d, fb_h=%d, phy_fb=0x%x, fbmemlen=%d, fb_line_len=%d\n",
           fb_w, fb_h, phy_fb, fbmemlen, fb_line_len);
}

#define get_phy_addr(vaddr) (vaddr & 0x1FFFFFFF)
#if 0
unsigned int get_phy_addr (unsigned int vaddr)
{
  int i;
  vaddr = vaddr & 0x1FFFFFFF;
  return vaddr;
}
#endif

/*----------------------------------------------------------------------------------*/

char *ipu_regs_name[] = {
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
};
typedef void (*PFunc_Hook_OSD)(void *fram);
PFunc_Hook_OSD pFunction = NULL;
//SetOSDFunc(d);
void SetOSDFunc(PFunc_Hook_OSD pFunc)
{
	pFunction = pFunc;
}
int jz47_dump_ipu_regs(int num)
{
  int i, total;
  if (num >= 0)
  {
    printf ("ipu_reg: %s: 0x%x\n", ipu_regs_name[num >> 2], REG32(ipu_vbase + num));
    return 1;
  }
  if (num == -1)
  {
    total = sizeof (ipu_regs_name) / sizeof (char *);

    for (i = 0; i < total; i++)
      printf ("ipu_reg: %s: 0x%x\n", ipu_regs_name[i], REG32(ipu_vbase + (i << 2)));
  }
  if (num == -2)
  {
    for (i = 0; i < IPU_LUT_LEN; i++)
      printf ("ipu_H_LUT(%d): 0x%x\n", i, REG32(ipu_vbase + HRSZ_LUT_BASE + i * 4));
    for (i = 0; i < IPU_LUT_LEN; i++)
      printf ("ipu_V_LUT(%d): 0x%x\n", i, REG32(ipu_vbase + VRSZ_LUT_BASE + i * 4));
  }
  return 1;
}

/* set ipu data format.  */
int jz47_set_ipu_csc_cfg (SwsContext *c, int outW, int outH, int Wdiff, int Hdiff)
{
  unsigned int in_fmt = 0, out_fmt = 0;
  //const int dstFormat= PIX_FMT_RGB565; //c->dstFormat;
  const int dstFormat= LCD_SHOWMODE; //c->dstFormat;
  const int srcFormat= c->srcFormat;

  switch (srcFormat)
  {
    case PIX_FMT_YUV420P:
      Hdiff = (Hdiff + 1) & ~1;
      Wdiff = (Wdiff + 1) & ~1;
      in_fmt = INFMT_YCbCr420;
      break;

    case PIX_FMT_YUV422P:
      Wdiff = (Wdiff + 1) & ~1;
      in_fmt = INFMT_YCbCr422;
      break;

    case PIX_FMT_YUV444P:
      in_fmt = INFMT_YCbCr444;
      break;

    case PIX_FMT_YUV411P:
      in_fmt = INFMT_YCbCr411;
      break;
  }

  int out_width =outW;
  int out_height=outH;
  F("outW = %d  outH %d",outW,outH);
 // printf ("++++++++0 outW=%d dstFormat=%d\n", outW, dstFormat);
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
      out_fmt = OUTFMT_RGB565;
      outW = outW << 1;
      break;
  }
//  printf ("++++++++1 outW=%d PIX_FMT_RGB565=%d\n", outW, PIX_FMT_RGB565);
  
// Set GS register
  REG32(ipu_vbase + REG_IN_FM_GS) = IN_FM_W(c->srcW - Wdiff) | IN_FM_H((c->srcH - Hdiff) & ~1);
  REG32(ipu_vbase + REG_OUT_GS) = OUT_FM_W(outW) | OUT_FM_H(outH);


// set Format
  REG32(ipu_vbase + REG_D_FMT) = in_fmt | out_fmt;

// set parameter
  REG32(ipu_vbase + REG_CSC_C0_COEF) = YUV_CSC_C0;
  REG32(ipu_vbase + REG_CSC_C1_COEF) = YUV_CSC_C1;
  REG32(ipu_vbase + REG_CSC_C2_COEF) = YUV_CSC_C2;
  REG32(ipu_vbase + REG_CSC_C3_COEF) = YUV_CSC_C3;
  REG32(ipu_vbase + REG_CSC_C4_COEF) = YUV_CSC_C4;
  return 0;
}

int init_ipu_ratio_table ()
{
  int i, j, cnt;
  float r, min, diff;

  // orig table, first calculate
   for (i = 1; i <= (IPU_LUT_LEN); i++)
    for (j = 1; j <= (IPU_LUT_LEN); j++)
    {
      ipu_ratio_table [(i - 1) * 20 + j - 1].ratio = i / (float)j;
      ipu_ratio_table [(i - 1) * 20 + j - 1].n = i;
      ipu_ratio_table [(i - 1) * 20 + j - 1].m = j;
    }

// Eliminate the ratio greater than 1:2
  for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++)
    if (ipu_ratio_table[i].ratio < 0.4999)
      ipu_ratio_table[i].n = ipu_ratio_table[i].m = -1;

// eliminate the same ratio
  for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++)
    for (j = i + 1; j < (IPU_LUT_LEN) * (IPU_LUT_LEN); j++)
    {
      diff = ipu_ratio_table[i].ratio - ipu_ratio_table[j].ratio;
      if (diff > -0.001 && diff < 0.001)
      {
        ipu_ratio_table[j].n = -1;
        ipu_ratio_table[j].m = -1;
      }
    }

// reorder ipu_ratio_table
cnt = 0;
  for (i = 0; i < (IPU_LUT_LEN) * (IPU_LUT_LEN); i++)
    if (ipu_ratio_table[i].n != -1)
    {
      if (cnt != i)
        ipu_ratio_table[cnt] = ipu_ratio_table[i];
      cnt++;
    }
  ipu_rtable_len = cnt;
  printf ("ipu_rtable_len = %d\n", ipu_rtable_len);
}

int find_ipu_ratio_factor (float ratio)
{
  int i, sel; 
  float diff, min;
  sel = ipu_rtable_len;

  for (i = 0; i < ipu_rtable_len; i++)
  {
    if (ratio > ipu_ratio_table[i].ratio)
      diff = ratio - ipu_ratio_table[i].ratio;
    else
      diff = ipu_ratio_table[i].ratio - ratio;

    if (diff < min || i == 0)
    {
      min = diff;
      sel = i;
    }
  }

  printf ("resize: sel = %d, n=%d, m=%d\n", sel, ipu_ratio_table[sel].n,
          ipu_ratio_table[sel].m);
  return sel;
}

int jz47_set_ipu_resize (SwsContext *c, int *outWp, int *outHp,
                         int *Wdiff, int *Hdiff)
{
  int Hsel = 0, Wsel = 0;
  int srcN, dstM, width_up, height_up;
  int Height_lut_max, Width_lut_max;
  int i;

  if (rsize_w == 0)
    rsize_w = c->dstW;
  if (rsize_h == 0)
    rsize_h = c->dstH;
    
  rsize_w = (rsize_w > fb_w) ? fb_w : rsize_w;
  rsize_h = (rsize_h > fb_h) ? fb_h : rsize_h;

  *outWp = rsize_w;
  *outHp = rsize_h;
  *Wdiff = *Hdiff = 0;

  // orig size: c->srcW, c->srcH
  printf ("c->srcW = %d, c->srcH=%d, rsize_w=%d, rsize_h=%d\n", 
          c->srcW, c->srcH, rsize_w, rsize_h);

// Don't need do resize ?
  if ((c->srcW == rsize_w) && (c->srcH == rsize_h) || (rsize_w > 2 * c->srcW)
      || (rsize_h > 2 * c->srcH))
  {
    disable_rsize (ipu_vbase);
    return 0;
  }

// init the resize factor table
  if (!ipu_rtable_init)
  {
    init_ipu_ratio_table ();
    ipu_rtable_init = 1;
  }

// enable resize
  enable_rsize (ipu_vbase);

// get the resize factor
  Wsel = find_ipu_ratio_factor(((float) c->srcW)/rsize_w);
  Hsel = find_ipu_ratio_factor(((float) c->srcH)/rsize_h);

// set IPU_CTRL register
  width_up  = rsize_w >= c->srcW;
  height_up = rsize_h >= c->srcH;
  REG32(ipu_vbase + REG_CTRL) |= (height_up << V_SCALE_SHIFT) | (width_up << H_SCALE_SHIFT);

// set IPU_INDEX register
  Width_lut_max  = width_up  ? ipu_ratio_table[Wsel].m : ipu_ratio_table[Wsel].n;
  Height_lut_max = height_up ? ipu_ratio_table[Hsel].m : ipu_ratio_table[Hsel].n;
  REG32(ipu_vbase + REG_RSZ_COEF_INDEX) =   ((Height_lut_max - 1) << VE_IDX_SFT) 
                                          | ((Width_lut_max - 1)  << HE_IDX_SFT);

// calculate out W/H and LUT
  srcN = ipu_ratio_table[Wsel].n;
  dstM = ipu_ratio_table[Wsel].m;
  *outWp = resize_out_cal (c->srcW, rsize_w, srcN, dstM, width_up,  Wdiff);
  resize_lut_cal (srcN, dstM, width_up,  h_lut);

  srcN = ipu_ratio_table[Hsel].n;
  dstM = ipu_ratio_table[Hsel].m;
  *outHp = resize_out_cal (c->srcH, rsize_h, srcN, dstM, height_up, Hdiff);
  resize_lut_cal (srcN, dstM, height_up, v_lut);

// set IPU LUT register
  for (i = 0; i < Height_lut_max; i++)
    REG32(ipu_vbase + VRSZ_LUT_BASE + i * 4) = (v_lut[i].coef << W_COEF_SFT)
          | (v_lut[i].in_n << IN_N_SFT) | (v_lut[i].out_n  << OUT_N_SFT);
  for (i = 0; i < Width_lut_max; i++)
    REG32(ipu_vbase + HRSZ_LUT_BASE + i * 4) = (h_lut[i].coef << W_COEF_SFT)
          | (h_lut[i].in_n << IN_N_SFT) | (h_lut[i].out_n  << OUT_N_SFT);

  return 0;
}
int resize_out_cal (int insize, int outsize, int srcN, int dstM, 
                    int upScale, int *diff)
{
  float calsize;
  int  delta;
  float tmp, tmp2;
  delta = 1;

  do {
    tmp = (insize - delta) * dstM / (float)srcN;
    tmp2 = tmp  * srcN / dstM;
    if (upScale)
    {
      if (tmp2 == insize - delta)
        calsize = tmp + 1;
      else
        calsize = tmp + 2;
    }
    else   // down Scale
    {
      if (tmp2 == insize - delta)
        calsize = tmp;
      else
        calsize = tmp + 1;
    }
    delta++;
  } while (calsize > outsize);

  *diff = delta - 2;

  return calsize;
}

int resize_lut_cal (int srcN, int dstM, int upScale, rsz_lut lut[])
{
  int i, t, x;
  float w_coef, factor, factor2;

  if (upScale)
  {
    for (i = 0, t=0; i < dstM; i++)
    {
      factor = (float) (i * srcN) / (float)dstM;
      factor2 = factor - (int)factor;
      w_coef = 1.0  - factor2;
      lut[i].coef = (unsigned int)(128.0 * w_coef);
// calculate in & out
      lut[i].out_n = 1;
      if (t <= factor)
      {
        lut[i].in_n = 1;
        t++;
      }
      else
       lut[i].in_n = 0;
    } // end for
  }
  else
  {
    for (i = 0, t = 0, x = 0; i < srcN; i++)
    {
      factor = (float) (t * srcN + 1) / (float)dstM;
      if (dstM == 1)
      {
// calculate in & out
        lut[i].coef =  (i == 0) ? 64 : 0;
        lut[i].out_n = (i == 0) ? 1  : 0;
      }
      else 
      if (((t * srcN + 1) / dstM - i) >= 1)
        lut[i].coef = 0;
      else
      if (factor - i == 0)
      {
        lut[i].coef = 128;
        t++;
      }
      else
      {
        factor2 = (float) (t * srcN) / (float)dstM;
        factor = factor - (int)factor2;
        w_coef = 1.0  - factor;
        lut[i].coef = (unsigned int)(128.0*w_coef);
        t++;
      }
// calculate in & out
      lut[i].in_n = 1;
      if (dstM != 1)
      {
        if (((x * srcN + 1) / dstM - i) >= 1)
          lut[i].out_n = 0;
        else
        {
          lut[i].out_n = 1;
          x++;
        }
      }
    } /* for end down Scale*/
  } /* else upScale */
  return 0;
}
static void eqscale(int w,int h)
{
	int xscale,yscale;
	int scale;

	xscale = w * 1000 / FB_LCD_WIDTH;
	yscale = h * 1000 / FB_LCD_HEIGHT;
	F("\nxscale %d yscale %d\n",xscale,yscale);
	scale = xscale > yscale ? xscale : yscale;

	if(scale <= 500) scale = 500;
	rsize_w = w * 1000 / scale;
	rsize_h = h * 1000 / scale;
	
	if(rsize_h > FB_LCD_HEIGHT) rsize_h = FB_LCD_HEIGHT;
	if(rsize_w > FB_LCD_WIDTH) rsize_w = FB_LCD_WIDTH;
				
	out_x = (FB_LCD_WIDTH - rsize_w) / 2;
	out_y = (FB_LCD_HEIGHT - rsize_h) / 2;
	F("\n3:nrsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);
		
}
static void fullscreen(int w,int h)
{
	int xscale,yscale;
	int scale;
	int scrw,scrh;
	xscale = w * 1000 / FB_LCD_WIDTH;
	yscale = h * 1000 / FB_LCD_HEIGHT;
	
	if(xscale <= 500)
		rsize_w = w * 1000 / 500;
	else
		rsize_w = FB_LCD_WIDTH;
	if(yscale <= 500)
		rsize_h = h * 1000 / 500;
	else
		rsize_h = FB_LCD_HEIGHT;	
	
	if(rsize_h > FB_LCD_HEIGHT) rsize_h = FB_LCD_HEIGHT;
	if(rsize_w > FB_LCD_WIDTH) rsize_w = FB_LCD_WIDTH;
		
	out_x = (FB_LCD_WIDTH - rsize_w) / 2;
	out_y = (FB_LCD_HEIGHT - rsize_h) / 2;
  F("\n4:rsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);
		
}
static unsigned int osd_change = 0;
int jz47_ipu_init (struct vf_instance_s* vf)
{
  SwsContext *c = vf->priv->ctx;
  int outW, outH, Wdiff, Hdiff;
  outW= c->dstW;
  outH= c->dstH;
  if (!ipu_inited)
  {
//init
    stop_ipu  (ipu_vbase);
    reset_ipu (ipu_vbase);
    clear_end_flag (ipu_vbase);

//disable irq
    disable_irq(ipu_vbase);

// set IPU resize field
    jz47_set_ipu_resize (c, &outW, &outH, &Wdiff, &Hdiff);

// set CSC parameter and format
	jz47_set_ipu_csc_cfg(c, outW, outH, Wdiff, Hdiff);
ipu_image_completed = 0;

// set out frame buffer
	
    //REG32(ipu_vbase + REG_OUT_ADDR) = get_phy_addr ((unsigned int)(img_ptr));
    //REG32(ipu_vbase + REG_OUT_ADDR) = get_phy_addr ((unsigned int)frame_buffer);
    REG32(ipu_vbase + REG_OUT_STRIDE) = fb_line_len;
    ipu_inited = 1;
    //ipu_size_cfg = 0;
  }
  if (ipu_size_cfg)
  {
  	outW= c->srcW;
  	outH= c->srcH;
   	switch(ipu_size_cfg)
   	{
		   case MP_IMAGE_TYPE_CUSTORM:
		    break;
		   case MP_IMAGE_TYPE_INITSIZE:
		   	if((outH <= FB_LCD_HEIGHT) && (outW <= FB_LCD_WIDTH))
		   	{	
		   		rsize_w = outW;
    			rsize_h = outH;
    			out_x = (FB_LCD_WIDTH - outW) / 2;
    			out_y = (FB_LCD_HEIGHT - outH) / 2;    			
    		}else
    			eqscale(outW,outH);
		   	break;
		   case MP_IMAGE_TYPE_EQSIZE:
		   	  eqscale(outW,outH);
		   	  break;
		   	break;
		   case MP_IMAGE_TYPE_FULLSCREEN:
		   		fullscreen(outW,outH);
		   	break;	
		}
		// set IPU resize field
		F("\nrsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);
		jz47_set_ipu_resize (c, &outW, &outH, &Wdiff, &Hdiff);
		// set CSC parameter and format
		jz47_set_ipu_csc_cfg(c, outW, outH, Wdiff, Hdiff);
		frame_offset = (out_x + out_y * FB_LCD_WIDTH) * LCD_BPP_BYTE;
		out_w = outW;
		out_h = outH;
		F("ipu_size_cfg = %d frame_offset = %x %x %x\n",ipu_size_cfg,frame_offset,out_x,out_y);
		ipu_size_cfg = 0;  
		lcd_clean_frame_all();
  }
  if(ipu_image_completed)
  {	
  	static unsigned int up_changetime = 0;
					unsigned int time;
					int diff;
					time = GetTimer();
					diff = (int)(time - up_changetime);
					if(diff < 18000)
					{	
						diff = (33000 - diff);
						unsigned int t1,t2 = time;
						while(diff > 1000)
						{
							usec_sleep(diff);
							t1 = GetTimer();
							diff -= (int)(t1 - t2);
							t2 = t1;
						} 
					}
					
  		if(osd_change == 0)
  		{	
	 if(pFunction)
        {
	      	//printf("1: %x\n",lcd_get_change_cframe());
                pFunction(lcd_get_change_cframe());
        }
	
      lcd_change_frame();
    		
    	}else
    	{
    		if(pFunction)
	      {
	      	//printf("3: %x\n",lcd_get_current_cframe());
	        pFunction(lcd_get_current_cframe());
	      }
  }
      lcd_flush_frame_all();	
  		up_changetime = time;
  
  }
  if((osd_change == 0) || (cur_frame_buf_address == 0))
  	cur_frame_buf_address = lcd_get_change_phyframe();
  osd_change = 0;
  add_frame_buf(cur_frame_buf_address);
  REG32(ipu_vbase + REG_OUT_ADDR) = frame_offset + cur_frame_buf_address;//lcd_get_change_phyframe();
  //jz47_dump_ipu_regs(-1);
  return 0;
}
void Flush_OSDScrean()
{
 	 #if 0
 	    while((!polling_end_flag(ipu_vbase)) && ipu_is_enable(ipu_vbase))
		  {
		  	  os_TimeDelay(1);
		  }
	   if(pFunction)
     {
				
		    #if 1
		    if((cur_frame_buf_address != 0) && (cur_frame_buf_address != get_phy_addr(lcd_get_current_cframe())))
				{
					lcd_change_frame();
					osd_change = 1;
				}
				#endif
				//printf("2: %x\n",lcd_get_current_cframe());
				pFunction(lcd_get_current_cframe());
	      lcd_flush_frame_all();
     }
    #endif
}
void Flush_Screen()
{
  while ((!polling_end_flag(ipu_vbase)) && ipu_is_enable(ipu_vbase))
    ;
  if(ipu_image_completed)
  {
         if(pFunction)
        {
						pFunction(lcd_get_current_cframe());
	          lcd_flush_frame_all();
        }

  }
	ipu_image_completed = 0;
}
void  ipu_image_start()
{
  printf ("------ipu_image_start set ipu_inited ------\n");
  ipu_inited = 0;
  ipu_image_completed = 0;
  rsize_w = 0;
  rsize_h = 0;
  out_x = out_y = 0;
  frame_offset = 0;
}

void ipu_image_stop()
{

}
#define copy_image_to_fb() lcd_change_frame()
#if 0
{  
  //memcpy (frame_buffer, img_ptr, fbmemlen);  
    lcd_change_frame();
  return;
}
#endif
int jz47_put_image_with_ipu (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
  unsigned int paddr;
  SwsContext *c = vf->priv->ctx;
  unsigned int img_area = c->srcW * c->srcH;
  if (!get_fbinfo_already)
  {
    get_fbaddr_info();
    get_fbinfo_already = 1;
  }

  
  while ((!polling_end_flag(ipu_vbase)) && ipu_is_enable(ipu_vbase))
  {
  	  os_TimeDelay(1);
  }
  stop_ipu(ipu_vbase);
  clear_end_flag(ipu_vbase);
  jz47_ipu_init (vf);

// set Y(mpi->planes[0]), U(mpi->planes[1]), V addr (mpi->planes[2])
  paddr = get_phy_addr ((unsigned int)mpi->planes[0]);
  if (paddr == 0)
  {
    printf ("+++ Can not found Y buffer(0x%x) physical addr since addr errors +++\n",
            mpi->planes[0]);
   
    return 0;
    
  }
  REG32(ipu_vbase + REG_Y_ADDR) = paddr;

// set U addr register (mpi->planes[1])
  paddr = get_phy_addr ((unsigned int)mpi->planes[1]);
  if (paddr == 0)
  {
    printf ("+++ Can not found U buffer(0x%x) physical addr since addr errors +++\n",
            mpi->planes[1]);
    return 0;
  }
  REG32(ipu_vbase + REG_U_ADDR) = paddr;

// set V addr register (mpi->planes[2])
  paddr = get_phy_addr ((unsigned int)mpi->planes[2]);
  if (paddr == 0)
  {
    printf ("+++ Can not found V buffer(0x%x) physical addr since addr errors +++\n",
            mpi->planes[2]);
    return 0;
  }
  REG32(ipu_vbase + REG_V_ADDR) = paddr;

// set Y(mpi->stride[0]) U(mpi->stride[1]) V(mpi->stride[2]) stride
  REG32(ipu_vbase + REG_Y_STRIDE) = mpi->stride[0];
  REG32(ipu_vbase + REG_UV_STRIDE) = U_STRIDE(mpi->stride[1]) | V_STRIDE(mpi->stride[2]);
// start ipu
  run_ipu(ipu_vbase);
  //volatile int polling = polling_end_flag(ipu_vbase);
  ipu_image_completed = 1;
  return 1;
}

// copy from vf_scale.c

static void scale(struct SwsContext *sws1, struct SwsContext *sws2, uint8_t *src[3], int src_stride[3], int y, int h, 
                  uint8_t *dst[3], int dst_stride[3], int interlaced){
#ifndef USE_16M_SDRAM
    if(interlaced){
        int i;
        uint8_t *src2[3]={src[0], src[1], src[2]};
        uint8_t *dst2[3]={dst[0], dst[1], dst[2]};
        int src_stride2[3]={2*src_stride[0], 2*src_stride[1], 2*src_stride[2]};
        int dst_stride2[3]={2*dst_stride[0], 2*dst_stride[1], 2*dst_stride[2]};

        sws_scale_ordered(sws1, src2, src_stride2, y>>1, h>>1, dst2, dst_stride2);
        for(i=0; i<3; i++){
            src2[i] += src_stride[i];
            dst2[i] += dst_stride[i];
        }
        sws_scale_ordered(sws2, src2, src_stride2, y>>1, h>>1, dst2, dst_stride2);
    }else{
        sws_scale_ordered(sws1, src, src_stride, y, h, dst, dst_stride);
    }                  
#endif
}


void  ipu_outsize_changed(int x,int y,int w, int h)
{

	out_x = x;
	out_y = y;
	if ((w==0) && (h==0))
	{
		if ((rsize_w == 0) && ( rsize_h == 0)) return;
		rsize_w = 0;
		rsize_h = 0;
		ipu_size_cfg = 1;	
	}
	else 
    { 
    	if (rsize_w != w || rsize_h != h) {
    		rsize_w = w;
    		rsize_h = h;    		
    		ipu_size_cfg = 1;
    	}
    }
}
void mplayer_showmode(int t)
{
	if(t + 2 <= MP_IMAGE_TYPE_FULLSCREEN)
		ipu_size_cfg = t + 2;
}

static int visual = 1;
void drop_image()
{
	visual = 2;
	F("drop_image\n");
}
void drop_image_seekmode()
{
	visual = 0;
}
int jz47_put_image(struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
	  mp_image_t *dmpi=mpi->priv;
    //printf("jz4740: processing whole frame! dmpi=%p flag=%d\n",
    //	dmpi, (mpi->flags&MP_IMGFLAG_DRAW_CALLBACK));
  
  if(!(mpi->flags&MP_IMGFLAG_DRAW_CALLBACK && dmpi)){
    if(mpi->pict_type == 1)
    		visual = 1;
    if(visual)
    {		
			jz47_put_image_with_ipu(vf, mpi, pts);
			if(visual == 2)
			{	 
				visual = 0;
  }
    }
		else
			Flush_OSDScrean();
  }
    return 1;
}
void vpGetImageInfo(int *x,int *y,int *w,int *h)
{
	*x = out_x;
	*y = out_y;
	*w = out_w;
	*h = out_h;
}

//modify  by jliu  2009.2.10
int get_display_is_direct()
{
	return 0;
}

void jz47_auto_select(int w,int h)
{
	display_mode = FRAMEBUF_16BIT;
}
//for decode alloc buffer
void jz47_soc_set_direct(int mode)
{
	display_mode |= DISPLAY_BUFFER;
	printf("set decode buf\n");

}

//modify end
