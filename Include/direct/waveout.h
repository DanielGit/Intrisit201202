//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	          ֱ����Ƶ���ſ��ƺ�������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DIRECT_WAVEOUT_H
#define _DIRECT_WAVEOUT_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>


#if defined(STC_EXP)
HANDLE sWaveOutOpen(int samplerate, int chs, int vol);
int sWaveOutClose(HANDLE hwo);
int sWaveOutSetVolume(HANDLE hwo, int vol);
int sWaveOutWrite(HANDLE hwo, short *buf, int size);
#else
HANDLE WaveOutOpen(int samplerate, int chs, int vol);
int WaveOutClose(HANDLE hwo);
int WaveOutSetVolume(HANDLE hwo, int vol);
int WaveOutWrite(HANDLE hwo, short *buf, int size);
#endif

#endif	// _DIRECT_SURFACE_H

