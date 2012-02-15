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

#ifndef _WIN_QUIT_H
#define _WIN_QUIT_H

typedef struct _QUIT_OBJ
{
	LIST Link;
	HWND hRequest;
	HWND hQuit;
}QUIT_OBJ;
typedef QUIT_OBJ *PQUIT_OBJ;

void WinQuitInit(void);
int WinQuitRequest(HWND hquit, HWND hrequest, int data);
int WinQuitAnswerWait(HWND hwnd, int data);
int WinQuitAnswerQuit(HWND hwnd);
int WinQuitAnswerCancel(HWND hwnd, int data);

#endif // _WIN_QUIT_H

