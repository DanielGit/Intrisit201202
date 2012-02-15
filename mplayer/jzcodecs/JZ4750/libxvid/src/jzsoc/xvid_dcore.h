/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *
 *  Copyright(C) 2002      MinChen <chenm001@163.com>
 *               2002-2004 Peter Ross <pross@xvid.org>
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
 ****************************************************************************
 *
 * xvid_dcore.h
 *   -- It defines MB level bitstream decode/aux reconstruction function
 *
 * $Id: xvid_dcore.h,v 1.1.1.1 2009/10/13 06:37:38 gysun Exp $
 *
 ****************************************************************************/
#ifndef _XVID_DCORE_H_
#define _XVID_DCORE_H_

#include "portab.h"
#include "global.h"
#include "xvid_arch.h"

/* decode an intra macroblock */
static void
decoder_mbintra_pbs(DECODER * dec,
		    MACROBLOCK * pMB,
		    const uint32_t x_pos,
		    const uint32_t y_pos,
		    const uint32_t acpred_flag,
		    const uint32_t cbp,
		    Bitstream * bs,
		    const uint32_t quant,
		    const uint32_t intra_dc_threshold,
		    const unsigned int bound);


/* decode an inter macroblock */
static void
decoder_mbinter_pbs(DECODER * dec,
		    const MACROBLOCK * pMB,
		    const uint32_t x_pos,
		    const uint32_t y_pos,
		    const uint32_t cbp,
		    Bitstream * bs,
		    const uint32_t rounding,
		    const int ref,
		    const int bvop);

/* decode an inter macroblock in field mode */
static void
decoder_mbinter_field_pbs(DECODER * dec,
			  const MACROBLOCK * pMB,
			  const uint32_t x_pos,
			  const uint32_t y_pos,
			  const uint32_t cbp,
			  Bitstream * bs,
			  const uint32_t rounding,
			  const int ref,
			  const int bvop);


/* GMC */
static void
decoder_mbgmc_pbs(DECODER * dec,
		  MACROBLOCK * const pMB,
		  const uint32_t x_pos,
		  const uint32_t y_pos,
		  const uint32_t fcode,
		  const uint32_t cbp,
		  Bitstream * bs,
		  const uint32_t rounding);


/* decode an B-frame direct & interpolate macroblock */
static void
decoder_bf_interpolate_mbinter_pbs(DECODER * dec,
				   IMAGE forward,
				   IMAGE backward,
				   MACROBLOCK * pMB,
				   const uint32_t x_pos,
				   const uint32_t y_pos,
				   Bitstream * bs,
				   const int direct);

////////////////////////////////////////////////////////
/* decode an intra macroblock --AUX CPU*/
static void
decoder_mbintra_2pass(FIFO_DECODER const *dec,
		      FIFO_MB const *pMB
		      );


/* decode an inter macroblock post bitstream decode --AUX CPU*/
static void
decoder_mb_decode_2pass(FIFO_DECODER const *dec,
			const uint32_t cbp,
			FIFO_MB const *pMB);

/* decode an inter macroblock --AUX CPU*/
static void
decoder_mbinter_2pass(FIFO_DECODER const *dec,
		      FIFO_MB const *pMB);

/* decode an inter macroblock in field mode --AUX CPU*/
static void
decoder_mbinter_field_2pass(FIFO_DECODER const * dec,
			    FIFO_MB const *pMB);


/* decode an B-frame direct & interpolate macroblock --AUX CPU*/
static void
decoder_bf_interpolate_mbinter_2pass(FIFO_DECODER const *dec,
				     FIFO_MB const *pMB,
				     const uint32_t x_pos,
				     const uint32_t y_pos,
				     const int direct);

/*JZ Dcore aux CPU global arguments init*/
static inline void 
aux_frame_arg_init(DECODER * dec);

static void 
aux_frame_arg_set(DECODER * dec);

static inline void 
aux_frame_arg_delete();

#endif /*_XVID_DCORE_H_*/
