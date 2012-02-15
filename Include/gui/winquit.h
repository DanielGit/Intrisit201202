//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨GUI  *****
//        --------------------------------------
//       	            GUIͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
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

