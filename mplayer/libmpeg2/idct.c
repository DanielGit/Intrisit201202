/*
 * idct.c
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Modified for use with MPlayer, see libmpeg-0.4.1.diff for the exact changes.
 * detailed changelog at http://svn.mplayerhq.hu/mplayer/trunk/
 * $Id: idct.c,v 1.5 2009/04/01 01:38:20 jyu Exp $
 */

#include "config.h"

#include <mplaylib.h>
#include <inttypes.h>

#include "mpeg2.h"
#include "attributes.h"
#include "mpeg2_internal.h"
#ifdef JZ4740_MXU_OPT
#include "jzmedia.h"
#endif
#define W1 2841 /* 2048 * sqrt (2) * cos (1 * pi / 16) */
#define W2 2676 /* 2048 * sqrt (2) * cos (2 * pi / 16) */
#define W3 2408 /* 2048 * sqrt (2) * cos (3 * pi / 16) */
#define W5 1609 /* 2048 * sqrt (2) * cos (5 * pi / 16) */
#define W6 1108 /* 2048 * sqrt (2) * cos (6 * pi / 16) */
#define W7 565  /* 2048 * sqrt (2) * cos (7 * pi / 16) */

/* idct main entry point  */
void (* mpeg2_idct_copy) (int16_t * block, uint8_t * dest, int stride);
void (* mpeg2_idct_add) (int last, int16_t * block,
			 uint8_t * dest, int stride);
extern int32_t idct_row_max,idct_row_max_intra;
/*
 * In legal streams, the IDCT output should be between -384 and +384.
 * In corrupted streams, it is possible to force the IDCT output to go
 * to +-3826 - this is the worst case for a column IDCT where the
 * column inputs are 16-bit values.
 */
uint8_t mpeg2_clip[3840 * 2 + 256];
#define CLIP(i) ((mpeg2_clip + 3840)[i])

#if 0
#define BUTTERFLY(t0,t1,W0,W1,d0,d1)	\
do {					\
    t0 = W0 * d0 + W1 * d1;		\
    t1 = W0 * d1 - W1 * d0;		\
} while (0)
#else
#define BUTTERFLY(t0,t1,W0,W1,d0,d1)	\
do {					\
    int tmp = W0 * (d0 + d1);		\
    t0 = tmp + (W1 - W0) * d1;		\
    t1 = tmp - (W1 + W0) * d0;		\
} while (0)
#endif
static inline void idct_row (int16_t * const block)
{
    int d0, d1, d2, d3;
    int a0, a1, a2, a3, b0, b1, b2, b3;
    int t0, t1, t2, t3;

    /* shortcut */
    if (likely (!(block[1] | ((int32_t *)block)[1] | ((int32_t *)block)[2] |
		  ((int32_t *)block)[3]))) {
	uint32_t tmp = (uint16_t) (block[0] >> 1);
	tmp |= tmp << 16;
	((int32_t *)block)[0] = tmp;
	((int32_t *)block)[1] = tmp;
	((int32_t *)block)[2] = tmp;
	((int32_t *)block)[3] = tmp;
	return;
    }

    d0 = (block[0] << 11) + 2048;
    d1 = block[1];
    d2 = block[2] << 11;
    d3 = block[3];
    t0 = d0 + d2;
    t1 = d0 - d2;
    BUTTERFLY (t2, t3, W6, W2, d3, d1);
    a0 = t0 + t2;
    a1 = t1 + t3;
    a2 = t1 - t3;
    a3 = t0 - t2;

    d0 = block[4];
    d1 = block[5];
    d2 = block[6];
    d3 = block[7];
    BUTTERFLY (t0, t1, W7, W1, d3, d0);
    BUTTERFLY (t2, t3, W3, W5, d1, d2);
    b0 = t0 + t2;
    b3 = t1 + t3;
    t0 -= t2;
    t1 -= t3;
    b1 = ((t0 + t1) >> 8) * 181;
    b2 = ((t0 - t1) >> 8) * 181;

    block[0] = (a0 + b0) >> 12;
    block[1] = (a1 + b1) >> 12;
    block[2] = (a2 + b2) >> 12;
    block[3] = (a3 + b3) >> 12;
    block[4] = (a3 - b3) >> 12;
    block[5] = (a2 - b2) >> 12;
    block[6] = (a1 - b1) >> 12;
    block[7] = (a0 - b0) >> 12;
}
static inline void idct_col (int16_t * const block)
{
    int d0, d1, d2, d3;
    int a0, a1, a2, a3, b0, b1, b2, b3;
    int t0, t1, t2, t3;

    d0 = (block[8*0] << 11) + 65536;
    d1 = block[8*1];
    d2 = block[8*2] << 11;
    d3 = block[8*3];
    t0 = d0 + d2;
    t1 = d0 - d2;
    BUTTERFLY (t2, t3, W6, W2, d3, d1);
    a0 = t0 + t2;
    a1 = t1 + t3;
    a2 = t1 - t3;
    a3 = t0 - t2;

    d0 = block[8*4];
    d1 = block[8*5];
    d2 = block[8*6];
    d3 = block[8*7];
    BUTTERFLY (t0, t1, W7, W1, d3, d0);
    BUTTERFLY (t2, t3, W3, W5, d1, d2);
    b0 = t0 + t2;
    b3 = t1 + t3;
    t0 -= t2;
    t1 -= t3;
    b1 = ((t0 + t1) >> 8) * 181;
    b2 = ((t0 - t1) >> 8) * 181;

    block[8*0] = (a0 + b0) >> 17;
    block[8*1] = (a1 + b1) >> 17;
    block[8*2] = (a2 + b2) >> 17;
    block[8*3] = (a3 + b3) >> 17;
    block[8*4] = (a3 - b3) >> 17;
    block[8*5] = (a2 - b2) >> 17;
    block[8*6] = (a1 - b1) >> 17;
    block[8*7] = (a0 - b0) >> 17;
}
#define  wxr5  0x5A827642
#define  wxr6  0x5A8230FC
#define  wxr7  0x7D876A6E
#define  wxr8  0x18F9471D
#define  wxr9  0x6A6E18F9
#define  wxr10  0x471D7D87

