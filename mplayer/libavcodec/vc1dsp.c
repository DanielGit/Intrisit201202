/*
 * VC-1 and WMV3 decoder - DSP functions
 * Copyright (c) 2006 Konstantin Shishkov
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
* @file vc1dsp.c
 * VC-1 and WMV3 decoder
 *
 */
#define JZ4740_MXU_OPT
#ifdef JZ4740_MXU_OPT
#include "jzmedia.h"
#endif
#include "dsputil.h"
#undef printf
/** Apply overlap transform to horizontal edge
*/
#define W1 0x4 //4
#define W2 0x6 //6
#define W3 0x9 //9
#define W4 0xC //12
#define W5 0xF //15
#define W6 0x10 //16
#define W7 0x11 //17
#define W8 0x16 //22
#define W9 0xA  //10
static void vc1_v_overlap_c(uint8_t* src, int stride)
{
    int i;
    int a, b, c, d;
    int d1, d2;
    int rnd = 1;
    for(i = 0; i < 8; i++) {
        a = src[-2*stride];
        b = src[-stride];
        c = src[0];
        d = src[stride];
        d1 = (a - d + 3 + rnd) >> 3;
        d2 = (a - d + b - c + 4 - rnd) >> 3;

        src[-2*stride] = a - d1;
        src[-stride] = av_clip_uint8 (b - d2);
        src[0] = av_clip_uint8 (c + d2);
        src[stride] = d + d1;
        src++;
        rnd = !rnd;
    }
}

/** Apply overlap transform to vertical edge
*/
static void vc1_h_overlap_c(uint8_t* src, int stride)
{
    int i;
    int a, b, c, d;
    int d1, d2;
    int rnd = 1;
    for(i = 0; i < 8; i++) {
        a = src[-2];
        b = src[-1];
        c = src[0];
        d = src[1];
        d1 = (a - d + 3 + rnd) >> 3;
        d2 = (a - d + b - c + 4 - rnd) >> 3;

        src[-2] = a - d1;
        src[-1] = av_clip_uint8 (b - d2);
        src[0]  = av_clip_uint8 (c + d2);
        src[1] = d + d1;
        src += stride;
        rnd = !rnd;
    }
}


/** Do inverse transform on 8x8 block
*/
#ifdef JZ4740_MXU_OPT

static void vc1_inv_trans_8x8_c(DCTELEM block[64])
{
   int i;
   DCTELEM *src, *dst;
   src = block;
   dst = block; 
   S32I2M(xr15,W4<<16|W4);  //xr15:12|12
   S32I2M(xr14,W6<<16|W2);  //xr14:16|6
   S32I2M(xr13,W6<<16|W5);  //xr13:16|15
   S32I2M(xr12,W3<<16|W1);  //xr12:9 |4
   for(i=0;i<8;i++)
   {
     S32LDD(xr1,src,0x0); //xr1:src[1] src[0]
     S32LDD(xr2,src,0x4); //xr2:src[3] src[2]
     S32LDD(xr3,src,0x8); //xr3:src[5] src[4]
     S32LDD(xr4,src,0xc); //xr4:src[7] src[6]
     D16MUL_LW(xr5,xr1,xr15,xr6); //xr5:12*src[0] xr6:12*src[0]
     D16MAC_AS_LW(xr5,xr3,xr15,xr6); //xr5:t1 12*src[0]+12*src[4] xr6:t2 12*src[0]-12*src[4]
     D16MUL_LW(xr7,xr2,xr14,xr8);      //xr7:16*src[2] xr8:6*src[2]
     D16MAC_SA_LW(xr8,xr4,xr14,xr7);    //xr8:t4 6*src[2]-16*src[6] xr7:t3 16*src[2]+6*src[6]
      
     D32ADD_AS(xr5,xr5,xr7,xr7);     //xr5:t5 t1+t3  xr7:t8 t1-t3
     D32ADD_AS(xr6,xr6,xr8,xr8);     //xr6:t6 t2+t4  xr8:t7 t2-t4
     S32I2M(xr9,4);
     D32ACC_AS(xr5,xr9,xr0,xr7);
     D32ACC_AS(xr6,xr9,xr0,xr8);
     D16MUL_HW(xr9,xr1,xr13,xr10);   //xr9:16*src[1] xr10:15*src[1]
     D16MUL_HW(xr11,xr1,xr12,xr1);   //xr11:9*src[1] xr1:4*src[1]
     D16MAC_SA_HW(xr11,xr2,xr13,xr9);//xr11:9*src[1]-16*src[3] xr9:16*src[1]+15*src[3]
     D16MAC_SS_HW(xr1,xr2,xr12,xr10);//xr1:4*src[1]-9*src[3]   xr10:15*src[1]-4*src[3]
    
     D16MAC_SA_HW(xr10,xr3,xr13,xr1);//xr10:15*src[1]-4*src[3]-16*src[5] xr1:4*src[1]-9*src[3]+15*src[5]
     D16MAC_AA_HW(xr9,xr3,xr12,xr11);//xr9:16*src[1]+15*src[3]+ 9*src[5] xr11:9*src[1]-16*src[3]+4*src[5]
    
     D16MAC_SA_HW(xr1,xr4,xr13,xr11);//xr1:t4 4*src[1]-9*src[3]+15*src[5]-16*src[7] xr11:t3 9*src[1]-16*src[3]+4*src[5]+15*src[7]
     D16MAC_SA_HW(xr10,xr4,xr12,xr9);//xr10:t2 15*src[1]-4*src[3]-16*src[5]-9*src[7] xr9:t1 16*src[1]+15*src[3]+ 9*src[5]+4*src[7]
     D32ADD_AS(xr5,xr5,xr9,xr9);     //xr5:t5+t1 xr9:t5-t1
     D32ADD_AS(xr6,xr6,xr10,xr10);   //xr6:t6+t2 xr10:t6-t2
     D32ADD_AS(xr8,xr8,xr11,xr11);   //xr8:t7+t3 xr11:t7-t3
     D32ADD_AS(xr7,xr7,xr1,xr1);     //xr7:t8+t4 xr1:t8-t4
     D32SARL(xr5,xr6,xr5,3);
     D32SARL(xr6,xr7,xr8,3);
     D32SARL(xr7,xr11,xr1,3);
     D32SARL(xr8,xr9,xr10,3);
     S32STD(xr5,dst,0x0);
     S32STD(xr6,dst,0x4);
     S32STD(xr7,dst,0x8);
     S32STD(xr8,dst,0xc);
     src+=8;
     dst+=8;
   } 
    src = block;
    dst = block;

    for(i = 0; i < 4; i++){
        S32LDD(xr1,src,0x00);  //xr1:src[1] src[0]        
        S32LDD(xr2,src,0x10);  //xr2:src[9] src[8]
        S32LDD(xr3,src,0x20);  //xr3:src[17] src[16]
        S32LDD(xr4,src,0x30);  //xr4:src[25] src[24]
        S32LDD(xr5,src,0x40);  //xr5:src[33] src[32]
        S32LDD(xr6,src,0x50);  //xr6:src[41] src[40]
        S32LDD(xr7,src,0x60);  //xr7:src[49] src[48]
        S32LDD(xr8,src,0x70);  //xr8:src[57] src[56]
               
        Q16ADD_AS_WW(xr9,xr1,xr5,xr10); //xr9:src[1]+src[33] src[0]+src[32] xr10:src[1]-src[33] src[0]-src[32]    
        Q16SLL(xr11,xr9,xr10,xr12,3);   //xr11:(src[1]+src[33])<<3 (src[0]+src[32])<<3 xr12:(src[1]-src[33])<<3 (src[0]-src[32])<<3       
        Q16SLL(xr13,xr9,xr10,xr14,2);   //xr13:(src[1]+src[33])<<2 (src[0]+src[32])<<2 xr14:(src[1]-src[33])<<2 (src[0]-src[32])<<2
        Q16ADD_AS_WW(xr11,xr11,xr13,xr0);//xr11:pt1 (src[1]+src[33])<<3+(src[1]+src[33])<<2 t1 (src[0]+src[32])<<3+(src[0]+src[32])<<2
        Q16ADD_AS_WW(xr12,xr12,xr14,xr0);//xr12:pt2 (src[1]-src[33])<<3+(src[1]-src[33])<<2 t2 (src[0]-src[32])<<3+(src[0]-src[32])<<2
        Q16SLL(xr9,xr3,xr7,xr10,4);      //xr9:src[17]<<4 src[16]<<4 xr10:src[49]<<4 src[48]<<4
        Q16SLL(xr13,xr3,xr7,xr14,3);     //xr13:src[17]<<3 src[16]<<3 xr14:src[49]<<3 src[48]<<3
        Q16SLL(xr15,xr3,xr7,xr1,1);      //xr15:src[17]<<1 src[16]<<1 xr1:src[49]<<1 src[48]<<1

        Q16ADD_AS_WW(xr9,xr9,xr14,xr0); //xr9:src[17]<<4+src[49]<<3   src[16]<<4+src[48]<<3
        Q16ADD_AS_WW(xr0,xr9,xr1,xr9);  //xr9:pt3 src[17]<<4+src[49]<<3-src[49]<<1  t3 src[16]<<4+src[48]<<3-src[48]<<1
        Q16ADD_AS_WW(xr0,xr13,xr15,xr13); //xr13:src[17]<<3-src[17]<<1 src[16]<<3-src[16]<<1
        Q16ADD_AS_WW(xr0,xr13,xr10,xr13); //xr13:pt4 src[17]<<3-src[17]<<1-src[49]<<4 t4  src[16]<<3-src[16]<<1-src[48]<<4
        Q16ADD_AS_WW(xr11,xr11,xr9,xr9);   //xr11:pt5 t5  xr9:pt8 t8
        Q16ADD_AS_WW(xr12,xr12,xr13,xr13); //xr12:pt6 t6  xr13:pt7 t7
        Q16ADD_AS_WW(xr1,xr6,xr4,xr3);  //xr1:src[41]+src[25] src[40]+src[24] xr3:src[41]-src[25] src[40]-src[24]
        Q16SLL(xr10,xr2,xr4,xr14,4);    //xr10:src[9]<<4 src[8]<<4   xr14:src[25]<<4 src[24]<<4
        Q16SLL(xr15,xr6,xr8,xr5,4);     //xr15:src[41]<<4 src[40]<<4 xr5:src[57]<<4 src[56]<<4
       

        Q16SLL(xr10,xr2,xr4,xr14,4);      //xr10:src[9]<<4 src[8]<<4  xr14:src[25]<<4 src[24]<<4
        Q16SLL(xr15,xr6,xr0,xr0,3);       //xr15:src[41]<<3 src[40]<<3 
        Q16SLL(xr5,xr8,xr0,xr0,2);        //xr5:src[57]<<2 src[56]<<2
        Q16ADD_AS_WW(xr7,xr10,xr14,xr0);  //xr7:src[9]<<4 + src[25]<<4  src[8]<<4 + src[24]<<4
        Q16ADD_AS_WW(xr5,xr15,xr5,xr0);   //xr5:src[41]<<3+src[57]<<2 src[40]<<3+src[56]<<2
        Q16ADD_AS_WW(xr5,xr7,xr5,xr0);    //xr5:src[9]<<4 + src[25]<<4+src[41]<<3+src[57]<<2   src[8]<<4 + src[24]<<4+src[40]<<3+src[56]<<2
        Q16ADD_AS_WW(xr5,xr5,xr3,xr0);    //xr5:pt1 t1
        Q16SLL(xr10,xr6,xr8,xr14,4);      //xr10:src[41]<<4 src[40]<<4 xr14:src[57]<<4 src[56]<<4
        Q16SLL(xr15,xr4,xr0,xr0,3);       //xr15:src[25]<<3 src[24]<<3 
        Q16SLL(xr3,xr2,xr0,xr0,2);        //xr3: src[9]<<2  src[8]<<2
        Q16ADD_AS_WW(xr0,xr10,xr14,xr7);  //xr7: src[41]<<4-src[57]<<4 src[40]<<4-src[56]<<4 
        Q16ADD_AS_WW(xr0,xr7,xr15,xr7);   //xr7: src[41]<<4-src[57]<<4-src[25]<<3 src[40]<<4-src[56]<<4-src[24]<<3
        Q16ADD_AS_WW(xr7,xr7,xr3,xr0);    //xr7:src[41]<<4-src[57]<<4-src[25]<<3+src[9] src[40]<<4-src[56]<<4-src[24]<<3+src[8]
        Q16ADD_AS_WW(xr0,xr7,xr1,xr7);    //xr7:pt4 t4
        
        Q16ADD_AS_WW(xr1,xr8,xr2,xr3);    //xr1:src[57]+src[9]  src[56]+src[8]  xr3:src[57]-src[9]  src[56]-src[8]
        Q16SLL(xr10,xr2,xr6,xr14,4);      //xr10:src[9]<<4 src[8]<<4  xr14:src[41]<<4 src[40]<<4
        Q16SLL(xr15,xr8,xr0,xr0,3);       //xr15:src[57]<<3 src[56]<<3
        Q16ADD_AS_WW(xr0,xr10,xr14,xr10); //xr10:src[9]<<4-src[41]<<4   src[8]<<4-src[40]<<4
        Q16ADD_AS_WW(xr0,xr10,xr15,xr10); //xr10:src[9]<<4-src[41]<<4-src[57]<<3   src[8]<<4-src[40]<<4-src[56]<<3
        Q16SLL(xr15,xr4,xr0,xr0,2);       //xr15:src[25]<<2 src[24]<<2
        Q16ADD_AS_WW(xr0,xr10,xr15,xr10); //xr10:src[9]<<4-src[41]<<4-src[57]<<3-src[25]<<2   src[8]<<4-src[40]<<4-src[56]<<3-src[24]<<2
        Q16ADD_AS_WW(xr0,xr10,xr1,xr1);   //xr1:pt2 t2
      
        Q16SLL(xr10,xr8,xr4,xr14,4);      //xr10:src[57]<<4 src[56]<<4  xr14:src[25]<<4 src[24]<<4
        Q16SLL(xr15,xr2,xr0,xr0,3);       //xr15:src[9]<<3 src[8]<<3
        Q16ADD_AS_WW(xr0,xr10,xr14,xr10); //xr10:src[57]<<4-src[25]<<4  src[56]<<4-src[24]<<4 
        Q16ADD_AS_WW(xr10,xr10,xr15,xr0); //xr10:src[57]<<4-src[25]<<4+src[9]<<3  src[56]<<4-src[24]<<4+src[8]<<3
        Q16SLL(xr15,xr6,xr0,xr0,2);       //xr15:src[41]<<2 src[40]<<2
        Q16ADD_AS_WW(xr10,xr10,xr15,xr0);
        Q16ADD_AS_WW(xr3,xr10,xr3,xr0);   //xr3:pt3 t3*/

        Q16ADD_AS_WW(xr11,xr11,xr5,xr5);  //xr11:pt5+pt1 t5+t1 xr5:pt5-pt1 t5-t1
        Q16ADD_AS_WW(xr12,xr12,xr1,xr1);  //xr12:pt6+pt2 t6+t2 xr1:pt6-pt2 t6-t2
        Q16ADD_AS_WW(xr13,xr13,xr3,xr3);  //xr13:pt7+pt3 t7+t3 xr3:pt7-pt3 t7-t3
        Q16ADD_AS_WW(xr9,xr9,xr7,xr7);    //xr9: pt8+pt4 t8+t4 xr7:pt8-pt4 t8-t4
        S32I2M(xr2,64<<16|64);
        Q16ACC_AS(xr11,xr2,xr0,xr12);     //xr11:pt5+pt1+64  t5+t1+64 xr12:pt6+pt2+64  t6+t2+64
        Q16ACC_AS(xr13,xr2,xr0,xr9);      //xr13:pt7+pt3+64  t7+t3+64 xr9: pt8+pt4+64  t8+t4+64
        S32I2M(xr4,65<<16|65);
        Q16ACC_AS(xr5,xr4,xr0,xr1);       //xr5:pt5-pt1+65  t5-t1+65  xr1:pt6-pt2+65  t6-t2+65
        Q16ACC_AS(xr3,xr4,xr0,xr7);       //xr3:pt7-pt3+65  t7-t3+65  xr7:pt8-pt4+65  t8-t4+65
        Q16SAR(xr11,xr11,xr12,xr12,7);    //xr11:dst[0] xr12:dst[8]
        Q16SAR(xr13,xr13,xr9,xr9,7);      //xr13:dst[16] xr9:dst[24]
        Q16SAR(xr7,xr7,xr3,xr3,7);        //xr7:dst[32] xr3: dst[40]
        Q16SAR(xr1,xr1,xr5,xr5,7);        //xr1:dst[48] xr5: dst[56]
        S32STD(xr11,dst,0x00);
        S32STD(xr12,dst,0x10);
        S32STD(xr13,dst,0x20);
        S32STD(xr9,dst,0x30);
        S32STD(xr7,dst,0x40);
        S32STD(xr3,dst,0x50);
        S32STD(xr1,dst,0x60);
        S32STD(xr5,dst,0x70);
        src+=2;
        dst+=2;
    }
}
#else

