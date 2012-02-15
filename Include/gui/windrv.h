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

#ifndef _LCDRV_H
#define _LCDRV_H

#include <kernel/kernel.h>
#include <gui/wintype.h>


// ****************************************************************
//   �豸���LCD������������
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
//   ����LCD������������
// ****************************************************************
int LcdrvGetProperty(int* w, int *h, int *bpp);
int LcdrvOn(void);
int LcdrvOff(void);
int LcdrvRamToDev(void);
int LcdrvClear(COLOR c);
int LcdrvInit(void);
void LcdrUnit(void);

#endif // _LCDRV_H
