//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//         	        
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _AP_MAN_H
#define _AP_MAN_H

#include <kernel/kernel.h>
int ApManInit(void);

#if defined(STC_EXP)
int sApManGetCreate(char *name, int max);
int sApManGetDestroy(char *name, int max);
#else
int ApManGetCreate(char *name, int max);
int ApManGetDestroy(char *name, int max);
#endif

#endif // _AP_MAN_H
