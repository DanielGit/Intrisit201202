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


#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <config.h>

#if defined(WIN32)
#include "./win32/platform.h"
#elif defined (__i386__)
#include "./i386/platform.h"
#elif defined (CONFIG_MCU_JZ4720)
#include "./jz4720/platform.h"
#elif defined (CONFIG_MCU_JZ4740)
#include "./jz4740/platform.h"
#elif defined (CONFIG_MCU_JZ4750)
#include "./jz4750/platform.h"
#elif defined (CONFIG_MCU_JZ4750L)
#include "./jz4750l/platform.h"
#elif defined (CONFIG_MCU_JZ4755)
#include "./jz4755/platform.h"
#elif defined (CONFIG_MCU_AK3620)
#include "./ak3620/platform.h"
#elif defined (CONFIG_MCU_AK7802)
#include "./ak7802/platform.h"
#elif defined (CONFIG_MCU_AK8802)
#include "./ak8802/platform.h"
#elif defined (CONFIG_MCU_C33L27)
#include "./c33l27/platform.h"
#elif defined (CONFIG_MCU_JZ4760B)
#include "./jz4760b/platform.h"
#else
#error platform not supported
#endif

#endif // _PLATFORM_H
