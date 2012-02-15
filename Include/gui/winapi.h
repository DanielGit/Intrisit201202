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


#ifndef _WIN_API_H
#define _WIN_API_H

void WinApiInit(void);

#if defined(STC_EXP)
// WinApi.c
void sGuiWinApiLock(void);
void sGuiWinApiUnlock(void);
void *sGuiSetUsrPoint(HWND hwnd, void *pusr, int no);
void *sGuiGetUsrPoint(HWND hwnd, int no);

// WinBase.c
int sGuiIsWinHandle(HWND hwnd);

// WinFocus.c
int sGuiFocusSet(HWND hwnd);
int sGuiFocusIsSet(HWND hwnd);
HWND sGuiFocusGet(void);
int sGuiFocusDisable(HWND hwnd);
int sGuiFocusEnable(HWND hwnd);
int sGuiFocusIsEnable(HWND hwnd);
HANDLE sGuiFocusTask(void);

// WinPaint.c
int sGuiPaintInvalidateWindowEx(HWND hwnd, RECT *rect);
int sGuiPaintInvalidateWindow(HWND hwnd);
void sGuiPaintInvalidateHerit(HWND hwnd);
void sGuiPaintInvalidateScreen(const RECT* rect);
void sGuiPaintListWait(HWND hwnd);
void sGuiPaintListRelease(HWND hwnd);

// WinRot.c
int sGuiWinRotAllWindows(int ang);
int sGuiWinRotWindowEx(HWND hwnd, RECT *crect, int ang);
int sGuiWinRotWindow(HWND hwnd, int ang);
DWORD sGuiWinRotGetFlag(HWND hwnd);
int sGuiWinRotGetAng(HWND hwnd);
int sGuiWinRotGetAngAbs(HWND hwnd);

// WinStatus.c
void sGuiWinStatusMove(HWND hwnd, int dx, int dy);
void sGuiWinStatusMoveTo(HWND hwnd, int x, int y);
void sGuiWinStatusResize(HWND hwnd, RECT *rect);
void sGuiWinStatusGetRect(HWND hwnd, RECT *rect);
void sGuiWinStatusGetAbsRect(HWND hwnd, RECT *rect);
int sGuiWinStatusIsTop(HWND hwnd);
int sGuiWinStatusIsTopSibling(HWND hwnd);
int sGuiWinStatusIsBottom(HWND hwnd);
int sGuiWinStatusIsBottomSibling(HWND hwnd);
int sGuiWinStatusBringTop(HWND hwnd);
int sGuiWinStatusBringBottom(HWND hwnd);
int sGuiWinStatusBringUp(HWND hwnd);
int sGuiWinStatusBringDown(HWND hwnd);
int sGuiWinStatusShow(HWND hwnd, int show);
int sGuiWinStatusIsShow(HWND hwnd);
int sGuiWinStatusMinimize(HWND hwnd, int min);
int sGuiWinStatusIsMinimize(HWND hwnd);
int sGuiWinStatusTrans(HWND hwnd, int trans);
int sGuiWinStatusIsTrans(HWND hwnd);
int sGuiWinStatusEnable(HWND hwnd, int enable);
int sGuiWinStatusIsEnable(HWND hwnd);
int sGuiWinStatusMsgQ(HWND hwnd);
int sGuiWinStatusModal(HWND hwnd);
DWORD sGuiWinStatusGetFlag(HWND hwnd, DWORD flag);
int sGuiWinStatusGrayBut(HWND hwnd);
int sGuiWinStatusColorAll(void);
int sGuiWinStatusGetLcdSize(char type);
int sGuiWinStatusIsTvOut(void);
HWND sGuiWinSetGridShortArea(char* flag, HWND hwnd);

// WinTree
HWND sGuiHeritGetParent(HWND hchild);
HWND sGuiHeritGetFirstChild(HWND hparent);
HWND sGuiHeritGetNextSibling(HWND hsibling);
HWND sGuiHeritGetPrevSibling(HWND hsibling);

// WinTimer.c
HANDLE sGuiWinTimerCreate(HWND hwnd, int id, DWORD period);
void sGuiWinTimerDelete(HANDLE htimer);
DWORD sGuiWinTimerTimer(HWND hTimer, DWORD period);

