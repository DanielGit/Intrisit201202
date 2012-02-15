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


#ifndef __RESAMPLE_H
#define __RESAMPLE_H

#include <kernel/kernel.h>

typedef struct _AUDIO_FILTER
{
	short *iBuf;
	short *oBuf;
	int iSize;
	int oSize;
}AUDIO_FILTER;
typedef AUDIO_FILTER *PAUDIO_FILTER;

int Resample(void *handle, PAUDIO_FILTER filter);
void *ResampleCreate(int chs, int irate, int orate, int mode);
void ResampleDestroy(void *handle);

#endif // __RESAMPLE_H
