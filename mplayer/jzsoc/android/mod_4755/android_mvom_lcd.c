#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <linux/fb.h>
#include <unistd.h>

#include "sys/ioctl.h"
#include "sys/mman.h"
#include "fcntl.h"

#include "android_mvom.h"

#define FBDEV_NAME "/dev/graphics/fb1"
#define IPU_TO_LCD

#define FB_TVE_WIDTH 704

volatile unsigned char *frame_buffer = NULL;
//unsigned char *frame_buffer = NULL;
unsigned int fb_w, fb_h, phy_fb, fbmemlen, fb_line_len;
int fbfd = 0;


static int get_fbaddr_info(void)
{
	struct fb_var_screeninfo fbvar;
	struct fb_fix_screeninfo fbfix;

	if ((fbfd = open(FBDEV_NAME, O_RDWR)) == -1) {
		printf("++ ERR: can't open %s ++\n", FBDEV_NAME);
#if defined(BUILD_MPLAYER_AS_SHARED_LIBRARIES)
		return -1;
#endif	/* #define BUILD_MPLAYER_AS_SHARED_LIBRARIES */
	}

	ioctl(fbfd, FBIOGET_VSCREENINFO, &fbvar);
	ioctl(fbfd, FBIOGET_FSCREENINFO, &fbfix);
	
	fb_w = fbvar.xres;
	fb_h = fbvar.yres;
	
	phy_fb= fbfix.smem_start;
	fbmemlen = fbfix.smem_len;
	fb_line_len=  fbfix.line_length;
	
	if ((frame_buffer = (uint8_t *) mmap(0, fbmemlen, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0)) == (uint8_t *) -1) 
	{
		printf ("++ jz4740_soc.c: Can't mmap frame buffer ++\n");
#if defined(BUILD_MPLAYER_AS_SHARED_LIBRARIES)
		return -1;
#endif	/* #define BUILD_MPLAYER_AS_SHARED_LIBRARIES */
	}
	
	printf ("++ jz47 fb_w=%d, fb_h=%d, phy_fb=0x%x, fbmemlen=%d, fb_line_len=%d\n",
		fb_w, fb_h, phy_fb, fbmemlen, fb_line_len);

}

int free_fbaddr_info(void)
{
	struct android_display_info_t adi;
#if 1
	/*disable fg1*/
	adi.flag = ANDROID_SET_FG1_ENABLE;
	adi.fg1_enable = 0;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
#endif
	/*set colorkey of fg0*/
	adi.fg0_colorkey =0x80000000;
	adi.flag = ANDROID_SET_FG0_COLORKEY;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	
	/*set alpha of fg0*/
	adi.fg0_alpha = 0xff;
	adi.flag = ANDROID_SET_FG0_ALPHA;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

#ifdef IPU_TO_LCD
	/*set size of fg1*/
	mvom_lcd_get_size(&adi.fg1_w, &adi.fg1_h);
	adi.flag = ANDROID_SET_FG1_SIZE;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

	/*set position of fg1*/
	adi.fg1_x = 0;
	adi.fg1_y = 0;
	adi.flag = ANDROID_SET_FG1_POS;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

	/*enable RGB=>YCC*/
	adi.rgb_to_ycc = 1;
	adi.flag =  ANDROID_RGBC_YCC;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
#if 1
        /*use dma*/
	//mvoc->ipu_direct_enable=0;
	adi.fg1_short_cut = 0;//mvoc->ipu_direct_enable;
	adi.flag =  ANDROID_SET_FG1_IPU_DIRECT;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
#endif

#endif

	munmap(frame_buffer,fbmemlen);

	close(fbfd);

	return 0;
}


int mvom_lcd_init(void)
{
	get_fbaddr_info();
	return 0;
}

int mvom_lcd_exit(void)
{
	free_fbaddr_info();
	return 0;
}

int mvom_lcd_get_size(int *w, int *h)
{
	struct android_display_info_t adi;

	adi.flag = ANDROID_GET_PANEL_SIZE;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	
	*w = adi.fg1_w;
	*h = adi.fg1_h;

	return 0;
}

int mvom_lcd_setup(struct mplayer_video_output_config_t *mvoc, int x, int y, int w, int h)
{
	struct android_display_info_t adi;
	printf("--------mvom_lcd_setup---------\n");
#if 0	
	adi.flag = ANDROID_SET_FG1_ENABLE;
	adi.fg1_enable = 0;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
#endif
#ifdef IPU_TO_LCD
	/*set size of fg1*/
	adi.fg1_w = w;
	adi.fg1_h = h;
	adi.flag = ANDROID_SET_FG1_SIZE;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

	/*set position of fg1*/
	adi.fg1_x = x;
	adi.fg1_y = y;
	adi.flag = ANDROID_SET_FG1_POS;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

#endif
	/*set colorkey of fg0*/
	adi.fg0_colorkey = mvoc->fg0_colorkey;
	adi.flag = ANDROID_SET_FG0_COLORKEY;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	
	/*set alpha of fg0*/
	adi.fg0_alpha = mvoc->fg0_alpha;
	adi.flag = ANDROID_SET_FG0_ALPHA;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

#ifndef IPU_TO_LCD	
	memset(frame_buffer,0,fb_w*(fb_h+10)*2);
#endif
#if 1	
	adi.flag = ANDROID_SET_FG1_ENABLE;
	adi.fg1_enable = 1;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
#endif
	return 0;
}


int set_lcd_fg1(struct mplayer_video_output_config_t *mvoc)
{
	struct android_display_info_t adi;
#if 0

	/*enable fg1*/

	adi.flag = ANDROID_SET_FG1_ENABLE;
	adi.fg1_enable = 1;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
#endif
	/*set ipu_direct*/
	mvoc->ipu_direct_enable=1;
	adi.fg1_short_cut = mvoc->ipu_direct_enable;
	adi.flag =  ANDROID_SET_FG1_IPU_DIRECT;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

	return 0;
}
