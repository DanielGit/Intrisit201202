/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Inverse DCT  -
 *
 *  These routines are from Independent JPEG Group's free JPEG software
 *  Copyright (C) 1991-1998, Thomas G. Lane (see the file README.IJG)
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
 * $Id: idct.c,v 1.3 2009/10/27 01:17:58 gysun Exp $
 *
 ****************************************************************************/

/* Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved. */

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis.  The MPEG Software Simulation Group disclaims
 * any and all warranties, whether express, implied, or statuary, including any
 * implied warranties or merchantability or of fitness for a particular
 * purpose.  In no event shall the copyright-holder be liable for any
 * incidental, punitive, or consequential damages of any kind whatsoever
 * arising from the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs and user's
 * customers, employees, agents, transferees, successors, and assigns.
 *
 * The MPEG Software Simulation Group does not represent or warrant that the
 * programs furnished hereunder are free of infringement of any third-party
 * patents.
 *
 * Commercial implementations of MPEG-1 and MPEG-2 video, including shareware,
 * are subject to royalty fees to patent holders.  Many of these patents are
 * general enough such that they are unavoidable regardless of implementation
 * design.
 *
 * MPEG2AVI
 * --------
 * v0.16B33 renamed the initialization function to init_idct_int32()
 * v0.16B32 removed the unused idct_row() and idct_col() functions
 * v0.16B3  changed var declarations to static, to enforce data align
 * v0.16B22  idct_FAST() renamed to idct_int32()
 *        also merged idct_FAST() into a single function, to help VC++
 *        optimize it.
 *
 * v0.14  changed int to long, to avoid confusion when compiling on x86
 *        platform ( in VC++ "int" -> 32bits )
 */

/**********************************************************/
/* inverse two dimensional DCT, Chen-Wang algorithm       */
/* (cf. IEEE ASSP-32, pp. 803-816, Aug. 1984)             */
/* 32-bit integer arithmetic (8 bit coefficients)         */
/* 11 mults, 29 adds per DCT                              */
/*                                      sE, 18.8.91       */
/**********************************************************/
/* coefficients extended to 12 bit for IEEE1180-1990      */
/* compliance                           sE,  2.1.94       */
/**********************************************************/

/* this code assumes >> to be a two's-complement arithmetic */
/* right shift: (-2)>>1 == -1 , (-3)>>1 == -2               */


/* right shift: (-2)>>1 == -1 , (-3)>>1 == -2               */
#include "../global.h"
#include "idct.h"
#include "../jzsoc/jzmedia.h"
idctFuncPtr idct;

#ifndef JZC_MXU_OPT
#define W1 2841					/* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676					/* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408					/* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609					/* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108					/* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565					/* 2048*sqrt(2)*cos(7*pi/16) */

/* private data
 * Initialized by idct_int32_init so it's mostly RO data,
 * doesn't hurt thread safety */
static short iclip[1024];		/* clipping table */
static short *iclp;

/* private prototypes */

/* row (horizontal) IDCT
 *
 *           7                       pi         1
 * dst[k] = sum c[l] * src[l] * cos( -- * ( k + - ) * l )
 *          l=0                      8          2
 *
 * where: c[0]    = 128
 *        c[1..7] = 128*sqrt(2)
 */

/* function pointer */
//idctFuncPtr idct;

