//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	              触摸屏矫正算法
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2009-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef __TOUCH_CALIBRATE_H__
#define __TOUCH_CALIBRATE_H__

#include <kernel/kernel.h>
#include <kernel/device.h>
typedef struct
{
	int An;               /* A = An/Divider */
	int Bn;               /* B = Bn/Divider */
	int Cn;               /* C = Cn/Divider */
	int Dn;               /* D = Dn/Divider */
	int En;               /* E = En/Divider */
	int Fn;               /* F = Fn/Divider */
	int Divider;
}TOUCH_MATRIX;
typedef TOUCH_MATRIX *PTOUCH_MATRIX;

// 3点矫正算法
int TouchCalibrateSetMatrix(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix);
int TouchCalibrateGetPoint(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix);

// 5点矫正算法
int TouchCalibrateMatrix(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix, int Point);
int TouchCalibrateGetPointEx(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix);

#endif  // __TOUCH_CALIBRATE_H__ 
