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


#ifndef _GDI_H
#define _GDI_H

#include <noahos.h>
#include <noahos/grlib.h>
#include <font/font.h>

HDC BeginPaint(HWND hwnd, PAINTSTRUCT* ps);;
void EndPaint(HWND hwnd, PAINTSTRUCT* ps);
void SetColor(HDC hdc, COLOR color);
COLOR GetColor(HDC hdc);
void SetColorIndex(HDC hdc, PINDEX index);
PINDEX GetColorIndex(HDC hdc);
void SetBkColor(HDC hdc, COLOR color);
COLOR GetBkColor(HDC hdc);
void SetBkColorIndex(HDC hdc, PINDEX index);
PINDEX GetBkColorIndex(HDC hdc);
int SetPenSize(HDC hdc, int PenSize);
int GetPenSize(HDC hdc);
void SetPenShape(HDC hdc, BYTE ps);
BYTE GetPenShape(HDC hdc);
void SetLineStyle(HDC hdc, BYTE ls);
BYTE GetLineStyle(HDC hdc);
void SetDrawMode(HDC hdc, BYTE dm);
BYTE GetDrawMode(HDC hdc);
HANDLE SetFontEx(HDC hdc, const char *font);
HANDLE SetFont(HDC hdc, HANDLE hfont);
HANDLE GetFont(HDC hdc);
int SetFontProperty(HDC hdc, int size, int italy, int bold);
void SetTextAlign(HDC hdc, BYTE ta);
BYTE GetTextAlign(HDC hdc);
void MoveTo(HDC hdc, int x, int y);
void GetXY(HDC hdc, int* x, int* y);
void MoveX(HDC hdc, int dx);
void MoveY(HDC hdc, int dy);
void Line(HDC hdc, int x0, int y0, int x1, int y1);
void LineTo(HDC hdc, int x, int y);
void Rect(HDC hdc, int x, int y, int w, int h);
void FillRect(HDC hdc, int x, int y, int w, int h);
void ReverseRect(HDC hdc, int x, int y, int w, int h);
void TranspareRect(HDC hdc, int x, int y, int w, int h, int alpha, int flag);
void Point(HDC hdc, int x, int y);
void Arc(HDC hdc, int ox, int oy, int rx, int ry, int a0, int a1);
void Circle(HDC hdc, int ox, int oy, int r);
void FillCircle(HDC hdc, int ox, int oy, int r);
void Ellipse(HDC hdc, int ox, int oy, int rx, int ry);
void FillEllipse(HDC hdc, int ox, int oy, int rx, int ry);
void Polygon(HDC hdc, const POINT* pPoints, int num);
void FillPolygon(HDC hdc, const POINT* pPoints, int num);
void Pie(HDC hdc, int x, int y, int r, int a0, int a1);
void PolyLine(HDC hdc, const POINT* pPoints, int num);
void Pixel(HDC hdc, int x, int y);
void FocusRect(HDC hdc, const RECT* rect);
void DrawHLine(HDC hdc, int x, int y, int w);
void DrawVLine(HDC hdc, int x, int y, int h);
void ClearWindow(HDC hdc);
void ReverseWindow(HDC hdc);
void DrawDIB(HDC hdc, int x, int y, const DIB* pdib);
void DrawDIBEx(HDC hdc, RECT *rect, int xoff, int yoff, const DIB* pdib);
void DrawFDIB(HDC hdc, int x, int y, HANDLE hfile);
void DrawBufDIB(HDC hdc, int x, int y, BYTE* pBuf);
void DrawFDIBEx(HDC hdc, RECT *rect, int xoff, int yoff, HANDLE hfile);
int DrawBMP(HDC hdc, int x, int y, HANDLE hfile);
int DrawBMPEx(HDC hdc, RECT *rect, int xoff, int yoff, HANDLE hfile);
int DrawBmpFile(HDC hdc, int x, int y, const char *filename);
int DrawBmpFileEx(HDC hdc, RECT *rect, int xoff, int yoff,  const char *filename);
int DrawBmpData(HDC hdc, int x, int y, const BYTE *bmpdata);
int DrawBmpDataEx(HDC hdc, RECT *rect, int xoff, int yoff,  const BYTE *bmpdata);
int DrawPng(HDC hdc, int x, int y, HANDLE hfile);
int DrawPngEx(HDC hdc, RECT *rect, int xoff, int yoff, HANDLE hfile);
int DrawPngFile(HDC hdc, int x, int y, const char *filename);
int DrawPngFileEx(HDC hdc, RECT *rect, int xoff, int yoff,  const char *filename);
int DrawPngData(HDC hdc, int x, int y, const BYTE *pngdata);
int DrawPngDataEx(HDC hdc, RECT *rect, int xoff, int yoff,  const BYTE *pngdata);
int DrawJpeg(HDC hdc, int x, int y, HANDLE hfile);
int DrawJpegEx(HDC hdc, RECT *rect, int xoff, int yoff, HANDLE hfile);
int DrawJpegFile(HDC hdc, int x, int y, const char *filename);
int DrawJpegFileEx(HDC hdc, RECT *rect, int xoff, int yoff,  const char *filename);
int DrawJpegData(HDC hdc, int x, int y, const BYTE *pngdata);
int DrawJpegDataEx(HDC hdc, RECT *rect, int xoff, int yoff,  const BYTE *pngdata);
int CharOut(HDC hdc, BYTE* s);
int CharOutAt(HDC hdc, BYTE* s, int x, int y);
int WordOut(HDC hdc, BYTE* s);
int WordOutAt(HDC hdc, BYTE* s, int x, int y);
int StrOut(HDC hdc, BYTE* s);
int StrOutAt(HDC hdc, BYTE* s, int x, int y);
int StrOutLR(HDC hdc, BYTE* s, int lx, int rx);
int StrOutLRFill(HDC hdc, BYTE* s, int lx, int rx);
int StrOutInRect(HDC hdc, BYTE* s, RECT* rect);
int StrOutInRectFill(HDC hdc, BYTE* s, RECT* rect);
int TextOut(HDC hdc, BYTE* s);
int TextOutAt(HDC hdc, BYTE* s, int x, int y);
int TextOutLR(HDC hdc, BYTE* s, int lx, int rx);
int TextOutLRFill(HDC hdc, BYTE* s, int lx, int rx);
int TextOutInRect(HDC hdc, BYTE* s, RECT* rect);
int TextOutInRectFill(HDC hdc, BYTE* s, RECT* rect);
void FillTextRect(HDC hdc, RECT* rect);
int WTextOut(HDC hdc, BYTE* s);
int WTextOutAt(HDC hdc, BYTE* s, int x, int y);
int WTextOutLR(HDC hdc, BYTE* s, int lx, int rx);
int WTextOutLRFill(HDC hdc, BYTE* s, int lx, int rx);
int WTextOutInRect(HDC hdc, BYTE* s, RECT* rect);
int WTextOutInRectFill(HDC hdc, BYTE* s, RECT* rect);
void FillWTextRect(HDC hdc, RECT* rect);
void UpdateWindowIcons(HWND hwnd);
void IconOn(HWND hwnd, int icon);
void IconOff(HWND hwnd, int icon);
int IsIconOff(HWND hwnd, int icon);
void IconClearAll(HWND hwnd);
void catchScreen(BYTE *filename, void *lcdbuffer);

#endif	// _GDI_H
