/*
 *  video_out.h
 *
 *      Copyright (C) Aaron Holtzman - Aug 1999
 *	Strongly modified, most parts rewritten: A'rpi/ESP-team - 2000-2001
 *	(C) MPlayer Developers
 *
 */
 
#ifndef VIDEO_OUT_H
#define VIDEO_OUT_H

#include <inttypes.h>
#include <stdarg.h>

//#include "font_load.h"
#include "libmpcodecs/img_format.h"
//#include "vidix/vidix.h"

#define VO_EVENT_EXPOSE 1
#define VO_EVENT_RESIZE 2
#define VO_EVENT_KEYPRESS 4

/* Obsolete: VOCTRL_QUERY_VAA 1 */
/* does the device support the required format */
#define VOCTRL_QUERY_FORMAT 2
/* signal a device reset seek */
#define VOCTRL_RESET 3
/* true if vo driver can use GUI created windows */
#define VOCTRL_GUISUPPORT 4
#define VOCTRL_GUI_NOWINDOW 19
/* used to switch to fullscreen */
#define VOCTRL_FULLSCREEN 5
#define VOCTRL_SCREENSHOT 6
/* signal a device pause */
#define VOCTRL_PAUSE 7
/* start/resume playback */
#define VOCTRL_RESUME 8
/* libmpcodecs direct rendering: */
#define VOCTRL_GET_IMAGE 9
#define VOCTRL_DRAW_IMAGE 13
#define VOCTRL_SET_SPU_PALETTE 14
/* decoding ahead: */
#define VOCTRL_GET_NUM_FRAMES 10
#define VOCTRL_GET_FRAME_NUM  11
#define VOCTRL_SET_FRAME_NUM  12
#define VOCTRL_GET_PANSCAN 15
#define VOCTRL_SET_PANSCAN 16
/* equalizer controls */
#define VOCTRL_SET_EQUALIZER 17
#define VOCTRL_GET_EQUALIZER 18
//#define VOCTRL_GUI_NOWINDOW 19
/* Frame duplication */
#define VOCTRL_DUPLICATE_FRAME 20
// ... 21
#define VOCTRL_START_SLICE 21

#define VOCTRL_ONTOP 25
#define VOCTRL_ROOTWIN 26
#define VOCTRL_BORDER 27
#define VOCTRL_DRAW_EOSD 28
#define VOCTRL_GET_EOSD_RES 29
typedef struct {
  int w, h; // screen dimensions, including black borders
  int mt, mb, ml, mr; // borders (top, bottom, left, right)
} mp_eosd_res_t;

#define VOCTRL_SET_DEINTERLACE 30
#define VOCTRL_GET_DEINTERLACE 31

#define VOCTRL_UPDATE_SCREENINFO 32

// Vo can be used by xover
#define VOCTRL_XOVERLAY_SUPPORT 22

#define VOCTRL_XOVERLAY_SET_COLORKEY 24
typedef struct {
  uint32_t x11; // The raw x11 color
  uint16_t r,g,b;
} mp_colorkey_t;

#define VOCTRL_XOVERLAY_SET_WIN 23
typedef struct {
  int x,y;
  int w,h;
} mp_win_t;

#define VO_TRUE		1
#define VO_FALSE	0
#define VO_ERROR	-1
#define VO_NOTAVAIL	-2
#define VO_NOTIMPL	-3

#define VOFLAG_FULLSCREEN	0x01
#define VOFLAG_MODESWITCHING	0x02
#define VOFLAG_SWSCALE		0x04
#define VOFLAG_FLIPPING		0x08
#define VOFLAG_XOVERLAY_SUB_VO  0x10000

typedef struct vo_info_s
{
        /* driver name ("Matrox Millennium G200/G400" */
        const char *name;
        /* short name (for config strings) ("mga") */
        const char *short_name;
        /* author ("Aaron Holtzman <aholtzma@ess.engr.uvic.ca>") */
        const char *author;
        /* any additional comments */
        const char *comment;
} vo_info_t;

