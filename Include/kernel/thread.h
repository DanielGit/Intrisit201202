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


#ifndef _THREAD_H
#define _THREAD_H

#include <kernel/queue.h>
#include <kernel/event.h>
#include <kernel/timer.h>
#include <kernel/sync.h>
#include <kernel/ipc.h>


// Description of a thread.
typedef struct _KERNEL_THREAD
{
	KERNEL_OBJECT Header;
	struct _KERNEL_TASK		*Task;		// pointer to owner task
	LIST 	TaskLink;					// link for threads in same task
	QUEUE 	Link;						// linkage on scheduling queue
	int		State;						// thread state
	int		Policy;						// scheduling policy
	int		Prio;						// current priority
	int		BasePrio;					// base priority
	int		TicksLeft;					// remaining ticks to run
	DWORD	TotalTicks;					// total running ticks
	int		NeedResched;				// true if rescheduling is needed
	int		LockCount;					// schedule lock counter
	DWORD	ExecBitmap;					// exception bitmap
	int		SuspendCount;				// suspend counter
	EVENT	*SleepEvent;				// sleep event
	int		SleepResult;				// sleep result
	TIMER 	Timeout;					// thread timer
	TIMER	*Periodic;					// pointer to periodic timer
	QUEUE 	IpcLink;					// linkage on IPC queue
	void	*MsgAddr;					// kernel address of IPC message
	size_t	MsgSize;					// size of IPC message
	struct _KERNEL_THREAD 	*Sender;	// thread that sends IPC message
	struct _KERNEL_THREAD 	*Receiver;	// thread that receives IPC message
	PKERNEL_PORT 	SendObj;			// IPC object sending to
	PKERNEL_PORT 	RecvObj;			// IPC object receiving from
	LIST 	Mutexes;					// mutexes locked by this thread
	struct _KERNEL_MUTEX	*WaitMutex;	// mutex pointer currently waiting
	int		Asid;						// MMU switch id
	void	*UStack;					// base address of user stack
	void	*KStack;					// base address of kernel stack
	void	*KStackTop;					// kernel stack top
	PCONTEXT 	UContext;				// usr mode context point
	KCONTEXT	KContext;				// kernel mode context
}KERNEL_THREAD;
typedef KERNEL_THREAD *PKERNEL_THREAD;

// Thread state
#define TH_RUN		0x00	// running or ready to run
#define TH_SLEEP	0x01	// awaiting an event
#define TH_SUSPEND	0x02	// suspend count is not 0
#define TH_EXIT		0x04	// terminated

// Sleep result
#define SLP_SUCCESS	0	// success
#define SLP_BREAK	1	// break due to some reasons
#define SLP_TIMEOUT	2	// timeout
#define SLP_INVAL	3	// target event becomes invalid
#define SLP_INTR	4	// interrupted by exception

// Priorities
#define PRIO_TIMER	15	// priority for timer thread
#define PRIO_IST	16	// top priority for interrupt threads
#define PRIO_DPC	33	// priority for Deferred Procedure Call
#define PRIO_GUI	64	// priority for gui msg thread
#define PRIO_IDLE	255	// priority for idle thread
#define PRIO_USER	CONFIG_USER_PRIO

#define MAX_PRIO	0
#define MIN_PRIO	255
#define NR_PRIOS	256	// number of thread priority

// Scheduling operations for Threadschedparam().
#define OP_GETPRIO	0		// get scheduling priority
#define OP_SETPRIO	1		// set scheduling priority
#define OP_GETPOLICY	2	// get scheduling policy
#define OP_SETPOLICY	3	// set scheduling policy

int	 ThreadKill(HANDLE);
void	 ThreadIdle(void);
HANDLE	 KernelThread(int, void (*)(DWORD), DWORD, int);
void	 ThreadDump(char *buf, char *obj);
void	 ThreadInit(void);
int ThreadSetAsid(int asid);
int ThreadResetAsid(void);
int ThreadSetAsidEx(HANDLE htask);

#if defined(STC_EXP)
HANDLE	 sThreadCreate(HANDLE);
int	 sThreadTerminate(HANDLE);
int sThreadLoad(HANDLE hth, void (*entry)(void), void *sp, void *stack);
HANDLE	 sThreadSelf(void);
void	 sThreadYield(void);
int	 sThreadSuspend(HANDLE);
int	 sThreadResume(HANDLE);
int	 sThreadSchedParam(HANDLE, int, int *);
#else
HANDLE	 ThreadCreate(HANDLE);
int	 ThreadTerminate(HANDLE);
int ThreadLoad(HANDLE hth, void (*entry)(void), void *sp, void *stack);
HANDLE	 ThreadSelf(void);
void	 ThreadYield(void);
int	 ThreadSuspend(HANDLE);
int	 ThreadResume(HANDLE);
int	 ThreadSchedParam(HANDLE, int, int *);
#endif

#endif // !_THREAD_H
