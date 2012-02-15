/*
 * Copyright (C) 2004 the ffmpeg project
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
 * @file vp3dsp.c
 * Standard C DSP-oriented functions cribbed from the original VP3
 * source code.
 */

#include "avcodec.h"
#include "dsputil.h"

#define IdctAdjustBeforeShift 8
#if 0
/* The following is the origenal code of vp3dsp.
   This algorithm uses fixed-point multiply two times, so the error is added.
   This window is 16bit sin(n*pi/16)*(1<<16).
*/
#define xC1S7 64277
#define xC2S6 60547
#define xC3S5 54491
#define xC4S4 46341
#define xC5S3 36410
#define xC6S2 25080
#define xC7S1 12785

#define M(a,b) (((a) * (b))>>16)

static av_always_inline void idct(uint8_t *dst, int stride, int16_t *input, int type)
{
    int16_t *ip = input;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    int A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    int Ed, Gd, Add, Bdd, Fd, Hd;

    int i;

    /* Inverse DCT on the rows now */
    for (i = 0; i < 8; i++) {
        /* Check for non-zero values */
        if ( ip[0] | ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7] ) {
            A = M(xC1S7, ip[1]) + M(xC7S1, ip[7]);
            B = M(xC7S1, ip[1]) - M(xC1S7, ip[7]);
            C = M(xC3S5, ip[3]) + M(xC5S3, ip[5]);
            D = M(xC3S5, ip[5]) - M(xC5S3, ip[3]);

            Ad = M(xC4S4, (A - C));
            Bd = M(xC4S4, (B - D));

            Cd = A + C;
            Dd = B + D;

            E = M(xC4S4, (ip[0] + ip[4]));
            F = M(xC4S4, (ip[0] - ip[4]));

            G = M(xC2S6, ip[2]) + M(xC6S2, ip[6]);
            H = M(xC6S2, ip[2]) - M(xC2S6, ip[6]);

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            /*  Final sequence of operations over-write original inputs. */
            ip[0] = Gd + Cd ;
            ip[7] = Gd - Cd ;

            ip[1] = Add + Hd;
            ip[2] = Add - Hd;

            ip[3] = Ed + Dd ;
            ip[4] = Ed - Dd ;

            ip[5] = Fd + Bdd;
            ip[6] = Fd - Bdd;

        }

        ip += 8;            /* next row */
    }

    ip = input;

    for ( i = 0; i < 8; i++) {
        /* Check for non-zero values (bitwise or faster than ||) */
        if ( ip[1 * 8] | ip[2 * 8] | ip[3 * 8] |
             ip[4 * 8] | ip[5 * 8] | ip[6 * 8] | ip[7 * 8] ) {

            A = M(xC1S7, ip[1*8]) + M(xC7S1, ip[7*8]);
            B = M(xC7S1, ip[1*8]) - M(xC1S7, ip[7*8]);
            C = M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8]);
            D = M(xC3S5, ip[5*8]) - M(xC5S3, ip[3*8]);

            Ad = M(xC4S4, (A - C));
            Bd = M(xC4S4, (B - D));

            Cd = A + C;
            Dd = B + D;

            E = M(xC4S4, (ip[0*8] + ip[4*8])) + 8;
            F = M(xC4S4, (ip[0*8] - ip[4*8])) + 8;

            if(type==1){  //HACK
                E += 16*128;
                F += 16*128;
            }

            G = M(xC2S6, ip[2*8]) + M(xC6S2, ip[6*8]);
            H = M(xC6S2, ip[2*8]) - M(xC2S6, ip[6*8]);

            Ed = E - G;
            Gd = E + G;

            Add = F + Ad;
            Bdd = Bd - H;

            Fd = F - Ad;
            Hd = Bd + H;

            /* Final sequence of operations over-write original inputs. */
            if(type==0){
                ip[0*8] = (Gd + Cd )  >> 4;
                ip[7*8] = (Gd - Cd )  >> 4;

                ip[1*8] = (Add + Hd ) >> 4;
                ip[2*8] = (Add - Hd ) >> 4;

                ip[3*8] = (Ed + Dd )  >> 4;
                ip[4*8] = (Ed - Dd )  >> 4;

                ip[5*8] = (Fd + Bdd ) >> 4;
                ip[6*8] = (Fd - Bdd ) >> 4;
            }else if(type==1){
                dst[0*stride] = cm[(Gd + Cd )  >> 4];
                dst[7*stride] = cm[(Gd - Cd )  >> 4];

                dst[1*stride] = cm[(Add + Hd ) >> 4];
                dst[2*stride] = cm[(Add - Hd ) >> 4];

                dst[3*stride] = cm[(Ed + Dd )  >> 4];
                dst[4*stride] = cm[(Ed - Dd )  >> 4];

                dst[5*stride] = cm[(Fd + Bdd ) >> 4];
                dst[6*stride] = cm[(Fd - Bdd ) >> 4];
            }else{
                dst[0*stride] = cm[dst[0*stride] + ((Gd + Cd )  >> 4)];
                dst[7*stride] = cm[dst[7*stride] + ((Gd - Cd )  >> 4)];

                dst[1*stride] = cm[dst[1*stride] + ((Add + Hd ) >> 4)];
                dst[2*stride] = cm[dst[2*stride] + ((Add - Hd ) >> 4)];

                dst[3*stride] = cm[dst[3*stride] + ((Ed + Dd )  >> 4)];
                dst[4*stride] = cm[dst[4*stride] + ((Ed - Dd )  >> 4)];

                dst[5*stride] = cm[dst[5*stride] + ((Fd + Bdd ) >> 4)];
                dst[6*stride] = cm[dst[6*stride] + ((Fd - Bdd ) >> 4)];
            }

        } else {
            if(type==0){
                ip[0*8] =
                ip[1*8] =
                ip[2*8] =
                ip[3*8] =
                ip[4*8] =
                ip[5*8] =
                ip[6*8] =
                ip[7*8] = ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<16))>>20);
            }else if(type==1){
                dst[0*stride]=
                dst[1*stride]=
                dst[2*stride]=
                dst[3*stride]=
                dst[4*stride]=
                dst[5*stride]=
                dst[6*stride]=
                dst[7*stride]= 128 + ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<16))>>20);
            }else{
                if(ip[0*8]){
                    int v= ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<16))>>20);
                    dst[0*stride] = cm[dst[0*stride] + v];
                    dst[1*stride] = cm[dst[1*stride] + v];
                    dst[2*stride] = cm[dst[2*stride] + v];
                    dst[3*stride] = cm[dst[3*stride] + v];
                    dst[4*stride] = cm[dst[4*stride] + v];
                    dst[5*stride] = cm[dst[5*stride] + v];
                    dst[6*stride] = cm[dst[6*stride] + v];
                    dst[7*stride] = cm[dst[7*stride] + v];
                }
            }
        }

        ip++;            /* next column */
        dst++;
    }
}
#else

/* The following is the vp3dsp code based origenal idct algortihm.
   This algorithm uses fixed-point multiply one times, so some error is removed in origenal vp3dsp code.
   This window is 15bit sin(n*pi/16)*(1<<15).
*/
#define xC1S7 0x7D87
#define xC2S6 0x7642
#define xC3S5 0x6A6E
#define xC4S4 0x5A82
#define xC5S3 0x471D
#define xC6S2 0x30FC
#define xC7S1 0x18F9
#define M(a,b) (((a) * (b))>>15)