static void vc1_inv_trans_8x8_c(DCTELEM block[64])
{
    int i;
    register int t1,t2,t3,t4,t5,t6,t7,t8;
    DCTELEM *src, *dst;
    src = block;
    dst = block;
    for(i = 0; i < 8; i++){
        t1 = 12 * (src[0] + src[4]);
        t2 = 12 * (src[0] - src[4]);
        t3 = 16 * src[2] +  6 * src[6];
        t4 =  6 * src[2] - 16 * src[6];
        t5 = t1 + t3;
        t6 = t2 + t4;
        t7 = t2 - t4;
        t8 = t1 - t3;
        t1 = 16 * src[1] + 15 * src[3] +  9 * src[5] +  4 * src[7];
        t2 = 15 * src[1] -  4 * src[3] - 16 * src[5] -  9 * src[7];
        t3 =  9 * src[1] - 16 * src[3] +  4 * src[5] + 15 * src[7];
        t4 =  4 * src[1] -  9 * src[3] + 15 * src[5] - 16 * src[7];
        dst[0] = (t5 + t1 + 4) >> 3;
        dst[1] = (t6 + t2 + 4) >> 3;
        dst[2] = (t7 + t3 + 4) >> 3;
        dst[3] = (t8 + t4 + 4) >> 3;
        dst[4] = (t8 - t4 + 4) >> 3;
        dst[5] = (t7 - t3 + 4) >> 3;
        dst[6] = (t6 - t2 + 4) >> 3;
        dst[7] = (t5 - t1 + 4) >> 3;
        src += 8;
        dst += 8;
    }
  src=block;
  dst=block;
   for(i = 0; i < 8; i++){
        t1 = 12 * (src[0] + src[32]);
        t2 = 12 * (src[0] - src[32]);
        t3 = 16 * src[16] +  6 * src[48];
        t4 =  6 * src[16] - 16 * src[48];
        t5 = t1 + t3;
        t6 = t2 + t4;
        t7 = t2 - t4;
        t8 = t1 - t3;
        
        t1 = 16 * src[8] + 15 * src[24] +  9 * src[40] +  4 * src[56];
        t2 = 15 * src[8] -  4 * src[24] - 16 * src[40] -  9 * src[56];
        t3 =  9 * src[8] - 16 * src[24] +  4 * src[40] + 15 * src[56];
        t4 =  4 * src[8] -  9 * src[24] + 15 * src[40] - 16 * src[56];
        dst[0]  = (t5 + t1 + 64) >> 7;
        dst[8]  = (t6 + t2 + 64) >> 7;
        dst[16] = (t7 + t3 + 64) >> 7;
        dst[24] = (t8 + t4 + 64) >> 7;
        dst[32] = (t8 - t4 + 64+1) >> 7;
        dst[40] = (t7 - t3 + 64+1) >> 7;
        dst[48] = (t6 - t2 + 64+1) >> 7;
        dst[56] = (t5 - t1 + 64+1) >> 7;
        src++;
        dst++;
    }
}
#endif
/** Do inverse transform on 8x4 part of block
*/
#ifdef JZ4740_MXU_OPT

static void vc1_inv_trans_8x4_c(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;
    off = n * 32;
    src = block + off;
    dst = block + off;
 
   S32I2M(xr15,W4<<16|W4);  //xr15:12|12
   S32I2M(xr14,W6<<16|W2);  //xr14:16|6
   S32I2M(xr13,W6<<16|W5);  //xr13:16|15
   S32I2M(xr12,W3<<16|W1);  //xr12:9 |4
   for(i=0;i<4;i++)
   {
     S32LDD(xr1,src,0x0); //xr1:src[1] src[0]
     S32LDD(xr2,src,0x4); //xr2:src[3] src[2]
     S32LDD(xr3,src,0x8); //xr3:src[5] src[4]
     S32LDD(xr4,src,0xc); //xr4:src[7] src[6]
     D16MUL_LW(xr5,xr1,xr15,xr6); //xr5:12*src[0] xr6:12*src[0]
     D16MAC_AS_LW(xr5,xr3,xr15,xr6); //xr5:t1 12*src[0]+12*src[4] xr6:t2 12*src[0]-12*src[4]
     D16MUL_LW(xr7,xr2,xr14,xr8);      //xr7:16*src[2] xr8:6*src[2]
     D16MAC_SA_LW(xr8,xr4,xr14,xr7);    //xr8:t4 6*src[2]-16*src[6] xr7:t3 16*src[2]+6*src[6]
      
     D32ADD_AS(xr5,xr5,xr7,xr7);     //xr5:t5 t1+t3  xr7:t8 t1-t3
     D32ADD_AS(xr6,xr6,xr8,xr8);     //xr6:t6 t2+t4  xr8:t7 t2-t4
     S32I2M(xr9,4);
     D32ACC_AS(xr5,xr9,xr0,xr7);
     D32ACC_AS(xr6,xr9,xr0,xr8);
     D16MUL_HW(xr9,xr1,xr13,xr10);   //xr9:16*src[1] xr10:15*src[1]
     D16MUL_HW(xr11,xr1,xr12,xr1);   //xr11:9*src[1] xr1:4*src[1]
     D16MAC_SA_HW(xr11,xr2,xr13,xr9);//xr11:9*src[1]-16*src[3] xr9:16*src[1]+15*src[3]
     D16MAC_SS_HW(xr1,xr2,xr12,xr10);//xr1:4*src[1]-9*src[3]   xr10:15*src[1]-4*src[3]
    
     D16MAC_SA_HW(xr10,xr3,xr13,xr1);//xr10:15*src[1]-4*src[3]-16*src[5] xr1:4*src[1]-9*src[3]+15*src[5]
     D16MAC_AA_HW(xr9,xr3,xr12,xr11);//xr9:16*src[1]+15*src[3]+ 9*src[5] xr11:9*src[1]-16*src[3]+4*src[5]
    
     D16MAC_SA_HW(xr1,xr4,xr13,xr11);//xr1:t4 4*src[1]-9*src[3]+15*src[5]-16*src[7] xr11:t3 9*src[1]-16*src[3]+4*src[5]+15*src[7]
     D16MAC_SA_HW(xr10,xr4,xr12,xr9);//xr10:t2 15*src[1]-4*src[3]-16*src[5]-9*src[7] xr9:t1 16*src[1]+15*src[3]+ 9*src[5]+4*src[7]
     D32ADD_AS(xr5,xr5,xr9,xr9);     //xr5:t5+t1 xr9:t5-t1
     D32ADD_AS(xr6,xr6,xr10,xr10);   //xr6:t6+t2 xr10:t6-t2
     D32ADD_AS(xr8,xr8,xr11,xr11);   //xr8:t7+t3 xr11:t7-t3
     D32ADD_AS(xr7,xr7,xr1,xr1);     //xr7:t8+t4 xr1:t8-t4
     D32SARL(xr5,xr6,xr5,3);
     D32SARL(xr6,xr7,xr8,3);
     D32SARL(xr7,xr11,xr1,3);
     D32SARL(xr8,xr9,xr10,3);
     S32STD(xr5,dst,0x0);
     S32STD(xr6,dst,0x4);
     S32STD(xr7,dst,0x8);
     S32STD(xr8,dst,0xc);
     src+=8;
     dst+=8;
   } 
    src = block + off;
    dst = block + off;
    for(i = 0; i < 4; i++){
        S32LDD(xr1,src,0x00);  //xr1:src[1] src[0]
        S32LDD(xr2,src,0x10);  //xr2:src[9] src[8]
        S32LDD(xr3,src,0x20);  //xr3:src[17] src[16]
        S32LDD(xr4,src,0x30);  //xr4:src[25] src[24]
        Q16ADD_AS_WW(xr5,xr1,xr3,xr6); //xr5:src[1]+src[17] src[0]+src[16] xr6:src[1]-src[17] src[0]-src[16]
        Q16SLL(xr7,xr5,xr6,xr8,4);     //xr7:(src[1]+src[17])<<4 (src[0]+src[16])<<4 xr8:(src[1]-src[17])<<4 (src[0]-src[16])<<4
        Q16ADD_AS_WW(xr5,xr7,xr5,xr0); //xr5:pt1 t1 
        Q16ADD_AS_WW(xr6,xr8,xr6,xr0); //xr6:pt2 t2
        S32I2M(xr11,64<<16|64);
        Q16ACC_AS(xr5,xr11,xr0,xr6);
        Q16SLL(xr1,xr2,xr4,xr3,3);     //xr1:src[9]<<3 src[8]<<3  xr3:src[25]<<3 src[24]<<3
        Q16SLL(xr7,xr2,xr4,xr8,1);     //xr7:src[9]<<1 src[8]<<1  xr8:src[25]<<1 src[24]<<1
        Q16ADD_AS_WW(xr9,xr1,xr7,xr0); //xr9:pt5 t5 
        Q16ADD_AS_WW(xr10,xr3,xr8,xr0);//xr10:pt6 t6
        Q16SLL(xr1,xr1,xr3,xr3,1);     //xr1:src[9]<<16 src[8]<<16  xr3:src[25]<<16 src[24]<<16
        Q16SLL(xr2,xr2,xr4,xr4,2);     //xr2:src[9]<<2 src[8]<<2    xr4:src[25]<<2  src[24]<<2

        Q16ADD_AS_WW(xr1,xr1,xr2,xr0); 
        Q16ADD_AS_WW(xr3,xr3,xr4,xr0);
        Q16ADD_AS_WW(xr1,xr1,xr7,xr0); //xr1:pt3 t3
        Q16ADD_AS_WW(xr3,xr3,xr8,xr0); //xr3:pt4 t4
        Q16ADD_AS_WW(xr5,xr5,xr1,xr1); //xr5:pt1+pt3 t1+t3 xr1:pt1-pt3 t1-t3
        Q16ADD_AS_WW(xr6,xr6,xr3,xr3); //xr6:pt2+pt4 t2+t4 xr3:pt2-pt4 t2-t4
        Q16ADD_AS_WW(xr5,xr5,xr10,xr0); //xr5:pt1+pt3+pt6 t1+t3+t6
        Q16ADD_AS_WW(xr0,xr1,xr10,xr1); //xr1:pt1-pt3-pt6 t1-t3-t6
        Q16ADD_AS_WW(xr3,xr3,xr9,xr0);  //xr3:pt2-pt4+pt5 t2-t4+t5
        Q16ADD_AS_WW(xr0,xr6,xr9,xr6);  //xr6:pt2+pt4-pt5 t2+t4-t5
        Q16SAR(xr5,xr5,xr1,xr1,7);
        Q16SAR(xr3,xr3,xr6,xr6,7);
        S32STD(xr5,dst,0x00);
        S32STD(xr3,dst,0x10);
        S32STD(xr6,dst,0x20);
        S32STD(xr1,dst,0x30);

        src +=2;
        dst +=2;
    }

}
#else

static void vc1_inv_trans_8x4_c(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;

    register int t1,t2,t3,t4,t5,t6,t7,t8;
    off = n * 32;
    src = block + off;
    dst = block + off;
    for(i = 0; i < 4; i++){
        t1 = 12 * (src[0] + src[4]);
        t2 = 12 * (src[0] - src[4]);
        t3 = 16 * src[2] +  6 * src[6];
        t4 =  6 * src[2] - 16 * src[6];

        t5 = t1 + t3;
        t6 = t2 + t4;
        t7 = t2 - t4;
        t8 = t1 - t3;

        t1 = 16 * src[1] + 15 * src[3] +  9 * src[5] +  4 * src[7];
        t2 = 15 * src[1] -  4 * src[3] - 16 * src[5] -  9 * src[7];
        t3 =  9 * src[1] - 16 * src[3] +  4 * src[5] + 15 * src[7];
        t4 =  4 * src[1] -  9 * src[3] + 15 * src[5] - 16 * src[7];

        dst[0] = (t5 + t1 + 4) >> 3;
        dst[1] = (t6 + t2 + 4) >> 3;
        dst[2] = (t7 + t3 + 4) >> 3;
        dst[3] = (t8 + t4 + 4) >> 3;
        dst[4] = (t8 - t4 + 4) >> 3;
        dst[5] = (t7 - t3 + 4) >> 3;
        dst[6] = (t6 - t2 + 4) >> 3;
        dst[7] = (t5 - t1 + 4) >> 3;

        src += 8;
        dst += 8;
    }

    src = block + off;
    dst = block + off;
    for(i = 0; i < 8; i++){
        t1 = 17 * (src[ 0] + src[16]);
        t2 = 17 * (src[ 0] - src[16]);
        t3 = 22 * src[ 8];
        t4 = 22 * src[24];
        t5 = 10 * src[ 8];
        t6 = 10 * src[24];
        dst[ 0] = (t1 + t3 + t6 + 64) >> 7;
        dst[ 8] = (t2 - t4 + t5 + 64) >> 7;
        dst[16] = (t2 + t4 - t5 + 64) >> 7;
        dst[24] = (t1 - t3 - t6 + 64) >> 7;

        src ++;
        dst ++;
    }
}
#endif
/** Do inverse transform on 4x8 parts of block
*/
#ifdef JZ4740_MXU_OPT

