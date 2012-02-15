//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//         	        
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DEVCHECK_H
#define _DEVCHECK_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

void DevCheckThread(DWORD param);
void DevCheckStandbySet(int standby);
int DevCheckResetCount(void);
void DevCheckPostMsg(DWORD msgid, int data);

#if defined(STC_EXP)
HWND sDevCheckTrustee(HWND hwnd);
int sDevCheckVoltageValve(int valve0, int valve1);
HWND sDevCheckSetMsgPost(HWND hwnd, DWORD msgid);
int sDevCheckAutoOffSet(int minute);
int sDevCheckAutoOffGet(int *);
int sDevCheckForceOffSet(int minute);
int sDevCheckForceOffGet(void);
int sDevCheckBacklightOffSet(int minute);
int sDevCheckBacklightOffGet(void);
#else
HWND DevCheckTrustee(HWND hwnd);
int DevCheckVoltageValve(int lowvoltage, int poweroffvoltage);
HWND DevCheckSetMsgPost(HWND hwnd, DWORD msgid);
int DevCheckAutoOffSet(int minute);
int DevCheckAutoOffGet(int *);
int DevCheckForceOffSet(int minute);
int DevCheckForceOffGet(void);
int DevCheckBacklightOffSet(int minute);
int DevCheckBacklightOffGet(void);
#endif

#endif // _DEVCHECK_H
