/*
 * H.264 IDCT
 * Copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
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
 * @file h264-idct.c
 * H.264 IDCT.
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

#include "dsputil.h"

#ifdef JZ4740_MXU_OPT
#include "jzmedia.h"
#endif

static av_always_inline void idct_internal(uint8_t *dst, DCTELEM *block, int stride, int block_stride, int shift, int add){
    int i;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    block[0] += 1<<(shift-1);

    for(i=0; i<4; i++){
        const int z0=  block[0 + block_stride*i]     +  block[2 + block_stride*i];
        const int z1=  block[0 + block_stride*i]     -  block[2 + block_stride*i];
        const int z2= (block[1 + block_stride*i]>>1) -  block[3 + block_stride*i];
        const int z3=  block[1 + block_stride*i]     + (block[3 + block_stride*i]>>1);

        block[0 + block_stride*i]= z0 + z3;
        block[1 + block_stride*i]= z1 + z2;
        block[2 + block_stride*i]= z1 - z2;
        block[3 + block_stride*i]= z0 - z3;
    }

    for(i=0; i<4; i++){
        const int z0=  block[i + block_stride*0]     +  block[i + block_stride*2];
        const int z1=  block[i + block_stride*0]     -  block[i + block_stride*2];
        const int z2= (block[i + block_stride*1]>>1) -  block[i + block_stride*3];
        const int z3=  block[i + block_stride*1]     + (block[i + block_stride*3]>>1);

        dst[i + 0*stride]= cm[ add*dst[i + 0*stride] + ((z0 + z3) >> shift) ];
        dst[i + 1*stride]= cm[ add*dst[i + 1*stride] + ((z1 + z2) >> shift) ];
        dst[i + 2*stride]= cm[ add*dst[i + 2*stride] + ((z1 - z2) >> shift) ];
        dst[i + 3*stride]= cm[ add*dst[i + 3*stride] + ((z0 - z3) >> shift) ];
    }
}

#ifdef JZ4740_MXU_OPT
void ff_h264_idct_add_c(uint8_t *dst, DCTELEM *block, int stride){
//    idct_internal(dst, block, stride, 4, 6, 1);
    int i;
    DCTELEM *blk = block;

    block[0] += 1<<(6-1);

/*
    for(i=0; i<4; i++){
        const int z0=  blk[0]     +  blk[2];
        const int z1=  blk[0]     -  blk[2];
        const int z2= (blk[1]>>1) -  blk[3];
        const int z3=  blk[1]     + (blk[3]>>1);

        blk[0]= z0 + z3;
        blk[1]= z1 + z2;
        blk[2]= z1 - z2;
        blk[3]= z0 - z3;
        blk += 4;
    }
*/
// Loop 0,1
// load data blk[0~7]
    S32LDD(xr1, blk, 0x00);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x04);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x08);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x0c);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x00);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x04);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x08);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x0c);    //  xr4 (Nblk3, Nblk2)

// Loop 2,3
// load data blk[0~7]
    S32LDD(xr1, blk, 0x10);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x14);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x18);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x1c);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x10);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x14);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x18);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x1c);    //  xr4 (Nblk3, Nblk2)
//
/*
    for(i=0; i<4; i++){
        const int z0=  block[i + 4*0]     +  block[i + 4*2];
        const int z1=  block[i + 4*0]     -  block[i + 4*2];
        const int z2= (block[i + 4*1]>>1) -  block[i + 4*3];
        const int z3=  block[i + 4*1]     + (block[i + 4*3]>>1);

        dst[i + 0*stride]= cm[dst[i + 0*stride] + ((z0 + z3) >> 6) ];
        dst[i + 1*stride]= cm[dst[i + 1*stride] + ((z1 + z2) >> 6) ];
        dst[i + 2*stride]= cm[dst[i + 2*stride] + ((z1 - z2) >> 6) ];
        dst[i + 3*stride]= cm[dst[i + 3*stride] + ((z0 - z3) >> 6) ];
    }
*/
// Loop 0,1
// load data and do shift op
    S32LDD(xr10, blk, 0x0);     //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x8);     //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x10);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x18);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr11, xr6, xr9, xr12);// xr11 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                       // xr12 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr13, xr7, xr8, xr14);  //xr13 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                         //xr14 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr11,xr11,xr12,xr12, 6);
    Q16SAR(xr13,xr13,xr14,xr14, 6);

// Loop 2,3
// load data and do shift op
    S32LDD(xr10, blk, 0x4);     //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0xc);     //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x14);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x1c);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr1, xr6, xr9, xr2);// xr1 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr2 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr3, xr7, xr8, xr4);//xr3 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                     //xr4 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr1,xr1,xr2,xr2, 6);
    Q16SAR(xr3,xr3,xr4,xr4, 6);

