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

#ifndef _WIN_GR_H
#define _WIN_GR_H

#include <kernel/kernel.h>
#include <gui/wintype.h>
#include <gui/windrv.h>

typedef struct _DIB_LINE
{
	int Width;
	int Offset;
	int Bpp;
	const BYTE *Data;
}DIB_LINE;
typedef DIB_LINE *PDIB_LINE;

#if defined(STC_EXP)
PINDEX sWinGrGetPixel(HWND hwnd, int x, int y);
void sWinGrDrawPixel(HWND hwnd, int x, int y);
void sWinGrDrawHLine(HWND hwnd, int x, int y, int w);
void sWinGrDrawVLine(HWND hwnd, int x, int y, int h);
void sWinGrDrawTransHLine(HWND hwnd, int x, int y, int w, COLOR c);
void sWinGrDrawTransVLine(HWND hwnd, int x, int y, int h, COLOR c);
void sWinGrDrawBLine(HWND hwnd, int x, int y, PDIB_LINE line);
void sWinGrDrawRect(HWND hwnd, PRECT r);
void sWinGrFillRect(HWND hwnd, PRECT r);
void sWinGrDrawChar(HWND hwnd, BYTE* data, int bpl, PRECT r);
void sWinGrDrawCharBmp(HWND hwnd, PCHAR_BITMAP bmp, PRECT r);

#ifdef CONFIG_MAINLCD_ICON_ENABLE
void sWinGrIconOff(HWND hwnd, int icon);
void sWinGrIconOn(HWND hwnd, int icon);
#endif

PINDEX sWinGrColor2Index(HWND hwnd, COLOR c);
COLOR sWinGrIndex2Color(HWND hwnd, PINDEX i);

int sWinGrSetDrawMode(HWND hwnd, int mode);
int sWinGrGetDrawMode(HWND hwnd);
COLOR sWinGrSetBgColor(HWND hwnd, COLOR c);
COLOR sWinGrGetBgColor(HWND hwnd);
COLOR sWinGrSetFgColor(HWND hwnd, COLOR c);
COLOR sWinGrGetFgColor(HWND hwnd);
#else

PINDEX WinGrGetPixel(HWND hwnd, int x, int y);
void WinGrDrawPixel(HWND hwnd, int x, int y);
void WinGrDrawHLine(HWND hwnd, int x, int y, int w);
void WinGrDrawVLine(HWND hwnd, int x, int y, int h);
void WinGrDrawBLine(HWND hwnd, int x, int y, PDIB_LINE line);
void WinGrDrawRect(HWND hwnd, PRECT r);
void WinGrFillRect(HWND hwnd, PRECT r);
void WinGrDrawChar(HWND hwnd, BYTE* data, int bpl, PRECT r);
void WinGrDrawCharBmp(HWND hwnd, PCHAR_BITMAP bmp, PRECT r);

#ifdef CONFIG_MAINLCD_ICON_ENABLE
void WinGrIconOff(HWND hwnd, int icon);
void WinGrIconOn(HWND hwnd, int icon);
#endif

PINDEX WinGrColor2Index(HWND hwnd, COLOR c);
COLOR WinGrIndex2Color(HWND hwnd, PINDEX i);

int WinGrSetDrawMode(HWND hwnd, int mode);
int WinGrGetDrawMode(HWND hwnd);
COLOR WinGrSetBgColor(HWND hwnd, COLOR c);
COLOR WinGrGetBgColor(HWND hwnd);
COLOR WinGrSetFgColor(HWND hwnd, COLOR c);
COLOR WinGrGetFgColor(HWND hwnd);
#endif

void WinGrMemToLcd(HWND hwnd, RECT *r, int ang, int special);
void WinGrLcdToMem(HWND hwnd, RECT *r, int ang, int special);
void WinGrRamToDev(void);
void WinGrLcdToMemUnder(HWND hwnd, RECT *r, int ang, int special, void *buf);
void WinGrMemToLcdUnder(HWND hwnd, RECT *r, int ang, int special, void *buf);

int WinGrInit(void);
int WinGrGetProperty(int* w, int *h, int *bpp);

#endif // _WIN_GR_H
