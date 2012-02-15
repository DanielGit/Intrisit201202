/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Native API implementation  -
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
 * $Id: xvid_dec.c,v 1.7 2009/06/06 08:05:36 yzhai Exp $
 *
 ****************************************************************************/
#ifdef _UCOS_
#include <mplaylib.h>
#include "mplaylib.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

#include "xvid.h"
#include "decoder.h"
#include "encoder.h"
#include "bitstream/cbp.h"
#include "dct/idct.h"
#include "dct/fdct.h"
#include "image/colorspace.h"
#include "image/interpolate8x8.h"
#include "utils/mem_transfer.h"
#include "utils/mbfunctions.h"
#include "quant/quant.h"
#include "motion/motion.h"
#include "motion/sad.h"
#include "utils/emms.h"
#include "utils/timer.h"
#include "bitstream/mbcoding.h"
#include "image/qpel.h"
#include "image/postprocessing.h"
#if defined(_DEBUG)
unsigned int xvid_debug = 0; /* xvid debug mask */
#endif

#if (defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64)) && defined(_MSC_VER)
#	include <windows.h>
#elif defined(ARCH_IS_IA32) || defined(ARCH_IS_X86_64) || defined(ARCH_IS_PPC)
#	include <signal.h>
#	include <setjmp.h>

	static jmp_buf mark;

	static void
	sigill_handler(int signal)
	{
	   longjmp(mark, 1);
	}
#endif


/*
 * Calls the funcptr, and returns whether SIGILL (illegal instruction) was
 * signalled
 *
 * Return values:
 *  -1 : could not determine
 *   0 : SIGILL was *not* signalled
 *   1 : SIGILL was signalled
 */

/* detect cpu flags  */
static unsigned int
detect_cpu_flags(void)
{
	/* enable native assembly optimizations by default */
	unsigned int cpu_flags = XVID_CPU_ASM;
	return cpu_flags;
}


/*****************************************************************************
 * XviD Init Entry point
 *
 * Well this function initialize all internal function pointers according
 * to the CPU features forced by the library client or autodetected (depending
 * on the XVID_CPU_FORCE flag). It also initializes vlc coding tables and all
 * image colorspace transformation tables.
 *
 * Returned value : XVID_ERR_OK
 *                  + API_VERSION in the input XVID_INIT_PARAM structure
 *                  + core build  "   "    "       "               "
 *
 ****************************************************************************/


