//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          直接音频播放控制函数部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
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

