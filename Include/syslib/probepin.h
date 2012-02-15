//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//       	        时间日期处理函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-9      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>

#ifndef _PROBE_PIN_H
#define _PROBE_PIN_H

#if defined(STC_EXP)
void sProbePinOpen(void);
void sProbePinClose(void);
int sProbePinFlp(void);
int sProbePinOut(int lvl);
int sProbePinGet(void);
#else
void ProbePinOpen(void);
void ProbePinClose(void);
int ProbePinFlp(void);
int ProbePinOut(int lvl);
int ProbePinGet(void);
#endif

#endif // _PROBE_PIN_H
