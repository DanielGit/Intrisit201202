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


#ifndef _WIN32_ASM_H
#define _WIN32_ASM_H

#define _ALIGN_TEXT .align 4
#define _ENTRY(x) \
	.text; _ALIGN_TEXT; .globl x; .type x,@function; x:

#define _C_LABEL(x)	x
#define	_ASM_LABEL(x)	x

#define	ENTRY(y)	_ENTRY(_C_LABEL(y));
#define	NENTRY(y)	_ENTRY(_C_LABEL(y))
#define	ASENTRY(y)	_ENTRY(_ASM_LABEL(y));

#if defined(PIC)
#ifdef __STDC__
#define	PIC_SYM(x,y)	x ## ( ## y ## )
#else
#define	PIC_SYM(x,y)	x/**/(/**/y/**/)
#endif
#else
#define	PIC_SYM(x,y)	x
#endif

#define	WEAK_ALIAS(alias,sym)						\
	.weak alias;							\
	alias = sym

#ifdef __STDC__
#define	WARN_REFERENCES(sym,msg)					\
	.stabs msg ## ,30,0,0,0 ;					\
	.stabs __STRING(_C_LABEL(sym)) ## ,1,0,0,0
#else
#define	WARN_REFERENCES(sym,msg)					\
	.stabs msg,30,0,0,0 ;						\
	.stabs __STRING(sym),1,0,0,0
#endif /* __STDC__ */

#endif /* !_WIN32_ASM_H */
