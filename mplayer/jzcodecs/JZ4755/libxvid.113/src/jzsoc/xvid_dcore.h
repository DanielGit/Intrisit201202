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
 * $Id: xvid_dcore.h,v 1.7 2009/06/02 07:59:31 yzhai Exp $
 *
 ****************************************************************************/
#ifndef _XVID_DCORE_H_
#define _XVID_DCORE_H_

#include "xvid_arch.h"
/* --- macroblock modes --- */

#define MODE_INTER		0
#define MODE_INTER_Q	1
#define MODE_INTER4V	2
#define	MODE_INTRA		3
#define MODE_INTRA_Q	4
#define MODE_NOT_CODED	16
#define MODE_NOT_CODED_GMC	17

/* --- bframe specific --- */

#define MODE_DIRECT			0
#define MODE_INTERPOLATE	1
#define MODE_BACKWARD		2
#define MODE_FORWARD		3
#define MODE_DIRECT_NONE_MV	4
#define MODE_DIRECT_NO4V	5


/*
 * vop coding types
 * intra, prediction, backward, sprite, not_coded
 */
#define I_VOP	0
#define P_VOP	1
#define B_VOP	2
#define S_VOP	3
#define N_VOP	4

#define CLIP(X,AMIN,AMAX)   (((X)<(AMIN)) ? (AMIN) : ((X)>(AMAX)) ? (AMAX) : (X))

#if 0
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
void
decoder_bf_interpolate_mbinter_pbs(DECODER * dec,
				   IMAGE forward,
				   IMAGE backward,
				   MACROBLOCK * pMB,
				   const uint32_t x_pos,
				   const uint32_t y_pos,
				   Bitstream * bs,
				   const int direct);
#endif
////////////////////////////////////////////////////////
/* decode an intra macroblock --AUX CPU*/
void
decoder_mbintra_2pass(FIFO_DECODER const *dec,
		      const uint16_t x_pos,
		      const uint16_t y_pos,
		      FIFO_MB const *pMB
		      );

/* decode an inter macroblock post bitstream decode --AUX CPU*/
void
decoder_mb_decode_2pass(FIFO_DECODER const *dec,
			const uint32_t cbp,
const uint16_t x_pos,
			    const uint16_t y_pos,
			FIFO_MB const *pMB);

/* decode an inter macroblock --AUX CPU*/
void
decoder_mbinter_2pass(FIFO_DECODER const *dec,
		     const uint16_t x_pos,
			    const uint16_t y_pos,
		      FIFO_MB const *pMB);

/* decode an inter macroblock in field mode --AUX CPU*/
void
decoder_mbinter_field_2pass(FIFO_DECODER const * dec,
			    const uint16_t x_pos,
			    const uint16_t y_pos,
			    FIFO_MB const *pMB);


/* decode an B-frame direct & interpolate macroblock --AUX CPU*/
void
decoder_bf_interpolate_mbinter_2pass(FIFO_DECODER const *dec,
				     const uint16_t x_pos,
				     const uint16_t y_pos,
				     FIFO_MB const *pMB);

/*JZ Dcore aux CPU global arguments init
static inline void 
aux_frame_arg_init(DECODER * dec);

static void 
aux_frame_arg_set(DECODER * dec);

static inline void 
aux_frame_arg_delete();*/

typedef struct{
  uint32_t CurrReconY;
  uint32_t CurrReconU;
}R4_XCHG;

typedef struct{
  uint8_t *ReconYBi;
  uint8_t *ReconUBi;
  int16_t *IDCTBuf;
  XVID_MC_DesNode *MCDes;
}R2_XCHG;

#endif /*_XVID_DCORE_H_*/