static
int xvid_gbl_init(xvid_gbl_init_t * init)
{
	unsigned int cpu_flags;

	if (XVID_VERSION_MAJOR(init->version) != 1) /* v1.x.x */
		return XVID_ERR_VERSION;

	cpu_flags = (init->cpu_flags & XVID_CPU_FORCE) ? init->cpu_flags : detect_cpu_flags();

	
#ifdef JZC_SYS
	if(init_mbcoding_data())
	  return XVID_ERR_MEMORY;
#endif

	/* Initialize the function pointers */
#ifdef JZC_MXU_OPT
#else
	idct_int32_init();
#endif
	init_vlc_tables();

	/* Fixed Point Forward/Inverse DCT transformations */
#ifdef JZC_MXU_OPT 
      	idct = jz_idct_c;
    	
#else
	idct = idct_int32;
#endif
	emms = emms_c;

	/* Qpel stuff */
	xvid_QP_Funcs = &xvid_QP_Funcs_C;
	xvid_QP_Add_Funcs = &xvid_QP_Add_Funcs_C;
	xvid_Init_QP();

	/* DQuantization functions */
#ifdef JZC_MXU_OPT
	dequant_h263_intra = dequant_h263_intra_mxu;
#else
	dequant_h263_intra = dequant_h263_intra_c;
#endif//
	dequant_h263_inter = dequant_h263_inter_c;
#ifdef JZC_MXU_OPT
       	dequant_mpeg_intra = dequant_mpeg_intra_mxu;

#else
	dequant_mpeg_intra = dequant_mpeg_intra_c;
#endif
	dequant_mpeg_inter = dequant_mpeg_inter_c;

	/* Block transfer related functions */
	transfer_8to16copy = transfer_8to16copy_c;
	transfer_16to8copy = transfer_16to8copy_c;
	transfer_8to16sub  = transfer_8to16sub_c;
	transfer_8to16subro  = transfer_8to16subro_c;
	transfer_8to16sub2 = transfer_8to16sub2_c;
	transfer_8to16sub2ro = transfer_8to16sub2ro_c;
	transfer_16to8add  = transfer_16to8add_c;
	transfer8x8_copy   = transfer8x8_copy_c;
	transfer8x4_copy   = transfer8x4_copy_c;

	/* Interlacing functions */
	//	MBFieldTest = MBFieldTest_c; ???

	/* Image interpolation related functions */
	interpolate8x8_halfpel_h  = interpolate8x8_halfpel_h_c;
	interpolate8x8_halfpel_v  = interpolate8x8_halfpel_v_c;
	interpolate8x8_halfpel_hv = interpolate8x8_halfpel_hv_c;

	interpolate8x4_halfpel_h  = interpolate8x4_halfpel_h_c;
	interpolate8x4_halfpel_v  = interpolate8x4_halfpel_v_c;
	interpolate8x4_halfpel_hv = interpolate8x4_halfpel_hv_c;

	interpolate8x8_halfpel_add = interpolate8x8_halfpel_add_c;
	interpolate8x8_halfpel_h_add = interpolate8x8_halfpel_h_add_c;
	interpolate8x8_halfpel_v_add = interpolate8x8_halfpel_v_add_c;
	interpolate8x8_halfpel_hv_add = interpolate8x8_halfpel_hv_add_c;

	interpolate16x16_lowpass_h = interpolate16x16_lowpass_h_c;
	interpolate16x16_lowpass_v = interpolate16x16_lowpass_v_c;
	interpolate16x16_lowpass_hv = interpolate16x16_lowpass_hv_c;

	interpolate8x8_lowpass_h = interpolate8x8_lowpass_h_c;
	interpolate8x8_lowpass_v = interpolate8x8_lowpass_v_c;
	interpolate8x8_lowpass_hv = interpolate8x8_lowpass_hv_c;

	interpolate8x8_6tap_lowpass_h = interpolate8x8_6tap_lowpass_h_c;
	interpolate8x8_6tap_lowpass_v = interpolate8x8_6tap_lowpass_v_c;

	interpolate8x8_avg2 = interpolate8x8_avg2_c;
	interpolate8x8_avg4 = interpolate8x8_avg4_c;

	/* postprocessing */
	image_brightness = image_brightness_c;

	/* Initialize internal colorspace transformation tables */
	colorspace_init();

	/* All colorspace transformation functions User Format->YV12 */
	yv12_to_yv12    = yv12_to_yv12_c;
	rgb555_to_yv12  = rgb555_to_yv12_c;
	rgb565_to_yv12  = rgb565_to_yv12_c;
	bgr_to_yv12     = bgr_to_yv12_c;
	bgra_to_yv12    = bgra_to_yv12_c;
	abgr_to_yv12    = abgr_to_yv12_c;
	rgba_to_yv12    = rgba_to_yv12_c;
	argb_to_yv12    = argb_to_yv12_c;
	yuyv_to_yv12    = yuyv_to_yv12_c;
	uyvy_to_yv12    = uyvy_to_yv12_c;

	rgb555i_to_yv12 = rgb555i_to_yv12_c;
	rgb565i_to_yv12 = rgb565i_to_yv12_c;
	bgri_to_yv12    = bgri_to_yv12_c;
	bgrai_to_yv12   = bgrai_to_yv12_c;
	abgri_to_yv12   = abgri_to_yv12_c;
	rgbai_to_yv12   = rgbai_to_yv12_c;
	argbi_to_yv12   = argbi_to_yv12_c;
	yuyvi_to_yv12   = yuyvi_to_yv12_c;
	uyvyi_to_yv12   = uyvyi_to_yv12_c;

	/* All colorspace transformation functions YV12->User format */
	yv12_to_rgb555  = yv12_to_rgb555_c;
	yv12_to_rgb565  = yv12_to_rgb565_c;
	yv12_to_bgr     = yv12_to_bgr_c;
	yv12_to_bgra    = yv12_to_bgra_c;
	yv12_to_abgr    = yv12_to_abgr_c;
	yv12_to_rgba    = yv12_to_rgba_c;
	yv12_to_argb    = yv12_to_argb_c;
	yv12_to_yuyv    = yv12_to_yuyv_c;
	yv12_to_uyvy    = yv12_to_uyvy_c;

	yv12_to_rgb555i = yv12_to_rgb555i_c;
	yv12_to_rgb565i = yv12_to_rgb565i_c;
	yv12_to_bgri    = yv12_to_bgri_c;
	yv12_to_bgrai   = yv12_to_bgrai_c;
	yv12_to_abgri   = yv12_to_abgri_c;
	yv12_to_rgbai   = yv12_to_rgbai_c;
	yv12_to_argbi   = yv12_to_argbi_c;
	yv12_to_yuyvi   = yv12_to_yuyvi_c;
	yv12_to_uyvyi   = yv12_to_uyvyi_c;

	/* Functions used in motion estimation algorithms */
	calc_cbp   = calc_cbp_c;


#if defined(_DEBUG)
    xvid_debug = init->debug;
#endif

    return(0);
}


