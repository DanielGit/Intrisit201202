//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//         	        
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2005-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _SLEEP_H
#define _SLEEP_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(STC_EXP)
int sSleepEnterRequest(int mode);
int sSleepEnterConfirm(HWND hwnd);
int sSleepExitRequest(int mode);
int sSleepExitConfirm(HWND hwnd);
int sSleepLogoOffQuit(void);
int sSleepLogoOnQuit(void);
void sSimLogoHandleRegister(char type, const char* program, const char *cmdline, int(*entry)(int, char**));
void sLogoHandleRegister(char type, const char* program, const char *cmdline);
#else
int SleepEnterRequest(int mode);
int SleepEnterConfirm(HWND hwnd);
int SleepExitRequest(int mode);
int SleepExitConfirm(HWND hwnd);
int SleepLogoOffQuit(void);
int SleepLogoOnQuit(void);
void SimLogoHandleRegister(char type, const char* program, const char *cmdline, int(*entry)(int, char**));
void LogoHandleRegister(char type, const char* program, const char *cmdline);
#endif

#endif // _SLEEP_H
