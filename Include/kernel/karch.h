//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典文件系统  *****
//        --------------------------------------
//                    内核相关头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2008-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <arch/arch.h>

void CpuReset(void);
void CpuInit(void);
void CpuSwitch(PKCONTEXT, PKCONTEXT);
void CpuTrap(int, PCONTEXT);

void InterruptMask(int);
void InterruptUnmask(int, int);
void InterruptSetup(int, int);
void InterruptInit(void);
void InterruptDisable(void);
void InterruptEnable(void);
void InterruptSave(DWORD *sts);
void InterruptRestore(DWORD sts);

void ContextInit(PKERNEL_THREAD);
void ContextSet(PKERNEL_THREAD, int, DWORD);
void ContextSwitch(PKERNEL_THREAD, PKERNEL_THREAD);
void ContextSave(PCONTEXT ctx, int exc);
void ContextRestore(PCONTEXT ctx, void *regs);

void UsrThreadEntry(void);
void KernelThreadEntry(void);

int UmemCopyin(void *, void *, size_t);
int UmemCopyout(void *, void *, size_t);
int UmemStrnlen(const char *, size_t, size_t *);

#endif // !_CONTEXT_H
