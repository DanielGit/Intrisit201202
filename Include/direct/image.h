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

#ifndef _DIRECT_IMAGE_H
#define _DIRECT_IMAGE_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(CONFIG_MCU_AK3620) || defined(WIN32)
#include "./AK3620/image_api.h"
#endif

void ImageCbSetCallback(void);

#endif	// _DIRECT_IMAGE_H

