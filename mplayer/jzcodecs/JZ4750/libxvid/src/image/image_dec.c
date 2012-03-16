/**************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Image management functions -
 *
 *  Copyright(C) 2001-2004 Peter Ross <pross@xvid.org>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: image_dec.c,v 1.3 2009/11/05 00:53:28 dyang Exp $
 *
 ****************************************************************************/

#ifdef _UCOS_
#include <mplaylib.h>
#include "mplaylib.h"
#else
#include <stdlib.h>
#include <string.h>				/* memcpy, memset */
#include <math.h>
#endif
#include "../portab.h"
#include "../global.h"			/* XVID_CSP_XXX's */
#include "../xvid.h"			/* XVID_CSP_XXX's */
#include "image.h"
#include "colorspace.h"
#include "interpolate8x8.h"
#include "../utils/mem_align.h"
#include "../motion/sad.h"

#include "font.h"		/* XXX: remove later */

#define SAFETY	64
#define EDGE_SIZE2  (EDGE_SIZE/2)

int32_t
image_create(IMAGE * image,
			 uint32_t edged_width,
			 uint32_t edged_height)
{
	const uint32_t edged_width2 = edged_width / 2;
	const uint32_t edged_height2 = edged_height / 2;

	image->y = XVIDMALLOC(edged_width * (edged_height + 1) + SAFETY, CACHE_LINE);
	if (image->y == NULL) {
		return -1;
	}
	memset(image->y, 0, edged_width * (edged_height + 1) + SAFETY);

	image->u = XVIDMALLOC(edged_width2 * edged_height2 + SAFETY, CACHE_LINE);
	if (image->u == NULL) {
		XVIDFREE(image->y);
		image->y = NULL;
		return -1;
	}
	memset(image->u, 0, edged_width2 * edged_height2 + SAFETY);

	image->v = XVIDMALLOC(edged_width2 * edged_height2 + SAFETY, CACHE_LINE);
	if (image->v == NULL) {
		XVIDFREE(image->u);
		image->u = NULL;
		XVIDFREE(image->y);
		image->y = NULL;
		return -1;
	}
	memset(image->v, 0, edged_width2 * edged_height2 + SAFETY);

	image->y += EDGE_SIZE * edged_width + EDGE_SIZE;
	image->u += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;
	image->v += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;

	return 0;
}



void
image_destroy(IMAGE * image,
			  uint32_t edged_width,
			  uint32_t edged_height)
{
	const uint32_t edged_width2 = edged_width / 2;

	if (image->y) {
		XVIDFREE(image->y - (EDGE_SIZE * edged_width + EDGE_SIZE));
		image->y = NULL;
	}
	if (image->u) {
		XVIDFREE(image->u - (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2));
		image->u = NULL;
	}
	if (image->v) {
		XVIDFREE(image->v - (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2));
		image->v = NULL;
	}
}


void
image_swap(IMAGE * image1,
		   IMAGE * image2)
{
    SWAP(uint8_t*, image1->y, image2->y);
    SWAP(uint8_t*, image1->u, image2->u);
    SWAP(uint8_t*, image1->v, image2->v);
}


void
image_copy(IMAGE * image1,
		   IMAGE * image2,
		   uint32_t edged_width,
		   uint32_t height)
{
	memcpy(image1->y, image2->y, edged_width * height);
	memcpy(image1->u, image2->u, edged_width * height / 4);
	memcpy(image1->v, image2->v, edged_width * height / 4);
}

/* setedges bug was fixed in this BS version */
#define SETEDGES_BUG_BEFORE		18