/* two dimensional inverse discrete cosine transform */
void
idct_int32(short *const block)
{

	/*
	 * idct_int32_init() must be called before the first call to this
	 * function!
	 */
	short *blk;
	long i;
	long X0, X1, X2, X3, X4, X5, X6, X7, X8;


	for (i = 0; i < 8; i++)		/* idct rows */
	{
		blk = block + (i << 3);
		if (!
			((X1 = blk[4] << 11) | (X2 = blk[6]) | (X3 = blk[2]) | (X4 =
																	blk[1]) |
			 (X5 = blk[7]) | (X6 = blk[5]) | (X7 = blk[3]))) {
			blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] =
				blk[7] = blk[0] << 3;
			continue;
		}

		X0 = (blk[0] << 11) + 128;	/* for proper rounding in the fourth stage  */

		/* first stage  */
		X8 = W7 * (X4 + X5);
		X4 = X8 + (W1 - W7) * X4;
		X5 = X8 - (W1 + W7) * X5;
		X8 = W3 * (X6 + X7);
		X6 = X8 - (W3 - W5) * X6;
		X7 = X8 - (W3 + W5) * X7;

		/* second stage  */
		X8 = X0 + X1;
		X0 -= X1;
		X1 = W6 * (X3 + X2);
		X2 = X1 - (W2 + W6) * X2;
		X3 = X1 + (W2 - W6) * X3;
		X1 = X4 + X6;
		X4 -= X6;
		X6 = X5 + X7;
		X5 -= X7;

		/* third stage  */
		X7 = X8 + X3;
		X8 -= X3;
		X3 = X0 + X2;
		X0 -= X2;
		X2 = (181 * (X4 + X5) + 128) >> 8;
		X4 = (181 * (X4 - X5) + 128) >> 8;

		/* fourth stage  */

		blk[0] = (short) ((X7 + X1) >> 8);
		blk[1] = (short) ((X3 + X2) >> 8);
		blk[2] = (short) ((X0 + X4) >> 8);
		blk[3] = (short) ((X8 + X6) >> 8);
		blk[4] = (short) ((X8 - X6) >> 8);
		blk[5] = (short) ((X0 - X4) >> 8);
		blk[6] = (short) ((X3 - X2) >> 8);
		blk[7] = (short) ((X7 - X1) >> 8);

	}							/* end for ( i = 0; i < 8; ++i ) IDCT-rows */



	for (i = 0; i < 8; i++)		/* idct columns */
	{
		blk = block + i;
		/* shortcut  */
		if (!
			((X1 = (blk[8 * 4] << 8)) | (X2 = blk[8 * 6]) | (X3 =
															 blk[8 *
																 2]) | (X4 =
																		blk[8 *
																			1])
			 | (X5 = blk[8 * 7]) | (X6 = blk[8 * 5]) | (X7 = blk[8 * 3]))) {
			blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] =
				blk[8 * 5] = blk[8 * 6] = blk[8 * 7] =
				iclp[(blk[8 * 0] + 32) >> 6];
			continue;
		}

		X0 = (blk[8 * 0] << 8) + 8192;

		/* first stage  */
		X8 = W7 * (X4 + X5) + 4;
		X4 = (X8 + (W1 - W7) * X4) >> 3;
		X5 = (X8 - (W1 + W7) * X5) >> 3;
		X8 = W3 * (X6 + X7) + 4;
		X6 = (X8 - (W3 - W5) * X6) >> 3;
		X7 = (X8 - (W3 + W5) * X7) >> 3;

		/* second stage  */
		X8 = X0 + X1;
		X0 -= X1;
		X1 = W6 * (X3 + X2) + 4;
		X2 = (X1 - (W2 + W6) * X2) >> 3;
		X3 = (X1 + (W2 - W6) * X3) >> 3;
		X1 = X4 + X6;
		X4 -= X6;
		X6 = X5 + X7;
		X5 -= X7;

		/* third stage  */
		X7 = X8 + X3;
		X8 -= X3;
		X3 = X0 + X2;
		X0 -= X2;
		X2 = (181 * (X4 + X5) + 128) >> 8;
		X4 = (181 * (X4 - X5) + 128) >> 8;

		/* fourth stage  */
		blk[8 * 0] = iclp[(X7 + X1) >> 14];
		blk[8 * 1] = iclp[(X3 + X2) >> 14];
		blk[8 * 2] = iclp[(X0 + X4) >> 14];
		blk[8 * 3] = iclp[(X8 + X6) >> 14];
		blk[8 * 4] = iclp[(X8 - X6) >> 14];
		blk[8 * 5] = iclp[(X0 - X4) >> 14];
		blk[8 * 6] = iclp[(X3 - X2) >> 14];
		blk[8 * 7] = iclp[(X7 - X1) >> 14];
	}

}								/* end function idct_int32(block) */


void
idct_int32_init(void)
{
	int i;

	iclp = iclip + 512;
	for (i = -512; i < 512; i++)
		iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
}

#else

#define  wxr5  0x5A827642
#define  wxr6  0x5A8230FC
#define  wxr7  0x7D876A6E
#define  wxr8  0x18F9471D
#define  wxr9  0x6A6E18F9
#define  wxr10  0x471D7D87

