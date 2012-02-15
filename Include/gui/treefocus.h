//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨GUI  *****
//        --------------------------------------
//       	            GUIͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
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

