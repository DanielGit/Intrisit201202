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
