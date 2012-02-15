//[PROPERTY]===========================================[PROPERTY]
//			*****  诺亚神舟操作系统V2  *****
//		--------------------------------------
//				  重采样处理函数头文件   
//		--------------------------------------
//				 版权: 新诺亚舟科技
//			 ---------------------------
//				  版   本   历   史
//		--------------------------------------
//  版本	日前		说明		
//  V0.1	20010-6	  Init.			 Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef __NOAHOS_MP3_H
#define __NOAHOS_MP3_H

#include <kernel/kernel.h>

typedef int (*MP3_DATACALLBACK)(DWORD id, BYTE* buf, int size);

HANDLE Mp3DecodeCreate(void);
int Mp3DecodeDestroy(HANDLE hmp3);
int Mp3DecodeSetCallback(HANDLE hmp3, void *callback, DWORD id);
int Mp3DecodeFrame(HANDLE hmp3);
int Mp3DecodeGetPcm(HANDLE hmp3, short *buf, int *max);
int Mp3DecodeSeek(HANDLE hmp3, DWORD ms);


#endif // __NOAHOS_MP3_H

