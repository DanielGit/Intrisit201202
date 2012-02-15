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


#ifndef ___SYSDEFS_H
#define ___SYSDEFS_H

#include <config.h>

#if defined (CONFIG_MCU_JZ4740)
#include "./jz4740/sysdefs.h"
#elif defined (CONFIG_MCU_JZ4750)
#include "./jz4750/sysdefs.h"
#elif defined (CONFIG_MCU_JZ4750L)
#include "./jz4750l/sysdefs.h"
#elif defined (CONFIG_MCU_JZ4755)
#include "./jz4755/sysdefs.h"
#elif defined (CONFIG_MCU_JZ4760B)
#include "./jz4760b/sysdefs.h"
#else
#error sysdefs not define
#endif

#endif // ___SYSDEFS_H
