//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	          LCD函数私有头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _WIN_GDI_H
#define _WIN_GDI_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

int WinGdiInit(void);

#if defined(STC_EXP)
int sGdiBeginPaint(HWND hwnd);
int sGdiEndPaint(HWND hwnd);
void sGdiGetClipRect(HWND hwnd, PWRECT rect);
void sGdiSetClipRect(HWND hwnd, PWRECT rect);
void *sGdiSetUserDc(HWND hwnd, void *dc);
void *sGdiGetUserDc(HWND hwnd);
#else

int GdiBeginPaint(HWND hwnd);
int GdiEndPaint(HWND hwnd);
void GdiGetClipRect(HWND hwnd, PWRECT rect);
void GdiSetClipRect(HWND hwnd, PWRECT rect);
void *GdiSetUserDc(HWND hwnd, void *dc);
void *GdiGetUserDc(HWND hwnd);
#endif


#endif // _WIN_GDI_H
