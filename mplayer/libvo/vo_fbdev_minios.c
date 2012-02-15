/* 
 *  video_out_null.c
 *
 *	Copyright (C) Aaron Holtzman - June 2000
 *
 *  This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 *	
 *  mpeg2dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  mpeg2dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <mplaylib.h>
#include "mplaylib.h"

#include <errno.h>
#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "video_out.h"
#include "video_out_internal.h"
#undef memcpy
#define memcpy uc_memcpy
static vo_info_t info = 
{
	"Null video output",
	"null",
	"Aaron Holtzman <aholtzma@ess.engr.uvic.ca>",
	""
};

LIBVO_EXTERN(fbminios)

static uint32_t image_width, image_height;
static int fb_line_len;
static uint8_t *center;	/* thx .so :) */

extern unsigned int lcd_get_width(void);
extern unsigned int lcd_get_height(void);
extern unsigned int lcd_get_bpp(void);
extern unsigned char* lcd_get_frame(void);
extern unsigned char* lcd_get_cframe(void);
extern unsigned char* lcd_get_change_frame(void);
//static uint32_t
//static int draw_slice(uint8_t *image[], int stride[], int w,int h,int x,int y)
static int draw_slice(uint8_t *src[], int stride[], int w, int h, int x,
		int y)
//draw_slice(uint8_t *src[], uint32_t slice_num)
{
	F("draw_slice\n");
	
	#if 0
	uint8_t *in = image[0];
  uint8_t *dest = lcd_get_change_frame();
	int i;
  int dx_size = w * 4;
  int in_size = lcd_get_width() * 4;
	for (i = 0; i < h; i++) {
		memcpy(dest, in, dx_size);
		dest += in_size;
		in += stride[0];
	}
	#endif
	uint8_t *d;
	uint8_t *s;

	d = center + fb_line_len * y + 4 * x;

	s = src[0];
	while (h) {
		memcpy(d, s, w * 4);
		d += fb_line_len;
		s += stride[0];
		h--;
	}

	return 0;
}

static void draw_osd(void)
{
	//F("draw_osd\n");
}

static void
flip_page(void)
{
		//F("flip_page\n");
}

static int
draw_frame(uint8_t *src[])
{
	F("draw_frame\n");
	return 0;
}
static int
query_format(uint32_t format)
{
	if ((format & IMGFMT_BGR_MASK) == IMGFMT_BGR) {	
		int fb_target_bpp = format & 0xff;
	//F("fb_target_bpp = %d\n",fb_target_bpp);

		if (32 == fb_target_bpp)
			return VFCAP_CSP_SUPPORTED|VFCAP_CSP_SUPPORTED_BY_HW|VFCAP_ACCEPT_STRIDE;
	}
	return 0;
}
static int
config(uint32_t width, uint32_t height, uint32_t d_width, uint32_t d_height, uint32_t flags, char *title, uint32_t format)
{
	F("config\n");
	image_width = width;
	image_height = height;
	
	fb_line_len = lcd_get_width() * 4;
	center = (uint8_t *)((int)lcd_get_frame() + fb_line_len + lcd_get_height() / 2);
	return 0;
}

static void
uninit(void)
{
}


static void check_events(void)
{
}
static signed int pre_init_err = -2;
static int preinit(const char *vo_subdevice)
{
    pre_init_err = 0;

    if(vo_subdevice)
    {
#ifdef CONFIG_VIDIX
	if (memcmp(vo_subdevice, "vidix", 5) == 0)
	    vidix_name = &vo_subdevice[5];
	if(vidix_name)
	    pre_init_err = vidix_preinit(vidix_name,&video_out_fbdev);
	
#endif
	  }
    if(!pre_init_err) return 0;
    return(-1);
}
static uint32_t get_image(mp_image_t *mpi)
{
   //F("1\n");
    
    if (
	!IMGFMT_IS_BGR(mpi->imgfmt) ||
	(IMGFMT_BGR_DEPTH(mpi->imgfmt) != 32) ||
	((mpi->type != MP_IMGTYPE_STATIC) && (mpi->type != MP_IMGTYPE_TEMP)) ||
	(mpi->flags & MP_IMGFLAG_PLANAR) ||
	(mpi->flags & MP_IMGFLAG_YUV)
	 )
    return(VO_FALSE);
   //F("2\n");
    //center = (uint8_t *)((int)lcd_get_change_frame() + fb_line_len + lcd_get_height() / 2);
    mpi->planes[0] = center;
    mpi->stride[0] = fb_line_len;
    mpi->flags |= MP_IMGFLAG_DIRECT;
    
    return(VO_TRUE);
}
static int control(uint32_t request, void *data, ...)
{
  switch (request) {
  case VOCTRL_QUERY_FORMAT:
    return query_format(*((uint32_t*)data));
    case VOCTRL_GET_IMAGE:
   return get_image(data);
  }
  return VO_NOTIMPL;
}
