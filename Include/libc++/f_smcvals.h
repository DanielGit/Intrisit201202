	/********************************************************************/
	/*                                                                  */
	/*                                                                  */
	/*              Copyright (C), 2003  SEIKO EPSON Corp.              */
	/*              ALL RIGHTS RESERVED                                 */
	/*                                                                  */
	/*                                                                  */
	/*   file name : f_smcvals.h                                        */
	/*     copied from smcvals.h of math.lib                            */
	/*                                                                  */
	/*                                                                  */
	/*   f_smcvals.h sets the some special macros and definition        */
	/*                                                                  */
	/*   Revision history                                               */
	/*       version 0          1992/08/31        K.Shinozuka           */
	/*       version 1          1992/11/10        K.Shinozuka           */
	/*                          1997/3/27         M.Kudo                */
	/*       version 2          2000/01/31        M.Igarashi            */
	/*                          2003/04/10        Iruma M.Takeishi      */
	/*                          changed for float-based library         */
	/*                                                                  */
	/********************************************************************/

#ifndef F_SMCVALS_H
#define F_SMCVALS_H

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
 

/******* definition for  f_math library *************************************/
#define	F_HUGE_V	(3.402823E+38F)
#define	F_B_FRAC		23
#define	F_MIN_E 		1
#define	F_MAX_E		0xfe
#define	F_INF_E 		0x7ff
#define	F_NAN_E 		0x7ff
#define	F_B_FLT		127
#define F_NEG_ZERO_L   0x80000000 /* lower bits of float -0.0 */
#define F_FLT_MIN_L    0x00100000 /* lower bits of the smallest, positive, normalized number */
#define F_NUM1_L       0x3f800000 /* lower bits of float 1.0 */
#define F_MASK_EXP     0x7f800000 /* the mask to get exponent part */
#define F_MASK_FRAC    0x007fffff /* the mask to get fraction part */
#define F_N_FRAC       23         /* the number of fraction bits */
#define F_L_FRAC       23         /* the number of fraction bits in lower word */
#define F_ONE_WORD     32         /* the number of bits in a word */ 

#define _UGETY      'Y'         /* unget character exist */
#define _UGETN      'E'         /* unget character does not exist */

#define _BOUNDARY   4           /* boundary size */
#define _STRM       'S'         /* input from stream */
#define _STR        'M'         /* input from memory */

/******* useful math constant *************************************/

#define F_LS			-6.93147180559945309417232121e-1f
#define	F_LD			2.30258509299404568402e0f             // log( 10 )
#define	F_INVP			1.27323954473516268e0f
#define	F_LSS	 		6.93147180559945309417e-1f            // log( 2 )
#define	F_LD_REVERSE		0.434294481903251827651128918f        // 1 / log( 10 )



#define	F_INVLS			1.4426950408889634073599247e0f           //   F_INVLS = 1 / ( log2 )
//#define	F_INVLS			1.442695140839e0f           //   F_INVLS = 1 / ( log2 ) + 2^(-22),  2^(-22) is for function exp(log(2))'s value.


#define	F_RTS			1.4142135623730950488016887e0f
#define	F_WSQ1			2.414213562373095048802e0f
#define	F_WSQ2			0.414213562373095048802e0f
#define	F_HLFP			1.57079632679489661923e0f
#define	F_QUAP			0.785398163397448309615e0f
#define	F_WHP			3.14159265358979323846e0f

/****** macros for math library ******************************************/
#define	F_GET_HI_FRAC( _LH )	( (_LH) & ( ( 1L << F_B_FRAC  ) - 1 ) )
#define	F_GET_LO_FRAC( _LL )	( 0x007fffff & (_LL) )
#define	F_GET_EXP( _LL )		( ( (_LL) >> (F_B_FRAC ) ) & 0xff )
#define	F_MASK_EXPO( _LH )	( (_LH) & 0x807fffff )
#define	F_EXP_PUT( _LH, E )	( (_LH) | ((unsigned long)E << (F_B_FRAC ) ) )
#define	F_COPY_SIGN( S, _LH )	( ((unsigned long)(S) & 0x7fffffff) |((unsigned long)(_LH) & 0x80000000) )

#define F_GET_EXP_RAW( _LL )	(  (_LL) & 0x7f800000 )


#ifndef FLT_LNG_TYPE
#define FLT_LNG_TYPE
typedef union {
    struct {
        unsigned long _LL;
    } st;
    float _F;
} FLT_LNG;
#endif

#define F_GETW(ix,x)					\
do {								\
  FLT_LNG dl;					\
  dl._F = (x);						\
  (ix) = dl.st._LL;						\
} while (0)

#define	F_CLR_SIGN( x2, x )					\
do {								\
  FLT_LNG dl;					\
  dl._F = (x);						\
  dl.st._LL &= 0x7fffffff;			\
  (x2) = dl._F;						\
} while (0)

#define	F_SET_SIGN( x2, x )					\
do {								\
  FLT_LNG dl;					\
  dl._F = (x);						\
  dl.st._LL |= 0x80000000;			\
  (x2) = dl._F;						\
} while (0)

/* check of domain error */ 
#define F_CHECK_ARG( VAL )   /* check args */ \
                           if (  ( (*VAL).st._LL | F_NEG_ZERO_L ) > f_N_INF.st._LL ) { \
                              errno = EDOM; \
                              return( f_NAN._F ); \
                           }




/* calculate pointer and size consider boundary size */
#define F_BOUND(siz) ((siz + _BOUNDARY - 1) & (~(_BOUNDARY - 1)))

/* check stdin stdout or stderr stream */
#define F_ISSTDIO( STRM )    ((STRM == stdin) || (STRM == stdout) || (STRM == stderr))

typedef int f__CODETYP;

extern FLT_LNG f_NAN;             /* special code for not a number ( NaN ) */
extern FLT_LNG f_P_INF;           /* special code for +overflow ( +Inf) */
extern FLT_LNG f_N_INF;           /* special code for -overflow ( -Inf) */

extern float f_HUGE_RAD;

#define F_HUGE_VAL f_P_INF._F


#endif
