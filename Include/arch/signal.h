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

#ifndef _INC_SIGNAL
#define _INC_SIGNAL

#if defined(__arm__)
#include "./arm/signal.h"
#elif defined (__i386__)
#include "./i386/signal.h"
#elif defined (__ppc__)
#include "./ppc/signal.h"
#elif defined (__mips__)
#include "./mips/signal.h"
#elif defined (__sh4__)
#include "./sh4/signal.h"
#elif defined (WIN32)
#include "./win32/signal.h"
#elif defined(__c33pe)
#include "./C33L27/signal.h"
#else
#error architecture not supported
#endif

#endif // _INC_SIGNAL
