/*
 * DSP utils
 * Copyright (c) 2000, 2001 Fabrice Bellard.
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * gmc & q-pel & 32/64 bit based MC by Michael Niedermayer <michaelni@gmx.at>
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
 * @file dsputil.c
 * DSP utils
 */
#if defined(_UCOS_)
#include "../../../config.h"
#endif
#include "avcodec.h"
#include "dsputil.h"
#include "mpegvideo.h"
#include "simple_idct.h"
#include "faandct.h"
#include "h263.h"
#include "snow.h"
#include "jzsoc/jzasm.h"
#ifdef JZ4740_MXU_OPT
#include "jzmedia.h"
#endif

#ifdef JZ4740_MXU_OPT
//put_pixels2_mxu (need not op by MXU)
static void put_pixels4_mxu(uint8_t *dst, const uint8_t *src, int stride, int h){
  uint32_t src_aln,src_rs;
  int i;
  src_aln = (uint32_t)src & 0xfffffffc;
  src_rs = 4 - ((uint32_t)src & 3);

  dst -= stride;
  for(i=0; i<h; i++) {
    S32LDD(xr1,src_aln,0);
    S32LDD(xr2,src_aln,4);
    src_aln += stride;

    S32ALN(xr1,xr2,xr1,src_rs);  //xr1 <- src[3:0]
  
    S32SDIV(xr1,dst,stride,0);
  }
}

static void put_pixels8_mxu(uint8_t *dst, const uint8_t *src, int stride, int h){
  uint32_t src_aln,src_rs;
  int i;
  src_aln = (uint32_t)src & 0xfffffffc;
  src_rs = 4 - ((uint32_t)src & 3);

  dst -= stride;
  src_aln -= stride;
  for(i=0; i<h; i++) {
    S32LDIV(xr1,src_aln,stride,0);
    S32LDD(xr2,src_aln,4);
    S32LDD(xr3,src_aln,8);

    S32ALN(xr1,xr2,xr1,src_rs);  //xr1 <- src[3:0]
    S32ALN(xr2,xr3,xr2,src_rs);  //xr2 <- src[7:4]
  
    S32SDIV(xr1,dst,stride,0);
    S32STD(xr2,dst,4);
  }
}

static void put_pixels16_mxu(uint8_t *dst, const uint8_t *src, int stride, int h){

  uint32_t src_aln,src_rs;
  int i;
  src_aln = (uint32_t)src & 0xfffffffc;
  src_rs = 4 - ((uint32_t)src & 3);

  dst -= stride;
  src_aln -= stride;
  for(i=0; i<h; i++) {
    S32LDIV(xr1,src_aln,stride,0);
    S32LDD(xr2,src_aln,4);
    S32LDD(xr3,src_aln,8);
    S32LDD(xr4,src_aln,12);
    S32LDD(xr5,src_aln,16);

    S32ALN(xr1,xr2,xr1,src_rs);  //xr1 <- src[3:0]
    S32ALN(xr2,xr3,xr2,src_rs);  //xr2 <- src[7:4]
    S32ALN(xr3,xr4,xr3,src_rs);  //xr3 <- src[11:8]
    S32ALN(xr4,xr5,xr4,src_rs);  //xr2 <- src[16:12]
  
    S32SDIV(xr1,dst,stride,0);
    S32STD(xr2,dst,4);
    S32STD(xr3,dst,8);
    S32STD(xr4,dst,12);
  }
}
#endif

#define PIXOP2(OPNAME, OP) \
static void OPNAME ## _pixels2_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    int i;\
    for(i=0; i<h; i++){\
        OP(*((uint16_t*)(block  )), AV_RN16(pixels  ));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
static void OPNAME ## _pixels4_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    int i;\
    for(i=0; i<h; i++){\
        OP(*((uint32_t*)(block  )), AV_RN32(pixels  ));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
static void OPNAME ## _pixels8_c(uint8_t *block, const uint8_t *pixels, int line_size, int h){\
    int i;\
    for(i=0; i<h; i++){\
        OP(*((uint32_t*)(block  )), AV_RN32(pixels  ));\
        OP(*((uint32_t*)(block+4)), AV_RN32(pixels+4));\
        pixels+=line_size;\
        block +=line_size;\
    }\
}\
static inline void OPNAME ## _pixels8_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a,b;\
        a= AV_RN32(&src1[i*src_stride1  ]);\
        b= AV_RN32(&src2[i*src_stride2  ]);\
        OP(*((uint32_t*)&dst[i*dst_stride  ]), rnd_avg32(a, b));\
        a= AV_RN32(&src1[i*src_stride1+4]);\
        b= AV_RN32(&src2[i*src_stride2+4]);\
        OP(*((uint32_t*)&dst[i*dst_stride+4]), rnd_avg32(a, b));\
    }\
}\
static inline void OPNAME ## _pixels4_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a,b;\
        a= AV_RN32(&src1[i*src_stride1  ]);\
        b= AV_RN32(&src2[i*src_stride2  ]);\
        OP(*((uint32_t*)&dst[i*dst_stride  ]), rnd_avg32(a, b));\
    }\
}\
\
static inline void OPNAME ## _pixels2_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    int i;\
    for(i=0; i<h; i++){\
        uint32_t a,b;\
        a= AV_RN16(&src1[i*src_stride1  ]);\
        b= AV_RN16(&src2[i*src_stride2  ]);\
        OP(*((uint16_t*)&dst[i*dst_stride  ]), rnd_avg32(a, b));\
    }\
}\
static inline void OPNAME ## _pixels16_l2(uint8_t *dst, const uint8_t *src1, const uint8_t *src2, int dst_stride, \
                                                int src_stride1, int src_stride2, int h){\
    OPNAME ## _pixels8_l2(dst  , src1  , src2  , dst_stride, src_stride1, src_stride2, h);\
    OPNAME ## _pixels8_l2(dst+8, src1+8, src2+8, dst_stride, src_stride1, src_stride2, h);\
}\
CALL_2X_PIXELS(OPNAME ## _pixels16_c  , OPNAME ## _pixels8_c  , 8)

#define op_put(a, b) a = b
#define op_avg(a, b) a = rnd_avg32(a, b)
PIXOP2(put, op_put)
PIXOP2(avg, op_avg)
#undef op_avg
#undef op_put

#ifdef JZ4740_MXU_OPT
static void h264_v_loop_filter_luma_mxu(uint8_t *pix, int xstride, int alpha, int beta, int8_t *tc0)
{
    int i, d;
    uint8_t  *tpix;
    S32I2M (xr14, beta);
    S32SFL (xr0,  xr14, xr14, xr14, ptn0);
    S32SFL (xr0,  xr14, xr14, xr14, ptn3);   // xr14: beta
    S32I2M (xr13, alpha);
    S32SFL (xr0,  xr13, xr13, xr13, ptn0);
    S32SFL (xr0,  xr13, xr13, xr13, ptn3);   // xr13: alpha

    for( i = 0; i < 4; i++ ) {
        int t0 = tc0[i];
        if( t0 < 0 ) {
            pix += 4;
            continue;
        }
    S32I2M (xr15, t0);
    S32SFL (xr0,  xr15, xr15, xr15, ptn0);
    S32SFL (xr0,  xr15, xr15, xr15, ptn3);  // xr15: tc0[i]
    tpix = pix - 4*xstride;

    S32LDIV(xr12, tpix, xstride, 0);  //-3, p2
    S32LDIV(xr11, tpix, xstride, 0);  //-2, p1
    S32LDIV(xr10, tpix, xstride, 0);  //-1, p0
    S32LDIV(xr9, tpix, xstride, 0);   // 0, q0
    S32LDIV(xr1, tpix, xstride, 0);   // 1, q1
    S32LDIV(xr2, tpix, xstride, 0);   // 2, q2
    Q8ABD(xr3, xr10, xr9 );   // FFABS (p0 - q0)
    Q8ABD(xr4, xr11, xr10);   // FFABS (p1 - p0)
    Q8ABD(xr5, xr1,  xr9 );   // FFABS (q1 - q0)
// FFABS(p0 - q0) - alpha, FFABS(p1 - p0) - beta
    Q8ADDE_SS (xr6, xr3, xr13,  xr7);  // FFABS(p0 - q0) - alpha
    Q8ADDE_SS (xr3, xr4, xr14,  xr4);  // FFABS(p1 - p0) - beta
    Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( p0 - q0 ) < alpha)
    Q16SLR (xr3, xr3, xr4, xr4, 15);   // 1: < 0 (FFABS( p1 - p0 ) < beta)
    Q16SAT (xr4, xr3, xr4);   // xr4: 1: < 0 (FFABS( p1 - p0 ) < beta)
    Q16SAT (xr3, xr6, xr7);   // xr3: 1: < 0 (FFABS( p0 - q0 ) < alpha)
// FFABS(q1 - q0) - beta
    Q8ADDE_SS (xr6, xr5, xr14,  xr7);  // FFABS(q1 - q0) - beta
    Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( q1 - q0 ) < beta)
    Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q1 - q0 ) < beta)

// if( FFABS( p0 - q0 ) < alpha && ..)
    Q8MADL_AA (xr0, xr3, xr4, xr3);
    Q8ABD(xr6, xr12, xr10 );  // FFABS (p2 - p0)
    Q8ABD(xr7, xr2,  xr9 );   // FFABS (q2 - q0)
    Q8MADL_AA (xr0, xr3, xr5, xr3);    // xr3: (FFABS( p0 - q0 ) < alpha 
                                       //      && FFABS( p1 - p0 ) < beta && ...)
    Q8ADDE_SS (xr4, xr6, xr14,  xr5);  // FFABS(p2 - p0) - beta
    Q8ADDE_SS (xr6, xr7, xr14,  xr7);  // FFABS(q2 - q0) - beta
    Q8MADL_AA (xr0, xr3, xr15, xr15);  // xr15: new clip value
    Q16SLR (xr4, xr4, xr5, xr5, 15);   // 1: < 0  FFABS(p2 - p0) - beta
    Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0  FFABS(q2 - q0) - beta
    Q16SAT (xr4, xr4, xr5);   // xr4: 1: < 0 (FFABS( p2 - p0 ) < beta)
    Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q2 - q0 ) < beta)
// calculate clip value
    Q8ADD_AA  (xr6, xr4, xr5);         // pre-calculate for tc++, tc++
    Q8MADL_AA (xr0, xr6, xr3, xr6);    // xr6: new clip differ
    Q8MADL_AA (xr0, xr4, xr15, xr4);   // xr4: new clip value for p1
    Q8MADL_AA (xr0, xr5, xr15, xr5);   // xr5: new clip value for q1
    Q8ADD_AA  (xr3, xr6, xr15);        // xr3: new clip value for p0,q0
//
    Q8AVGR (xr6,  xr9, xr10);  // (p0 + q0 + 1) >> 1
    Q8AVG  (xr12, xr6, xr12);  // (p2 + (p0 + q0 + 1) >> 1) >> 1
    Q8AVG  (xr2,  xr6, xr2);   // (q2 + (p0 + q0 + 1) >> 1) >> 1
    Q8ADDE_SS (xr12, xr12, xr11, xr7); // (p2 + (p0 + q0 + 1) >> 1) >> 1 - p1
    Q8ADDE_SS (xr2,  xr2,  xr1,  xr8); // (q2 + (p0 + q0 + 1) >> 1) >> 1 - q1
    Q8ADDE_SS (xr6,  xr0, xr4, xr15);      //-tc
//av_clip( (( p2 + ( ( p0 + q0 + 1 ) >> 1 ) ) >> 1) - p1, -tc0[i], tc0[i] );
    D16MAX (xr12, xr12, xr6);
    D16MAX (xr7,  xr7,  xr15);
    Q8ADDE_AA (xr6,  xr0, xr4, xr15);      //tc
    D16MIN (xr12, xr12, xr6);
    D16MIN (xr7,  xr7,  xr15);
    Q8ADDE_SS (xr6,  xr0, xr5, xr15);      //-tc
    S32SFL (xr0, xr12, xr7, xr12, ptn1);
//av_clip( (( q2 + ( ( p0 + q0 + 1 ) >> 1 ) ) >> 1) - q1, -tc0[i], tc0[i] );
    D16MAX (xr2, xr2, xr6);
    D16MAX (xr8, xr8, xr15);
    Q8ADDE_AA (xr6,  xr0, xr5, xr15);      //tc
    D16MIN (xr2, xr2, xr6);
    D16MIN (xr8, xr8, xr15);
// p1 + ...; q1 + ...
    Q8ADD_AA  (xr4, xr12, xr11); // p1 + ...     //final p1
    S32SFL (xr0, xr2, xr8, xr2, ptn1);
    Q8ADD_AA  (xr5, xr2, xr1);    // q1 + ...    //final q1
//
//  (q0 - p0 ) << 2 + (p1 - q1)
    Q8ADDE_SS (xr2, xr9, xr10, xr12);   // q0 - p0
    Q16SLL (xr2, xr2, xr12, xr12, 2);   // (q0 - p0) << 2
    Q8ACCE_SS (xr2, xr11, xr1, xr12);    // (q0 - p0) << 2 + (p1 - q1)
//  -tc, tc
    Q8ADDE_AA (xr11, xr0, xr3, xr15);     //+tc (xr11, xr15)
    Q8ADDE_SS (xr1,  xr0, xr3, xr6);      //-tc (xr1,  xr6)
//  i_delta = av_clip (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 ....)
    Q16SAR (xr2, xr2, xr12, xr12, 2);   // ((q0 - p0) << 2 + (p1 - q1) + 0) >> 2
    D16AVGR(xr2,  xr2,  xr0);
    D16AVGR(xr12, xr12, xr0);           // ((q0 - p0) << 2 + (p1 - q1) + 1) >> 1
    D16MAX (xr2,  xr2,  xr1);
    D16MAX (xr12, xr12, xr6);
    D16MIN (xr2,  xr2,  xr11);
    D16MIN (xr12, xr12, xr15);
    Q16ADD_SS_WW (xr1,  xr0, xr2,  xr0);    // xr1  = -xr2
    Q16ADD_SS_WW (xr11, xr0, xr12, xr0);    // xr11 = -xr12
//  pix[-xstride]=...; pix[0]=...;
    Q8ACCE_AA (xr2, xr0, xr10, xr12);    // (p0 + i_delta)
    Q8ACCE_AA (xr1, xr0, xr9,  xr11);    // (q0 - i_delta)
    tpix = pix - 2*xstride;
    Q16SAT    (xr10, xr2, xr12);         // final p0
    Q16SAT    (xr9, xr1, xr11);          // final q0
// store
    S32STD (xr4, tpix, 0);               // p1
    S32SDIV(xr10, tpix, xstride, 0);     //-1, p0
    S32SDIV(xr9,  tpix, xstride, 0);     //0, q0
    S32SDIV(xr5,  tpix, xstride, 0);     //0, q1
    pix += 4;
    }
}
static void h264_h_loop_filter_luma_mxu(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0)
{
    int i, d, t0;
    uint8_t *tpix;
    unsigned int t1, t2, t3, t4;
    S32I2M (xr14, beta);
    S32SFL (xr0,  xr14, xr14, xr14, ptn0);
    S32SFL (xr0,  xr14, xr14, xr14, ptn3);   // xr14: beta
    S32I2M (xr13, alpha); 
    S32SFL (xr0,  xr13, xr13, xr13, ptn0);
    S32SFL (xr0,  xr13, xr13, xr13, ptn3);   // xr13: alpha 
    for( i = 0; i < 4; i++ ) {
        t0 = tc0[i];
        if( t0 < 0 ) {
            pix += 4*stride;
            continue;
        }
        S32I2M (xr15, t0);
        S32SFL (xr0,  xr15, xr15, xr15, ptn0);
        S32SFL (xr0,  xr15, xr15, xr15, ptn3);  // xr15: tc0[i]
//
        tpix = pix;
        S32LDD  (xr2, tpix,  0);          // xr2: q3,q2,q1,q0
        S32LDD  (xr1, tpix, -4);          // xr1: p0,p1,p2,p3
        S32LDIV (xr4, tpix, stride, 0);    // xr4: q3',q2',q1',q0'
        S32LDD  (xr3, tpix, -4);          // xr3: p0',p1',p2',p3'
//
	S32SFL  (xr6, xr4, xr2, xr8, ptn0); // xr6: q3'q3q2'q2, xr8:q1'q1q0'q0
	S32SFL  (xr5, xr3, xr1, xr7, ptn0); // xr5: p0'p0p1'p1, xr7:p2'p2p3'p3
//
        S32LDIV (xr2, tpix, stride, 0);    // xr2: q3",q2",q1",q0"
        S32LDD  (xr1, tpix, -4);          // xr1: p0",p1",p2",p3"
        S32LDIV (xr4, tpix, stride, 0);    // xr4: q3^,q2^,q1^,q0^
        S32LDD  (xr3, tpix, -4);          // xr3: p0^,p1^,p2^,p3^
//
	S32SFL  (xr4, xr4, xr2, xr2, ptn0); // xr4: q3^q3"q2^q2", xr2:q1^q1"q0^q0"
	S32SFL  (xr3, xr3, xr1, xr1, ptn0); // xr3: p0^p0"p1^p1", xr1:p2^p2"p3^p3"
//
        S32SFL  (xr10, xr3, xr5, xr11, ptn3); // xr10: p0^p0"p0'p0, xr11:p1^p1"p1'p1
        S32SFL  (xr12, xr1, xr7, xr0,  ptn3); // xr12: p2^p2"p2'p2
        S32SFL  (xr1,  xr2, xr8, xr9,  ptn3); // xr1: q1^q1"q1'q1, xr9:q0^q0"q0'q0
        S32SFL  (xr0,  xr4, xr6, xr2,  ptn3); // xr2: q2^q2"q2'q2
//
        Q8ABD(xr3, xr10, xr9 );   // FFABS (p0 - q0)
        Q8ABD(xr4, xr11, xr10);   // FFABS (p1 - p0)
        Q8ABD(xr5, xr1,  xr9 );   // FFABS (q1 - q0)
// FFABS(p0 - q0) - alpha, FFABS(p1 - p0) - beta
        Q8ADDE_SS (xr6, xr3, xr13,  xr7);  // FFABS(p0 - q0) - alpha
        Q8ADDE_SS (xr3, xr4, xr14,  xr4);  // FFABS(p1 - p0) - beta
        Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( p0 - q0 ) < alpha)
        Q16SLR (xr3, xr3, xr4, xr4, 15);   // 1: < 0 (FFABS( p1 - p0 ) < beta)
        Q16SAT (xr4, xr3, xr4);   // xr4: 1: < 0 (FFABS( p1 - p0 ) < beta)
        Q16SAT (xr3, xr6, xr7);   // xr3: 1: < 0 (FFABS( p0 - q0 ) < alpha)
// FFABS(q1 - q0) - beta
        Q8ADDE_SS (xr6, xr5, xr14,  xr7);  // FFABS(q1 - q0) - beta
        Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( q1 - q0 ) < beta)
        Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q1 - q0 ) < beta)

// if( FFABS( p0 - q0 ) < alpha && ..)
        Q8MADL_AA (xr0, xr3, xr4, xr3);
        Q8ABD(xr6, xr12, xr10 );  // FFABS (p2 - p0)
        Q8ABD(xr7, xr2,  xr9 );   // FFABS (q2 - q0)
        Q8MADL_AA (xr0, xr3, xr5, xr3);    // xr3: (FFABS( p0 - q0 ) < alpha
                                           //      && FFABS( p1 - p0 ) < beta && ...)
        Q8ADDE_SS (xr4, xr6, xr14,  xr5);  // FFABS(p2 - p0) - beta
        Q8ADDE_SS (xr6, xr7, xr14,  xr7);  // FFABS(q2 - q0) - beta
        Q8MADL_AA (xr0, xr3, xr15, xr15);  // xr15: new clip value
        Q16SLR (xr4, xr4, xr5, xr5, 15);   // 1: < 0  FFABS(p2 - p0) - beta
        Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0  FFABS(q2 - q0) - beta
        Q16SAT (xr4, xr4, xr5);   // xr4: 1: < 0 (FFABS( p2 - p0 ) < beta)
        Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q2 - q0 ) < beta)