void
image_setedges(IMAGE * image,
			   uint32_t edged_width,
			   uint32_t edged_height,
			   uint32_t width,
			   uint32_t height,
			   int bs_version)
{
	uint32_t width2;
	uint32_t i;
	uint8_t *dst;
	uint8_t *src;
	const uint32_t edged_width2 = edged_width / 2;
	/*this should be bigger than 16 for quartet pixel interpolate,and smalller than 64*/
	const register int edge_size_lum = 19;  
	/*this should be bigger than 8 and smaller than 32*/
	const register int edge_size_chrom = 10; 
	const register int edge_step_lum = edged_width - 2*EDGE_SIZE +edge_size_lum;
	const register int edge_step_chrom = edged_width2 - 2*EDGE_SIZE2 +edge_size_chrom;

	dst = image->y - (edge_size_lum + edge_size_lum * edged_width);
	src = image->y;

	/* According to the Standard Clause 7.6.4, padding is done starting at 16
	 * pixel width and height multiples. This was not respected in old xvids */
	if (bs_version == 0 || bs_version >= SETEDGES_BUG_BEFORE) {
		width  = (width+15)&~15;
		height = (height+15)&~15;
	}

	width2 = width/2;

	for (i = 0; i < edge_size_lum; i++) {
		memset(dst, *src, edge_size_lum);
		memcpy(dst + edge_size_lum, src, width);
		memset(dst + edge_step_lum, *(src + width - 1),
			   edge_size_lum);
		dst += edged_width;
	}

	for (i = 0; i < height; i++) {
		memset(dst, *src,edge_size_lum);
		memset(dst + edge_step_lum, src[width - 1], edge_size_lum);
		dst += edged_width;
		src += edged_width;
	}

	src -= edged_width;
	for (i = 0; i < edge_size_lum; i++) {
		memset(dst, *src, edge_size_lum);
		memcpy(dst + edge_size_lum, src, width);
		memset(dst + edge_step_lum, *(src + width - 1),
				   edge_size_lum);
		dst += edged_width;
	}


	/* U */
	dst = image->u - (edge_size_chrom + edge_size_chrom * edged_width2);
	src = image->u;

	for (i = 0; i < edge_size_chrom; i++) {
		memset(dst, *src, edge_size_chrom);
		memcpy(dst + edge_size_chrom, src, width2);
		memset(dst + edge_step_chrom, *(src + width2 - 1),
			   edge_size_chrom);
		dst += edged_width2;
	}

	for (i = 0; i < height / 2; i++) {
		memset(dst, *src, edge_size_chrom);
		memset(dst + edge_step_chrom, src[width2 - 1],edge_size_chrom);
		dst += edged_width2;
		src += edged_width2;
	}
	src -= edged_width2;
	for (i = 0; i < edge_size_chrom; i++) {
		memset(dst, *src, edge_size_chrom);
		memcpy(dst + edge_size_chrom, src, width2);
		memset(dst + edge_step_chrom, *(src + width2 - 1),
			   edge_size_chrom);
		dst += edged_width2;
	}


	/* V */
	dst = image->v - (edge_size_chrom + edge_size_chrom * edged_width2);
	src = image->v;

	for (i = 0; i < edge_size_chrom; i++) {
		memset(dst, *src, edge_size_chrom);
		memcpy(dst + edge_size_chrom, src, width2);
		memset(dst + edge_step_chrom, *(src + width2 - 1),
			   edge_size_chrom);
		dst += edged_width2;
	}

	for (i = 0; i < height / 2; i++) {
		memset(dst, *src, edge_size_chrom);
		memset(dst + edge_step_chrom, *(src + width2 - 1),
		       edge_size_chrom);
		dst += edged_width2;
		src += edged_width2;
	}
	src -= edged_width2;
	for (i = 0; i < edge_size_chrom; i++) {
		memset(dst, *src, edge_size_chrom);
		memcpy(dst + edge_size_chrom, src, width2);
		memset(dst + edge_step_chrom, *(src + width2 - 1),
			   edge_size_chrom);
		dst += edged_width2;
	}
}

/*
  perform safe packed colorspace conversion, by splitting
  the image up into an optimized area (pixel width divisible by 16),
  and two unoptimized/plain-c areas (pixel width divisible by 2)
*/

static void
safe_packed_conv(uint8_t * x_ptr, int x_stride,
				 uint8_t * y_ptr, uint8_t * u_ptr, uint8_t * v_ptr,
				 int y_stride, int uv_stride,
				 int width, int height, int vflip,
				 packedFunc * func_opt, packedFunc func_c, int size)
{
	int width_opt, width_c;

	if (func_opt != func_c && x_stride < size*((width+15)/16)*16)
	{
		width_opt = width & (~15);
		width_c = width - width_opt;
	}
	else
	{
		width_opt = width;
		width_c = 0;
	}

	func_opt(x_ptr, x_stride,
			y_ptr, u_ptr, v_ptr, y_stride, uv_stride,
			width_opt, height, vflip);

	if (width_c)
	{
		func_c(x_ptr + size*width_opt, x_stride,
			y_ptr + width_opt, u_ptr + width_opt/2, v_ptr + width_opt/2,
			y_stride, uv_stride, width_c, height, vflip);
	}
}

