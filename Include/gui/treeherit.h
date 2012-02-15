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

