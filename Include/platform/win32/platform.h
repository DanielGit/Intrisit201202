//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    处理器相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _PC_PLATFORM_H
#define _PC_PLATFORM_H

#include <arch/arch.h>

#define NIRQS		16		// number of interrupt vectors
#define IMODE_EDGE	0		// edge trigger
#define IMODE_LEVEL	1		// level trigger

void SimCpuSleep(unsigned long ms);
int SimMachineStandby(int param);
void SimMachinePowerOff(int param);
DWORD SimRtcValue(void);
volatile int bIntEnable;

#define MachinePowerOff(a)	SimMachinePowerOff(a)
#define MachineIdle()		SimCpuSleep(1)
#define MachineStandby(a)	SimMachineStandby(a)
#define MachineRtcValue()	SimRtcValue()
#define MachineStandbyWakeup()		SimStandbyWakeup()
#define MachineGetStandbyWakeup()	SimGetStandbyWakeup()
#define MachineClrStandbyWakeup()	SimClrStandbyWakeup()
#define BreakPoint()		

#define KernArea(addr)		1
#define UserArea(addr)		1

#endif // !_PC_PLATFORM_H