static int32_t whirl_idct[6] = {wxr5, wxr6, wxr7, wxr8, wxr9, wxr10};
#ifdef JZ4740_MXU_OPT
static void mpeg2_idct_copy_c (int16_t * block, uint8_t * dest,
			        const int stride)
{ 
  int i, tmp0, tmp1;
  int16_t  *blk;
  int32_t wf = (int32_t)whirl_idct;

  S32LDD(xr5, wf, 0x0);         // xr5(w7, w3)
  S32LDD(xr6, wf, 0x4);         // xr6(w9, w8)
  S32LDD(xr7, wf, 0x8);         // xr7(w11,w10)
  S32LDD(xr8, wf, 0xc);         // xr8(w13,w12)
  S32LDD(xr9, wf, 0x10);        // xr9(w6, w0)
  S32LDD(xr10,wf, 0x14);

  blk = block - 8;
  for (i = 0; i < idct_row_max_intra + 1; i++)       /* idct rows */
  {

      S32LDI(xr1, blk, 0x10);       //  xr1 (x4, x0)
      tmp0 = S32M2I(xr1);
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      // shortcut
      S32SFL(xr0, xr1, xr1, xr11, ptn3);
      D16MUL_HW(xr11, xr5, xr11, xr12);
      D16MACF_AA_WW(xr11, xr0, xr0, xr12);
      tmp0 >>= 16;

      S32OR(xr12,xr2,xr3);
      S32OR(xr12,xr12,xr4);
      tmp1 = (S32M2I(xr12));
      tmp0 |= tmp1;

      if (tmp0 == 0) {
        S32STD(xr11, blk, 0x0);
        S32STD(xr11, blk, 0x4);
        S32STD(xr11, blk, 0x8);
        S32STD(xr11, blk, 0xc);
        continue;
         }
      S32SFL(xr1,xr1,xr2,xr2, ptn3);  //xr1:s1, s3, xr2: s0, s2
      S32SFL(xr3,xr3,xr4,xr4, ptn3);  //xr3:s5, s7, xr4: s4, s6

      D16MUL_WW(xr11, xr2, xr5, xr12);//xr11: s0*c4, xr12: s2*c2
      D16MAC_AA_WW(xr11,xr4,xr6,xr12);//xr11: s0*c4+s4*c4, xr12: s2*c2+s6*c6

      D16MUL_WW(xr13, xr2, xr6, xr14);//xr13: s0*c4, xr14: s2*c6
      D16MAC_SS_WW(xr13,xr4,xr5,xr14);//xr13: s0*c4 - s4*c4, xr14: s2*c6-s6*c2

      D16MUL_HW(xr2, xr1, xr7, xr4);  //xr2: s1*c1, xr4: s1*c3 
      D16MAC_AS_LW(xr2,xr1,xr9,xr4);  //xr2: s1*c1+s3*c3, xr4: s1*c3-s3*c7
      D16MAC_AS_HW(xr2,xr3,xr10,xr4); //xr2: s1*c1+s3*c3+s5*c5, xr4: s1*c3-s3*c7-s5*c1
      D16MAC_AS_LW(xr2,xr3,xr8,xr4);  //xr2: s1*c1+s3*c3+s5*c5+s7*c7,
                                      //xr4: s1*c3-s3*c7-s5*c1-s7*c5
      D16MACF_AA_WW(xr2, xr0, xr0, xr4); //xr2: rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16
                                         //   : rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>16

      D16MACF_AA_WW(xr11, xr0, xr0, xr13);//xr11: rnd(s0*c4+s4*c4)>>16, rnd(s0*c4 - s4*c4)>>16
      D16MACF_AA_WW(xr12, xr0, xr0, xr14);//xr12: rnd(s2*c2+s6*c6)>>16, rnd(s2*c6-s6*c2)>>16

      D16MUL_HW(xr4, xr1, xr8, xr15);     //xr4: s1*c7, xr15:s1*c5
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);    //xr4: s1*c7-s3*c5, xr15: s1*c5-s3*c1
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);     //xr4: s1*c7-s3*c5+s5*c3, xr15: s1*c5-s3*c1+s5*c7
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);     //xr4: s1*c7-s3*c5+s5*c3-s7*c1
                                          //xr15: s1*c5-s3*c1+s5*c7+s7*c3
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);  //xr11: rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16
                                          //      rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16
                                          //xr12: rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16
                                          //      rnd(s0*c4-s4*c4)>>16-rnd(s2*c6-s6*c2)>>16
      D16MACF_AA_WW(xr15, xr0, xr0, xr4); //xr15: rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>16
                                          //    : rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>16

      Q16ADD_AS_WW(xr11, xr11, xr2, xr2);
              //xr11: rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16 + rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16
              //    : rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16 + rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>16
              //xr2: rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16 - rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16
              //   : rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16 - rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>16

      Q16ADD_AS_XW(xr12, xr12, xr15, xr15);
              //xr12: rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16+rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>16
              //    : rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16+rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>16
              //xr15: rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16-rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>16
              //    : rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16-rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>16

      S32SFL(xr11,xr11,xr12,xr12, ptn3);
              //xr11: rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16 + rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16
              //    : rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16+rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>16
              //xr12: rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16 + rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>16
              //    : rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16+rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>16
      S32SFL(xr12,xr12,xr11,xr11, ptn3);
              //xr12: rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16 + rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>16
              //    : rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16 + rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16
              //xr11: rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16+rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>16
              //    : rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16+rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>16

      S32STD(xr12, blk, 0x0);
      S32STD(xr11, blk, 0x4);
      S32STD(xr15, blk, 0x8);
      S32STD(xr2, blk, 0xc);
    }

  blk  = block - 2;
  for (i = 0; i < 4; i++)               /* idct columns */
    {
      S32LDI(xr1, blk, 0x4);   //xr1: ss0, s0
      S32LDD(xr3, blk, 0x20);  //xr3: ss2, s2
      S32I2M(xr5,wxr5);        //xr5: c4 , c2
      S32LDD(xr11, blk, 0x40); //xr11: ss4, s4
      S32LDD(xr13, blk, 0x60); //xr13: ss6, s6

      D16MUL_HW(xr15, xr5, xr1, xr9);    //xr15: ss0*c4, xr9: s0*c4
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);   //xr15: ss0*c4+ss4*c4, xr9: s0*c4+s4*c4
      D16MACF_AA_WW(xr15, xr0, xr0, xr9);//xr15: rnd(ss0*c4+ss4*c4)>>16
                                         //   :  rnd(s0*c4+s4*c4)>>16
      D16MUL_LW(xr10, xr5, xr3, xr9);    //xr10: ss2*c2, xr9: s2*c2
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);   //xr10: ss2*c2+ss6*c6, xr9: s2*c2+s6*c6
      D16MACF_AA_WW(xr10, xr0, xr0, xr9);//xr10: rnd(ss2*c2+ss6*c6)>>16
                                         //    : rnd(s2*c2 + s6*c6)>>16
      S32LDD(xr2, blk, 0x10);            //xr2: ss1, s1
      S32LDD(xr4, blk, 0x30);            //xr4: ss3, s3
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);  //xr15: rnd(ss0*c4+ss4*c4)>>16+rnd(ss2*c2+ss6*c6)>>16
                                         //    :rnd(s0*c4+s4*c4)>>16 + rnd(s2*c2 + s6*c6)>>16
                                         //xr9: rnd(ss0*c4+ss4*c4)>>16 - rnd(ss2*c2+ss6*c6)>>16
                                         //   : rnd(s0*c4+s4*c4)>>16 - rnd(s2*c2 + s6*c6)>>16
      D16MUL_HW(xr10, xr5, xr1, xr1);    //xr10: ss0*c4, xr1: s0*c4
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);   //xr10: ss0*c4-ss4*c4, xr1: s0*c4 - s4*c4
      D16MACF_AA_WW(xr10, xr0, xr0, xr1);//xr10: rnd(ss0*c4-ss4*c4)>>16
                                         //    : rnd(s0*c4 - s4*c4)>>16
      D16MUL_LW(xr11, xr6, xr3, xr1);    //xr11: ss2*c6, xr1: s2*c6
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);   //xr11: ss2*c6-ss6*c2, xr1: s2*c6-s6*c2
      D16MACF_AA_WW(xr11, xr0, xr0, xr1);//xr11: rnd(ss2*c6-ss6*c2)>>16
                                         //    : rnd(s2*c6 - s6*c2)>>16
      S32LDD(xr12, blk, 0x50);           //xr12: ss5, s5
      S32LDD(xr14, blk, 0x70);           //xr14: ss7, s7
      Q16ADD_AS_WW(xr10,xr10,xr11,xr1);  //xr10: rnd(ss0*c4-ss4*c4)>>16)+rnd(ss2*c6-ss6*c2)>>16
                                         //    : rnd(s0*c4 - s4*c4)>>16 +rnd(s2*c6 - s6*c2)>>16
                                         //xr1 : rnd(ss0*c4-ss4*c4)>>16-rnd(ss2*c6-ss6*c2)>>16
                                         //    : rnd(s0*c4 - s4*c4)>>16-rnd(s2*c6 - s6*c2)>>16

      D16MUL_HW(xr11, xr7, xr2, xr13);   //xr11: ss1*c1, xr13: s1*c1
      D16MAC_AA_LW(xr11,xr7,xr4,xr13);   //xr11: ss1*c1+ss3*c3, xr13: s1*c1+s3*c3
      D16MAC_AA_LW(xr11,xr8,xr12,xr13);  //xr11: ss1*c1+ss3*c3+ss5*c5
                                         //xr13: s1*c1+s3*c3+s5*c5
      D16MAC_AA_HW(xr11,xr8,xr14,xr13);  //xr11: ss1*c1+ss3*c3+ss5*c5+ss7*c7
                                         //xr13: s1*c1+s3*c3+s5*c5+s7*c7
      D16MACF_AA_WW(xr11, xr0, xr0, xr13);//xr11: rnd(ss1*c1+ss3*c3+ss5*c5+ss7*c7)>>16
                                          //    : rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16

      D16MUL_LW(xr3, xr7, xr2, xr13);    //xr3: ss1*c3, xr13: s1*c3
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);    //xr3: ss1*c3-ss3*c7, xr13: s1*c3-s3*c7
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1
                                         //xr13: s1*c3-s3*c7-s5*c1
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1-ss7*c5
                                         //xr13: s1*c3-s3*c7-s7*c5
      D16MACF_AA_WW(xr3, xr0, xr0, xr13);//xr3: rnd(ss1*c3-ss3*c7-ss5*c1-ss7*c5)>>16
                                         //   : rnd(s1*c3-s3*c7-s7*c5)>>16
      D16MUL_LW(xr5, xr8, xr2, xr13);    //xr5: ss1*c5, xr13:s1*c5
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);    //xr5: ss1*c5-ss3*c1, xr13:s1*c5-s3*c1
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7
                                         //   : s1*c5 - s3*c1+ s5*c7
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7+ss7*c1
                                         //   : s1*c5 - s3*c1+ s5*c7+ s7*c1
      D16MACF_AA_WW(xr5, xr0, xr0, xr13);//xr5: rnd(ss1*c5-ss3*c1+ss5*c7+ss7*c1)>>16
                                         //   : rnd(s1*c5 - s3*c1+ s5*c7+ s7*c1)>>16

      D16MUL_HW(xr2, xr8, xr2, xr13);    //xr2: ss1*c7, xr13: s1*c7
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);    //xr2: ss1*c7-ss3*c5, xr13: s1*c7-s3*c5
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1
                                         //xr13: s1*c7-s3*c5+s5*c1
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1-ss7*c3
                                         //xr13: s1*c7-s3*c5+s5*c1-s7*c3
      D16MACF_AA_WW(xr2, xr0, xr0, xr13);//xr2: rnd(ss1*c7-ss3*c5+ss5*c1-ss7*c3)>>16
                                         //   : rnd(s1*c7-s3*c5+s5*c1-s7*c3)>>16
      Q16ADD_AS_WW(xr15,xr15,xr11,xr11); //xr15:rnd(ss0*c4+ss4*c4)>>16+rnd(ss2*c2+ss6*c6)>>16+
                                         //     rnd(ss1*c1+ss3*c3+ss5*c5+ss7*c7)>>16
                                         //     rnd(s0*c4+s4*c4)>>16 + rnd(s2*c2 + s6*c6)>>16+
                                         //     rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16

                                         //xr11:rnd(ss0*c4+ss4*c4)>>16+rnd(ss2*c2+ss6*c6)>>16-
                                         //     rnd(ss1*c1+ss3*c3+ss5*c5+ss7*c7)>>16
                                         //     rnd(s0*c4+s4*c4)>>16 + rnd(s2*c2 + s6*c6)>>16-
                                         //     rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>16
      Q16ADD_AS_WW(xr10,xr10,xr3,xr3);   //xr10:rnd(ss0*c4-ss4*c4)>>16)+rnd(ss2*c6-ss6*c2)>>16+
                                         //     rnd(ss1*c3-ss3*c7-ss5*c1-ss7*c5)>>16
                                         //     rnd(s0*c4 - s4*c4)>>16 +rnd(s2*c6 - s6*c2)>>16+
                                         //     rnd(s1*c3-s3*c7-s7*c5)>>16
                                         //xr10:rnd(ss0*c4-ss4*c4)>>16)+rnd(ss2*c6-ss6*c2)>>16-
                                         //     rnd(ss1*c3-ss3*c7-ss5*c1-ss7*c5)>>16
                                         //     rnd(s0*c4 - s4*c4)>>16 +rnd(s2*c6 - s6*c2)>>16-
                                         //     rnd(s1*c3-s3*c7-s7*c5)>>16
      Q16ADD_AS_WW(xr1,xr1,xr5,xr5);     //xr1: rnd(ss0*c4-ss4*c4)>>16-rnd(ss2*c6-ss6*c2)>>16+
                                         //     rnd(ss1*c5-ss3*c1+ss5*c7+ss7*c1)>>16
                                         //     rnd(s0*c4 - s4*c4)>>16 +rnd(s2*c6 - s6*c2)>>16+
                                         //     rnd(s1*c5 - s3*c1+ s5*c7+ s7*c1)>>16
                                         //xr1: rnd(ss0*c4-ss4*c4)>>16-rnd(ss2*c6-ss6*c2)>>16-
                                         //     rnd(ss1*c5-ss3*c1+ss5*c7+ss7*c1)>>16
                                         //     rnd(s0*c4 - s4*c4)>>16 +rnd(s2*c6 - s6*c2)>>16-
                                         //     rnd(s1*c5 - s3*c1+ s5*c7+ s7*c1)>>16
      Q16ADD_AS_WW(xr9,xr9,xr2,xr2);     //xr9: rnd(ss0*c4+ss4*c4)>>16 - rnd(ss2*c2+ss6*c6)>>16+
                                         //     rnd(ss1*c7-ss3*c5+ss5*c1-ss7*c3)>>16
                                         //     rnd(s0*c4+s4*c4)>>16 - rnd(s2*c2 + s6*c6)>>16+
                                         //     rnd(s1*c7-s3*c5+s5*c1-s7*c3)>>16
                                         //xr9: rnd(ss0*c4+ss4*c4)>>16 - rnd(ss2*c2+ss6*c6)>>16-
                                         //     rnd(ss1*c7-ss3*c5+ss5*c1-ss7*c3)>>16
                                         //     rnd(s0*c4+s4*c4)>>16 - rnd(s2*c2 + s6*c6)>>16-
                                         //     rnd(s1*c7-s3*c5+s5*c1-s7*c3)>>16

      S32STD(xr15, blk, 0x00);
      S32STD(xr10, blk, 0x10);
      S32STD(xr1, blk, 0x20);
      S32STD(xr9, blk, 0x30);
      S32STD(xr2, blk, 0x40);
      S32STD(xr5, blk, 0x50);
      S32STD(xr3, blk, 0x60);
      S32STD(xr11, blk, 0x70);
    }
  blk = block -8;
  dest -= stride;
  for (i=0; i< 8; i++) {
    S32LDI(xr1, blk, 0x10);
    S32LDD(xr2, blk, 0x4);
    S32LDD(xr3, blk, 0x8);
    S32LDD(xr4, blk, 0xc);
    S32STD(xr0, blk, 0x0);
    S32STD(xr0, blk, 0x4); 
    S32STD(xr0, blk, 0x8);
    S32STD(xr0, blk, 0xc);
    Q16SAT(xr5, xr2, xr1);
    Q16SAT(xr6, xr4, xr3);
    S32SDIV(xr5, dest, stride, 0x0);
    S32STD(xr6, dest, 0x4);
  }
}
#else
static void mpeg2_idct_copy_c (int16_t * block, uint8_t * dest,
                               const int stride)
{
    int i;
    for (i = 0; i < 8; i++)
        idct_row (block + 8 * i);
    for (i = 0; i < 8; i++)
        idct_col (block + i);
    do {
        dest[0] = CLIP (block[0]);
        dest[1] = CLIP (block[1]);
        dest[2] = CLIP (block[2]);
        dest[3] = CLIP (block[3]);
        dest[4] = CLIP (block[4]);
        dest[5] = CLIP (block[5]);
        dest[6] = CLIP (block[6]);
        dest[7] = CLIP (block[7]);

        ((int32_t *)block)[0] = 0;      ((int32_t *)block)[1] = 0;
        ((int32_t *)block)[2] = 0;      ((int32_t *)block)[3] = 0;

        dest += stride;
        block += 8;
    } while (--i);
}
#endif

