//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    ���������ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _JZ4740_PLATFORM_H
#define _JZ4740_PLATFORM_H

#define NIRQS		176		// number of interrupt vectors
#define IMODE_EDGE	0		// edge trigger
#define IMODE_LEVEL	1		// level trigger

#include <arch/mips/mips.h>
#include "jz4740.h"

void MachineIdle(void);
int MachineStandby(int param);
void MachinePowerOff(int param);
unsigned long MachineRtcValue(void);
#define BreakPoint()		

#define KernArea(addr)		1
#define UserArea(addr)		1

#endif // !_JZ4740_PLATFORM_H
