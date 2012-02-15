#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "config.h"
#include "mp_msg.h"
#include "sys/stat.h"
#ifdef HAVE_ANDROID_OS
#include "fcntl.h"
#else
#include "sys/fcntl.h"
#endif	/* BUILD_WITH_ANDROID */
#include "sys/ioctl.h"
#include "sys/mman.h"
#include "fcntl.h"
#include <linux/fb.h>
#include <unistd.h>

#include "mplayer_surface_output.h"

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

#include "android_mvom.h"
#define JZ4750_IPU

#ifdef USE_ANDROID_SURFACE_FLINGER /* Wolfgang, 05-09-2009 */
#define NEED_COPY_IMGBUF
#endif	/* USE_ANDROID_SURFACE_FLINGER */

#ifdef JZ4750_IPU
//#include "jz4750_android_ipu.h"
#undef printf
#define USE_IMEM_ALLOC 
#define IPU_TO_LCD
#define LCD_BPP 16
#define FB_LCD_WIDTH fb_w 
#define FB_LCD_HEIGHT fb_h
#define LCD_BPP_BYTE (LCD_BPP / 8)

#define PAGE_SIZE (4096)
#define MEM_ALLOC_DEV_NUM 1

struct mem_dev {
  unsigned int vaddr;
  unsigned int paddr;
  unsigned int totalsize;
  unsigned int usedsize;
} memdev[MEM_ALLOC_DEV_NUM]; 

#ifdef USE_IMEM_ALLOC 
char *memfname[]={"/proc/jz/imem", 
                  "/proc/jz/imem",
                  "/proc/jz/imem", 
                  "/proc/jz/imem"};
#endif
//---------------------------------------------------------
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

//from ../common/Mplayer_Global.c
enum DisplayFormat {
	DISP_TV_OUT = 0,
	DISP_LCD_OUT,
};


//from jz4750 default
static img_param_t img_param = {0};
struct YuvStride stride = {0};
struct YuvCsc ipu_csc = {YUV_CSC_C0,YUV_CSC_C1,YUV_CSC_C2,YUV_CSC_C3,YUV_CSC_C4,YUV_CSC_CHROM,YUV_CSC_LUMA};


#define FRAMEBUF_BPP  16

#define MP_IMAGE_TYPE_SIZE_FINISH 0
#define MP_IMAGE_TYPE_CUSTORM     1
#define MP_IMAGE_TYPE_INITSIZE    2
#define MP_IMAGE_TYPE_EQSIZE      3
#define MP_IMAGE_TYPE_FULLSCREEN  4

static int  g_out_x = 0, g_out_y = 0, g_out_w = 0,g_out_h = 0;

unsigned int display_mode = 0;
#define TV_OUT        0x00000000
#define LCD_OUT       0x01000000

#define SETOUTMODE(x) \
	do{				  \
		display_mode &= ~0x01000000;	\
		display_mode |= x;		\
	}while(0)

#define CHANGE_OUT_MODE 0x10000000

#define DISP_16BIT     0x0000
#define DISP_32BIT     0x0100
#define DISP_DIRECT    0x1000
#define DISP_ALL       0x1100

#define DISPLAY_BUFFER     0x00
#define DECODE_BUFFER  	   0x01

#define IPU_PEND       0x10000


static int display_change = 1;

static unsigned int useframebuf = 0;

static unsigned int framebuf_offset = 0;


#define BUFFER_COUNT 3
typedef struct __tag_YUVFrame__
{
	unsigned char *y_framebuf;
	unsigned char *u_framebuf;
	unsigned char *v_framebuf;

	unsigned int ystride;
	unsigned int ustride;
	unsigned int vstride;
	struct __tag_YUVFrame__ *nextframe;
}YUVFrame ,*PYUVFrame;

static PYUVFrame yuv_framebuf[BUFFER_COUNT] = {0};
static PYUVFrame yuv_cur = 0;
static PYUVFrame yuv_next = 0;

//-------------------------------------------------
#define IPU_Y_ADDR 0
#define IPU_U_ADDR 1
#define IPU_V_ADDR 2

#define IPU_OUT_ADDR      3
#define IPU_Y_STRIDE_ADDR 4
#define IPU_UV_STRIDE_ADDR 5

#define IPU_CTRL_ADDR      6
#define IPU_STATUS_ADDR    7

static unsigned int ipu_addr[8] = {0};
//-------------------------------------------------

