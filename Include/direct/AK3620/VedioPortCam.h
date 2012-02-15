//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	            视频解压缩库依赖函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2009-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef     __AVI_CAM_PORT__
#define __AVI_CAM_PORT__

#include <platform/platform.h>

void VD_Cam_CapYUV(T_U8 *dstY,T_U8 *dstU,T_U8 *dstV,  T_S32 dstWidth, T_S32 dstHeight);

void VD_Cam_CapStart(void);

T_U8 *VD_Cam_CapGetData(void);

T_BOOL VD_Cam_CapComplete(void);

void VD_Cam_ShowFrame(T_U8 *srcImg, T_S16 src_width, T_S16 src_height);

#endif

