/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2004 M. Bakker, Ahead Software AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Ahead Software through Mpeg4AAClicense@nero.com.
**
** $Id: fixed.h,v 1.3 2008/12/27 08:47:55 xyzhang Exp $
**/

#ifndef __FIXED_H__
#define __FIXED_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32_WCE) && defined(_ARM_)
#include <cmnintrin.h>
#endif

#if defined(JZ4750_OPT)
#undef JZ4740_MXU_OPT
#include "jz_mxu.h"
#endif

#define COEF_BITS 28
#define COEF_PRECISION (1 << COEF_BITS)
#define REAL_BITS 14 // MAXIMUM OF 14 FOR FIXED POINT SBR
#define REAL_PRECISION (1 << REAL_BITS)

/* FRAC is the fractional only part of the fixed point number [0.0..1.0) */
#define FRAC_SIZE 32 /* frac is a 32 bit integer */
#define FRAC_BITS 31
#define FRAC_PRECISION ((uint32_t)(1 << FRAC_BITS))
#define FRAC_MAX 0x7FFFFFFF

typedef int32_t real_t;


#define REAL_CONST(A) (((A) >= 0) ? ((real_t)((A)*(REAL_PRECISION)+0.5)) : ((real_t)((A)*(REAL_PRECISION)-0.5)))
#define COEF_CONST(A) (((A) >= 0) ? ((real_t)((A)*(COEF_PRECISION)+0.5)) : ((real_t)((A)*(COEF_PRECISION)-0.5)))
#define FRAC_CONST(A) (((A) == 1.00) ? ((real_t)FRAC_MAX) : (((A) >= 0) ? ((real_t)((A)*(FRAC_PRECISION)+0.5)) : ((real_t)((A)*(FRAC_PRECISION)-0.5))))
//#define FRAC_CONST(A) (((A) >= 0) ? ((real_t)((A)*(FRAC_PRECISION)+0.5)) : ((real_t)((A)*(FRAC_PRECISION)-0.5)))

#define Q2_BITS 22
#define Q2_PRECISION (1 << Q2_BITS)
#define Q2_CONST(A) (((A) >= 0) ? ((real_t)((A)*(Q2_PRECISION)+0.5)) : ((real_t)((A)*(Q2_PRECISION)-0.5)))

#if defined(_WIN32) && !defined(_WIN32_WCE)

/* multiply with real shift */
static INLINE real_t MUL_R(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        shrd eax,edx,REAL_BITS
    }
}

/* multiply with coef shift */
static INLINE real_t MUL_C(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        shrd eax,edx,COEF_BITS
    }
}

static INLINE real_t MUL_Q2(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        shrd eax,edx,Q2_BITS
    }
}

static INLINE real_t MUL_SHIFT6(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        shrd eax,edx,6
    }
}

static INLINE real_t MUL_SHIFT23(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        shrd eax,edx,23
    }
}

#if 1
static INLINE real_t _MulHigh(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        mov eax,edx
    }
}

/* multiply with fractional shift */
static INLINE real_t MUL_F(real_t A, real_t B)
{
    return _MulHigh(A,B) << (FRAC_SIZE-FRAC_BITS);
}

/* Complex multiplication */
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    *y1 = (_MulHigh(x1, c1) + _MulHigh(x2, c2))<<(FRAC_SIZE-FRAC_BITS);
    *y2 = (_MulHigh(x2, c1) - _MulHigh(x1, c2))<<(FRAC_SIZE-FRAC_BITS);
}
#else
static INLINE real_t MUL_F(real_t A, real_t B)
{
    _asm {
        mov eax,A
        imul B
        shrd eax,edx,FRAC_BITS
    }
}

/* Complex multiplication */
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    *y1 = MUL_F(x1, c1) + MUL_F(x2, c2);
    *y2 = MUL_F(x2, c1) - MUL_F(x1, c2);
}
#endif

#elif defined(__GNUC__) && defined (__arm__)

/* taken from MAD */
#define arm_mul(x, y, SCALEBITS) \
({ \
    uint32_t __hi; \
    uint32_t __lo; \
    uint32_t __result; \
    asm("smull  %0, %1, %3, %4\n\t" \
        "movs   %0, %0, lsr %5\n\t" \
        "adc    %2, %0, %1, lsl %6" \
        : "=&r" (__lo), "=&r" (__hi), "=r" (__result) \
        : "%r" (x), "r" (y), \
        "M" (SCALEBITS), "M" (32 - (SCALEBITS)) \
        : "cc"); \
        __result; \
})

static INLINE real_t MUL_R(real_t A, real_t B)
{
    return arm_mul(A, B, REAL_BITS);
}

static INLINE real_t MUL_C(real_t A, real_t B)
{
    return arm_mul(A, B, COEF_BITS);
}

static INLINE real_t MUL_Q2(real_t A, real_t B)
{
    return arm_mul(A, B, Q2_BITS);
}

static INLINE real_t MUL_SHIFT6(real_t A, real_t B)
{
    return arm_mul(A, B, 6);
}

