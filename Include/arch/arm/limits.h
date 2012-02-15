//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    处理器相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef	_I386_LIMITS_H
#define	_I386_LIMITS_H

#define	CHAR_BIT	8		/* number of bits in a char */
#define	MB_LEN_MAX	32		/* no multibyte characters */


/*
 * According to ANSI (section 2.2.4.2), the values below must be usable by
 * #if preprocessing directives.  Additionally, the expression must have the
 * same type as would an expression that is an object of the corresponding
 * type converted according to the integral promotions.  The subtraction for
 * INT_MIN and LONG_MIN is so the value is not unsigned; 2147483648 is an
 * unsigned int for 32-bit two's complement ANSI compilers (section 3.1.3.2).
 * These numbers work for pcc as well.  The UINT_MAX and ULONG_MAX values
 * are written as hex so that GCC will be quiet about large integer constants.
 */
#define	SCHAR_MAX	127		/* min value for a signed char */
#define	SCHAR_MIN	(-128)		/* max value for a signed char */

#define	UCHAR_MAX	255		/* max value for an unsigned char */
#define	CHAR_MAX	127		/* max value for a char */
#define	CHAR_MIN	(-128)		/* min value for a char */

#define	USHRT_MAX	65535		/* max value for an unsigned short */
#define	SHRT_MAX	32767		/* max value for a short */
#define	SHRT_MIN	(-32768)	/* min value for a short */

#define	UINT_MAX	0xffffffff	/* max value for an unsigned int */
#define	INT_MAX		2147483647	/* max value for an int */
#define	INT_MIN		(-2147483647-1)	/* min value for an int */

#define	ULONG_MAX	0xffffffffL	/* max value for an unsigned long */
#define	LONG_MAX	2147483647L	/* max value for a long */
					/* min value for a long */
#define	LONG_MIN	(-2147483647L-1L)

#ifndef __LONG_LONG_MAX__
#define __LONG_LONG_MAX__ 9223372036854775807LL
#endif
#undef LONG_LONG_MIN
#define LONG_LONG_MIN (-LONG_LONG_MAX-1)
#undef LONG_LONG_MAX
#define LONG_LONG_MAX __LONG_LONG_MAX__
#undef ULONG_LONG_MAX
#define ULONG_LONG_MAX (LONG_LONG_MAX * 2ULL + 1)


#if !defined(_ANSI_SOURCE)
#define	SSIZE_MAX	INT_MAX		/* max value for a ssize_t */

#if !defined(_POSIX_SOURCE)
#define	SIZE_T_MAX	UINT_MAX	/* max value for a size_t */

#endif /* !_POSIX_SOURCE */
#endif /* !_ANSI_SOURCE */

#endif	/* _I386_LIMITS_H */
