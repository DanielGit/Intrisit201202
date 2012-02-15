//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	          GUI私有头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _WINPRV_H
#define _WINPRV_H

#include <gui/wintype.h>
#include <gui/winmsg.h>
#include <kernel/thread.h>
#include <kernel/device.h>

#define Min(v0,v1)	((v0>v1) ? v1 : v0)
#define Max(v0,v1)	((v0>v1) ? v0 : v1)

// 全局变量
extern HWND _ghRootWnd;		// 根窗口句柄

// WinMain.c
#if defined(STC_EXP)
HWND sIsWinHandle(HWND hwnd);
#else
HWND IsWinHandle(HWND hwnd);
#endif
void WinMainThread(DWORD param);
void WinMainReleaseKeyEvent(void);
void WinMainReleaseTimerEvent(void);
void WinMainReleaseTouchEvent(void);
void WinMainReleaseMouseEvent(void);
void WinMainReleasePaintEvent(void);
int WinMainInit(void);
int WinMainPower(int swtch);
void SendMsgToAppRoot(DWORD msgId);

// WinClip.c
RECT* ClipGetRect(HWND hwnd, int *count);
void WinClipMask(RECT *rect);

// WinFocus.c
int FocusSet(HWND hwnd);
int FocusIsSet(HWND hwnd);
HWND FocusGet(void);
int FocusDisable(HWND hwnd);
int FocusEnable(HWND hwnd);
int FocusIsEnable(HWND hwnd);
HANDLE FocusTask(void);

// WinPaint.c
void PaintInvalidateBeforeDelete(HWND hwnd);
int PaintInvalidateWindowEx(HWND hwnd, RECT *rect);
int PaintInvalidateWindow(HWND hwnd);
void PaintInvalidateHerit(HWND hwnd);
void PaintInvalidateScreen(const RECT* rect);
void PaintPostMsg(void);
int PaintSetUsrInvalidRect(HWND hwnd, PRECT rect);

// WinPaintList.c
void PaintListInit(void);
int PaintListIsEmpty(void);
void PaintListAddStart(void);
void PaintListAdd(HWND hwnd);
void PaintListAddEnd(void);
void PaintListDel(HWND hwnd);
void PaintListWait(HWND hwnd);
void PaintListRelease(HWND hwnd);
void PaintListClearRefresh(HWND hwnd);

// WinSaveUnder.c
int SaveUnderInvalid(HWND hwnd);
int SaveUnderAdjust(HWND hwnd, RECT *rects, int count);
int SaveUnderIsUpdated(HWND hwnd);
int SaveUnderDestroy(HWND hwnd);
int SaveUnderRam2Lcd(HWND hwnd);
int SaveUnderLcd2Ram(HWND hwnd);
int SaveUnderInit(void);

// WinRect.c
int RectIntersect(RECT* rect0, const RECT* rect1);
void RectMove(RECT* rect, int dx, int dy);
void RectMerge(RECT* rect0, const RECT* rect1);
void RectMergeTrans(RECT* rect0, const RECT* rect1);
int RectOverlap(RECT *r1, RECT *r2);
int RectIsEmpty(const RECT* rect);
void RectUsrToAbs(RECT *r, DWORD flag);
void RectAbsToUsr(RECT *r, DWORD flag);
int RectIsEqu(RECT *p0, RECT *p1);
int RectInclude(RECT *r1, RECT *r2);
int RectIncludeRect(RECT *r1, RECT *r2);

// WinRot.c
int WinRotAllWindows(int ang);
int WinRotWindowEx(HWND hwnd, RECT *crect, int ang);
int WinRotWindow(HWND hwnd, int ang);
DWORD WinRotGetFlag(HWND hwnd);
int WinRotGetAng(HWND hwnd);
int WinRotGetAngAbs(HWND hwnd);
void WinRotAbsToUsr(RECT *usr, RECT *abs, DWORD rotflg);
void WinRotUsrToAbs(RECT *abs, RECT *usr, DWORD rotflg);

// WinStatus.c
void WinStatusMove(HWND hwnd, int dx, int dy);
void WinStatusMoveTo(HWND hwnd, int x, int y);
void WinStatusResize(HWND hwnd, RECT *rect);
void WinStatusGetRect(HWND hwnd, RECT *rect);
void WinStatusGetAbsRect(HWND hwnd, RECT *rect);
int WinStatusIsTop(HWND hwnd);
int WinStatusIsTopSibling(HWND hwnd);
int WinStatusIsBottom(HWND hwnd);
int WinStatusIsBottomSibling(HWND hwnd);
int WinStatusBringTop(HWND hwnd);
int WinStatusBringBottom(HWND hwnd);
int WinStatusBringUp(HWND hwnd);
int WinStatusBringDown(HWND hwnd);
int WinStatusShow(HWND hwnd, int show);
int WinStatusIsShow(HWND hwnd);
int WinStatusMinimize(HWND hwnd, int min);
int WinStatusIsMinimize(HWND hwnd);
int WinStatusTrans(HWND hwnd, int trans);
int WinStatusIsTrans(HWND hwnd);
int WinStatusEnable(HWND hwnd, int enable);
int WinStatusIsEnable(HWND hwnd);
int WinStatusMsgQ(HWND hwnd);
int WinStatusModal(HWND hwnd);
DWORD WinStatusGetFlag(HWND hwnd, DWORD flag);
int WinStatusGrayBut(HWND hwnd);
int WinStatusColorAll(void);

