//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    内核相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _EXCEPT_H
#define _EXCEPT_H


#include <platform/platform.h>
#include <kernel/kernel.h>

// 最大异常数
#define	MAX_EXCS		32

void ExceptionInit(void);
void ExceptionMark(int);
void ExceptionDeliver(void);

#if defined(STC_EXP)
int sExceptionSetup(void (*)(int, void *));
int sExceptionReturn(void *);
int sExceptionRaise(HANDLE, int);
int sExceptionWait(int *);
#else
int ExceptionSetup(void (*)(int, void *));
int ExceptionReturn(void *);
int ExceptionRaise(HANDLE, int);
int ExceptionWait(int *);
#endif

#endif // _EXCEPT_H
