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

#ifndef _SHARE_VAR_H
#define _SHARE_VAR_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(STC_EXP)
int sShareVarCreate(int size);
int sShareVarWrite(int offset, int data);
int sShareVarRead(int offset);
#else
int ShareVarCreate(int size);
int ShareVarWrite(int offset, int data);
int	ShareVarRead(int offset);
#endif

#endif // _SHARE_VAR_H
