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


#ifndef _WIN32_STDARG_H
#define	_WIN32_STDARG_H

#include <arch/ansi.h>

typedef _BSD_VA_LIST_	va_list;

#ifdef __lint__
#define __builtin_next_arg(t)		((t) ? 0 : 0)
#define	__builtin_stdarg_start(a, l)	((a) = ((l) ? 0 : 0))
#define	__builtin_va_arg(a, t)		((a) ? 0 : 0)
#define	__builtin_va_end		/* nothing */
#define	__builtin_va_copy(d, s)		((d) = (s))
#endif

#ifdef __GNUC__
#define	va_start(ap, last)	__builtin_stdarg_start((ap), (last))
#define	va_arg			__builtin_va_arg
#define	va_end			__builtin_va_end
#define	va_copy(dest, src)	__builtin_va_copy((dest), (src))
#elif defined(WIN32)
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )
#else
#define	__va_size(type) \
	(((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))
#define	va_start(ap, last) \
	((ap) = (va_list)__builtin_next_arg(last))
#define	va_arg(ap, type) \
	(*(type *)(void *)((ap) += __va_size(type), (ap) - __va_size(type)))
#define	va_end(ap)	
#define	va_copy(dest, src)	((dest) = (src))
#endif

#endif /* !_WIN32_STDARG_H */