// calculate clip value
        Q8ADD_AA  (xr6, xr4, xr5);         // pre-calculate for tc++, tc++
        Q8MADL_AA (xr0, xr6, xr3, xr6);    // xr6: new clip differ
        Q8MADL_AA (xr0, xr4, xr15, xr4);   // xr4: new clip value for p1
        Q8MADL_AA (xr0, xr5, xr15, xr5);   // xr5: new clip value for q1
        Q8ADD_AA  (xr3, xr6, xr15);        // xr3: new clip value for p0,q0
//
        Q8AVGR (xr6,  xr9, xr10);  // (p0 + q0 + 1) >> 1
        Q8AVG  (xr12, xr6, xr12);  // (p2 + (p0 + q0 + 1) >> 1) >> 1
        Q8AVG  (xr2,  xr6, xr2);   // (q2 + (p0 + q0 + 1) >> 1) >> 1
        Q8ADDE_SS (xr12, xr12, xr11, xr7); // (p2 + (p0 + q0 + 1) >> 1) >> 1 - p1
        Q8ADDE_SS (xr2,  xr2,  xr1,  xr8); // (q2 + (p0 + q0 + 1) >> 1) >> 1 - q1
        Q8ADDE_SS (xr6,  xr0, xr4, xr15);      //-tc
//av_clip( (( p2 + ( ( p0 + q0 + 1 ) >> 1 ) ) >> 1) - p1, -tc0[i], tc0[i] );
        D16MAX (xr12, xr12, xr6);
        D16MAX (xr7,  xr7,  xr15);
        Q8ADDE_AA (xr6,  xr0, xr4, xr15);      //tc
        D16MIN (xr12, xr12, xr6);
        D16MIN (xr7,  xr7,  xr15);
        Q8ADDE_SS (xr6,  xr0, xr5, xr15);      //-tc
        S32SFL (xr0, xr12, xr7, xr12, ptn1);
//av_clip( (( q2 + ( ( p0 + q0 + 1 ) >> 1 ) ) >> 1) - q1, -tc0[i], tc0[i] );
        D16MAX (xr2, xr2, xr6);
        D16MAX (xr8, xr8, xr15);
        Q8ADDE_AA (xr6,  xr0, xr5, xr15);      //tc
        D16MIN (xr2, xr2, xr6);
        D16MIN (xr8, xr8, xr15);
// p1 + ...; q1 + ...
        Q8ADD_AA  (xr4, xr12, xr11); // p1 + ...     //final p1
        S32SFL (xr0, xr2, xr8, xr2, ptn1);
        Q8ADD_AA  (xr5, xr2, xr1);    // q1 + ...    //final q1
//  (q0 - p0 ) << 2 + (p1 - q1)
        Q8ADDE_SS (xr2, xr9, xr10, xr12);   // q0 - p0
        Q16SLL (xr2, xr2, xr12, xr12, 2);   // (q0 - p0) << 2
        Q8ACCE_SS (xr2, xr11, xr1, xr12);    // (q0 - p0) << 2 + (p1 - q1)
//  -tc, tc
        Q8ADDE_AA (xr11, xr0, xr3, xr15);     //+tc (xr11, xr15)
        Q8ADDE_SS (xr1,  xr0, xr3, xr6);      //-tc (xr1,  xr6)
//  i_delta = av_clip (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 ....)
        Q16SAR (xr2, xr2, xr12, xr12, 2);   // ((q0 - p0) << 2 + (p1 - q1) + 0) >> 2
        D16AVGR(xr2,  xr2,  xr0);
        D16AVGR(xr12, xr12, xr0);           // ((q0 - p0) << 2 + (p1 - q1) + 1) >> 1
        D16MAX (xr2,  xr2,  xr1);
        D16MAX (xr12, xr12, xr6);
        D16MIN (xr2,  xr2,  xr11);
        D16MIN (xr12, xr12, xr15);
        Q16ADD_SS_WW (xr1,  xr0, xr2,  xr0);    // xr1  = -xr2
        Q16ADD_SS_WW (xr11, xr0, xr12, xr0);    // xr11 = -xr12
//
        Q8ACCE_AA (xr2, xr0, xr10, xr12);    // (p0 + i_delta)
        Q8ACCE_AA (xr1, xr0, xr9,  xr11);    // (q0 - i_delta)
        Q16SAT    (xr10, xr2, xr12);         // final p0
        Q16SAT    (xr9, xr1, xr11);          // final q0
// store
        S32SFL    (xr2, xr10, xr4, xr1, ptn0);   // xr2: p0^p1^p0"p1", xr1:p0'p1'p0p1
        S32SFL    (xr4, xr5,  xr9, xr3, ptn0);   // xr4: q1^q0^q1"q0", xr3:q1'q0'q1q0
        t1 = S32M2I(xr1);
        t2 = S32M2I(xr2);
        t3 = S32M2I(xr3);
        t4 = S32M2I(xr4);
        *((unsigned short *)pix - 1) = t1;
        *((unsigned short *)pix + 0) = t3;
        pix += stride;
        *((unsigned short *)pix - 1) = (t1 >> 16);
        *((unsigned short *)pix + 0) = (t3 >> 16);
        pix += stride;
        *((unsigned short *)pix - 1) = t2;
        *((unsigned short *)pix + 0) = t4;
        pix += stride;
        *((unsigned short *)pix - 1) = (t2 >> 16);
        *((unsigned short *)pix + 0) = (t4 >> 16);
        pix += stride;
    }
}
#else
static inline void h264_loop_filter_luma_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta, int8_t *tc0)
{
    int i, d;
    for( i = 0; i < 4; i++ ) {
        if( tc0[i] < 0 ) {
            pix += 4*ystride;
            continue;
        }
        for( d = 0; d < 4; d++ ) {
            const int p0 = pix[-1*xstride];
            const int p1 = pix[-2*xstride];
            const int p2 = pix[-3*xstride];
            const int q0 = pix[0];
            const int q1 = pix[1*xstride];
            const int q2 = pix[2*xstride];

            if( FFABS( p0 - q0 ) < alpha &&
                FFABS( p1 - p0 ) < beta &&
                FFABS( q1 - q0 ) < beta ) {

                int tc = tc0[i];
                int i_delta;

                if( FFABS( p2 - p0 ) < beta ) {
                    pix[-2*xstride] = p1 + av_clip( (( p2 + ( ( p0 + q0 + 1 ) >> 1 ) ) >> 1) - p1, -tc0[i], tc0[i] );
                    tc++;
                }
                if( FFABS( q2 - q0 ) < beta ) {
                    pix[   xstride] = q1 + av_clip( (( q2 + ( ( p0 + q0 + 1 ) >> 1 ) ) >> 1) - q1, -tc0[i], tc0[i] );
                    tc++;
                }

                i_delta = av_clip( (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3, -tc, tc );
                pix[-xstride] = av_clip_uint8( p0 + i_delta );    /* p0' */
                pix[0]        = av_clip_uint8( q0 - i_delta );    /* q0' */
            }
            pix += ystride;
        }
    }
}
static void h264_v_loop_filter_luma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0)
{
    h264_loop_filter_luma_c(pix, stride, 1, alpha, beta, tc0);
}
static void h264_h_loop_filter_luma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0)
{
    h264_loop_filter_luma_c(pix, 1, stride, alpha, beta, tc0);
}
#endif

#ifdef JZ4740_MXU_OPT
static void h264_v_loop_filter_chroma_mxu(uint8_t *pix, int xstride, int alpha, int beta, int8_t *tc0)
{
    uint8_t *tpix;
    int i, d;

    S32I2M (xr14, beta);
    S32SFL (xr0,  xr14, xr14, xr14, ptn0);
    S32SFL (xr0,  xr14, xr14, xr14, ptn3);   // xr14: beta
    S32I2M (xr13, alpha);
    S32SFL (xr0,  xr13, xr13, xr13, ptn0);
    S32SFL (xr0,  xr13, xr13, xr13, ptn3);   // xr13: alpha
    S32I2M (xr8,  0x01010101);

    for(i = 0; i < 2; i++) {
      const int tc = *((short *)tc0 + i);
//
      tpix = pix - 2*xstride;
      S32LDD (xr11, tpix, 0);            // p1
      S32LDIV(xr10, tpix, xstride, 0);   // p0
      S32LDIV(xr9,  tpix, xstride, 0);   // q0
      S32LDIV(xr1,  tpix, xstride, 0);   // q1
      Q8ABD(xr3, xr10, xr9 );   // FFABS (p0 - q0)
      Q8ABD(xr4, xr11, xr10);   // FFABS (p1 - p0)
      Q8ABD(xr5, xr1,  xr9 );   // FFABS (q1 - q0)
// FFABS(p0 - q0) - alpha, FFABS(p1 - p0) - beta
      Q8ADDE_SS (xr6, xr3, xr13,  xr7);  // FFABS(p0 - q0) - alpha
      Q8ADDE_SS (xr3, xr4, xr14,  xr4);  // FFABS(p1 - p0) - beta
      Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( p0 - q0 ) < alpha)
      Q16SLR (xr3, xr3, xr4, xr4, 15);   // 1: < 0 (FFABS( p1 - p0 ) < beta)
      Q16SAT (xr4, xr3, xr4);   // xr4: 1: < 0 (FFABS( p1 - p0 ) < beta
      Q16SAT (xr3, xr6, xr7);   // xr3: 1: < 0 (FFABS( p0 - q0 ) < alpha)
// FFABS(q1 - q0) - beta
      Q8ADDE_SS (xr6, xr5, xr14,  xr7);  // FFABS(q1 - q0) - beta
      Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( q1 - q0 ) < beta)
      Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q1 - q0 ) < beta)
// if( FFABS( p0 - q0 ) < alpha && ..)
      Q8MADL_AA (xr0, xr3, xr4, xr3);
// xr15 = tc
      S32I2M (xr2, tc);
      S32SFL (xr0, xr2, xr2, xr15, ptn0);
//
      Q8ADDE_SS (xr2, xr9, xr10, xr12);   // q0 - p0
      Q8ACCE_AA (xr2, xr0, xr8, xr12);    // (q0 - p0 + 1)
      Q8MADL_AA (xr0, xr3, xr5, xr3);    // xr3: (FFABS( p0 - q0 ) < alpha
                                         //      && FFABS( p1 - p0 ) < beta && ...)
      Q8MAX  (xr15, xr0, xr15);
      Q16SLL (xr2, xr2, xr12, xr12, 2);   // (q0 - p0) << 2 + 4
      Q8MADL_AA (xr0, xr3, xr15, xr15);    // xr15: (FFABS( p0 - q0 ) < alpha .... ) * tc
      Q8ACCE_SS (xr2, xr11, xr1, xr12);    // (q0 - p0) << 2 + (p1 - q1) + 4
//  -tc, tc
      Q8ADDE_AA (xr7, xr0, xr15, xr6);     //  tc1,  tc0
      Q8ADDE_SS (xr5, xr0, xr15, xr4);     // -tc1, -tc0
      Q16SAR (xr2, xr2, xr12, xr12, 3);    
      D16MAX (xr2,  xr2,  xr5);
      D16MAX (xr12, xr12, xr4);
      D16MIN (xr2,  xr2,  xr7);
      D16MIN (xr12, xr12, xr6);
// p0 + delta, p1 - delta
      Q16ADD_SS_WW (xr1,  xr0, xr2,  xr0);    // xr1  = -xr2     -delta
      Q16ADD_SS_WW (xr11, xr0, xr12, xr0);    // xr11 = -xr12    -delta
      Q8ACCE_AA (xr2, xr0, xr10, xr12);    // (p0 + i_delta)
      Q8ACCE_AA (xr1, xr0, xr9,  xr11);    // (q0 - i_delta)
      tpix = pix - 2*xstride;
      Q16SAT    (xr2, xr2, xr12);
      Q16SAT    (xr1, xr1, xr11);
      S32SDIV(xr2,  tpix, xstride, 0);      //-1, p0
      S32SDIV(xr1,  tpix, xstride, 0);      //0, q0
      pix += 4;
    }
}

static void h264_h_loop_filter_chroma_mxu(uint8_t *pix, int ystride, int alpha, int beta, int8_t *tc0)
{
    int i, d;
    uint8_t *tpix;
    unsigned int p0, q0;
    
    S32I2M (xr14, beta);
    S32SFL (xr0,  xr14, xr14, xr14, ptn0);
    S32SFL (xr0,  xr14, xr14, xr14, ptn3);   // xr14: beta
    S32I2M (xr13, alpha);
    S32SFL (xr0,  xr13, xr13, xr13, ptn0);
    S32SFL (xr0,  xr13, xr13, xr13, ptn3);   // xr13: alpha
    S32I2M (xr8,  0x01010101);

    for(i = 0; i < 2; i++) {
      const int tc = *((short *)tc0 + i);
//
      tpix = pix;
      S32LDD  (xr2, tpix,  0);          // xr2: q3,q2,q1,q0
      S32LDD  (xr1, tpix, -4);          // xr1: p0,p1,p2,p3
      S32LDIV (xr4, tpix, ystride, 0);  // xr4: q3',q2',q1',q0'
      S32LDD  (xr3, tpix, -4);          // xr3: p0',p1',p2',p3'
//
      S32SFL  (xr6, xr4, xr2, xr12, ptn0); // xr6: q3'q3q2'q2, xr12:q1'q1q0'q0
      S32SFL  (xr5, xr3, xr1, xr7, ptn0); // xr5: p0'p0p1'p1, xr7:p2'p2p3'p
//
      S32LDIV (xr2, tpix, ystride, 0);    // xr2: q3",q2",q1",q0"
      S32LDD  (xr1, tpix, -4);          // xr1: p0",p1",p2",p3"
      S32LDIV (xr4, tpix, ystride, 0);    // xr4: q3^,q2^,q1^,q0^
      S32LDD  (xr3, tpix, -4);          // xr3: p0^,p1^,p2^,p3^
//
      S32SFL  (xr4, xr4, xr2, xr2, ptn0); // xr4: q3^q3"q2^q2", xr2:q1^q1"q0^q0"
      S32SFL  (xr3, xr3, xr1, xr1, ptn0); // xr3: p0^p0"p1^p1", xr1:p2^p2"p3^p3"
//
      S32SFL  (xr10, xr3, xr5, xr11, ptn3); // xr10: p0^p0"p0'p0, xr11:p1^p1"p1'p1
      S32SFL  (xr1,  xr2, xr12, xr9,  ptn3); // xr1: q1^q1"q1'q1, xr9:q0^q0"q0'q0
//
      Q8ABD(xr3, xr10, xr9 );   // FFABS (p0 - q0)
      Q8ABD(xr4, xr11, xr10);   // FFABS (p1 - p0)
      Q8ABD(xr5, xr1,  xr9 );   // FFABS (q1 - q0)
//    FFABS(p0 - q0) - alpha, FFABS(p1 - p0) - beta
      Q8ADDE_SS (xr6, xr3, xr13,  xr7);  // FFABS(p0 - q0) - alpha
      Q8ADDE_SS (xr3, xr4, xr14,  xr4);  // FFABS(p1 - p0) - beta
      Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( p0 - q0 ) < alpha)
      Q16SLR (xr3, xr3, xr4, xr4, 15);   // 1: < 0 (FFABS( p1 - p0 ) < beta)
      Q16SAT (xr4, xr3, xr4);   // xr4: 1: < 0 (FFABS( p1 - p0 ) < beta
      Q16SAT (xr3, xr6, xr7);   // xr3: 1: < 0 (FFABS( p0 - q0 ) < alpha)
// FFABS(q1 - q0) - beta
      Q8ADDE_SS (xr6, xr5, xr14,  xr7);  // FFABS(q1 - q0) - beta
      Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( q1 - q0 ) < beta)
      Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q1 - q0 ) < beta)
// if( FFABS( p0 - q0 ) < alpha && ..)
      Q8MADL_AA (xr0, xr3, xr4, xr3);
// xr15 = tc
      S32I2M (xr2, tc);
      S32SFL (xr0, xr2, xr2, xr15, ptn0);
//
      Q8ADDE_SS (xr2, xr9, xr10, xr12);   // q0 - p0
      Q8ACCE_AA (xr2, xr0, xr8, xr12);    // (q0 - p0 + 1)
      Q8MADL_AA (xr0, xr3, xr5, xr3);     // xr3: (FFABS( p0 - q0 ) < alpha
                                          //      && FFABS( p1 - p0 ) < beta && ...)      
      Q8MAX  (xr15, xr0, xr15);
      Q16SLL (xr2, xr2, xr12, xr12, 2);   // (q0 - p0) << 2 + 4
      Q8MADL_AA (xr0, xr3, xr15, xr15);    // xr15: (FFABS( p0 - q0 ) < alpha .... ) * tc
      Q8ACCE_SS (xr2, xr11, xr1, xr12);    // (q0 - p0) << 2 + (p1 - q1) + 4
//  -tc, tc
      Q8ADDE_AA (xr7, xr0, xr15, xr6);     //  tc1,  tc0
      Q8ADDE_SS (xr5, xr0, xr15, xr4);     // -tc1, -tc0
      Q16SAR (xr2, xr2, xr12, xr12, 3);
      D16MAX (xr2,  xr2,  xr5);
      D16MAX (xr12, xr12, xr4);
      D16MIN (xr2,  xr2,  xr7);
      D16MIN (xr12, xr12, xr6);
// p0 + delta, p1 - delta
      Q16ADD_SS_WW (xr1,  xr0, xr2,  xr0);    // xr1  = -xr2     -delta
      Q16ADD_SS_WW (xr11, xr0, xr12, xr0);    // xr11 = -xr12    -delta
      Q8ACCE_AA (xr2, xr0, xr10, xr12);    // (p0 + i_delta)
      Q8ACCE_AA (xr1, xr0, xr9,  xr11);    // (q0 - i_delta)
//
      Q16SAT    (xr2, xr2, xr12);
      Q16SAT    (xr1, xr1, xr11);      
      p0 = S32M2I (xr2);
      q0 = S32M2I (xr1);
//
      pix[-1] = p0;
      pix[0]  = q0;
      pix += ystride;
      pix[-1] = (p0 >> 8);
      pix[0]  = (q0 >> 8);
      pix += ystride;
      pix[-1] = (p0 >> 16);
      pix[0]  = (q0 >> 16);
      pix += ystride;
      pix[-1] = (p0 >> 24);
      pix[0]  = (q0 >> 24);
      pix += ystride;
 }
}

#else
static inline void h264_loop_filter_chroma_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta, int8_t *tc0)
{
    int i, d;
    for( i = 0; i < 4; i++ ) {
        const int tc = tc0[i];
        if( tc <= 0 ) {
            pix += 2*ystride;
            continue;
        }
        for( d = 0; d < 2; d++ ) {
            const int p0 = pix[-1*xstride];
            const int p1 = pix[-2*xstride];
            const int q0 = pix[0];
            const int q1 = pix[1*xstride];

            if( FFABS( p0 - q0 ) < alpha &&
                FFABS( p1 - p0 ) < beta &&
                FFABS( q1 - q0 ) < beta ) {

                int delta = av_clip( (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3, -tc, tc );

                pix[-xstride] = av_clip_uint8( p0 + delta );    /* p0' */
                pix[0]        = av_clip_uint8( q0 - delta );    /* q0' */
            }
            pix += ystride;
        }
    }
}
static void h264_v_loop_filter_chroma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0)
{
    h264_loop_filter_chroma_c(pix, stride, 1, alpha, beta, tc0);
}
static void h264_h_loop_filter_chroma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0)
{
    h264_loop_filter_chroma_c(pix, 1, stride, alpha, beta, tc0);
}
#endif