static void vc1_inv_trans_4x8_c(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;

    off = n * 4;
    src = block + off;
    dst = block + off;
    S32I2M(xr15,W7<<16|W8);  //xr15:17|17
    S32I2M(xr14,W8<<16|W9);  //xr14:22|10
    for(i = 0; i < 8; i++){
        S32LDD(xr1,src,0x0); //xr1:src[1] src[0]
        S32LDD(xr2,src,0x4); //xr2:src[3] src[2]
        D16MUL_LW(xr3,xr1,xr15,xr4); //xr3:17*src[0] xr4:17*src[0]
        D16MAC_AS_LW(xr3,xr2,xr15,xr4); //xr3:t1  17*src[0]+src[2]*17 //xr4:t2   17*src[0]-17*src[2]
        S32I2M(xr13,4);
        D32ACC_AS(xr3,xr13,xr0,xr4);    //xr3:t1+4  xr4:t2+4
        D16MUL_HW(xr5,xr1,xr14,xr6);    //xr5:t3  22*src[1]  xr6:t5  10*src[1]
        D16MUL_HW(xr7,xr2,xr14,xr8);    //xr7:t4  22*src[3]  xr8:t6  10*src[3]
        D32ADD_AS(xr5,xr5,xr8,xr0);      //xr5:t3+t6
        D32ADD_AS(xr0,xr7,xr6,xr6);      //xr6:t4-t5
        D32ADD_AS(xr3,xr3,xr5,xr5);      //xr3:t1+4+t3+t6 xr5:t1+4-t3-t6
        D32ADD_AS(xr4,xr4,xr6,xr6);      //xr4:t2+4+t4-t5 xr6:t2+4-t4+t5
        D32SARL(xr3,xr6,xr3,3);
        D32SARL(xr5,xr5,xr4,3);
        S32STD(xr3,dst,0x0);
        S32STD(xr5,dst,0x4);
        src += 8;
        dst += 8;
    }

    src = block + off;
    dst = block + off;

    for(i = 0; i < 2; i++){
        S32LDD(xr1,src,0x00);  //xr1:src[1] src[0]        
        S32LDD(xr2,src,0x10);  //xr2:src[9] src[8]
        S32LDD(xr3,src,0x20);  //xr3:src[17] src[16]
        S32LDD(xr4,src,0x30);  //xr4:src[25] src[24]
        S32LDD(xr5,src,0x40);  //xr5:src[33] src[32]
        S32LDD(xr6,src,0x50);  //xr6:src[41] src[40]
        S32LDD(xr7,src,0x60);  //xr7:src[49] src[48]
        S32LDD(xr8,src,0x70);  //xr8:src[57] src[56]
               
        Q16ADD_AS_WW(xr9,xr1,xr5,xr10); //xr9:src[1]+src[33] src[0]+src[32] xr10:src[1]-src[33] src[0]-src[32]    
        Q16SLL(xr11,xr9,xr10,xr12,3);   //xr11:(src[1]+src[33])<<3 (src[0]+src[32])<<3 xr12:(src[1]-src[33])<<3 (src[0]-src[32])<<3       
        Q16SLL(xr13,xr9,xr10,xr14,2);   //xr13:(src[1]+src[33])<<2 (src[0]+src[32])<<2 xr14:(src[1]-src[33])<<2 (src[0]-src[32])<<2
        Q16ADD_AS_WW(xr11,xr11,xr13,xr0);//xr11:pt1 (src[1]+src[33])<<3+(src[1]+src[33])<<2 t1 (src[0]+src[32])<<3+(src[0]+src[32])<<2
        Q16ADD_AS_WW(xr12,xr12,xr14,xr0);//xr12:pt2 (src[1]-src[33])<<3+(src[1]-src[33])<<2 t2 (src[0]-src[32])<<3+(src[0]-src[32])<<2
        Q16SLL(xr9,xr3,xr7,xr10,4);      //xr9:src[17]<<4 src[16]<<4 xr10:src[49]<<4 src[48]<<4
        Q16SLL(xr13,xr3,xr7,xr14,3);     //xr13:src[17]<<3 src[16]<<3 xr14:src[49]<<3 src[48]<<3
        Q16SLL(xr15,xr3,xr7,xr1,1);      //xr15:src[17]<<1 src[16]<<1 xr1:src[49]<<1 src[48]<<1

        Q16ADD_AS_WW(xr9,xr9,xr14,xr0); //xr9:src[17]<<4+src[49]<<3   src[16]<<4+src[48]<<3
        Q16ADD_AS_WW(xr0,xr9,xr1,xr9);  //xr9:pt3 src[17]<<4+src[49]<<3-src[49]<<1  t3 src[16]<<4+src[48]<<3-src[48]<<1
        Q16ADD_AS_WW(xr0,xr13,xr15,xr13); //xr13:src[17]<<3-src[17]<<1 src[16]<<3-src[16]<<1
        Q16ADD_AS_WW(xr0,xr13,xr10,xr13); //xr13:pt4 src[17]<<3-src[17]<<1-src[49]<<4 t4  src[16]<<3-src[16]<<1-src[48]<<4
        Q16ADD_AS_WW(xr11,xr11,xr9,xr9);   //xr11:pt5 t5  xr9:pt8 t8
        Q16ADD_AS_WW(xr12,xr12,xr13,xr13); //xr12:pt6 t6  xr13:pt7 t7
        Q16ADD_AS_WW(xr1,xr6,xr4,xr3);  //xr1:src[41]+src[25] src[40]+src[24] xr3:src[41]-src[25] src[40]-src[24]
        Q16SLL(xr10,xr2,xr4,xr14,4);    //xr10:src[9]<<4 src[8]<<4   xr14:src[25]<<4 src[24]<<4
        Q16SLL(xr15,xr6,xr8,xr5,4);     //xr15:src[41]<<4 src[40]<<4 xr5:src[57]<<4 src[56]<<4
       

        Q16SLL(xr10,xr2,xr4,xr14,4);      //xr10:src[9]<<4 src[8]<<4  xr14:src[25]<<4 src[24]<<4
        Q16SLL(xr15,xr6,xr0,xr0,3);       //xr15:src[41]<<3 src[40]<<3 
        Q16SLL(xr5,xr8,xr0,xr0,2);        //xr5:src[57]<<2 src[56]<<2
        Q16ADD_AS_WW(xr7,xr10,xr14,xr0);  //xr7:src[9]<<4 + src[25]<<4  src[8]<<4 + src[24]<<4
        Q16ADD_AS_WW(xr5,xr15,xr5,xr0);   //xr5:src[41]<<3+src[57]<<2 src[40]<<3+src[56]<<2
        Q16ADD_AS_WW(xr5,xr7,xr5,xr0);    //xr5:src[9]<<4 + src[25]<<4+src[41]<<3+src[57]<<2   src[8]<<4 + src[24]<<4+src[40]<<3+src[56]<<2
        Q16ADD_AS_WW(xr5,xr5,xr3,xr0);    //xr5:pt1 t1
        Q16SLL(xr10,xr6,xr8,xr14,4);      //xr10:src[41]<<4 src[40]<<4 xr14:src[57]<<4 src[56]<<4
        Q16SLL(xr15,xr4,xr0,xr0,3);       //xr15:src[25]<<3 src[24]<<3 
        Q16SLL(xr3,xr2,xr0,xr0,2);        //xr3: src[9]<<2  src[8]<<2
        Q16ADD_AS_WW(xr0,xr10,xr14,xr7);  //xr7: src[41]<<4-src[57]<<4 src[40]<<4-src[56]<<4 
        Q16ADD_AS_WW(xr0,xr7,xr15,xr7);   //xr7: src[41]<<4-src[57]<<4-src[25]<<3 src[40]<<4-src[56]<<4-src[24]<<3
        Q16ADD_AS_WW(xr7,xr7,xr3,xr0);    //xr7:src[41]<<4-src[57]<<4-src[25]<<3+src[9] src[40]<<4-src[56]<<4-src[24]<<3+src[8]
        Q16ADD_AS_WW(xr0,xr7,xr1,xr7);    //xr7:pt4 t4
        
        Q16ADD_AS_WW(xr1,xr8,xr2,xr3);    //xr1:src[57]+src[9]  src[56]+src[8]  xr3:src[57]-src[9]  src[56]-src[8]
        Q16SLL(xr10,xr2,xr6,xr14,4);      //xr10:src[9]<<4 src[8]<<4  xr14:src[41]<<4 src[40]<<4
        Q16SLL(xr15,xr8,xr0,xr0,3);       //xr15:src[57]<<3 src[56]<<3
        Q16ADD_AS_WW(xr0,xr10,xr14,xr10); //xr10:src[9]<<4-src[41]<<4   src[8]<<4-src[40]<<4
        Q16ADD_AS_WW(xr0,xr10,xr15,xr10); //xr10:src[9]<<4-src[41]<<4-src[57]<<3   src[8]<<4-src[40]<<4-src[56]<<3
        Q16SLL(xr15,xr4,xr0,xr0,2);       //xr15:src[25]<<2 src[24]<<2
        Q16ADD_AS_WW(xr0,xr10,xr15,xr10); //xr10:src[9]<<4-src[41]<<4-src[57]<<3-src[25]<<2   src[8]<<4-src[40]<<4-src[56]<<3-src[24]<<2
        Q16ADD_AS_WW(xr0,xr10,xr1,xr1);   //xr1:pt2 t2
      
        Q16SLL(xr10,xr8,xr4,xr14,4);      //xr10:src[57]<<4 src[56]<<4  xr14:src[25]<<4 src[24]<<4
        Q16SLL(xr15,xr2,xr0,xr0,3);       //xr15:src[9]<<3 src[8]<<3
        Q16ADD_AS_WW(xr0,xr10,xr14,xr10); //xr10:src[57]<<4-src[25]<<4  src[56]<<4-src[24]<<4 
        Q16ADD_AS_WW(xr10,xr10,xr15,xr0); //xr10:src[57]<<4-src[25]<<4+src[9]<<3  src[56]<<4-src[24]<<4+src[8]<<3
        Q16SLL(xr15,xr6,xr0,xr0,2);       //xr15:src[41]<<2 src[40]<<2
        Q16ADD_AS_WW(xr10,xr10,xr15,xr0);
        Q16ADD_AS_WW(xr3,xr10,xr3,xr0);   //xr3:pt3 t3*/

        Q16ADD_AS_WW(xr11,xr11,xr5,xr5);  //xr11:pt5+pt1 t5+t1 xr5:pt5-pt1 t5-t1
        Q16ADD_AS_WW(xr12,xr12,xr1,xr1);  //xr12:pt6+pt2 t6+t2 xr1:pt6-pt2 t6-t2
        Q16ADD_AS_WW(xr13,xr13,xr3,xr3);  //xr13:pt7+pt3 t7+t3 xr3:pt7-pt3 t7-t3
        Q16ADD_AS_WW(xr9,xr9,xr7,xr7);    //xr9: pt8+pt4 t8+t4 xr7:pt8-pt4 t8-t4
        S32I2M(xr2,64<<16|64);
        Q16ACC_AS(xr11,xr2,xr0,xr12);     //xr11:pt5+pt1+64  t5+t1+64 xr12:pt6+pt2+64  t6+t2+64
        Q16ACC_AS(xr13,xr2,xr0,xr9);      //xr13:pt7+pt3+64  t7+t3+64 xr9: pt8+pt4+64  t8+t4+64
        S32I2M(xr4,65<<16|65);
        Q16ACC_AS(xr5,xr4,xr0,xr1);       //xr5:pt5-pt1+65  t5-t1+65  xr1:pt6-pt2+65  t6-t2+65
        Q16ACC_AS(xr3,xr4,xr0,xr7);       //xr3:pt7-pt3+65  t7-t3+65  xr7:pt8-pt4+65  t8-t4+65
        Q16SAR(xr11,xr11,xr12,xr12,7);    //xr11:dst[0] xr12:dst[8]
        Q16SAR(xr13,xr13,xr9,xr9,7);      //xr13:dst[16] xr9:dst[24]
        Q16SAR(xr7,xr7,xr3,xr3,7);        //xr7:dst[32] xr3: dst[40]
        Q16SAR(xr1,xr1,xr5,xr5,7);        //xr1:dst[48] xr5: dst[56]
        S32STD(xr11,dst,0x00);
        S32STD(xr12,dst,0x10);
        S32STD(xr13,dst,0x20);
        S32STD(xr9,dst,0x30);
        S32STD(xr7,dst,0x40);
        S32STD(xr3,dst,0x50);
        S32STD(xr1,dst,0x60);
        S32STD(xr5,dst,0x70);
        src+=2;
        dst+=2;
    }

}
#else
static void vc1_inv_trans_4x8_c(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;

    register int t1,t2,t3,t4,t5,t6,t7,t8;
    off = n * 4;
    src = block + off;
    dst = block + off;
    for(i = 0; i < 8; i++){
        t1 = 17 * (src[0] + src[2]);
        t2 = 17 * (src[0] - src[2]);
        t3 = 22 * src[1];
        t4 = 22 * src[3];
        t5 = 10 * src[1];
        t6 = 10 * src[3];

        dst[0] = (t1 + t3 + t6 + 4) >> 3;
        dst[1] = (t2 - t4 + t5 + 4) >> 3;
        dst[2] = (t2 + t4 - t5 + 4) >> 3;
        dst[3] = (t1 - t3 - t6 + 4) >> 3;

        src += 8;
        dst += 8;
    }

    src = block + off;
    dst = block + off;
    for(i = 0; i < 4; i++){
        t1 = 12 * (src[ 0] + src[32]);
        t2 = 12 * (src[ 0] - src[32]);
        t3 = 16 * src[16] +  6 * src[48];
        t4 =  6 * src[16] - 16 * src[48];

        t5 = t1 + t3;
        t6 = t2 + t4;
        t7 = t2 - t4;
        t8 = t1 - t3;

        t1 = 16 * src[ 8] + 15 * src[24] +  9 * src[40] +  4 * src[56];
        t2 = 15 * src[ 8] -  4 * src[24] - 16 * src[40] -  9 * src[56];
        t3 =  9 * src[ 8] - 16 * src[24] +  4 * src[40] + 15 * src[56];
        t4 =  4 * src[ 8] -  9 * src[24] + 15 * src[40] - 16 * src[56];

        dst[ 0] = (t5 + t1 + 64) >> 7;
        dst[ 8] = (t6 + t2 + 64) >> 7;
        dst[16] = (t7 + t3 + 64) >> 7;
        dst[24] = (t8 + t4 + 64) >> 7;
        dst[32] = (t8 - t4 + 64 + 1) >> 7;
        dst[40] = (t7 - t3 + 64 + 1) >> 7;
        dst[48] = (t6 - t2 + 64 + 1) >> 7;
        dst[56] = (t5 - t1 + 64 + 1) >> 7;

        src++;
        dst++;
    }
}
#endif
#ifdef JZ4740_MXU_OPT

static void vc1_inv_trans_4x4_c(DCTELEM block[64], int n)
{
    int i;
    DCTELEM *src, *dst;
    int off;
    off = (n&1) * 4 + (n&2) * 16;
    src = block + off;
    dst = block + off;

    S32I2M(xr15,W7<<16|W8);  //xr15:17|17
    S32I2M(xr14,W8<<16|W9);  //xr14:22|10
    for(i = 0; i < 4; i++){
        S32LDD(xr1,src,0x0); //xr1:src[1] src[0]
        S32LDD(xr2,src,0x4); //xr2:src[3] src[2]
        D16MUL_LW(xr3,xr1,xr15,xr4); //xr3:17*src[0] xr4:17*src[0]
        D16MAC_AS_LW(xr3,xr2,xr15,xr4); //xr3:t1  17*src[0]+src[2]*17 //xr4:t2   17*src[0]-17*src[2]
        S32I2M(xr13,4);
        D32ACC_AS(xr3,xr13,xr0,xr4);    //xr3:t1+4  xr4:t2+4
        D16MUL_HW(xr5,xr1,xr14,xr6);    //xr5:t3  22*src[1]  xr6:t5  10*src[1]
        D16MUL_HW(xr7,xr2,xr14,xr8);    //xr7:t4  22*src[3]  xr8:t6  10*src[3]
        D32ADD_AS(xr5,xr5,xr8,xr0);      //xr5:t3+t6
        D32ADD_AS(xr0,xr7,xr6,xr6);      //xr6:t4-t5
        D32ADD_AS(xr3,xr3,xr5,xr5);      //xr3:t1+4+t3+t6 xr5:t1+4-t3-t6
        D32ADD_AS(xr4,xr4,xr6,xr6);      //xr4:t2+4+t4-t5 xr6:t2+4-t4+t5
        D32SARL(xr3,xr6,xr3,3);
        D32SARL(xr5,xr5,xr4,3);
        S32STD(xr3,dst,0x0);
        S32STD(xr5,dst,0x4);
        src += 8;
        dst += 8;
    }
    src = block + off;
    dst = block + off;
   
    for(i = 0; i < 2; i++){
        S32LDD(xr1,src,0x00);  //xr1:src[1] src[0]
        S32LDD(xr2,src,0x10);  //xr2:src[9] src[8]
        S32LDD(xr3,src,0x20);  //xr3:src[17] src[16]
        S32LDD(xr4,src,0x30);  //xr4:src[25] src[24]
        Q16ADD_AS_WW(xr5,xr1,xr3,xr6); //xr5:src[1]+src[17] src[0]+src[16] xr6:src[1]-src[17] src[0]-src[16]
        Q16SLL(xr7,xr5,xr6,xr8,4);     //xr7:(src[1]+src[17])<<4 (src[0]+src[16])<<4 xr8:(src[1]-src[17])<<4 (src[0]-src[16])<<4
        Q16ADD_AS_WW(xr5,xr7,xr5,xr0); //xr5:pt1 t1 
        Q16ADD_AS_WW(xr6,xr8,xr6,xr0); //xr6:pt2 t2
        S32I2M(xr11,64<<16|64);
        Q16ACC_AS(xr5,xr11,xr0,xr6);
        Q16SLL(xr1,xr2,xr4,xr3,3);     //xr1:src[9]<<3 src[8]<<3  xr3:src[25]<<3 src[24]<<3
        Q16SLL(xr7,xr2,xr4,xr8,1);     //xr7:src[9]<<1 src[8]<<1  xr8:src[25]<<1 src[24]<<1
        Q16ADD_AS_WW(xr9,xr1,xr7,xr0); //xr9:pt5 t5 
        Q16ADD_AS_WW(xr10,xr3,xr8,xr0);//xr10:pt6 t6
        Q16SLL(xr1,xr1,xr3,xr3,1);     //xr1:src[9]<<16 src[8]<<16  xr3:src[25]<<16 src[24]<<16
        Q16SLL(xr2,xr2,xr4,xr4,2);     //xr2:src[9]<<2 src[8]<<2    xr4:src[25]<<2  src[24]<<2

        Q16ADD_AS_WW(xr1,xr1,xr2,xr0); 
        Q16ADD_AS_WW(xr3,xr3,xr4,xr0);
        Q16ADD_AS_WW(xr1,xr1,xr7,xr0); //xr1:pt3 t3
        Q16ADD_AS_WW(xr3,xr3,xr8,xr0); //xr3:pt4 t4
        Q16ADD_AS_WW(xr5,xr5,xr1,xr1); //xr5:pt1+pt3 t1+t3 xr1:pt1-pt3 t1-t3
        Q16ADD_AS_WW(xr6,xr6,xr3,xr3); //xr6:pt2+pt4 t2+t4 xr3:pt2-pt4 t2-t4
        Q16ADD_AS_WW(xr5,xr5,xr10,xr0); //xr5:pt1+pt3+pt6 t1+t3+t6
        Q16ADD_AS_WW(xr0,xr1,xr10,xr1); //xr1:pt1-pt3-pt6 t1-t3-t6
        Q16ADD_AS_WW(xr3,xr3,xr9,xr0);  //xr3:pt2-pt4+pt5 t2-t4+t5
        Q16ADD_AS_WW(xr0,xr6,xr9,xr6);  //xr6:pt2+pt4-pt5 t2+t4-t5
        Q16SAR(xr5,xr5,xr1,xr1,7);
        Q16SAR(xr3,xr3,xr6,xr6,7);
        S32STD(xr5,dst,0x00);
        S32STD(xr3,dst,0x10);
        S32STD(xr6,dst,0x20);
        S32STD(xr1,dst,0x30);

        src +=2;
        dst +=2;
    }

}

#else
/** Do inverse transform on 4x4 part of block
*/
static void vc1_inv_trans_4x4_c(DCTELEM block[64], int n)
{
    int i;
    register int t1,t2,t3,t4,t5,t6;
    DCTELEM *src, *dst;
    int off;
    off = (n&1) * 4 + (n&2) * 16;
    src = block + off;
    dst = block + off;
    for(i = 0; i < 4; i++){
        t1 = 17 * (src[0] + src[2]);
        t2 = 17 * (src[0] - src[2]);
        t3 = 22 * src[1];
        t4 = 22 * src[3];
        t5 = 10 * src[1];
        t6 = 10 * src[3];

        dst[0] = (t1 + t3 + t6 + 4) >> 3;
        dst[1] = (t2 - t4 + t5 + 4) >> 3;
        dst[2] = (t2 + t4 - t5 + 4) >> 3;
        dst[3] = (t1 - t3 - t6 + 4) >> 3;

        src += 8;
        dst += 8;
    }

    src = block + off;
    dst = block + off;
    for(i = 0; i < 4; i++){
        t1 = 17 * (src[ 0] + src[16]);
        t2 = 17 * (src[ 0] - src[16]);
        t3 = 22 * src[ 8];
        t4 = 22 * src[24];
        t5 = 10 * src[ 8];
        t6 = 10 * src[24];

        dst[ 0] = (t1 + t3 + t6 + 64) >> 7;
        dst[ 8] = (t2 - t4 + t5 + 64) >> 7;
        dst[16] = (t2 + t4 - t5 + 64) >> 7;
        dst[24] = (t1 - t3 - t6 + 64) >> 7;

        src ++;
        dst ++;
    }
}
#endif
/* motion compensation functions */
/** Filter in case of 2 filters */
#define VC1_MSPEL_FILTER_16B(DIR, TYPE)                                 \
static av_always_inline int vc1_mspel_ ## DIR ## _filter_16bits(const TYPE *src, int stride, int mode) \
{\
   switch(mode){                                                       \
    case 0: /* no shift - should not occur */                           \
        return 0;                                                       \
    case 1: /* 1/4 shift */                                             \
        return -4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2]; \
    case 2: /* 1/2 shift */                                             \
        return -src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2]; \
    case 3: /* 3/4 shift */                                             \
        return -3*src[-stride] + 18*src[0] + 53*src[stride] - 4*src[stride*2]; \
    }                                                                   \
    return 0; /* should not occur */                                    \
}\

