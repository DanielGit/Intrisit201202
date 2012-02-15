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

#ifndef _WIN_LAYER_H
#define _WIN_LAYER_H

void LayerInit(HWND hwnd);
void LayerAddWnd(HWND hwnd);
void LayerDeleteWnd(HWND hwnd);

int LayerBringUp(HWND hwnd);
int LayerBringDown(HWND hwnd);
int LayerBringTop(HWND hwnd);
int LayerBringBottom(HWND hwnd);

HWND LayerSiblingNext(HWND hwnd);
HWND LayerSiblingPrev(HWND hwnd);
HWND LayerSiblingBottom(HWND hwnd);
HWND LayerSiblingTop(HWND hwnd);
HWND LayerParent(HWND hwnd);
HWND LayerChildFirst(HWND hwnd);
HWND LayerChildLast(HWND hwnd);

HWND LayerTopWindow(HWND hwnd);
HWND LayerModalWindow(HWND hwnd);
void AddRootAppMsg(MESSAGE* pMsg);

#endif // _WIN_LAYER_H