#ifdef JZ4740_MXU_OPT
static void h264_v_loop_filter_chroma_intra_mxu(uint8_t *pix, int xstride, int alpha, int beta)
{
  int d;
  uint8_t *tpix;

  S32I2M (xr14, beta);
  S32SFL (xr0,  xr14, xr14, xr14, ptn0);
  S32SFL (xr0,  xr14, xr14, xr14, ptn3);   // xr14: beta
  S32I2M (xr13, alpha);
  S32SFL (xr0,  xr13, xr13, xr13, ptn0);
  S32SFL (xr0,  xr13, xr13, xr13, ptn3);   // xr13: alpha
  S32I2M (xr8, 0x01010101);

  for (d = 0; d < 2; d++)
  {
    tpix = pix - 2*xstride;
    S32LDD (xr11, tpix, 0);             // p1
    S32LDIV(xr10, tpix, xstride, 0);    // p0
    S32LDIV(xr9,  tpix, xstride, 0);    // q0
    S32LDIV(xr1,  tpix, xstride, 0);    // q1
    Q8ABD(xr3, xr10, xr9 );   // FFABS (p0 - q0)
    Q8ABD(xr4, xr11, xr10);   // FFABS (p1 - p0)
    Q8ABD(xr5, xr1,  xr9 );   // FFABS (q1 - q0)
// FFABS(p0 - q0) - alpha, FFABS(p1 - p0) - beta
    Q8ADDE_SS (xr6, xr3, xr13,  xr7);  // FFABS(p0 - q0) - alpha
    Q8ADDE_SS (xr3, xr4, xr14,  xr4);  // FFABS(p1 - p0) - beta
    Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( p0 - q0 ) < alpha)
    Q16SLR (xr3, xr3, xr4, xr4, 15);   // 1: < 0 (FFABS( p1 - p0 ) < beta)
    Q16SAT (xr4, xr3, xr4);   // xr4: 1: < 0 (FFABS( p1 - p0 ) < beta)
    Q16SAT (xr3, xr6, xr7);   // xr3: 1: < 0 (FFABS( p0 - q0 ) < alpha)
// FFABS(q1 - q0) - beta
    Q8ADDE_SS (xr6, xr5, xr14,  xr7);  // FFABS(q1 - q0) - beta
    Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( q1 - q0 ) < beta)
    Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q1 - q0 ) < beta)

// if( FFABS( p0 - q0 ) < alpha && ..)
    Q8MADL_AA (xr0, xr3, xr4, xr3);        // if( FFABS( p0 - q0 ) < alpha && ..)
//
    Q8ADDE_AA (xr12, xr11, xr8, xr7);      // (xr12, xr7): p1 + 1
    Q16SLL (xr12, xr12, xr7, xr7, 1);      // (xr12, xr7): 2*p1 + 2
    Q8ACCE_AA (xr12, xr10, xr1, xr7);      // (xr12, xr7): 2*p1 + p0 + q1 + 2
//
    Q8MADL_AA (xr0, xr3, xr5, xr3);        // if( FFABS( p0 - q0 ) < alpha && ... && ...)
//
    Q8ADDE_AA (xr2, xr1, xr8, xr6);        // (xr2,  xr6): q1 + 1
    Q16SLL (xr2, xr2, xr6, xr6, 1);        // (xr2,  xr6): 2*q1 + 2
    Q8ACCE_AA (xr2, xr9, xr11, xr6);       // (xr2,  xr6): 2*q1 + q0 + p1 + 2
//
    Q16SAR (xr12, xr12, xr7, xr7, 2);      // (xr12, xr7): (2*p1 + p0 + q1 + 2) >> 2, new p0
    S32SFL (xr0, xr12, xr7, xr12, ptn1);   // xr12: new p0
    Q8MADL_AA (xr0, xr3, xr12, xr12);      // xr12: new p0 mask
    Q16SAR (xr2,  xr2, xr6, xr6, 2);       // (xr2, xr6): (2*q1 + q0 + p1 + 2) >> 2, new  q0
    S32SFL (xr0, xr2, xr6, xr2, ptn1);   // xr12: new q0
    Q8MADL_AA (xr0, xr3, xr2, xr2);      // xr2: new q0 mask
//
    Q8ADD_SS  (xr4, xr8, xr3);             // xr4: ~(xr3)
    Q8MADL_AA (xr0, xr4, xr10, xr10);      // xr10: old p0 mask
    Q8MADL_AA (xr0, xr4, xr9,  xr9);       //  xr9:  old q0 mask
// store
    tpix = pix - xstride;
    Q8ADD_AA  (xr10, xr10, xr12);         // xr10: final p0'
    S32STD    (xr10, tpix, 0);            // pix[-xstride] = ...p0'
    Q8ADD_AA  (xr9,  xr9, xr2);           // xr9:  final q0'
    S32STDV   (xr9,  tpix, xstride, 0);   // pix[0] = ...q0'
    pix += 4;
  }
}

static void h264_h_loop_filter_chroma_intra_mxu(uint8_t *pix, int ystride, int alpha, int beta)
{
    int d;
    uint8_t *tpix;
    unsigned int p0, q0;

    S32I2M (xr14, beta);
    S32SFL (xr0,  xr14, xr14, xr14, ptn0);
    S32SFL (xr0,  xr14, xr14, xr14, ptn3);   // xr14: beta
    S32I2M (xr13, alpha);
    S32SFL (xr0,  xr13, xr13, xr13, ptn0);
    S32SFL (xr0,  xr13, xr13, xr13, ptn3);   // xr13: alpha
    S32I2M (xr8,  0x01010101);
    
    for( d = 0; d < 2; d++ ) {
      tpix = pix;
      S32LDD  (xr2, tpix,  0);          // xr2: q3,q2,q1,q0
      S32LDD  (xr1, tpix, -4);          // xr1: p0,p1,p2,p3
      S32LDIV (xr4, tpix, ystride, 0);  // xr4: q3',q2',q1',q0'
      S32LDD  (xr3, tpix, -4);          // xr3: p0',p1',p2',p3'
//
      S32SFL  (xr6, xr4, xr2, xr12, ptn0); // xr6: q3'q3q2'q2, xr12:q1'q1q0'q0
      S32SFL  (xr5, xr3, xr1, xr7, ptn0); // xr5: p0'p0p1'p1, xr7:p2'p2p3'p3
//
      S32LDIV (xr2, tpix, ystride, 0);    // xr2: q3",q2",q1",q0"
      S32LDD  (xr1, tpix, -4);          // xr1: p0",p1",p2",p3"
      S32LDIV (xr4, tpix, ystride, 0);    // xr4: q3^,q2^,q1^,q0^
      S32LDD  (xr3, tpix, -4);          // xr3: p0^,p1^,p2^,p3^
//
      S32SFL  (xr4, xr4, xr2, xr2, ptn0); // xr4: q3^q3"q2^q2", xr2:q1^q1"q0^q0"
      S32SFL  (xr3, xr3, xr1, xr1, ptn0); // xr3: p0^p0"p1^p1", xr1:p2^p2"p3^p3"
//
      S32SFL  (xr10, xr3, xr5, xr11, ptn3); // xr10: p0^p0"p0'p0, xr11:p1^p1"p1'p1
      S32SFL  (xr1,  xr2, xr12, xr9,  ptn3); // xr1: q1^q1"q1'q1, xr9:q0^q0"q0'q0
//
      Q8ABD(xr3, xr10, xr9 );   // FFABS (p0 - q0)
      Q8ABD(xr4, xr11, xr10);   // FFABS (p1 - p0)
      Q8ABD(xr5, xr1,  xr9 );   // FFABS (q1 - q0)
// FFABS(p0 - q0) - alpha, FFABS(p1 - p0) - beta
      Q8ADDE_SS (xr6, xr3, xr13,  xr7);  // FFABS(p0 - q0) - alpha
      Q8ADDE_SS (xr3, xr4, xr14,  xr4);  // FFABS(p1 - p0) - beta
      Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( p0 - q0 ) < alpha)
      Q16SLR (xr3, xr3, xr4, xr4, 15);   // 1: < 0 (FFABS( p1 - p0 ) < beta)
      Q16SAT (xr4, xr3, xr4);   // xr4: 1: < 0 (FFABS( p1 - p0 ) < beta)
      Q16SAT (xr3, xr6, xr7);   // xr3: 1: < 0 (FFABS( p0 - q0 ) < alpha)
// FFABS(q1 - q0) - beta
      Q8ADDE_SS (xr6, xr5, xr14,  xr7);  // FFABS(q1 - q0) - beta
      Q16SLR (xr6, xr6, xr7, xr7, 15);   // 1: < 0 (FFABS( q1 - q0 ) < beta)
      Q16SAT (xr5, xr6, xr7);   // xr5: 1: < 0 (FFABS( q1 - q0 ) < beta)

// if( FFABS( p0 - q0 ) < alpha && ..)
      Q8MADL_AA (xr0, xr3, xr4, xr3);        // if( FFABS( p0 - q0 ) < alpha && ..)
//
      Q8ADDE_AA (xr12, xr11, xr8, xr7);      // (xr12, xr7): p1 + 1
      Q16SLL (xr12, xr12, xr7, xr7, 1);      // (xr12, xr7): 2*p1 + 2
      Q8ACCE_AA (xr12, xr10, xr1, xr7);      // (xr12, xr7): 2*p1 + p0 + q1 + 2
//
      Q8MADL_AA (xr0, xr3, xr5, xr3);        // if( FFABS( p0 - q0 ) < alpha && ... && ...)//
      Q8ADDE_AA (xr2, xr1, xr8, xr6);        // (xr2,  xr6): q1 + 1
      Q16SLL (xr2, xr2, xr6, xr6, 1);        // (xr2,  xr6): 2*q1 + 2
      Q8ACCE_AA (xr2, xr9, xr11, xr6);       // (xr2,  xr6): 2*q1 + q0 + p1 + 2
//
      Q16SAR (xr12, xr12, xr7, xr7, 2);      // (xr12, xr7): (2*p1 + p0 + q1 + 2) >> 2, new p0
      S32SFL (xr0, xr12, xr7, xr12, ptn1);   // xr12: new p0
      Q8MADL_AA (xr0, xr3, xr12, xr12);      // xr12: new p0 mask
      Q16SAR (xr2,  xr2, xr6, xr6, 2);       // (xr2, xr6): (2*q1 + q0 + p1 + 2) >> 2, new  q0
      S32SFL (xr0, xr2, xr6, xr2, ptn1);   // xr12: new q0
      Q8MADL_AA (xr0, xr3, xr2, xr2);      // xr2: new q0 mask
//
      Q8ADD_SS  (xr4, xr8, xr3);             // xr4: ~(xr3)
      Q8MADL_AA (xr0, xr4, xr10, xr10);      // xr10: old p0 mask
      Q8MADL_AA (xr0, xr4, xr9,  xr9);       //  xr9:  old q0 mask
//
      Q8ADD_AA  (xr10, xr10, xr12);         // xr10: final p0'
      Q8ADD_AA  (xr9,  xr9, xr2);           // xr9:  final q0'
//
      p0 = S32M2I (xr10);
      q0 = S32M2I (xr9);
//
      pix[-1] = p0;
      pix[0]  = q0;
      pix += ystride;
      pix[-1] = (p0 >> 8);
      pix[0]  = (q0 >> 8);
      pix += ystride;
      pix[-1] = (p0 >> 16);
      pix[0]  = (q0 >> 16);
      pix += ystride;
      pix[-1] = (p0 >> 24);
      pix[0]  = (q0 >> 24);
      pix += ystride;
    }
}

#else

static inline void h264_loop_filter_chroma_intra_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta)
{
    int d;
    for( d = 0; d < 8; d++ ) {
        const int p0 = pix[-1*xstride];
        const int p1 = pix[-2*xstride];
        const int q0 = pix[0];
        const int q1 = pix[1*xstride];

        if( FFABS( p0 - q0 ) < alpha &&
            FFABS( p1 - p0 ) < beta &&
            FFABS( q1 - q0 ) < beta ) {

            pix[-xstride] = ( 2*p1 + p0 + q1 + 2 ) >> 2;   /* p0' */
            pix[0]        = ( 2*q1 + q0 + p1 + 2 ) >> 2;   /* q0' */
        }
        pix += ystride;
    }
}
static void h264_v_loop_filter_chroma_intra_c(uint8_t *pix, int stride, int alpha, int beta)
{
    h264_loop_filter_chroma_intra_c(pix, stride, 1, alpha, beta);
}
static void h264_h_loop_filter_chroma_intra_c(uint8_t *pix, int stride, int alpha, int beta)
{
    h264_loop_filter_chroma_intra_c(pix, 1, stride, alpha, beta);
}
#endif

#if 1
#define H264_LOWPASS(OPNAME, OP, OP2) \
static av_unused void OPNAME ## h264_qpel2_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int h=2;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3]));\
        OP(dst[1], (src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4]));\
        dst+=dstStride;\
        src+=srcStride;\
    }\
}\
\
static av_unused void OPNAME ## h264_qpel2_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int w=2;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<w; i++)\
    {\
        const int srcB= src[-2*srcStride];\
        const int srcA= src[-1*srcStride];\
        const int src0= src[0 *srcStride];\
        const int src1= src[1 *srcStride];\
        const int src2= src[2 *srcStride];\
        const int src3= src[3 *srcStride];\
        const int src4= src[4 *srcStride];\
        OP(dst[0*dstStride], (src0+src1)*20 - (srcA+src2)*5 + (srcB+src3));\
        OP(dst[1*dstStride], (src1+src2)*20 - (src0+src3)*5 + (srcA+src4));\
        dst++;\
        src++;\
    }\
}\
\
static av_unused void OPNAME ## h264_qpel2_hv_lowpass(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride){\
    const int h=2;\
    const int w=2;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    src -= 2*srcStride;\
    for(i=0; i<h+5; i++)\
    {\
        tmp[0]= (src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3]);\
        tmp[1]= (src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4]);\
        tmp+=tmpStride;\
        src+=srcStride;\
    }\
    tmp -= tmpStride*(h+5-2);\
    for(i=0; i<w; i++)\
    {\
        const int tmpB= tmp[-2*tmpStride];\
        const int tmpA= tmp[-1*tmpStride];\
        const int tmp0= tmp[0 *tmpStride];\
        const int tmp1= tmp[1 *tmpStride];\
        const int tmp2= tmp[2 *tmpStride];\
        const int tmp3= tmp[3 *tmpStride];\
        const int tmp4= tmp[4 *tmpStride];\
        OP2(dst[0*dstStride], (tmp0+tmp1)*20 - (tmpA+tmp2)*5 + (tmpB+tmp3));\
        OP2(dst[1*dstStride], (tmp1+tmp2)*20 - (tmp0+tmp3)*5 + (tmpA+tmp4));\
        dst++;\
        tmp++;\
    }\
}\
static void OPNAME ## h264_qpel4_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int h=4;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3]));\
        OP(dst[1], (src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4]));\
        OP(dst[2], (src[2]+src[3])*20 - (src[1 ]+src[4])*5 + (src[0 ]+src[5]));\
        OP(dst[3], (src[3]+src[4])*20 - (src[2 ]+src[5])*5 + (src[1 ]+src[6]));\
        dst+=dstStride;\
        src+=srcStride;\
    }\
}\
\
static void OPNAME ## h264_qpel4_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int w=4;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<w; i++)\
    {\
        const int srcB= src[-2*srcStride];\
        const int srcA= src[-1*srcStride];\
        const int src0= src[0 *srcStride];\
        const int src1= src[1 *srcStride];\
        const int src2= src[2 *srcStride];\
        const int src3= src[3 *srcStride];\
        const int src4= src[4 *srcStride];\
        const int src5= src[5 *srcStride];\
        const int src6= src[6 *srcStride];\
        OP(dst[0*dstStride], (src0+src1)*20 - (srcA+src2)*5 + (srcB+src3));\
        OP(dst[1*dstStride], (src1+src2)*20 - (src0+src3)*5 + (srcA+src4));\
        OP(dst[2*dstStride], (src2+src3)*20 - (src1+src4)*5 + (src0+src5));\
        OP(dst[3*dstStride], (src3+src4)*20 - (src2+src5)*5 + (src1+src6));\
        dst++;\
        src++;\
    }\
}\
\
static void OPNAME ## h264_qpel4_hv_lowpass(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride){\
    const int h=4;\
    const int w=4;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    src -= 2*srcStride;\
    for(i=0; i<h+5; i++)\
    {\
        tmp[0]= (src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3]);\
        tmp[1]= (src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4]);\
        tmp[2]= (src[2]+src[3])*20 - (src[1 ]+src[4])*5 + (src[0 ]+src[5]);\
        tmp[3]= (src[3]+src[4])*20 - (src[2 ]+src[5])*5 + (src[1 ]+src[6]);\
        tmp+=tmpStride;\
        src+=srcStride;\
    }\
    tmp -= tmpStride*(h+5-2);\
    for(i=0; i<w; i++)\
    {\
        const int tmpB= tmp[-2*tmpStride];\
        const int tmpA= tmp[-1*tmpStride];\
        const int tmp0= tmp[0 *tmpStride];\
        const int tmp1= tmp[1 *tmpStride];\
        const int tmp2= tmp[2 *tmpStride];\
        const int tmp3= tmp[3 *tmpStride];\
        const int tmp4= tmp[4 *tmpStride];\
        const int tmp5= tmp[5 *tmpStride];\
        const int tmp6= tmp[6 *tmpStride];\
        OP2(dst[0*dstStride], (tmp0+tmp1)*20 - (tmpA+tmp2)*5 + (tmpB+tmp3));\
        OP2(dst[1*dstStride], (tmp1+tmp2)*20 - (tmp0+tmp3)*5 + (tmpA+tmp4));\
        OP2(dst[2*dstStride], (tmp2+tmp3)*20 - (tmp1+tmp4)*5 + (tmp0+tmp5));\
        OP2(dst[3*dstStride], (tmp3+tmp4)*20 - (tmp2+tmp5)*5 + (tmp1+tmp6));\
        dst++;\
        tmp++;\
    }\
}\
\
static void OPNAME ## h264_qpel8_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int h=8;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3 ]));\
        OP(dst[1], (src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4 ]));\
        OP(dst[2], (src[2]+src[3])*20 - (src[1 ]+src[4])*5 + (src[0 ]+src[5 ]));\
        OP(dst[3], (src[3]+src[4])*20 - (src[2 ]+src[5])*5 + (src[1 ]+src[6 ]));\
        OP(dst[4], (src[4]+src[5])*20 - (src[3 ]+src[6])*5 + (src[2 ]+src[7 ]));\
        OP(dst[5], (src[5]+src[6])*20 - (src[4 ]+src[7])*5 + (src[3 ]+src[8 ]));\
        OP(dst[6], (src[6]+src[7])*20 - (src[5 ]+src[8])*5 + (src[4 ]+src[9 ]));\
        OP(dst[7], (src[7]+src[8])*20 - (src[6 ]+src[9])*5 + (src[5 ]+src[10]));\
        dst+=dstStride;\
        src+=srcStride;\
    }\
}\
\
static void OPNAME ## h264_qpel8_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    const int w=8;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    for(i=0; i<w; i++)\
    {\
        const int srcB= src[-2*srcStride];\
        const int srcA= src[-1*srcStride];\
        const int src0= src[0 *srcStride];\
        const int src1= src[1 *srcStride];\
        const int src2= src[2 *srcStride];\
        const int src3= src[3 *srcStride];\
        const int src4= src[4 *srcStride];\
        const int src5= src[5 *srcStride];\
        const int src6= src[6 *srcStride];\
        const int src7= src[7 *srcStride];\
        const int src8= src[8 *srcStride];\
        const int src9= src[9 *srcStride];\
        const int src10=src[10*srcStride];\
        OP(dst[0*dstStride], (src0+src1)*20 - (srcA+src2)*5 + (srcB+src3));\
        OP(dst[1*dstStride], (src1+src2)*20 - (src0+src3)*5 + (srcA+src4));\
        OP(dst[2*dstStride], (src2+src3)*20 - (src1+src4)*5 + (src0+src5));\
        OP(dst[3*dstStride], (src3+src4)*20 - (src2+src5)*5 + (src1+src6));\
        OP(dst[4*dstStride], (src4+src5)*20 - (src3+src6)*5 + (src2+src7));\
        OP(dst[5*dstStride], (src5+src6)*20 - (src4+src7)*5 + (src3+src8));\
        OP(dst[6*dstStride], (src6+src7)*20 - (src5+src8)*5 + (src4+src9));\
        OP(dst[7*dstStride], (src7+src8)*20 - (src6+src9)*5 + (src5+src10));\
        dst++;\
        src++;\
    }\
}\
\
static void OPNAME ## h264_qpel8_hv_lowpass(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride){\
    const int h=8;\
    const int w=8;\
    uint8_t *cm = ff_cropTbl + MAX_NEG_CROP;\
    int i;\
    src -= 2*srcStride;\
    for(i=0; i<h+5; i++)\
    {\
        tmp[0]= (src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3 ]);\
        tmp[1]= (src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4 ]);\
        tmp[2]= (src[2]+src[3])*20 - (src[1 ]+src[4])*5 + (src[0 ]+src[5 ]);\
        tmp[3]= (src[3]+src[4])*20 - (src[2 ]+src[5])*5 + (src[1 ]+src[6 ]);\
        tmp[4]= (src[4]+src[5])*20 - (src[3 ]+src[6])*5 + (src[2 ]+src[7 ]);\
        tmp[5]= (src[5]+src[6])*20 - (src[4 ]+src[7])*5 + (src[3 ]+src[8 ]);\
        tmp[6]= (src[6]+src[7])*20 - (src[5 ]+src[8])*5 + (src[4 ]+src[9 ]);\
        tmp[7]= (src[7]+src[8])*20 - (src[6 ]+src[9])*5 + (src[5 ]+src[10]);\
        tmp+=tmpStride;\
        src+=srcStride;\
    }\
    tmp -= tmpStride*(h+5-2);\
    for(i=0; i<w; i++)\
    {\
        const int tmpB= tmp[-2*tmpStride];\
        const int tmpA= tmp[-1*tmpStride];\
        const int tmp0= tmp[0 *tmpStride];\
        const int tmp1= tmp[1 *tmpStride];\
        const int tmp2= tmp[2 *tmpStride];\
        const int tmp3= tmp[3 *tmpStride];\
        const int tmp4= tmp[4 *tmpStride];\
        const int tmp5= tmp[5 *tmpStride];\
        const int tmp6= tmp[6 *tmpStride];\
        const int tmp7= tmp[7 *tmpStride];\
        const int tmp8= tmp[8 *tmpStride];\
        const int tmp9= tmp[9 *tmpStride];\
        const int tmp10=tmp[10*tmpStride];\
        OP2(dst[0*dstStride], (tmp0+tmp1)*20 - (tmpA+tmp2)*5 + (tmpB+tmp3));\
        OP2(dst[1*dstStride], (tmp1+tmp2)*20 - (tmp0+tmp3)*5 + (tmpA+tmp4));\
        OP2(dst[2*dstStride], (tmp2+tmp3)*20 - (tmp1+tmp4)*5 + (tmp0+tmp5));\
        OP2(dst[3*dstStride], (tmp3+tmp4)*20 - (tmp2+tmp5)*5 + (tmp1+tmp6));\
        OP2(dst[4*dstStride], (tmp4+tmp5)*20 - (tmp3+tmp6)*5 + (tmp2+tmp7));\
        OP2(dst[5*dstStride], (tmp5+tmp6)*20 - (tmp4+tmp7)*5 + (tmp3+tmp8));\
        OP2(dst[6*dstStride], (tmp6+tmp7)*20 - (tmp5+tmp8)*5 + (tmp4+tmp9));\
        OP2(dst[7*dstStride], (tmp7+tmp8)*20 - (tmp6+tmp9)*5 + (tmp5+tmp10));\
        dst++;\
        tmp++;\
    }\
}\
\
static void OPNAME ## h264_qpel16_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    OPNAME ## h264_qpel8_v_lowpass(dst  , src  , dstStride, srcStride);\
    OPNAME ## h264_qpel8_v_lowpass(dst+8, src+8, dstStride, srcStride);\
    src += 8*srcStride;\
    dst += 8*dstStride;\
    OPNAME ## h264_qpel8_v_lowpass(dst  , src  , dstStride, srcStride);\
    OPNAME ## h264_qpel8_v_lowpass(dst+8, src+8, dstStride, srcStride);\
}\
\
static void OPNAME ## h264_qpel16_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    OPNAME ## h264_qpel8_h_lowpass(dst  , src  , dstStride, srcStride);\
    OPNAME ## h264_qpel8_h_lowpass(dst+8, src+8, dstStride, srcStride);\
    src += 8*srcStride;\
    dst += 8*dstStride;\
    OPNAME ## h264_qpel8_h_lowpass(dst  , src  , dstStride, srcStride);\
    OPNAME ## h264_qpel8_h_lowpass(dst+8, src+8, dstStride, srcStride);\
}\
\
static void OPNAME ## h264_qpel16_hv_lowpass(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride){\
    OPNAME ## h264_qpel8_hv_lowpass(dst  , tmp  , src  , dstStride, tmpStride, srcStride);\
    OPNAME ## h264_qpel8_hv_lowpass(dst+8, tmp+8, src+8, dstStride, tmpStride, srcStride);\
    src += 8*srcStride;\
    dst += 8*dstStride;\
    OPNAME ## h264_qpel8_hv_lowpass(dst  , tmp  , src  , dstStride, tmpStride, srcStride);\
    OPNAME ## h264_qpel8_hv_lowpass(dst+8, tmp+8, src+8, dstStride, tmpStride, srcStride);\
}\

