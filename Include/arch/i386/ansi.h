//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    ���������ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef	_I386_ANSI_H
#define	_I386_ANSI_H

#include <sys/cdefs.h>

/*
 * Types which are fundamental to the implementation and may appear in
 * more than one standard header are defined here.  Standard headers
 * then use:
 *	#ifdef	_BSD_SIZE_T_
 *	typedef	_BSD_SIZE_T_ size_t;
 *	#undef	_BSD_SIZE_T_
 *	#endif
 */
#define	_BSD_CLOCK_T_		unsigned long	/* clock() */
#define	_BSD_PTRDIFF_T_		int		/* ptr1 - ptr2 */
#define	_BSD_SIZE_T_		unsigned int	/* sizeof() */
#define	_BSD_SSIZE_T_		int		/* byte count or error */
#define	_BSD_TIME_T_		long		/* time() */
#ifdef __GNUC__
#define	_BSD_VA_LIST_		__builtin_va_list /* GCC built-in type */
#else
#define	_BSD_VA_LIST_		char *		/* va_list */
#endif
/*
 * Runes (wchar_t) is declared to be an ``int'' instead of the more natural
 * ``unsigned long'' or ``long''.  Two things are happening here.  It is not
 * unsigned so that EOF (-1) can be naturally assigned to it and used.  Also,
 * it looks like 10646 will be a 31 bit standard.  This means that if your
 * ints cannot hold 32 bits, you will be in trouble.  The reason an int was
 * chosen over a long is that the is*() and to*() routines take ints (says
 * ANSI C), but they use _RUNE_T_ instead of int.  By changing it here, you
 * lose a bit of ANSI conformance, but your programs will still work.
 *
 * Note that _WCHAR_T_ and _RUNE_T_ must be of the same type.  When wchar_t
 * and rune_t are typedef'd, _WCHAR_T_ will be undef'd, but _RUNE_T remains
 * defined for ctype.h.
 */
#define	_BSD_WCHAR_T_		int		/* wchar_t */
#define	_BSD_RUNE_T_		int		/* rune_t */

#endif	/* _I386_ANSI_H */