void
jz_idct_c(short *const block,uint8_t yuv_len,uint8_t * const recondst)
{
  //int xr1, xr2, xr3, xr4, xr5, xr6, xr7, xr8;
  //int xr9, xr10, xr11, xr12, xr13, xr14, xr15;
  short *blk;
  unsigned int i;// mid0, mid1, tmp0, tmp1;

  S32I2M(xr5,wxr5) ;         // xr5(w7, w3)
  S32I2M(xr6,wxr6) ;         // xr6(w9, w8)
  S32I2M(xr7,wxr7) ;         // xr7(w11,w10)
  S32I2M(xr8,wxr8) ;         // xr8(w13,w12)
  S32I2M(xr9,wxr9) ;         // xr9(w6, w0)  
  S32I2M(xr10,wxr10);       


  blk = block - 8;
  for (i = 0; i < yuv_len; i++)	/* idct rows */
    {
      int hi_b, lo_b, hi_c, lo_c;
       blk += 8;
      S32LDD(xr1, blk, 0x0);        //  xr1 (x4, x0)
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      
      S32SFL(xr1,xr1,xr2,xr2, ptn3);  
      
      S32SFL(xr3,xr3,xr4,xr4, ptn3);  
      
      D16MUL_WW(xr11, xr2, xr5, xr12);         
      D16MAC_AA_WW(xr11,xr4,xr6,xr12);        
      D16MUL_WW(xr13, xr2, xr6, xr14);         
      D16MAC_SS_WW(xr13,xr4,xr5,xr14);        
      D16MUL_HW(xr2, xr1, xr7, xr4);         
      D16MAC_AS_LW(xr2,xr1,xr9,xr4);        
      D16MAC_AS_HW(xr2,xr3,xr10,xr4);        
      D16MAC_AS_LW(xr2,xr3,xr8,xr4);        

      D16MACF_AA_WW(xr2, xr0, xr0, xr4); 
      D16MACF_AA_WW(xr11, xr0, xr0, xr13);       
      D16MACF_AA_WW(xr12, xr0, xr0, xr14); 

      D16MUL_HW(xr4, xr1, xr8, xr15);         
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);        
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);        
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);        
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);

      D16MACF_AA_WW(xr15, xr0, xr0, xr4); 
      Q16ADD_AS_WW(xr11, xr11, xr2, xr2);    
      Q16ADD_AS_XW(xr12, xr12, xr15, xr15);       


       S32SFL(xr11,xr11,xr12,xr12, ptn3);
      
      S32SFL(xr12,xr12,xr11,xr11, ptn3);
      
      S32STD(xr12, blk, 0x0);
      S32STD(xr11, blk, 0x4); 
      S32STD(xr15, blk, 0x8); 
      S32STD(xr2, blk, 0xc);       
    }

  blk = block - 2;
  for (i = 0; i < 4; i++)		/* idct columns */
    {
      int hi_b, lo_b, hi_c, lo_c;
       blk += 2;
      S32LDD(xr1, blk, 0x00);
      //S32LDI(xr1, blk, 0x04);
      S32LDD(xr3, blk, 0x20);
      S32I2M(xr5,wxr5);
      S32LDD(xr11, blk, 0x40);
      S32LDD(xr13, blk, 0x60);
       
      D16MUL_HW(xr15, xr5, xr1, xr9);
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);        

      D16MACF_AA_WW(xr15, xr0, xr0, xr9); 

      D16MUL_LW(xr10, xr5, xr3, xr9);         
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);        

      D16MACF_AA_WW(xr10, xr0, xr0, xr9); 

      S32LDD(xr2, blk, 0x10);
      S32LDD(xr4, blk, 0x30);
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);
      D16MUL_HW(xr10, xr5, xr1, xr1);         
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);        

      D16MACF_AA_WW(xr10, xr0, xr0, xr1); 

      D16MUL_LW(xr11, xr6, xr3, xr1);         
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);        

      D16MACF_AA_WW(xr11, xr0, xr0, xr1); 

      S32LDD(xr12, blk, 0x50);
      S32LDD(xr14, blk, 0x70);
      Q16ADD_AS_WW(xr10,xr10,xr11,xr1);
      D16MUL_HW(xr11, xr7, xr2, xr13);         
      D16MAC_AA_LW(xr11,xr7,xr4,xr13);        
      D16MAC_AA_LW(xr11,xr8,xr12,xr13);        
      D16MAC_AA_HW(xr11,xr8,xr14,xr13);        

      D16MACF_AA_WW(xr11, xr0, xr0, xr13); 

      D16MUL_LW(xr3, xr7, xr2, xr13);         
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);        
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);        
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);        
      
      D16MACF_AA_WW(xr3, xr0, xr0, xr13); 

      D16MUL_LW(xr5, xr8, xr2, xr13);         
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);        
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);        
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);        
      
      D16MACF_AA_WW(xr5, xr0, xr0, xr13); 

      D16MUL_HW(xr2, xr8, xr2, xr13);         
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);        
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);        
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);        
      
      D16MACF_AA_WW(xr2, xr0, xr0, xr13); 

      Q16ADD_AS_WW(xr15,xr15,xr11,xr11);    
      Q16ADD_AS_WW(xr10,xr10,xr3,xr3);    
      Q16ADD_AS_WW(xr1,xr1,xr5,xr5);          
      Q16ADD_AS_WW(xr9,xr9,xr2,xr2);      

      S32STD(xr15, blk, 0x00);
      S32STD(xr10, blk, 0x10); 
      S32STD(xr1, blk, 0x20); 
      S32STD(xr9, blk, 0x30); 
      S32STD(xr2, blk, 0x40);      
      S32STD(xr5, blk, 0x50); 
      S32STD(xr3, blk, 0x60); 
      S32STD(xr11, blk, 0x70);
    }
}