#define IPU_REG_OUT(x,y) ((*(volatile unsigned int *)(ipu_addr[x])) = y)
#define IPU_REG(x) (*(volatile unsigned int *)(ipu_addr[x]))
#if 0//1104
#define PEND_IPU() \
do{	\
   while((!(IPU_REG(IPU_STATUS_ADDR) & 1)) && (IPU_REG(IPU_CTRL_ADDR) & (1<<1)) ) ; \ 
    IPU_REG(IPU_CTRL_ADDR) &= ~(1 << 1);	\
}while(0)
#endif//1104
#define PEND_IPU() \
do{	\
    IPU_REG(IPU_CTRL_ADDR) &= ~(1 << 1);	\
}while(0)

#define START_IPU() \
do{     \
	IPU_REG(IPU_STATUS_ADDR) &= ~1; \
	IPU_REG(IPU_CTRL_ADDR) |= (1 << 1);\
}while(0)

#define PHY(x) ((unsigned int )x & 0x1fffffff)
#define USE_OSD 0

static struct android_display_info_t adi={0};

//---------------------------------------------------------
extern int fbfd;
/* 0 : toggle ;  1 : no toggle */
static int put_image_cnt = 0;
static int toggle_flag = 0;
//unsigned int dcache[4096];
static int mem_init = 0, memdev_count = 0;
int mmapfd = 0;
static int ipu_inited = 0, ipu_size_cfg = 0;
//, get_fbinfo_already = 0;
static int ipu_rtable_init = 0, ipu_rtable_len;
int  ipufd, ipu_maped = 0, ipu_image_completed = 0;
volatile unsigned char *ipu_vbase=NULL, *img_ptr=NULL;
//volatile unsigned char *frame_buffer=NULL;
extern unsigned int fb_w, fb_h, phy_fb, fbmemlen, fb_line_len;
//unsigned int rsize_w = 0, rsize_h = 0;
//unsigned int out_width, out_height;
//static unsigned int out_x,out_y,out_w,out_h,
static unsigned int frame_offset = 0;
extern unsigned int fg1_first_start;
#define IPU_LUT_LEN 32
struct Ration2m ipu_ratio_table[(IPU_LUT_LEN) * (IPU_LUT_LEN)];

#if 1
#define PRINT_DEBUD() printf("Emily--%s--%s--%d++\n",__FILE__,__FUNCTION__,__LINE__);
#else 
#define PRINT_DEBUD()
#endif

/* Mplayer output interface module */
struct mplayer_video_output_config_t g_mvoc;
/* Mplayer output interface module */


unsigned int get_phy_addr (unsigned int vaddr)
{
	int i;	
	for (i = 0; i < memdev_count; i++)
	{
		if (vaddr >= memdev[i].vaddr && vaddr < memdev[i].vaddr + memdev[i].totalsize) {
			return memdev[i].paddr + (vaddr -  memdev[i].vaddr);
		}
	}
	
	return 0;
}

void jz4740_free_devmem ()
{
	int i;
	unsigned char cmdbuf[100];
	
	for (i = 0; i < memdev_count; i++)
	{
		if (memdev[i].vaddr) {
			munmap((void *)memdev[i].vaddr,memdev[i].totalsize);
			memset(&memdev[i],0,sizeof(struct mem_dev));
		}	
	} 
	
	mem_init = 0;
	memdev_count = 0;
	
#ifdef USE_IMEM_ALLOC 
	sprintf (cmdbuf, "echo FF > %s", memfname[0]);
	system (cmdbuf);
#endif

}