typedef struct vo_functions_s
{
	vo_info_t *info;
	/*
	 * Preinitializes driver (real INITIALIZATION)
	 *   arg - currently it's vo_subdevice
	 *   returns: zero on successful initialization, non-zero on error.
	 */
	int (*preinit)(const char *arg);
        /*
         * Initialize (means CONFIGURE) the display driver.
	 * params:
         *   width,height: image source size
	 *   d_width,d_height: size of the requested window size, just a hint
	 *   fullscreen: flag, 0=windowd 1=fullscreen, just a hint
	 *   title: window title, if available
	 *   format: fourcc of pixel format
         * returns : zero on successful initialization, non-zero on error.
         */
        int (*config)(uint32_t width, uint32_t height, uint32_t d_width,
			 uint32_t d_height, uint32_t fullscreen, char *title,
			 uint32_t format);

	/*
	 * Control interface
	 */
	int (*control)(uint32_t request, void *data, ...);

        /*
         * Display a new RGB/BGR frame of the video to the screen.
         * params:
	 *   src[0] - pointer to the image
         */
        int (*draw_frame)(uint8_t *src[]);

        /*
         * Draw a planar YUV slice to the buffer:
	 * params:
	 *   src[3] = source image planes (Y,U,V)
         *   stride[3] = source image planes line widths (in bytes)
	 *   w,h = width*height of area to be copied (in Y pixels)
         *   x,y = position at the destination image (in Y pixels)
         */
        int (*draw_slice)(uint8_t *src[], int stride[], int w,int h, int x,int y);

   	/*
         * Draws OSD to the screen buffer
         */
        void (*draw_osd)(void);

        /*
         * Blit/Flip buffer to the screen. Must be called after each frame!
         */
        void (*flip_page)(void);

        /*
         * This func is called after every frames to handle keyboard and
	 * other events. It's called in PAUSE mode too!
         */
        void (*check_events)(void);

        /*
         * Closes driver. Should restore the original state of the system.
         */
        void (*uninit)(void);

} vo_functions_t;

vo_functions_t* init_best_video_out(char** vo_list);
int config_video_out(vo_functions_t *vo, uint32_t width, uint32_t height,
                     uint32_t d_width, uint32_t d_height, uint32_t flags,
                     char *title, uint32_t format);
void list_video_out(void);

// NULL terminated array of all drivers
extern vo_functions_t* video_out_drivers[];

extern int vo_flags;

extern int vo_config_count;

extern int xinerama_screen;
extern int xinerama_x;
extern int xinerama_y;

// correct resolution/bpp on screen:  (should be autodetected by vo_init())
extern int vo_depthonscreen;
extern int vo_screenwidth;
extern int vo_screenheight;

// requested resolution/bpp:  (-x -y -bpp options)
extern int vo_dx;
extern int vo_dy;
extern int vo_dwidth;
extern int vo_dheight;
extern int vo_dbpp;

extern int vo_grabpointer;
extern int vo_doublebuffering;
extern int vo_directrendering;
extern int vo_vsync;
extern int vo_fs;
extern int vo_fsmode;
extern float vo_panscan;
extern int vo_adapter_num;
extern int vo_refresh_rate;
extern int vo_keepaspect;
extern int vo_rootwin;
extern int vo_ontop;
extern int vo_border;

extern int vo_gamma_gamma;
extern int vo_gamma_brightness;
extern int vo_gamma_saturation;
extern int vo_gamma_contrast;
extern int vo_gamma_hue;
extern int vo_gamma_red_intensity;
extern int vo_gamma_green_intensity;
extern int vo_gamma_blue_intensity;

extern int vo_nomouse_input;

extern int vo_pts;
extern float vo_fps;

extern char *vo_subdevice;

extern int vo_colorkey;

extern int WinID;

#if defined(HAVE_FBDEV)||defined(HAVE_VESA) 

typedef struct {
        float min;
	float max;
	} range_t;

extern float range_max(range_t *r);
extern int in_range(range_t *r, float f);
extern range_t *str2range(char *s);
extern char *monitor_hfreq_str;
extern char *monitor_vfreq_str;
extern char *monitor_dotclock_str;

#endif 
		
#endif