#ifdef JZ4740_MXU_OPT

static void mpeg2_idct_add_c (const int last, int16_t * block,
			      uint8_t * dest, const int stride)
{
  int i;
  int16_t  *blk;
  int32_t wf = (int32_t)whirl_idct;

  S32LDD(xr5, wf, 0x0);         // xr5(w7, w3)
  S32LDD(xr6, wf, 0x4);         // xr6(w9, w8)
  S32LDD(xr7, wf, 0x8);         // xr7(w11,w10)
  S32LDD(xr8, wf, 0xc);         // xr8(w13,w12)
  S32LDD(xr9, wf, 0x10);        // xr9(w6, w0)
  S32LDD(xr10,wf, 0x14);

  blk = block - 8;
  do { /* idct rows */
      S32LDI(xr1, blk, 0x10);        //  xr1 (x4, x0)
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
  } while (idct_row_max--);
     blk  = block - 2;
  for (i = 0; i < 4; i++)               /* idct columns */
    {
      S32LDI(xr1, blk, 0x4);
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
  blk = block - 8;
  dest -= stride;
  for (i=0; i< 8; i++) {
    S32LDIV(xr5, dest, stride, 0x0);
    S32LDI(xr1, blk, 0x10);
    S32LDD(xr2, blk, 0x4);
    Q8ACCE_AA(xr2, xr5, xr0, xr1);

    S32LDD(xr6, dest, 0x4);
    S32LDD(xr3, blk, 0x8);
    S32LDD(xr4, blk, 0xc);
    Q8ACCE_AA(xr4, xr6, xr0, xr3);
    S32STD(xr0, blk, 0x0);
    S32STD(xr0, blk, 0x4);
    S32STD(xr0, blk, 0x8);
    S32STD(xr0, blk, 0xc);
    Q16SAT(xr5, xr2, xr1);
    S32STD(xr5, dest, 0x0);
    Q16SAT(xr6, xr4, xr3);
    S32STD(xr6, dest, 0x4);
  }

}

#else
static void mpeg2_idct_add_c (const int last, int16_t * block,
			      uint8_t * dest, const int stride)
{
    int i;
   
    if (last != 129 || (block[0] & (7 << 4)) == (4 << 4)) {
	for (i = 0; i < 8; i++)
	    idct_row (block + 8 * i);
	for (i = 0; i < 8; i++)
	    idct_col (block + i);
	do {
	    dest[0] = CLIP (block[0] + dest[0]);
	    dest[1] = CLIP (block[1] + dest[1]);
	    dest[2] = CLIP (block[2] + dest[2]);
	    dest[3] = CLIP (block[3] + dest[3]);
	    dest[4] = CLIP (block[4] + dest[4]);
	    dest[5] = CLIP (block[5] + dest[5]);
	    dest[6] = CLIP (block[6] + dest[6]);
	    dest[7] = CLIP (block[7] + dest[7]);

	    ((int32_t *)block)[0] = 0;	((int32_t *)block)[1] = 0;
	    ((int32_t *)block)[2] = 0;	((int32_t *)block)[3] = 0;

	    dest += stride;
	    block += 8;
	} while (--i);
    } else {
	int DC;

	DC = (block[0] + 64) >> 7;
	block[0] = block[63] = 0;
	i = 8;
	do {
	    dest[0] = CLIP (DC + dest[0]);
	    dest[1] = CLIP (DC + dest[1]);
	    dest[2] = CLIP (DC + dest[2]);
	    dest[3] = CLIP (DC + dest[3]);
	    dest[4] = CLIP (DC + dest[4]);
	    dest[5] = CLIP (DC + dest[5]);
	    dest[6] = CLIP (DC + dest[6]);
	    dest[7] = CLIP (DC + dest[7]);
	    dest += stride;
	} while (--i);
    }
}
#endif
void mpeg2_idct_init (uint32_t accel)
{
	
#ifdef HAVE_MMX2
    if (accel & MPEG2_ACCEL_X86_MMXEXT) {
	mpeg2_idct_copy = mpeg2_idct_copy_mmxext;
	mpeg2_idct_add = mpeg2_idct_add_mmxext;
	mpeg2_idct_mmx_init ();
    } else
#endif
#ifdef HAVE_MMX
    if (accel & MPEG2_ACCEL_X86_MMX) {
	mpeg2_idct_copy = mpeg2_idct_copy_mmx;
	mpeg2_idct_add = mpeg2_idct_add_mmx;
	mpeg2_idct_mmx_init ();
    } else
#endif
#if defined(ARCH_PPC) && defined(HAVE_ALTIVEC)
    if (accel & MPEG2_ACCEL_PPC_ALTIVEC) {
	mpeg2_idct_copy = mpeg2_idct_copy_altivec;
	mpeg2_idct_add = mpeg2_idct_add_altivec;
	mpeg2_idct_altivec_init ();
    } else
#endif
#ifdef ARCH_ALPHA
#ifdef CAN_COMPILE_ALPHA_MVI
    if (accel & MPEG2_ACCEL_ALPHA_MVI) {
	mpeg2_idct_copy = mpeg2_idct_copy_mvi;
	mpeg2_idct_add = mpeg2_idct_add_mvi;
	mpeg2_idct_alpha_init ();
    } else
#endif
    if (accel & MPEG2_ACCEL_ALPHA) {
	int i;

	mpeg2_idct_copy = mpeg2_idct_copy_alpha;
	mpeg2_idct_add = mpeg2_idct_add_alpha;
	mpeg2_idct_alpha_init ();
	for (i = -3840; i < 3840 + 256; i++)
	    CLIP(i) = (i < 0) ? 0 : ((i > 255) ? 255 : i);
    } else
#endif
    {
	extern uint8_t mpeg2_scan_norm[64];
	extern uint8_t mpeg2_scan_alt[64];
	int i, j;

	mpeg2_idct_copy = mpeg2_idct_copy_c;
	mpeg2_idct_add = mpeg2_idct_add_c;
	static int dInit = 0;
	if(dInit != 0)
		return ;
	dInit = 1;
#ifdef JZ4740_MXU_OPT
#else
	for (i = -3840; i < 3840 + 256; i++)
	    CLIP(i) = (i < 0) ? 0 : ((i > 255) ? 255 : i);
	for (i = 0; i < 64; i++) {
	    j = mpeg2_scan_norm[i];
	    mpeg2_scan_norm[i] = ((j & 0x36) >> 1) | ((j & 0x09) << 2);
	    j = mpeg2_scan_alt[i];
	    mpeg2_scan_alt[i] = ((j & 0x36) >> 1) | ((j & 0x09) << 2);
	}
#endif
    }
}