VC1_MSPEL_FILTER_16B(ver, uint8_t);
VC1_MSPEL_FILTER_16B(hor, int16_t);


/** Filter used to interpolate fractional pel values
 */
static av_always_inline int vc1_mspel_filter(const uint8_t *src, int stride, int mode, int r)
{
    switch(mode){
    case 0: //no shift
        return src[0];
    case 1: // 1/4 shift
         return (-4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2] + 32 - r) >> 6;
    case 2: // 1/2 shift
        return (-src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2] + 8 - r) >> 4;
    case 3: // 3/4 shift
        return (-3*src[-stride] + 18*src[0] + 53*src[stride] - 4*src[stride*2] + 32 - r) >> 6;
    }
    return 0; //should not occur
}

/** Function used to do motion compensation with bicubic interpolation
 */
static void vc1_mspel_mc(uint8_t *dst, const uint8_t *src, int stride, int hmode, int vmode, int rnd)
{
    int     i, j;
    if (vmode) 
        { /* Horizontal filter to apply */
          int r;
          if (hmode)
            { /* Vertical filter to apply, output to tmp */
            static const int shift_value[] = { 0, 5, 1, 5 };
            int              shift = (shift_value[hmode]+shift_value[vmode])>>1;
            int16_t          tmp[11*8], *tptr = tmp;

            r = (1<<(shift-1)) + rnd-1;

            src -= 1;
            for(j = 0; j < 8; j++)
               {
                for(i = 0; i < 11; i++)
                    tptr[i] = (vc1_mspel_ver_filter_16bits(src + i, stride, vmode)+r)>>shift;
                src += stride;
                tptr += 11;
               }

            r = 64-rnd;
            tptr = tmp+1;
            for(j = 0; j < 8; j++) 
               {
                for(i = 0; i < 8; i++)
                    dst[i] = av_clip_uint8((vc1_mspel_hor_filter_16bits(tptr + i, 1, hmode)+r)>>7);
                dst += stride;
                tptr += 11;
               }
            return;
           } 
         else 
            { /* No horizontal filter, output 8 lines to dst */
            r = 1-rnd;

            for(j = 0; j < 8; j++) 
                {
                for(i = 0; i < 8; i++)
                    dst[i] = av_clip_uint8(vc1_mspel_filter(src + i, stride, vmode, r));
                src += stride;
                dst += stride;
                }
            return;
             }
    }

    /* Horizontal mode with no vertical mode */
    for(j = 0; j < 8; j++) {
        for(i = 0; i < 8; i++)
            dst[i] = av_clip_uint8(vc1_mspel_filter(src + i, 1, hmode, rnd));
        dst += stride;
        src += stride;
    }
}

/* pixel functions - really are entry points to vc1_mspel_mc */

/* this one is defined in dsputil.c */
void ff_put_vc1_mspel_mc00_c(uint8_t *dst, const uint8_t *src, int stride, int rnd);
/*
#define PUT_VC1_MSPEL(a, b)\
static void put_vc1_mspel_mc ## a ## b ##_c(uint8_t *dst, const uint8_t *src, int stride, int rnd) { \
     vc1_mspel_mc(dst, src, stride, a, b, rnd);                         \
}
*/

//PUT_VC1_MSPEL(1, 0)
#ifdef JZ4740_MXU_OPT
static void put_vc1_mspel_mc10_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{ 
  uint32_t  i,j;
  uint32_t src_aln0,src_aln1,src_rs0,src_rs1,src_rs2,src_rs3;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int ct0,rnd1;
  mul_ct1=0x12121212;   //18 18 18 18
  mul_ct0=0x35353535;   //53 53 53 53
  mul_ct3=0x03030303;   //3  3  3  3
  mul_ct2=0x04040404;   //4  4  4  4
  ct0 = 0x00200020;     //32    32
  rnd1   = rnd * 0x00010001;
  src_aln0 = (((uint32_t)src-1) & 0xFFFFFFFC);
  src_aln1 = (((uint32_t)src+1) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - 1) & 3);
  src_rs1 = src_rs0-1;
  src_rs2 = 4-(((int)src + 1) & 3);
  src_rs3 = src_rs2-1; 
  S32I2M(xr15,mul_ct0); //xr15:18 18 18 18
  S32I2M(xr14,mul_ct1); //xr14:53 53 53 53
  S32I2M(xr11,mul_ct2); //xr11:3  3  3  3
  S32I2M(xr10,mul_ct3); //xr10:4  4  4  4
  S32I2M(xr13,ct0);     //xr13:  32  32
  S32I2M(xr12,rnd1);    //xr12:rnd rnd
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   
   S32LDD(xr5,src_aln1,0);       
   S32LDD(xr6,src_aln1,4);      
   
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[2] src[1] src[0] src[-1]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[3] src[2] src[1] src[0]
   S32ALN(xr7,xr6,xr5,src_rs2);  //xr7:src[4] src[3] src[2] src[1]
   S32ALN(xr8,xr6,xr5,src_rs3);  //xr8:src[5] src[4] src[3] src[2]
   Q8MUL(xr5,xr4,xr15,xr6);   //xr5:18*src[3] 18*src[2] xr6:18*src[1] 18*src[0]
   Q8MAC_SS(xr5,xr3,xr11,xr6);//xr5:18*src[3]-3*src[2] 18*src[2]-3*src[1] xr6:18*src[1]-3*src[0] 18*src[0]-3*src[-1]
   Q8MUL(xr9,xr7,xr14,xr1);  //xr9:53*src[4] 53*src[3] xr1:53*src[2] 53*src[1]
   Q8MAC_SS(xr9,xr8,xr10,xr1); //xr9:53*src[4]-4*src[5] 53*src[3]-4*src[4] xr1:53*src[2]-4*src[3] 53*src[1]-4*src[2]
   Q16ADD_AA_WW(xr0,xr5,xr9,xr5); //xr5:dst[3] 18*src[3]-3*src[2]+53*src[4]-4*src[5] dst[2] 18*src[2]-3*src[1]+53*src[3]-4*src[4]
   Q16ADD_AA_WW(xr0,xr6,xr1,xr6); //xr6:dst[1] 18*src[1]-3*src[0]+53*src[2]-4*src[3] dst[0] 18*src[0]-3*src[-1]+53*src[1]-4src[2]
   Q16ACC_SS(xr5,xr13,xr12,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,6);
   Q16SAT(xr5,xr5,xr6);      //xr5:dst[3]dst[2]dst[1]dst[0]
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[6] src[5] src[4] src[3]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[7] src[6] src[5] src[4]

   S32LDI(xr1,src_aln1,4);
   S32LDD(xr2,src_aln1,4);
  
   S32ALN(xr7,xr2,xr1,src_rs2);  //xr7:src[8] src[7] src[6] src[5]
   S32ALN(xr8,xr2,xr1,src_rs3);  //xr8:src[9] src[8] src[7] src[6]
   Q8MUL(xr1,xr4,xr15,xr2);   //xr1:18*src[7] 18*src[6] xr2:18*src[5] 18*src[4]
   Q8MAC_SS(xr1,xr3,xr11,xr2);//xr1:18*src[7]-3*src[6] 18*src[6]-3*src[5] xr2:18*src[5]-3*src[4] 18*src[4]-3*src[3]

   Q8MUL(xr9,xr7,xr14,xr3);  //xr9:53*src[8] 53*src[7] xr3:53*src[6] 53*src[5]
   Q8MAC_SS(xr9,xr8,xr10,xr3); //xr9:53*src[8]-4*src[9] 53*src[7]-4*src[8] xr3:53*src[6]-4*src[7] 53*src[5]-4*src[6]
   Q16ADD_AA_WW(xr0,xr1,xr9,xr1); //xr1:dst[3] 18*src[7]-3*src[6]+53*src[8]-4*src[9]  dst[2] 18*src[6]-3*src[5]+53*src[7]-4*src[8]

   Q16ADD_AA_WW(xr0,xr2,xr3,xr2); //xr2:dst[1] 18*src[5]-3*src[4]+53*src[6]-4*src[7]  dst[0] 18*src[4]-3*src[3]53+*src[5]-4*src[6]
   Q16ACC_SS(xr1,xr13,xr12,xr2);
   Q16SAR(xr1,xr1,xr2,xr2,6);
   Q16SAT(xr1,xr1,xr2);      //xr1:dst[7]dst[6]dst[5]dst[4]
   S32STD(xr5,dst,0);
   S32STD(xr1,dst,4);
   dst+=stride;
   src_aln0+=(stride-4);
   src_aln1+=(stride-4);
}
}
#else
static void put_vc1_mspel_mc10_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  int  j;
  for(j = 0; j < 8; j++) {
 dst[j*stride+0]=av_clip_uint8((-4 * src[j*stride-1] + 53 * src[j*stride+0] + 18 * src[j*stride+1] - 3 * src[j*stride+2] + 32 - rnd)>>6);
 dst[j*stride+1]=av_clip_uint8((-4 * src[j*stride+0] + 53 * src[j*stride+1] + 18 * src[j*stride+2] - 3 * src[j*stride+3]  + 32 - rnd)>>6);
 dst[j*stride+2]=av_clip_uint8((-4 * src[j*stride+1] + 53 * src[j*stride+2]+ 18 * src[j*stride+3] - 3 * src[j*stride+4] + 32 - rnd)>>6);
 dst[j*stride+3]=av_clip_uint8((-4 * src[j*stride+2] + 53 * src[j*stride+3] + 18 * src[j*stride+4] -3 * src[j*stride+5] + 32 - rnd)>>6);
 dst[j*stride+4]=av_clip_uint8((-4 * src[j*stride+3] + 53 * src[j*stride+4] + 18 * src[j*stride+5] -3 * src[j*stride+6] + 32 - rnd)>>6);
 dst[j*stride+5]=av_clip_uint8((-4 * src[j*stride+4] + 53 * src[j*stride+5]+ 18 * src[j*stride+6] - 3 * src[j*stride+7] + 32 - rnd)>>6);
 dst[j*stride+6]=av_clip_uint8((-4 * src[j*stride+5] + 53 * src[j*stride+6]+ 18 * src[j*stride+7] - 3 * src[j*stride+8] + 32 - rnd)>>6);
 dst[j*stride+7]=av_clip_uint8((-4 * src[j*stride+6] + 53 * src[j*stride+7]+ 18 * src[j*stride+8] - 3 * src[j*stride+9] + 32 - rnd)>>6);
}
}
#endif
/*static void put_vc1_mspel_mc10_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
 vc1_mspel_mc(dst,src,stride,1,0,rnd);
} 
*/
//PUT_VC1_MSPEL(2, 0)
/*static void put_vc1_mspel_mc20_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{

 vc1_mspel_mc(dst,src,stride,2,0,rnd);

} */
#ifdef JZ4740_MXU_OPT
static void put_vc1_mspel_mc20_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  uint32_t  i,j;
  uint32_t src_aln0,src_aln1,src_rs0,src_rs1,src_rs2,src_rs3;
  int mul_ct0,mul_ct1;
  int ct0,rnd1;
  mul_ct0=0x09090909;   //9 9 9 9
  mul_ct1=0x01010101;   //1 1 1 1
  ct0 = 0x00080008;
  rnd1   = rnd * 0x00010001;
  src_aln0 = (((uint32_t)src-1) & 0xFFFFFFFC);
  src_aln1 = (((uint32_t)src+1) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - 1) & 3);
  src_rs1 = src_rs0-1;
  src_rs2 = 4-(((int)src + 1) & 3);
  src_rs3 = src_rs2-1; 
  S32I2M(xr15,mul_ct0); //xr15:9 9 9 9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr13,ct0);     //xr13:  8   8
  S32I2M(xr12,rnd1);     //xr12:rnd rnd
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   
   S32LDD(xr5,src_aln1,0);       
   S32LDD(xr6,src_aln1,4);      
   
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[2] src[1] src[0] src[-1]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[3] src[2] src[1] src[0]
   S32ALN(xr7,xr6,xr5,src_rs2);  //xr7:src[4] src[3] src[2] src[1]
   S32ALN(xr8,xr6,xr5,src_rs3);  //xr8:src[5] src[4] src[3] src[2]
   Q8MUL(xr5,xr4,xr15,xr6);   //xr5:9*src[3] 9*src[2] xr6:9*src[1] 9*src[0]
   Q8MAC_SS(xr5,xr3,xr14,xr6);//xr5:9*src[3]-src[2] 9*src[2]-src[1] xr6:9*src[1]-src[0] 9*src[0]-src[-1]
   Q8MUL(xr9,xr7,xr15,xr10);  //xr9:9*src[4] 9*src[3] xr10:9*src[2] 9*src[1]
   Q8MAC_SS(xr9,xr8,xr14,xr10); //xr9:9*src[4]-src[5] 9*src[3]-src[4] xr10:9*src[2]-src[3] 9*src[1]-src[2]
   Q16ADD_AA_WW(xr0,xr5,xr9,xr5); //xr5:dst[3] 9*src[3]-src[2]+9*src[4]-src[5] dst[2] 9*src[2]-src[1]+9*src[3]-src[4]
   Q16ADD_AA_WW(xr0,xr6,xr10,xr6); //xr6:dst[1] 9*src[1]-src[0]+9*src[2]-src[3] dst[0] 9*src[0]-src[-1]+9*src[1]-src[2]
   Q16ACC_SS(xr5,xr13,xr12,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,4);
   Q16SAT(xr5,xr5,xr6);      //xr5:dst[3]dst[2]dst[1]dst[0]
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[6] src[5] src[4] src[3]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[7] src[6] src[5] src[4]

   S32LDI(xr1,src_aln1,4);
   S32LDD(xr2,src_aln1,4);
   S32ALN(xr7,xr2,xr1,src_rs2);  //xr7:src[8] src[7] src[6] src[5]
   S32ALN(xr8,xr2,xr1,src_rs3);  //xr8:src[9] src[8] src[7] src[6]
   Q8MUL(xr1,xr4,xr15,xr2);   //xr5:9*src[3] 9*src[2] xr6:9*src[1] 9*src[0]
   Q8MAC_SS(xr1,xr3,xr14,xr2);//xr5:9*src[3]-src[2] 9*src[2]-src[1] xr6:9*src[1]-src[0] 9*src[0]-src[-1]

   Q8MUL(xr9,xr7,xr15,xr10);  //xr9:9*src[4] 9*src[3] xr10:9*src[2] 9*src[1]
   Q8MAC_SS(xr9,xr8,xr14,xr10); //xr9:9*src[4]-src[5] 9*src[3]-src[4] xr10:9*src[2]-src[3] 9*src[1]-src[2]
   Q16ADD_AA_WW(xr0,xr1,xr9,xr1); //xr5:dst[3] 9*src[3]-src[2]+9*src[4]-src[5] dst[2] 9*src[2]-src[1]+9*src[3]-src[4]

   Q16ADD_AA_WW(xr0,xr2,xr10,xr2); //xr6:dst[1] 9*src[1]-src[0]+9*src[2]-src[3] dst[0] 9*src[0]-src[-1]+9*src[1]-src[2]
   Q16ACC_SS(xr1,xr13,xr12,xr2);
   Q16SAR(xr1,xr1,xr2,xr2,4);
   Q16SAT(xr1,xr1,xr2);      //xr5:dst[3]dst[2]dst[1]dst[0]
   S32STD(xr5,dst,0);
   S32STD(xr1,dst,4);
   dst+=stride;
   src_aln0+=(stride-4);
   src_aln1+=(stride-4);
}

}
#else
static void put_vc1_mspel_mc20_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{

int i,j;
for(j = 0; j < 8; j++) 
   {
       dst[0]=av_clip_uint8((-src[-1] + 9*src[0] + 9*src[1] - src[2] + 8 - rnd) >> 4);
       dst[1]=av_clip_uint8((-src[0]  + 9*src[1] + 9*src[2] - src[3] + 8 - rnd) >> 4);
       dst[2]=av_clip_uint8((-src[1]  + 9*src[2] + 9*src[3] - src[4] + 8 - rnd) >> 4);
       dst[3]=av_clip_uint8((-src[2]  + 9*src[3] + 9*src[4] - src[5] + 8 - rnd) >> 4);
       dst[4]=av_clip_uint8((-src[3]  + 9*src[4] + 9*src[5] - src[6] + 8 - rnd) >> 4);
       dst[5]=av_clip_uint8((-src[4]  + 9*src[5] + 9*src[6] - src[7] + 8 - rnd) >> 4);
       dst[6]=av_clip_uint8((-src[5]  + 9*src[6] + 9*src[7] - src[8] + 8 - rnd) >> 4);
       dst[7]=av_clip_uint8((-src[6]  + 9*src[7] + 9*src[8] - src[9] + 8 - rnd) >> 4);
       dst += stride;
       src += stride;
   }

}
#endif
 
