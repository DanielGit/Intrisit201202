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
#include <gui/treebase.h>

#ifndef _WIN_HERIT_H
#define _WIN_HERIT_H

void HeritInit(HWND hwnd);
void HeritAddWnd(HWND hparent, HWND hchild);
void HeritDeleteWnd(HWND hwnd);
HWND HeritGetParent(HWND hwnd);
HWND HeritGetAppmain(HWND hwnd);
HWND HeritLastChild(HWND hwnd);
HWND HeritFirstChild(HWND hwnd);
HWND HeritNextSibling(HWND hwnd);
HWND HeritPrevSibling(HWND hwnd);

#endif // _WIN_HERIT_H