#define RECON_BUF_STRIDE 16
void
jz_idct_intra(short *const block,uint8_t yuv_len, uint8_t * const recondst)
{
  //int xr1, xr2, xr3, xr4, xr5, xr6, xr7, xr8;
  //int xr9, xr10, xr11, xr12, xr13, xr14, xr15;
  short *blk;
  unsigned int i;// mid0, mid1, tmp0, tmp1;
uint8_t *dst_mid = recondst;

  S32I2M(xr5,wxr5) ;         // xr5(w7, w3)
  S32I2M(xr6,wxr6) ;         // xr6(w9, w8)
  S32I2M(xr7,wxr7) ;         // xr7(w11,w10)
  S32I2M(xr8,wxr8) ;         // xr8(w13,w12)
  S32I2M(xr9,wxr9) ;         // xr9(w6, w0)  
  S32I2M(xr10,wxr10);       


  blk = block - 8;
  for (i = 0; i < yuv_len; i++)	/* idct rows */
    {
      int hi_b, lo_b, hi_c, lo_c;
       blk += 8;
      S32LDD(xr1, blk, 0x0);        //  xr1 (x4, x0)
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      
      S32SFL(xr1,xr1,xr2,xr2, ptn3);  
      
      S32SFL(xr3,xr3,xr4,xr4, ptn3);  
      
      D16MUL_WW(xr11, xr2, xr5, xr12);         
      D16MAC_AA_WW(xr11,xr4,xr6,xr12);        
      D16MUL_WW(xr13, xr2, xr6, xr14);         
      D16MAC_SS_WW(xr13,xr4,xr5,xr14);        
      D16MUL_HW(xr2, xr1, xr7, xr4);         
      D16MAC_AS_LW(xr2,xr1,xr9,xr4);        
      D16MAC_AS_HW(xr2,xr3,xr10,xr4);        
      D16MAC_AS_LW(xr2,xr3,xr8,xr4);        

      D16MACF_AA_WW(xr2, xr0, xr0, xr4); 
      D16MACF_AA_WW(xr11, xr0, xr0, xr13);       
      D16MACF_AA_WW(xr12, xr0, xr0, xr14); 

      D16MUL_HW(xr4, xr1, xr8, xr15);         
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);        
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);        
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);        
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);

      D16MACF_AA_WW(xr15, xr0, xr0, xr4); 
      Q16ADD_AS_WW(xr11, xr11, xr2, xr2);    
      Q16ADD_AS_XW(xr12, xr12, xr15, xr15);       


       S32SFL(xr11,xr11,xr12,xr12, ptn3);
      
      S32SFL(xr12,xr12,xr11,xr11, ptn3);
      
      S32STD(xr12, blk, 0x0);
      S32STD(xr11, blk, 0x4); 
      S32STD(xr15, blk, 0x8); 
      S32STD(xr2, blk, 0xc);       
    }

  blk = block - 2;
  for (i = 0; i < 4; i++)		/* idct columns */
    {
      int hi_b, lo_b, hi_c, lo_c;
       blk += 2;
      S32LDD(xr1, blk, 0x00);
      //S32LDI(xr1, blk, 0x04);
      S32LDD(xr3, blk, 0x20);
      S32I2M(xr5,wxr5);
      S32LDD(xr11, blk, 0x40);
      S32LDD(xr13, blk, 0x60);
       
      D16MUL_HW(xr15, xr5, xr1, xr9);
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);        

      D16MACF_AA_WW(xr15, xr0, xr0, xr9); 

      D16MUL_LW(xr10, xr5, xr3, xr9);         
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);        

      D16MACF_AA_WW(xr10, xr0, xr0, xr9); 

      S32LDD(xr2, blk, 0x10);
      S32LDD(xr4, blk, 0x30);
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);
      D16MUL_HW(xr10, xr5, xr1, xr1);         
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);        

      D16MACF_AA_WW(xr10, xr0, xr0, xr1); 

      D16MUL_LW(xr11, xr6, xr3, xr1);         
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);        

      D16MACF_AA_WW(xr11, xr0, xr0, xr1); 

      S32LDD(xr12, blk, 0x50);
      S32LDD(xr14, blk, 0x70);
      Q16ADD_AS_WW(xr10,xr10,xr11,xr1);
      D16MUL_HW(xr11, xr7, xr2, xr13);         
      D16MAC_AA_LW(xr11,xr7,xr4,xr13);        
      D16MAC_AA_LW(xr11,xr8,xr12,xr13);        
      D16MAC_AA_HW(xr11,xr8,xr14,xr13);        

      D16MACF_AA_WW(xr11, xr0, xr0, xr13); 

      D16MUL_LW(xr3, xr7, xr2, xr13);         
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);        
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);        
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);        
      
      D16MACF_AA_WW(xr3, xr0, xr0, xr13); 

      D16MUL_LW(xr5, xr8, xr2, xr13);         
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);        
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);        
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);        
      
      D16MACF_AA_WW(xr5, xr0, xr0, xr13); 

      D16MUL_HW(xr2, xr8, xr2, xr13);         
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);        
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);        
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);        
      
      D16MACF_AA_WW(xr2, xr0, xr0, xr13); 

      Q16ADD_AS_WW(xr15,xr15,xr11,xr11);    
      Q16ADD_AS_WW(xr10,xr10,xr3,xr3);    
      Q16ADD_AS_WW(xr1,xr1,xr5,xr5);          
      Q16ADD_AS_WW(xr9,xr9,xr2,xr2);      
      /*
      S32STD(xr15, blk, 0x00);
      S32STD(xr10, blk, 0x10); 
      S32STD(xr1, blk, 0x20); 
      S32STD(xr9, blk, 0x30); 
      S32STD(xr2, blk, 0x40);      
      S32STD(xr5, blk, 0x50); 
      S32STD(xr3, blk, 0x60); 
      S32STD(xr11, blk, 0x70);
      */
      // saturate
      Q16SAT(xr15, xr15, xr10);
      Q16SAT(xr1, xr1,  xr9);
      Q16SAT(xr2, xr2,  xr5);
      Q16SAT(xr3, xr3,  xr11);
  
      // store it
      dst_mid = recondst + i * 2;
      S16STD(xr15, dst_mid, 0,                1);
      S16SDI(xr15, dst_mid, RECON_BUF_STRIDE, 0);
  
      S16SDI(xr1, dst_mid, RECON_BUF_STRIDE,  1);
      S16SDI(xr1, dst_mid, RECON_BUF_STRIDE,  0);
      
      S16SDI(xr2, dst_mid, RECON_BUF_STRIDE,  1);
      S16SDI(xr2, dst_mid, RECON_BUF_STRIDE,  0);
      
      S16SDI(xr3, dst_mid, RECON_BUF_STRIDE,  1);
      S16STD(xr3, dst_mid, RECON_BUF_STRIDE,  0);
    }
}

#endif