int
image_output(IMAGE * image,
			 uint32_t width,
			 int height,
			 uint32_t edged_width,
			 uint8_t * dst[4],
			 int dst_stride[4],
			 int csp,
			 int interlacing)
{
	const int edged_width2 = edged_width/2;
	int height2 = height/2;

/*
	if (interlacing)
		image_printf(image, edged_width, height, 5,100, "[i]=%i,%i",width,height);
	image_dump_yuvpgm(image, edged_width, width, height, "\\decode.pgm");
*/

	switch (csp & ~XVID_CSP_VFLIP) {
	case XVID_CSP_RGB555:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgb555i  :yv12_to_rgb555,
			interlacing?yv12_to_rgb555i_c:yv12_to_rgb555_c, 2);
		return 0;

	case XVID_CSP_RGB565:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgb565i  :yv12_to_rgb565,
			interlacing?yv12_to_rgb565i_c:yv12_to_rgb565_c, 2);
		return 0;

    case XVID_CSP_BGR:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_bgri  :yv12_to_bgr,
			interlacing?yv12_to_bgri_c:yv12_to_bgr_c, 3);
		return 0;

	case XVID_CSP_BGRA:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_bgrai  :yv12_to_bgra,
			interlacing?yv12_to_bgrai_c:yv12_to_bgra_c, 4);
		return 0;

	case XVID_CSP_ABGR:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_abgri  :yv12_to_abgr,
			interlacing?yv12_to_abgri_c:yv12_to_abgr_c, 4);
		return 0;

	case XVID_CSP_RGBA:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_rgbai  :yv12_to_rgba,
			interlacing?yv12_to_rgbai_c:yv12_to_rgba_c, 4);
		return 0;

	case XVID_CSP_ARGB:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_argbi  :yv12_to_argb,
			interlacing?yv12_to_argbi_c:yv12_to_argb_c, 4);
		return 0;

	case XVID_CSP_YUY2:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_yuyvi  :yv12_to_yuyv,
			interlacing?yv12_to_yuyvi_c:yv12_to_yuyv_c, 2);
		return 0;

	case XVID_CSP_YVYU:		/* u,v swapped */
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->v, image->u,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_yuyvi  :yv12_to_yuyv,
			interlacing?yv12_to_yuyvi_c:yv12_to_yuyv_c, 2);
		return 0;

	case XVID_CSP_UYVY:
		safe_packed_conv(
			dst[0], dst_stride[0], image->y, image->u, image->v,
			edged_width, edged_width2, width, height, (csp & XVID_CSP_VFLIP),
			interlacing?yv12_to_uyvyi  :yv12_to_uyvy,
			interlacing?yv12_to_uyvyi_c:yv12_to_uyvy_c, 2);
		return 0;

	case XVID_CSP_I420: /* YCbCr == YUV == internal colorspace for MPEG */
		yv12_to_yv12(dst[0], dst[0] + dst_stride[0]*height, dst[0] + dst_stride[0]*height + (dst_stride[0]/2)*height2,
			dst_stride[0], dst_stride[0]/2,
			image->y, image->u, image->v, edged_width, edged_width2,
			width, height, (csp & XVID_CSP_VFLIP));
		return 0;

	case XVID_CSP_YV12:	/* YCrCb == YVU == U and V plane swapped */
		yv12_to_yv12(dst[0], dst[0] + dst_stride[0]*height, dst[0] + dst_stride[0]*height + (dst_stride[0]/2)*height2,
			dst_stride[0], dst_stride[0]/2,
			image->y, image->v, image->u, edged_width, edged_width2,
			width, height, (csp & XVID_CSP_VFLIP));
		return 0;

	case XVID_CSP_PLANAR:  /* YCbCr with arbitrary pointers and different strides for Y and UV */
		yv12_to_yv12(dst[0], dst[1], dst[2],
			dst_stride[0], dst_stride[1],	/* v: dst_stride[2] not yet supported */
			image->y, image->u, image->v, edged_width, edged_width2,
			width, height, (csp & XVID_CSP_VFLIP));
		return 0;

	case XVID_CSP_INTERNAL :
		dst[0] = image->y;
		dst[1] = image->u;
		dst[2] = image->v;
		dst_stride[0] = edged_width;
		dst_stride[1] = edged_width/2;
		dst_stride[2] = edged_width/2;
		return 0;

	case XVID_CSP_NULL:
	case XVID_CSP_SLICE:
		return 0;

	}

	return -1;
}



void
output_slice(IMAGE * cur, int stride, int width, xvid_image_t* out_frm, int mbx, int mby,int mbl) {
  uint8_t *dY,*dU,*dV,*sY,*sU,*sV;
  int stride2 = stride >> 1;
  int w = mbl << 4, w2,i;

  if(w > width)
    w = width;
  w2 = w >> 1;

  dY = (uint8_t*)out_frm->plane[0] + (mby << 4) * out_frm->stride[0] + (mbx << 4);
  dU = (uint8_t*)out_frm->plane[1] + (mby << 3) * out_frm->stride[1] + (mbx << 3);
  dV = (uint8_t*)out_frm->plane[2] + (mby << 3) * out_frm->stride[2] + (mbx << 3);
  sY = cur->y + (mby << 4) * stride + (mbx << 4);
  sU = cur->u + (mby << 3) * stride2 + (mbx << 3);
  sV = cur->v + (mby << 3) * stride2 + (mbx << 3);

  for(i = 0 ; i < 16 ; i++) {
    memcpy(dY,sY,w);
    dY += out_frm->stride[0];
    sY += stride;
  }
  for(i = 0 ; i < 8 ; i++) {
    memcpy(dU,sU,w2);
    dU += out_frm->stride[1];
    sU += stride2;
  }
  for(i = 0 ; i < 8 ; i++) {
    memcpy(dV,sV,w2);
    dV += out_frm->stride[2];
    sV += stride2;
  }
}


void
image_clear(IMAGE * img, int width, int height, int edged_width,
					int y, int u, int v)
{
	uint8_t * p;
	int i;

	p = img->y;
	for (i = 0; i < height; i++) {
		memset(p, y, width);
		p += edged_width;
	}

	p = img->u;
	for (i = 0; i < height/2; i++) {
		memset(p, u, width/2);
		p += edged_width/2;
	}

	p = img->v;
	for (i = 0; i < height/2; i++) {
		memset(p, v, width/2);
		p += edged_width/2;
	}
}