void toggle_jz47_tlb(void);
void *jz4740_alloc_frame (int align, int size)
{
	int power, pages = 1, fd, i;
	unsigned int paddr, vaddr;
	unsigned char cmdbuf[100];
	
	/* Free all proc memory. */
	if (mem_init == 0) {
#ifdef USE_IMEM_ALLOC 
		sprintf (cmdbuf, "echo FF > %s", memfname[0]);
		//echo FF > /proc/jz/imem
		system (cmdbuf);
#endif
		mem_init = 1;
	}
	/* open /dev/mem for mmap the memory. */
	if (mmapfd == 0)
	 	mmapfd = open ("/dev/mem", O_RDWR);
	if (mmapfd <= 0) {
		printf("++ Jz47 alloc frame failed: can not mmap the memory ++\n");
		return 0;
	}
	
	if (memdev_count <  MEM_ALLOC_DEV_NUM) {
		/* request the new memory space */
		power = 12;    /* request mem size of 2 ^ 12 pages, 4M space */
		pages = 1 << power; /* total page numbers */
		// request mem
		sprintf (cmdbuf, "echo %x > %s", power, memfname[memdev_count]);
		system (cmdbuf);
		// get paddr
		fd = open(memfname[memdev_count], O_RDWR);
		if (fd >= 0)
			read (fd, &paddr, 4); 
		close(fd);
		/* failed, so free the memory. */
		if (paddr == 0) { 
			return 0;
		} else {
			/* mmap the memory and record vadd and paddr. */
			vaddr = (unsigned int)mmap ((void *)0x60000000,
						    pages * PAGE_SIZE * 2,
						    (PROT_READ|PROT_WRITE),
						    MAP_SHARED, 
						    mmapfd, 
						    paddr);
			
			memdev[memdev_count].vaddr = vaddr;
			memdev[memdev_count].paddr = paddr;
			memdev[memdev_count].totalsize = (pages * PAGE_SIZE);
			memdev[memdev_count].usedsize = 0;
			memdev_count++;

			toggle_jz47_tlb();

			printf ("jz Dev alloc 2: vaddr = 0x%x, paddr = 0x%x size = 0x%x ++\n", vaddr, paddr, (pages * PAGE_SIZE));

		}
	}
	
	for (i = 0; i < memdev_count; i++) {
		int alloc_size, used_size, total_size;
		used_size = memdev[i].usedsize;
		total_size = memdev[i].totalsize;
		alloc_size = total_size - used_size;
		printf("1 [ i=%d uesd=%d total=%d alloc_size=%d size=%d ] \n",i,memdev[i].usedsize,memdev[i].totalsize,alloc_size,size);
		if (alloc_size >= size) {
			/* align to 16 boundary.  */
			alloc_size = (size + 15) & ~(15);

			memdev[i].usedsize = used_size + alloc_size;
			vaddr = memdev[i].vaddr + used_size;

			printf ("jz mem alloc [%d]: vaddr = 0x%x, paddr = 0x%x size = 0x%x\n", i, vaddr, memdev[i].paddr + used_size, size);
			return (void *)vaddr;
		}
	}
	printf ("++ Jz47 alloc: memory allocated is failed (size = %d) ++\n", size);
}

/*----------------------------------------------------------------------------------*/

void dump(img_param_t *t)
{
	printf("-----------------------------\n");
	printf("ipu_ctrl = %x\n",t->ipu_ctrl);
	printf("ipu_d_fmt = %x\n",t->ipu_d_fmt);
	printf("in_width = %x\n",t->in_width);
	printf("in_height = %x\n",t->in_height);
	printf("in_bpp = %x\n",t->in_bpp);
	printf("out_width = %x\n",t->out_width);
	printf("out_height = %x\n",t->out_height);
	printf("y_buf = %x\n",t->y_buf);
	printf("u_buf = %x\n",t->u_buf);
	printf("v_buf = %x\n",t->v_buf);
	printf("out_buf = %x\n",t->out_buf);
	printf("y_t_addr = %x\n",t->y_t_addr);
	printf("u_t_addr = %x\n",t->u_t_addr);
	printf("v_t_addr = %x\n",t->v_t_addr);
	printf("out_t_addr = %x\n",t->out_t_addr);
	printf("ratio_table = %x\n",t->ratio_table);
	printf("csc = %x\n",t->csc);
	printf("out_x = %x\n",t->out_x);
	printf("out_y = %x\n",t->out_y);
	printf("-----------------------------\n");
}


static int CreateImage(int w,int h,int cw,int ch,int bpp)
{
	int i;
	int *framebuf;
	//printf("-------disp_w = %d disp_h = %d cw = %d ch = %d bpp = %d------\n",w,h,cw,ch,bpp);
	framebuf = (int *)memalign(64,(w * h * 2) * 3);
	if(framebuf == 0)
	{
		printf("mem too small\n");
		return 0;
	}
	
	for(i = 0; i < BUFFER_COUNT;i++)
	{
		yuv_framebuf[i] = (PYUVFrame)malloc(sizeof(YUVFrame));
		yuv_framebuf[i]->y_framebuf = (unsigned char*)((unsigned int)framebuf + i * (w * h * 2) );
		yuv_framebuf[i]->u_framebuf = (unsigned char*)((unsigned int)yuv_framebuf[i]->y_framebuf + w * h);
		yuv_framebuf[i]->v_framebuf = (unsigned char*)((unsigned int)yuv_framebuf[i]->u_framebuf + cw * h);
		printf("i = %d y = %x u = %x v = %x\n",i,yuv_framebuf[i]->y_framebuf,yuv_framebuf[i]->u_framebuf,yuv_framebuf[i]->v_framebuf);
		yuv_framebuf[i]->ystride = 0;
		yuv_framebuf[i]->ustride = 0;
		yuv_framebuf[i]->vstride = 0;
		if(i > 0)
		{
			yuv_framebuf[i]->nextframe = yuv_framebuf[i - 1];
		}
	}
	yuv_framebuf[0]->nextframe = yuv_framebuf[BUFFER_COUNT - 1];
	yuv_cur = yuv_framebuf[0];
	yuv_next = yuv_framebuf[0]->nextframe;
	return 1;
}