#define H264_MC(OPNAME, SIZE) \
static void OPNAME ## h264_qpel ## SIZE ## _mc00_c (uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## pixels ## SIZE ## _c(dst, src, stride, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc10_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(half, src, SIZE, stride);\
    OPNAME ## pixels ## SIZE ## _l2(dst, src, half, stride, stride, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc20_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ## _h_lowpass(dst, src, stride, stride);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc30_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(half, src, SIZE, stride);\
    OPNAME ## pixels ## SIZE ## _l2(dst, src+1, half, stride, stride, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc01_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    uint8_t half[SIZE*SIZE];\
    copy_block ## SIZE (full, src - stride*2, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(half, full_mid, SIZE, SIZE);\
    OPNAME ## pixels ## SIZE ## _l2(dst, full_mid, half, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc02_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    copy_block ## SIZE (full, src - stride*2, SIZE,  stride, SIZE + 5);\
    OPNAME ## h264_qpel ## SIZE ## _v_lowpass(dst, full_mid, stride, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc03_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    uint8_t half[SIZE*SIZE];\
    copy_block ## SIZE (full, src - stride*2, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(half, full_mid, SIZE, SIZE);\
    OPNAME ## pixels ## SIZE ## _l2(dst, full_mid+SIZE, half, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc11_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    uint8_t halfH[SIZE*SIZE];\
    uint8_t halfV[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(halfH, src, SIZE, stride);\
    copy_block ## SIZE (full, src - stride*2, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(halfV, full_mid, SIZE, SIZE);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfH, halfV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc31_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    uint8_t halfH[SIZE*SIZE];\
    uint8_t halfV[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(halfH, src, SIZE, stride);\
    copy_block ## SIZE (full, src - stride*2 + 1, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(halfV, full_mid, SIZE, SIZE);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfH, halfV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc13_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    uint8_t halfH[SIZE*SIZE];\
    uint8_t halfV[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(halfH, src + stride, SIZE, stride);\
    copy_block ## SIZE (full, src - stride*2, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(halfV, full_mid, SIZE, SIZE);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfH, halfV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc33_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    uint8_t halfH[SIZE*SIZE];\
    uint8_t halfV[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(halfH, src + stride, SIZE, stride);\
    copy_block ## SIZE (full, src - stride*2 + 1, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(halfV, full_mid, SIZE, SIZE);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfH, halfV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc22_c(uint8_t *dst, uint8_t *src, int stride){\
    int32_t tmp[SIZE*(SIZE+5)];\
    OPNAME ## h264_qpel ## SIZE ## _hv_lowpass(dst, tmp, src, stride, SIZE, stride);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc21_c(uint8_t *dst, uint8_t *src, int stride){\
    int32_t tmp[SIZE*(SIZE+5)];\
    uint8_t halfH[SIZE*SIZE];\
    uint8_t halfHV[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(halfH, src, SIZE, stride);\
    put_h264_qpel ## SIZE ## _hv_lowpass(halfHV, tmp, src, SIZE, SIZE, stride);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfH, halfHV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc23_c(uint8_t *dst, uint8_t *src, int stride){\
    int32_t tmp[SIZE*(SIZE+5)];\
    uint8_t halfH[SIZE*SIZE];\
    uint8_t halfHV[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass(halfH, src + stride, SIZE, stride);\
    put_h264_qpel ## SIZE ## _hv_lowpass(halfHV, tmp, src, SIZE, SIZE, stride);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfH, halfHV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc12_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    int32_t tmp[SIZE*(SIZE+5)];\
    uint8_t halfV[SIZE*SIZE];\
    uint8_t halfHV[SIZE*SIZE];\
    copy_block ## SIZE (full, src - stride*2, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(halfV, full_mid, SIZE, SIZE);\
    put_h264_qpel ## SIZE ## _hv_lowpass(halfHV, tmp, src, SIZE, SIZE, stride);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfV, halfHV, stride, SIZE, SIZE, SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc32_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t full[SIZE*(SIZE+5)];\
    uint8_t * const full_mid= full + SIZE*2;\
    int32_t tmp[SIZE*(SIZE+5)];\
    uint8_t halfV[SIZE*SIZE];\
    uint8_t halfHV[SIZE*SIZE];\
    copy_block ## SIZE (full, src - stride*2 + 1, SIZE,  stride, SIZE + 5);\
    put_h264_qpel ## SIZE ## _v_lowpass(halfV, full_mid, SIZE, SIZE);\
    put_h264_qpel ## SIZE ## _hv_lowpass(halfHV, tmp, src, SIZE, SIZE, stride);\
    OPNAME ## pixels ## SIZE ## _l2(dst, halfV, halfHV, stride, SIZE, SIZE, SIZE);\
}\

#define op_avg(a, b)  a = (((a)+cm[((b) + 16)>>5]+1)>>1)
//#define op_avg2(a, b) a = (((a)*w1+cm[((b) + 16)>>5]*w2 + o + 64)>>7)
#define op_put(a, b)  a = cm[((b) + 16)>>5]
#define op2_avg(a, b)  a = (((a)+cm[((b) + 512)>>10]+1)>>1)
#define op2_put(a, b)  a = cm[((b) + 512)>>10]

#ifdef JZ4740_MXU_OPT

static void put_h264_qpel4_h_lowpass_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride)
{
  uint32_t src_aln1,src_aln2;
  uint32_t src_rs1,src_rs2;
  int i;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t round = 0x100010; //(16,16)

  src_aln1 = ((uint32_t)src - 2) & 0xfffffffc;
  src_aln2 = (uint32_t)src & 0xfffffffc;
  src_rs1 = 4 - (((uint32_t)src - 2) & 3);
  src_rs2 = 4 - ((uint32_t)src & 3);

  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  S32I2M(xr13,round);

  dst -= dstStride;
  for(i=0; i<4; i++) {
    //2 pixel
    S32LDD(xr1,src_aln1,0);
    S32LDD(xr2,src_aln1,4);
    S32LDD(xr3,src_aln1,8);
    src_aln1 += srcStride;
    i_pref(0, src_aln1, 0);
    //for pixel[0]
    S32ALN(xr4,xr2,xr1,src_rs1); //xr4 <- src[-2] ~ src[1]
    S32ALN(xr5,xr3,xr2,src_rs1); //xr5 <- src[2] ~ src[5]
    //for pixel[1]
    S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
    D32SLR(xr1,xr5,xr0,xr0,8);
    S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
    Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
    Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
    Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
    Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
    Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
    Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
    S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
    Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
    Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
    Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
    Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
    S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
    Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

    S32SFL(xr0,xr3,xr1,xr1,ptn3);
    Q16ADD_AA_WW(xr0,xr1,xr13,xr11); //xr11 <- tow new pixel


    //2 pixel
    S32LDD(xr1,src_aln2,0);
    S32LDD(xr2,src_aln2,4);
    S32LDD(xr3,src_aln2,8);
    src_aln2 += srcStride;
    i_pref(0, src_aln2, 0);
    //for pixel[0]
    S32ALN(xr4,xr2,xr1,src_rs2); //xr4 <- src[-2] ~ src[1]
    S32ALN(xr5,xr3,xr2,src_rs2); //xr5 <- src[2] ~ src[5]
    //for pixel[1]
    S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
    D32SLR(xr1,xr5,xr0,xr0,8);
    S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
    Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
    Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
    Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
    Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
    Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
    Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
    S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
    Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
    Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
    Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
    Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
    S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
    Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

    S32SFL(xr0,xr3,xr1,xr1,ptn3);
    Q16ADD_AA_WW(xr0,xr1,xr13,xr12); //xr12 <- tow new pixel
  
    // >>5 and clip to (0~255)
    Q16SAR(xr12,xr12,xr11,xr11,5);
    Q16SAT(xr1,xr12,xr11); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
    S32SDIV(xr1,dst,dstStride,0);
  }
}

static void put_h264_qpel4_v_lowpass_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride)
{
  uint32_t src_aln,src_aln0, src_al4;
  uint32_t src_rs;
  int i;
  uint32_t mul_20 = 0x14141414; //(20,20,20,20)
  uint32_t mul_5 = 0x05050505; //(5,5,5,5)
  uint32_t round = 0x00100010; //(0,16,0,16)

  src_aln = ((uint32_t)src - (srcStride<<1)) & 0xfffffffc;
  src_aln0 = src_aln - (srcStride<<1);
  src_rs = 4 - (((uint32_t)src - (srcStride<<1)) & 3);

  S32I2M(xr15,mul_20);
  S32I2M(xr14,mul_5);
  S32I2M(xr13,round);
  dst -= dstStride;
  for(i=0; i<4; i++) {
    src_aln0 += srcStride;
    src_aln = src_aln0;
    src_al4 = src_aln0 + 4;
    //first line
    S32LDIV(xr1, src_aln, srcStride, 0);
    S32LDIV(xr2, src_al4, srcStride, 0);
    //second line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    //ALN 1st and 2nd line 4 pixel
    S32ALN(xr1,xr2,xr1,src_rs); 
    S32ALN(xr2,xr4,xr3,src_rs);
    //third line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    Q8MUL(xr10,xr2,xr14,xr9); //2nd 4 pixel mul 5 <--here
    //ALN 3rd line 4 pixel
    S32ALN(xr3,xr4,xr3,src_rs);

    Q8MUL(xr8,xr3,xr15,xr7); //3rd 4 pixel mul 20
    //fourth line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    S32LDIV(xr2, src_aln, srcStride, 0); // <--here
    S32LDIV(xr5, src_al4, srcStride, 0);
    //ALN 4th line 4 pixel
    S32ALN(xr4,xr4,xr3,src_rs);

    //fiveth line
    //ALN 5th line 4 pixel
    S32ALN(xr5,xr5,xr2,src_rs);

    S32LDIV(xr11, src_aln, srcStride, 0); // <--here
    S32LDIV(xr12, src_al4, srcStride, 0);
    Q8MUL(xr6,xr5,xr14,xr5); //5th 4 pixel mul 5
    Q8MUL(xr4,xr4,xr15,xr3); //4th 4 pixel mul 20
    
    //sixth line
    //ALN 6th line 4 pixel
    S32ALN(xr2,xr12,xr11,src_rs);
    
    //1st line + 6th line
    Q8ADDE_AA(xr2,xr1,xr2,xr1);
    
    //1st line + 6th line + 3rd line - 2nd line
    Q16ACC_SS(xr2,xr8,xr10,xr0);
    Q16ACC_SS(xr1,xr7,xr9,xr0);
    
    //1st line + 6th line + 3rd line - 2nd line + 4th line - 5th line
    Q16ACC_SS(xr2,xr4,xr6,xr0);
    Q16ACC_SS(xr1,xr3,xr5,xr0);

    //+ round
    Q16ADD_AA_WW(xr2,xr2,xr13,xr0);
    Q16ADD_AA_WW(xr1,xr1,xr13,xr0);

    // >>5 and clip to (0~255)
    Q16SAR(xr2,xr2,xr1,xr1,5);
    Q16SAT(xr1,xr2,xr1); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
    S32SDIV(xr1, dst, dstStride, 0);
  }
}

static void put_h264_qpel4_hv_lowpass_mxu(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride)
{
  uint32_t src_aln[2],tmp_h,tmp_v;
  uint32_t src_rs[2];
  int i,j;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t mul_20 = 0x00140014; //(0,20,0,20)
  uint32_t mul_5 = 0x00050005; //(0,5,0,5)
  uint32_t round = 0x200; //512

  tmp_h = (uint32_t)tmp;
  src_aln[0] = ((uint32_t)src - 2*srcStride - 2) & 0xfffffffc;
  src_aln[1] = ((uint32_t)src - 2*srcStride) & 0xfffffffc;
  src_rs[0] = 4 - (((uint32_t)src - 2) & 3);
  src_rs[1] = 4 - ((uint32_t)src & 3);

  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  tmp_h -= 4;
  for(i=0; i<9; i++) {
    for(j=0; j<2; j++) {
      //2 pixel
      S32LDD(xr1,src_aln[j],0);
      S32LDD(xr2,src_aln[j],4);
      S32LDD(xr3,src_aln[j],8);
      src_aln[j] += srcStride;
      i_pref(0, src_aln[j], 0);
      //for pixel[0]
      S32ALN(xr4,xr2,xr1,src_rs[j]); //xr4 <- src[-2] ~ src[1]
      S32ALN(xr5,xr3,xr2,src_rs[j]); //xr5 <- src[2] ~ src[5]
      //for pixel[1]
      S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
      D32SLR(xr1,xr5,xr0,xr0,8);
      S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
      Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
      Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
      Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
      
      Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
      Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
      Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
      S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
      Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
      Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
      Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
      Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
      S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
      Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

      S32SFL(xr0,xr3,xr1,xr1,ptn3);
      S32SDI(xr1, tmp_h, 4);
    }
    tmp_h += tmpStride - 8;
  }


  S32I2M(xr15,mul_20);
  S32I2M(xr14,mul_5);
  S32I2M(xr13,round);
  S32I2M(xr11,0x10001);
  tmp_v = (uint32_t)tmp - (tmpStride << 1);
  for(i=0; i<4; i++) {
    tmp_v += tmpStride;
    for(j=0; j<2; j++) {
      tmp_h = tmp_v + (j<<2);
      //1st line 2 pixel
      S32LDIV(xr1, tmp_h, tmpStride, 0);
      //2nd line 2 pixel
      S32LDIV(xr2, tmp_h, tmpStride, 0);
      //3rd line 2 pixel
      S32LDIV(xr3, tmp_h, tmpStride, 0);
      //2line*5; 3line*20
      D16MUL_WW(xr10,xr2,xr14,xr9); //2nd 2 pixel mul 5
      D16MUL_WW(xr8,xr3,xr15,xr7); //3rd 2 pixel mul 20

      //4th line 2 pixel
      S32LDIV(xr4, tmp_h, tmpStride, 0);
      //5th line 2 pixel
      S32LDIV(xr5, tmp_h, tmpStride, 0);
      S32LDDV(xr12, tmp_h, tmpStride, 0);
      //5line*5; 4line*20
      D16MUL_WW(xr3,xr5,xr14,xr2); //5th 2 pixel mul 5
      D16MUL_WW(xr5,xr4,xr15,xr4); //4th 2 pixel mul 20
      //6st line 2 pixel

      //1st line + 6th line
      Q16ADD_AA_WW(xr12,xr1,xr12,xr0);
      D16MUL_WW(xr12,xr12,xr11,xr1);

      //1st line + 6th line + 3rd line - 2nd line
      D32ACC_SS(xr12,xr8,xr10,xr0);
      D32ACC_SS(xr1,xr7,xr9,xr0);
    
      //1st line + 6th line + 3rd line - 2nd line + 4th line - 5th line
      D32ACC_SS(xr12,xr5,xr3,xr0);
      D32ACC_SS(xr1,xr4,xr2,xr0);

      //+ round
      D32ADD_AA(xr12,xr12,xr13,xr0);
      D32ADD_AA(xr1,xr1,xr13,xr0);
      
      //right shift
      D32SAR(xr2,xr12,xr1,xr1,10);
      S32SFL(xr0, xr2, xr1, xr1, ptn3);
      Q16SAT(xr1, xr0, xr1);
      
      *((uint16_t*)dst) = S32M2I(xr1);
      dst += 2;
    }
    dst += dstStride - 4;
  }
}

static void put_h264_qpel8_h_lowpass_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride)
{
  uint32_t src_aln1,src_aln2;
  uint32_t src_rs1,src_rs2;
  int i,j;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t round = 0x100010; //(16,16)

  src_aln1 = ((uint32_t)src - 2) & 0xfffffffc;
  src_aln2 = (uint32_t)src & 0xfffffffc;
  src_rs1 = 4 - (((uint32_t)src - 2) & 3);
  src_rs2 = 4 - ((uint32_t)src & 3);

  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  S32I2M(xr13,round);

  dst -= 4;
  for(i=0; i<8; i++) {
    for(j=0; j<2; j++) {
      //2 pixel
      S32LDD(xr1,src_aln1,0);
      S32LDD(xr2,src_aln1,4);
      S32LDD(xr3,src_aln1,8);
      i_pref(0, src_aln1 + srcStride, 0);
      src_aln1 += 4;
      //for pixel[0]
      S32ALN(xr4,xr2,xr1,src_rs1); //xr4 <- src[-2] ~ src[1]
      S32ALN(xr5,xr3,xr2,src_rs1); //xr5 <- src[2] ~ src[5]
      //for pixel[1]
      S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
      D32SLR(xr1,xr5,xr0,xr0,8);
      S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
      Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
      Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
      Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
      Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
      Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
      Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
      S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
      Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
      
      Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
      Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
      Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
      S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
      Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

      S32SFL(xr0,xr3,xr1,xr1,ptn3);
      Q16ADD_AA_WW(xr0,xr1,xr13,xr11); //xr11 <- tow new pixel


      //2 pixel
      S32LDD(xr1,src_aln2,0);
      S32LDD(xr2,src_aln2,4);
      S32LDD(xr3,src_aln2,8);
      i_pref(0, src_aln2 + srcStride, 0);
      src_aln2 += 4;
      //for pixel[0]
      S32ALN(xr4,xr2,xr1,src_rs2); //xr4 <- src[-2] ~ src[1]
      S32ALN(xr5,xr3,xr2,src_rs2); //xr5 <- src[2] ~ src[5]
      //for pixel[1]
      S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
      
      D32SLR(xr1,xr5,xr0,xr0,8);
      S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
      Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
      Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
      Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
      Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
      Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
      Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
      S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
      Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
      Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
      Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
      Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
      S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
      Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

      S32SFL(xr0,xr3,xr1,xr1,ptn3);
      Q16ADD_AA_WW(xr0,xr1,xr13,xr12); //xr12 <- tow new pixel
  
      // >>5 and clip to (0~255)
      Q16SAR(xr12,xr12,xr11,xr11,5);
      Q16SAT(xr1,xr12,xr11); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
      S32SDI(xr1, dst, 4);
    }
    src_aln1 += srcStride - 8;
    src_aln2 += srcStride - 8;
    dst += dstStride - 8;
  }
}

static void put_h264_qpel8_v_lowpass_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride)
{
  uint32_t src_aln,src_aln0,src_al4;
  uint32_t src_rs;
  int i,j;
  uint32_t mul_20 = 0x14141414; //(20,20,20,20)
  uint32_t mul_5 = 0x05050505; //(5,5,5,5)
  uint32_t round = 0x00100010; //(0,16,0,16)
  src_aln = ((uint32_t)src - (srcStride<<1)) & 0xfffffffc;
  src_aln0 = src_aln;
  src_rs = 4 - (((uint32_t)src - (srcStride<<1)) & 3);

  S32I2M(xr15,mul_20);
  S32I2M(xr14,mul_5);
  S32I2M(xr13,round);
  dst -= 4;
  for(i=0; i<8; i++) {
    for(j=0; j<2; j++) {
      src_aln = src_aln0 + i*srcStride + (j<<2);
      src_al4 = src_aln + 4;
      //first line
      S32LDD(xr1,src_aln,0);
      S32LDD(xr2,src_al4,0);
      //second line
      S32LDIV(xr3, src_aln, srcStride, 0);
      S32LDIV(xr4, src_al4, srcStride, 0);
      //ALN 1st and 2nd line 4 pixel
      S32ALN(xr1,xr2,xr1,src_rs); 
      S32ALN(xr2,xr4,xr3,src_rs);
      //third line
      S32LDIV(xr3, src_aln, srcStride, 0);
      S32LDIV(xr4, src_al4, srcStride, 0);
      Q8MUL(xr10,xr2,xr14,xr9); //2nd 4 pixel mul 5 <--here
      //ALN 3rd line 4 pixel
      S32ALN(xr3,xr4,xr3,src_rs);

      Q8MUL(xr8,xr3,xr15,xr7); //3rd 4 pixel mul 20
      //fourth line
      S32LDIV(xr3, src_aln, srcStride, 0);
      S32LDIV(xr4, src_al4, srcStride, 0);
      S32LDIV(xr2, src_aln, srcStride, 0); // <--here
      S32LDIV(xr5, src_al4, srcStride, 0);
      //ALN 4th line 4 pixel
      S32ALN(xr4,xr4,xr3,src_rs);

      //fiveth line
      //ALN 5th line 4 pixel
      S32ALN(xr5,xr5,xr2,src_rs);

      S32LDIV(xr11, src_aln, srcStride, 0); // <--here
      S32LDIV(xr12, src_al4, srcStride, 0);
      Q8MUL(xr6,xr5,xr14,xr5); //5th 4 pixel mul 5
      Q8MUL(xr4,xr4,xr15,xr3); //4th 4 pixel mul 20
    
      //sixth line
      //ALN 6th line 4 pixel
      S32ALN(xr2,xr12,xr11,src_rs);
    
      //1st line + 6th line
      Q8ADDE_AA(xr2,xr1,xr2,xr1);
    
      //1st line + 6th line + 3rd line - 2nd line
      Q16ACC_SS(xr2,xr8,xr10,xr0);
      Q16ACC_SS(xr1,xr7,xr9,xr0);
    
      //1st line + 6th line + 3rd line - 2nd line + 4th line - 5th line
      Q16ACC_SS(xr2,xr4,xr6,xr0);
      Q16ACC_SS(xr1,xr3,xr5,xr0);

      //+ round
      Q16ADD_AA_WW(xr2,xr2,xr13,xr0);
      Q16ADD_AA_WW(xr1,xr1,xr13,xr0);

      // >>5 and clip to (0~255)
      Q16SAR(xr2,xr2,xr1,xr1,5);
      Q16SAT(xr1,xr2,xr1); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
      S32SDI(xr1,dst,4);
    }
    dst += dstStride - 8;
  }
}

static void put_h264_qpel8_hv_lowpass_mxu(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride)
{
  uint32_t src_aln[4],tmp_h,tmp_v;
  uint32_t src_rs[4];
  int i,j;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t mul_20 = 0x00140014; //(0,20,0,20)
  uint32_t mul_5 = 0x00050005; //(0,5,0,5)
  uint32_t round = 0x200; //512

  tmp_h = (uint32_t)tmp;
  src_aln[0] = ((uint32_t)src - 2*srcStride - 2) & 0xfffffffc;
  src_aln[1] = ((uint32_t)src - 2*srcStride) & 0xfffffffc;
  src_aln[2] = src_aln[0] + 4;
  src_aln[3] = src_aln[1] + 4;
  src_rs[0] = 4 - (((uint32_t)src - 2) & 3);
  src_rs[1] = 4 - ((uint32_t)src & 3);
  src_rs[2] = src_rs[0];
  src_rs[3] = src_rs[1];

  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  tmp_h -= 4;
  for(i=0; i<13; i++) {
    for(j=0; j<4; j++) {
      //2 pixel
      S32LDD(xr1,src_aln[j],0);
      S32LDD(xr2,src_aln[j],4);
      S32LDD(xr3,src_aln[j],8);
      src_aln[j] += srcStride;
      i_pref(0, src_aln[j], 0);
      //for pixel[0]
      S32ALN(xr4,xr2,xr1,src_rs[j]); //xr4 <- src[-2] ~ src[1]
      S32ALN(xr5,xr3,xr2,src_rs[j]); //xr5 <- src[2] ~ src[5]
      //for pixel[1]
      S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
      D32SLR(xr1,xr5,xr0,xr0,8);
      S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
      Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
      Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
      Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
      
      Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
      Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
      Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
      S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
      Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
      Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
      Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
      Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
      S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
      Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

      S32SFL(xr0,xr3,xr1,xr1,ptn3);
      S32SDI(xr1, tmp_h, 4);
    }
    tmp_h += tmpStride - 16;
  }

  S32I2M(xr15,mul_20);
  S32I2M(xr14,mul_5);
  S32I2M(xr13,round);
  S32I2M(xr11,0x10001);
  tmp_v = (uint32_t)tmp - (tmpStride << 1);
  for(i=0; i<8; i++) {
    tmp_v += tmpStride;
    for(j=0; j<4; j++) {
      tmp_h = tmp_v + (j<<2);
      //1st line 2 pixel
      S32LDIV(xr1, tmp_h, tmpStride, 0);
      //2nd line 2 pixel
      S32LDIV(xr2, tmp_h, tmpStride, 0);
      //3rd line 2 pixel
      S32LDIV(xr3, tmp_h, tmpStride, 0);
      //2line*5; 3line*20
      D16MUL_WW(xr10,xr2,xr14,xr9); //2nd 2 pixel mul 5
      D16MUL_WW(xr8,xr3,xr15,xr7); //3rd 2 pixel mul 20

      //4th line 2 pixel
      S32LDIV(xr4, tmp_h, tmpStride, 0);
      //5th line 2 pixel
      S32LDIV(xr5, tmp_h, tmpStride, 0);
      S32LDDV(xr12, tmp_h, tmpStride, 0);
      //5line*5; 4line*20
      D16MUL_WW(xr3,xr5,xr14,xr2); //5th 2 pixel mul 5
      D16MUL_WW(xr5,xr4,xr15,xr4); //4th 2 pixel mul 20
      //6st line 2 pixel

      //1st line + 6th line
      Q16ADD_AA_WW(xr12,xr1,xr12,xr0);
      D16MUL_WW(xr12,xr12,xr11,xr1);

      //1st line + 6th line + 3rd line - 2nd line
      D32ACC_SS(xr12,xr8,xr10,xr0);
      D32ACC_SS(xr1,xr7,xr9,xr0);
    
      //1st line + 6th line + 3rd line - 2nd line + 4th line - 5th line
      D32ACC_SS(xr12,xr5,xr3,xr0);
      D32ACC_SS(xr1,xr4,xr2,xr0);

      //+ round
      D32ADD_AA(xr12,xr12,xr13,xr0);
      D32ADD_AA(xr1,xr1,xr13,xr0);
      
      //right shift
      D32SAR(xr2,xr12,xr1,xr1,10);
      S32SFL(xr0, xr2, xr1, xr1, ptn3);
      Q16SAT(xr1, xr0, xr1);
      
      *((uint16_t*)dst) = S32M2I(xr1);
      dst += 2;
    }
    dst += dstStride - 8;
  }
}

static void put_h264_qpel16_h_lowpass_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){
    put_h264_qpel8_h_lowpass_mxu(dst  , src  , dstStride, srcStride);
    put_h264_qpel8_h_lowpass_mxu(dst+8, src+8, dstStride, srcStride);
    src += 8*srcStride;
    dst += 8*dstStride;
    put_h264_qpel8_h_lowpass_mxu(dst  , src  , dstStride, srcStride);
    put_h264_qpel8_h_lowpass_mxu(dst+8, src+8, dstStride, srcStride);
}

