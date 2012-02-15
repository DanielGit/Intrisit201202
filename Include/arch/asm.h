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

#ifndef _INC_ASM
#define _INC_ASM

#if defined(__arm__)
#include "./arm/asm.h"
#elif defined (__i386__)
#include "./i386/asm.h"
#elif defined (__ppc__)
#include "./ppc/asm.h"
#elif defined (__mips__)
#include "./mips/asm.h"
#elif defined (__sh4__)
#include "./sh4/asm.h"
#elif defined (WIN32)
#include "./win32/asm.h"
#elif defined (__c33pe)
#include "./C33L27/asm.h"
#else
#error architecture not supported
#endif

#endif // _INC_ASM