//PUT_VC1_MSPEL(3, 0)
/*static void put_vc1_mspel_mc30_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,3,0,rnd);
} */

#ifdef JZ4740_MXU_OPT
static void put_vc1_mspel_mc30_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  uint32_t  i,j;
  uint32_t src_aln0,src_aln1,src_rs0,src_rs1,src_rs2,src_rs3;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int ct0,rnd1;
  mul_ct0=0x12121212;   //18 18 18 18
  mul_ct1=0x35353535;   //53 53 53 53
  mul_ct2=0x03030303;   //3  3  3  3
  mul_ct3=0x04040404;   //4  4  4  4
  ct0 = 0x00200020;     //32    32
  rnd1   = rnd * 0x00010001;
  src_aln0 = (((uint32_t)src-1) & 0xFFFFFFFC);
  src_aln1 = (((uint32_t)src+1) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - 1) & 3);
  src_rs1 = src_rs0-1;
  src_rs2 = 4-(((int)src + 1) & 3);
  src_rs3 = src_rs2-1; 
  S32I2M(xr15,mul_ct0); //xr15:18 18 18 18
  S32I2M(xr14,mul_ct1); //xr14:53 53 53 53
  S32I2M(xr11,mul_ct2); //xr11:3  3  3  3
  S32I2M(xr10,mul_ct3); //xr10:4  4  4  4
  S32I2M(xr13,ct0);     //xr13:  32  32
  S32I2M(xr12,rnd1);    //xr12:rnd rnd
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   
   S32LDD(xr5,src_aln1,0);       
   S32LDD(xr6,src_aln1,4);      
  
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[2] src[1] src[0] src[-1]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[3] src[2] src[1] src[0]
   S32ALN(xr7,xr6,xr5,src_rs2);  //xr7:src[4] src[3] src[2] src[1]
   S32ALN(xr8,xr6,xr5,src_rs3);  //xr8:src[5] src[4] src[3] src[2]
   Q8MUL(xr5,xr4,xr15,xr6);   //xr5:18*src[3] 18*src[2] xr6:18*src[1] 18*src[0]
   Q8MAC_SS(xr5,xr3,xr11,xr6);//xr5:18*src[3]-3*src[2] 18*src[2]-3*src[1] xr6:18*src[1]-3*src[0] 18*src[0]-3*src[-1]
   Q8MUL(xr9,xr7,xr14,xr1);  //xr9:53*src[4] 53*src[3] xr1:53*src[2] 53*src[1]
   Q8MAC_SS(xr9,xr8,xr10,xr1); //xr9:53*src[4]-4*src[5] 53*src[3]-4*src[4] xr1:53*src[2]-4*src[3] 53*src[1]-4*src[2]
   Q16ADD_AA_WW(xr0,xr5,xr9,xr5); //xr5:dst[3] 18*src[3]-3*src[2]+53*src[4]-4*src[5] dst[2] 18*src[2]-3*src[1]+53*src[3]-4*src[4]
   Q16ADD_AA_WW(xr0,xr6,xr1,xr6); //xr6:dst[1] 18*src[1]-3*src[0]+53*src[2]-4*src[3] dst[0] 18*src[0]-3*src[-1]+53*src[1]-4src[2]
   Q16ACC_SS(xr5,xr13,xr12,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,6);
   Q16SAT(xr5,xr5,xr6);      //xr5:dst[3]dst[2]dst[1]dst[0]
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   S32ALN(xr3,xr2,xr1,src_rs0);  //xr3:src[6] src[5] src[4] src[3]
   S32ALN(xr4,xr2,xr1,src_rs1);  //xr4:src[7] src[6] src[5] src[4]

   S32LDI(xr1,src_aln1,4);
   S32LDD(xr2,src_aln1,4);
   S32ALN(xr7,xr2,xr1,src_rs2);  //xr7:src[8] src[7] src[6] src[5]
   S32ALN(xr8,xr2,xr1,src_rs3);  //xr8:src[9] src[8] src[7] src[6]
   Q8MUL(xr1,xr4,xr15,xr2);   //xr1:18*src[7] 18*src[6] xr2:18*src[5] 18*src[4]
   Q8MAC_SS(xr1,xr3,xr11,xr2);//xr1:18*src[7]-3*src[6] 18*src[6]-3*src[5] xr2:18*src[5]-3*src[4] 18*src[4]-3*src[3]

   Q8MUL(xr9,xr7,xr14,xr3);  //xr9:53*src[8] 53*src[7] xr3:53*src[6] 53*src[5]
   Q8MAC_SS(xr9,xr8,xr10,xr3); //xr9:53*src[8]-4*src[9] 53*src[7]-4*src[8] xr3:53*src[6]-4*src[7] 53*src[5]-4*src[6]
   Q16ADD_AA_WW(xr0,xr1,xr9,xr1); //xr1:dst[3] 18*src[7]-3*src[6]+53*src[8]-4*src[9]  dst[2] 18*src[6]-3*src[5]+53*src[7]-4*src[8]

   Q16ADD_AA_WW(xr0,xr2,xr3,xr2); //xr2:dst[1] 18*src[5]-3*src[4]+53*src[6]-4*src[7]  dst[0] 18*src[4]-3*src[3]53+*src[5]-4*src[6]
   Q16ACC_SS(xr1,xr13,xr12,xr2);
   Q16SAR(xr1,xr1,xr2,xr2,6);
   Q16SAT(xr1,xr1,xr2);      //xr1:dst[7]dst[6]dst[5]dst[4]
   
   S32STD(xr5,dst,0);
   S32STD(xr1,dst,4);
   dst+=stride;
   src_aln0+=(stride-4);
   src_aln1+=(stride-4);
}
}
#else
static void put_vc1_mspel_mc30_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
int j;
for(j = 0; j < 8; j++) 
   {
       dst[0]=av_clip_uint8((-3*src[-1]+ 18*src[0] + 53*src[1] - 4*src[2] + 32 - rnd) >> 6);
       dst[1]=av_clip_uint8((-3*src[0] + 18*src[1] + 53*src[2] - 4*src[3] + 32 - rnd) >> 6);
       dst[2]=av_clip_uint8((-3*src[1] + 18*src[2] + 53*src[3] - 4*src[4] + 32 - rnd) >> 6);
       dst[3]=av_clip_uint8((-3*src[2] + 18*src[3] + 53*src[4] - 4*src[5] + 32 - rnd) >> 6);
       dst[4]=av_clip_uint8((-3*src[3] + 18*src[4] + 53*src[5] - 4*src[6] + 32 - rnd) >> 6);
       dst[5]=av_clip_uint8((-3*src[4] + 18*src[5] + 53*src[6] - 4*src[7] + 32 - rnd) >> 6);
       dst[6]=av_clip_uint8((-3*src[5] + 18*src[6] + 53*src[7] - 4*src[8] + 32 - rnd) >> 6);
       dst[7]=av_clip_uint8((-3*src[6] + 18*src[7] + 53*src[8] - 4*src[9] + 32 - rnd) >> 6);
       dst += stride;
       src += stride;
   }
}
#endif
 
//PUT_VC1_MSPEL(0, 1)
/*static void put_vc1_mspel_mc01_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,0,1,rnd);
} */
#ifdef JZ4740_MXU_OPT
static void put_vc1_mspel_mc01_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  uint32_t  j,r;
  r = 1-rnd;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int ct0,rnd1;
  mul_ct1=0x12121212;   //18 18 18 18
  mul_ct0=0x35353535;   //53 53 53 53
  mul_ct3=0x03030303;   //3  3  3  3
  mul_ct2=0x04040404;   //4  4  4  4
  ct0 = 0x00200020;     //32    32
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr13,ct0);     //xr13:  32  32
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
  
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_SS(xr5,xr13,xr12,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,6);
   Q16SAT(xr5,xr5,xr6);      //xr5:dst[3]dst[2]dst[1]dst[0]
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr6);   
   Q8MAC_SS(xr9,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_SS(xr9,xr13,xr12,xr6); //two instructions with one
   Q16SAR(xr9,xr9,xr6,xr6,6);
   Q16SAT(xr9,xr9,xr6);      //xr5:dst[3]dst[2]dst[1]dst[0]
  
   S32STD(xr5,dst,0);
   S32STD(xr9,dst,4);
   dst+=stride;
   src_aln0+=(-2*stride-4);
  }
}
#else 
static void put_vc1_mspel_mc01_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{   int j,r;
    r = 1-rnd;
    for(j = 0; j < 8; j++) 
       {
     dst[0] = av_clip_uint8((-4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2] + 32 - r) >> 6);   
     dst[1] = av_clip_uint8((-4*src[-stride+1] + 53*src[1] + 18*src[stride+1] - 3*src[stride*2+1] + 32 - r) >> 6);   
     dst[2] = av_clip_uint8((-4*src[-stride+2] + 53*src[2] + 18*src[stride+2] - 3*src[stride*2+2] + 32 - r) >> 6);       
     dst[3] = av_clip_uint8((-4*src[-stride+3] + 53*src[3] + 18*src[stride+3] - 3*src[stride*2+3] + 32 - r) >> 6);       
     dst[4] = av_clip_uint8((-4*src[-stride+4] + 53*src[4] + 18*src[stride+4] - 3*src[stride*2+4] + 32 - r) >> 6);       
     dst[5] = av_clip_uint8((-4*src[-stride+5] + 53*src[5] + 18*src[stride+5] - 3*src[stride*2+5] + 32 - r) >> 6);       
     dst[6] = av_clip_uint8((-4*src[-stride+6] + 53*src[6] + 18*src[stride+6] - 3*src[stride*2+6] + 32 - r) >> 6);       
     dst[7] = av_clip_uint8((-4*src[-stride+7] + 53*src[7] + 18*src[stride+7] - 3*src[stride*2+7] + 32 - r) >> 6);       
     src += stride;
     dst += stride;
      }
}
#endif
//PUT_VC1_MSPEL(1, 1)
/*
static void put_vc1_mspel_mc11_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,1,1,rnd);
} */
 
#ifdef JZ4740_MXU_OPT
#define w1 53
#define w2 4
#define w3 18
#define w4 3
static void put_vc1_mspel_mc11_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{ 
 int j,r;
 int tmp[12*4];
 int16_t *tptr = tmp; 
  r = 15 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct1=0x12121212;   //18 18 18 18
  mul_ct0=0x35353535;   //53 53 53 53
  mul_ct3=0x03030303;   //3  3  3  3
  mul_ct2=0x04040404;   //4  4  4  4
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,5);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
    r = 64-rnd;
    tptr = tmp;
   
    S32I2M(xr15,(w1<<16)|w2); //xr15:53|4
    S32I2M(xr14,(w3<<16)|w4); //xr14:18|3
    S32I2M(xr13,r);
    for(j=0;j<8;j++)
    {
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]
    
    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:53*tptr[2] xr10:53*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:53*tptr[2]-4*tptr[1] xr10:53*tptr[1]-4*tptr[0]
    D16MAC_AA_HW(xr9,xr14,xr2,xr10);//xr9:53*tptr[2]-4*tptr[1]+18*tptr[3] xr10:53*tptr[1]-4*tptr[0]+18*tptr[2]
    D16MAC_SS_LW(xr9,xr14,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr14,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr14,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);
    
    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]
     
    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr14,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr14,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);
     
    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr14,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr14,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);
   
    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }
   
}
#else 
static void put_vc1_mspel_mc11_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{ 
    static const int shift_value[] = { 0, 5, 1, 5 };
     int              shift = (shift_value[1]+shift_value[1])>>1;
    int              tmp[12*4];
    int16_t          *tptr = tmp;
    int j,r;
    r = (1<<(shift-1)) + rnd-1;
    src -= 1;
    for(j = 0; j < 8; j++)
     {
        tptr[0]=(-4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2]+r)>>shift;
        tptr[1]=(-4*src[-stride+1] + 53*src[1] + 18*src[stride+1] - 3*src[stride*2+1]+r)>>shift;
        tptr[2]=(-4*src[-stride+2] + 53*src[2] + 18*src[stride+2] - 3*src[stride*2+2]+r)>>shift;
        tptr[3]=(-4*src[-stride+3] + 53*src[3] + 18*src[stride+3] - 3*src[stride*2+3]+r)>>shift;
        tptr[4]=(-4*src[-stride+4] + 53*src[4] + 18*src[stride+4] - 3*src[stride*2+4]+r)>>shift;
        tptr[5]=(-4*src[-stride+5] + 53*src[5] + 18*src[stride+5] - 3*src[stride*2+5]+r)>>shift;
        tptr[6]=(-4*src[-stride+6] + 53*src[6] + 18*src[stride+6] - 3*src[stride*2+6]+r)>>shift;
        tptr[7]=(-4*src[-stride+7] + 53*src[7] + 18*src[stride+7] - 3*src[stride*2+7]+r)>>shift;
        tptr[8]=(-4*src[-stride+8] + 53*src[8] + 18*src[stride+8] - 3*src[stride*2+8]+r)>>shift;
        tptr[9]=(-4*src[-stride+9] + 53*src[9] + 18*src[stride+9] - 3*src[stride*2+9]+r)>>shift;
        tptr[10]=(-4*src[-stride+10] + 53*src[10] + 18*src[stride+10] - 3*src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 12;
     }
    r = 64-rnd;
    tptr = tmp;
   for(j = 0; j < 8; j++)
    { 
        dst[0]=av_clip_uint8((-4*tptr[0] + 53*tptr[1] + 18*tptr[2] - 3*tptr[3]+r)>>7);
        dst[1]=av_clip_uint8((-4*tptr[1] + 53*tptr[2] + 18*tptr[3] - 3*tptr[4]+r)>>7);
        dst[2]=av_clip_uint8((-4*tptr[2] + 53*tptr[3] + 18*tptr[4] - 3*tptr[5]+r)>>7);
        dst[3]=av_clip_uint8((-4*tptr[3] + 53*tptr[4] + 18*tptr[5] - 3*tptr[6]+r)>>7);
        dst[4]=av_clip_uint8((-4*tptr[4] + 53*tptr[5] + 18*tptr[6] - 3*tptr[7]+r)>>7);
        dst[5]=av_clip_uint8((-4*tptr[5] + 53*tptr[6] + 18*tptr[7] - 3*tptr[8]+r)>>7);
        dst[6]=av_clip_uint8((-4*tptr[6] + 53*tptr[7] + 18*tptr[8] - 3*tptr[9]+r)>>7);
        dst[7]=av_clip_uint8((-4*tptr[7] + 53*tptr[8] + 18*tptr[9] - 3*tptr[10]+r)>>7);
       
        dst += stride;
        tptr += 12;
    }   
}
#endif
//PUT_VC1_MSPEL(2, 1)
/*
static void put_vc1_mspel_mc21_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,2,1,rnd);
} 
*/
#ifdef JZ4740_MXU_OPT
#define w5 9
#define w6 1
static void put_vc1_mspel_mc21_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  int j,r;
  int  tmp[12*4];
  int16_t  *tptr = tmp;
  r = 3 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct1=0x12121212;   //18 18 18 18
  mul_ct0=0x35353535;   //53 53 53 53
  mul_ct3=0x03030303;   //3  3  3  3
  mul_ct2=0x04040404;   //4  4  4  4
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,3);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
  r = 64-rnd;
  tptr = tmp;

  S32I2M(xr15,(w5<<16)|w6); //xr15:9|1
  S32I2M(xr13,r);
  for(j=0;j<8;j++)
    {
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]

    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:9*tptr[2] xr10:9*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:9*tptr[2]-1*tptr[1] xr10:9*tptr[1]-1*tptr[0]
    D16MAC_AA_HW(xr9,xr15,xr2,xr10);//xr9:9*tptr[2]-1*tptr[1]+9*tptr[3] xr10:9*tptr[1]-1*tptr[0]+9*tptr[2]
    D16MAC_SS_LW(xr9,xr15,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr15,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr15,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);

    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]

    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr15,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr15,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);

    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr15,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr15,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);

    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }
}
#else
static void put_vc1_mspel_mc21_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
   static const int shift_value[] = { 0, 5, 1, 5 };
   int              shift = (shift_value[2]+shift_value[1])>>1;
   int16_t          tmp[11*8], *tptr = tmp;
   int j,r;
   r = (1<<(shift-1)) + rnd-1;
   src -= 1;
   for(j = 0; j < 8; j++)
   {
    tptr[0]=(-4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2]+r)>>shift;
    tptr[1]=(-4*src[-stride+1] + 53*src[1] + 18*src[stride+1] - 3*src[stride*2+1]+r)>>shift;
    tptr[2]=(-4*src[-stride+2] + 53*src[2] + 18*src[stride+2] - 3*src[stride*2+2]+r)>>shift;
    tptr[3]=(-4*src[-stride+3] + 53*src[3] + 18*src[stride+3] - 3*src[stride*2+3]+r)>>shift;
    tptr[4]=(-4*src[-stride+4] + 53*src[4] + 18*src[stride+4] - 3*src[stride*2+4]+r)>>shift;
    tptr[5]=(-4*src[-stride+5] + 53*src[5] + 18*src[stride+5] - 3*src[stride*2+5]+r)>>shift;
    tptr[6]=(-4*src[-stride+6] + 53*src[6] + 18*src[stride+6] - 3*src[stride*2+6]+r)>>shift;
    tptr[7]=(-4*src[-stride+7] + 53*src[7] + 18*src[stride+7] - 3*src[stride*2+7]+r)>>shift;
    tptr[8]=(-4*src[-stride+8] + 53*src[8] + 18*src[stride+8] - 3*src[stride*2+8]+r)>>shift;
    tptr[9]=(-4*src[-stride+9] + 53*src[9] + 18*src[stride+9] - 3*src[stride*2+9]+r)>>shift;
    tptr[10]=(-4*src[-stride+10] + 53*src[10] + 18*src[stride+10] - 3*src[stride*2+10]+r)>>shift;
    src += stride;
    tptr += 11;
   }
   r = 64-rnd;
   tptr = tmp+1;
   for(j = 0; j < 8; j++)
     {
     dst[0] = av_clip_uint8((-tptr[-1] + 9*tptr[0] + 9*tptr[1] - tptr[2]+r)>>7);
     dst[1] = av_clip_uint8((-tptr[0] + 9*tptr[1] + 9*tptr[2] - tptr[3]+r)>>7);
     dst[2] = av_clip_uint8((-tptr[1] + 9*tptr[2] + 9*tptr[3] - tptr[4]+r)>>7);
     dst[3] = av_clip_uint8((-tptr[2] + 9*tptr[3] + 9*tptr[4] - tptr[5]+r)>>7);
     dst[4] = av_clip_uint8((-tptr[3] + 9*tptr[4] + 9*tptr[5] - tptr[6]+r)>>7);
     dst[5] = av_clip_uint8((-tptr[4] + 9*tptr[5] + 9*tptr[6] - tptr[7]+r)>>7);
     dst[6] = av_clip_uint8((-tptr[5] + 9*tptr[6] + 9*tptr[7] - tptr[8]+r)>>7);
     dst[7] = av_clip_uint8((-tptr[6] + 9*tptr[7] + 9*tptr[8] - tptr[9]+r)>>7);
     dst += stride;
     tptr += 11;
     }
}