static void FreeImage()
{
	int i;
	if((yuv_framebuf[0] != 0) && (yuv_framebuf[0]->y_framebuf))
		free(yuv_framebuf[0]->y_framebuf);
	for(i = 0;i < BUFFER_COUNT;i++)
	{
		if(yuv_framebuf[i] != 0)
			free(yuv_framebuf[i]);
		yuv_framebuf[i] = 0;
	}
	yuv_cur = 0;
	yuv_next = 0;
}


static void eqscale(int w,int h,int *out_x,int *out_y,int *out_w,int *out_h)
{
	int xscale,yscale;
	int scale;
	int rsize_w,rsize_h;
	xscale = w * 1000 / (int)FB_LCD_WIDTH;
	yscale = h * 1000 / (int)FB_LCD_HEIGHT;
	printf("\nxscale %d yscale %d\n",xscale,yscale);
	scale = xscale > yscale ? xscale : yscale;

	if(scale <= 500) scale = 500;
	rsize_w = w * 1000 / scale;
	rsize_h = h * 1000 / scale;

	if(rsize_h > FB_LCD_HEIGHT) rsize_h = FB_LCD_HEIGHT;
	if(rsize_w > FB_LCD_WIDTH) rsize_w = FB_LCD_WIDTH;

	*out_x = (FB_LCD_WIDTH - rsize_w) / 2;
	*out_y = (FB_LCD_HEIGHT - rsize_h) / 2;
	*out_w = rsize_w;
	*out_h = rsize_h;
	printf("\n3:nrsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);

}
static void fullscreen(int w,int h,int *out_x,int *out_y,int *out_w,int *out_h)
{
	int xscale,yscale;
	int rsize_w,rsize_h;
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

	*out_x = (FB_LCD_WIDTH - rsize_w) / 2;
	*out_y = (FB_LCD_HEIGHT - rsize_h) / 2;
	*out_w = rsize_w;
	*out_h = rsize_h;
	printf("\n4:rsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);

}

static void jz47_ipu_init (struct vf_instance_s* vf,mp_image_t *mpi)
{
	SwsContext *c = vf->priv->ctx;
	int outW, outH;
	int out_w,out_h;
	int out_x,out_y;
	unsigned int dsp_mod;
	unsigned int new_dsp_mod;

	dsp_mod = display_mode & ~CHANGE_OUT_MODE;
	mvom_lcd_get_size(&new_dsp_mod, &fb_w, &fb_h);//Emily
	if(new_dsp_mod != dsp_mod){
	  PRINT_DEBUD();
	  display_mode = new_dsp_mod | CHANGE_OUT_MODE ;
	}
	else {
	  PRINT_DEBUD();
	  display_mode &= ~CHANGE_OUT_MODE;
	}	

	outW = c->dstW;
	outH = c->dstH;
	/*
	outW = mpi->width;
	outH = mpi->height;
	c->dstW = outW;
	c->dstH = outH;
	*/
	if(ipu_inited &&(new_dsp_mod == dsp_mod)){
	  printf("PEND_IPU()......\n");
	  PEND_IPU();
	}

	ipu_size_cfg = g_mvoc.resize_mode;
	printf("ipu_size_cfg=%d\n",ipu_size_cfg);
	if(ipu_size_cfg){ PRINT_DEBUD();
	  outW = c->srcW;//Emily
	  outH = c->srcH;//Emily
	  switch(ipu_size_cfg)
	  {
	  case MP_IMAGE_TYPE_CUSTORM:
	  //	  out_w = g_out_w;
	  //	  out_h = g_out_h;
	    break;
	  case MP_IMAGE_TYPE_INITSIZE:
	    if((outH <= FB_LCD_HEIGHT) && (outW <= FB_LCD_WIDTH))
	    {
	      out_x = (FB_LCD_WIDTH - outW) / 2;
	      out_y = (FB_LCD_HEIGHT - outH) / 2;
	      out_w = outW;
	      out_h = outH;
	    }else
	      eqscale(outW,outH,&out_x,&out_y,&out_w,&out_h);
	    break;
	  case MP_IMAGE_TYPE_EQSIZE:
	    eqscale(outW,outH,&out_x,&out_y,&out_w,&out_h);
	    break;
	  case MP_IMAGE_TYPE_FULLSCREEN:
	    fullscreen(outW,outH,&out_x,&out_y,&out_w,&out_h);
	    break;
	  }
	}
	/*
	g_out_w = out_w;
	g_out_h = out_h;
	g_out_x = out_x;
	g_out_y = out_y;
	*/
	img_param.ratio_table = ipu_ratio_table;

	{
	  img_param.in_width = outW;
	  img_param.in_height = outH;
	  img_param.out_width = out_w;
	  img_param.out_height = out_h;
	  img_param.out_x = out_x;
	  img_param.out_y = out_y;
	  img_param.stride = &stride;
	  img_param.csc = &ipu_csc;
	  //-----------------------------------------------------------------//
	  img_param.y_buf = get_phy_addr(mpi->planes[0]);
	  img_param.u_buf = get_phy_addr(mpi->planes[1]);
	  img_param.v_buf = get_phy_addr(mpi->planes[2]);
	  //-----------------------------------------------------------------//
	}
	//printf("out_x=%d,out_y=%d,out_w=%d,out_h=%d\n",out_x,out_y,out_w,out_h);
	//	dump(&img_param);
	if((ipu_inited == 0) || (display_mode & CHANGE_OUT_MODE))
	{	PRINT_DEBUD();
		display_mode &= ~CHANGE_OUT_MODE;
		//	int srcFormat= c->srcFormat;
		const int srcFormat = c->srcFormat;
		const int dstFormat = c->dstFormat;
		switch (srcFormat)
		{
		case PIX_FMT_YUV420P:
		  printf("========PIX_FMT_YUV420P=======\n");
			img_param.ipu_d_fmt |= IN_FMT_YUV420;
			break;
		case PIX_FMT_YUV422P:
		  printf("========PIX_FMT_YUV422P======\n");
			img_param.ipu_d_fmt |= IN_FMT_YUV422;
			break;
		case PIX_FMT_YUV444P:
		  printf("========PIX_FMT_YUV444P======\n");
			img_param.ipu_d_fmt |= IN_FMT_YUV444;
			break;
		case PIX_FMT_YUV411P:
		  printf("========PIX_FMT_YUV411P======\n");
			img_param.ipu_d_fmt |= IN_FMT_YUV411;
			break;
		default:
		  break;
		}
		switch (dstFormat)
		{
		//case PIX_FMT_RGB32:
		case PIX_FMT_RGBA32:
		  printf("===PIX_FMT_RGBA32=====\n");
		  img_param.ipu_d_fmt |= OUT_FMT_RGB888;
		  break;
		  
		case PIX_FMT_RGB555:
		  printf("====PIX_FMT_RGB555===\n");
		  img_param.ipu_d_fmt |= OUT_FMT_RGB555;
		  break;
		  
		case PIX_FMT_RGB565:
		case PIX_FMT_BGR565:
		  printf("====PIX_FMT_RGB565===\n");
		  img_param.ipu_d_fmt |= OUT_FMT_RGB565;
		  break;
		default:
		  printf("no support out format, check jz47_set_ipu_csc_cfg()\n");
		  break;
		}

		adi.flag = ANDROID_IPU_IOCTL;
	        adi.buff = ipu_addr;
		adi.ipu_cmd = IOCTL_IPU_GET_ADDR;
		ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

		//	display_mode |= DISP_DIRECT;//Emily


		display_mode &= ~IPU_PEND; //Emily
		Init_Direct_Mode(mpi);//Emily
		

		mvom_lcd_setup(&g_mvoc, img_param.out_x, img_param.out_y, img_param.out_width, img_param.out_height);
		ipu_inited = 1;
	}else
  	{
	  PRINT_DEBUD();

	          printf("+++++++++++++++++++resize\n");
		  adi.flag = ANDROID_FG1_RESIZE;
                  adi.buff = &img_param;
		  ioctl(fbfd, FBIO_ANDROID_CTL, &adi);
		  //ioctl(&img_param, IOCTL_LCD_RESIZE_ALL);//Emily

		//	Flush_OSDScrean();
		printf("-------------------resize\n");
	}

	//mplayer_setdisplaymode(DISP_LCD_OUT);
	framebuf_offset = (img_param.out_x + img_param.out_y * fb_w) * (img_param.in_bpp / 8);
	//mvom_lcd_setup(&g_mvoc, img_param.out_x, img_param.out_y, img_param.out_width, img_param.out_height);
	//	dump(&img_param);
}

void toggle_jz47_tlb(void)
{
	int cur_pid = 0;
	unsigned char cmdbuf[100];

	if (!toggle_flag && memdev[0].vaddr && memdev[0].paddr) {

		toggle_flag = 1;
		cur_pid = getpid();
		
		sprintf(cmdbuf, "echo 00%08x 00%08x 00%08x 00%08x > %s", 
			cur_pid, memdev[0].vaddr, memdev[0].paddr, 0x400000, "/proc/jz/ipu");
		
		printf("toggle : system call command: cmdbuf=%s\n",cmdbuf);
		system(cmdbuf);
	}
	
	return;
}

void free_jz47_tlb(void)
{
	unsigned char cmdbuf[100];
	
#ifndef USE_ANDROID_SURFACE_FLINGER /* Wolfgang, 05-09-2009 */
//	jzfb_uninit();
#endif
	if (toggle_flag) {
		sprintf (cmdbuf, "echo %s > %s", "release tlb", "/proc/jz/ipu");
		printf("free : system call command: cmdbuf=%s\n",cmdbuf);

		system (cmdbuf);
		jz4740_free_devmem();
		toggle_flag = 0;
	}

	return;
}

void  ipu_image_start()
{
	jz4740_free_devmem();
	ipu_inited = 0;
	ipu_image_completed = 0;
	g_out_w = 0;
	g_out_h = 0;
	g_out_x = g_out_y = 0;
	ipu_maped = 0;
	//	frame_offset = 0;
	toggle_flag = 0;
	display_mode = 0;
}

void  ipu_outsize_changed(int x,int y,int w, int h)
{PRINT_DEBUD();
	g_out_x = x;
	g_out_y = y;
	if ((w==0) && (h==0))
	{
		g_out_w = FB_LCD_WIDTH;
		g_out_h = FB_LCD_HEIGHT;
		ipu_size_cfg = 1;
		display_change = 1;
	}
	else
	{
  		if(w > FB_LCD_WIDTH) w = FB_LCD_WIDTH;
  		if(h > FB_LCD_HEIGHT) h = FB_LCD_HEIGHT;

  		g_out_w = w;
  		g_out_h = h;
  		ipu_size_cfg = 1;
  		display_change = 1;

	}

}



void mpi_copy(PYUVFrame yuvframe,mp_image_t* mpi)
{
    char * ty,*tu,*tv,*sy,*su,*sv;
    ty = (char *)yuvframe->y_framebuf;
    tu = (char *)yuvframe->u_framebuf;
    tv = (char *)yuvframe->v_framebuf;

    sy = (char *)mpi->planes[0];
    su = (char *)mpi->planes[1];
    sv = (char *)mpi->planes[2];

	int i;
	for(i = 0;i < mpi->height;i++)
	{
		memcpy(ty,sy,mpi->width);
		ty += mpi->width;
		sy += mpi->stride[0];
	}
	for(i = 0;i < mpi->chroma_height;i++){
		
		memcpy(tu,su,mpi->chroma_width);
		tu += mpi->chroma_width;
		su += mpi->stride[1];
		memcpy(tv,sv,mpi->chroma_width);
		tv += mpi->chroma_width;
		sv += mpi->stride[2];
	}
	
	
	int w = (mpi->width + 7) & 7;
	yuvframe->ystride = mpi->width; //mpi->stride[0];
	yuvframe->ustride = mpi->chroma_width;//mpi->stride[1];
	yuvframe->vstride = mpi->chroma_width;

	//printf("mpi->width = %d\n",mpi->width);
	//printf("mpi->height = %d\n",mpi->height);
	//printf("mpi->chroma_width = %d\n",mpi->chroma_width);
	//printf("mpi->chroma_height = %d\n",mpi->chroma_height);
}

int jz47_put_image_with_default (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{PRINT_DEBUD();
	vf = vf;
	mpi = mpi;
	pts = pts;
	return 0;

}
static int (*jz47_put_image_with_ipu)(struct vf_instance_s* vf, mp_image_t *mpi, double pts) = jz47_put_image_with_default;
#ifdef VERIFY_CRC32
unsigned int crc32_verify(unsigned char *buf, int line, int col, int stride);
#endif
int jz47_put_image_with_direct_ipu_buffer (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
	unsigned int paddr[5];
#ifdef VERIFY_CRC32
  SwsContext *c = vf->priv->ctx;

        crc32_verify (mpi->planes[0], c->srcH, c->srcW, mpi->stride[0]);
        crc32_verify (mpi->planes[1], c->srcH / 2, c->srcW / 2, mpi->stride[1]);
        crc32_verify (mpi->planes[2], c->srcH / 2, c->srcW / 2, mpi->stride[2]);
#endif

	if(yuv_next)
	{
			mpi_copy(yuv_next,mpi);
			yuv_next = yuv_next->nextframe;
	}
	if(yuv_cur)
	{
		// set Y(mpi->planes[0]), U(mpi->planes[1]), V addr (mpi->planes[2])
	  img_param.y_buf = get_phy_addr((unsigned int)yuv_cur->y_framebuf); 
	// set U addr register (mpi->planes[1])
	  img_param.u_buf = get_phy_addr((unsigned int)yuv_cur->u_framebuf);
	// set V addr register (mpi->planes[2])
	  img_param.v_buf = get_phy_addr((unsigned int)yuv_cur->v_framebuf);

	// set Y(mpi->stride[0]) U(mpi->stride[1]) V(mpi->stride[2]) stride
	  stride.y = yuv_cur->ystride;
	  stride.u = yuv_cur->ustride;
	  stride.v = yuv_cur->vstride;
	  yuv_cur = yuv_cur->nextframe;
	  //Emily  lcd_flush_frame_all();

	  adi.flag = ANDROID_IPU_IOCTL;
	  adi.buff = &img_param;
	  // adi.buff = (void *)paddr;
	  adi.ipu_cmd = IOCTL_IPU_CHANGE_BUFF;
	  ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	}

	return 1;
}

int jz47_put_image_with_direct_ipu_nobuffer(struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
  unsigned int paddr[5];
  //  unsigned int paddr;
#ifdef VERIFY_CRC32
  SwsContext *c = vf->priv->ctx;

        crc32_verify (mpi->planes[0], c->srcH, c->srcW, mpi->stride[0]);
        crc32_verify (mpi->planes[1], c->srcH / 2, c->srcW / 2, mpi->stride[1]);
        crc32_verify (mpi->planes[2], c->srcH / 2, c->srcW / 2, mpi->stride[2]);
#endif


	//img_param.y_buf = get_phy_addr((unsigned int)mpi->planes[0]);
	//img_param.u_buf = get_phy_addr((unsigned int)mpi->planes[1]);
	//img_param.v_buf = get_phy_addr((unsigned int)mpi->planes[2]);
	img_param.y_buf = (unsigned int)mpi->planes[0];
	img_param.u_buf = (unsigned int)mpi->planes[1];
	img_param.v_buf = (unsigned int)mpi->planes[2];
	stride.y = mpi->stride[0];
	stride.u = mpi->stride[1];
	stride.v = mpi->stride[2];

	paddr[0] = get_phy_addr((unsigned int) mpi->planes[0]);
	paddr[1] = get_phy_addr((unsigned int) mpi->planes[1]);
	paddr[2] = get_phy_addr((unsigned int) mpi->planes[2]);
	paddr[3] = mpi->stride[0]; 
	paddr[4] = U_STRIDE(mpi->stride[1]) | V_STRIDE(mpi->stride[2]);

	adi.flag = ANDROID_IPU_IOCTL;
	//adi.buff = &img_param;
	adi.buff = (void *)paddr;
	adi.ipu_cmd = IOCTL_IPU_CHANGE_BUFF;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

	return 1;
}

int jz47_put_image_with_framebuf (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{PRINT_DEBUD();

  PEND_IPU();

  //if((unsigned int)lcd_get_current_phyframe() != useframebuf) lcd_change_frame();

  IPU_REG_OUT(IPU_Y_ADDR,PHY(mpi->planes[0]));
  IPU_REG_OUT(IPU_U_ADDR,PHY(mpi->planes[1]));
  IPU_REG_OUT(IPU_V_ADDR,PHY(mpi->planes[2]));

  IPU_REG_OUT(IPU_Y_STRIDE_ADDR,PHY(mpi->stride[0]));
  IPU_REG_OUT(IPU_UV_STRIDE_ADDR,(mpi->stride[1] << 16) |mpi->stride[2]);
  //useframebuf = (unsigned int)lcd_get_change_phyframe();
  //add_frame_buf(useframebuf);
  IPU_REG_OUT(IPU_OUT_ADDR,framebuf_offset + useframebuf);
  //lcd_flush_frame_all();
  START_IPU();
  return 1;
}


void Init_Frame_Buffer_Mode(mp_image_t *mpi)
{PRINT_DEBUD();
	if(display_mode & LCD_OUT)
	{
		if(display_mode & DISP_32BIT)
			img_param.ipu_d_fmt |= OUT_FMT_RGB888;
		else // 16BIT
			img_param.ipu_d_fmt |= OUT_FMT_RGB565;
	}
	else //tv_out
		img_param.ipu_d_fmt |= (OUT_FMT_YUV422 | (1 << 16));

	img_param.in_bpp = 16;
	useframebuf = phy_fb;//(unsigned char *) jz4740_alloc_frame(32, fbmemlen);//(int)lcd_get_change_phyframe();
	img_param.out_buf = useframebuf;//get_phy_addr(useframebuf);

	stride.out = fb_w * (img_param.in_bpp / 8 );
	jz47_put_image_with_ipu = jz47_put_image_with_framebuf;
	img_param.y_buf = mpi->planes[0];
	img_param.u_buf = mpi->planes[1];
	img_param.v_buf = mpi->planes[2];

	stride.y = mpi->stride[0];
	stride.u = mpi->stride[1];
	stride.v = mpi->stride[2];

	//	dump(&img_param);
	//	lcd_clean_frame_all();
	adi.buff = &img_param;
	adi.flag = ANDROID_SET_IPU;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
}
void Init_Direct_Mode(mp_image_t *mpi)
{
	img_param.ipu_d_fmt |= OUT_FMT_RGB888;
	img_param.in_bpp = 32;
	if(1)//1104
	//	if(display_mode & DECODE_BUFFER)
	{
	  img_param.y_buf = get_phy_addr(mpi->planes[0]);
	  img_param.u_buf = get_phy_addr(mpi->planes[1]);
	  img_param.v_buf = get_phy_addr(mpi->planes[2]);
		stride.y = mpi->stride[0];
		stride.u = mpi->stride[1];
		stride.v = mpi->stride[2];
		//	lcd_flush_frame_all();
  		jz47_put_image_with_ipu = jz47_put_image_with_direct_ipu_nobuffer;
		printf("direct buf\n");
	}else
	{

		CreateImage(mpi->width,mpi->height,mpi->chroma_width,mpi->chroma_height,mpi->bpp);
		mpi_copy(yuv_cur,mpi);
		img_param.y_buf = get_phy_addr(yuv_cur->y_framebuf);
		img_param.u_buf = get_phy_addr(yuv_cur->u_framebuf);
		img_param.v_buf = get_phy_addr(yuv_cur->v_framebuf);
		stride.y = yuv_cur->ystride;
		stride.u = yuv_cur->ustride;
		stride.v = yuv_cur->vstride;
		yuv_cur = yuv_cur->nextframe;
		jz47_put_image_with_ipu = jz47_put_image_with_direct_ipu_buffer;
		printf("create buf\n");
	}

	adi.flag = ANDROID_SET_FG1_IPU_DIRECT;
	adi.fg1_short_cut = 1;
	adi.buff = &img_param;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	PRINT_DEBUD();
	//	dump(&img_param);	
}


static int visual = 0;
void drop_image()
{PRINT_DEBUD();
	visual = 0;
}

int jz47_put_image(struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
	mp_image_t *dmpi=mpi->priv;

	if(display_change)
	{PRINT_DEBUD();
	  jz47_ipu_init(vf,mpi);
	  display_change = 0;
	  visual = 1;
	}
#if 0
	if((mpi->width != img_param.in_width) || (mpi->height != img_param.in_height))
	{
  	 	display_change = 1;
  	 	visual = 0;
  	 	mpi->pict_type = 0;
	}
#endif//1117
	if(!(mpi->flags&MP_IMGFLAG_DRAW_CALLBACK && dmpi)){
		if(mpi->pict_type == 1)
			visual = 1;
		if(visual)
		{
		  jz47_put_image_with_ipu (vf, mpi, pts);
		  
		}
	}
	//PRINT_DEBUD();
	return 1;
}

#endif

static int jz_ipu_init(void)
{
	if (!ipu_maped)
	{
		ipufd = open ("/dev/mem", O_RDWR);
		if (ipufd < 0) {
			printf("++ open /dev/mem error. program aborted. ++\n");
			return 0;
		}
		ipu_vbase = mmap((void *)0, IPU__SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, ipufd, IPU__OFFSET);
		ipu_maped = 1;
	}


}


/* Mplayer output interface module */
int mvom_init(void)
{printf("mvom_init exit!\n");
//	mvom_data_init();
	mvom_lcd_init();
	ipu_image_start();
	jz_ipu_init();
//	ahb_get_bus_clock();
}

void mvom_exit(void)
{
	printf("mvom_exit exit!\n");
	free_jz47_tlb();
	ipu_image_start();

//	mvom_lcd_exit();
}

int mvom_config(struct mplayer_video_output_config_t *mvoc)
{
	PRINT_DEBUD();
#if 0
	/* check start position */
	if (g_out_x < 0 || g_out_y < 0 
	    || g_out_x > fb_w || g_out_y > fb_h)
	{
		printf("mvom_config par check fail! \n");
		return -1;
	}
#endif
	memcpy(&g_mvoc, mvoc, sizeof(struct mplayer_video_output_config_t));
	//fg1_first_start = 1;
	display_change = 1;
	//ipu_inited = 0;
	return 0;
}

/* Mplayer output interface module */