static INLINE real_t MUL_SHIFT23(real_t A, real_t B)
{
    return arm_mul(A, B, 23);
}

static INLINE real_t _MulHigh(real_t x, real_t y)
{
    uint32_t __lo;
    uint32_t __hi;
    asm("smull\t%0, %1, %2, %3"
        : "=&r"(__lo),"=&r"(__hi)
        : "%r"(x),"r"(y)
        : "cc");
    return __hi;
}

static INLINE real_t MUL_F(real_t A, real_t B)
{
    return _MulHigh(A, B) << (FRAC_SIZE-FRAC_BITS);
}

/* Complex multiplication */
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    int32_t tmp, yt1, yt2;
    asm("smull %0, %1, %4, %6\n\t"
        "smlal %0, %1, %5, %7\n\t"
        "rsb   %3, %4, #0\n\t"
        "smull %0, %2, %5, %6\n\t"
        "smlal %0, %2, %3, %7"
        : "=&r" (tmp), "=&r" (yt1), "=&r" (yt2), "=r" (x1)
        : "3" (x1), "r" (x2), "r" (c1), "r" (c2)
        : "cc" );
    *y1 = yt1 << (FRAC_SIZE-FRAC_BITS);
    *y2 = yt2 << (FRAC_SIZE-FRAC_BITS);
}
#elif defined(JZ4740_MXU_OPT)

#define INSN_CMPMULPLUS(y1, im_x, re_x, re_y, im_y)      \
__extension__({                                  \
  __asm__ __volatile ("mult %1, %2;              \
                       madd %3, %4;              \
                       mfhi $8;              \
                       sll  $8, $8, 1;              \
                       sw   $8, 0(%0)"            \
                      :                         \
              :"d"(y1), "d"(re_x), "d"(im_y), "d"(im_x), "d"(re_y)\
              :"hi", "lo", "$8");                      \
})
 
#define INSN_CMPMULMINUS(y2, im_x, re_x, re_y, im_y)      \
__extension__({                                  \
  __asm__ __volatile ("mult %1, %2;              \
                       msub %3, %4;              \
                       mfhi $8;              \
                       sll  $8, $8, 1;              \
                       sw   $8, 0(%0)"            \
                      :                         \
              :"d"(y2), "d"(re_x), "d"(re_y), "d"(im_x), "d"(im_y)\
              :"hi", "lo", "$8");                      \
})

#define MUL_F(A,B)          \
__extension__({                                    \
  int hi, lo;                                      \
  __asm__ __volatile("mult %2, %3"                 \
              :"=h"(hi), "=l"(lo)                  \
              :"d"(A), "d"(B));                    \
  hi<<1;                                           \
})
  /* multiply with real shift */
  #define MUL_R(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (REAL_BITS-1))) >> REAL_BITS)
  /* multiply with coef shift */
  #define MUL_C(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (COEF_BITS-1))) >> COEF_BITS)
  /* multiply with fractional shift */
  #define _MulHigh(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (FRAC_SIZE-1))) >> FRAC_SIZE)
//  #define MUL_F(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (FRAC_BITS-1))) >> FRAC_BITS)
  #define MUL_Q2(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (Q2_BITS-1))) >> Q2_BITS)
  #define MUL_SHIFT6(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (6-1))) >> 6)
  #define MUL_SHIFT23(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (23-1))) >> 23)

/* Complex multiplication */
#if 1
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    INSN_CMPMULPLUS(y1, x1, x2, c1, c2); 
    INSN_CMPMULMINUS(y2, x1, x2, c1, c2); 
}
#else 

static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{   __asm__ __volatile ("mult %2, %3\n\t"                    
                        "madd %4, %5\n\t"                    
                        "mfhi $8\n\t"                        
                        "sll  $8, $8, 1\n\t"                
                        "sw   $8, 0(%0)\n\t"                 
                        "mult %2, %5\n\t"                    
                        "msub %3, %4\n\t"                   
                        "mfhi $8\n\t"                        
                        "sll  $8, $8, 1\n\t"                 
                        "sw   $8, 0(%1)\n\t"                 
                        : "=d"(y1), "=d"(y2)                              
             : "d"(x1), "d"(c2), "d"(x2), "d"(c1)
             :"hi", "lo", "$8");                        
}
#endif

#elif defined(JZ4750_OPT)

#define INSN_CMPMULPLUS(y1, im_x, re_x, re_y, im_y)      \
__extension__({                                  \
  __asm__ __volatile ("mult %1, %2;              \
                       madd %3, %4;              \
                       mfhi $8;              \
                       sll  $8, $8, 1;              \
                       sw   $8, 0(%0)"            \
                      :                         \
              :"d"(y1), "d"(re_x), "d"(im_y), "d"(im_x), "d"(re_y)\
              :"hi", "lo", "$8");                      \
})
 
