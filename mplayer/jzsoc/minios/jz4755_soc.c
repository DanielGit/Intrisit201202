#include <mplaylib.h>
#include "mplaylib.h"
#include "mplayertm.h"
#include "config.h"
#include "mp_msg.h"
#include "avcodec.h"
#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

#include "libswscale/swscale.h"
#include "libswscale/swscale_internal.h"

#include "vf_scale.h"

#include "./uc_inc/ipu_app.h"
#include "./uc_inc/lcd_app.h"

unsigned char* lcd_get_current_cframe(void);
unsigned char* lcd_get_current_phyframe(void);

unsigned char* lcd_get_change_phyframe(void);
unsigned char* lcd_get_change_cframe(void);
unsigned char* lcd_get_change_frame(void);

void lcd_clean_frame_all();
void lcd_flush_frame_all();
void lcd_change_frame(void);

void get_ipu_addr(unsigned int* addr);


#include <uclib.h>
//#undef printf

#if 1
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
#endif

//from ../common/Mplayer_Global.c
enum DisplayFormat {
	DISP_TV_OUT = 0,
	DISP_LCD_OUT,
};
int get_screen_width();
int get_screen_height();
int get_screen_mode();
void SetVideoOutMode(void *fun);


//from jz4750 default
static img_param_t img_param = {0};
struct YuvStride stride = {0};
struct YuvCsc ipu_csc = {YUV_CSC_C0,YUV_CSC_C1,YUV_CSC_C2,YUV_CSC_C3,YUV_CSC_C4,YUV_CSC_CHROM,YUV_CSC_LUMA};


#define FB_LCD_WIDTH  get_screen_width()
#define FB_LCD_HEIGHT get_screen_height()
#define FRAMEBUF_BPP  16


#define MP_IMAGE_TYPE_SIZE_FINISH 0
#define MP_IMAGE_TYPE_CUSTORM     1
#define MP_IMAGE_TYPE_INITSIZE    2
#define MP_IMAGE_TYPE_EQSIZE      3
#define MP_IMAGE_TYPE_FULLSCREEN  4
static int ipu_size_cfg = MP_IMAGE_TYPE_EQSIZE;

static int  g_out_x = 0,
			g_out_y = 0,
			g_out_w = 0,
			g_out_h = 0;