// load dst data and do add op
// row 0
    S32LDD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr1, xr5, xr0, xr11);  // xr11 (b1, b0) xr1 (b3, b2)
    Q16SAT(xr5, xr1, xr11);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 1
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr3, xr5, xr0, xr13);  // xr13 (b1, b0) xr3 (b3, b2)
    Q16SAT(xr5, xr3, xr13);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 2
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr4, xr5, xr0, xr14);  // xr14 (b1, b0) xr4 (b3, b2)
    Q16SAT(xr5, xr4, xr14);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 3
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr2, xr5, xr0, xr12);  // xr12 (b1, b0) xr2 (b3, b2)
    Q16SAT(xr5, xr2, xr12);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
}
#else
void ff_h264_idct_add_c(uint8_t *dst, DCTELEM *block, int stride){
    idct_internal(dst, block, stride, 4, 6, 1);
}
#endif

#ifdef __LINUX__
#ifdef JZ4740_MXU_OPT
void ff_h264_lowres_idct_add_c(uint8_t *dst, int stride, DCTELEM *block){
    //idct_internal(dst, block, stride, 8, 3, 1);
    int i;
    DCTELEM *blk = block;

    block[0] += 1<<(3-1);
/*
    for(i=0; i<4; i++){
        const int z0=  blk[0]     +  blk[2];
        const int z1=  blk[0]     -  blk[2];
        const int z2= (blk[1]>>1) -  blk[3];
        const int z3=  blk[1]     + (blk[3]>>1);

        blk[0]= z0 + z3;
        blk[1]= z1 + z2;
        blk[2]= z1 - z2;
        blk[3]= z0 - z3;
        blk += 8;
    }
*/
// Loop 0,1
// load data blk[0~7]
    S32LDD(xr1, blk, 0x00);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x04);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x10);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x14);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x00);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x04);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x10);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x14);    //  xr4 (Nblk3, Nblk2)

// Loop 2,3
// load data blk[0~7]
    S32LDD(xr1, blk, 0x20);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x24);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x30);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x34);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x20);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x24);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x30);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x34);    //  xr4 (Nblk3, Nblk2)
//
/*
    for(i=0; i<4; i++){
        const int z0=  block[i + 8*0]     +  block[i + 8*2];
        const int z1=  block[i + 8*0]     -  block[i + 8*2];
        const int z2= (block[i + 8*1]>>1) -  block[i + 8*3];
        const int z3=  block[i + 8*1]     + (block[i + 8*3]>>1);

        dst[i + 0*stride]= cm[ dst[i + 0*stride] + ((z0 + z3) >> 3) ];
        dst[i + 1*stride]= cm[ dst[i + 1*stride] + ((z1 + z2) >> 3) ];
        dst[i + 2*stride]= cm[ dst[i + 2*stride] + ((z1 - z2) >> 3) ];
        dst[i + 3*stride]= cm[ dst[i + 3*stride] + ((z0 - z3) >> 3) ];
    }
*/
// Loop 0,1
// load data and do shift op
    S32LDD(xr10, blk, 0x00);    //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x10);    //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x20);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x30);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr11, xr6, xr9, xr12);// xr11 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                       // xr12 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr13, xr7, xr8, xr14);  //xr13 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                         //xr14 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr11,xr11,xr12,xr12, 3);
    Q16SAR(xr13,xr13,xr14,xr14, 3);

// Loop 2,3
// load data and do shift op
    S32LDD(xr10, blk, 0x04);     //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x14);     //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x24);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x34);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr1, xr6, xr9, xr2);// xr1 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr2 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr3, xr7, xr8, xr4);//xr3 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                     //xr4 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr1,xr1,xr2,xr2, 3);
    Q16SAR(xr3,xr3,xr4,xr4, 3);

// load dst data and do add op
// row 0
    S32LDD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr1, xr5, xr0, xr11);  // xr11 (b1, b0) xr1 (b3, b2)
    Q16SAT(xr5, xr1, xr11);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 1
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr3, xr5, xr0, xr13);  // xr13 (b1, b0) xr3 (b3, b2)
    Q16SAT(xr5, xr3, xr13);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 2
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr4, xr5, xr0, xr14);  // xr14 (b1, b0) xr4 (b3, b2)
    Q16SAT(xr5, xr4, xr14);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 3
    S32LDIV(xr5, dst, stride, 0);     // Xr5 (b3, b2, b1, b0)
    Q8ACCE_AA(xr2, xr5, xr0, xr12);  // xr12 (b1, b0) xr2 (b3, b2)
    Q16SAT(xr5, xr2, xr12);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
}
#else
void ff_h264_lowres_idct_add_c(uint8_t *dst, int stride, DCTELEM *block){
    idct_internal(dst, block, stride, 8, 3, 1);
}
#endif

