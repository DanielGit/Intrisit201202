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
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _SCHED_H
#define _SCHED_H

#include <kernel/event.h>
#include <kernel/thread.h>

// Scheduling policies defined by IEEE Std 1003.1-2001
#define SCHED_FIFO	0	// First in-first out
#define SCHED_RR	1	// Round robin
#define	SCHED_OTHER	2	// Another scheduling policy

// Scheduling quantum (Ticks for context switch)
#define QUANTUM		(CONFIG_TIME_SLICE * CONFIG_HZ / 1000)

// 延迟过程调用对象
typedef struct _DPC
{
	QUEUE	Link;				// Linkage on DPC queue
	int		State;
	void	(*Func)(void *);	// Callback routine
	void	*Arg;				// Argument to pass
}DPC;
typedef DPC *PDPC;

// DPC魔数
#define DPC_FREE	0x4470463f	// 'DpF?'
#define DPC_PENDING	0x4470503f	// 'DpP?'

#define SchedSleep(evt)	SchedTsleep((evt), 0)

int	SchedTsleep(PEVENT, DWORD);
void SchedWakeup(PEVENT);
void SchedWakeupByDestory(PEVENT evt);
PKERNEL_THREAD SchedWakeOne(PEVENT);
void SchedUnsleep(PKERNEL_THREAD, int);
void SchedYield(void);
void SchedSuspend(PKERNEL_THREAD);
void SchedResume(PKERNEL_THREAD);
void SchedTick(void);
void SchedStart(PKERNEL_THREAD);
void SchedStop(PKERNEL_THREAD);
void SchedLock(void);
void SchedUnlock(void);
int	 SchedGetPrio(PKERNEL_THREAD);
void SchedSetPrio(PKERNEL_THREAD, int, int);
int	 SchedGetPolicy(PKERNEL_THREAD);
int	 SchedSetPolicy(PKERNEL_THREAD, int);
void SchedDpc(PDPC, void (*)(void *), void *);
void SchedInit(void);

#endif // !_SCHED_H
