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

#ifndef _SHARE_VAR_H
#define _SHARE_VAR_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(STC_EXP)
int sShareVarCreate(int size);
int sShareVarWrite(int offset, int data);
int sShareVarRead(int offset);
#else
int ShareVarCreate(int size);
int ShareVarWrite(int offset, int data);
int	ShareVarRead(int offset);
#endif

#endif // _SHARE_VAR_H
