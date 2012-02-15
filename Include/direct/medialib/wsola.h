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


#ifndef __WSOLA_H__
#define __WSOLA_H__

#include <kernel/kernel.h>

typedef struct _AUDIO_FILTER
{
	short *iBuf;
	short *oBuf;
	int iSize;
	int oSize;
}AUDIO_FILTER;
typedef AUDIO_FILTER *PAUDIO_FILTER;


// Disable Hanning window to conserve memory.
#define WSOLA_OPTION_NO_HANNING	1

// Specify that the WSOLA will not be used for PLC.
#define WSOLA_OPTION_NO_PLC		2

// Specify that the WSOLA will not be used to discard frames in non-contiguous buffer.
#define WSOLA_OPTION_NO_DISCARD	4

// Disable fade-in and fade-out feature in the transition between
// actual and synthetic frames in WSOLA. With fade feature enabled, 
// WSOLA will only generate a limited number of synthetic frames 
// (configurable with #Wsolaset_max_expand()), fading out 
// the volume on every more samples it generates, and when it reaches
// the limit it will only generate silence.
#define WSOLA_OPTION_NO_FADING	8
    
#define WSOLA_OPTION_LITE		16
    
#define WSOLA_OPTION_LINER_WIN	32


HANDLE WsolaCreate(DWORD sample_rate, DWORD channel_count, DWORD options, int rate);
int WsolaDestroy(HANDLE hwsola);
int WsolaConvert(HANDLE hwsola, PAUDIO_FILTER filter);


#endif	// __WSOLA_H__