#ifdef JZ4740_MXU_OPT
void ff_h264_lowres_idct_put_c(uint8_t *dst, int stride, DCTELEM *block){
//    idct_internal(dst, block, stride, 8, 3, 0);
    int i;
    DCTELEM *blk = block;

    block[0] += 1<<(3-1);

// Loop 0,1
// load data blk[0~7]
    S32LDD(xr1, blk, 0x00);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x04);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x10);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x14);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x00);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x04);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x10);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x14);    //  xr4 (Nblk3, Nblk2)

// Loop 2,3
// load data blk[0~7]
    S32LDD(xr1, blk, 0x20);    //  xr1 (x1, x0)
    S32LDD(xr2, blk, 0x24);    //  xr2 (x3, x2)
    S32LDD(xr3, blk, 0x30);    //  xr3 (Nx1, Nx0)
    S32LDD(xr4, blk, 0x34);    //  xr4 (Nx3, Nx2)
// data reorder and shift
    S32SFL(xr7,xr1,xr3,xr5, ptn3);    // xr5 (x0, Nx0), xr7(x1, Nx1)
    S32SFL(xr8,xr2,xr4,xr6, ptn3);    // xr6 (x2, Nx2), xr8(x3, Nx3)
    Q16SAR(xr9,xr7,xr8,xr10, 1);      // xr9 (x1>>1, Nx1>>1), xr10(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr1, xr5, xr6, xr2); // xr1 (x0+x2, Nx0+Nx2) xr2(x0-x2, Nx0-Nx2)
                                      // xr1 (z0, Nz0) xr2 (z1, Nz1)
    Q16ADD_SA_WW(xr3, xr9, xr8, xr0); // xr3 (x1>>1 - x3, Nx1>>1 - Nx3) xr3(z2, Nz2)
    Q16ADD_AA_WW(xr4, xr7, xr10, xr0);// xr4 (x1 + x3>>1, Nx1 + Nx3>>1) xr4(z3, Nz3)

// calculate result
    Q16ADD_AS_WW(xr5, xr1, xr4, xr6);// xr5 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr6 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr7, xr2, xr3, xr8);// xr7 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
// reorder data
    S32SFL(xr1,xr7,xr5,xr3, ptn3);    // xr1 (blk1, blk0), xr3(Nblk1, Nblk0)
    S32SFL(xr2,xr6,xr8,xr4, ptn3);    // xr2 (blk3, blk2), xr4(Nblk3, Nblk2)
//store data
    S32STD(xr1, blk, 0x20);    //  xr1 (blk1, blk0)
    S32STD(xr2, blk, 0x24);    //  xr2 (blk3, blk2)
    S32STD(xr3, blk, 0x30);    //  xr3 (Nblk1, Nblk0)
    S32STD(xr4, blk, 0x34);    //  xr4 (Nblk3, Nblk2)
//
/*
    for(i=0; i<4; i++){
        const int z0=  block[i + 8*0]     +  block[i + 8*2];
        const int z1=  block[i + 8*0]     -  block[i + 8*2];
        const int z2= (block[i + 8*1]>>1) -  block[i + 8*3];
        const int z3=  block[i + 8*1]     + (block[i + 8*3]>>1);

        dst[i + 0*stride]= cm[(z0 + z3) >> 3 ];
        dst[i + 1*stride]= cm[(z1 + z2) >> 3 ];
        dst[i + 2*stride]= cm[(z1 - z2) >> 3 ];
        dst[i + 3*stride]= cm[(z0 - z3) >> 3 ];
    }
*/
// Loop 0,1
// load data and do shift op
    S32LDD(xr10, blk, 0x00);    //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x10);    //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x20);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x30);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr11, xr6, xr9, xr12);// xr11 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                       // xr12 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr13, xr7, xr8, xr14);  //xr13 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                         //xr14 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr11,xr11,xr12,xr12, 3);
    Q16SAR(xr13,xr13,xr14,xr14, 3);

// Loop 2,3
// load data and do shift op
    S32LDD(xr10, blk, 0x04);     //  xr10 (x0, Nx0)
    S32LDD(xr1,  blk, 0x14);     //  xr1  (x1, Nx1)
    S32LDD(xr2,  blk, 0x24);    //  xr2  (x2, Nx2)
    S32LDD(xr3,  blk, 0x34);    //  xr3  (x3, Nx3)
    Q16SAR(xr4,xr1,xr3,xr5, 1); //  xr4  (x1>>1, Nx1>>1), xr5(x3>>1, Nx3>>1)