static void put_h264_qpel16_v_lowpass_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){
    put_h264_qpel8_v_lowpass_mxu(dst  , src  , dstStride, srcStride);
    put_h264_qpel8_v_lowpass_mxu(dst+8, src+8, dstStride, srcStride);
    src += 8*srcStride;
    dst += 8*dstStride;
    put_h264_qpel8_v_lowpass_mxu(dst  , src  , dstStride, srcStride);
    put_h264_qpel8_v_lowpass_mxu(dst+8, src+8, dstStride, srcStride);
}

static void put_h264_qpel16_hv_lowpass_mxu(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride){
    put_h264_qpel8_hv_lowpass_mxu(dst  , tmp  , src  , dstStride, tmpStride, srcStride);
    put_h264_qpel8_hv_lowpass_mxu(dst+8, tmp+8, src+8, dstStride, tmpStride, srcStride);
    src += 8*srcStride;
    dst += 8*dstStride;
    put_h264_qpel8_hv_lowpass_mxu(dst  , tmp  , src  , dstStride, tmpStride, srcStride);
    put_h264_qpel8_hv_lowpass_mxu(dst+8, tmp+8, src+8, dstStride, tmpStride, srcStride);
}

///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

static void put_h264_qpel4_h_lowpass_avg_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int pos){

  uint32_t src_aln1,src_aln2,src_org;
  uint32_t src_rs1,src_rs2,src_rso;
  int i;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t round = 0x100010; //(16,16)

  src_aln1 = ((uint32_t)src - 2) & 0xfffffffc;
  src_aln2 = (uint32_t)src & 0xfffffffc;
  src_org =  ((uint32_t)src + pos) & 0xfffffffc;
  src_rso = 4 - (((uint32_t)src + pos) & 3);
  src_rs1 = 4 - (((uint32_t)src - 2) & 3);
  src_rs2 = 4 - ((uint32_t)src & 3);
  
  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  S32I2M(xr13,round);

  for(i=0; i<4; i++) {
    //2 pixel
    S32LDD(xr1,src_aln1,0);
    S32LDD(xr2,src_aln1,4);
    S32LDD(xr3,src_aln1,8);
    src_aln1 += srcStride;
    i_pref(0, src_aln1, 0);
    //for pixel[0]
    S32ALN(xr4,xr2,xr1,src_rs1); //xr4 <- src[-2] ~ src[1]
    S32ALN(xr5,xr3,xr2,src_rs1); //xr5 <- src[2] ~ src[5]
    //for pixel[1]
    S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
    D32SLR(xr1,xr5,xr0,xr0,8);
    S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
    Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
    Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
    Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
    Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
    Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
    Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
    S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
    Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
    Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
    Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
    Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
    S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
    Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

    S32SFL(xr0,xr3,xr1,xr1,ptn3);
    Q16ADD_AA_WW(xr0,xr1,xr13,xr11); //xr11 <- tow new pixel


    //2 pixel
    S32LDD(xr1,src_aln2,0);
    S32LDD(xr2,src_aln2,4);
    S32LDD(xr3,src_aln2,8);
    src_aln2 += srcStride;
    i_pref(0, src_aln2, 0);
    //for pixel[0]
    S32ALN(xr4,xr2,xr1,src_rs2); //xr4 <- src[-2] ~ src[1]
    S32ALN(xr5,xr3,xr2,src_rs2); //xr5 <- src[2] ~ src[5]

    ///!!!!!! xr9 stores original 4 pixels of the reference block /////////////
    S32LDD(xr1,src_org,0);
    S32LDD(xr2,src_org,4);
    src_org += srcStride;

    //for pixel[1]
    S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]

    ///!!!!!! xr9 stores original 4 pixels of the reference block /////////////
    S32ALN(xr9,xr2,xr1,src_rso); //xr9 <- src[4] ~ src[0]

    D32SLR(xr1,xr5,xr0,xr0,8);
    S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
    Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
    Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
    Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
    Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
    Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
    Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
    S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
    Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
    Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
    Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
    Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
    S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
    Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

    S32SFL(xr0,xr3,xr1,xr1,ptn3);
    Q16ADD_AA_WW(xr0,xr1,xr13,xr12); //xr12 <- tow new pixel
  
    // >>5 and clip to (0~255)
    Q16SAR(xr12,xr12,xr11,xr11,5);
    Q16SAT(xr1,xr12,xr11); //xr1 <- dst[3],dst[2],dst[1],dst[0]

    //H-pel position AVG --> get Q-pel pos pixel
    Q8AVGR(xr1,xr1,xr9);
    
    S32STD(xr1,dst,0);
    dst += dstStride;
  }
}

static void put_h264_qpel4_v_lowpass_avg_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int pos){

  uint32_t src_aln,src_aln0, src_al4;
  uint32_t src_rs;
  int i;
  uint32_t mul_20 = 0x14141414; //(20,20,20,20)
  uint32_t mul_5 = 0x05050505; //(5,5,5,5)
  uint32_t round = 0x00100010; //(0,16,0,16)

  src_aln = ((uint32_t)src - (srcStride<<1)) & 0xfffffffc;
  src_aln0 = src_aln - (srcStride<<1);
  src_rs = 4 - (((uint32_t)src - (srcStride<<1)) & 3);

  S32I2M(xr15,mul_20);
  S32I2M(xr14,mul_5);
  S32I2M(xr13,round);
  dst -= dstStride;
  for(i=0; i<4; i++) {
    //    src_aln = src_aln0 + i*srcStride;
    src_aln0 += srcStride;
    src_aln = src_aln0;
    src_al4 = src_aln0 + 4;
    //first line
    S32LDIV(xr1, src_aln, srcStride, 0);
    S32LDIV(xr2, src_al4, srcStride, 0);
    //second line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    //ALN 1st and 2nd line 4 pixel
    S32ALN(xr1,xr2,xr1,src_rs); 
    S32ALN(xr2,xr4,xr3,src_rs);
    //third line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    Q8MUL(xr10,xr2,xr14,xr9); //2nd 4 pixel mul 5 <-- here
    //ALN 3rd line 4 pixel
    S32ALN(xr3,xr4,xr3,src_rs);

    if(pos == 0)
      S32ALN(xr12,xr3,xr0,0);

    Q8MUL(xr8,xr3,xr15,xr7); //3rd 4 pixel mul 20
    //fourth line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    S32LDIV(xr2, src_aln, srcStride, 0); // <--here
    S32LDIV(xr5, src_al4, srcStride, 0);
    //ALN 4th line 4 pixel
    S32ALN(xr4,xr4,xr3,src_rs);

    if(pos != 0)
      S32ALN(xr12,xr4,xr0,0);

    //fiveth line
    //ALN 5th line 4 pixel
    S32ALN(xr5,xr5,xr2,src_rs);

    S32LDIV(xr11, src_aln, srcStride, 0); // <--here
    S32LDIV(xr2, src_al4, srcStride, 0);
    Q8MUL(xr6,xr5,xr14,xr5); //5th 4 pixel mul 5
    Q8MUL(xr4,xr4,xr15,xr3); //4th 4 pixel mul 20
    
    //sixth line
    //ALN 6th line 4 pixel
    S32ALN(xr2,xr2,xr11,src_rs);
    
    //1st line + 6th line
    Q8ADDE_AA(xr2,xr1,xr2,xr1);
    
    //1st line + 6th line + 3rd line - 2nd line
    Q16ACC_SS(xr2,xr8,xr10,xr0);
    Q16ACC_SS(xr1,xr7,xr9,xr0);
    
    //1st line + 6th line + 3rd line - 2nd line + 4th line - 5th line
    Q16ACC_SS(xr2,xr4,xr6,xr0);
    Q16ACC_SS(xr1,xr3,xr5,xr0);

    //+ round
    Q16ADD_AA_WW(xr2,xr2,xr13,xr0);
    Q16ADD_AA_WW(xr1,xr1,xr13,xr0);

    // >>5 and clip to (0~255)
    Q16SAR(xr2,xr2,xr1,xr1,5);
    Q16SAT(xr1,xr2,xr1); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
    //AVGround
    
    Q8AVGR(xr1,xr12,xr1);

    S32SDIV(xr1, dst, dstStride, 0);
  }

}

static void put_h264_qpel8_h_lowpass_avg_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int pos)
{
  uint32_t src_aln1,src_aln2, src_org;
  uint32_t src_rs1,src_rs2,src_rso;
  int i,j;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t round = 0x100010; //(16,16)
  src_aln1 = ((uint32_t)src - 2) & 0xfffffffc;
  src_aln2 = (uint32_t)src & 0xfffffffc;
  src_org  = ((uint32_t)src + pos) & 0xfffffffc;
  src_rs1 = 4 - (((uint32_t)src - 2) & 3);
  src_rs2 = 4 - ((uint32_t)src & 3);
  src_rso = 4 - (((uint32_t)src + pos) & 3);

  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  S32I2M(xr13,round);

  dst -= 4;
  for(i=0; i<8; i++) {
    for(j=0; j<2; j++) {
      //2 pixel
      S32LDD(xr1,src_aln1,0);
      S32LDD(xr2,src_aln1,4);
      S32LDD(xr3,src_aln1,8);
      i_pref(0, src_aln1 + srcStride, 0);
      src_aln1 += 4;
      //for pixel[0]
      S32ALN(xr4,xr2,xr1,src_rs1); //xr4 <- src[-2] ~ src[1]
      S32ALN(xr5,xr3,xr2,src_rs1); //xr5 <- src[2] ~ src[5]
      //for pixel[1]
      S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
      D32SLR(xr1,xr5,xr0,xr0,8);
      S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
      Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
      Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
      Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
      Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
      Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
      Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
      S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
      Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
      
      Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
      Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
      Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
      S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
      Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

      S32SFL(xr0,xr3,xr1,xr1,ptn3);
      Q16ADD_AA_WW(xr0,xr1,xr13,xr11); //xr11 <- tow new pixel


      //2 pixel
      S32LDD(xr1,src_aln2,0);
      S32LDD(xr2,src_aln2,4);
      S32LDD(xr3,src_aln2,8);
      i_pref(0, src_aln2 + srcStride, 0);
      src_aln2 += 4;
      //for pixel[0]
      S32ALN(xr4,xr2,xr1,src_rs2); //xr4 <- src[-2] ~ src[1]
      S32ALN(xr5,xr3,xr2,src_rs2); //xr5 <- src[2] ~ src[5]

      ///!!!!!! xr9 stores original 4 pixels of the reference block /////////////
      S32LDD(xr1,src_org,0);
      S32LDD(xr2,src_org,4);
      src_org += 4;

      //for pixel[1]
      S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]

      ///!!!!!! xr9 stores original 4 pixels of the reference block /////////////
      S32ALN(xr9,xr2,xr1,src_rso); //xr9 <- src[4] ~ src[0]
      
      D32SLR(xr1,xr5,xr0,xr0,8);
      S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
      Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
      Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
      Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
      Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
      Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
      Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
      S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
      Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
      Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
      Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
      Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
      S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
      Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

      S32SFL(xr0,xr3,xr1,xr1,ptn3);
      Q16ADD_AA_WW(xr0,xr1,xr13,xr12); //xr12 <- tow new pixel
  
      // >>5 and clip to (0~255)
      Q16SAR(xr12,xr12,xr11,xr11,5);
      Q16SAT(xr1,xr12,xr11); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
      //H-pel position AVG --> get Q-pel pos pixel
      Q8AVGR(xr1,xr1,xr9);
      S32SDI(xr1, dst, 4);
    }
    src_org  += srcStride - 8;
    src_aln1 += srcStride - 8;
    src_aln2 += srcStride - 8;
    dst += dstStride - 8;
  }
}