// WinTimer.c
void WinTimerInit(void);
HANDLE WinTimerCreate(HWND hwnd, int id, DWORD period);
void WinTimerDelete(HANDLE htimer);
DWORD WinTimerTimer(HWND htimer, DWORD period);
void WinTimerDeteteByWnd(HWND hwnd);
void WinPostTimerMsg(void);

// WinObj.c
void WinObjInit(void);
#if defined(STC_EXP)
int sWinObjWaitDel(HWND hwnd, int tmout);
HWND sWinObjCreate(RECT *r, HWND hparent, DWORD style, HANDLE thread);
void sWinObjDelete(HWND hwnd);
void sWinObjSetCallback(HWND hwnd, WIN_CALLBACK cb);
WIN_CALLBACK sWinObjGetCallback(HWND hwnd);
HWND sWinObjSetIme(HWND hwnd, HWND hime);
HWND sWinObjGetIme(HWND hwnd);
#else
int WinObjWaitDel(HWND hwnd, int tmout);
HWND WinObjCreate(RECT *r, HWND hparent, DWORD style, HANDLE thread);
void WinObjDelete(HWND hwnd);
void WinObjSetCallback(HWND hwnd, WIN_CALLBACK cb);
WIN_CALLBACK WinObjGetCallback(HWND hwnd);
HWND WinObjSetIme(HWND hwnd, HWND hime);
HWND WinObjGetIme(HWND hwnd);
#endif
HWND WinObjCreateRoot(void);


// WinKey.c
void WinKeyInit(void);
void WinKeyFocusSwitch(void);
int WinKeyShortcutAdd(HWND hwnd, int key);
int WinKeyShortcutDel(HWND hwnd, int key);
HWND WinKeyShortcutRedir(HWND hredir, int key);
int WinKeyIsShortcut(int key);
int WinKeyPost(int key, int event);
void WinKeyClearShortcutKey(HWND hwnd);
void WinKeyEnable(void);
void WinKeyDisable(void);
char WinKeyIsEnable(void);
HWND WinKeyBindKey(HWND hwnd);
int WinKeyUnBindKey(void);
HWND WinKeyGetBindKey(void);


// WinTouch.c
void WinTouchInit(void);
void WinTouchFocusSwitch(void);
int WinTouchSetting(PTOUCH_KEY tpkey, int areas);
int WinTouchPost(TOUCH_EVENT *event);
void WinTouchEnable(void);
void WinTouchDisable(void);
char WinTouchIsEnable(void);
void WinTouchSmartDisable(void);
void WinTouchSmartEnable(void);
char WinTouchSmartIsEnable(void);
HWND WinTouchBindTouch(HWND hwnd);
int WinTouchUnBindTouch(void);
HWND WinTouchGetBindTouch(void);

// WinGridPen.c
void WinGridInit(void);
int WinGridShortcutAdd(HWND hwnd, HWND hShort,PRECT area);
int WinGridShortcutDel(HWND hwnd, PRECT area);
int WinGridIsShortcut(PRECT area);
HWND WinGridShortcutRedir(HWND hredir, PRECT area);
int WinGridPost(PPOINT pt, int event);
void WinGridClearShortcutArea(HWND hwnd);

#ifdef CONFIG_MOUSE_ENABLE
//winmouse.c
void WinMouseSetMoveArea(RECT *cr,int mode);
int WinMousePos(MOUSE_EVENT *event,int mode);
void SetMouseShowReflash();
int InitMouseShowReflush();
void WinMouseGetPosition(int* x, int* y);
void WinMouseSetPosition(int x, int y);
void WinMouseSetPadArea(RECT* cr);
HWND GuiWinGetPosHwnd(int x, int y);
void SetMouseBusyMode(int mode);

//winmouseshow.c
void SetMouseShowMode(int mode );
int GetMouseShowMode();
void SetMouseMode(int mode );
int GetMouseMode();
int InitMouseShowShpae(PMOUSE_SHAPE pMouseStr);
int SetMouseShowShpae(int start, int end, int time);
void WinMouseSetMode(int mode);
#endif

#endif