// calculate z0 ~ z3
    Q16ADD_AS_WW(xr6, xr10, xr2, xr7); // xr6 (x0+x2, Nx0+Nx2) xr7(x0-x2, Nx0-Nx2)
                                       // xr6 (z0, Nz0) xr7 (z1, Nz1)
    Q16ADD_SA_WW(xr8, xr4, xr3, xr0);  // xr8 (x1>>1 - x3, Nx1>>1 - Nx3) xr8(z2, Nz2)
    Q16ADD_AA_WW(xr9, xr1, xr5, xr0);  // xr9 (x1 + x3>>1, Nx1 + Nx3>>1) xr9(z3, Nz3)
// calculate result
    Q16ADD_AS_WW(xr1, xr6, xr9, xr2);// xr1 (z0 + z3, Nz0 + Nz3) (blk0, Nblk0)
                                     // xr2 (z0 - z3, Nz0 - Nz3) (blk3, Nblk3)
    Q16ADD_AS_WW(xr3, xr7, xr8, xr4);//xr3 (z1 + z2, Nz1 + Nz2) (blk1, Nblk1)
                                     //xr4 (z1 - z2, Nz1 - Nz2) (blk2, Nblk2)
    Q16SAR(xr1,xr1,xr2,xr2, 3);
    Q16SAR(xr3,xr3,xr4,xr4, 3);

// load dst data and do add op
// row 0
    Q16SAT(xr5, xr1, xr11);
    S32STD(xr5, dst, 0x0);     //  xr5 (b3, b2, b1, b0)
// row 1
    Q16SAT(xr5, xr3, xr13);
    S32SDIV(xr5, dst, stride, 0);     //  xr5 (b3, b2, b1, b0)
// row 2
    Q16SAT(xr5, xr4, xr14);
    S32SDIV(xr5, dst, stride, 0);     //  xr5 (b3, b2, b1, b0)
// row 3
    Q16SAT(xr5, xr2, xr12);
    S32SDIV(xr5, dst, stride, 0);     //  xr5 (b3, b2, b1, b0)
}
#else
void ff_h264_lowres_idct_put_c(uint8_t *dst, int stride, DCTELEM *block){
    idct_internal(dst, block, stride, 8, 3, 0);
}
#endif
#endif   //#ifndef USE_16M_SDRAM

void ff_h264_idct8_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int i;
    DCTELEM (*src)[8] = (DCTELEM(*)[8])block;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;

    block[0] += 32;

    for( i = 0; i < 8; i++ )
    {
        const int a0 =  src[i][0] + src[i][4];
        const int a2 =  src[i][0] - src[i][4];
        const int a4 = (src[i][2]>>1) - src[i][6];
        const int a6 = (src[i][6]>>1) + src[i][2];

        const int b0 = a0 + a6;
        const int b2 = a2 + a4;
        const int b4 = a2 - a4;
        const int b6 = a0 - a6;

        const int a1 = -src[i][3] + src[i][5] - src[i][7] - (src[i][7]>>1);
        const int a3 =  src[i][1] + src[i][7] - src[i][3] - (src[i][3]>>1);
        const int a5 = -src[i][1] + src[i][7] + src[i][5] + (src[i][5]>>1);
        const int a7 =  src[i][3] + src[i][5] + src[i][1] + (src[i][1]>>1);

        const int b1 = (a7>>2) + a1;
        const int b3 =  a3 + (a5>>2);
        const int b5 = (a3>>2) - a5;
        const int b7 =  a7 - (a1>>2);

        src[i][0] = b0 + b7;
        src[i][7] = b0 - b7;
        src[i][1] = b2 + b5;
        src[i][6] = b2 - b5;
        src[i][2] = b4 + b3;
        src[i][5] = b4 - b3;
        src[i][3] = b6 + b1;
        src[i][4] = b6 - b1;
    }
    for( i = 0; i < 8; i++ )
    {
        const int a0 =  src[0][i] + src[4][i];
        const int a2 =  src[0][i] - src[4][i];
        const int a4 = (src[2][i]>>1) - src[6][i];
        const int a6 = (src[6][i]>>1) + src[2][i];

        const int b0 = a0 + a6;
        const int b2 = a2 + a4;
        const int b4 = a2 - a4;
        const int b6 = a0 - a6;

        const int a1 = -src[3][i] + src[5][i] - src[7][i] - (src[7][i]>>1);
        const int a3 =  src[1][i] + src[7][i] - src[3][i] - (src[3][i]>>1);
        const int a5 = -src[1][i] + src[7][i] + src[5][i] + (src[5][i]>>1);
        const int a7 =  src[3][i] + src[5][i] + src[1][i] + (src[1][i]>>1);

        const int b1 = (a7>>2) + a1;
        const int b3 =  a3 + (a5>>2);
        const int b5 = (a3>>2) - a5;
        const int b7 =  a7 - (a1>>2);

        dst[i + 0*stride] = cm[ dst[i + 0*stride] + ((b0 + b7) >> 6) ];
        dst[i + 1*stride] = cm[ dst[i + 1*stride] + ((b2 + b5) >> 6) ];
        dst[i + 2*stride] = cm[ dst[i + 2*stride] + ((b4 + b3) >> 6) ];
        dst[i + 3*stride] = cm[ dst[i + 3*stride] + ((b6 + b1) >> 6) ];
        dst[i + 4*stride] = cm[ dst[i + 4*stride] + ((b6 - b1) >> 6) ];
        dst[i + 5*stride] = cm[ dst[i + 5*stride] + ((b4 - b3) >> 6) ];
        dst[i + 6*stride] = cm[ dst[i + 6*stride] + ((b2 - b5) >> 6) ];
        dst[i + 7*stride] = cm[ dst[i + 7*stride] + ((b0 - b7) >> 6) ];
    }
}