static int
xvid_gbl_info(xvid_gbl_info_t * info)
{
	if (XVID_VERSION_MAJOR(info->version) != 1) /* v1.x.x */
		return XVID_ERR_VERSION;

	info->actual_version = XVID_VERSION;
	info->build = "xvid-1.1.3";
	info->cpu_flags = detect_cpu_flags();

#if defined(_SMP) && defined(WIN32)
	info->num_threads = pthread_num_processors_np();;
#else
	info->num_threads = 0;
#endif

	return 0;
}


static int
xvid_gbl_convert(xvid_gbl_convert_t* convert)
{
	int width;
	int height;
	int width2;
	int height2;
	IMAGE img;

	if (XVID_VERSION_MAJOR(convert->version) != 1)   /* v1.x.x */
	      return XVID_ERR_VERSION;

	width = convert->width;
	height = convert->height;
	width2 = convert->width/2;
	height2 = convert->height/2;

	switch (convert->input.csp & ~XVID_CSP_VFLIP)
	{
		case XVID_CSP_YV12 :
			img.y = convert->input.plane[0];
			img.v = (uint8_t*)convert->input.plane[0] + convert->input.stride[0]*height;
			img.u = (uint8_t*)convert->input.plane[0] + convert->input.stride[0]*height + (convert->input.stride[0]/2)*height2;
			image_output(&img, width, height, width,
						(uint8_t**)convert->output.plane, convert->output.stride,
						convert->output.csp, convert->interlacing);
			break;

		default :
			return XVID_ERR_FORMAT;
	}

	return 0;
}

/*****************************************************************************
 * XviD Global Entry point
 *
 * Well this function initialize all internal function pointers according
 * to the CPU features forced by the library client or autodetected (depending
 * on the XVID_CPU_FORCE flag). It also initializes vlc coding tables and all
 * image colorspace transformation tables.
 *
 ****************************************************************************/


int
xvid_global(void *handle,
		  int opt,
		  void *param1,
		  void *param2)
{
	switch(opt)
	{
		case XVID_GBL_INIT :
			return xvid_gbl_init((xvid_gbl_init_t*)param1);

        case XVID_GBL_INFO :
            return xvid_gbl_info((xvid_gbl_info_t*)param1);

		case XVID_GBL_CONVERT :
			return xvid_gbl_convert((xvid_gbl_convert_t*)param1);

		default :
			return XVID_ERR_FAIL;
	}
}

/*****************************************************************************
 * XviD Native decoder entry point
 *
 * This function is just a wrapper to all the option cases.
 *
 * Returned values : XVID_ERR_FAIL when opt is invalid
 *                   else returns the wrapped function result
 *
 ****************************************************************************/

int
xvid_decore(void *handle,
			int opt,
			void *param1,
			void *param2)
{
	switch (opt) {
	case XVID_DEC_CREATE:
		return decoder_create((xvid_dec_create_t *) param1);

	case XVID_DEC_DESTROY:
		return decoder_destroy((DECODER *) handle);

	case XVID_DEC_DECODE:
		return decoder_decode((DECODER *) handle, (xvid_dec_frame_t *) param1, (xvid_dec_stats_t*) param2);

	default:
		return XVID_ERR_FAIL;
	}
}
