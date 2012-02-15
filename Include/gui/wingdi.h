//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨GUI  *****
//        --------------------------------------
//       	          LCD����˽��ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
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