// WinKey.c
int sGuiKeyShortcutAdd(HWND hwnd, int key);
HWND sGuiKeyShortcutRedir(HWND hwnd, int key);
int sGuiKeyShortcutDel(HWND hwnd, int key);
int sGuiKeyIsShortcut(int key);
void sGuiKeyEnable(void);
void sGuiKeyDisable(void);
char sGuiKeyIsEnable(void);
HWND sGuiKeyBindKey(HWND hwnd);
int sGuiKeyUnBindKey(void);
HWND sGuiKeyGetBindKey(void);

// WinTouch.c
int sGuiTouchSetting(PTOUCH_KEY tpkey, int areas);
void sGuiTouchEnable(void);
void sGuiTouchDisable(void);
char sGuiTouchIsEnable(void);
void sGuiTouchSmartEnable(void);
void sGuiTouchSmartDisable(void);
char sGuiTouchSmartIsEnable(void);
HWND sGuiTouchBindTouch(HWND hwnd);
int sGuiTouchUnBindTouch(void);
HWND sGuiTouchGetBindTouch(void);

// WinGridPen.c
int sGuiGridShortcutAdd(HWND hwnd, PRECT area);
HWND sGuiGridShortcutRedir(HWND hwnd, PRECT area);
int sGuiGridShortcutDel(HWND hwnd, PRECT area);
int sGuiGridIsShortcut(PRECT area);

// WinQuit.c
int sGuiQuitRequest(HWND hquit, HWND hrequest, int data);
int sGuiQuitAnswerWait(HWND hwnd, int data);
int sGuiQuitAnswerCancel(HWND hwnd, int data);

#ifdef CONFIG_MOUSE_ENABLE
// winmouseshow.c
int sGuiMouseSetMoveRect(RECT *cr,int mode);
int sGuiMouseSetShape(int start, int end, int time);
int sGuiMouseInitShape(PMOUSE_SHAPE pShape);
int sGuiMouseGetMode();
void sGuiMouseSetMode(int mode);
int sGuiMouseGetDisplayMode();
void sGuiMouseSetDisplayMode(int mode);
int sGuiMouseGetPosition(int *x, int *y);
int sGuiMouseSetPosition(int x, int y);
void sGuiMouseSetPadArea(RECT *cr);
HWND sGuiWinGetPostionHwnd(int x, int y);
#endif

#else


// WinApi.c
void GuiWinObjLock(void);
void GuiWinObjUnlock(void);
void *GuiSetUsrPoint(HWND hwnd, void *pusr, int no);
void *GuiGetUsrPoint(HWND hwnd, int no);

// WinBase.c
int GuiIsWinHandle(HWND hwnd);

// WinFocus.c
int GuiFocusSet(HWND hwnd);
int GuiFocusIsSet(HWND hwnd);
HWND GuiFocusGet(void);
int GuiFocusDisable(HWND hwnd);
int GuiFocusEnable(HWND hwnd);
int GuiFocusIsEnable(HWND hwnd);
HANDLE GuiFocusTask(void);

// WinPaint.c
int GuiPaintInvalidateWindowEx(HWND hwnd, RECT *rect);
int GuiPaintInvalidateWindow(HWND hwnd);
void GuiPaintInvalidateHerit(HWND hwnd);
void GuiPaintInvalidateScreen(const RECT* rect);
void GuiPaintListWait(HWND hwnd);
void GuiPaintListRelease(HWND hwnd);

// WinRot.c
int GuiWinRotAllWindows(int ang);
int GuiWinRotWindowEx(HWND hwnd, RECT *crect, int ang);
int GuiWinRotWindow(HWND hwnd, int ang);
DWORD GuiWinRotGetFlag(HWND hwnd);
int GuiWinRotGetAng(HWND hwnd);
int GuiWinRotGetAngAbs(HWND hwnd);

// WinTree
HWND GuiHeritGetParent(HWND hchild);
HWND GuiHeritGetFirstChild(HWND hparent);
HWND GuiHeritGetNextSibling(HWND hsibling);
HWND GuiHeritGetPrevSibling(HWND hsibling);