static av_always_inline void idct(uint8_t *dst, int stride, int16_t *input, int type)
{
    int16_t *ip = input;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    int a0, a1, a2, a3;
    int b0, b1, b2, b3;
    int ip_zero;

    int i;

    /* Inverse DCT on the rows now */
    for (i = 0; i < 8; i++) {
        /* Check for non-zero values */
        ip_zero= ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7];
        if(!(ip_zero|ip[0])){
           ip += 8;
           continue;
        }
        if(ip_zero==0&&ip[0]!=0){
           a0=M(xC4S4, ip[0]);
           ip[0]=a0;
           ip[1]=a0;
           ip[2]=a0;
           ip[3]=a0;
           ip[4]=a0;
           ip[5]=a0;
           ip[6]=a0;
           ip[7]=a0;
           ip += 8;
           continue;
        }
           int tmp_v;
           tmp_v=xC4S4*(ip[0]+ip[4]);
           a0=tmp_v>>15;
           tmp_v=xC2S6*ip[2];
           tmp_v+=xC6S2*ip[6];
           a0+= tmp_v>>15;
           //a0 = M(xC4S4, (ip[0]+ip[4])) + M(xC2S6, ip[2])+M(xC6S2, ip[6]);
           tmp_v=xC4S4*(ip[0]-ip[4]);
           a1=tmp_v>>15;
           tmp_v=xC6S2*ip[2];
           tmp_v-=xC2S6*ip[6];
           a1+= tmp_v>>15;
           //a1 = M(xC4S4, (ip[0]-ip[4])) + M(xC6S2, ip[2])-M(xC2S6, ip[6]);
           tmp_v=xC4S4*(ip[0]-ip[4]);
           a2=tmp_v>>15;
           tmp_v=xC6S2*ip[2];
           tmp_v-=xC2S6*ip[6];
           a2-= tmp_v>>15;
           //a2 = M(xC4S4, (ip[0]-ip[4])) - M(xC6S2, ip[2])+M(xC2S6, ip[6]);
           tmp_v=xC4S4*(ip[0]+ip[4]);
           a3=tmp_v>>15;
           tmp_v=xC2S6*ip[2];
           tmp_v+=xC6S2*ip[6];
           a3-= tmp_v>>15;
           //a3 = M(xC4S4, (ip[0]+ip[4])) - M(xC2S6, ip[2])-M(xC6S2, ip[6]);
           tmp_v=xC1S7*ip[1];
           tmp_v+=xC3S5*ip[3];
           tmp_v+=xC5S3*ip[5];
           tmp_v+=xC7S1*ip[7];
           b0 = tmp_v>>15;
           //b0 = M(xC1S7, ip[1]) + M(xC3S5, ip[3]) + M(xC5S3, ip[5])+M(xC7S1, ip[7]);
           tmp_v=xC3S5*ip[1];
           tmp_v-=xC7S1*ip[3];
           tmp_v-=xC1S7*ip[5];
           tmp_v-=xC5S3*ip[7];
           b1 = tmp_v>>15;
           //b1 = M(xC3S5, ip[1]) - M(xC7S1, ip[3]) - M(xC1S7, ip[5])-M(xC5S3, ip[7]);
           tmp_v=xC5S3*ip[1];
           tmp_v-=xC1S7*ip[3];
           tmp_v+=xC7S1*ip[5];
           tmp_v+=xC3S5*ip[7];
           b2 = tmp_v>>15;
           //b2 = M(xC5S3, ip[1]) - M(xC1S7, ip[3]) + M(xC7S1, ip[5])+M(xC3S5, ip[7]);
           tmp_v=xC7S1*ip[1];
           tmp_v-=xC5S3*ip[3];
           tmp_v+=xC3S5*ip[5];
           tmp_v-=xC1S7*ip[7];
           b3 = tmp_v>>15;
           //b3 = M(xC7S1, ip[1]) - M(xC5S3, ip[3]) + M(xC3S5, ip[5])-M(xC1S7, ip[7]);
           ip[0]=a0+b0;
           ip[7]=a0-b0;
           ip[1]=a1+b1;
           ip[6]=a1-b1;
           ip[2]=a2+b2;
           ip[5]=a2-b2;
           ip[3]=a3+b3;
           ip[4]=a3-b3;

        ip += 8;            /* next row */
    }

    ip = input;

    for ( i = 0; i < 8; i++) {
        /* Check for non-zero values (bitwise or faster than ||) */
        if ( ip[1 * 8] | ip[2 * 8] | ip[3 * 8] |
             ip[4 * 8] | ip[5 * 8] | ip[6 * 8] | ip[7 * 8] ) {
           int tmp_v;
           tmp_v=xC4S4*(ip[0*8]+ip[4*8]);
           a0=tmp_v>>15;
           tmp_v=xC2S6*ip[2*8];
           tmp_v+=xC6S2*ip[6*8];
           a0+= tmp_v>>15;
           a0+= 8;
           //a0 = M(xC4S4, (ip[0*8]+ip[4*8])) + M(xC2S6, ip[2*8])+M(xC6S2, ip[6*8])+8;

           tmp_v=xC4S4*(ip[0*8]-ip[4*8]);
           a1=tmp_v>>15;
           tmp_v=xC6S2*ip[2*8];
           tmp_v-=xC2S6*ip[6*8];
           a1+= tmp_v>>15;
           a1+=8;
           //a1 = M(xC4S4, (ip[0*8]-ip[4*8])) + M(xC6S2, ip[2*8])-M(xC2S6, ip[6*8])+8;

           tmp_v=xC4S4*(ip[0*8]-ip[4*8]);
           a2=tmp_v>>15;
           tmp_v=xC6S2*ip[2*8];
           tmp_v-=xC2S6*ip[6*8];
           a2-= tmp_v>>15;
           a2+=8;
           //a2 = M(xC4S4, (ip[0*8]-ip[4*8])) - M(xC6S2, ip[2*8])+M(xC2S6, ip[6*8])+8;

           tmp_v=xC4S4*(ip[0*8]+ip[4*8]);
           a3=tmp_v>>15;
           tmp_v=xC2S6*ip[2*8];
           tmp_v+=xC6S2*ip[6*8];
           a3-= tmp_v>>15;
           a3+=8;
           //a3 = M(xC4S4, (ip[0*8]+ip[4*8])) - M(xC2S6, ip[2*8])-M(xC6S2, ip[6*8])+8;

           tmp_v=xC1S7*ip[1*8];
           tmp_v+=xC3S5*ip[3*8];
           tmp_v+=xC5S3*ip[5*8];
           tmp_v+=xC7S1*ip[7*8];
           b0 = tmp_v>>15;
           //b0 = M(xC1S7, ip[1*8]) + M(xC3S5, ip[3*8]) + M(xC5S3, ip[5*8])+M(xC7S1, ip[7*8]);
           tmp_v=xC3S5*ip[1*8];
           tmp_v-=xC7S1*ip[3*8];
           tmp_v-=xC1S7*ip[5*8];
           tmp_v-=xC5S3*ip[7*8];
           b1 = tmp_v>>15;
           //b1 = M(xC3S5, ip[1*8]) - M(xC7S1, ip[3*8]) - M(xC1S7, ip[5*8])-M(xC5S3, ip[7*8]);
           tmp_v=xC5S3*ip[1*8];
           tmp_v-=xC1S7*ip[3*8];
           tmp_v+=xC7S1*ip[5*8];
           tmp_v+=xC3S5*ip[7*8];
           b2 = tmp_v>>15;
           //b2 = M(xC5S3, ip[1*8]) - M(xC1S7, ip[3*8]) + M(xC7S1, ip[5*8])+M(xC3S5, ip[7*8]);
           tmp_v=xC7S1*ip[1*8];
           tmp_v-=xC5S3*ip[3*8];
           tmp_v+=xC3S5*ip[5*8];
           tmp_v-=xC1S7*ip[7*8];
           b3 = tmp_v>>15;
           //b3 = M(xC7S1, ip[1*8]) - M(xC5S3, ip[3*8]) + M(xC3S5, ip[5*8])-M(xC1S7, ip[7*8]);
            if(type==1){  //HACK
                a0 += 16*128;
                a1 += 16*128;
                a2 += 16*128;
                a3 += 16*128;
            }

            if(type==0){
                ip[0*8] = (a0 + b0 )  >> 4;
                ip[7*8] = (a0 - b0 )  >> 4;

                ip[1*8] = (a1 + b1 ) >> 4;
                ip[6*8] = (a1 - b1 ) >> 4;

                ip[2*8] = (a2 + b2 )  >> 4;
                ip[5*8] = (a2 - b2 )  >> 4;

                ip[3*8] = (a3 + b3 ) >> 4;
                ip[4*8] = (a3 - b3 ) >> 4;
            }else if(type==1){
                dst[0*stride] = cm[(a0 + b0 ) >> 4];
                dst[7*stride] = cm[(a0 - b0 ) >> 4];

                dst[1*stride] = cm[(a1 + b1 ) >> 4];
                dst[6*stride] = cm[(a1 - b1 ) >> 4];

                dst[2*stride] = cm[(a2 + b2 ) >> 4];
                dst[5*stride] = cm[(a2 - b2 ) >> 4];

                dst[3*stride] = cm[(a3 + b3 ) >> 4];
                dst[4*stride] = cm[(a3 - b3 ) >> 4];

            }else{
                dst[0*stride] = cm[dst[0*stride] + ((a0 + b0 )  >> 4)];
                dst[7*stride] = cm[dst[7*stride] + ((a0 - b0 )  >> 4)];

                dst[1*stride] = cm[dst[1*stride] + ((a1 + b1 ) >> 4)];
                dst[6*stride] = cm[dst[6*stride] + ((a1 - b1 ) >> 4)];

                dst[2*stride] = cm[dst[2*stride] + ((a2 + b2 )  >> 4)];
                dst[5*stride] = cm[dst[5*stride] + ((a2 - b2 )  >> 4)];

                dst[3*stride] = cm[dst[3*stride] + ((a3 + b3 ) >> 4)];
                dst[4*stride] = cm[dst[4*stride] + ((a3 - b3 ) >> 4)];
            }

        } else {
            if(type==0){
                ip[0*8] =
                ip[1*8] =
                ip[2*8] =
                ip[3*8] =
                ip[4*8] =
                ip[5*8] =
                ip[6*8] =
                ip[7*8] = ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<15))>>19);
            }else if(type==1){
                dst[0*stride]=
                dst[1*stride]=
                dst[2*stride]=
                dst[3*stride]=
                dst[4*stride]=
                dst[5*stride]=
                dst[6*stride]=
                dst[7*stride]= 128 + ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<15))>>19);
            }else{
                if(ip[0*8]){
                    int v= ((xC4S4 * ip[0*8] + (IdctAdjustBeforeShift<<15))>>19);
                    dst[0*stride] = cm[dst[0*stride] + v];
                    dst[1*stride] = cm[dst[1*stride] + v];
                    dst[2*stride] = cm[dst[2*stride] + v];
                    dst[3*stride] = cm[dst[3*stride] + v];
                    dst[4*stride] = cm[dst[4*stride] + v];
                    dst[5*stride] = cm[dst[5*stride] + v];
                    dst[6*stride] = cm[dst[6*stride] + v];
                    dst[7*stride] = cm[dst[7*stride] + v];
                }
            }
        }

        ip++;            /* next column */
        dst++;
    }
}
#endif
#define  wxr5  0x5A827642  //c4c2
#define  wxr6  0x5A8230FC  //c4c6
#define  wxr7  0x7D876A6E  //c1c3
#define  wxr8  0x18F9471D  //c7c5
#define  wxr9  0x6A6E18F9  //c3c7
#define  wxr10  0x471D7D87 //c5c1
static int32_t whirl_idct[6] = {wxr5, wxr6, wxr7, wxr8, wxr9, wxr10};
#ifdef JZ4750_OPT
void ff_vp3_idct_c( DCTELEM *input/*align 16*/){
    int a0, a1, a2, a3;
    int b0, b1, b2, b3;
    int i, j;
    int16_t  *blk;
    int32_t wf = (int32_t)whirl_idct;

    S32LDD(xr5, wf, 0x0);         // xr5(w7, w3)
    S32LDD(xr6, wf, 0x4);         // xr6(w9, w8)
    S32LDD(xr7, wf, 0x8);         // xr7(w11,w10)
    S32LDD(xr8, wf, 0xc);         // xr8(w13,w12)
    S32LDD(xr9, wf, 0x10);        // xr9(w6, w0)
    S32LDD(xr10,wf, 0x14);
    blk = input - 8;
    /* Inverse DCT on the rows now */
    for (i = 0; i < 8; i++) {
      S32LDI(xr1, blk, 0x10);       //  xr1 (x4, x0)
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      S32OR(xr12, xr2,xr3);
      S32OR(xr11,xr12,xr4);
      S32OR(xr12,xr11,xr1);
      if (S32M2I(xr12)==0) {
             continue;            //blk[0]= blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=0
      }
      S32SFL(xr12,xr0,xr1,xr13,ptn3);
      S32OR(xr11,xr11,xr12);
      if (S32M2I(xr11)==0&&S32M2I(xr13)!=0) {
         D16MUL_HW(xr0,xr5,xr13,xr13);
         D32SAR(xr0,xr0,xr13,xr13,15);
         S32SFL(xr0,xr13,xr13,xr13,ptn3);
         S32STD(xr13,blk, 0x0);
         S32STD(xr13,blk, 0x4);
         S32STD(xr13,blk, 0x8);
         S32STD(xr13,blk, 0xc);
         continue;            //blk[0]!=0, and blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=0
      }

        /* Check for non-zero values */
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
      D32SAR(xr2, xr2,xr4,xr4,15);
      S32SFL(xr0, xr2,xr4,xr2,ptn3);
      D32SAR(xr11, xr11,xr13,xr13,15);
      S32SFL(xr0, xr11,xr13,xr11,ptn3);
      D32SAR(xr12,xr12,xr14,xr14,15);
      S32SFL(xr0, xr12,xr14,xr12,ptn3);
      D16MUL_HW(xr4, xr1, xr8, xr15);     //xr4: s1*c7, xr15:s1*c5
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);    //xr4: s1*c7-s3*c5, xr15: s1*c5-s3*c1
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);     //xr4: s1*c7-s3*c5+s5*c3, xr15: s1*c5-s3*c1+s5*c7
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);     //xr4: s1*c7-s3*c5+s5*c3-s7*c1
                                          //xr15: s1*c5-s3*c1+s5*c7+s7*c3
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);  //xr11: rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16
                                          //      rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16
                                          //xr12: rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16
                                          //      rnd(s0*c4-s4*c4)>>16-rnd(s2*c6-s6*c2)>>16
      D32SAR(xr15,xr15,xr4,xr4,15);
      S32SFL(xr0,xr15,xr4,xr15,ptn3);
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

    blk = input - 2;

    for (i = 0; i < 4; i++)               /* idct columns */
    {
      S32LDI(xr1, blk, 0x4);   //xr1: ss0, s0
      S32LDD(xr3, blk, 0x20);  //xr3: ss2, s2
      S32I2M(xr5,wxr5);        //xr5: c4 , c2
      S32LDD(xr11, blk, 0x40); //xr11: ss4, s4
      S32LDD(xr13, blk, 0x60); //xr13: ss6, s6

      D16MUL_HW(xr15, xr5, xr1, xr9);    //xr15: ss0*c4, xr9: s0*c4
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);   //xr15: ss0*c4+ss4*c4, xr9: s0*c4+s4*c4
      D32SAR(xr15,xr15,xr9,xr9,15);
      S32SFL(xr0,xr15,xr9,xr15,ptn3);
      D16MUL_LW(xr10, xr5, xr3, xr9);    //xr10: ss2*c2, xr9: s2*c2
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);   //xr10: ss2*c2+ss6*c6, xr9: s2*c2+s6*c6
      D32SAR(xr10,xr10,xr9,xr9,15);
      S32SFL(xr0,xr10,xr9,xr10,ptn3);
      S32LDD(xr2, blk, 0x10);            //xr2: ss1, s1
      S32LDD(xr4, blk, 0x30);            //xr4: ss3, s3
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);  //xr15: rnd(ss0*c4+ss4*c4)>>16+rnd(ss2*c2+ss6*c6)>>16
                                         //    :rnd(s0*c4+s4*c4)>>16 + rnd(s2*c2 + s6*c6)>>16
                                         //xr9: rnd(ss0*c4+ss4*c4)>>16 - rnd(ss2*c2+ss6*c6)>>16
                                         //   : rnd(s0*c4+s4*c4)>>16 - rnd(s2*c2 + s6*c6)>>16
      D16MUL_HW(xr10, xr5, xr1, xr1);    //xr10: ss0*c4, xr1: s0*c4
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);   //xr10: ss0*c4-ss4*c4, xr1: s0*c4 - s4*c4
      D32SAR(xr10,xr10,xr1,xr1,15);
      S32SFL(xr0,xr10,xr1,xr10,ptn3);
      D16MUL_LW(xr11, xr6, xr3, xr1);    //xr11: ss2*c6, xr1: s2*c6
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);   //xr11: ss2*c6-ss6*c2, xr1: s2*c6-s6*c2
      D32SAR(xr11,xr11,xr1,xr1,15);
      S32SFL(xr0,xr11,xr1,xr11,ptn3);
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
      D32SAR(xr11,xr11,xr13,xr13,15);
      S32SFL(xr0,xr11,xr13,xr11,ptn3);

      D16MUL_LW(xr3, xr7, xr2, xr13);    //xr3: ss1*c3, xr13: s1*c3
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);    //xr3: ss1*c3-ss3*c7, xr13: s1*c3-s3*c7
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1
                                         //xr13: s1*c3-s3*c7-s5*c1
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1-ss7*c5
                                         //xr13: s1*c3-s3*c7-s7*c5
      D32SAR(xr3,xr3,xr13,xr13,15);
      S32SFL(xr0,xr3,xr13,xr3,ptn3);
      D16MUL_LW(xr5, xr8, xr2, xr13);    //xr5: ss1*c5, xr13:s1*c5
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);    //xr5: ss1*c5-ss3*c1, xr13:s1*c5-s3*c1
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7
                                         //   : s1*c5 - s3*c1+ s5*c7
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7+ss7*c1
                                         //   : s1*c5 - s3*c1+ s5*c7+ s7*c1
      D32SAR(xr5,xr5,xr13,xr13,15);
      S32SFL(xr0,xr5,xr13,xr5,ptn3);

      D16MUL_HW(xr2, xr8, xr2, xr13);    //xr2: ss1*c7, xr13: s1*c7
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);    //xr2: ss1*c7-ss3*c5, xr13: s1*c7-s3*c5
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1
                                         //xr13: s1*c7-s3*c5+s5*c1
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1-ss7*c3
                                         //xr13: s1*c7-s3*c5+s5*c1-s7*c3
      D32SAR(xr2,xr2,xr13,xr13,15);
      S32SFL(xr0,xr2,xr13,xr2,ptn3);
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
}
#else

