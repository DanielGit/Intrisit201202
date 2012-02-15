//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	            ��Ƶ��ѹ������������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2009-6      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _AVI_LCD_PORT_H__
#define _AVI_LCD_PORT_H__

#include <platform/platform.h>
#include <gui/wintype.h>

typedef enum
{
    CAMSHOW_NONE = 0,
    CAMSHOW_LEFTTOP,
    CAMSHOW_RIGHTTOP,
    CAMSHOW_LEFTBOTTOM,
    CAMSHOW_RIGHTBOTTOM,
    CAMSHOW_CENTER,
    CAMSHOW_MODE_MAX
} T_eCAM_MODE;          //Cam Picture Position in frame

typedef struct
{
    T_eCAM_MODE camShow;
 //   T_RECT camRect;
} T_CAM_INFO;           //Cam Show related Info


void VD_LCD_SetCamInfo(T_CAM_INFO *camInfo);

//void VD_LCD_SetPaintInfo(PAINTINFO *paintInfo);

//xiong void VD_LCD_Recover(void);

void  VD_TOOLBAR_Show(T_BOOL flag);

//xiong void VD_Disp_ShowFrame(T_U8 *srcImg, T_S16 src_width, T_S16 src_height);

#endif
