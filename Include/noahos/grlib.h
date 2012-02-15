//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	        字体驱动私有头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2008-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _GR_LIB_H
#define _GR_LIB_H

#include <noahos.h>
#include <font/font.h>

typedef struct _GDC
{	
	HWND hWnd;
	WRECT ClipRect;
	RECT UsrRect;
	
	COLOR BgColor;
	COLOR FgColor;
	
	POINT DrawPos;
	int LineStyle;
	int PenSize;
	int PenShape;
	int LcdWidth;
	int LcdHeight;
	int DrawMode;
	int TextAlign;
	int TtfSize;
	int TtfItaly;
	int TtfBold;
	HANDLE Font;
}GDC;
typedef GDC *PDC;
typedef GDC *HDC;

#define TRANS_RECT_TRIM		0x8000
#define TRANS_RECT_VERTICAL	0x4000

#define TRANS_RECT_FLAT		0x0000
#define TRANS_RECT_FADE		0x0001
#define TRANS_RECT_3D		0x0002
#define TRANS_RECT_COLUMN	0x0003

void GrDrawLine(HDC dc, int x0, int y0, int x1, int y1);
void GrLineTo(HDC dc, int x, int y);
void GrGetXY(HDC dc, int* x, int* y);
void GrMoveTo(HDC dc, int x, int y);
void GrMoveX(HDC dc, int dx);
void GrMoveY(HDC dc, int dy);
void GrDrawRect(HDC dc, int x, int y, int w, int h);
void GrFillRect(HDC dc, int x, int y, int w, int h);

void GrTranspareColumnRect(HDC dc, int x, int y, int w, int h, int alpha, int trim, int ver);
void GrTranspareFadeRect(HDC dc, int x, int y, int w, int h, int alpha, int trim, int ver);
void GrTranspare3DRect(HDC dc, int x, int y, int w, int h, int alpha, int trim);
void GrTranspareFlatRect(HDC dc, int x, int y, int w, int h, int alpha, int trim);
void GrTranspareRect(HDC dc, int x, int y, int w, int h, int alpha, DWORD flag);

void GrDrawPoint(HDC dc, int x, int y);
void GrDrawArc(HDC dc, int x, int y, int rx, int ry, int a0, int a1);
void GrDrawPolygon(HDC dc, const POINT* points, int num, int ox, int oy);
void GrFillPolygon(HDC dc, const POINT* pPoint, int num, int ox, int oy);
void GrDrawPie(HDC dc, int x, int y, int r, int a0, int a1);
void GrDrawCircle(HDC dc, int ox, int oy, int r);
void GrFillCircle(HDC dc, int ox, int oy, int r);
void GrDrawEllipse(HDC dc, int ox, int oy, int rx, int ry);
void GrFillEllipse(HDC dc, int ox, int oy, int rx, int ry);
void GrDrawPolyLine(HDC dc, const POINT* points, int num, int ox, int oy);
void GrDrawPixel(HDC dc, int x, int y);
void GrDrawFocusRect(HDC dc, const RECT* rect);
void GrDrawVLine(HDC dc, int x, int y, int h);
void GrDrawHLine(HDC dc, int x, int y, int w);

void GrFillTextRect(HDC dc, RECT *rect);
int GrCharOut(HDC dc, BYTE* s);
int GrCharOutAt(HDC dc, BYTE* s, int x, int y);
int GrWordOut(HDC dc, BYTE* s);
int GrWordOutAt(HDC dc, BYTE* s, int x, int y);
int GrStrOut(HDC dc, BYTE* s);
int GrStrOutAt(HDC dc, BYTE* s, int x, int y);
int GrStrOutLR(HDC dc, BYTE* s, int lx, int rx);
int GrStrOutLRFill(HDC dc, BYTE* s, int lx, int rx);
int GrStrOutInRect(HDC dc, BYTE* s, RECT* rect);
int GrStrOutInRectFill(HDC dc, BYTE* s, RECT* rect);
int GrTextOut(HDC dc, BYTE* s);
int GrTextOutAt(HDC dc, BYTE* s, int x, int y);
int GrTextOutLR(HDC dc, BYTE* s, int lx, int rx);
int GrTextOutLRFill(HDC dc, BYTE* s, int lx, int rx);
int GrTextOutInRect(HDC dc, BYTE* s, RECT* rect);
int GrTextOutInRectFill(HDC dc, BYTE* s, RECT* rect);
int GrWTextOut(HDC dc, BYTE* s);
int GrWTextOutAt(HDC dc, BYTE* s, int x, int y);
int GrWTextOutLR(HDC dc, BYTE* s, int lx, int rx);
int GrWTextOutLRFill(HDC dc, BYTE* s, int lx, int rx);
int GrWTextOutInRect(HDC dc, BYTE* s, RECT* rect);
int GrWTextOutInRectFill(HDC dc, BYTE* s, RECT* rect);

void GrDrawDIBEx(HDC dc, RECT *rect, int xoff, int yoff, const DIB* pdib);
void GrDrawDIB(HDC dc, int x, int y, const DIB *pdib);
void GrDrawFDIBEx(HDC dc, RECT *rect, int xoff, int yoff, HANDLE hfile);
void GrDrawFDIB(HDC dc, int x, int y, HANDLE hfile);

int GrDrawBMPEx(HDC dc, RECT *rect, int xoff, int yoff, HANDLE hfile, const BYTE *bmpdata);
int GrDrawBMP(HDC dc, int x, int y, HANDLE hfile, const BYTE *bmpdata);
int GrDrawBmpFileEx(HDC dc, RECT *rect, int xoff, int yoff, const char *filename);
int GrDrawBmpFile(HDC dc, int x, int y, const char *filename);
int GrDrawBmpDataEx(HDC dc, RECT *rect, int xoff, int yoff, const BYTE *bmpdata);
int GrDrawBmpData(HDC dc, int x, int y, const BYTE *bmpdata);

int GrDrawPngEx(HDC dc, RECT *rect, int xoff, int yoff, HANDLE hfile, const BYTE *pngdata);
int GrDrawPng(HDC dc, int x, int y, HANDLE hfile, const BYTE *pngdata);
int GrDrawPngFileEx(HDC dc, RECT *rect, int xoff, int yoff, const char *filename);
int GrDrawPngFile(HDC dc, int x, int y, const char *filename);
int GrDrawPngDataEx(HDC dc, RECT *rect, int xoff, int yoff, const BYTE *pngdata);
int GrDrawPngData(HDC dc, int x, int y, const BYTE *pngdata);

int GrDrawJpegEx(HDC dc, RECT *rect, int xoff, int yoff, HANDLE hfile, const BYTE *jpgdata);
int GrDrawJpeg(HDC dc, int x, int y, HANDLE hfile, const BYTE *jpgdata);
int GrDrawJpegFileEx(HDC dc, RECT *rect, int xoff, int yoff, const char *filename);
int GrDrawJpegFile(HDC dc, int x, int y, const char *filename);
int GrDrawJpegDataEx(HDC dc, RECT *rect, int xoff, int yoff, const BYTE *jpgdata);
int GrDrawJpegData(HDC dc, int x, int y, const BYTE *jpgdata);

#endif	// _GR_LIB_H
