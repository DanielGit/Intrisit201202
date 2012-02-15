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

#ifndef _LCDRV_H
#define _LCDRV_H

#include <kernel/kernel.h>
#include <gui/wintype.h>


// ****************************************************************
//   设备相关LCD驱动函数定义
// ****************************************************************
PINDEX LcdrvGetPixel(PLCD_DC dc, int x, int y);

void LcdrvDrawPixel(PLCD_DC dc, int x, int y);
void LcdrvDrawHLine(PLCD_DC dc, int x, int y, int w);
void LcdrvDrawVLine(PLCD_DC dc, int x, int y, int h);
void LcdrvDrawBLine(PLCD_DC dc, int x, int y, int w, int off, const BYTE* data, int bpp);
void LcdrvDrawRect(PLCD_DC dc, int x, int y, int w, int h);
void LcdrvFillRect(PLCD_DC dc, int x, int y, int w, int h);
void LcdrvDrawChar(PLCD_DC dc, const BYTE* data, int bpl, RECT *draw, RECT *clip);
void LcdrvDrawCharBmp(PLCD_DC dc, PCHAR_BITMAP bmp, RECT *draw, RECT *clip);

PINDEX LcdrvColor2Index(COLOR c);
COLOR LcdrvIndex2Color(PINDEX i);

void LcdrvMemToLcd(RECT *r, int ang, int special);
void LcdrvLcdToMem(RECT *r, int ang, int special);

void LcdrvMemToLcdUnder(RECT *pRect, int ang, int special, void *buf);
void LcdrvLcdToMemUnder(RECT *pRect, int ang, int special, void *buf);

#ifdef CONFIG_MAINLCD_ICON_ENABLE
void LcdrvIconOff(int icon);
void LcdrvIconOn(int icon);
#endif

// ****************************************************************
//   公共LCD驱动函数定义
// ****************************************************************
int LcdrvGetProperty(int* w, int *h, int *bpp);
int LcdrvOn(void);
int LcdrvOff(void);
int LcdrvRamToDev(void);
int LcdrvClear(COLOR c);
int LcdrvInit(void);
void LcdrUnit(void);

#endif // _LCDRV_H