#if 0
static void put_h264_qpel8_v_lowpass_avg_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int pos)
{
  const int w=8;
  uint8_t *cm = ff_cropTbl + 1024;
  int i;
  for(i=0; i<w; i++)
  {
    const int srcB= src[-2*srcStride];
    const int srcA= src[-1*srcStride];
    const int src0= src[0 *srcStride];
    const int src1= src[1 *srcStride];
    const int src2= src[2 *srcStride];
    const int src3= src[3 *srcStride];
    const int src4= src[4 *srcStride];
    const int src5= src[5 *srcStride];
    const int src6= src[6 *srcStride];
    const int src7= src[7 *srcStride];
    const int src8= src[8 *srcStride];
    const int src9= src[9 *srcStride];
    const int src10=src[10*srcStride];

    dst[0*dstStride] = cm[(((src0+src1)*20 - (srcA+src2)*5 + (srcB+src3)) + 16)>>5];
    dst[1*dstStride] = cm[(((src1+src2)*20 - (src0+src3)*5 + (srcA+src4)) + 16)>>5];
    dst[2*dstStride] = cm[(((src2+src3)*20 - (src1+src4)*5 + (src0+src5)) + 16)>>5];
    dst[3*dstStride] = cm[(((src3+src4)*20 - (src2+src5)*5 + (src1+src6)) + 16)>>5];
    dst[4*dstStride] = cm[(((src4+src5)*20 - (src3+src6)*5 + (src2+src7)) + 16)>>5];
    dst[5*dstStride] = cm[(((src5+src6)*20 - (src4+src7)*5 + (src3+src8)) + 16)>>5];
    dst[6*dstStride] = cm[(((src6+src7)*20 - (src5+src8)*5 + (src4+src9)) + 16)>>5];
    dst[7*dstStride] = cm[(((src7+src8)*20 - (src6+src9)*5 + (src5+src10)) + 16)>>5];

    dst[0*dstStride] = (dst[0*dstStride] + src[(pos+0)*srcStride] + 1) / 2;
    dst[1*dstStride] = (dst[1*dstStride] + src[(pos+1)*srcStride] + 1) / 2;
    dst[2*dstStride] = (dst[2*dstStride] + src[(pos+2)*srcStride] + 1) / 2;
    dst[3*dstStride] = (dst[3*dstStride] + src[(pos+3)*srcStride] + 1) / 2;
    dst[4*dstStride] = (dst[4*dstStride] + src[(pos+4)*srcStride] + 1) / 2;
    dst[5*dstStride] = (dst[5*dstStride] + src[(pos+5)*srcStride] + 1) / 2;
    dst[6*dstStride] = (dst[6*dstStride] + src[(pos+6)*srcStride] + 1) / 2;
    dst[7*dstStride] = (dst[7*dstStride] + src[(pos+7)*srcStride] + 1) / 2;

    dst++;
    src++;
  }
}
#else
static void put_h264_qpel8_v_lowpass_avg_mxu(uint8_t *origdst, uint8_t *src, int dstStride, int srcStride, int pos)
{
  const int w=8;
  uint8_t *cm = ff_cropTbl + 1024;
  int i, off, align, posalign;
  uint8_t *dst, *origsrc, *src1, *origsrc2, *avgsrc, *avgsrc1;

  S32I2M (xr8, 0x14101410);
  avgsrc = src + pos*srcStride;
  src = src - 2*srcStride;
  align = (unsigned int)src & 0x03;
  origsrc = src - align;
  align = 4 - align;
  S32SFL (xr8, xr8, xr8, xr9, ptn2);  //xr8: 0x14141414, xr9: 0x10101010
  D32SLR (xr7, xr8, xr0, xr0, 2);     //xr7: 0x05050505

  posalign = (unsigned int)avgsrc & 0x03;
  origsrc2 = avgsrc - posalign;
  posalign = 4 - posalign;

  for (off = 0; off < 2; off++)
  {
    src = origsrc;
    avgsrc = origsrc2 - srcStride;
    dst = origdst - dstStride;

      S32LDD (xr1, src, 0);
      S32LDD (xr2, src, 4);
      S32LDIV(xr3, src, srcStride, 0);
      S32LDD (xr4, src, 4);
      S32ALN (xr15, xr2, xr1, align);    //xr15:src[-2*srcStride],srcB
      S32ALN (xr14, xr4, xr3, align);    //xr14:src[-1*srcStride],srcA

      S32LDIV(xr1, src, srcStride, 0);
      S32LDD (xr2, src, 4);
      S32LDIV(xr3, src, srcStride, 0);
      S32LDD (xr4, src, 4);
      S32ALN (xr5, xr2, xr1, align);    // xr5: src[0*srcStride],src0
      S32LDIV(xr1, src, srcStride, 0);
      S32LDD (xr2, src, 4);
      S32ALN (xr6, xr4, xr3, align);    // xr6: src[1*srcStride],src1
      S32ALN (xr12, xr2, xr1, align);   // xr12: src[2*srcStride],src2

    for (i = 0; i < 8; i++)
    {
      S32LDIV(xr3, src, srcStride, 0);
      S32LDD (xr4, src, 4);

      Q8ADDE_AA (xr11, xr15,xr9, xr10);  //xr11,xr10: dst[0*dstStride][3-0] srcB+16
      Q8MAC_SS  (xr11, xr14,xr7, xr10);  //xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*srcA

      Q8MAC_AA  (xr11, xr5, xr8, xr10); // xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*srcA+20*src0
      Q8MAC_AA  (xr11, xr6, xr8, xr10); // xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*srcA+20*(src0+src1)

      S32ALN (xr13, xr4, xr3, align);   // xr13: src[3*srcStride],src3
      Q8MAC_SS  (xr11, xr12,xr7, xr10); // xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*(srcA+src2)+20*(src0+src1)
      Q8ACCE_AA (xr11, xr13,xr0, xr10);  // xr11,xr10: dst[0*dstStride][3-0] srcB+src3+16-5*(srcA+src2)+20*(src0+src1)

      S32LDIV (xr2, avgsrc, srcStride, 0);
      S32LDD (xr4, avgsrc, 4);
      Q16SAR (xr11, xr11, xr10, xr10, 5);   //dst[0*dstStride] >> 5
      Q16SAT (xr10, xr11, xr10);            //dst[0*dstStride]: cm[exp >> 5]
      S32ALN (xr4, xr4, xr2, posalign);
      Q8AVGR (xr10, xr10, xr4);             //(dst[0*dstStride] + src[(pos+0)*srcStride] + 1)/2
      S32SDIV(xr10, dst, dstStride, 0);                //dst[0*dstStride]

      D32SLL (xr15, xr14, xr5, xr14, 0);
      D32SLL (xr5,  xr6,  xr12,xr6,  0);
      D32SLL (xr12, xr13, xr0, xr0,  0);
    }
    origsrc  += 4;
    origsrc2 += 4;
    origdst  += 4;
  }
}
#endif

static void put_h264_qpel16_h_lowpass_avg_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int pos){

    put_h264_qpel8_h_lowpass_avg_mxu(dst  , src  , dstStride, srcStride, pos);
    put_h264_qpel8_h_lowpass_avg_mxu(dst+8, src+8, dstStride, srcStride, pos);
    src += 8*srcStride;
    dst += 8*dstStride;
    put_h264_qpel8_h_lowpass_avg_mxu(dst  , src  , dstStride, srcStride, pos);
    put_h264_qpel8_h_lowpass_avg_mxu(dst+8, src+8, dstStride, srcStride, pos);

}

static void put_h264_qpel16_v_lowpass_avg_mxu(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int pos){

    put_h264_qpel8_v_lowpass_avg_mxu(dst  , src  , dstStride, srcStride, pos);
    put_h264_qpel8_v_lowpass_avg_mxu(dst+8, src+8, dstStride, srcStride, pos);
    src += 8*srcStride;
    dst += 8*dstStride;
    put_h264_qpel8_v_lowpass_avg_mxu(dst  , src  , dstStride, srcStride, pos);
    put_h264_qpel8_v_lowpass_avg_mxu(dst+8, src+8, dstStride, srcStride, pos);

}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Interpolate AVG
static void put_h264_qpel4_h_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int srcStride, int src2Stride){

  uint32_t src_aln1,src_aln2;
  uint32_t src_rs1,src_rs2;
  int i;
  uint32_t mul_1 = 0x14140501; //(20,20,5,1)
  uint32_t mul_2 = 0x01050105; //(1,5,1,5)
  uint32_t round = 0x100010; //(16,16)
  src_aln1 = ((uint32_t)src - 2) & 0xfffffffc;
  src_aln2 = (uint32_t)src & 0xfffffffc;
  src_rs1 = 4 - (((uint32_t)src - 2) & 3);
  src_rs2 = 4 - ((uint32_t)src & 3);

  S32I2M(xr15,mul_2);
  S32I2M(xr14,mul_1);
  S32I2M(xr13,round);

  src2 -= 4;
  for(i=0; i<4; i++) {
    //2 pixel
    S32LDD(xr1,src_aln1,0);
    S32LDD(xr2,src_aln1,4);
    S32LDD(xr3,src_aln1,8);
    src_aln1 += srcStride;
    i_pref(0, src_aln1, 0);
    //for pixel[0]
    S32ALN(xr4,xr2,xr1,src_rs1); //xr4 <- src[-2] ~ src[1]
    S32ALN(xr5,xr3,xr2,src_rs1); //xr5 <- src[2] ~ src[5]
    //for pixel[1]
    S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]
    
    D32SLR(xr1,xr5,xr0,xr0,8);
    S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
    Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
    Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
    Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
    Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
    Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
    Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
    S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
    Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
    Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
    Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
    Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
    S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
    Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

    S32SFL(xr0,xr3,xr1,xr1,ptn3);
    Q16ADD_AA_WW(xr0,xr1,xr13,xr11); //xr11 <- tow new pixel


    //2 pixel
    S32LDD(xr1,src_aln2,0);
    S32LDD(xr2,src_aln2,4);
    S32LDD(xr3,src_aln2,8);
    src_aln2 += srcStride;
    i_pref(0, src_aln2, 0);
    //for pixel[0]
    S32ALN(xr4,xr2,xr1,src_rs2); //xr4 <- src[-2] ~ src[1]
    S32ALN(xr5,xr3,xr2,src_rs2); //xr5 <- src[2] ~ src[5]

    ///!!!!!! xr9 stores original 4 pixels of the reference block /////////////
    S32LDI(xr9,src2,4);

    //for pixel[1]
    S32ALN(xr6,xr5,xr4,3); //xr6 <- src[-1] ~ src[2]

    D32SLR(xr1,xr5,xr0,xr0,8);
    S32SFL(xr0,xr1,xr5,xr7,ptn3); //xr7 <- src[4],src[3],src[3],src[2]
    
    Q8MUL(xr2,xr4,xr14,xr1); //xr2 <- 20*src[1],20*src[0]  xr1 <- 5*src[-1],src[-2]
    Q8MUL(xr8,xr7,xr15,xr7); //xr8 <- src[4],5*src[3]  xr7 <- src[3],5*src[2]
    Q8MUL(xr4,xr6,xr14,xr3); //xr4 <- 20*src[2],20*src[1]  xr3 <- 5*src[0],src[-1]
    
    Q16ADD_AA_XW(xr2,xr2,xr2,xr0); //xr2 <- 20*src[1] + 20*src[0]
    Q16ADD_SS_XW(xr0,xr1,xr1,xr1); //xr1 <- src[-2] - 5*src[-1]
    Q16ADD_SS_XW(xr0,xr7,xr7,xr7); //xr7 <- src[3] - 5*src[2]
    S32SFL(xr1,xr0,xr1,xr0,ptn3); //xr1 <- src[-2] - 5*src[-1]
    
    Q16ACC_AA(xr0,xr2,xr7,xr1); //xr1 <- 20*src[1] + 20*src[0] + src[-2] - 5*src[-1] + src[3] - 5*src[2]
    
    Q16ADD_AA_XW(xr4,xr4,xr4,xr0); //xr4 <- 20*src[1] + 20*src[2]
    Q16ADD_SS_XW(xr3,xr3,xr3,xr0); //xr3 <- src[-1] - 5*src[0]
    Q16ADD_SS_XW(xr0,xr8,xr8,xr8); //xr8 <- src[4] - 5*src[3]
    S32SFL(xr3,xr0,xr3,xr0,ptn3); //xr3 <- src[-1] - 5*src[0]
    
    Q16ACC_AA(xr0,xr4,xr8,xr3); //xr3 <- 20*src[1] + 20*src[2] + src[-1] - 5*src[0] + src[4] - 5*src[3]

    S32SFL(xr0,xr3,xr1,xr1,ptn3);
    Q16ADD_AA_WW(xr0,xr1,xr13,xr12); //xr12 <- tow new pixel
  
    // >>5 and clip to (0~255)
    Q16SAR(xr12,xr12,xr11,xr11,5);
    Q16SAT(xr1,xr12,xr11); //xr1 <- dst[3],dst[2],dst[1],dst[0]

    //H-pel position AVG --> get Q-pel pos pixel
    Q8AVGR(xr1,xr1,xr9);
    
    S32STD(xr1,dst,0);
    dst += dstStride;
  }

}

static void put_h264_qpel4_v_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int srcStride, int src2Stride){

  uint32_t src_aln,src_aln0, src_al4;
  uint32_t src_rs;
  int i;
  uint32_t mul_20 = 0x14141414; //(20,20,20,20)
  uint32_t mul_5 = 0x05050505; //(5,5,5,5)
  uint32_t round = 0x00100010; //(0,16,0,16)

  src_aln = ((uint32_t)src - (srcStride<<1)) & 0xfffffffc;
  src_aln0 = src_aln - (srcStride<<1);
  src_rs = 4 - (((uint32_t)src - (srcStride<<1)) & 3);

  S32I2M(xr15,mul_20);
  S32I2M(xr14,mul_5);
  S32I2M(xr13,round);
  dst -= dstStride;
  src2 -= 4;
  for(i=0; i<4; i++) {
    src_aln0 += srcStride;
    src_aln = src_aln0;
    src_al4 = src_aln0 + 4;
    //first line
    S32LDIV(xr1, src_aln, srcStride, 0);
    S32LDIV(xr2, src_al4, srcStride, 0);
    //second line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    //ALN 1st and 2nd line 4 pixel
    S32ALN(xr1,xr2,xr1,src_rs); 
    S32ALN(xr2,xr4,xr3,src_rs);
    //third line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    Q8MUL(xr10,xr2,xr14,xr9); //2nd 4 pixel mul 5 <-- here
    //ALN 3rd line 4 pixel
    S32ALN(xr3,xr4,xr3,src_rs);

    Q8MUL(xr8,xr3,xr15,xr7); //3rd 4 pixel mul 20
    //fourth line
    S32LDIV(xr3, src_aln, srcStride, 0);
    S32LDIV(xr4, src_al4, srcStride, 0);
    S32LDIV(xr2, src_aln, srcStride, 0); // <--here
    S32LDIV(xr5, src_al4, srcStride, 0);
    //ALN 4th line 4 pixel
    S32ALN(xr4,xr4,xr3,src_rs);

    S32LDI(xr12,src2,4);

    //fiveth line
    //ALN 5th line 4 pixel
    S32ALN(xr5,xr5,xr2,src_rs);

    S32LDIV(xr11, src_aln, srcStride, 0); // <--here
    S32LDIV(xr2, src_al4, srcStride, 0);
    Q8MUL(xr6,xr5,xr14,xr5); //5th 4 pixel mul 5
    Q8MUL(xr4,xr4,xr15,xr3); //4th 4 pixel mul 20
    
    //sixth line
    //ALN 6th line 4 pixel
    S32ALN(xr2,xr2,xr11,src_rs);
    
    //1st line + 6th line
    Q8ADDE_AA(xr2,xr1,xr2,xr1);
    
    //1st line + 6th line + 3rd line - 2nd line
    Q16ACC_SS(xr2,xr8,xr10,xr0);
    Q16ACC_SS(xr1,xr7,xr9,xr0);
    
    //1st line + 6th line + 3rd line - 2nd line + 4th line - 5th line
    Q16ACC_SS(xr2,xr4,xr6,xr0);
    Q16ACC_SS(xr1,xr3,xr5,xr0);

    //+ round
    Q16ADD_AA_WW(xr2,xr2,xr13,xr0);
    Q16ADD_AA_WW(xr1,xr1,xr13,xr0);

    // >>5 and clip to (0~255)
    Q16SAR(xr2,xr2,xr1,xr1,5);
    Q16SAT(xr1,xr2,xr1); //xr1 <- dst[3],dst[2],dst[1],dst[0]
    
    //AVGround
    
    Q8AVGR(xr1,xr12,xr1);

    S32SDIV(xr1, dst, dstStride, 0);
  }
}