#ifdef JZ4740_MXU_OPT
// assumes all AC coefs are 0
void ff_h264_idct_dc_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int dc = (block[0] + 32) >> 6;
/*
    int i, j;
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    for( j = 0; j < 4; j++ )
    {
        for( i = 0; i < 4; i++ )
            dst[i] = cm[ dst[i] + dc ];
        dst += stride;
    }
*/
    S32I2M(xr1, dc);
    S32SFL(xr0,xr1,xr1,xr2, ptn3);    // Xr2 (dc, dc)
    S32SFL(xr12,xr2,xr2,xr13, ptn3);  // Xr12 (dc, dc) Xr13 (dc, dc)

//Loop 0
//i = 0 - 3
    S32LDD(xr1, dst, 0);              // Xr1 (b3, b2, b1, b0)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//Loop 1
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//Loop 2
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//Loop 3
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);
}
#else
// assumes all AC coefs are 0
void ff_h264_idct_dc_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int i, j;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    int dc = (block[0] + 32) >> 6;
    for( j = 0; j < 4; j++ )
    {
        for( i = 0; i < 4; i++ )
            dst[i] = cm[ dst[i] + dc ];
        dst += stride;
    }
}
#endif

#ifdef JZ4740_MXU_OPT
void ff_h264_idct8_dc_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int dc = (block[0] + 32) >> 6;
/*
    int i, j;
    uint8_t *cm = cropTbl + MAX_NEG_CROP;
    for( j = 0; j < 8; j++ )
    {
        for( i = 0; i < 8; i++ )
            dst[i] = cm[ dst[i] + dc ];
        dst += stride;
    }
*/

    S32I2M(xr1, dc);
    S32SFL(xr0,xr1,xr1,xr2, ptn3);    // Xr2 (dc, dc)
    S32SFL(xr12,xr2,xr2,xr13, ptn3);  // Xr12 (dc, dc) Xr13 (dc, dc)

//Loop 0
//i = 0 - 3
    S32LDD(xr1, dst, 0);              // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 1
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 2
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 3
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 4
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 5
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 6
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);

//Loop 7
//i = 0 - 3
    S32LDIV(xr1, dst, stride, 0);     // Xr1 (b3, b2, b1, b0)
    S32LDD(xr3, dst, 4);              // Xr3 (b7, b6, b5, b4)
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr1,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr1,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr1, dst, 0);

//i = 4 - 7
    D32SLL(xr4,xr12,xr13,xr5,0);      // move (xr12, xr13) to (xr4,xr5)
    Q8ACCE_AA(xr4,xr3,xr0,xr5);    // Xr4 (b3 + dc, b2 + dc)
                                      // Xr5 (b1 + dc, b0 + dc)
    Q16SAT(xr3,xr4,xr5);              // Xr4 (b3, b2, b1, b0)
    S32STD(xr3, dst, 4);
}
#else
// assumes all AC coefs are 0
void ff_h264_idct8_dc_add_c(uint8_t *dst, DCTELEM *block, int stride){
    int i, j;
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;
    int dc = (block[0] + 32) >> 6;
    for( j = 0; j < 8; j++ )
    {
        for( i = 0; i < 8; i++ )
            dst[i] = cm[ dst[i] + dc ];
        dst += stride;
    }
}
#endif
