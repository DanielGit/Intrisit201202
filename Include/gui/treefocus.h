//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	            GUI头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>
#include <gui/wintype.h>

#ifndef _WIN_FOCUS_H
#define _WIN_FOCUS_H

void FocusInit(HWND hwnd);
HWND FocusSetWnd(HWND hwnd);
HWND FocusGetTopWnd(HWND href);
HWND FocusDelete(HWND hwnd);
int FocusCheckSwitch(HWND hfrom, HWND hto);
int FocusSwitch(HWND hfrom, HWND hto);
HWND FocusTopWnd(void);
HWND FocusGetUnlost(HWND hcom, HWND hset);
HWND FocusGetUnset(HWND hcom, HWND hset);
HWND FocusMainWnd(HWND hfrom, HWND hto);
int FocusSwitchToAncestor(HWND hfrm, HWND hto);

#endif // _WIN_FOCUS_H