#if 0
static void put_h264_qpel8_h_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int srcStride, int src2Stride)
{
  const int h=8;
  uint8_t *cm = ff_cropTbl + 1024;
  int i;
  for(i=0; i<h; i++)
  {
    dst[0] = cm[(((src[0]+src[1])*20 - (src[-1]+src[2])*5 + (src[-2]+src[3 ])) + 16)>>5];
    dst[1] = cm[(((src[1]+src[2])*20 - (src[0 ]+src[3])*5 + (src[-1]+src[4 ])) + 16)>>5];
    dst[2] = cm[(((src[2]+src[3])*20 - (src[1 ]+src[4])*5 + (src[0 ]+src[5 ])) + 16)>>5];
    dst[3] = cm[(((src[3]+src[4])*20 - (src[2 ]+src[5])*5 + (src[1 ]+src[6 ])) + 16)>>5];
    dst[4] = cm[(((src[4]+src[5])*20 - (src[3 ]+src[6])*5 + (src[2 ]+src[7 ])) + 16)>>5];
    dst[5] = cm[(((src[5]+src[6])*20 - (src[4 ]+src[7])*5 + (src[3 ]+src[8 ])) + 16)>>5];
    dst[6] = cm[(((src[6]+src[7])*20 - (src[5 ]+src[8])*5 + (src[4 ]+src[9 ])) + 16)>>5];
    dst[7] = cm[(((src[7]+src[8])*20 - (src[6 ]+src[9])*5 + (src[5 ]+src[10])) + 16)>>5];

    dst[0] = (dst[0] + src2[0] + 1) / 2;
    dst[1] = (dst[1] + src2[1] + 1) / 2;
    dst[2] = (dst[2] + src2[2] + 1) / 2;
    dst[3] = (dst[3] + src2[3] + 1) / 2;
    dst[4] = (dst[4] + src2[4] + 1) / 2;
    dst[5] = (dst[5] + src2[5] + 1) / 2;
    dst[6] = (dst[6] + src2[6] + 1) / 2;
    dst[7] = (dst[7] + src2[7] + 1) / 2;

    dst+=dstStride;
    src+=srcStride;
    src2 += src2Stride;
  }
}
#else
static void put_h264_qpel8_h_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *avgsrc, int dstStride, int srcStride, int avgsrcStride)
{
  uint8_t *dstn, *src1;
  int align, align1, align2, align3, alignx;
  int avgalign;

  S32I2M (xr7, 0x14101410);
  src1 = src - 2;
  align = (unsigned int)src1 & 0x03;
  src1 = src1 - align;
  i_pref(0, src1, 0);
  align = 4 - align;

  align3 = 3;
  align2 = 2;
  align1 = 1;
  alignx = align - 1;

  avgalign = (unsigned int)avgsrc & 0x03;
  avgsrc = avgsrc - avgalign;
  avgalign = 4 - avgalign;

  S32SFL (xr7, xr7, xr7, xr9, ptn2);  //xr7: 0x14141414, xr9: 0x10101010
  D32SLR (xr8, xr7, xr0, xr0, 2);     //xr8: 0x05050505
  src1 -=srcStride;
  dst  -=dstStride;
  avgsrc -= avgsrcStride;
  dstn = dst + (dstStride << 3);
  //for(i=0; i<h; i++)
  while (dst < dstn)
  {
    S32LDIV (xr1,  src1, srcStride, 0x0);
    S32LDD  (xr2,  src1, 0x4);
    S32LDD  (xr3,  src1, 0x8);
    S32LDD  (xr4,  src1, 0xc);
    i_pref(0, src1 + srcStride, 0);
//
    S32ALN (xr5,  xr2, xr1, align);    //xr5: src[1][0][-1][-2]
    S32ALN (xr6,  xr3, xr2, align);    //xr6: src[5][4][3][2]
    S32ALN (xr1,  xr4, xr3, align);    //xr1: src[9][8][7][6]
    S32ALN (xr3,  xr4, xr3, alignx);   //xr3: src[10][9][8][7]
//
    S32ALN (xr10, xr6, xr5, align3);  //xr10: src[2][1][0][-1]
    S32ALN (xr11, xr6, xr5, align2);  //xr11: src[3][2][1][0]
    S32ALN (xr12, xr6, xr5, align1);  //xr12: src[4][3][2][1]
    S32ALN (xr13, xr1, xr6, align3);  //xr13: src[6][5][4][3]
//
    Q8ADDE_AA (xr15, xr5, xr9, xr14);    //src[1...-2] + 16
    Q8MAC_AA  (xr15, xr11, xr7, xr14);    // + src[3...0]*20
    Q8MAC_AA  (xr15, xr12, xr7, xr14);    // + src[3...1]*20
    Q8MAC_SS  (xr15, xr10, xr8, xr14);    // + src[2...-1]*5
    Q8MAC_SS  (xr15, xr6,  xr8, xr14);    // + src[5...2]*5
    Q8ACCE_AA (xr15, xr13, xr0, xr14);    // + src[6...3]
//
    S32ALN (xr5, xr1, xr6, align2);  //xr5: src[7][6][5][4]
    S32ALN (xr10,xr1, xr6, align1);  //xr10: src[8][7][6][5]
//
    Q8ADDE_AA (xr4, xr3, xr9, xr3);     //src[10...7] + 16
    Q8MAC_AA  (xr4, xr5, xr7, xr3);    // + src[7...4]*20
    Q8MAC_AA  (xr4, xr10,xr7, xr3);    // + src[8...5]*20
    Q8MAC_SS  (xr4, xr13,xr8, xr3);    // + src[6...3]*5
    Q8MAC_SS  (xr4, xr1, xr8, xr3);    // + src[9...6]*5
    Q8ACCE_AA (xr4, xr6, xr0, xr3);    // + src[5...2]
//
    Q16SAR    (xr15, xr15, xr14, xr14, 5);
    Q16SAT    (xr15, xr15, xr14);
    Q16SAR    (xr4,  xr4,  xr3,  xr3,  5);
    Q16SAT    (xr4,  xr4,  xr3);

    S32LDIV   (xr1,  avgsrc, avgsrcStride, 0x0);
    S32LDD    (xr2,  avgsrc, 0x4);
    S32LDD    (xr3,  avgsrc, 0x8);
    S32ALN    (xr1, xr2, xr1, avgalign);  //xr1: avgsrc[3][2][1][0]
    S32ALN    (xr2, xr3, xr2, avgalign);  //xr2: avgsrc[7][6][5][4]

    Q8AVGR    (xr15, xr15, xr1);
    Q8AVGR    (xr4,  xr4,  xr2);
    S32SDIV   (xr15, dst, dstStride, 0);
    S32STD    (xr4, dst, 4);
  }
}
#endif

#if 0
static void put_h264_qpel8_v_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *avgsrc, int dstStride, int srcStride, int avgsrcStride)
{
  const int w=8;
  uint8_t *cm = ff_cropTbl + 1024;
  int i;
  for(i=0; i<w; i++)
  {
    const int srcB= src[-2*srcStride];
    const int srcA= src[-1*srcStride];
    const int src0= src[0 *srcStride];
    const int src1= src[1 *srcStride];
    const int src2= src[2 *srcStride];
    const int src3= src[3 *srcStride];
    const int src4= src[4 *srcStride];
    const int src5= src[5 *srcStride];
    const int src6= src[6 *srcStride];
    const int src7= src[7 *srcStride];
    const int src8= src[8 *srcStride];
    const int src9= src[9 *srcStride];
    const int src10=src[10*srcStride];

    dst[0*dstStride] = cm[(((src0+src1)*20 - (srcA+src2)*5 + (srcB+src3)) + 16)>>5];
    dst[1*dstStride] = cm[(((src1+src2)*20 - (src0+src3)*5 + (srcA+src4)) + 16)>>5];
    dst[2*dstStride] = cm[(((src2+src3)*20 - (src1+src4)*5 + (src0+src5)) + 16)>>5];
    dst[3*dstStride] = cm[(((src3+src4)*20 - (src2+src5)*5 + (src1+src6)) + 16)>>5];
    dst[4*dstStride] = cm[(((src4+src5)*20 - (src3+src6)*5 + (src2+src7)) + 16)>>5];
    dst[5*dstStride] = cm[(((src5+src6)*20 - (src4+src7)*5 + (src3+src8)) + 16)>>5];
    dst[6*dstStride] = cm[(((src6+src7)*20 - (src5+src8)*5 + (src4+src9)) + 16)>>5];
    dst[7*dstStride] = cm[(((src7+src8)*20 - (src6+src9)*5 + (src5+src10)) + 16)>>5];

    dst[0*dstStride] = (dst[0*dstStride] + avgsrc[0*avgsrcStride] + 1) / 2;
    dst[1*dstStride] = (dst[1*dstStride] + avgsrc[1*avgsrcStride] + 1) / 2;
    dst[2*dstStride] = (dst[2*dstStride] + avgsrc[2*avgsrcStride] + 1) / 2;
    dst[3*dstStride] = (dst[3*dstStride] + avgsrc[3*avgsrcStride] + 1) / 2;
    dst[4*dstStride] = (dst[4*dstStride] + avgsrc[4*avgsrcStride] + 1) / 2;
    dst[5*dstStride] = (dst[5*dstStride] + avgsrc[5*avgsrcStride] + 1) / 2;
    dst[6*dstStride] = (dst[6*dstStride] + avgsrc[6*avgsrcStride] + 1) / 2;
    dst[7*dstStride] = (dst[7*dstStride] + avgsrc[7*avgsrcStride] + 1) / 2;
    dst++;
    src++;
    avgsrc++;
  }
}
#else
static void put_h264_qpel8_v_lowpass_itpavg_mxu(uint8_t *origdst, uint8_t *src, uint8_t *avgsrc, int dstStride, int srcStride, int avgsrcStride)
{
  int i, off, align, avgalign;
  uint8_t *dst, *origsrc, *src1, *origsrc2, *avgsrc1;

  S32I2M (xr8, 0x14101410);
  src = src - 2*srcStride;
  align = (unsigned int)src & 0x03;
  origsrc = src - align;
  align = 4 - align;
  S32SFL (xr8, xr8, xr8, xr9, ptn2);  //xr8: 0x14141414, xr9: 0x10101010
  D32SLR (xr7, xr8, xr0, xr0, 2);     //xr7: 0x05050505

  avgalign = (unsigned int)avgsrc & 0x03;
  origsrc2 = avgsrc - avgalign;
  avgalign = 4 - avgalign;

  for (off = 0; off < 2; off++)
  {
    src = origsrc;
    avgsrc = origsrc2 - avgsrcStride;
    dst = origdst - dstStride;
      S32LDD (xr1, src, 0);
      S32LDD (xr2, src, 4);
      S32LDIV(xr3, src, srcStride, 0);
      S32LDD (xr4, src, 4);
      S32ALN (xr15, xr2, xr1, align);    //xr15: src[-2*srcStride],srcB
      S32ALN (xr14, xr4, xr3, align);    //xr14: src[-1*srcStride],srcA

      S32LDIV(xr1, src, srcStride, 0);
      S32LDD (xr2, src, 4);
      S32LDIV(xr3, src, srcStride, 0);
      S32LDD (xr4, src, 4);
      S32ALN (xr5, xr2, xr1, align);    // xr5: src[0*srcStride],src0
      S32LDIV(xr1, src, srcStride, 0);
      S32LDD (xr2, src, 4);
      S32ALN (xr6, xr4, xr3, align);    // xr6: src[1*srcStride],src1
      S32ALN (xr12,xr2, xr1, align);    // xr12: src[2*srcStride],src2

    for (i = 0; i < 8; i++)
    {
      S32LDIV(xr3, src, srcStride, 0);
      S32LDD (xr4, src, 4);
      Q8ADDE_AA (xr11, xr15,xr9, xr10);  //xr11,xr10: dst[0*dstStride][3-0] srcB+16
      Q8MAC_SS  (xr11, xr14,xr7, xr10);  //xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*srcA
      Q8MAC_AA  (xr11, xr5, xr8, xr10); // xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*srcA+20*src0
      Q8MAC_AA  (xr11, xr6, xr8, xr10); // xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*srcA+20*(src0+src1)
      S32ALN (xr13,xr4, xr3, align);    // xr13: src[3*srcStride],src3
      Q8MAC_SS  (xr11, xr12,xr7, xr10); // xr11,xr10: dst[0*dstStride][3-0] srcB+16-5*(srcA+src2)+20*(src0+src1)
      Q8ACCE_AA (xr11, xr13,xr0, xr10);  // xr11,xr10: dst[0*dstStride][3-0] srcB+src3+16-5*(srcA+src2)+20*(src0+src1)

      S32LDIV(xr2, avgsrc, avgsrcStride, 0);
      S32LDD (xr4, avgsrc, 4);
      Q16SAR (xr11, xr11, xr10, xr10, 5);   //dst[0*dstStride] >> 5
      Q16SAT (xr10, xr11, xr10);            //dst[0*dstStride]: cm[exp >> 5]
      S32ALN (xr4, xr4, xr2, avgalign);
      Q8AVGR (xr10, xr10, xr4);             //(dst[0*dstStride] + avgsrc[0*srcStride] + 1)/2
      S32SDIV(xr10, dst, dstStride, 0);     //dst[0*dstStride]

      D32SLL (xr15, xr14, xr5, xr14, 0);
      D32SLL (xr5,  xr6,  xr12,xr6,  0);
      D32SLL (xr12, xr13, xr0, xr0,  0);
    }
    origsrc  += 4;
    origsrc2 += 4;
    origdst  += 4;
  }
}
#endif

static void put_h264_qpel16_h_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int srcStride, int src2Stride){

    put_h264_qpel8_h_lowpass_itpavg_mxu(dst  , src  , src2  , dstStride, srcStride, src2Stride);
    put_h264_qpel8_h_lowpass_itpavg_mxu(dst+8, src+8, src2+8, dstStride, srcStride, src2Stride);
    src += 8*srcStride;
    dst += 8*dstStride;
    src2 += 128;
    put_h264_qpel8_h_lowpass_itpavg_mxu(dst  , src  , src2  , dstStride, srcStride, src2Stride);
    put_h264_qpel8_h_lowpass_itpavg_mxu(dst+8, src+8, src2+8, dstStride, srcStride, src2Stride);

}

static void put_h264_qpel16_v_lowpass_itpavg_mxu(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int srcStride, int src2Stride){

    put_h264_qpel8_v_lowpass_itpavg_mxu(dst  , src  , src2  , dstStride, srcStride, src2Stride);
    put_h264_qpel8_v_lowpass_itpavg_mxu(dst+8, src+8, src2+8, dstStride, srcStride, src2Stride);
    src += 8*srcStride;
    dst += 8*dstStride;
    src2 += 8*16;
    put_h264_qpel8_v_lowpass_itpavg_mxu(dst  , src  , src2  , dstStride, srcStride, src2Stride);
    put_h264_qpel8_v_lowpass_itpavg_mxu(dst+8, src+8, src2+8, dstStride, srcStride, src2Stride);

}


#define H264_MC_MXU(OPNAME, SIZE) \
static void OPNAME ## h264_qpel ## SIZE ## _mc00_c (uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## pixels ## SIZE ## _mxu(dst, src, stride, SIZE);\
}\
static void OPNAME ## h264_qpel ## SIZE ## _mc10_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ##_h_lowpass_avg_mxu(dst,src,stride,stride,0);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc20_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ## _h_lowpass_mxu(dst, src, stride, stride);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc30_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ##_h_lowpass_avg_mxu(dst,src,stride,stride,1);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc01_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_avg_mxu(dst,src,stride,stride,0);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc02_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ## _v_lowpass_mxu(dst, src, stride, stride);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc03_c(uint8_t *dst, uint8_t *src, int stride){\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_avg_mxu(dst,src,stride,stride,1);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc11_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass_mxu(half, src, SIZE, stride);\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_itpavg_mxu(dst,src,half,stride,stride,SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc31_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass_mxu(half, src, SIZE, stride);\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_itpavg_mxu(dst,src+1,half,stride,stride,SIZE);\
}\
static void OPNAME ## h264_qpel ## SIZE ## _mc13_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass_mxu(half, src + stride, SIZE, stride);\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_itpavg_mxu(dst,src,half,stride,stride,SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc33_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    put_h264_qpel ## SIZE ## _h_lowpass_mxu(half, src + stride, SIZE, stride);\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_itpavg_mxu(dst,src+1,half,stride,stride,SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc22_c(uint8_t *dst, uint8_t *src, int stride){\
    int32_t tmp[SIZE*(SIZE+5)];\
    OPNAME ## h264_qpel ## SIZE ## _hv_lowpass_mxu(dst, tmp, src, stride, SIZE*2, stride);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc21_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    int32_t tmp[SIZE*(SIZE+5)];\
    put_h264_qpel ## SIZE ## _hv_lowpass_mxu(half, tmp, src, SIZE, SIZE*2, stride);\
    OPNAME ## h264_qpel ## SIZE ##_h_lowpass_itpavg_mxu(dst,src,half,stride,stride,SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc23_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    int32_t tmp[SIZE*(SIZE+5)];\
    put_h264_qpel ## SIZE ## _hv_lowpass_mxu(half, tmp, src, SIZE, SIZE*2, stride);\
    OPNAME ## h264_qpel ## SIZE ##_h_lowpass_itpavg_mxu(dst,src+stride,half,stride,stride,SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc12_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    int32_t tmp[SIZE*(SIZE+5)];\
    put_h264_qpel ## SIZE ## _hv_lowpass_mxu(half, tmp, src, SIZE, SIZE*2, stride);\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_itpavg_mxu(dst,src,half,stride,stride,SIZE);\
}\
\
static void OPNAME ## h264_qpel ## SIZE ## _mc32_c(uint8_t *dst, uint8_t *src, int stride){\
    uint8_t half[SIZE*SIZE];\
    int32_t tmp[SIZE*(SIZE+5)];\
    put_h264_qpel ## SIZE ## _hv_lowpass_mxu(half, tmp, src, SIZE, SIZE*2, stride);\
    OPNAME ## h264_qpel ## SIZE ##_v_lowpass_itpavg_mxu(dst,src+1,half,stride,stride,SIZE);\
}


H264_MC_MXU(put_, 4)
H264_MC_MXU(put_, 8)
H264_MC_MXU(put_, 16)

H264_LOWPASS(put_       , op_put, op2_put)
H264_LOWPASS(avg_       , op_avg, op2_avg)

H264_MC(put_, 2)
H264_MC(avg_, 4)
H264_MC(avg_, 8)
H264_MC(avg_, 16)
#else

H264_LOWPASS(put_       , op_put, op2_put)
H264_LOWPASS(avg_       , op_avg, op2_avg)
H264_MC(put_, 2)
H264_MC(put_, 4)
H264_MC(put_, 8)
H264_MC(put_, 16)
H264_MC(avg_, 4)
H264_MC(avg_, 8)
H264_MC(avg_, 16)

#endif    //JZ4740_MXU_OPT

#undef op_avg
#undef op_put
#undef op2_avg
#undef op2_put
#endif

#define H264_CHROMA_MC(OPNAME, OP)\
static void OPNAME ## h264_chroma_mc2_c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
    \
    assert(x<8 && y<8 && x>=0 && y>=0);\
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        dst+= stride;\
        src+= stride;\
    }\
}\
\
static void OPNAME ## h264_chroma_mc4_c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
    \
    assert(x<8 && y<8 && x>=0 && y>=0);\
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        OP(dst[2], (A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3]));\
        OP(dst[3], (A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4]));\
        dst+= stride;\
        src+= stride;\
    }\
}\
\
static void OPNAME ## h264_chroma_mc8_c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
    \
    assert(x<8 && y<8 && x>=0 && y>=0);\
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        OP(dst[2], (A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3]));\
        OP(dst[3], (A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4]));\
        OP(dst[4], (A*src[4] + B*src[5] + C*src[stride+4] + D*src[stride+5]));\
        OP(dst[5], (A*src[5] + B*src[6] + C*src[stride+5] + D*src[stride+6]));\
        OP(dst[6], (A*src[6] + B*src[7] + C*src[stride+6] + D*src[stride+7]));\
        OP(dst[7], (A*src[7] + B*src[8] + C*src[stride+7] + D*src[stride+8]));\
        dst+= stride;\
        src+= stride;\
    }\
}

#define op_avg(a, b) a = (((a)+(((b) + 32)>>6)+1)>>1)
#define op_put(a, b) a = (((b) + 32)>>6)

#ifdef JZ4740_MXU_OPT

#define H264_CHROMA_MC_mc2(OPNAME, OP)\
static void OPNAME ## h264_chroma_mc2_c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y){\
    const int A=(8-x)*(8-y);\
    const int B=(  x)*(8-y);\
    const int C=(8-x)*(  y);\
    const int D=(  x)*(  y);\
    int i;\
    \
    assert(x<8 && y<8 && x>=0 && y>=0);\
\
    for(i=0; i<h; i++)\
    {\
        OP(dst[0], (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]));\
        OP(dst[1], (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2]));\
        dst+= stride;\
        src+= stride;\
    }\
}

