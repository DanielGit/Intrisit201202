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

#ifndef _DIRECT_OSD_H
#define _DIRECT_OSD_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

int OsdInit(void);

#if defined(STC_EXP)
HANDLE sOsdOpen(RECT *r, DWORD *pallete, int alpha);
int sOsdClose(HANDLE hosd);
int sOsdSet(HANDLE hosd, RECT *r, DWORD *pallete, int alpha);
int sOsdWrite(HANDLE hosd, void *buf, int size);
#else
HANDLE OsdOpen(RECT *r, DWORD *pallete, int alpha);
int OsdClose(HANDLE hosd);
int OsdSet(HANDLE hosd, RECT *r, DWORD *pallete, int alpha);
int OsdWrite(HANDLE hosd, void *buf, int size);
#endif

#endif	// _DIRECT_IMAGE_H

