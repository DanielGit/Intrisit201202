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


#ifndef _AK3620_PLATFORM_H
#define _AK3620_PLATFORM_H

#include <arch/arch.h>
#include "anyka_cpu.h"
#include "nand_list.h"

#define NIRQS		48		// number of interrupt vectors
#define IMODE_EDGE	0		// edge trigger
#define IMODE_LEVEL	1		// level trigger

void MachineIdle(void);
int MachineStandby(int param);
void MachinePowerOff(int param);
DWORD MachineRtcValue(void);
#define BreakPoint()		
#define KernArea(addr)		1
#define UserArea(addr)		1

DWORD MMU_GetProcessId(void);
void MMU_SetProcessId(DWORD pid);
void MMU_SetITLBLockdown(DWORD baseVictim);
void MMU_SetDTLBLockdown(DWORD baseVictim);
void MMU_SetICacheLockdownBase(DWORD base);
void MMU_SetDCacheLockdownBase(DWORD base);
void MMU_InvalidateDTLBMVA(DWORD mva);
void MMU_InvalidateDTLB(void);
void MMU_InvalidateITLBMVA(DWORD mva);
void MMU_InvalidateITLB(void);
void MMU_InvalidateTLB(void);
void MMU_WaitForInterrupt(void);
void MMU_CleanInvalidateDCacheIndex(DWORD index);
void MMU_CleanDCacheIndex(DWORD index);
void MMU_CleanInvalidateDCacheMVA(DWORD mva);
void MMU_CleanDCacheMVA(DWORD mva);
void MMU_InvalidateDCacheMVA(DWORD mva);
void MMU_InvalidateDCache(void);
void MMU_PrefetchICacheMVA(DWORD mva);
void MMU_InvalidateICacheMVA(DWORD mva);
void MMU_InvalidateICache(void);
void MMU_InvalidateIDCache(void);
void MMU_SetDomain(int domain);
void MMU_SetTTBase(int base);
void MMU_DisableMMU(void);
void MMU_EnableMMU(void);
void MMU_DisableAlignFault(void);
void MMU_EnableAlignFault(void);
void MMU_DisableWriteBuffer(void);
void MMU_EnableWriteBuffer(void);
void MMU_DisableDCache(void);
void MMU_EnableDCache(void);
void MMU_DisableICache(void);
void MMU_EnableICache(void);

void BootGpioSet(DWORD pin, int in);
void BootGpioOut(DWORD pin, int data);
int BootGpioIn(DWORD pin);

#endif // !_AK3620_PLATFORM_H
