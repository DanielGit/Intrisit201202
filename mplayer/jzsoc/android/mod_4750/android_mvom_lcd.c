#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <linux/fb.h>
//#include <unistd.h>

#include "sys/ioctl.h"
#include "sys/mman.h"
#include "fcntl.h"

#include "android_mvom.h"

#define FBDEV_NAME "/dev/graphics/fb1"
#define IPU_TO_LCD

#define  DISP_LCD_OUT 0x01

volatile unsigned char *frame_buffer = NULL;
//unsigned char *frame_buffer = NULL;
unsigned int fb_w, fb_h, phy_fb, fbmemlen, fb_line_len;
int fbfd = 0;
unsigned int fg1_first_start=1;
extern unsigned int display_mode;
struct android_display_info_t adi={0};

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

	return 0;

}

static int free_fbaddr_info(void)
{
  //	struct android_display_info_t adi;

	/*disable fg1*/
	adi.flag = ANDROID_SET_FG1_ENABLE;
	adi.fg1_enable = 0;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

	/*set colorkey of fg0*/
	adi.fg0_colorkey =0x80000000;
	adi.flag = ANDROID_SET_FG0_COLORKEY;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	
	/*set alpha of fg0*/
	adi.fg0_alpha = 0xff;
	adi.flag = ANDROID_SET_FG0_ALPHA;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);

#ifdef IPU_TO_LCD
#if 1//1106
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
        fg1_first_start = 1;

	free_fbaddr_info();
	return 0;
}

int mvom_lcd_get_size(unsigned int *new_display_mode, int *w, int *h)
{
  //struct android_display_info_t adi;

	adi.flag = ANDROID_GET_PANEL_SIZE;
	ioctl(fbfd,FBIO_ANDROID_CTL, &adi);
	
	*w = adi.fg1_w;
	*h = adi.fg1_h;
	*new_display_mode = adi.fg0_index;
	display_mode = *new_display_mode;

	return 0;
}

int mvom_lcd_setup(struct mplayer_video_output_config_t *mvoc, int x, int y, int w, int h)
{
  //	struct android_display_info_t adi;
	printf("--------mvom_lcd_setup---------\n");

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

	return 0;
}
