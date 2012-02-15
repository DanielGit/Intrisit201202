/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - MPEG4 Quantization H263 implementation -
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
 * $Id: dequant_h263.c,v 1.6 2009/06/10 02:43:09 gysun Exp $
 *
 ****************************************************************************/

#include "../global.h"
#include "quant.h"

/*****************************************************************************
 * Global function pointers
 ****************************************************************************/


/* DeQuant */
quant_intraFuncPtr dequant_h263_intra;
quant_interFuncPtr dequant_h263_inter;

/*****************************************************************************
 * Local data
 ****************************************************************************/


/*****************************************************************************
 * Function definitions
 ****************************************************************************/

/*	quantize intra-block
 */

/*	dequantize intra-block & clamp to [-2048,2047]
 */
#ifdef JZC_MXU_OPT
uint32_t
dequant_h263_intra_mxu(int16_t * data,					
					 const uint32_t quant,
					 const uint32_t dcscalar,
					 const uint16_t * mpeg_quant_matrices)
{     
	uint32_t i = 0; 
	
	S32LUI(xr9,1,0);
	S32I2M(xr1,quant);
	
	D32SLL(xr5,xr1,xr0,xr0,1);// quant_m_2

	/* quant_add  */
	S32AND(xr15,xr1,xr9);
	S32MOVN(xr2,xr15,xr1);
	D32ADD_SS(xr1,xr1,xr9,xr3);
	S32MOVZ(xr2,xr15,xr1);

	S32I2M(xr3,-2048);
	S32I2M(xr4,2047);

	/* part1 */
	//S32MUL(xr4,xr6,*data,dcscalar);
	S32MUL(xr0,xr6,(int32_t)data[0],dcscalar);
	D16MUL_WW(xr0,xr6,xr9,xr6);
	S32MIN(xr6,xr6,xr4);
	S32MAX(xr6,xr6,xr3);  

	/* part2 */
	data-=2;    
	for (i = 0; i < 32; i++) {
	    S32LDI(xr1,data,4);
      
	    D16MUL_LW(xr13,xr9,xr1,xr14);// resave sign of data[i] and data[i+1] 	    
	    D16CPS(xr1,xr1,xr1); 

	    /*  quant_m_2 * acLevel + quant_add */
	    D16MUL_LW(xr7,xr5,xr1,xr8);
            D32ADD_AA(xr7,xr7,xr2,xr0);
	    D32ADD_AA(xr8,xr8,xr2,xr0);
	   
	    /* -2048 < data[i+1] <2047  */
            S32CPS(xr7,xr7,xr13);
	    S32MAX(xr10,xr7,xr3);
	    S32MIN(xr10,xr10,xr4);
	    S32MOVZ(xr10,xr13,xr13);
	    
	    /* -2048 < data[i] <2047  */
	    S32CPS(xr8,xr8,xr14);
	    S32MAX(xr11,xr8,xr3);
	    S32MIN(xr11,xr11,xr4);
	    S32MOVZ(xr11,xr14,xr14);
	   
            S32SFL(xr0,xr10,xr11,xr12,3);
	   
	       S32STD(xr12,data,0);
        }  
	S16STD(xr6,data,-62*2,0);// data[0]

	return(0);
}

#else


uint32_t
dequant_h263_intra_c(int16_t * data,
					 const int16_t * coeff,
					 const uint32_t quant,
					 const uint32_t dcscalar,
					 const uint16_t * mpeg_quant_matrices)
{	const int32_t quant_m_2 = quant << 1;
	const int32_t quant_add = (quant & 1 ? quant : quant - 1);
	int i;

	data[0] = coeff[0] * dcscalar;
	if (data[0] < -2048) {
		data[0] = -2048;
	} else if (data[0] > 2047) {
		data[0] = 2047;
	}

	for (i = 1; i < 64; i++) {
		int32_t acLevel = coeff[i];

		if (acLevel == 0) {
			data[i] = 0;
		} else if (acLevel < 0) {
			acLevel = quant_m_2 * -acLevel + quant_add;
			data[i] = (acLevel <= 2048 ? -acLevel : -2048);
		} else {
			acLevel = quant_m_2 * acLevel + quant_add;
			data[i] = (acLevel <= 2047 ? acLevel : 2047);
		}
	}
	return(0);

}
#endif //

/* dequantize inter-block & clamp to [-2048,2047]
 */

uint32_t
dequant_h263_inter_c(int16_t * data,
					 const int16_t * coeff,
					 const uint32_t quant,
					 const uint16_t * mpeg_quant_matrices)
{
	const uint16_t quant_m_2 = quant << 1;
	const uint16_t quant_add = (quant & 1 ? quant : quant - 1);
	int i;

	for (i = 0; i < 64; i++) {
		int16_t acLevel = coeff[i];

		if (acLevel == 0) {
			data[i] = 0;
		} else if (acLevel < 0) {
			acLevel = acLevel * quant_m_2 - quant_add;
			data[i] = (acLevel >= -2048 ? acLevel : -2048);
		} else {
			acLevel = acLevel * quant_m_2 + quant_add;
			data[i] = (acLevel <= 2047 ? acLevel : 2047);
		}
	}

	return(0);
}
