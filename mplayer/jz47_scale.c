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

#include "../../inc/ipu_app.h"
#include "../../inc/lcd_app.h"

#include <uclib.h>
#undef printf

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



//from jz4750 default 
#if 0
#define YUV_CSC_C0 0x4A8        /* 1.164 * 1024 */
#define YUV_CSC_C1 0x662        /* 1.596 * 1024 */
#define YUV_CSC_C2 0x191        /* 0.392 * 1024 */
#define YUV_CSC_C3 0x341        /* 0.813 * 1024 */
#define YUV_CSC_C4 0x811        /* 2.017 * 1024 */
#define YUV_CSC_CHROM 0x80
#define YUV_CSC_LUMA  0x20
#endif
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

static int g_out_x = 0,
					 g_out_y = 0,
					 g_out_w = 0,
					 g_out_h = 0;

#define FRAMEBUF_16BIT 0
#define DISPLAY_DIRECT 1
#define DISPLAY_TV_OUT 2
#define DISPLAY_ALL    3
static unsigned int display_mode = 0;

typedef void (*PFunc_Hook_OSD)(void *fram);
static PFunc_Hook_OSD pFunction = NULL;


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

static int CreateImage(int w,int h,int cw,int ch,int bpp)
{
	int i;
	int *framebuf;
	printf("disp_w = %d disp_h = %d cw = %d ch = %d bpp = %d\n",w,h,cw,ch,bpp);

	framebuf = (int *)memalign(64,(w * h * 2) * 3);
	
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

//--------------------------------------------------------------
int  ipu_image_start(int w,int h)
{
	
	return 1;
}
void ipu_image_stop()
{
	lcd_ioctl(0, IOCTL_IPU_TO_DMA1);
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
	F("-----------------------------\n");
}


static int ipu_inited = 0;
void mpi_copy(PYUVFrame yuvframe,mp_image_t* mpi){

//	memset(dmpi->planes[0], 0, dmpi->stride[0]*dmpi->h);
//  memset(dmpi->planes[1], 128, dmpi->stride[1]*dmpi->h>>dmpi->chroma_y_shift);
//  memset(dmpi->planes[2], 128, dmpi->stride[2]*dmpi->h>>dmpi->chroma_y_shift);
    printf("mpi->stride[0]*mpi->h = %d\n",mpi->stride[0]*mpi->h);
    printf("mpi->stride[1]*mpi->h>>mpi->chroma_y_shift = %d\n",mpi->stride[1]*mpi->h>>mpi->chroma_y_shift);
    char * ty,*tu,*tv,*sy,*su,*sv;
    ty = yuvframe->y_framebuf;
    tu = yuvframe->u_framebuf;
    tv = yuvframe->v_framebuf;
    
    sy = mpi->planes[0];
    su = mpi->planes[1];
    sv = mpi->planes[2];
	
	int i;
	for(i = 0;i < mpi->height;i++)
	{
		memcpy(ty,sy,mpi->width);
		ty += mpi->width;
		sy += mpi->stride[0];
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
}
static int (*jz47_put_image_with_ipu)(struct vf_instance_s* vf, mp_image_t *mpi, double pts) = jz47_put_image_with_default;
int jz47_put_image_with_direct_ipu (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
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
int jz47_put_image_with_framebuf (struct vf_instance_s* vf, mp_image_t *mpi, double pts)
{
		lcd_ioctl((void *)0,IOCTL_PENDING_IPU_END);
		if(pFunction)
    {
                //pFunction(lcd_get_change_cframe());
        pFunction(lcd_get_current_cframe());
        lcd_flush_frame_all();
    }
		lcd_change_frame();
		img_param.y_buf = mpi->planes[0];
		img_param.u_buf = mpi->planes[1];
		img_param.v_buf = mpi->planes[2];
		
		stride.y = mpi->stride[0];
	  stride.u = mpi->stride[1];
	  stride.v = mpi->stride[2];
		img_param.out_buf = lcd_get_change_phyframe();
		lcd_flush_frame_all();
		lcd_ioctl((void*)&img_param,IOCTL_SET_IPU);
		
		return 1;
}
#define DYM_CONVERT (480*272*2*2)
void jz47_auto_select(int w,int h)
{
		if(w * h > DYM_CONVERT)
			display_mode = FRAMEBUF_16BIT;
		else
			display_mode = DISPLAY_DIRECT;
}		
int get_display_is_direct()
{
	return (display_mode == DISPLAY_DIRECT);
}	
void jz47_ipu_init (struct vf_instance_s* vf,mp_image_t *mpi)
{
	F("1\n");
	SwsContext *c = vf->priv->ctx;
  int outW, outH;
  int out_w,out_h;
  int out_x,out_y;
  outW= c->dstW;
  outH= c->dstH;
  
  if((ipu_inited == 0)&&(display_mode == DISPLAY_ALL))
	{
			if(mpi->width * mpi->height > DYM_CONVERT)
				display_mode = FRAMEBUF_16BIT;
			else
				display_mode = DISPLAY_DIRECT;			
	}
  
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
		   	break;
		   case MP_IMAGE_TYPE_FULLSCREEN:
		   		fullscreen(outW,outH,&out_x,&out_y,&out_w,&out_h);
		   	break;	
		}
  img_param.ratio_table = ipu_ratio_table;
	
  int srcFormat= c->srcFormat;
	switch (srcFormat)
  {
    case PIX_FMT_YUV420P:
      img_param.ipu_d_fmt |= IN_FMT_YUV420;
      break;
    case PIX_FMT_YUV422P:
    	img_param.ipu_d_fmt |= IN_FMT_YUV422;
      break;
    case PIX_FMT_YUV444P:
      img_param.ipu_d_fmt |= IN_FMT_YUV444;
      break;
    case PIX_FMT_YUV411P:
      img_param.ipu_d_fmt |= IN_FMT_YUV411;
      break;
  }
  
	img_param.in_width = outW;
	img_param.in_height = outH;
	img_param.out_width = out_w;
	img_param.out_height = out_h;
	img_param.out_x = out_x;
	img_param.out_y = out_y;
	img_param.stride = &stride;
	img_param.csc =	&ipu_csc;
	//init_ipu_ratio_table();
	dump(&img_param);
	if(ipu_inited == 0)
	{	
		if(display_mode == DISPLAY_ALL)
		{
			#define DYM_CONVERT 480*272*2*2
			if(mpi->width * mpi->height > DYM_CONVERT)
				display_mode = FRAMEBUF_16BIT;
			else
				display_mode = DISPLAY_DIRECT;			
		}
		switch(display_mode)
		{
			case FRAMEBUF_16BIT:
			case DISPLAY_TV_OUT:
				img_param.out_buf = lcd_get_change_phyframe();
				
				  switch (get_screen_mode())
				  {
					  case PIX_FMT_RGBA32:
				      img_param.ipu_d_fmt = OUT_FMT_RGB888;
				      img_param.in_bpp = 32;
				      break;
				
				    case PIX_FMT_RGB555:
				      img_param.ipu_d_fmt = OUT_FMT_RGB555;
				      img_param.in_bpp = 16;
				      break;
				
				    case PIX_FMT_RGB565:
				      img_param.ipu_d_fmt = OUT_FMT_RGB565;
				      img_param.in_bpp = 16;
				      break;
				   case PIX_FMT_YUV422P:
				   		img_param.ipu_d_fmt = OUT_FMT_YUV422 | (1 << 16); //(1 << 16)Êä³ö¸ñÊ½ Y1V0Y0U0
				    	img_param.in_bpp = 16;
				    	break;
				  }
			
			stride.out = out_w * (img_param.in_bpp / 8);
			jz47_put_image_with_ipu = jz47_put_image_with_framebuf;
				
			img_param.y_buf = mpi->planes[0];
			img_param.u_buf = mpi->planes[1];
			img_param.v_buf = mpi->planes[2];
			
			stride.y = mpi->stride[0];
		  stride.u = mpi->stride[1];
		  stride.v = mpi->stride[2];
			
			lcd_ioctl((void*)&img_param,IOCTL_SET_IPU);
				break;
			case DISPLAY_DIRECT:
				img_param.ipu_d_fmt = OUT_FMT_RGB888;
				img_param.in_bpp = 32;
				stride.out = out_w * (img_param.in_bpp / 8);
				CreateImage(mpi->width,mpi->height,mpi->chroma_width,mpi->chroma_height,mpi->bpp);
				mpi_copy(yuv_cur,mpi);
				img_param.y_buf = yuv_cur->y_framebuf;
		 		img_param.u_buf = yuv_cur->u_framebuf;
		 		img_param.v_buf = yuv_cur->v_framebuf;	  
		  	stride.y = yuv_cur->ystride;
		  	stride.u = yuv_cur->ustride;
		  	stride.v = yuv_cur->vstride;
		  	yuv_cur = yuv_cur->nextframe;
		  	printf("stride.y = %d\n",stride.y);
		  	printf("stride.u = %d\n",stride.u);
		  	printf("stride.v = %d\n",stride.v);
				lcd_ioctl(&img_param, IOCTL_DMA1_TO_IPU);
				jz47_put_image_with_ipu = jz47_put_image_with_direct_ipu;
				break;
			
				
				break;
		}
		
		ipu_inited = 1;
	}else
	{
		printf("+++++++++++++++++++resize\n");
		switch(display_mode)
		{
			case FRAMEBUF_16BIT:
			case DISPLAY_TV_OUT:
				lcd_ioctl(&img_param, IOCTL_LCD_RESIZE_ALL);
				break;
				
			case DISPLAY_DIRECT:
				lcd_ioctl(&img_param, IOCTL_LCD_RESIZE);
				break;
		}
		printf("-------------------resize\n");
	}
	dump(&img_param);
}


void SetOSDFunc(PFunc_Hook_OSD pFunc)
{
	pFunction = pFunc;
}

void Flush_OSDScrean()
{
	 if(pFunction)
   {
			pFunction(lcd_get_current_cframe());
      lcd_flush_frame_all();
   }
}
void Flush_Screen()
{
				if(pFunction)
        {         
		  			pFunction(lcd_get_current_cframe());
						lcd_flush_frame_all();
        }

      lcd_change_frame();
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
	}
	else 
  { 
  		if(w > FB_LCD_WIDTH) w = FB_LCD_WIDTH;
  		if(h > FB_LCD_HEIGHT) h = FB_LCD_HEIGHT;
  			
  		g_out_w = w;
  		g_out_h = h;
  		ipu_size_cfg = 1;
    	
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
	    if(ipu_size_cfg != 0)
			{
				jz47_ipu_init(vf,mpi);
				ipu_size_cfg = 0;
			}
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
