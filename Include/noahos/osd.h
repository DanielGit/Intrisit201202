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

HANDLE OsdOpen(RECT *r, DWORD *pallete, BYTE alpha);
int OsdClose(HANDLE hosd);
int OsdSet(HANDLE hosd, RECT *r, DWORD *pallete, BYTE alpha);
int OsdWrite(HANDLE hosd, void *buf, int size);

#endif	// _DIRECT_IMAGE_H

