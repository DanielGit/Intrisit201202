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

#ifndef _WIN_EMBODY_H
#define _WIN_EMBODY_H

void EmbodyInit(HWND hwnd);
void EmbodyAddWnd(HWND hparent, HWND hchild);
void EmbodyDeleteWnd(HWND hwnd);
HWND EmbodyGetParent(HWND hwnd);
HWND EmbodyGetAncestor(HWND hwnd);
HWND EmbodyLastChild(HWND hwnd);
HWND EmbodyFirstChild(HWND hwnd);
HWND EmbodyNextSibling(HWND hwnd);
HWND EmbodyPrevSibling(HWND hwnd);

#endif // _WIN_EMBODY_H