#endif 
//PUT_VC1_MSPEL(3, 1)
/*
static void put_vc1_mspel_mc31_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,3,1,rnd);
} 
*/
#ifdef JZ4740_MXU_OPT
#define w1 18
#define w2 3
#define w3 53
#define w4 4
static void put_vc1_mspel_mc31_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
 int j,r;
 int  tmp[12*4];
 int16_t  *tptr = tmp;
  r = 15 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct1=0x12121212;   //18 18 18 18
  mul_ct0=0x35353535;   //53 53 53 53
  mul_ct3=0x03030303;   //3  3  3  3
  mul_ct2=0x04040404;   //4  4  4  4
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,5);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);   
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
    r = 64-rnd;
    tptr = tmp;
 
    S32I2M(xr15,(w1<<16)|w2); //xr15:18|3
    S32I2M(xr14,(w3<<16)|w4); //xr14:53|4
    S32I2M(xr13,r);
    for(j=0;j<8;j++)
    {    
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]
    
    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:53*tptr[2] xr10:53*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:53*tptr[2]-4*tptr[1] xr10:53*tptr[1]-4*tptr[0]
    D16MAC_AA_HW(xr9,xr14,xr2,xr10);//xr9:53*tptr[2]-4*tptr[1]+18*tptr[3] xr10:53*tptr[1]-4*tptr[0]+18*tptr[2]
    D16MAC_SS_LW(xr9,xr14,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr14,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr14,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);
    
    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]
     
    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr14,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr14,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);
     
    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr14,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr14,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);
   
    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }

}
#else
static void put_vc1_mspel_mc31_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
   int r,j;
   static const int shift_value[] = { 0, 5, 1, 5 };
   int              shift = (shift_value[3]+shift_value[1])>>1;
   int16_t          tmp[11*8], *tptr = tmp;
   r = (1<<(shift-1)) + rnd-1;
   src -= 1;
   for(j = 0; j < 8; j++)
   {
    tptr[0]=(-4*src[-stride] + 53*src[0] + 18*src[stride] - 3*src[stride*2]+r)>>shift;
    tptr[1]=(-4*src[-stride+1] + 53*src[1] + 18*src[stride+1] - 3*src[stride*2+1]+r)>>shift;
    tptr[2]=(-4*src[-stride+2] + 53*src[2] + 18*src[stride+2] - 3*src[stride*2+2]+r)>>shift;
    tptr[3]=(-4*src[-stride+3] + 53*src[3] + 18*src[stride+3] - 3*src[stride*2+3]+r)>>shift;
    tptr[4]=(-4*src[-stride+4] + 53*src[4] + 18*src[stride+4] - 3*src[stride*2+4]+r)>>shift;
    tptr[5]=(-4*src[-stride+5] + 53*src[5] + 18*src[stride+5] - 3*src[stride*2+5]+r)>>shift;
    tptr[6]=(-4*src[-stride+6] + 53*src[6] + 18*src[stride+6] - 3*src[stride*2+6]+r)>>shift;
    tptr[7]=(-4*src[-stride+7] + 53*src[7] + 18*src[stride+7] - 3*src[stride*2+7]+r)>>shift;
    tptr[8]=(-4*src[-stride+8] + 53*src[8] + 18*src[stride+8] - 3*src[stride*2+8]+r)>>shift;
    tptr[9]=(-4*src[-stride+9] + 53*src[9] + 18*src[stride+9] - 3*src[stride*2+9]+r)>>shift;
    tptr[10]=(-4*src[-stride+10] + 53*src[10] + 18*src[stride+10] - 3*src[stride*2+10]+r)>>shift;
    src += stride;
    tptr += 11;
   }
    r = 64-rnd;
    tptr = tmp+1;
   for(j = 0; j < 8; j++)
   {
    dst[0] = av_clip_uint8((-3*tptr[-1] + 18*tptr[0] + 53*tptr[1] - 4*tptr[2]+r)>>7);
    dst[1] = av_clip_uint8((-3*tptr[0] + 18*tptr[1] + 53*tptr[2] - 4*tptr[3]+r)>>7);
    dst[2] = av_clip_uint8((-3*tptr[1] + 18*tptr[2] + 53*tptr[3] - 4*tptr[4]+r)>>7);
    dst[3] = av_clip_uint8((-3*tptr[2] + 18*tptr[3] + 53*tptr[4] - 4*tptr[5]+r)>>7);
    dst[4] = av_clip_uint8((-3*tptr[3] + 18*tptr[4] + 53*tptr[5] - 4*tptr[6]+r)>>7);
    dst[5] = av_clip_uint8((-3*tptr[4] + 18*tptr[5] + 53*tptr[6] - 4*tptr[7]+r)>>7);
    dst[6] = av_clip_uint8((-3*tptr[5] + 18*tptr[6] + 53*tptr[7] - 4*tptr[8]+r)>>7);
    dst[7] = av_clip_uint8((-3*tptr[6] + 18*tptr[7] + 53*tptr[8] - 4*tptr[9]+r)>>7);
    dst += stride;
    tptr += 11;
    }
 } 
#endif
//PUT_VC1_MSPEL(0, 2)
/*
static void put_vc1_mspel_mc02_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,0,2,rnd);
} 
*/
#ifdef JZ4740_MXU_OPT
static void put_vc1_mspel_mc02_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
 {
   int r,j;
   r = 1-rnd;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2;
  int rnd1;
  mul_ct0=0x09090909;   //9 9 9 9
  mul_ct1=0x01010101;   //1 1 1 1
  mul_ct2=0x00080008;   //8   8
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:9 9 9 9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr13,mul_ct2); //xr13:  8   8
  S32I2M(xr12,rnd1);    //xr12:  r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr15,xr6);   
   Q8MAC_SS(xr5,xr8,xr14,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr13,xr12,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,4);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   Q16SAT(xr5,xr5,xr6);
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr11,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr11,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr11,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr11,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr11);   
   Q8MAC_SS(xr9,xr8,xr14,xr11); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr11,xr2,xr11); 
  
   Q16ACC_SS(xr9,xr13,xr12,xr11); //two instructions with one
   Q16SAR(xr9,xr9,xr11,xr11,4);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   Q16SAT(xr9,xr9,xr11);
   S32STD(xr5,dst,0);
   S32STD(xr9,dst,4);
   dst += stride;
   src_aln0+=(-2*stride-4);
  }

 }
#else
static void put_vc1_mspel_mc02_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  int r,j;
  r = 1-rnd;
  for(j = 0; j < 8; j++)
    {
     dst[0]=av_clip_uint8((-src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2] + 8 - r) >> 4);    
     dst[1]=av_clip_uint8((-src[-stride+1] + 9*src[1] + 9*src[stride+1] - src[stride*2+1] + 8 - r) >> 4);    
     dst[2]=av_clip_uint8((-src[-stride+2] + 9*src[2] + 9*src[stride+2] - src[stride*2+2] + 8 - r) >> 4);    
     dst[3]=av_clip_uint8((-src[-stride+3] + 9*src[3] + 9*src[stride+3] - src[stride*2+3] + 8 - r) >> 4);    
     dst[4]=av_clip_uint8((-src[-stride+4] + 9*src[4] + 9*src[stride+4] - src[stride*2+4] + 8 - r) >> 4);    
     dst[5]=av_clip_uint8((-src[-stride+5] + 9*src[5] + 9*src[stride+5] - src[stride*2+5] + 8 - r) >> 4);    
     dst[6]=av_clip_uint8((-src[-stride+6] + 9*src[6] + 9*src[stride+6] - src[stride*2+6] + 8 - r) >> 4);    
     dst[7]=av_clip_uint8((-src[-stride+7] + 9*src[7] + 9*src[stride+7] - src[stride*2+7] + 8 - r) >> 4);    
     src += stride;
     dst += stride;
    }
}
#endif
//PUT_VC1_MSPEL(1, 2)
/*
static void put_vc1_mspel_mc12_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,1,2,rnd);
} 
*/
#ifdef JZ4740_MXU_OPT
#define w1 53
#define w2 4
#define w3 18
#define w4 3
static void put_vc1_mspel_mc12_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  int j,r;
  int  tmp[12*4];
  int16_t  *tptr = tmp;
  r = 3 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct0=0x09090909;   //9  9  9  9
  mul_ct1=0x01010101;   //1  1  1  1
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:9  9  9  9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr15,xr6);   
   Q8MAC_SS(xr5,xr8,xr14,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,3);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr13);   
   Q8MAC_SS(xr9,xr8,xr14,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);   

   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr13);   
   Q8MAC_SS(xr9,xr8,xr14,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
      r = 64-rnd;
      tptr = tmp;

    S32I2M(xr15,(w1<<16)|w2); //xr15:53|4
    S32I2M(xr14,(w3<<16)|w4); //xr14:18|3
    S32I2M(xr13,r);
    S32I2M(xr12,0x20);
    for(j=0;j<8;j++)
    {
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]
    
    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:53*tptr[2] xr10:53*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:53*tptr[2]-4*tptr[1] xr10:53*tptr[1]-4*tptr[0]
    D16MAC_AA_HW(xr9,xr14,xr2,xr10);//xr9:53*tptr[2]-4*tptr[1]+18*tptr[3] xr10:53*tptr[1]-4*tptr[0]+18*tptr[2]
    D16MAC_SS_LW(xr9,xr14,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr12,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr14,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr14,xr7,xr10); //

    D32ACC_AA(xr11,xr12,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);
    
    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]
     
    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr14,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr14,xr6,xr8); //

    D32ACC_AA(xr9,xr12,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);
     
    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr14,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr14,xr5,xr8); //

    D32ACC_AA(xr11,xr12,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);
   
    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }
}
#else
static void put_vc1_mspel_mc12_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
     int r,j;
     static const int shift_value[] = { 0, 5, 1, 5 };
     int              shift = (shift_value[1]+shift_value[2])>>1;
     int16_t          tmp[11*8], *tptr = tmp;
     r = (1<<(shift-1)) + rnd-1;
     src -= 1;
     for(j = 0; j < 8; j++)
       {
        tptr[0]= (-src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2]+r)>>shift; 
        tptr[1]= (-src[-stride+1] + 9*src[1] + 9*src[stride+1] - src[stride*2+1]+r)>>shift;
        tptr[2]= (-src[-stride+2] + 9*src[2] + 9*src[stride+2] - src[stride*2+2]+r)>>shift;
        tptr[3]= (-src[-stride+3] + 9*src[3] + 9*src[stride+3] - src[stride*2+3]+r)>>shift;
        tptr[4]= (-src[-stride+4] + 9*src[4] + 9*src[stride+4] - src[stride*2+4]+r)>>shift;
        tptr[5]= (-src[-stride+5] + 9*src[5] + 9*src[stride+5] - src[stride*2+5]+r)>>shift;
        tptr[6]= (-src[-stride+6] + 9*src[6] + 9*src[stride+6] - src[stride*2+6]+r)>>shift;
        tptr[7]= (-src[-stride+7] + 9*src[7] + 9*src[stride+7] - src[stride*2+7]+r)>>shift;
        tptr[8]= (-src[-stride+8]  + 9*src[8] + 9*src[stride+8] - src[stride*2+8]+r)>>shift;
        tptr[9]= (-src[-stride+9] + 9*src[9] + 9*src[stride+9] - src[stride*2+9]+r)>>shift;
        tptr[10]= (-src[-stride+10] + 9*src[10] + 9*src[stride+10] - src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 11;
       }

      r = 64-rnd;
      tptr = tmp+1;
     for(j = 0; j < 8; j++)
        {
        dst[0]=av_clip_uint8((-4*tptr[-1] + 53*tptr[0] + 18*tptr[1] - 3*tptr[2] + 32 - r) >> 7); 
        dst[1]=av_clip_uint8((-4*tptr[0] + 53*tptr[1] + 18*tptr[2] - 3*tptr[3] + 32 - r) >> 7);
        dst[2]=av_clip_uint8((-4*tptr[1] + 53*tptr[2] + 18*tptr[3] - 3*tptr[4] + 32 - r) >> 7);
        dst[3]=av_clip_uint8((-4*tptr[2] + 53*tptr[3] + 18*tptr[4] - 3*tptr[5] + 32 - r) >> 7);
        dst[4]=av_clip_uint8((-4*tptr[3] + 53*tptr[4] + 18*tptr[5] - 3*tptr[6] + 32 - r) >> 7);
        dst[5]=av_clip_uint8((-4*tptr[4] + 53*tptr[5] + 18*tptr[6] - 3*tptr[7] + 32 - r) >> 7);
        dst[6]=av_clip_uint8((-4*tptr[5] + 53*tptr[6] + 18*tptr[7] - 3*tptr[8] + 32 - r) >> 7);
        dst[7]=av_clip_uint8((-4*tptr[6] + 53*tptr[7] + 18*tptr[8] - 3*tptr[9] + 32 - r) >> 7);
        dst += stride;
        tptr += 11;
        }
}
#endif 
//PUT_VC1_MSPEL(2, 2)
/*
static void put_vc1_mspel_mc22_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,2,2,rnd);
} 
*/
#ifdef JZ4740_MXU_OPT
#define w5 9
#define w6 1
static void put_vc1_mspel_mc22_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
        
  int j,r;
  int tmp[12*4];
  int16_t  *tptr = tmp;
  r = 1 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct0=0x09090909;   //9  9  9  9
  mul_ct1=0x01010101;   //1  1  1  1
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:9  9  9  9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr15,xr6);   
   Q8MAC_SS(xr5,xr8,xr14,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,1);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr13);   
   Q8MAC_SS(xr9,xr8,xr14,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,1);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr13);   
   Q8MAC_SS(xr9,xr8,xr14,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,1);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);   

   tptr += 12;
   src_aln0+=(-2*stride-8);
  }

  r = 64-rnd;
  tptr = tmp;

  S32I2M(xr15,(w5<<16)|w6); //xr15:9|1
  S32I2M(xr13,r);
  for(j=0;j<8;j++)
    {
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]

    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:9*tptr[2] xr10:9*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:9*tptr[2]-1*tptr[1] xr10:9*tptr[1]-1*tptr[0]
    D16MAC_AA_HW(xr9,xr15,xr2,xr10);//xr9:9*tptr[2]-1*tptr[1]+9*tptr[3] xr10:9*tptr[1]-1*tptr[0]+9*tptr[2]
    D16MAC_SS_LW(xr9,xr15,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr15,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr15,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);

    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]

    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr15,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr15,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);

    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr15,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr15,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);

    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }
}
#else
static void put_vc1_mspel_mc22_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
    int r,j;
    static const int shift_value[] = { 0, 5, 1, 5 };
    int              shift = (shift_value[2]+shift_value[2])>>1;
    int16_t          tmp[11*8], *tptr = tmp;
    r = (1<<(shift-1)) + rnd-1;
    src -= 1;
    for(j = 0; j < 8; j++)
      {
        tptr[0]= (-src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2]+r)>>shift;
        tptr[1]= (-src[-stride+1] + 9*src[1] + 9*src[stride+1] - src[stride*2+1]+r)>>shift;
        tptr[2]= (-src[-stride+2] + 9*src[2] + 9*src[stride+2] - src[stride*2+2]+r)>>shift;
        tptr[3]= (-src[-stride+3] + 9*src[3] + 9*src[stride+3] - src[stride*2+3]+r)>>shift;
        tptr[4]= (-src[-stride+4] + 9*src[4] + 9*src[stride+4] - src[stride*2+4]+r)>>shift;
        tptr[5]= (-src[-stride+5] + 9*src[5] + 9*src[stride+5] - src[stride*2+5]+r)>>shift;
        tptr[6]= (-src[-stride+6] + 9*src[6] + 9*src[stride+6] - src[stride*2+6]+r)>>shift;
        tptr[7]= (-src[-stride+7] + 9*src[7] + 9*src[stride+7] - src[stride*2+7]+r)>>shift;
        tptr[8]= (-src[-stride+8]  + 9*src[8] + 9*src[stride+8] - src[stride*2+8]+r)>>shift;
        tptr[9]= (-src[-stride+9] + 9*src[9] + 9*src[stride+9] - src[stride*2+9]+r)>>shift;
        tptr[10]= (-src[-stride+10] + 9*src[10] + 9*src[stride+10] - src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 11;
       }
        r = 64-rnd;
        tptr = tmp+1;
    for(j = 0; j < 8; j++)
      {
      dst[0]=av_clip_uint8((-tptr[-1] + 9*tptr[0] + 9*tptr[1] - tptr[2]+r)>>7);
      dst[1]=av_clip_uint8((-tptr[0] + 9*tptr[1] + 9*tptr[2] - tptr[3]+r)>>7);
      dst[2]=av_clip_uint8((-tptr[1] + 9*tptr[2] + 9*tptr[3] - tptr[4]+r)>>7);
      dst[3]=av_clip_uint8((-tptr[2] + 9*tptr[3] + 9*tptr[4] - tptr[5]+r)>>7);
      dst[4]=av_clip_uint8((-tptr[3] + 9*tptr[4] + 9*tptr[5] - tptr[6]+r)>>7);
      dst[5]=av_clip_uint8((-tptr[4] + 9*tptr[5] + 9*tptr[6] - tptr[7]+r)>>7);
      dst[6]=av_clip_uint8((-tptr[5] + 9*tptr[6] + 9*tptr[7] - tptr[8]+r)>>7);
      dst[7]=av_clip_uint8((-tptr[6] + 9*tptr[7] + 9*tptr[8] - tptr[9]+r)>>7);
      dst += stride;
      tptr += 11;
      }
} 
#endif
//PUT_VC1_MSPEL(3, 2)
/*
static void put_vc1_mspel_mc32_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,3,2,rnd);
} 
*/
#ifdef JZ4740_MXU_OPT
#define w1 18
#define w2 3
#define w3 53
#define w4 4
static void put_vc1_mspel_mc32_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{

  int j,r;
  int tmp[12*4];
  int16_t  *tptr = tmp;
  r = 3 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct0=0x09090909;   //9  9  9  9
  mul_ct1=0x01010101;   //1  1  1  1
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:9  9  9  9
  S32I2M(xr14,mul_ct1); //xr14:1 1 1 1
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr15,xr6);   
   Q8MAC_SS(xr5,xr8,xr14,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,3);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr13);   
   Q8MAC_SS(xr9,xr8,xr14,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc); 
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr15,xr13);   
   Q8MAC_SS(xr9,xr8,xr14,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
   r = 64-rnd;
   tptr = tmp;

   S32I2M(xr15,(w1<<16)|w2); //xr15:18|3
   S32I2M(xr14,(w3<<16)|w4); //xr14:53|4
   S32I2M(xr13,r);
   for(j=0;j<8;j++)
   {
    
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]
    
    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:53*tptr[2] xr10:53*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:53*tptr[2]-4*tptr[1] xr10:53*tptr[1]-4*tptr[0]
    D16MAC_AA_HW(xr9,xr14,xr2,xr10);//xr9:53*tptr[2]-4*tptr[1]+18*tptr[3] xr10:53*tptr[1]-4*tptr[0]+18*tptr[2]
    D16MAC_SS_LW(xr9,xr14,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr14,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr14,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);
    
    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]
     
    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr14,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr14,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);
     
    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr14,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr14,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);
   
    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
   }

}
#else
static void put_vc1_mspel_mc32_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
    int r,j;
    static const int shift_value[] = { 0, 5, 1, 5 };
    int              shift = (shift_value[3]+shift_value[2])>>1;
    int16_t          tmp[11*8], *tptr = tmp;
    r = (1<<(shift-1)) + rnd-1;
    src -= 1;
    for(j = 0; j < 8; j++)
      {
        tptr[0]= (-src[-stride] + 9*src[0] + 9*src[stride] - src[stride*2]+r)>>shift;
        tptr[1]= (-src[-stride+1] + 9*src[1] + 9*src[stride+1] - src[stride*2+1]+r)>>shift;
        tptr[2]= (-src[-stride+2] + 9*src[2] + 9*src[stride+2] - src[stride*2+2]+r)>>shift;
        tptr[3]= (-src[-stride+3] + 9*src[3] + 9*src[stride+3] - src[stride*2+3]+r)>>shift;
        tptr[4]= (-src[-stride+4] + 9*src[4] + 9*src[stride+4] - src[stride*2+4]+r)>>shift;
        tptr[5]= (-src[-stride+5] + 9*src[5] + 9*src[stride+5] - src[stride*2+5]+r)>>shift;
        tptr[6]= (-src[-stride+6] + 9*src[6] + 9*src[stride+6] - src[stride*2+6]+r)>>shift;
        tptr[7]= (-src[-stride+7] + 9*src[7] + 9*src[stride+7] - src[stride*2+7]+r)>>shift;
        tptr[8]= (-src[-stride+8]  + 9*src[8] + 9*src[stride+8] - src[stride*2+8]+r)>>shift;
        tptr[9]= (-src[-stride+9] + 9*src[9] + 9*src[stride+9] - src[stride*2+9]+r)>>shift;
        tptr[10]= (-src[-stride+10] + 9*src[10] + 9*src[stride+10] - src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 11;
       }
        r = 64-rnd;
        tptr = tmp+1;

     for(j = 0; j < 8; j++)
      {
      dst[0]=av_clip_uint8((-3*tptr[-1] + 18*tptr[0] + 53*tptr[1] - 4*tptr[2]+r)>>7);
      dst[1]=av_clip_uint8((-3*tptr[0] + 18*tptr[1] + 53*tptr[2] - 4*tptr[3]+r)>>7);
      dst[2]=av_clip_uint8((-3*tptr[1] + 18*tptr[2] + 53*tptr[3] - 4*tptr[4]+r)>>7);
      dst[3]=av_clip_uint8((-3*tptr[2] + 18*tptr[3] + 53*tptr[4] - 4*tptr[5]+r)>>7);
      dst[4]=av_clip_uint8((-3*tptr[3] + 18*tptr[4] + 53*tptr[5] - 4*tptr[6]+r)>>7);
      dst[5]=av_clip_uint8((-3*tptr[4] + 18*tptr[5] + 53*tptr[6] - 4*tptr[7]+r)>>7);
      dst[6]=av_clip_uint8((-3*tptr[5] + 18*tptr[6] + 53*tptr[7] - 4*tptr[8]+r)>>7);
      dst[7]=av_clip_uint8((-3*tptr[6] + 18*tptr[7] + 53*tptr[8] - 4*tptr[9]+r)>>7);
      dst += stride;
      tptr += 11;
      }
}    