// WinStatus.c
void GuiWinStatusMove(HWND hwnd, int dx, int dy);
void GuiWinStatusMoveTo(HWND hwnd, int x, int y);
void GuiWinStatusResize(HWND hwnd, RECT *rect);
void GuiWinStatusGetRect(HWND hwnd, RECT *rect);
void GuiWinStatusGetAbsRect(HWND hwnd, RECT *rect);
int GuiWinStatusIsTop(HWND hwnd);
int GuiWinStatusIsTopSibling(HWND hwnd);
int GuiWinStatusIsBottom(HWND hwnd);
int GuiWinStatusIsBottomSibling(HWND hwnd);
int GuiWinStatusBringTop(HWND hwnd);
int GuiWinStatusBringBottom(HWND hwnd);
int GuiWinStatusBringUp(HWND hwnd);
int GuiWinStatusBringDown(HWND hwnd);
int GuiWinStatusShow(HWND hwnd, int show);
int GuiWinStatusIsShow(HWND hwnd);
int GuiWinStatusMinimize(HWND hwnd, int min);
int GuiWinStatusIsMinimize(HWND hwnd);
int GuiWinStatusTrans(HWND hwnd, int trans);
int GuiWinStatusIsTrans(HWND hwnd);
int GuiWinStatusEnable(HWND hwnd, int enable);
int GuiWinStatusIsEnable(HWND hwnd);
int GuiWinStatusMsgQ(HWND hwnd);
int GuiWinStatusModal(HWND hwnd);
DWORD GuiWinStatusGetFlag(HWND hwnd, DWORD flag);
int GuiWinStatusGrayBut(HWND hwnd);
int GuiWinStatusColorAll(void);
int GuiWinStatusGetLcdSize(char type);
int sIsTvOutMode();

// WinTimer.c
HANDLE GuiWinTimerCreate(HWND hwnd, int id, DWORD period);
void GuiWinTimerDelete(HANDLE htimer);
DWORD GuiWinTimerTimer(HWND hTimer, DWORD period);

// WinKey.c
int GuiKeyShortcutAdd(HWND hwnd, int key);
HWND GuiKeyShortcutRedir(HWND hwnd, int key);
int GuiKeyShortcutDel(int key);
int GuiKeyShortcutEnable(int key);
int GuiKeyShortcutDisable(int key);
int GuiKeyIsShortcut(int key);
void GuiKeyEnable(void);
void GuiKeyDisable(void);
char GuiKeyIsEnsable(void);

// WinTouch.c
int GuiTouchSetting(PTOUCH_KEY tpkey, int areas);
void GuiTouchEnable(void);
void GuiTouchDisable(void);
char GuiTouchIsEnsable(void);
void sGuiTouchSmartEnable(void);
void sGuiTouchSmartDisable(void);
char sGuiTouchSmartIsEnable(void)

// WinGridPen.c
int GuiGridShortcutAdd(HWND hwnd, PRECT area);
HWND GuiGridShortcutRedir(HWND hwnd, PRECT area);
int GuiGridShortcutDel(HWND hwnd, PRECT area);
int GuiGridIsShortcut(PRECT area);

// WinQuit.c
int GuiQuitRequest(HWND hquit, HWND hrequest, int data);
int GuiQuitAnswerWait(HWND hwnd, int data);
int GuiQuitAnswerCancel(HWND hwnd, int data);

#ifdef CONFIG_MOUSE_ENABLE
// winmouseshow.c
int GuiMouseSetMoveRect(RECT *cr,int mode);
int GuiMouseSetShape(int start, int end, int time);
int GuiMouseInitShape(PMOUSE_SHAPE pShape);
int GuiMouseGetMode();
void GuiMouseSetMode(int mode);
int GuiMouseGetDisplayMode();
void GuiMouseSetDisplayMode(int mode);
int GuiMouseGetPosition(int *x, int *y);
int GuiMouseSetPosition(int x, int y);
void GuiMouseSetPadArea(RECT *cr);
HWND GuiWinGetPostionHwnd(int x, int y);
#endif

#endif  // STC_EXP

#endif	// _WIN_API_H
