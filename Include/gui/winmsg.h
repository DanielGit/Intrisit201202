//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	            GUI头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2006-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>
#include <gui/wintype.h>

#ifndef _WIN_MSG_H
#define _WIN_MSG_H

int QSendPutMsg(PLIST queue, PMESSAGEEX msg);
int QSendPollMsg(PLIST queue, PMESSAGEEX msg);
int QSendPollStamp(PLIST queue, DWORD *stamp);
int QSendGetMsg(PLIST queue, PMESSAGEEX msg);
void QSendClearMsg(PLIST queue);

int QReplyPutMsg(PLIST queue, PMESSAGEEX msg);
int QReplyPollMsg(PLIST queue, PMESSAGEEX msg);
int QReplyPollStamp(PLIST queue, DWORD *stamp);
int QReplyGetMsg(PLIST queue, PMESSAGEEX msg);
void QReplyClearMsg(PLIST queue);

int QPostPutMsg(PLIST queue, PMESSAGE msg);
int QPostPollMsg(PLIST queue, PMESSAGE msg);
int QPostPollStamp(PLIST queue, DWORD *stamp);
int QPostGetMsg(PLIST queue, PMESSAGE msg);
void QPostClearMsg(PLIST queue);

int QQuitPutMsg(PLIST queue, PMESSAGE msg);
int QQuitPollMsg(PLIST queue, PMESSAGE msg);
int QQuitPollStamp(PLIST queue, DWORD *stamp);
int QQuitGetMsg(PLIST queue, HWND hwnd, PMESSAGE msg);
void QQuitClearMsg(PLIST queue);

int QKeyPutMsg(PLIST queue, PMESSAGE msg, DWORD stamp);
int QKeyPollMsg(PLIST queue, PMESSAGE msg);
int QKeyPollStamp(PLIST queue, DWORD *stamp);
int QKeyGetMsg(PLIST queue, PMESSAGE msg);
void QKeyClearMsg(PLIST queue);

#ifdef CONFIG_TOUCH_ENABLE
int QTouchPutMsg(PLIST queue, PMESSAGE msg, DWORD stamp);
int QTouchPollMsg(PLIST queue, PMESSAGE msg);
int QTouchPollStamp(PLIST queue, DWORD *stamp);
int QTouchGetMsg(PLIST queue, PMESSAGE msg);
void QTouchClearMsg(PLIST queue);
#endif

#ifdef CONFIG_MOUSE_ENABLE
int QMousePutMsg(PLIST queue, PMESSAGE msg, DWORD stamp);
int QMousePollMsg(PLIST queue, PMESSAGE msg);
int QMousePollStamp(PLIST queue, DWORD *stamp);
int QMouseGetMsg(PLIST queue, PMESSAGE msg);
void QSendClearMsg(PLIST queue);
#endif

int QPaintPutMsg(PLIST queue, PMESSAGE msg);
int QPaintPollMsg(PLIST queue, PMESSAGE msg);
int QPaintPollStamp(PLIST queue, DWORD *stamp);
int QPaintGetMsg(PLIST queue, PMESSAGE msg);
void QPaintClearMsg(PLIST queue);
int QPaintClearHwndMsg(PLIST queue,HWND hwnd);

int QTimerPutMsg(PLIST queue, PMESSAGE msg);
int QTimerPollMsg(PLIST queue, PMESSAGE msg);
int QTimerPollStamp(PLIST queue, DWORD *stamp);
int QTimerGetMsg(PLIST queue, PMESSAGE msg);
void QTimerClearMsg(PLIST queue);

int QFastPutMsg(PLIST queue, PMESSAGE msg);
int QFastPollMsg(PLIST queue, PMESSAGE msg);
int QFastPollStamp(PLIST queue, DWORD *stamp);
int QFastGetMsg(PLIST queue, PMESSAGE msg);
void QFastClearMsg(PLIST queue);


int GuiMsgInit(void);
int GuiMsgCreate(HANDLE thread, HWND hwnd);

void GuiMsgPowerOff(HWND hwnd);
void GuiMsgPowerOn(void);
int GuiMsgPowerMode(HWND hwnd);
int GuiMsgPutStampMsg(PMESSAGE msg, DWORD stamp);

#if defined(STC_EXP)
int sGuiMsgPutMsg(PMESSAGE msg);
int sGuiMsgPollMsg(HANDLE thread, PMESSAGE msg);
int sGuiMsgGetMsg(HANDLE thread, PMESSAGE msg);
int sGuiMsgPutSendMsg(PMESSAGEEX msg);
int sGuiMsgPollSendMsg(HANDLE thread, PMESSAGEEX msg);
int sGuiMsgGetSendMsg(HANDLE thread, PMESSAGEEX msg);
int sGuiMsgPutReplyMsg(PMESSAGEEX msg);
int sGuiMsgPollReplyMsg(HANDLE thread, PMESSAGEEX msg);
int sGuiMsgGetReplyMsg(HANDLE thread, PMESSAGEEX msg);
int sGuiMsgCheck(HWND hwnd0, HWND hwnd1);
int sGuiMsgDelete(HANDLE thread);
int sGuiMsgModalGetMsg(HANDLE thread, HWND hmodal, PMESSAGE msg);

#else
int GuiMsgPutMsg(PMESSAGE msg);
int GuiMsgPollMsg(HANDLE thread, PMESSAGE msg);
int GuiMsgGetMsg(HANDLE thread, PMESSAGE msg);
int GuiMsgPutSendMsg(PMESSAGEEX msg);
int GuiMsgPollSendMsg(HANDLE thread, PMESSAGEEX msg);
int GuiMsgGetSendMsg(HANDLE thread, PMESSAGEEX msg);
int GuiMsgPutReplyMsg(PMESSAGEEX msg);
int GuiMsgPollReplyMsg(HANDLE thread, PMESSAGEEX msg);
int GuiMsgGetReplyMsg(HANDLE thread, PMESSAGEEX msg);
int GuiMsgCheck(HWND hwnd0, HWND hwnd1);
int GuiMsgDelete(HANDLE thread);
int GuiMsgModalGetMsg(HANDLE thread, HWND hmodal, PMESSAGE msg);

#endif

#endif // _WIN_MSG_H

