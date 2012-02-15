//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	         兼容上一版本GUI相关函数定义    
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2008-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef NOAHOS_WIDGET_H
#define NOAHOS_WIDGET_H

#include <noahos.h>

#define	EDN_TEXT_CHANGED		(WM_USER+00)	// EDIT控件向父窗口发送的文本内容改变标识

#define SBN_VALUE_CHANGED		(WM_USER+10)	// SCROLL控件值改变消息(该消息要通过WM_NOTIFY处理)
#define SBN_CREATE				(WM_USER+11)	// SCROLL控件创建消息(该消息要通过WM_NOTIFY处理)
#define SBM_GET_STATE			(WM_USER+12)	// 获取SCROLL控件状态消息
#define SBM_SET_STATE			(WM_USER+13)	// 设置SCROLL控件状态消息
#define SBM_ADD					(WM_USER+14)	// 添加SCROLL控件消息

#define LBN_SEL_CHANGED			(WM_USER+20)	// LISTBOX控件选择条目改变消息(该消息要通过WM_NOTIFY处理)

#define SLN_VALUE_CHANGED		(WM_USER+30)	// SLIDER控件值改变消息(该消息要通过WM_NOTIFY处理)

#define BM_CLICK           		(WM_USER+40)

// 滚动条控件数据结构
typedef struct _SCROLL_STATE
{
	int TotalItems;		// 滚动条标识条目总数
	int StartItem;		// 滚动条表示显示起始条目
	int PageItems;		// 滚动条表示显示页条目数
}SCROLL_STATE;
typedef SCROLL_STATE *PSCROLL_STATE;

// Button.c
HWND ButtonCreate(int x, int y, int w, int h, HWND hparent, DWORD style, const char *str);
void ButtonSetText(HWND hwnd, const char* s);
void ButtonSetFont(HWND hwnd, const char* font);

// Caret.c 
HWND CaretCreate(HWND hparent);
void CaretSetRect(HWND hwnd, RECT *pr);
void CaretInit(HWND hwnd);

// Checkbox.c
HWND CheckboxCreate(int x, int y, int w, int h,	HWND hparent, DWORD style, const char *str);
void CheckboxCheck(HWND hwnd);
void CheckboxUncheck(HWND hwnd);
int CheckboxIsChecked(HWND hwnd);

// Edit.c
HWND EditCreate(int x, int y, int w, int h,	HWND hparent, DWORD style, int maxlen);
void EditSetFont(HWND hwnd, const char* font);
void EditSetText(HWND hwnd, const char* sNew);
void EditGetText(HWND hwnd, char* sDest, int MaxLen);
void EditGetValue(HWND hwnd, void* value);
void EditSetValue(HWND hwnd, void* value);
int EditSetMaxLen(HWND hwnd, int MaxLen);
void EditSetTextAlign(HWND hwnd, char Align);
void EditSetDecMode(HWND hwnd, int value);
void EditSetHexMode(HWND hwnd, DWORD value);

// Listbox.c
HWND ListBoxCreate(HWND hparent, int x, int y, int w, int h, DWORD style);
void ListBoxAddKey(HWND hwnd, int key);
void ListBoxSetText(HWND hwnd, char** pptext);
void ListBoxSetFont(HWND hwnd, const char *font);
void ListBoxSetSel(HWND hwnd, int select);
void ListBoxIncSel(HWND hwnd);
void ListBoxDecSel(HWND hwnd);
int ListBoxGetSel(HWND hwnd);

// Messagbox.c
int MessageBox(int x, int y, int w, int h, HWND hparent, DWORD style, 
					const char* msg_str, const char* title_str, DWORD mode);

// Progbar.c
HWND ProgBarCreate(int x, int y, int w, int h,	HWND hparent, DWORD style);
void ProgBarSetValue(HWND hwnd, int value);
void ProgBarSetFont(HWND hwnd, const char* font);
void ProgBarSetText(HWND hwnd, const char* s);
void ProgBarSetMinMax(HWND hwnd, int min, int max);

// Radio.c
HWND RadioCreate(int x, int y, int w, int h, HWND hParent, DWORD style);
int RadioGroup(HWND hold, HWND hnew);
HWND RadioGetChecked(HWND hwnd);

// Scrollbar.c
HWND ScrollBarCreate(int x, int y, int w, int h, HWND hparent, DWORD style);
HWND ScrollBarAttached(HWND hparent, DWORD style);
void ScrollBarDec(HWND hwnd);
void ScrollBarInc(HWND hwnd);
void ScrollBarSetStartItem(HWND hwnd, int start);
void ScrollBarSetTotalItems(HWND hwnd, int items);
void ScrollBarSetPageItems(HWND hwnd, int items);
void ScrollBarSetState(HWND hwnd, const SCROLL_STATE* state);
int ScrollBarGetValue(HWND hwnd);
void ScrollbarGetStatus(HWND hwnd, SCROLL_STATE* state);
void ScrollbarSetStatus(HWND hwnd, SCROLL_STATE* state);

// Slider.c
HWND SliderCreate(int x, int y, int w, int h, HWND hparent, DWORD style);
void SliderDec(HWND hwnd);
void SliderInc(HWND hwnd);
void SliderSetWidth(HWND hwnd, int width);
void SliderSetValue(HWND hwnd, int value);
void SliderSetRange(HWND hwnd, int min, int max);
int SliderGetValue(HWND hwnd);

// Text.c
HWND TextCreate(int x, int y, int w, int h, HWND hParent, int id, DWORD style);
void TextSetFont(HWND hwnd, const char* font);
void TextInitBuf(HWND hwnd, BYTE* pBuf, int BufSize);

#endif // NOAHOS_WIDGET_H
