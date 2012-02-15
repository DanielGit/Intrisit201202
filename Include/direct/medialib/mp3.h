//[PROPERTY]===========================================[PROPERTY]
//			*****  ŵ�����۲���ϵͳV2  *****
//		--------------------------------------
//				  �ز���������ͷ�ļ�   
//		--------------------------------------
//				 ��Ȩ: ��ŵ���ۿƼ�
//			 ---------------------------
//				  ��   ��   ��   ʷ
//		--------------------------------------
//  �汾	��ǰ		˵��		
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

