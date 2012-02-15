//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//         	        
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2005-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _AP_MAN_H
#define _AP_MAN_H

#include <kernel/kernel.h>
int ApManInit(void);

#if defined(STC_EXP)
int sApManGetCreate(char *name, int max);
int sApManGetDestroy(char *name, int max);
#else
int ApManGetCreate(char *name, int max);
int ApManGetDestroy(char *name, int max);
#endif

#endif // _AP_MAN_H