#endif 
//PUT_VC1_MSPEL(0, 3)
/*
static void put_vc1_mspel_mc03_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,0,3,rnd);
} */
#ifdef JZ4740_MXU_OPT
 static void put_vc1_mspel_mc03_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
 {
  int j,r;
  r = 1-rnd;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1,ct0;
  mul_ct0=0x12121212;   //18 18 18 18
  mul_ct1=0x03030303;   //3  3  3  3
  mul_ct2=0x35353535;   //53 53 53 53
  mul_ct3=0x04040404;   //4  4  4  4 
  ct0=0x00200020;       //32    32
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:18 18 18 18
  S32I2M(xr14,mul_ct1); //xr14:3  3  3  3
  S32I2M(xr13,mul_ct2); //xr13:53 53 53 53
  S32I2M(xr12,mul_ct3); //xr12:4  4  4  4
  S32I2M(xr11,rnd1);    //xr11:r   r
  S32I2M(xr10,ct0);     //xr10:32  32 
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr13,xr6);   
   Q8MAC_SS(xr5,xr8,xr12,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr10,xr11,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,6);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   Q16SAT(xr5,xr5,xr6);
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr14,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr13,xr6);   
   Q8MAC_SS(xr9,xr8,xr12,xr6); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_SS(xr9,xr10,xr11,xr6); //two instructions with one
   Q16SAR(xr9,xr9,xr6,xr6,6);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   Q16SAT(xr9,xr9,xr6);
   S32STD(xr5,dst,0);
   S32STD(xr9,dst,4);
   dst += stride;
   src_aln0+=(-2*stride-4);
  }

}
#else
static void put_vc1_mspel_mc03_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
    int j,r;
    r = 1-rnd;
  for(j = 0; j < 8; j++)
     {
      dst[0]=av_clip_uint8((-3*src[-stride] + 18*src[0] + 53*src[stride] - 4*src[stride*2] + 32 - r) >> 6);
      dst[1]=av_clip_uint8((-3*src[-stride+1] + 18*src[1] + 53*src[stride+1] - 4*src[stride*2+1] + 32 - r) >> 6);
      dst[2]=av_clip_uint8((-3*src[-stride+2] + 18*src[2] + 53*src[stride+2] - 4*src[stride*2+2] + 32 - r) >> 6);
      dst[3]=av_clip_uint8((-3*src[-stride+3] + 18*src[3] + 53*src[stride+3] - 4*src[stride*2+3] + 32 - r) >> 6);
      dst[4]=av_clip_uint8((-3*src[-stride+4] + 18*src[4] + 53*src[stride+4] - 4*src[stride*2+4] + 32 - r) >> 6);
      dst[5]=av_clip_uint8((-3*src[-stride+5] + 18*src[5] + 53*src[stride+5] - 4*src[stride*2+5] + 32 - r) >> 6);
      dst[6]=av_clip_uint8((-3*src[-stride+6] + 18*src[6] + 53*src[stride+6] - 4*src[stride*2+6] + 32 - r) >> 6);
      dst[7]=av_clip_uint8((-3*src[-stride+7] + 18*src[7] + 53*src[stride+7] - 4*src[stride*2+7] + 32 - r) >> 6);
      src += stride;
      dst += stride;
     }
} 
#endif
//PUT_VC1_MSPEL(1, 3)
/*static void put_vc1_mspel_mc13_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,1,3,rnd);
} */
#ifdef JZ4740_MXU_OPT
#define w1 53
#define w2 4
#define w3 18
#define w4 3
static void put_vc1_mspel_mc13_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
 int j,r;
 int  tmp[12*4];
 int16_t  *tptr = tmp;
  r = 15 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct0=0x12121212;   //18 18 18 18
  mul_ct1=0x35353535;   //53 53 53 53
  mul_ct2=0x03030303;   //3  3  3  3
  mul_ct3=0x04040404;   //4  4  4  4
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,5);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
      r = 64-rnd;
      tptr = tmp;
    S32I2M(xr15,(w1<<16)|w2); //xr15:53|4
    S32I2M(xr14,(w3<<16)|w4); //xr14:18|3
    S32I2M(xr13,r);
    S32I2M(xr12,0x20);
    for(j=0;j<8;j++)
    {
    
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]
    
    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:53*tptr[2] xr10:53*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:53*tptr[2]-4*tptr[1] xr10:53*tptr[1]-4*tptr[0]
    D16MAC_AA_HW(xr9,xr14,xr2,xr10);//xr9:53*tptr[2]-4*tptr[1]+18*tptr[3] xr10:53*tptr[1]-4*tptr[0]+18*tptr[2]
    D16MAC_SS_LW(xr9,xr14,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr12,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr14,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr14,xr7,xr10); //

    D32ACC_AA(xr11,xr12,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);
    
    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]
     
    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr14,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr14,xr6,xr8); //

    D32ACC_AA(xr9,xr12,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);
     
    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr14,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr14,xr5,xr8); //

    D32ACC_AA(xr11,xr12,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);
   
    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;

    }
}
#else
static void put_vc1_mspel_mc13_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
     int r,j;
     static const int shift_value[] = { 0, 5, 1, 5 };
     int              shift = (shift_value[1]+shift_value[3])>>1;
     int16_t          tmp[11*8], *tptr = tmp;
     r = (1<<(shift-1)) + rnd-1;
     src -= 1;
     for(j = 0; j < 8; j++)
       {
        tptr[0]= (-3*src[-stride] + 18*src[0] + 53*src[stride] -4* src[stride*2]+r)>>shift;
        tptr[1]= (-3*src[-stride+1] + 18*src[1] + 53*src[stride+1] - 4*src[stride*2+1]+r)>>shift;
        tptr[2]= (-3*src[-stride+2] + 18*src[2] + 53*src[stride+2] - 4*src[stride*2+2]+r)>>shift;
        tptr[3]= (-3*src[-stride+3] + 18*src[3] + 53*src[stride+3] - 4*src[stride*2+3]+r)>>shift;
        tptr[4]= (-3*src[-stride+4] + 18*src[4] + 53*src[stride+4] - 4*src[stride*2+4]+r)>>shift;
        tptr[5]= (-3*src[-stride+5] + 18*src[5] + 53*src[stride+5] - 4*src[stride*2+5]+r)>>shift;
        tptr[6]= (-3*src[-stride+6] + 18*src[6] + 53*src[stride+6] - 4*src[stride*2+6]+r)>>shift;
        tptr[7]= (-3*src[-stride+7] + 18*src[7] + 53*src[stride+7] - 4*src[stride*2+7]+r)>>shift;
        tptr[8]= (-3*src[-stride+8] + 18*src[8] + 53*src[stride+8] - 4*src[stride*2+8]+r)>>shift;
        tptr[9]= (-3*src[-stride+9] + 18*src[9] + 53*src[stride+9] - 4*src[stride*2+9]+r)>>shift;
        tptr[10]= (-3*src[-stride+10] + 18*src[10] + 53*src[stride+10] - 4*src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 11;
     }
      r = 64-rnd;
      tptr = tmp+1;
     for(j = 0; j < 8; j++)
        {
        dst[0]=av_clip_uint8((-4*tptr[-1] + 53*tptr[0] + 18*tptr[1] - 3*tptr[2] + 32 - r) >> 7);
        dst[1]=av_clip_uint8((-4*tptr[0] + 53*tptr[1] + 18*tptr[2] - 3*tptr[3] + 32 - r) >> 7);
        dst[2]=av_clip_uint8((-4*tptr[1] + 53*tptr[2] + 18*tptr[3] - 3*tptr[4] + 32 - r) >> 7);
        dst[3]=av_clip_uint8((-4*tptr[2] + 53*tptr[3] + 18*tptr[4] - 3*tptr[5] + 32 - r) >> 7);
        dst[4]=av_clip_uint8((-4*tptr[3] + 53*tptr[4] + 18*tptr[5] - 3*tptr[6] + 32 - r) >> 7);
        dst[5]=av_clip_uint8((-4*tptr[4] + 53*tptr[5] + 18*tptr[6] - 3*tptr[7] + 32 - r) >> 7);
        dst[6]=av_clip_uint8((-4*tptr[5] + 53*tptr[6] + 18*tptr[7] - 3*tptr[8] + 32 - r) >> 7);
        dst[7]=av_clip_uint8((-4*tptr[6] + 53*tptr[7] + 18*tptr[8] - 3*tptr[9] + 32 - r) >> 7);
        dst += stride;
        tptr += 11;
        }
}
#endif
//PUT_VC1_MSPEL(2, 3)
/*
static void put_vc1_mspel_mc23_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,2,3,rnd);
}
*/ 
#ifdef JZ4740_MXU_OPT
#define w5 9
#define w6 1
static void put_vc1_mspel_mc23_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
 int j,r;
 int  tmp[12*4];
 int16_t  *tptr = tmp;
  r = 3 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct0=0x12121212;   //18 18 18 18
  mul_ct1=0x35353535;   //53 53 53 53
  mul_ct2=0x03030303;   //3  3  3  3
  mul_ct3=0x04040404;   //4  4  4  4
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)src - stride) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)src - stride) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,3);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
    S32STD(xr6,tptr,0x0);
    S32STD(xr5,tptr,0x4);
    S32STD(xr13,tptr,0x8);
    S32STD(xr9,tptr,0xc);
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,3);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
    r = 64-rnd;
    tptr = tmp;

  S32I2M(xr15,(w5<<16)|w6); //xr15:9|1
  S32I2M(xr13,r);
    for(j=0;j<8;j++)
    {
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]

    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:9*tptr[2] xr10:9*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:9*tptr[2]-1*tptr[1] xr10:9*tptr[1]-1*tptr[0]
    D16MAC_AA_HW(xr9,xr15,xr2,xr10);//xr9:9*tptr[2]-1*tptr[1]+9*tptr[3] xr10:9*tptr[1]-1*tptr[0]+9*tptr[2]
    D16MAC_SS_LW(xr9,xr15,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr15,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr15,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);

    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]

    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr15,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr15,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);

    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr15,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr15,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);

    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }
}
#else
static void put_vc1_mspel_mc23_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
     int r,j;
     static const int shift_value[] = { 0, 5, 1, 5 };
     int              shift = (shift_value[2]+shift_value[3])>>1;
     int16_t          tmp[11*8], *tptr = tmp;
     r = (1<<(shift-1)) + rnd-1;
     src -= 1;
     for(j = 0; j < 8; j++)
       {
        tptr[0]= (-3*src[-stride] + 18*src[0] + 53*src[stride] -4* src[stride*2]+r)>>shift;
        tptr[1]= (-3*src[-stride+1] + 18*src[1] + 53*src[stride+1] - 4*src[stride*2+1]+r)>>shift;
        tptr[2]= (-3*src[-stride+2] + 18*src[2] + 53*src[stride+2] - 4*src[stride*2+2]+r)>>shift;
        tptr[3]= (-3*src[-stride+3] + 18*src[3] + 53*src[stride+3] - 4*src[stride*2+3]+r)>>shift;
        tptr[4]= (-3*src[-stride+4] + 18*src[4] + 53*src[stride+4] - 4*src[stride*2+4]+r)>>shift;
        tptr[5]= (-3*src[-stride+5] + 18*src[5] + 53*src[stride+5] - 4*src[stride*2+5]+r)>>shift;
        tptr[6]= (-3*src[-stride+6] + 18*src[6] + 53*src[stride+6] - 4*src[stride*2+6]+r)>>shift;
        tptr[7]= (-3*src[-stride+7] + 18*src[7] + 53*src[stride+7] - 4*src[stride*2+7]+r)>>shift;
        tptr[8]= (-3*src[-stride+8]  + 18*src[8] + 53*src[stride+8] - 4*src[stride*2+8]+r)>>shift;
        tptr[9]= (-3*src[-stride+9] + 18*src[9] + 53*src[stride+9] - 4*src[stride*2+9]+r)>>shift;
        tptr[10]= (-3*src[-stride+10] + 18*src[10] + 53*src[stride+10] - 4*src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 11;
       }
      r = 64-rnd;
      tptr = tmp+1;
     for(j = 0; j < 8; j++)
      {
      dst[0]=av_clip_uint8((-tptr[-1] + 9*tptr[0] + 9*tptr[1] - tptr[2]+r)>>7);
      dst[1]=av_clip_uint8((-tptr[0] + 9*tptr[1] + 9*tptr[2] - tptr[3]+r)>>7);
      dst[2]=av_clip_uint8((-tptr[1] + 9*tptr[2] + 9*tptr[3] - tptr[4]+r)>>7);
      dst[3]=av_clip_uint8((-tptr[2] + 9*tptr[3] + 9*tptr[4] - tptr[5]+r)>>7);
      dst[4]=av_clip_uint8((-tptr[3] + 9*tptr[4] + 9*tptr[5] - tptr[6]+r)>>7);
      dst[5]=av_clip_uint8((-tptr[4] + 9*tptr[5] + 9*tptr[6] - tptr[7]+r)>>7);
      dst[6]=av_clip_uint8((-tptr[5] + 9*tptr[6] + 9*tptr[7] - tptr[8]+r)>>7);
      dst[7]=av_clip_uint8((-tptr[6] + 9*tptr[7] + 9*tptr[8] - tptr[9]+r)>>7);
      dst += stride;
      tptr += 11;
      }
}
#endif 
//PUT_VC1_MSPEL(3, 3)
/*static void put_vc1_mspel_mc33_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  vc1_mspel_mc(dst,src,stride,3,3,rnd);
} */

