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


#ifndef NOAHOS_GUI_V1_H
#define NOAHOS_GUI_V1_H

HWND GetParent(HWND hwnd);
void GetWindowRect(HWND hwnd, RECT* rect);
void ValidateRect(HWND hwnd, const RECT* rect);
void ValidateWindow(HWND hwnd);
void SetUserClipRect(const RECT* rect);
void SetWindowTrans(HWND hwnd);
void ClearWindowTrans(HWND hwnd);
DWORD GetWindowTrans(HWND hwnd);
void ResizeWindow(HWND hwnd, int dx, int dy);
void ResizeWindowTo(HWND hwnd, int w, int h);
void ResizeWindowRect(HWND hwnd, RECT rnew);
int SetFocusWindow(HWND hwnd);
HWND GetFocusWindow(void);
void FocusWindowDisable(HWND hwnd);
void FocusWindowEnable(HWND hwnd);
int IsFocusWindowEnable(HWND hwnd);
void MoveWindow(HWND hwnd, int dx, int dy);
void MoveWindowTo(HWND hwnd, int x, int y);
void SetWindowToBottom(HWND hwnd);
void SetWindowToTop(HWND hwnd);
void EnableWindow(HWND hwnd);
void DisableWindow(HWND hwnd);
WIN_CALLBACK* SetWindowCallback(HWND hwnd, WIN_CALLBACK* cb);
void SendToParent(HWND hChild, MESSAGE* msg);
void NotifyParent(HWND hwnd, int notification);
HWND GetClientWindow(HWND hwnd);
HWND GetWindowItem(HWND hwnd, int id);
HWND GetWindowFirstChild(HWND hwnd);
HWND GetWindowFocusChild(HWND hwnd);
int GetWindowId(HWND hwnd);
void GetWindowInsideRect(HWND hwnd, RECT* rect);
int GetWindowX(HWND hwnd);
int GetWindowY(HWND hwnd);
int GetWindowWidth(HWND hwnd);
int GetWindowHeight(HWND hwnd);
void PaintWindow(HWND hwnd);
HWND GetDialogItem(HWND hwnd, int id);
HWND SetFocusOnNextChild(HWND hparent);
HWND GetNextSibling(HWND hwnd);
HWND GetPreSibling(HWND hwnd);
HWND GetLastSibling(HWND hwnd);
void BroadcastMsg(MESSAGE* msg);
void SendMsg(HWND hwnd, MESSAGE* msg);
void InvalidateRect(HWND hwnd, RECT* rect);
void InvalidateWindow(HWND hwnd);
void InvalidateAllHerit(HWND hwnd);
void DeleteChildren(HWND hwnd);
void DeleteWindow(HWND hwnd);
int DeleteModalWindow(HWND hwnd,  int retval);
HWND SelectWindow(HWND hwnd);
HWND GetActiveWindow(void);
void ActivateWindow(void);
void DeactivateWindow(void);
int PostMsg(HWND hwnd, MESSAGE* msg);
int GetMsg(MESSAGE *Msg);
void PostQuitMsg(HWND hwnd);
void DispatchMsg(HWND hwnd, MESSAGE* msg);

void ClearNortifyWin(HWND hwnd,BYTE mode);
void RegisterNortifyWin(HWND hwnd);
void ClearNortifyImeWin(HWND hwnd,BYTE mode);
void RegisterNortifyImeWin(HWND hwnd);
void RegisterImeWindow(HWND hedit, HWND hime);
void UnregisterImeWindow(HWND hedit, HWND hime);
HWND GetRegisterIme(HWND hedit);
void HideWindow(HWND hwnd);
void ShowWindow(HWND hwnd);
int IsWindowHide(HWND hwnd);
int IsWindowShow(HWND hwnd);
DWORD GetWindowStatus(HWND hwnd);
int IsWindowOnTop(HWND hwnd);
int IsWindowOnBottom(HWND hwnd);
HWND GetTaskMainWnd(int taskid);
HWND GetTaskFocusWnd(int taskid);
int RotateWindowsEx(HWND hwnd, RECT *crect, int ang);
int RotateWindows(HWND hwnd, int ang);
int RotateAllWindows(int ang);
DWORD GetWindowsRotateFlag(HWND hwnd);
int GetWindowsAng(HWND hwnd);
int GetWindowsAngAbs(HWND hwnd);
int GetWindowsMaxWidth(HWND hwnd);
int GetWindowsMaxHeight(HWND hwnd);
int WinTpInit(void);
int WinTpGetPosition(HWND hwnd, POINT *point);

HWND NewTimer(HWND hwnd, int id, DWORD period);
void DeleteTimer(HWND hwnd);
DWORD TimerTimer(HWND htimer, DWORD period);

#endif // NOAHOS_GUI_V1_H