#define INSN_CMPMULMINUS(y2, im_x, re_x, re_y, im_y)      \
__extension__({                                  \
  __asm__ __volatile ("mult %1, %2;              \
                       msub %3, %4;              \
                       mfhi $8;              \
                       sll  $8, $8, 1;              \
                       sw   $8, 0(%0)"            \
                      :                         \
              :"d"(y2), "d"(re_x), "d"(re_y), "d"(im_x), "d"(im_y)\
              :"hi", "lo", "$8");                      \
})
#if 1
#define MUL_F(A,B)          \
  ({S32MUL(xr1, xr2, A, B);   \
  D32SLL(xr1, xr1, xr0, xr0, 1); \
  S32M2I(xr1);})
#define MUL_R(A,B)    \
  ({S32MUL(xr1, xr2, A, B);    \
    D32SLL(xr1, xr1, xr0,xr0,15);\
    D32SLL(xr1, xr1, xr0,xr0,3);\
    D32SLR(xr2, xr2, xr0,xr0,14);\
    S32OR(xr1, xr1,xr2);        \
    S32M2I(xr1);})
#define MUL_C(A,B)    \
  ({S32MUL(xr1, xr2, A, B);    \
    D32SLL(xr1, xr1, xr0,xr0,4);\
    D32SLR(xr2, xr2, xr0,xr0,15);\
    D32SLR(xr2, xr2, xr0,xr0,13);\
    S32OR(xr1, xr1,xr2);        \
    S32M2I(xr1);})
#define _MulHigh(A,B) MUL_F(A,B)
#define MUL_SHIFT6(A,B)    \
  ({S32MUL(xr1, xr2, A, B);    \
    D32SLL(xr1, xr1, xr0,xr0,6);\
    D32SLR(xr2, xr2, xr0,xr0,13);\
    D32SLR(xr2, xr2, xr0,xr0,13);\
    S32OR(xr1, xr1,xr2);        \
    S32M2I(xr1);})
#define MUL_SHIFT23(A,B)    \
  ({S32MUL(xr1, xr2, A, B);    \
    D32SLL(xr1, xr1, xr0,xr0,3);\
    D32SLL(xr2, xr2, xr0,xr0,10);\
    D32SLL(xr2, xr2, xr0,xr0,13);\
    S32OR(xr1, xr1,xr2);        \
    S32M2I(xr1);})
#else
  #define MUL_F(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (FRAC_BITS-1))) >> FRAC_BITS)
  /* multiply with real shift */
  #define MUL_R(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (REAL_BITS-1))) >> REAL_BITS)
  /* multiply with coef shift */
  #define MUL_C(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (COEF_BITS-1))) >> COEF_BITS)
  #define _MulHigh(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (FRAC_SIZE-1))) >> FRAC_SIZE)
  #define MUL_SHIFT6(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (6-1))) >> 6)
  /* multiply with fractional shift */
  #define MUL_SHIFT23(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (23-1))) >> 23)
#endif

#define CMUL(imagv, realv, x1, x2, w1, w2) \
 do{\
                S32MUL(xr10,xr11, x1, w1); \
                S32MADD(xr10,xr11, x2, w2);\
                S32MUL(xr12,xr13, x2, w1);\
                S32MSUB(xr12,xr13, x1, w2);\
                D32SLL(xr10,xr10,xr12,xr12,1);\
                imagv = S32M2I(xr10);\
                realv = S32M2I(xr12);\
   }while(0)


/* Complex multiplication */
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    INSN_CMPMULPLUS(y1, x1, x2, c1, c2); 
    INSN_CMPMULMINUS(y2, x1, x2, c1, c2); 
}
#else

  /* multiply with real shift */
  #define MUL_R(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (REAL_BITS-1))) >> REAL_BITS)
  /* multiply with coef shift */
  #define MUL_C(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (COEF_BITS-1))) >> COEF_BITS)
  /* multiply with fractional shift */
#if defined(_WIN32_WCE) && defined(_ARM_)
  /* eVC for PocketPC has an intrinsic function that returns only the high 32 bits of a 32x32 bit multiply */
  static INLINE real_t MUL_F(real_t A, real_t B)
  {
      return _MulHigh(A,B) << (32-FRAC_BITS);
  }
#else
  #define _MulHigh(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (FRAC_SIZE-1))) >> FRAC_SIZE)
  #define MUL_F(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (FRAC_BITS-1))) >> FRAC_BITS)
#endif
  #define MUL_Q2(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (Q2_BITS-1))) >> Q2_BITS)
  #define MUL_SHIFT6(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (6-1))) >> 6)
  #define MUL_SHIFT23(A,B) (real_t)(((int64_t)(A)*(int64_t)(B)+(1 << (23-1))) >> 23)

/* Complex multiplication */
static INLINE void ComplexMult(real_t *y1, real_t *y2,
    real_t x1, real_t x2, real_t c1, real_t c2)
{
    *y1 = (_MulHigh(x1, c1) + _MulHigh(x2, c2))<<(FRAC_SIZE-FRAC_BITS);
    *y2 = (_MulHigh(x2, c1) - _MulHigh(x1, c2))<<(FRAC_SIZE-FRAC_BITS);
}

#endif
#ifdef __cplusplus
}
#endif
#endif