#if 0
static void put_h264_chroma_mc4_c(uint8_t *dst , uint8_t *src , int stride, int h, int x, int y)
{
   const int A=(8-x)*(8-y);
   const int B=( x)*(8-y);
   const int C=(8-x)*( y);
   const int D=( x)*( y);
   int i;
   ((void) (0));
   for(i=0; i<h; i++)
   {
     dst[0] = ((((A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1])) + 32)>>6);
     dst[1] = ((((A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2])) + 32)>>6);
     dst[2] = ((((A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3])) + 32)>>6);
     dst[3] = ((((A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4])) + 32)>>6);
     dst+= stride;
     src+= stride;
   }
}
#endif
static void put_h264_chroma_mc4_mxu(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y) {
   int i;
   uint8_t *src1, *src2;
   int align, align1;

   align = (unsigned int)src & 0x3;
   src2 = src - align;
   align = 4 - align;
   align1 = align - 1;
   dst -= stride;
   src1 = src2 - stride;

   if ( (x + y) == 0)
   {
     S32LDIV(xr9, src1, stride, 0x0);
     S32LDD (xr2,  src1, 0x4);
     for(i=0; i<h; i++)
     {
       S32ALN  (xr1, xr2, xr9, align);  // src[3], src[2], src[1], src[0]
       S32SDIV (xr1, dst, stride, 0);
       S32LDIV(xr9, src1, stride, 0x0);
       S32LDD (xr2,  src1, 0x4);
     }
     return;
   }

   S32I2M (xr1, x);
   S32I2M (xr2, y);
   S32I2M (xr13, 0x08080808);
   S32SFL (xr0, xr1, xr2, xr5, ptn0);  // xr5: 00xy
   S32SFL (xr0, xr5, xr5, xr5, ptn3);  // xr5: xyxy
   S32SFL (xr3, xr5, xr5, xr4, ptn2);  // xr3: xxxx   xr4:yyyy
   Q8ADD_SS (xr1, xr13, xr3);          // xr1 (8 - x)
   Q8ADD_SS (xr2, xr13, xr4);          // xr2 (8 - y)
   Q8MADL_AA (xr0, xr1, xr2, xr5);     // xr5: AAAA (8-x)*(8-y)
   Q8MADL_AA (xr0, xr3, xr2, xr6);     // xr6: BBBB x*(8-y)
   Q8MADL_AA (xr0, xr1, xr4, xr7);     // xr7: CCCC (8-x)*y
   Q8MADL_AA (xr0, xr3, xr4, xr8);     // xr8: DDDD x*y
   D32SLL (xr15, xr13, xr0, xr0, 2);     // xr15: 0x20202020

   S32LDIV(xr9, src1, stride, 0x0);
   S32LDD (xr2,  src1, 0x4);
   for(i=0; i<h; i++)
   {
     S32LDIV(xr14, src2, stride, 0x0);
     S32LDD (xr4,  src2, 0x4);
     S32ALN(xr1, xr2, xr9, align);  // src[3], src[2], src[1], src[0]
     S32ALN(xr2, xr2, xr9, align1); // src[4], src[3], src[2], src[1]
     S32ALN(xr3, xr4, xr14, align);  // src[stride + 3], 2, 1, 0
     S32ALN(xr4, xr4, xr14, align1); // src[stride + 4], 3, 2, 1

     Q8ADDE_AA (xr11, xr15, xr0, xr10); // 32
     Q8MAC_AA  (xr11, xr1,  xr5, xr10); // A * src + 32
     Q8MAC_AA  (xr11, xr2,  xr6, xr10); // A * src + B * src + 32
     Q8MAC_AA  (xr11, xr3,  xr7, xr10); // A*src + B*src + C*src + 32
     Q8MAC_AA  (xr11, xr4,  xr8, xr10); // A*src + B*src + C*src + D*src +32
     S32LDIV(xr9, src1, stride, 0x0);
     S32LDD (xr2,  src1, 0x4);
     Q16SLR    (xr11, xr11, xr10, xr10, 6);
     S32SFL    (xr0,  xr11, xr10, xr10, ptn1);
     S32SDIV   (xr10, dst, stride, 0);
   }
}
static void put_h264_chroma_mc8_mxu(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y) {
    put_h264_chroma_mc4_mxu(dst  , src  , stride, h, x, y);
    put_h264_chroma_mc4_mxu(dst+4, src+4, stride, h, x, y);
}

H264_CHROMA_MC_mc2(put_   , op_put)
H264_CHROMA_MC(avg_       , op_avg)
#undef op_avg
#undef op_put

static void put_no_rnd_h264_chroma_mc4_mxu(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y) {
  uint32_t src_aln,src_al4,src_al8;
  uint32_t src_rs;
  int i;
  const int A=(8-x)*(8-y);
  const int B=(  x)*(8-y);
  const int C=(8-x)*(  y);
  const int D=(  x)*(  y);
  int mul_badc = (A<<16) | (B<<24) | C | (D<<8);

  src_aln = ((uint32_t)src & 0xfffffffc) - stride;
  src_al4 = src_aln + 4;
  src_al8 = src_aln + 8;
  dst -= stride;
  src_rs = 4 - ((uint32_t)src & 3);

  S32I2M(xr15,mul_badc);
  S32I2M(xr14,28);

  //first line
  S32LDIV(xr1,src_aln,stride,0);
  S32LDIV(xr2,src_al4,stride,0);
  S32LDIV(xr3,src_al8,stride,0);
  
  S32LDIV(xr7,src_aln,stride,0);
  S32LDIV(xr8,src_al4,stride,0);
  S32LDIV(xr9,src_al8,stride,0);

  S32ALN(xr4,xr2,xr1,src_rs);  //xr4 <- src[3:0]
  S32ALN(xr5,xr3,xr2,src_rs);  //xr5 <- src[7:4]

  S32ALN(xr13,xr8,xr7,src_rs); //*********xr13 <- src'[3:0] (src' = src + stride)*****
  S32ALN(xr12,xr9,xr8,src_rs); //xr12 <- src'[7:4]

  S32SFL(xr7,xr4,xr13,xr6,ptn3); //xr7 <- src[3:2],src'[3:2] xr6 <- src[1:0],src'[1:0]

  S32ALN(xr5,xr5,xr4,3); //xr4 <- src[4:1]
  S32ALN(xr12,xr12,xr13,3); //*************xr12 <- src'[4:1]**************

  //dst0
  Q8MUL(xr9,xr6,xr15,xr8);//A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]
  S32SFL(xr6,xr5,xr12,xr3,ptn3); // xr6 <- src[4:3],src'[4:3] xr3 <- src[2:1],src'[2:1]

  //dst2
  Q8MUL(xr7,xr7,xr15,xr10);//A*src[3] + B*src[2] + C*src[stride+3] + D*src[stride+2]

  Q16ACC_AA(xr0,xr14,xr9,xr8);
  Q16ADD_AA_XW(xr0,xr8,xr8,xr11);

  Q16ACC_AA(xr0,xr14,xr7,xr10);
  Q16ADD_AA_XW(xr0,xr10,xr10,xr9);

  //dst1
  Q8MUL(xr3,xr3,xr15,xr8);//A*src[2] + B*src[1] + C*src[stride+2] + D*src[stride+1]

  //dst3
  Q8MUL(xr6,xr6,xr15,xr7);//A*src[4] + B*src[3] + C*src[stride+4] + D*src[stride+3]

  Q16ACC_AA(xr0,xr14,xr3,xr8);
  Q16ADD_AA_XW(xr0,xr8,xr8,xr10);

  Q16ACC_AA(xr0,xr14,xr6,xr7);
  Q16ADD_AA_XW(xr0,xr7,xr7,xr8);

  Q16SLR(xr11,xr11,xr10,xr10,6);
  Q16SLR(xr9,xr9,xr8,xr8,6);

  S32SFL(xr0,xr10,xr11,xr10,ptn3);
  S32SFL(xr0,xr8,xr9,xr11,ptn3);

  Q16SAT(xr11,xr11,xr10); //xr11 <- dst[3],dst[2],dst[1],dst[0]
  S32SDIV(xr11,dst,stride,0);
  
  for(i=0; i<h-1; i++) {
    S32LDIV(xr1,src_aln,stride,0);
    S32LDIV(xr2,src_al4,stride,0);
    S32LDIV(xr3,src_al8,stride,0);
    i_pref(0, src_aln + stride, 0); 
    S32ALN(xr4,xr2,xr1,src_rs);  //xr4 <- src[3:0]
    S32ALN(xr5,xr3,xr2,src_rs);  //xr5 <- src[7:4]
    S32ALN(xr5,xr5,xr4,3); //xr5 <- src[4:1]

    S32SFL(xr7,xr13,xr4,xr6,ptn3); //xr7 <- src[3:2],src'[3:2] xr6 <- src[1:0],src'[1:0]

    //dst0
    Q8MUL(xr9,xr6,xr15,xr8);//A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1]
    S32SFL(xr6,xr12,xr5,xr3,ptn3); // xr6 <- src[4:3],src'[4:3] xr3 <- src[2:1],src'[2:1]
    S32ALN(xr13,xr4,xr0,0);
    S32ALN(xr12,xr5,xr0,0);

    //dst2
    Q8MUL(xr7,xr7,xr15,xr10);//A*src[3] + B*src[2] + C*src[stride+3] + D*src[stride+2]

    Q16ACC_AA(xr0,xr14,xr9,xr8);
    Q16ADD_AA_XW(xr0,xr8,xr8,xr11);

    //dst1
    Q8MUL(xr3,xr3,xr15,xr8);//A*src[2] + B*src[1] + C*src[stride+2] + D*src[stride+1]

    Q16ACC_AA(xr0,xr14,xr7,xr10);
    Q16ADD_AA_XW(xr0,xr10,xr10,xr9);

    //dst3
    Q8MUL(xr6,xr6,xr15,xr7);//A*src[4] + B*src[3] + C*src[stride+4] + D*src[stride+3]
    
    Q16ACC_AA(xr0,xr14,xr3,xr8);
    Q16ADD_AA_XW(xr0,xr8,xr8,xr10);

    Q16ACC_AA(xr0,xr14,xr6,xr7);
    Q16ADD_AA_XW(xr0,xr7,xr7,xr8);

    Q16SLR(xr11,xr11,xr10,xr10,6);
    Q16SLR(xr9,xr9,xr8,xr8,6);

    S32SFL(xr0,xr10,xr11,xr10,ptn3);
    S32SFL(xr0,xr8,xr9,xr11,ptn3);

    Q16SAT(xr11,xr11,xr10); //xr11 <- dst[3],dst[2],dst[1],dst[0]
    S32SDIV(xr11,dst,stride,0);
  }
}

static void put_no_rnd_h264_chroma_mc8_mxu(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y) {
    put_no_rnd_h264_chroma_mc4_mxu(dst  , src  , stride, h, x, y);
    put_no_rnd_h264_chroma_mc4_mxu(dst+4, src+4, stride, h, x, y);
}


#else
H264_CHROMA_MC(put_       , op_put)
H264_CHROMA_MC(avg_       , op_avg)
#undef op_avg
#undef op_put

static void put_no_rnd_h264_chroma_mc8_c(uint8_t *dst/*align 8*/, uint8_t *src/*align 1*/, int stride, int h, int x, int y){
    const int A=(8-x)*(8-y);
    const int B=(  x)*(8-y);
    const int C=(8-x)*(  y);
    const int D=(  x)*(  y);
    int i;

    assert(x<8 && y<8 && x>=0 && y>=0);

    for(i=0; i<h; i++)
    {
        dst[0] = (A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1] + 32 - 4) >> 6;
        dst[1] = (A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2] + 32 - 4) >> 6;
        dst[2] = (A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3] + 32 - 4) >> 6;
        dst[3] = (A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4] + 32 - 4) >> 6;
        dst[4] = (A*src[4] + B*src[5] + C*src[stride+4] + D*src[stride+5] + 32 - 4) >> 6;
        dst[5] = (A*src[5] + B*src[6] + C*src[stride+5] + D*src[stride+6] + 32 - 4) >> 6;
        dst[6] = (A*src[6] + B*src[7] + C*src[stride+6] + D*src[stride+7] + 32 - 4) >> 6;
        dst[7] = (A*src[7] + B*src[8] + C*src[stride+7] + D*src[stride+8] + 32 - 4) >> 6;
        dst+= stride;
        src+= stride;
    }
}
#endif

#define op_scale1(x)  block[x] = av_clip_uint8( (block[x]*weight + offset) >> log2_denom )
#define op_scale2(x)  dst[x] = av_clip_uint8( (src[x]*weights + dst[x]*weightd + offset) >> (log2_denom+1))
#define H264_WEIGHT(W,H) \
static void weight_h264_pixels ## W ## x ## H ## _c(uint8_t *block, int stride, int log2_denom, int weight, int offset){ \
    int y; \
    offset <<= log2_denom; \
    if(log2_denom) offset += 1<<(log2_denom-1); \
    for(y=0; y<H; y++, block += stride){ \
        op_scale1(0); \
        op_scale1(1); \
        if(W==2) continue; \
        op_scale1(2); \
        op_scale1(3); \
        if(W==4) continue; \
        op_scale1(4); \
        op_scale1(5); \
        op_scale1(6); \
        op_scale1(7); \
        if(W==8) continue; \
        op_scale1(8); \
        op_scale1(9); \
        op_scale1(10); \
        op_scale1(11); \
        op_scale1(12); \
        op_scale1(13); \
        op_scale1(14); \
        op_scale1(15); \
    } \
} \
static void biweight_h264_pixels ## W ## x ## H ## _c(uint8_t *dst, uint8_t *src, int stride, int log2_denom, int weightd, int weights, int offset){ \
    int y; \
    offset = ((offset + 1) | 1) << log2_denom; \
    for(y=0; y<H; y++, dst += stride, src += stride){ \
        op_scale2(0); \
        op_scale2(1); \
        if(W==2) continue; \
        op_scale2(2); \
        op_scale2(3); \
        if(W==4) continue; \
        op_scale2(4); \
        op_scale2(5); \
        op_scale2(6); \
        op_scale2(7); \
        if(W==8) continue; \
        op_scale2(8); \
        op_scale2(9); \
        op_scale2(10); \
        op_scale2(11); \
        op_scale2(12); \
        op_scale2(13); \
        op_scale2(14); \
        op_scale2(15); \
    } \
}

H264_WEIGHT(16,16)
H264_WEIGHT(16,8)
H264_WEIGHT(8,16)
H264_WEIGHT(8,8)
H264_WEIGHT(8,4)
H264_WEIGHT(4,8)
H264_WEIGHT(4,4)
H264_WEIGHT(4,2)
H264_WEIGHT(2,4)
H264_WEIGHT(2,2)

#undef op_scale1
#undef op_scale2
#undef H264_WEIGHT

void h264dsputil_init(DSPContext* c, AVCodecContext *avctx)
{
    int i;

    if (ENABLE_H264_DECODER) {
        c->h264_idct_add= ff_h264_idct_add_c;
        c->h264_idct8_add= ff_h264_idct8_add_c;
        c->h264_idct_dc_add= ff_h264_idct_dc_add_c;
        c->h264_idct8_dc_add= ff_h264_idct8_dc_add_c;
    }

#define dspfunc(PFX, IDX, NUM) \
    c->PFX ## _pixels_tab[IDX][ 0] = PFX ## NUM ## _mc00_c; \
    c->PFX ## _pixels_tab[IDX][ 1] = PFX ## NUM ## _mc10_c; \
    c->PFX ## _pixels_tab[IDX][ 2] = PFX ## NUM ## _mc20_c; \
    c->PFX ## _pixels_tab[IDX][ 3] = PFX ## NUM ## _mc30_c; \
    c->PFX ## _pixels_tab[IDX][ 4] = PFX ## NUM ## _mc01_c; \
    c->PFX ## _pixels_tab[IDX][ 5] = PFX ## NUM ## _mc11_c; \
    c->PFX ## _pixels_tab[IDX][ 6] = PFX ## NUM ## _mc21_c; \
    c->PFX ## _pixels_tab[IDX][ 7] = PFX ## NUM ## _mc31_c; \
    c->PFX ## _pixels_tab[IDX][ 8] = PFX ## NUM ## _mc02_c; \
    c->PFX ## _pixels_tab[IDX][ 9] = PFX ## NUM ## _mc12_c; \
    c->PFX ## _pixels_tab[IDX][10] = PFX ## NUM ## _mc22_c; \
    c->PFX ## _pixels_tab[IDX][11] = PFX ## NUM ## _mc32_c; \
    c->PFX ## _pixels_tab[IDX][12] = PFX ## NUM ## _mc03_c; \
    c->PFX ## _pixels_tab[IDX][13] = PFX ## NUM ## _mc13_c; \
    c->PFX ## _pixels_tab[IDX][14] = PFX ## NUM ## _mc23_c; \
    c->PFX ## _pixels_tab[IDX][15] = PFX ## NUM ## _mc33_c

    dspfunc(put_h264_qpel, 0, 16);
    dspfunc(put_h264_qpel, 1, 8);
    dspfunc(put_h264_qpel, 2, 4);
    dspfunc(put_h264_qpel, 3, 2);
    dspfunc(avg_h264_qpel, 0, 16);
    dspfunc(avg_h264_qpel, 1, 8);
    dspfunc(avg_h264_qpel, 2, 4);

#undef dspfunc
#ifdef JZ4740_MXU_OPT
    c->put_h264_chroma_pixels_tab[0]= put_h264_chroma_mc8_mxu;
    c->put_h264_chroma_pixels_tab[1]= put_h264_chroma_mc4_mxu;
    c->put_h264_chroma_pixels_tab[2]= put_h264_chroma_mc2_c;
    c->avg_h264_chroma_pixels_tab[0]= avg_h264_chroma_mc8_c;
    c->avg_h264_chroma_pixels_tab[1]= avg_h264_chroma_mc4_c;
    c->avg_h264_chroma_pixels_tab[2]= avg_h264_chroma_mc2_c;
    c->put_no_rnd_h264_chroma_pixels_tab[0]= put_no_rnd_h264_chroma_mc8_mxu;
#else
    c->put_h264_chroma_pixels_tab[0]= put_h264_chroma_mc8_c;
    c->put_h264_chroma_pixels_tab[1]= put_h264_chroma_mc4_c;
    c->put_h264_chroma_pixels_tab[2]= put_h264_chroma_mc2_c;
    c->avg_h264_chroma_pixels_tab[0]= avg_h264_chroma_mc8_c;
    c->avg_h264_chroma_pixels_tab[1]= avg_h264_chroma_mc4_c;
    c->avg_h264_chroma_pixels_tab[2]= avg_h264_chroma_mc2_c;
    c->put_no_rnd_h264_chroma_pixels_tab[0]= put_no_rnd_h264_chroma_mc8_c;
#endif

    c->weight_h264_pixels_tab[0]= weight_h264_pixels16x16_c;
    c->weight_h264_pixels_tab[1]= weight_h264_pixels16x8_c;
    c->weight_h264_pixels_tab[2]= weight_h264_pixels8x16_c;
    c->weight_h264_pixels_tab[3]= weight_h264_pixels8x8_c;
    c->weight_h264_pixels_tab[4]= weight_h264_pixels8x4_c;
    c->weight_h264_pixels_tab[5]= weight_h264_pixels4x8_c;
    c->weight_h264_pixels_tab[6]= weight_h264_pixels4x4_c;
    c->weight_h264_pixels_tab[7]= weight_h264_pixels4x2_c;
    c->weight_h264_pixels_tab[8]= weight_h264_pixels2x4_c;
    c->weight_h264_pixels_tab[9]= weight_h264_pixels2x2_c;
    c->biweight_h264_pixels_tab[0]= biweight_h264_pixels16x16_c;
    c->biweight_h264_pixels_tab[1]= biweight_h264_pixels16x8_c;
    c->biweight_h264_pixels_tab[2]= biweight_h264_pixels8x16_c;
    c->biweight_h264_pixels_tab[3]= biweight_h264_pixels8x8_c;
    c->biweight_h264_pixels_tab[4]= biweight_h264_pixels8x4_c;
    c->biweight_h264_pixels_tab[5]= biweight_h264_pixels4x8_c;
    c->biweight_h264_pixels_tab[6]= biweight_h264_pixels4x4_c;
    c->biweight_h264_pixels_tab[7]= biweight_h264_pixels4x2_c;
    c->biweight_h264_pixels_tab[8]= biweight_h264_pixels2x4_c;
    c->biweight_h264_pixels_tab[9]= biweight_h264_pixels2x2_c;

#ifdef JZ4740_MXU_OPT
    c->h264_v_loop_filter_luma= h264_v_loop_filter_luma_mxu;
    c->h264_h_loop_filter_luma= h264_h_loop_filter_luma_mxu;
    c->h264_v_loop_filter_chroma= h264_v_loop_filter_chroma_mxu;
    c->h264_h_loop_filter_chroma= h264_h_loop_filter_chroma_mxu;
    c->h264_v_loop_filter_chroma_intra= h264_v_loop_filter_chroma_intra_mxu;
    c->h264_h_loop_filter_chroma_intra= h264_h_loop_filter_chroma_intra_mxu;
#else
    c->h264_v_loop_filter_luma= h264_v_loop_filter_luma_c;
    c->h264_h_loop_filter_luma= h264_h_loop_filter_luma_c;
    c->h264_v_loop_filter_chroma= h264_v_loop_filter_chroma_c;
    c->h264_h_loop_filter_chroma= h264_h_loop_filter_chroma_c;
    c->h264_v_loop_filter_chroma_intra= h264_v_loop_filter_chroma_intra_c;
    c->h264_h_loop_filter_chroma_intra= h264_h_loop_filter_chroma_intra_c;
#endif
    c->h264_loop_filter_strength= NULL;

}

