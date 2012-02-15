    /********************************************************************/
    /*                                                                  */
    /*                                                                  */
    /*              Copyright (C), 2000  SEIKO EPSON Corp.              */
    /*              ALL RIGHTS RESERVED                                 */
    /*                                                                  */
    /*                                                                  */
    /*   file name : smcvals.c                                          */
    /*                                                                  */
    /*                                                                  */
    /*   smcvals.c sets the some special macros and definition          */
    /*                                                                  */
    /*   Revision history                                               */
    /*       version 0          1992/08/31        K.Shinozuka           */
    /*       version 1          1992/11/10        K.Shinozuka           */
    /*                          1997/3/27         M.Kudo                */
    /*       version 2          2000/01/31        M.Igarashi            */
    /*                                                                  */
    /********************************************************************/

#ifndef SMCVALS_H
#define SMCVALS_H

/*  the architecture of double floating point
 *
 *   0 1          11                  31 32                               63 bit
 *   -----------------------------------------------------------------------
 *  | |   exponent  |                    fraction                           |
 *   -----------------------------------------------------------------------
 *
 *  |               |                   |                                   |
 *  |    12 bits          20 bits       |             32 bits               |
 *  |            lower word             |            higher word            |
 *
 *         bit    0         sign bit         (  1 bit  )
 *              1 - 11      exponent part    ( 11 bits )
 *             12 - 63      fraction part    ( 52 bits )
 *
 */
 

/******* definition for  math library *************************************/
#define HUGE_V  (3.402823E+38F)
#define B_FRAC      52
#define MIN_E       1
#define MAX_E       0x7fe
#define INF_E       0x7ff
#define NAN_E       0x7ff
#define B_DBL       1023
#define NEG_ZERO_L   0x80000000 /* lower bits of double -0.0 */
#define DBL_MIN_L    0x00100000 /* lower bits of the smallest, positive, normalized number */
#define NUM1_L       0x3ff00000 /* lower bits of double 1.0 */
#define MASK_EXP     0x7ff00000 /* the mask to get exponent part */
#define MASK_FRAC    0x000fffff /* the mask to get fraction part */
#define N_FRAC       52         /* the number of fraction bits */
#define L_FRAC       20         /* the number of fraction bits in lower word */
#define ONE_WORD     32         /* the number of bits in a word */ 

#define _UGETY      'Y'         /* unget character exist */
#define _UGETN      'E'         /* unget character does not exist */

#define _BOUNDARY   4           /* boundary size */
#define _STRM       'S'         /* input from stream */
#define _STR        'M'         /* input from memory */

/******* useful math constant *************************************/

#define LS      -6.93147180559945309417232121e-1
#define LD       2.30258509299404568402e0
#define INVP      1.27323954473516268e0
#define LSS     6.93147180559945309417e-1
#define INVLS   1.4426950408889634073599247e0
#define RTS     1.4142135623730950488016887e0
#define WSQ1    2.414213562373095048802e0
#define WSQ2    0.414213562373095048802e0
#define HLFP        1.57079632679489661923e0
#define QUAP        0.785398163397448309615e0
#define WHP         3.14159265358979323846e0

/****** macros for math library ******************************************/
#define GET_HI_FRAC( _LH )  ( (_LH) & ( ( 1L << (B_FRAC - 32) ) - 1 ) )
#define GET_LO_FRAC( _LL )  (_LL)
#define GET_EXP( _LH )  ( ( (_LH) >> (B_FRAC - 32) ) & 0x7ff )
#define MASK_EXPO( _LH )    ( (_LH) & 0x800fffff )
#define EXP_PUT( _LH, E )   ( (_LH) | ((unsigned long)E << (B_FRAC - 32) ) )
#define COPY_SIGN( S, _LH ) ( ((unsigned long)(S) & 0x7fffffff) |((unsigned long)(_LH) & 0x80000000) )

#ifndef DBL_LNG_TYPE
#define DBL_LNG_TYPE
typedef union {
    struct {
#ifdef BIG_ENDIAN
        unsigned long _LL;
        unsigned long _LH;
#else
        unsigned long _LH;
        unsigned long _LL;
#endif
    } st;
    double _D;
} DBL_LNG;
#endif

#define GETW_H(ix,x)                    \
do {                                \
  DBL_LNG dl;                   \
  dl._D = (x);                      \
  (ix) = dl.st._LH;                     \
} while (0)

#define GETW_L(ix,x)                    \
do {                                \
  DBL_LNG dl;                   \
  dl._D = (x);                      \
  (ix) = dl.st._LL;                     \
} while (0)

#define CLR_SIGN( x2, x )                   \
do {                                \
  DBL_LNG dl;                   \
  dl._D = (x);                      \
  dl.st._LL &= 0x7fffffff;          \
  (x2) = dl._D;                     \
} while (0)

#define SET_SIGN( x2, x )                   \
do {                                \
  DBL_LNG dl;                   \
  dl._D = (x);                      \
  dl.st._LL |= 0x80000000;          \
  (x2) = dl._D;                     \
} while (0)

/* check of domain error */ 
#define CHECK_ARG( VAL )   /* check higer word */ \
                           if ( (( (*VAL).st._LL | NEG_ZERO_L ) == N_INF.st._LL ) \
                                 && ( (*VAL).st._LH > N_INF.st._LH ) ) { \
                              errno = EDOM; \
                              return( NAN._D ); \
                           /* check lower word */ \
                           } else if ( ( (*VAL).st._LL | NEG_ZERO_L ) > N_INF.st._LL ) { \
                              errno = EDOM; \
                              return( NAN._D ); \
                           }




/* calculate pointer and size consider boundary size */
#define _BOUND(siz) ((siz + _BOUNDARY - 1) & (~(_BOUNDARY - 1)))

/* check stdin stdout or stderr stream */
#define _ISSTDIO( STRM )    ((STRM == stdin) || (STRM == stdout) || (STRM == stderr))

typedef int _CODETYP;

extern DBL_LNG NAN;             /* special code for not a number ( NaN ) */
extern DBL_LNG P_INF;           /* special code for +overflow ( +Inf) */
extern DBL_LNG N_INF;           /* special code for -overflow ( -Inf) */

extern double HUGE_RAD;

#define HUGE_VAL P_INF._D


#endif
