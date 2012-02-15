//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	              �����������㷨
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
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

// 3������㷨
int TouchCalibrateSetMatrix(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix);
int TouchCalibrateGetPoint(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix);

// 5������㷨
int TouchCalibrateMatrix(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix, int Point);
int TouchCalibrateGetPointEx(PTOUCH_POINT lcd, PTOUCH_POINT touch, PTOUCH_MATRIX matrix);

#endif  // __TOUCH_CALIBRATE_H__ 