#ifdef JZ4740_MXU_OPT
#define w1 18
#define w2 3
#define w3 53
#define w4 4
static void put_vc1_mspel_mc33_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
  int j,r;
  int  tmp[12*4];
  int16_t  *tptr = tmp;
  r = 15 + rnd;
  src -= 1;
  uint32_t src_aln0,src_rs0;
  int mul_ct0,mul_ct1,mul_ct2,mul_ct3;
  int rnd1;
  mul_ct0=0x12121212;   //18 18 18 18
  mul_ct1=0x35353535;   //53 53 53 53
  mul_ct2=0x03030303;   //3  3  3  3
  mul_ct3=0x04040404;   //4  4  4  4
  rnd1 = r * 0x00010001;
  src_aln0 = (((uint32_t)(src - stride)) & 0xFFFFFFFC);
  src_rs0 = 4-(((uint32_t)(src - stride)) & 3);
  S32I2M(xr15,mul_ct0); //xr15:53 53 53 53
  S32I2M(xr14,mul_ct1); //xr14:18 18 18 18
  S32I2M(xr11,mul_ct2); //xr11:4  4  4  4
  S32I2M(xr10,mul_ct3); //xr10:3  3  3  3
  S32I2M(xr12,rnd1);    //xr12:   r   r
  for(j=0;j<8;j++){
   S32LDD(xr1,src_aln0,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);       
   S32LDD(xr6,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr6,xr5,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);
   S32LDIV(xr5,src_aln0,stride,0);
   S32LDD(xr6,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr6,xr5,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr5,xr4,xr14,xr6);   
   Q8MAC_SS(xr5,xr8,xr10,xr6); //xr5:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr6: 
 
   Q16ADD_AA_WW(xr0,xr5,xr1,xr5); 
   Q16ADD_AA_WW(xr0,xr6,xr2,xr6); 
  
   Q16ACC_AA(xr5,xr12,xr0,xr6); //two instructions with one
   Q16SAR(xr5,xr5,xr6,xr6,5);  //xr5:tptr[3] tptr[2] xr6:tptr[1] tptr[0]
   
   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[7] tptr[6] xr13:tptr[5] tptr[4]
   S32STD(xr6,tptr,0x0);
   S32STD(xr5,tptr,0x4);
   S32STD(xr13,tptr,0x8);
   S32STD(xr9,tptr,0xc);

   src_aln0 = src_aln0 - 3*stride;   
   S32LDI(xr1,src_aln0,4);
   S32LDD(xr2,src_aln0,4);
   
   S32LDIV(xr9,src_aln0,stride,0);       
   S32LDD(xr13,src_aln0,4);      
   S32ALN(xr3,xr2,xr1,src_rs0);  
   S32ALN(xr7,xr13,xr9,src_rs0);  
   S32LDIV(xr1,src_aln0,stride,0);
   S32LDD(xr2,src_aln0,4);

   S32LDIV(xr9,src_aln0,stride,0);
   S32LDD(xr13,src_aln0,4);
   
   S32ALN(xr4,xr2,xr1,src_rs0);
   S32ALN(xr8,xr13,xr9,src_rs0);
   
   Q8MUL(xr1,xr7,xr15,xr2);   
   Q8MAC_SS(xr1,xr3,xr11,xr2); //xr1:-4*src[-stride+3] + 53*src[3] -4*src[-stride+2] + 53*src[2] xr2:
   Q8MUL(xr9,xr4,xr14,xr13);   
   Q8MAC_SS(xr9,xr8,xr10,xr13); //xr9:18*src[stride+3]-3*src[stride*2+3] 18*src[stride+2]-3*src[stride*2+2] xr13: 
 
   Q16ADD_AA_WW(xr0,xr9,xr1,xr9); 
   Q16ADD_AA_WW(xr0,xr13,xr2,xr13); 
  
   Q16ACC_SS(xr9,xr12,xr0,xr13); //two instructions with one
   Q16SAR(xr9,xr9,xr13,xr13,5);  //xr9:tptr[11] tptr[10] xr13:tptr[9] tptr[8]
   S32STD(xr13,tptr,0x10);
   S32STD(xr9,tptr,0x14);
   tptr += 12;
   src_aln0+=(-2*stride-8);
  }
      r = 64-rnd;
      tptr = tmp;
    S32I2M(xr15,(w1<<16)|w2); //xr15:18|3
    S32I2M(xr14,(w3<<16)|w4); //xr14:53|4
    S32I2M(xr13,r);
    for(j=0;j<8;j++)
    {
     
    S32LDD(xr1,tptr,0x0);   //xr1:tptr[1] tptr[0]
    S32LDD(xr2,tptr,0x4);   //xr2:tptr[3] tptr[2]
    S32LDD(xr3,tptr,0x8);   //xr3:tptr[5] tptr[4]
    S32LDD(xr4,tptr,0xc);   //xr4:tptr[7] tptr[6]
    
    S32ALN(xr5,xr2,xr1,2);      //xr5:tptr[2] tptr[1]
    S32ALN(xr6,xr3,xr2,2);      //xr6:tptr[4] tptr[3]
    S32ALN(xr7,xr4,xr3,2);      //xr7:tptr[6] tptr[5]

    D16MUL_HW(xr9,xr15,xr5,xr10);    //xr9:53*tptr[2] xr10:53*tptr[1]
    D16MAC_SS_LW(xr9,xr15,xr1,xr10); //xr9:53*tptr[2]-4*tptr[1] xr10:53*tptr[1]-4*tptr[0]
    D16MAC_AA_HW(xr9,xr14,xr2,xr10);//xr9:53*tptr[2]-4*tptr[1]+18*tptr[3] xr10:53*tptr[1]-4*tptr[0]+18*tptr[2]
    D16MAC_SS_LW(xr9,xr14,xr6,xr10); //xr9:dst[1] xr10:dst[0]
    D32ACC_AA(xr9,xr0,xr13,xr10);
    D32SARL(xr9,xr9,xr10,7);

    D16MUL_HW(xr11,xr15,xr6,xr10);    //
    D16MAC_SS_LW(xr11,xr15,xr2,xr10); //
    D16MAC_AA_HW(xr11,xr14,xr3,xr10); //
    D16MAC_SS_LW(xr11,xr14,xr7,xr10); //

    D32ACC_AA(xr11,xr0,xr13,xr10);
    D32SARL(xr10,xr11,xr10,7);
    Q16SAT(xr10,xr10,xr9);
    
    S32LDD(xr1,tptr,0x10); //xr1:tptr[9] tptr[8]
    S32LDD(xr2,tptr,0x14); //xr2:tptr[11] tptr[10]
    S32ALN(xr5,xr2,xr1,2);     //xr5:tptr[10] tptr[9]
    S32ALN(xr6,xr1,xr4,2);     //xr6:tptr[8]  tptr[7]
     
    D16MUL_HW(xr9,xr15,xr7,xr8);    //
    D16MAC_SS_LW(xr9,xr15,xr3,xr8); //
    D16MAC_AA_HW(xr9,xr14,xr4,xr8); //
    D16MAC_SS_LW(xr9,xr14,xr6,xr8); //

    D32ACC_AA(xr9,xr0,xr13,xr8);
    D32SARL(xr9,xr9,xr8,7);
     
    D16MUL_HW(xr11,xr15,xr6,xr8);    //
    D16MAC_SS_LW(xr11,xr15,xr4,xr8); //
    D16MAC_AA_HW(xr11,xr14,xr1,xr8); //
    D16MAC_SS_LW(xr11,xr14,xr5,xr8); //

    D32ACC_AA(xr11,xr0,xr13,xr8);
    D32SARL(xr11,xr11,xr8,7);
    Q16SAT(xr11,xr11,xr9);
   
    S32STD(xr10,dst,0);
    S32STD(xr11,dst,4);
    dst+=stride;
    tptr+=12;
    }
}
#else
static void put_vc1_mspel_mc33_c(uint8_t *dst,const uint8_t *src,int stride,int rnd)
{
     int r,j;
     static const int shift_value[] = { 0, 5, 1, 5 };
     int              shift = (shift_value[3]+shift_value[3])>>1;
     int16_t          tmp[11*8], *tptr = tmp;
     r = (1<<(shift-1)) + rnd-1;
     src -= 1;
     for(j = 0; j < 8; j++)
       {
        tptr[0]= (-3*src[-stride] + 18*src[0] + 53*src[stride] -4* src[stride*2]+r)>>shift;
        tptr[1]= (-3*src[-stride+1] + 18*src[1] + 53*src[stride+1] - 4*src[stride*2+1]+r)>>shift;
        tptr[2]= (-3*src[-stride+2] + 18*src[2] + 53*src[stride+2] - 4*src[stride*2+2]+r)>>shift;
        tptr[3]= (-3*src[-stride+3] + 18*src[3] + 53*src[stride+3] - 4*src[stride*2+3]+r)>>shift;
        tptr[4]= (-3*src[-stride+4] + 18*src[4] + 53*src[stride+4] - 4*src[stride*2+4]+r)>>shift;
        tptr[5]= (-3*src[-stride+5] + 18*src[5] + 53*src[stride+5] - 4*src[stride*2+5]+r)>>shift;
        tptr[6]= (-3*src[-stride+6] + 18*src[6] + 53*src[stride+6] - 4*src[stride*2+6]+r)>>shift;
        tptr[7]= (-3*src[-stride+7] + 18*src[7] + 53*src[stride+7] - 4*src[stride*2+7]+r)>>shift;
        tptr[8]= (-3*src[-stride+8]  + 18*src[8] + 53*src[stride+8] - 4*src[stride*2+8]+r)>>shift;
        tptr[9]= (-3*src[-stride+9] + 18*src[9] + 53*src[stride+9] - 4*src[stride*2+9]+r)>>shift;
        tptr[10]= (-3*src[-stride+10] + 18*src[10] + 53*src[stride+10] - 4*src[stride*2+10]+r)>>shift;
        src += stride;
        tptr += 11;
       }

      r = 64-rnd;
      tptr = tmp+1;
     for(j = 0; j < 8; j++)
      {
      dst[0]=av_clip_uint8((-3*tptr[-1] + 18*tptr[0] + 53*tptr[1] - 4*tptr[2]+r)>>7);
      dst[1]=av_clip_uint8((-3*tptr[0] + 18*tptr[1] + 53*tptr[2] - 4*tptr[3]+r)>>7);
      dst[2]=av_clip_uint8((-3*tptr[1] + 18*tptr[2] + 53*tptr[3] - 4*tptr[4]+r)>>7);
      dst[3]=av_clip_uint8((-3*tptr[2] + 18*tptr[3] + 53*tptr[4] - 4*tptr[5]+r)>>7);
      dst[4]=av_clip_uint8((-3*tptr[3] + 18*tptr[4] + 53*tptr[5] - 4*tptr[6]+r)>>7);
      dst[5]=av_clip_uint8((-3*tptr[4] + 18*tptr[5] + 53*tptr[6] - 4*tptr[7]+r)>>7);
      dst[6]=av_clip_uint8((-3*tptr[5] + 18*tptr[6] + 53*tptr[7] - 4*tptr[8]+r)>>7);
      dst[7]=av_clip_uint8((-3*tptr[6] + 18*tptr[7] + 53*tptr[8] - 4*tptr[9]+r)>>7);
      dst += stride;
      tptr += 11;
      }
} 
#endif

#if !defined(JZ4755_OPT) || defined(JZ4760_OPT)
void ff_vc1dsp_init(DSPContext* dsp, AVCodecContext *avctx) {
    dsp->vc1_inv_trans_8x8 = vc1_inv_trans_8x8_c;
    dsp->vc1_inv_trans_4x8 = vc1_inv_trans_4x8_c;
    dsp->vc1_inv_trans_8x4 = vc1_inv_trans_8x4_c;
    dsp->vc1_inv_trans_4x4 = vc1_inv_trans_4x4_c;
    dsp->vc1_h_overlap = vc1_h_overlap_c;
    dsp->vc1_v_overlap = vc1_v_overlap_c;

    dsp->put_vc1_mspel_pixels_tab[ 0] = ff_put_vc1_mspel_mc00_c;
    dsp->put_vc1_mspel_pixels_tab[ 1] = put_vc1_mspel_mc10_c;
    dsp->put_vc1_mspel_pixels_tab[ 2] = put_vc1_mspel_mc20_c;
    dsp->put_vc1_mspel_pixels_tab[ 3] = put_vc1_mspel_mc30_c;
    dsp->put_vc1_mspel_pixels_tab[ 3] = put_vc1_mspel_mc30_c;
    dsp->put_vc1_mspel_pixels_tab[ 4] = put_vc1_mspel_mc01_c;
    dsp->put_vc1_mspel_pixels_tab[ 5] = put_vc1_mspel_mc11_c;
    dsp->put_vc1_mspel_pixels_tab[ 6] = put_vc1_mspel_mc21_c;
    dsp->put_vc1_mspel_pixels_tab[ 7] = put_vc1_mspel_mc31_c;
    dsp->put_vc1_mspel_pixels_tab[ 8] = put_vc1_mspel_mc02_c;
    dsp->put_vc1_mspel_pixels_tab[ 9] = put_vc1_mspel_mc12_c;
    dsp->put_vc1_mspel_pixels_tab[10] = put_vc1_mspel_mc22_c;
    dsp->put_vc1_mspel_pixels_tab[11] = put_vc1_mspel_mc32_c;
    dsp->put_vc1_mspel_pixels_tab[12] = put_vc1_mspel_mc03_c;
    dsp->put_vc1_mspel_pixels_tab[13] = put_vc1_mspel_mc13_c;
    dsp->put_vc1_mspel_pixels_tab[14] = put_vc1_mspel_mc23_c;
    dsp->put_vc1_mspel_pixels_tab[15] = put_vc1_mspel_mc33_c;
}
#endif