void ff_vp3_idct_c(DCTELEM *block/* align 16*/){
    idct(NULL, 0, block, 0);
}
#endif
#ifdef JZ4750_OPT
void ff_vp3_idct_put_c(uint8_t *dst/*align 8*/, int stride, DCTELEM *input/*align 16*/){
    int i, j;
    int16_t  *blk;
    int32_t wf = (int32_t)whirl_idct;
    S32LDD(xr5, wf, 0x0);         // xr5(w7, w3)
    S32LDD(xr6, wf, 0x4);         // xr6(w9, w8)
    S32LDD(xr7, wf, 0x8);         // xr7(w11,w10)
    S32LDD(xr8, wf, 0xc);         // xr8(w13,w12)
    S32LDD(xr9, wf, 0x10);        // xr9(w6, w0)
    S32LDD(xr10,wf, 0x14);
    blk = input - 8;
    /* Inverse DCT on the rows now */
    for (i = 0; i < 8; i++) {
      S32LDI(xr1, blk, 0x10);       //  xr1 (x4, x0)
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      S32OR(xr12, xr2,xr3);
      S32OR(xr11,xr12,xr4);
      S32OR(xr12,xr11,xr1);
      if(S32M2I(xr12)==0) {
          continue;            //blk[0]= blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=0
      }
      S32SFL(xr12,xr0,xr1,xr13,ptn3);
      S32OR(xr11,xr11,xr12);
      if (S32M2I(xr11)==0&&S32M2I(xr13)!=0) {
         D16MUL_HW(xr0,xr5,xr13,xr13);
         D32SAR(xr0,xr0,xr13,xr13,15);
         S32SFL(xr0,xr13,xr13,xr13,ptn3);
         S32STD(xr13,blk, 0x0);
         S32STD(xr13,blk, 0x4);
         S32STD(xr13,blk, 0x8);
         S32STD(xr13,blk, 0xc);
         continue;            //blk[0]!=0, and blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=0
      }
      S32SFL(xr1,xr1,xr2,xr2, ptn3);  //xr1:s1, s3, xr2: s0, s2
      S32SFL(xr3,xr3,xr4,xr4, ptn3);  //xr3:s5, s7, xr4: s4, s6

      D16MUL_WW(xr11, xr2, xr5, xr12);//xr11: s0*c4, xr12: s2*c2
      D16MAC_AA_WW(xr11,xr4,xr6,xr12);//xr11: s0*c4+s4*c4, xr12: s2*c2+s6*c6

      D16MUL_WW(xr13, xr2, xr6, xr14);//xr13: s0*c4, xr14: s2*c6
      D16MAC_SS_WW(xr13,xr4,xr5,xr14);//xr13: s0*c4 - s4*c4, xr14: s2*c6-s6*c2

      D16MUL_HW(xr2, xr1, xr7, xr4);  //xr2: s1*c1, xr4: s1*c3 
      D16MAC_AS_LW(xr2,xr1,xr9,xr4);  //xr2: s1*c1+s3*c3, xr4: s1*c3-s3*c7
      D16MAC_AS_HW(xr2,xr3,xr10,xr4); //xr2: s1*c1+s3*c3+s5*c5,
                                      // xr4: s1*c3-s3*c7-s5*c1
      D16MAC_AS_LW(xr2,xr3,xr8,xr4);  //xr2: s1*c1+s3*c3+s5*c5+s7*c7,
                                      //xr4: s1*c3-s3*c7-s5*c1-s7*c5
      D32SAR(xr2, xr2,xr4,xr4,15);
      S32SFL(xr0, xr2,xr4,xr2,ptn3);
      D32SAR(xr11, xr11,xr13,xr13,15);
      S32SFL(xr0, xr11,xr13,xr11,ptn3);
      D32SAR(xr12,xr12,xr14,xr14,15);
      S32SFL(xr0, xr12,xr14,xr12,ptn3);
      D16MUL_HW(xr4, xr1, xr8, xr15);     //xr4: s1*c7, xr15:s1*c5
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);    //xr4: s1*c7-s3*c5, xr15: s1*c5-s3*c1
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);     //xr4: s1*c7-s3*c5+s5*c3, xr15: s1*c5-s3*c1+s5*c7
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);     //xr4: s1*c7-s3*c5+s5*c3-s7*c1
                                          //xr15: s1*c5-s3*c1+s5*c7+s7*c3
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);  //xr11: rnd(s0*c4+s4*c4)>>15+rnd(s2*c2+s6*c6)>>15
                                          //      rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15
                                          //xr12: rnd(s0*c4+s4*c4)>>15-rnd(s2*c2+s6*c6)>>15
                                          //      rnd(s0*c4-s4*c4)>>15-rnd(s2*c6-s6*c2)>>15
      D32SAR(xr15,xr15,xr4,xr4,15);
      S32SFL(xr0,xr15,xr4,xr15,ptn3);
      Q16ADD_AS_WW(xr11, xr11, xr2, xr2);
              //xr11: rnd(s0*c4+s4*c4)>>15+rnd(s2*c2+s6*c6)>>15 + rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>15
              //    : rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15 + rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>15
              //xr2: rnd(s0*c4+s4*c4)>>15+rnd(s2*c2+s6*c6)>>15 - rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>15
              //   : rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15 - rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>15

      Q16ADD_AS_XW(xr12, xr12, xr15, xr15);
              //xr12: rnd(s0*c4+s4*c4)>>15-rnd(s2*c2+s6*c6)>>15+rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>15
              //    : rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15+rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>15
              //xr15: rnd(s0*c4+s4*c4)>>15-rnd(s2*c2+s6*c6)>>15-rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>15
              //    : rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15-rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>15

      S32SFL(xr11,xr11,xr12,xr12, ptn3);
              //xr11: rnd(s0*c4+s4*c4)>>15+rnd(s2*c2+s6*c6)>>15 + rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>15
              //    : rnd(s0*c4+s4*c4)>>15-rnd(s2*c2+s6*c6)>>15+rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>15
              //xr12: rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15 + rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>15
              //    : rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15+rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>15
      S32SFL(xr12,xr12,xr11,xr11, ptn3);
              //xr12: rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15 + rnd(s1*c3-s3*c7-s5*c1-s7*c5)>>15
              //    : rnd(s0*c4+s4*c4)>>15+rnd(s2*c2+s6*c6)>>15 + rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>15
              //xr11: rnd(s0*c4-s4*c4)>>15+rnd(s2*c6-s6*c2)>>15+rnd(s1*c7-s3*c5+s5*c3-s7*c1)>>15
              //    : rnd(s0*c4+s4*c4)>>15-rnd(s2*c2+s6*c6)>>15+rnd(s1*c5-s3*c1+s5*c7+s7*c3)>>15
      S32STD(xr12, blk, 0x0);
      S32STD(xr11, blk, 0x4);
      S32STD(xr15, blk, 0x8);
      S32STD(xr2 , blk, 0xc);
    }

    blk = input - 2;
    for (i = 0; i < 4; i++)               /* idct columns */
    {
      S32LDI(xr1, blk, 0x4);   //xr1: ss0, s0
      S32LDD(xr3, blk, 0x20);  //xr3: ss2, s2
      S32I2M(xr5,wxr5);        //xr5: c4 , c2
      S32LDD(xr11, blk, 0x40); //xr11: ss4, s4
      S32LDD(xr13, blk, 0x60); //xr13: ss6, s6

      D16MUL_HW(xr15, xr5, xr1, xr9);    //xr15: ss0*c4, xr9: s0*c4
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);   //xr15: ss0*c4+ss4*c4, xr9: s0*c4+s4*c4
      D32SAR(xr15,xr15,xr9,xr9,15);
      S32SFL(xr0,xr15,xr9,xr15,ptn3);    //xr15: (ss0*c4+ss4*c4)>>15
                                         //    : (s0*c4+s4*c4)>>15
      D16MUL_LW(xr10, xr5, xr3, xr9);    //xr10: ss2*c2, xr9: s2*c2
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);   //xr10: ss2*c2+ss6*c6, xr9: s2*c2+s6*c6
      D32SAR(xr10,xr10,xr9,xr9,15);
      S32SFL(xr0,xr10,xr9,xr10,ptn3);
      S32LDD(xr2, blk, 0x10);            //xr2: ss1, s1
      S32LDD(xr4, blk, 0x30);            //xr4: ss3, s3
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);  //xr15: rnd(ss0*c4+ss4*c4)>>15+rnd(ss2*c2+ss6*c6)>>15
                                         //    :rnd(s0*c4+s4*c4)>>15 + rnd(s2*c2 + s6*c6)>>15
                                         //xr9: rnd(ss0*c4+ss4*c4)>>15 - rnd(ss2*c2+ss6*c6)>>15
                                         //   : rnd(s0*c4+s4*c4)>>15 - rnd(s2*c2 + s6*c6)>>15
      D16MUL_HW(xr10, xr5, xr1, xr1);    //xr10: ss0*c4, xr1: s0*c4
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);   //xr10: ss0*c4-ss4*c4, xr1: s0*c4 - s4*c4
      D32SAR(xr10,xr10,xr1,xr1,15);
      S32SFL(xr0,xr10,xr1,xr10,ptn3);
      D16MUL_LW(xr11, xr6, xr3, xr1);    //xr11: ss2*c6, xr1: s2*c6
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);   //xr11: ss2*c6-ss6*c2, xr1: s2*c6-s6*c2
      D32SAR(xr11,xr11,xr1,xr1,15);
      S32SFL(xr0,xr11,xr1,xr11,ptn3);
      S32LDD(xr12, blk, 0x50);           //xr12: ss5, s5
      S32LDD(xr14, blk, 0x70);           //xr14: ss7, s7
      Q16ADD_AS_WW(xr10,xr10,xr11,xr1);  //xr10: rnd(ss0*c4-ss4*c4)>>15)+rnd(ss2*c6-ss6*c2)>>15
                                         //    : rnd(s0*c4 - s4*c4)>>15 +rnd(s2*c6 - s6*c2)>>15
                                         //xr1 : rnd(ss0*c4-ss4*c4)>>15-rnd(ss2*c6-ss6*c2)>>15
                                         //    : rnd(s0*c4 - s4*c4)>>15-rnd(s2*c6 - s6*c2)>>15

      D16MUL_HW(xr11, xr7, xr2, xr13);   //xr11: ss1*c1, xr13: s1*c1
      D16MAC_AA_LW(xr11,xr7,xr4,xr13);   //xr11: ss1*c1+ss3*c3, xr13: s1*c1+s3*c3
      D16MAC_AA_LW(xr11,xr8,xr12,xr13);  //xr11: ss1*c1+ss3*c3+ss5*c5
                                         //xr13: s1*c1+s3*c3+s5*c5
      D16MAC_AA_HW(xr11,xr8,xr14,xr13);  //xr11: ss1*c1+ss3*c3+ss5*c5+ss7*c7
                                         //xr13: s1*c1+s3*c3+s5*c5+s7*c7
      D32SAR(xr11,xr11,xr13,xr13,15);
      S32SFL(xr0,xr11,xr13,xr11,ptn3);

      D16MUL_LW(xr3, xr7, xr2, xr13);    //xr3: ss1*c3, xr13: s1*c3
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);    //xr3: ss1*c3-ss3*c7, xr13: s1*c3-s3*c7
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1
                                         //xr13: s1*c3-s3*c7-s5*c1
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1-ss7*c5
                                         //xr13: s1*c3-s3*c7-s7*c5
      D32SAR(xr3,xr3,xr13,xr13,15);
      S32SFL(xr0,xr3,xr13,xr3,ptn3);
      D16MUL_LW(xr5, xr8, xr2, xr13);    //xr5: ss1*c5, xr13:s1*c5
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);    //xr5: ss1*c5-ss3*c1, xr13:s1*c5-s3*c1
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7
                                         //   : s1*c5 - s3*c1+ s5*c7
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7+ss7*c1
                                         //   : s1*c5 - s3*c1+ s5*c7+ s7*c1
      D32SAR(xr5,xr5,xr13,xr13,15);
      S32SFL(xr0,xr5,xr13,xr5,ptn3);

      D16MUL_HW(xr2, xr8, xr2, xr13);    //xr2: ss1*c7, xr13: s1*c7
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);    //xr2: ss1*c7-ss3*c5, xr13: s1*c7-s3*c5
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1
                                         //xr13: s1*c7-s3*c5+s5*c1
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1-ss7*c3
                                         //xr13: s1*c7-s3*c5+s5*c1-s7*c3
      D32SAR(xr2,xr2,xr13,xr13,15);
      S32SFL(xr0,xr2,xr13,xr2,ptn3);
      Q16ADD_AS_WW(xr15,xr15,xr11,xr11); //xr15:rnd(ss0*c4+ss4*c4)>>15+rnd(ss2*c2+ss6*c6)>>15+
                                         //     rnd(ss1*c1+ss3*c3+ss5*c5+ss7*c7)>>15
                                         //     rnd(s0*c4+s4*c4)>>15 + rnd(s2*c2 + s6*c6)>>15+
                                         //     rnd(s1*c1+s3*c3+s5*c5+s7*c7)>>15
                                         //xr11:rnd(ss0*c4+ss4*c4)>>15+rnd(ss2*c2+ss6*c6)>>15-
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
      S32I2M(xr4, 0x08080808); //0x808=16*128+8
      Q16ACCM_AA(xr15,xr4,xr4,xr10);
      Q16SAR(xr15,xr15,xr10,xr10,4);
      Q16ACCM_AA(xr11,xr4,xr4,xr1);
      Q16SAR(xr11,xr11,xr1,xr1,4);
      Q16ACCM_AA(xr9,xr4,xr4,xr2);
      Q16SAR(xr9,xr9,xr2,xr2,4);
      Q16ACCM_AA(xr5,xr4,xr4,xr3);
      Q16SAR(xr5,xr5,xr3,xr3,4);

      S32STD(xr15, blk, 0x00);
      S32STD(xr10, blk, 0x10);
      S32STD(xr1, blk, 0x20);
      S32STD(xr9, blk, 0x30);
      S32STD(xr2, blk, 0x40);
      S32STD(xr5, blk, 0x50);
      S32STD(xr3, blk, 0x60);
      S32STD(xr11, blk, 0x70);
    }
    

    

    blk = input -8;
    dst -= stride;
    for (i=0; i< 8; i++) {
     S32LDI(xr1, blk, 0x10);
     S32LDD(xr2, blk, 0x4);
     S32LDD(xr3, blk, 0x8);
     S32LDD(xr4, blk, 0xc);
     Q16SAT(xr5, xr2, xr1);
     Q16SAT(xr6, xr4, xr3);
     S32SDIV(xr5, dst, stride, 0x0);
     S32STD(xr6, dst, 0x4);
    }
}
#else
void ff_vp3_idct_put_c(uint8_t *dest/*align 8*/, int line_size, DCTELEM *block/*align 16*/){
    idct(dest, line_size, block, 1);
}