static unsigned int display_mode = 0;
#define TV_OUT        0x00000000
#define LCD_OUT       0x01000000
#define SETOUTMODE(x) \
	do{				  \
		display_mode &= ~0x01000000;							\
		display_mode |= x;									\
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

static int ipu_inited = 0;
static unsigned int framebuf_offset = 0;

typedef void (*PFunc_Hook_OSD)(void *fram);
static PFunc_Hook_OSD pFunction = NULL;


static void jz47_ipu_init (struct vf_instance_s* vf,mp_image_t *mpi);



static void eqscale(int w,int h,int *out_x,int *out_y,int *out_w,int *out_h)
{
	int xscale,yscale;
	int scale;
	int rsize_w,rsize_h;
	xscale = w * 1000 / (int)FB_LCD_WIDTH;
	yscale = h * 1000 / (int)FB_LCD_HEIGHT;
	F("\nxscale %d yscale %d\n",xscale,yscale);
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
	F("\n3:nrsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);

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
  F("\n4:rsize_w = %d rsize_h = %d\n",rsize_w,rsize_h);

}
//--------------------------------------------------------------
#define IPU_LUT_LEN 0x20
struct Ration2m ipu_ratio_table[(IPU_LUT_LEN) * (IPU_LUT_LEN)];

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
#define IPU_Y_ADDR 0
#define IPU_U_ADDR 1
#define IPU_V_ADDR 2

#define IPU_OUT_ADDR      3
#define IPU_Y_STRIDE_ADDR 4
#define IPU_UV_STRIDE_ADDR 5

#define IPU_CTRL_ADDR      6
#define IPU_STATUS_ADDR    7
static unsigned int ipu_addr[8] = {0};
#define IPU_REG_OUT(x,y) ((*(volatile unsigned int *)(ipu_addr[x])) = y)
#define IPU_REG(x) (*(volatile unsigned int *)(ipu_addr[x]))
#define PEND_IPU() \
do{	\
	if(display_mode & IPU_PEND){										\
	   	while((!(IPU_REG(IPU_STATUS_ADDR) & 1)) && (IPU_REG(IPU_CTRL_ADDR) & (1<<1)) ) ;	\
		IPU_REG(IPU_CTRL_ADDR) &= ~(1 << 1); \
    }	\
}while(0)

#define START_IPU() \
do{     \
	IPU_REG(IPU_STATUS_ADDR) &= ~1; \
	IPU_REG(IPU_CTRL_ADDR) |= (1 << 1);\
}while(0)

#define PHY(x) ((unsigned int )x & 0x1fffffff)
#define USE_OSD 1

static int CreateImage(int w,int h,int cw,int ch,int bpp)
{
	int i;
	int *framebuf;
	printf("disp_w = %d disp_h = %d cw = %d ch = %d bpp = %d\n",w,h,cw,ch,bpp);
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

//------------------------------------------------------------------------------
int  ipu_image_start(int w,int h)
{
	return 1;
}
void ipu_image_stop()
{
	lcd_ioctl(0, IOCTL_IPU_TO_DMA1);
	SetVideoOutMode(NULL);
	FreeImage();
}
void dump(img_param_t *t)
{
	F("-----------------------------\n");
	F("ipu_ctrl = %x\n",t->ipu_ctrl);
	F("ipu_d_fmt = %x\n",t->ipu_d_fmt);
	F("in_width = %x\n",t->in_width);
	F("in_height = %x\n",t->in_height);
	F("in_bpp = %x\n",t->in_bpp);
	F("out_width = %x\n",t->out_width);
	F("out_height = %x\n",t->out_height);
	F("y_buf = %x\n",t->y_buf);
	F("u_buf = %x\n",t->u_buf);
	F("v_buf = %x\n",t->v_buf);
	F("out_buf = %x\n",t->out_buf);
	F("y_t_addr = %x\n",t->y_t_addr);
	F("u_t_addr = %x\n",t->u_t_addr);
	F("v_t_addr = %x\n",t->v_t_addr);
	F("out_t_addr = %x\n",t->out_t_addr);
	F("ratio_table = %x\n",t->ratio_table);
	F("csc = %x\n",t->csc);

	F("out_x = %x\n",t->out_x);

	F("out_y = %x\n",t->out_y);

	F("-----------------------------\n");
}


void mpi_copy(PYUVFrame yuvframe,mp_image_t* mpi){

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
{
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
#ifdef VERIFY_CRC32
  SwsContext *c = vf->priv->ctx;

        crc32_verify (mpi->planes[0], c->srcH, c->srcW, mpi->stride[0]);
        crc32_verify (mpi->planes[1], c->srcH / 2, c->srcW / 2, mpi->stride[1]);
        crc32_verify (mpi->planes[2], c->srcH / 2, c->srcW / 2, mpi->stride[2]);
#endif
	#if USE_OSD
		if(pFunction)
        pFunction(NULL);
    #endif

	if(yuv_next)
	{
			mpi_copy(yuv_next,mpi);
			yuv_next = yuv_next->nextframe;
	}
	if(yuv_cur)
	{
		// set Y(mpi->planes[0]), U(mpi->planes[1]), V addr (mpi->planes[2])
	 img_param.y_buf = yuv_cur->y_framebuf;
	// set U addr register (mpi->planes[1])
	 img_param.u_buf = yuv_cur->u_framebuf;
	// set V addr register (mpi->planes[2])
	 img_param.v_buf = yuv_cur->v_framebuf;

	// set Y(mpi->stride[0]) U(mpi->stride[1]) V(mpi->stride[2]) stride
	  stride.y = yuv_cur->ystride;
	  stride.u = yuv_cur->ustride;
	  stride.v = yuv_cur->vstride;
	  yuv_cur = yuv_cur->nextframe;
	  lcd_flush_frame_all();
	  ipu_ioctl((void *)0, IOCTL_IPU_CHANGE_BUFF);
	}
	return 1;
}
int jz47_put_image_with_direct_ipu_nobuffer (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
#ifdef VERIFY_CRC32
  SwsContext *c = vf->priv->ctx;

        crc32_verify (mpi->planes[0], c->srcH, c->srcW, mpi->stride[0]);
        crc32_verify (mpi->planes[1], c->srcH / 2, c->srcW / 2, mpi->stride[1]);
        crc32_verify (mpi->planes[2], c->srcH / 2, c->srcW / 2, mpi->stride[2]);
#endif

	#if USE_OSD
	if(pFunction)
        pFunction(NULL);
    #endif
	img_param.y_buf = mpi->planes[0];
	img_param.u_buf = mpi->planes[1];
	img_param.v_buf = mpi->planes[2];
	stride.y = mpi->stride[0];
	stride.u = mpi->stride[1];
	stride.v = mpi->stride[2];
	lcd_flush_frame_all();
	ipu_ioctl((void *)0, IOCTL_IPU_CHANGE_BUFF);
	return 1;
}
#if 0
static unsigned int frame_buf[12] = {0};

void add_frame_buf(unsigned int frame)
{
	static int count = 0;
	int i;
	memcpy(frame_buf,&frame_buf[1],11 * 4);
	frame_buf[11] = frame;
	count++;
	if((frame_buf[7] != frame_buf[11]) && (frame_buf[0] != 0))
	{
			printf("---------------%d-------------\n",count);
			for(i = 0;i < 12 / 4 ;i++)
				printf("0:%x 1:%x 2:%x 3:%x\n",frame_buf[i*4 + 0],frame_buf[i*4 + 1],frame_buf[i*4 + 2],frame_buf[i*4 + 3]);
	}

}
#endif
int jz47_put_image_with_framebuf (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
		//lcd_ioctl((void *)0,IOCTL_PENDING_IPU_END);
		//printf("+\n");
		PEND_IPU();
		//printf("-\n");
		#if USE_OSD
		if(pFunction)
        pFunction(lcd_get_current_cframe());
    #endif
		if((unsigned int)lcd_get_current_phyframe() != useframebuf)
			lcd_change_frame();
		IPU_REG_OUT(IPU_Y_ADDR,PHY(mpi->planes[0]));
		IPU_REG_OUT(IPU_U_ADDR,PHY(mpi->planes[1]));
		IPU_REG_OUT(IPU_V_ADDR,PHY(mpi->planes[2]));

		IPU_REG_OUT(IPU_Y_STRIDE_ADDR,PHY(mpi->stride[0]));
		IPU_REG_OUT(IPU_UV_STRIDE_ADDR,(mpi->stride[1] << 16) |mpi->stride[2]);
		useframebuf = (unsigned int)lcd_get_change_phyframe();
		//add_frame_buf(useframebuf);
		IPU_REG_OUT(IPU_OUT_ADDR,framebuf_offset + useframebuf);
		lcd_flush_frame_all();
		START_IPU();
		return 1;
}

void jz47_auto_select(int w,int h)
{
	jz47_soc_set_direct(1);
//	if(get_screen_height() * get_screen_width() <= 480 * 272 * 2)
	{
		display_mode |= DISP_DIRECT;
		//display_mode |= DISP_16BIT; //test
	}
	//else
	{
		//display_mode |= DISP_16BIT;
	}

	switch(get_screen_mode())
	{
		case DISP_LCD_OUT:
			display_mode |= LCD_OUT;
	  		break;
	  	case DISP_TV_OUT:
			display_mode |= TV_OUT;
			break;
	}
	printf("diplay_mode = %x\n",display_mode);

}

int get_display_is_direct()
{
//	if (!(display_mode & LCD_OUT))
//		return 0;
	return ((display_mode & DISP_DIRECT) == DISP_DIRECT);
}

void mplayer_setdisplaymode(int t)
{
	unsigned int d;
	display_change = 1;
	switch(t)
	{
		case DISP_LCD_OUT:
			d = LCD_OUT;
			break;
		case DISP_TV_OUT:
			d = TV_OUT;
			break;
	}

//	if((display_mode & LCD_OUT) == d)
//		return;
//	else
	{
		display_mode |= CHANGE_OUT_MODE;
		SETOUTMODE(d);
		if(d == LCD_OUT)
			display_mode |= DISP_ALL;
		printf("set display_mode = 0x%x\n",display_mode);
	}


}
//for decode alloc buffer
void jz47_soc_set_direct(int mode)
{
	if(mode == 1)
	{
		display_mode |= DECODE_BUFFER;
		printf("set decode buf\n");
	}else
	{
		printf("clear decode buf\n");
		display_mode = ~DECODE_BUFFER;
	}


}

void Init_Frame_Buffer_Mode(mp_image_t *mpi)
{
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
    useframebuf = (int)lcd_get_change_phyframe();
	img_param.out_buf = useframebuf;

	stride.out = get_screen_width() * (img_param.in_bpp / 8 );
	jz47_put_image_with_ipu = jz47_put_image_with_framebuf;
	img_param.y_buf = mpi->planes[0];
	img_param.u_buf = mpi->planes[1];
	img_param.v_buf = mpi->planes[2];

	stride.y = mpi->stride[0];
	stride.u = mpi->stride[1];
	stride.v = mpi->stride[2];

	dump(&img_param);
	lcd_clean_frame_all();
	lcd_ioctl((void*)&img_param,IOCTL_SET_IPU);
}
void Init_Direct_Mode(mp_image_t *mpi)
{
	if ((LCD_OUT & display_mode) == TV_OUT)
	{
		img_param.ipu_d_fmt |= (OUT_FMT_YUV422 | (1 << 16));
		img_param.in_bpp = 16;
	}
	else
	{
		img_param.ipu_d_fmt |= OUT_FMT_RGB888;
		img_param.in_bpp = 32;
	}

	if(display_mode & DECODE_BUFFER)
	{
  		img_param.y_buf = mpi->planes[0];
		img_param.u_buf = mpi->planes[1];
		img_param.v_buf = mpi->planes[2];
		stride.y = mpi->stride[0];
		stride.u = mpi->stride[1];
		stride.v = mpi->stride[2];
		lcd_flush_frame_all();
  		jz47_put_image_with_ipu = jz47_put_image_with_direct_ipu_nobuffer;
		printf("direct buf\n");
	}else
	{

		CreateImage(mpi->width,mpi->height,mpi->chroma_width,mpi->chroma_height,mpi->bpp);
		mpi_copy(yuv_cur,mpi);
		img_param.y_buf = yuv_cur->y_framebuf;
		img_param.u_buf = yuv_cur->u_framebuf;
		img_param.v_buf = yuv_cur->v_framebuf;
		stride.y = yuv_cur->ystride;
		stride.u = yuv_cur->ustride;
		stride.v = yuv_cur->vstride;
		yuv_cur = yuv_cur->nextframe;
		jz47_put_image_with_ipu = jz47_put_image_with_direct_ipu_buffer;
		printf("create buf\n");
	}
 	printf("stride.y = %d\n",stride.y);
  	printf("stride.u = %d\n",stride.u);
  	printf("stride.v = %d\n",stride.v);
	printf("display_mode = %x\n",display_mode);

	lcd_ioctl(&img_param, IOCTL_DMA1_TO_IPU);

}
static void jz47_ipu_init (struct vf_instance_s* vf,mp_image_t *mpi)
{
	F("1\n");
	SwsContext *c = vf->priv->ctx;
	int outW, outH;
	int out_w,out_h;
	int out_x,out_y;
	outW = mpi->width;
	outH = mpi->height;
	c->dstW = outW;
	c->dstH = outH;
	if(ipu_inited)
	  PEND_IPU();
	switch(ipu_size_cfg)
	{
	case MP_IMAGE_TYPE_CUSTORM:
	  out_w = g_out_w;
	  out_h = g_out_h;
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
	img_param.ratio_table = ipu_ratio_table;


{
	img_param.in_width = outW;
	img_param.in_height = outH;
	img_param.out_width = out_w;
	img_param.out_height = out_h;
	img_param.out_x = out_x;
	img_param.out_y = out_y;
	img_param.stride = &stride;
	img_param.csc =	&ipu_csc;
}
	dump(&img_param);
	if((ipu_inited == 0) || (display_mode & CHANGE_OUT_MODE))
	{

		display_mode &= ~CHANGE_OUT_MODE;
		int srcFormat= c->srcFormat;
		switch (srcFormat)
		{
		case PIX_FMT_YUV420P:
			img_param.ipu_d_fmt = IN_FMT_YUV420;
			break;
		case PIX_FMT_YUV422P:
			img_param.ipu_d_fmt = IN_FMT_YUV422;
			break;
		case PIX_FMT_YUV444P:
			img_param.ipu_d_fmt = IN_FMT_YUV444;
			break;
		case PIX_FMT_YUV411P:
			img_param.ipu_d_fmt = IN_FMT_YUV411;
			break;
		}
		get_ipu_addr(ipu_addr);
//		if(display_mode & DISP_ALL)
//		{
//		  if(1)//get_screen_height() * get_screen_width() <= 480 * 272 * 2)
//		  	  display_mode |= DISP_DIRECT;
//		  else
//			  display_mode &= ~DISP_ALL;
//		}

		display_mode &= ~IPU_PEND;
		Init_Direct_Mode(mpi);

//		if((display_mode & LCD_OUT) && (display_mode & DISP_DIRECT))
//		{
//			display_mode &= ~IPU_PEND;
//			Init_Direct_Mode(mpi);
//		}else
//		{
//			printf("Init Frame Buffer mode\n");
//			display_mode |= IPU_PEND;
//			Init_Frame_Buffer_Mode(mpi);
//		}
		ipu_inited = 1;
	}else
	{
		printf("+++++++++++++++++++resize\n");
		if((display_mode & DISP_DIRECT))
		{
			lcd_ioctl(&img_param, IOCTL_LCD_RESIZE_ALL);

		}else
		{
			lcd_clean_frame_all();
			useframebuf = lcd_get_change_phyframe();
			img_param.out_buf = useframebuf;
			lcd_ioctl(&img_param, IOCTL_LCD_RESIZE);
		}
		Flush_OSDScrean();
		printf("-------------------resize\n");
	}
	SetVideoOutMode(mplayer_setdisplaymode);
	framebuf_offset = (img_param.out_x + img_param.out_y * get_screen_width()) * (img_param.in_bpp / 8);
	dump(&img_param);
}


void SetOSDFunc(PFunc_Hook_OSD pFunc)
{
	pFunction = pFunc;
}

void Flush_OSDScrean()
{
	#if USE_OSD
//	F("ipu_inited = %d\n",ipu_inited);
	 if((display_mode & DISP_ALL) == DISP_DIRECT)
	 {
 				if(pFunction)
        {
		  			pFunction(NULL);
						lcd_flush_frame_all();
				}
	 }else
	 {
		 if(ipu_inited)
		 {
			PEND_IPU();
			if(pFunction)
		    {
					pFunction(lcd_get_current_cframe());
		      //memset(lcd_get_current_cframe(),0xff,get_screen_width() * 20);
		      lcd_flush_frame_all();

		   }
			if((unsigned int)lcd_get_current_phyframe() != useframebuf)
		   		lcd_change_frame();
		   //F("lcd_get_current_cframe() = %x\n",lcd_get_current_cframe());
		   //F("useframebuf = %x\n",useframebuf);
		   //F("lcd_get_change_phyframe = %x\n",lcd_get_change_phyframe());

	   }
   }
  #endif
}
void Flush_Screen()
{
	#if USE_OSD
  	if((display_mode & DISP_ALL) == DISP_DIRECT)
	  {
	  			if(pFunction)
	        {
			  			pFunction(NULL);
							lcd_flush_frame_all();
					}
	   }else{
		 if(ipu_inited)
		 {
					PEND_IPU();
					if(pFunction)
	        {
			  			pFunction(lcd_get_current_cframe());
							lcd_flush_frame_all();
	        }
					if((unsigned int)lcd_get_current_phyframe() != useframebuf)
		   		lcd_change_frame();
		}
	}
	#endif
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
void mplayer_showmode(int t)
{
	if(t + 2 <= MP_IMAGE_TYPE_FULLSCREEN)
	{
			display_change = 1;
			ipu_size_cfg = t + 2;
	}
}

  	//#undef printf
static int visual = 1;
static int T_B_Frame = 0;
void drop_image()
{
	visual = 0;
	T_B_Frame = 1;
	F("drop_image\n");
}
void drop_image_seekmode()
{
	visual = 0;
	T_B_Frame = 1;
}


int jz47_put_image(struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{

	  mp_image_t *dmpi=mpi->priv;

//   printf("jz4740: processing whole frame! dmpi=%p flag=%d visual = %d \n",
//   	dmpi, (mpi->flags&MP_IMGFLAG_DRAW_CALLBACK),visual);
  if((mpi->width != img_param.in_width) || (mpi->height !=	img_param.in_height))
  {
  	 	display_change = 1;
  	 	visual = 0;
  	 	mpi->pict_type = 0;
  }
  if(!(mpi->flags&MP_IMGFLAG_DRAW_CALLBACK && dmpi)){

  	//printf("mpi->pict_type = %d visual = %d T_B_Frame = %d\n",mpi->pict_type,visual,T_B_Frame);
      if(mpi->pict_type == 1)
           visual = 1;
    if(((visual) && (T_B_Frame == 0)) || (mpi->pict_type == 0))
    {
    	  jz47_put_image_with_ipu(vf, mpi, pts);
        //mp_cache_delay();
    }else
    {

        if(visual && (mpi->pict_type != 3)) //from mp_image.h 3 = B frame
      	{
            if(mpi->pict_type != 1)
            	T_B_Frame = 0;
            jz47_put_image_with_ipu(vf, mpi, pts);
      	}else
      	{
      		if(mpi->pict_type == 1){
      			 jz47_put_image_with_ipu(vf, mpi, pts);
      			}
      		else
      			Flush_OSDScrean();
      	}

    }
    if(display_change != 0)
    {
        jz47_ipu_init(vf,mpi);
        display_change = 0;
        visual = 1;
    }
  }
 	 return 1;
}
void vpGetImageInfo(int *x,int *y,int *w,int *h)
{

	*x = img_param.out_x;
	*y = img_param.out_y;
	*w = img_param.out_width;
	*h = img_param.out_height;

}
//#ifdef VERIFY_CRC32
#if 1
#define CRC_32 0x04c11db7
unsigned int image_crc32;
unsigned int crc32_tab[256]={
         0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
         0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
         0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
         0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
         0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
         0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
         0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
         0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
         0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
         0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
         0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
         0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
         0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
         0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
         0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
         0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
         0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
         0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
         0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
         0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
         0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
         0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
         0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
         0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
         0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
         0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
         0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
         0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
         0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
         0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
         0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
         0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
         0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
         0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
         0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
         0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
         0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
         0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
         0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
         0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
         0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
         0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
         0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
         0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
         0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
         0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
         0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
         0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
         0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
         0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
         0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
         0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
         0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
         0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
         0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
         0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
         0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
         0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
         0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
         0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
         0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
         0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
         0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
         0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4, };

unsigned int frame_no=0;
unsigned int crc32_verify(unsigned char *buf, int line, int col, int stride)
{
  int i, j, counter = 0;

  for (j = 0; j < line; j++)
  {
    for (i = 0; i < col; i++){
      image_crc32 = ((image_crc32 << 8) | buf[i]) ^ (crc32_tab[(image_crc32 >> 24) & 0xff]);
    }
    buf += stride;
  }
}

#endif
