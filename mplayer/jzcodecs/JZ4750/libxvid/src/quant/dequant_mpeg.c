/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - MPEG4 Quantization related header  -
 *
 *  Copyright(C) 2001-2003 Peter Ross <pross@xvid.org>
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
 * $Id: dequant_mpeg.c,v 1.3 2009/10/30 00:21:55 dyang Exp $
 *
 ****************************************************************************/

#include "../global.h"
#include "quant.h"
#include "quant_matrix.h"
#include "../jzsoc/jzmedia.h"
/*****************************************************************************
 * Global function pointers
 ****************************************************************************/

/* DeQuant */
quant_intraFuncPtr dequant_mpeg_intra;
quant_interFuncPtr dequant_mpeg_inter;

/*****************************************************************************
 * Local data
 ****************************************************************************/

/*****************************************************************************
 * Function definitions
 ****************************************************************************/

/* dequantize intra-block & clamp to [-2048,2047]
 *
 * data[i] = (coeff[i] * default_intra_matrix[i] * quant2) >> 4;
 */
#ifdef JZC_MXU_OPT
uint32_t
dequant_mpeg_intra_mxu(int16_t * data,
		                         uint8_t yuv_len,
					 const uint32_t quant,
					 const uint32_t dcscalar,
					 const uint16_t * mpeg_quant_matrices)
{
	const uint16_t *intra_matrix = mpeg_quant_matrices;
	int32_t i = 0;
	/* deal with data[0] then save to xr6  */
	
	S32I2M(xr3,-2048);
	S32I2M(xr4,2047);
    	S32I2M(xr5,quant);

	S32MUL(xr0,xr6,(int32_t)data[0],dcscalar);
	S32LUI(xr9,1,0);
	D16MUL_WW(xr0,xr6,xr9,xr6);
	S32MIN(xr6,xr6,xr4);
	S32MAX(xr6,xr6,xr3);

	yuv_len = ((yuv_len&~1)+3)>>1;
       	data-=2;
	intra_matrix-=2;
	

	for (i = 0; i < yuv_len; i++) {
	    S32LDI(xr1,data,4);
	    S32LDI(xr2,intra_matrix,4);

	    D16MUL_LW(xr13,xr9,xr1,xr14); // resave values of data[i] and data[i+1] 
	    D16CPS(xr1,xr1,xr1);         

	    /* abs(level) *( intra_matrix[i]*quant) >> 3   */
	    D16MUL_LW(xr7,xr5,xr2,xr8);
	    S32SFL(xr15,xr7,xr8,xr2,3);
	    D16MUL_WW(xr7,xr1,xr2,xr8);
	    D32SLR(xr7,xr7,xr8,xr8,3); 
	   
#if 0
	    /* -2048 < data[i+1] < 2047  */
	    S32CPS(xr7,xr7,xr13);
	    S32MAX(xr10,xr7,xr3);
	    S32MIN(xr10,xr10,xr4);

            /* -2048 < data[i] < 2047  */
	    S32CPS(xr8,xr8,xr14);
	    S32MAX(xr11,xr8,xr3);
	    S32MIN(xr11,xr11,xr4);
#else
	    /* -2048 < data[i+1] < 2047  */
	    S32AND(xr7,xr7,xr4);
	    S32CPS(xr10,xr7,xr13);	    

            /* -2048 < data[i] < 2047  */
	    S32AND(xr8,xr8,xr4);
	    S32CPS(xr11,xr8,xr14);

#endif 

            S32SFL(xr0,xr10,xr11,xr12,3);
	    S32STD(xr12,data,0);	    
        } 
 
	S16STD(xr6,data-(yuv_len*2-2),0,0);//xr6 to data[0]
	return(0);
}
#else
uint32_t
dequant_mpeg_intra_c(int16_t * data,
					 const int16_t * coeff,
					 const uint32_t quant,
					 const uint32_t dcscalar,
					 const uint16_t * mpeg_quant_matrices)
{
	const uint16_t *intra_matrix = get_intra_matrix(mpeg_quant_matrices);
	int i;

	data[0] = coeff[0] * dcscalar;
	if (data[0] < -2048) {
		data[0] = -2048;
	} else if (data[0] > 2047) {
		data[0] = 2047;
	}

	for (i = 1; i < 64; i++) {
		if (coeff[i] == 0) {
			data[i] = 0;
		} else if (coeff[i] < 0) {
			uint32_t level = -coeff[i];

			level = (level * intra_matrix[i] * quant) >> 3;
			data[i] = (level <= 2048 ? -(int16_t) level : -2048);
		} else {
			uint32_t level = coeff[i];

			level = (level * intra_matrix[i] * quant) >> 3;
			data[i] = (level <= 2047 ? level : 2047);
		}
	}

	return(0);
}
#endif

/* dequantize inter-block & clamp to [-2048,2047]
 * data = ((2 * coeff + SIGN(coeff)) * inter_matrix[i] * quant) / 16
 */

uint32_t
dequant_mpeg_inter_c(int16_t * data,
					 const int16_t * coeff,
					 const uint32_t quant,
					 const uint16_t * mpeg_quant_matrices)
{
	uint32_t sum = 0;
	const uint16_t *inter_matrix = get_inter_matrix(mpeg_quant_matrices);
	int i;

	for (i = 0; i < 64; i++) {
		if (coeff[i] == 0) {
			data[i] = 0;
		} else if (coeff[i] < 0) {
			int32_t level = -coeff[i];

			level = ((2 * level + 1) * inter_matrix[i] * quant) >> 4;
			data[i] = (level <= 2048 ? -level : -2048);
		} else {
			uint32_t level = coeff[i];

			level = ((2 * level + 1) * inter_matrix[i] * quant) >> 4;
			data[i] = (level <= 2047 ? level : 2047);
		}

		sum ^= data[i];
	}

	/*	mismatch control */
	if ((sum & 1) == 0) {
		data[63] ^= 1;
	}

	return(0);
}