#endif
#ifdef JZ4750_OPT
void ff_vp3_idct_add_c(uint8_t *dst/*align 8*/, int stride, DCTELEM *input/*align 16*/){
    int a0, a1, a2, a3;
    int b0, b1, b2, b3;
    int i, j;
    int16_t  *blk;
    int32_t wf = (int32_t)whirl_idct;

    S32LDD(xr5, wf, 0x0);         // xr5(w7, w3)
    S32LDD(xr6, wf, 0x4);         // xr6(w9, w8)
    S32LDD(xr7, wf, 0x8);         // xr7(w11,w10)
    S32LDD(xr8, wf, 0xc);         // xr8(w13,w12)
    S32LDD(xr9, wf, 0x10);        // xr9(w6, w0)
    S32LDD(xr10,wf, 0x14);
    blk = input - 8;
    /* Inverse DCT on the rows now */
    for (i = 0; i < 8; i++) {
      S32LDI(xr1, blk, 0x10);       //  xr1 (x4, x0)
      S32LDD(xr2, blk, 0x4);        //  xr2 (x7, x3)
      S32LDD(xr3, blk, 0x8);        //  xr3 (x6, x1)
      S32LDD(xr4, blk, 0xc);        //  xr4 (x5, x2)
      S32OR(xr12, xr2,xr3);
      S32OR(xr11,xr12,xr4);
      S32OR(xr12,xr11,xr1);
      if (S32M2I(xr12)==0) {
             continue;            //blk[0]= blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=0
      }
      S32SFL(xr12,xr0,xr1,xr13,ptn3);
      S32OR(xr11,xr11,xr12);
      if (S32M2I(xr11)==0&&S32M2I(xr13)!=0) {
         D16MUL_HW(xr0,xr5,xr13,xr13);
         D32SAR(xr0,xr0,xr13,xr13,15);
         S32SFL(xr0,xr13,xr13,xr13,ptn3);
         S32STD(xr13,blk, 0x0);
         S32STD(xr13,blk, 0x4);
         S32STD(xr13,blk, 0x8);
         S32STD(xr13,blk, 0xc);
         continue;            //blk[0]!=0, and blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=0
      }
      S32SFL(xr1,xr1,xr2,xr2, ptn3);  //xr1:s1, s3, xr2: s0, s2
      S32SFL(xr3,xr3,xr4,xr4, ptn3);  //xr3:s5, s7, xr4: s4, s6

      D16MUL_WW(xr11, xr2, xr5, xr12);//xr11: s0*c4, xr12: s2*c2
      D16MAC_AA_WW(xr11,xr4,xr6,xr12);//xr11: s0*c4+s4*c4, xr12: s2*c2+s6*c6

      D16MUL_WW(xr13, xr2, xr6, xr14);//xr13: s0*c4, xr14: s2*c6
      D16MAC_SS_WW(xr13,xr4,xr5,xr14);//xr13: s0*c4 - s4*c4, xr14: s2*c6-s6*c2

      D16MUL_HW(xr2, xr1, xr7, xr4);  //xr2: s1*c1, xr4: s1*c3 
      D16MAC_AS_LW(xr2,xr1,xr9,xr4);  //xr2: s1*c1+s3*c3, xr4: s1*c3-s3*c7
      D16MAC_AS_HW(xr2,xr3,xr10,xr4); //xr2: s1*c1+s3*c3+s5*c5,
                                      // xr4: s1*c3-s3*c7-s5*c1
      D16MAC_AS_LW(xr2,xr3,xr8,xr4);  //xr2: s1*c1+s3*c3+s5*c5+s7*c7,
                                      //xr4: s1*c3-s3*c7-s5*c1-s7*c5
      D32SAR(xr2, xr2,xr4,xr4,15);
      S32SFL(xr0, xr2,xr4,xr2,ptn3);
      D32SAR(xr11, xr11,xr13,xr13,15);
      S32SFL(xr0, xr11,xr13,xr11,ptn3);
      D32SAR(xr12,xr12,xr14,xr14,15);
      S32SFL(xr0, xr12,xr14,xr12,ptn3);
      D16MUL_HW(xr4, xr1, xr8, xr15);     //xr4: s1*c7, xr15:s1*c5
      D16MAC_SS_LW(xr4,xr1,xr10,xr15);    //xr4: s1*c7-s3*c5, xr15: s1*c5-s3*c1
      D16MAC_AA_HW(xr4,xr3,xr9,xr15);     //xr4: s1*c7-s3*c5+s5*c3, xr15: s1*c5-s3*c1+s5*c7
      D16MAC_SA_LW(xr4,xr3,xr7,xr15);     //xr4: s1*c7-s3*c5+s5*c3-s7*c1
                                          //xr15: s1*c5-s3*c1+s5*c7+s7*c3
      Q16ADD_AS_WW(xr11,xr11,xr12,xr12);  //xr11: rnd(s0*c4+s4*c4)>>16+rnd(s2*c2+s6*c6)>>16
                                          //      rnd(s0*c4-s4*c4)>>16+rnd(s2*c6-s6*c2)>>16
                                          //xr12: rnd(s0*c4+s4*c4)>>16-rnd(s2*c2+s6*c6)>>16
                                          //      rnd(s0*c4-s4*c4)>>16-rnd(s2*c6-s6*c2)>>16
      D32SAR(xr15,xr15,xr4,xr4,15);
      S32SFL(xr0,xr15,xr4,xr15,ptn3);
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
    blk = input - 2;
    for (i = 0; i < 4; i++)               /* idct columns */
    {
      S32LDI(xr1, blk, 0x4);   //xr1: ss0, s0
      S32LDD(xr3, blk, 0x20);  //xr3: ss2, s2
      S32I2M(xr5,wxr5);        //xr5: c4 , c2
      S32LDD(xr11, blk, 0x40); //xr11: ss4, s4
      S32LDD(xr13, blk, 0x60); //xr13: ss6, s6

      D16MUL_HW(xr15, xr5, xr1, xr9);    //xr15: ss0*c4, xr9: s0*c4
      D16MAC_AA_HW(xr15,xr5,xr11,xr9);   //xr15: ss0*c4+ss4*c4, xr9: s0*c4+s4*c4
      D32SAR(xr15,xr15,xr9,xr9,15);      
      S32SFL(xr0,xr15,xr9,xr15,ptn3);    //xr15: (ss0*c4+ss4*c4)>>15
                                         //    : (s0*c4+s4*c4)>>15
      D16MUL_LW(xr10, xr5, xr3, xr9);    //xr10: ss2*c2, xr9: s2*c2
      D16MAC_AA_LW(xr10,xr6,xr13,xr9);   //xr10: ss2*c2+ss6*c6, xr9: s2*c2+s6*c6
      D32SAR(xr10,xr10,xr9,xr9,15);      
      S32SFL(xr0,xr10,xr9,xr10,ptn3);    //xr10: (ss2*c2+ss6*c6)>>15
                                         //    : (s2*c2+s6*c6)>>15
      S32LDD(xr2, blk, 0x10);            //xr2: ss1, s1
      S32LDD(xr4, blk, 0x30);            //xr4: ss3, s3
      Q16ADD_AS_WW(xr15,xr15,xr10,xr9);  //xr15: rnd(ss0*c4+ss4*c4)>>15+rnd(ss2*c2+ss6*c6)>>15
                                         //    :rnd(s0*c4+s4*c4)>>15 + rnd(s2*c2 + s6*c6)>>15
                                         //xr9: rnd(ss0*c4+ss4*c4)>>15 - rnd(ss2*c2+ss6*c6)>>15
                                         //   : rnd(s0*c4+s4*c4)>>15 - rnd(s2*c2 + s6*c6)>>15
      D16MUL_HW(xr10, xr5, xr1, xr1);    //xr10: ss0*c4, xr1: s0*c4
      D16MAC_SS_HW(xr10,xr5,xr11,xr1);   //xr10: ss0*c4-ss4*c4, xr1: s0*c4 - s4*c4
      D32SAR(xr10,xr10,xr1,xr1,15);
      S32SFL(xr0,xr10,xr1,xr10,ptn3);    //xr10: (ss0*c4-ss4*c4)>>15
                                         //    : (s0*c4 - s4*c4)>>15
      D16MUL_LW(xr11, xr6, xr3, xr1);    //xr11: ss2*c6, xr1: s2*c6
      D16MAC_SS_LW(xr11,xr5,xr13,xr1);   //xr11: ss2*c6-ss6*c2, xr1: s2*c6-s6*c2
      D32SAR(xr11,xr11,xr1,xr1,15);      
      S32SFL(xr0,xr11,xr1,xr11,ptn3);    //xr11:(ss2*c6-ss6*c2)>>15
                                         //    :(s2*c6-s6*c2)>>15
      S32LDD(xr12, blk, 0x50);           //xr12: ss5, s5
      S32LDD(xr14, blk, 0x70);           //xr14: ss7, s7
      Q16ADD_AS_WW(xr10,xr10,xr11,xr1);  //xr10: rnd(ss0*c4-ss4*c4)>>15)+rnd(ss2*c6-ss6*c2)>>15
                                         //    : rnd(s0*c4 - s4*c4)>>15 +rnd(s2*c6 - s6*c2)>>15
                                         //xr1 : rnd(ss0*c4-ss4*c4)>>15-rnd(ss2*c6-ss6*c2)>>15
                                         //    : rnd(s0*c4 - s4*c4)>>15-rnd(s2*c6 - s6*c2)>>15

      D16MUL_HW(xr11, xr7, xr2, xr13);   //xr11: ss1*c1, xr13: s1*c1
      D16MAC_AA_LW(xr11,xr7,xr4,xr13);   //xr11: ss1*c1+ss3*c3, xr13: s1*c1+s3*c3
      D16MAC_AA_LW(xr11,xr8,xr12,xr13);  //xr11: ss1*c1+ss3*c3+ss5*c5
                                         //xr13: s1*c1+s3*c3+s5*c5
      D16MAC_AA_HW(xr11,xr8,xr14,xr13);  //xr11: ss1*c1+ss3*c3+ss5*c5+ss7*c7
                                         //xr13: s1*c1+s3*c3+s5*c5+s7*c7
      D32SAR(xr11,xr11,xr13,xr13,15); 
      S32SFL(xr0,xr11,xr13,xr11,ptn3);   //xr11: (ss1*c1+ss3*c3+ss5*c5+ss7*c7)>>15
                                         //    : (s1*c1+s3*c3+s5*c5+s7*c7)>>15

      D16MUL_LW(xr3, xr7, xr2, xr13);    //xr3: ss1*c3, xr13: s1*c3
      D16MAC_SS_HW(xr3,xr8,xr4,xr13);    //xr3: ss1*c3-ss3*c7, xr13: s1*c3-s3*c7
      D16MAC_SS_HW(xr3,xr7,xr12,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1
                                         //xr13: s1*c3-s3*c7-s5*c1
      D16MAC_SS_LW(xr3,xr8,xr14,xr13);   //xr3: ss1*c3-ss3*c7-ss5*c1-ss7*c5
                                         //xr13: s1*c3-s3*c7-s7*c5
      D32SAR(xr3,xr3,xr13,xr13,15);
      S32SFL(xr0,xr3,xr13,xr3,ptn3);     //xr3: (ss1*c3-ss3*c7-ss5*c1-ss7*c5)>>15
                                         //   : (s1*c3-s3*c7-s7*c5)>>15
      D16MUL_LW(xr5, xr8, xr2, xr13);    //xr5: ss1*c5, xr13:s1*c5
      D16MAC_SS_HW(xr5,xr7,xr4,xr13);    //xr5: ss1*c5-ss3*c1, xr13:s1*c5-s3*c1
      D16MAC_AA_HW(xr5,xr8,xr12,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7
                                         //   : s1*c5 - s3*c1+ s5*c7
      D16MAC_AA_LW(xr5,xr7,xr14,xr13);   //xr5: ss1*c5-ss3*c1+ss5*c7+ss7*c1
                                         //   : s1*c5 - s3*c1+ s5*c7+ s7*c1
      D32SAR(xr5,xr5,xr13,xr13,15);
      S32SFL(xr0,xr5,xr13,xr5,ptn3);     //xr5: (ss1*c5-ss3*c1+ss5*c7+ss7*c1)>>15
                                         //   :(s1*c5 - s3*c1+ s5*c7+ s7*c1)>>15

      D16MUL_HW(xr2, xr8, xr2, xr13);    //xr2: ss1*c7, xr13: s1*c7
      D16MAC_SS_LW(xr2,xr8,xr4,xr13);    //xr2: ss1*c7-ss3*c5, xr13: s1*c7-s3*c5
      D16MAC_AA_LW(xr2,xr7,xr12,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1
                                         //xr13: s1*c7-s3*c5+s5*c1
      D16MAC_SS_HW(xr2,xr7,xr14,xr13);   //xr2: ss1*c7-ss3*c5+ss5*c1-ss7*c3
                                         //xr13: s1*c7-s3*c5+s5*c1-s7*c3
      D32SAR(xr2,xr2,xr13,xr13,15);
      S32SFL(xr0,xr2,xr13,xr2,ptn3);     //xr2:(ss1*c7-ss3*c5+ss5*c1-ss7*c3)>>15
                                         //   :(s1*c7-s3*c5+s5*c1-s7*c3)>>15
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
      S32I2M(xr4, 0x00080008);//round value 8;
      Q16ACCM_AA(xr15,xr4,xr4,xr10);
      Q16SAR(xr15,xr15,xr10,xr10,4);
      Q16ACCM_AA(xr11,xr4,xr4,xr1);
      Q16SAR(xr11,xr11,xr1,xr1,4);
      Q16ACCM_AA(xr9,xr4,xr4,xr2);
      Q16SAR(xr9,xr9,xr2,xr2,4);
      Q16ACCM_AA(xr5,xr4,xr4,xr3);
      Q16SAR(xr5,xr5,xr3,xr3,4);

      S32STD(xr15, blk, 0x00);
      S32STD(xr10, blk, 0x10);
      S32STD(xr1, blk, 0x20);
      S32STD(xr9, blk, 0x30);
      S32STD(xr2, blk, 0x40);
      S32STD(xr5, blk, 0x50);
      S32STD(xr3, blk, 0x60);
      S32STD(xr11, blk, 0x70);
    }

    blk = input -8;
    dst -= stride;
    for (i=0; i< 8; i++) {
      S32LDIV(xr5, dst, stride, 0x0);
      S32LDI(xr1, blk, 0x10);
      S32LDD(xr2, blk, 0x4);
      Q8ACCE_AA(xr2, xr5, xr0, xr1);
      S32LDD(xr6, dst, 0x4);
      S32LDD(xr3, blk, 0x8);
      S32LDD(xr4, blk, 0xc);
      Q8ACCE_AA(xr4, xr6, xr0, xr3);
      Q16SAT(xr5, xr2, xr1);
      S32STD(xr5, dst, 0x0);
      Q16SAT(xr6, xr4, xr3);
      S32STD(xr6, dst, 0x4);
    }
}
#else
void ff_vp3_idct_add_c(uint8_t *dest/*align 8*/, int line_size, DCTELEM *block/*align 16*/){
    idct(dest, line_size, block, 2);
}
#endif
