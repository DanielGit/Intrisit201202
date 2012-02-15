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


#ifndef _JZ4760B_PLATFORM_H
#define _JZ4760B_PLATFORM_H

#define IMODE_EDGE	0		// edge trigger
#define IMODE_LEVEL	1		// level trigger

#include <arch/mips/mips.h>
#include "./jz4760b.h"

#define NIRQS		IRQ_MAX		// number of interrupt vectors

void BootGpioSet(unsigned long pin, int in);
void BootGpioOut(unsigned long pin, int data);
int BootGpioIn(unsigned long pin);
void MachineIdle(void);
int MachineStandby(int param);
void MachinePowerOff(int param);
unsigned long MachineRtcValue(void);
#define BreakPoint()		

#define KernArea(addr)		1
#define UserArea(addr)		1

#endif // !_JZ4755_PLATFORM_H
