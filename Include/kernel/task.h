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


#ifndef _TaskH
#define _TaskH

#include <kernel/timer.h>
#include <kernel/vmem.h>

// Task struct
typedef struct _KERNEL_TASK
{
	KERNEL_OBJECT Header;
	char    *Path;							// task path
	char	*Name;							// task elf name path
	DWORD	NameHash;						// task name hash
	int		MultiInst;						// task multi-instance enable or disable
	int		MsgFlag;						// flag whether the task accept system's message
	LIST	Link;							// link for all tasks in system
	LIST	Ports;							// ports owned by this task
	LIST	Threads;						// threads in this task
	int		SuspendCount;					// suspend counter
	TIMER	Alarm;							// alarm timer
	void	(*ExecHandler)(int, void*);		// pointer to excepion handler
	struct _KERNEL_TASK	*Parent;			// parent task
	void 	*UPool;							// user memory pool
	int		Asid;							// MMU swith id
	HANDLE	MainWnd;						// main window handle bound to task
	int		Class;							// class of task
}KERNEL_TASK;
typedef KERNEL_TASK *PKERNEL_TASK;

typedef struct _TASK_INFO
{
	HANDLE Handle;							//任务的句柄
	char   *Name;							//任务的名称
	int    MaxLen;							//任务名称的最大长度
	int	   Handles;							//任务的句柄数
	int	   Threads;							//任务的线程数
	DWORD  AllocSize;						//申请的内存空间大小
	HANDLE hMainWnd;						//任务的主窗体
	int	Class;								//任务的类别
}TASK_INFO;
typedef TASK_INFO *PTASK_INFO;

#define CurTask()	  (gCurThread->Task)

#define VM_NEW		0	// Create new memory map
#define VM_SHARE	1	// Share parent's memory map
#define VM_COPY		2	// Duplicate parent's memory map

int	 TaskAccess(PKERNEL_TASK);
int	 TaskBootstrap(DWORD vm_start);
void	 TaskDump(char *buf, char *obj);
void BootDump(char *buf, char *obj);
void	 TaskInit(void);
int TaskSetMainWnd(HANDLE hwnd);

#if defined(STC_EXP)
HANDLE	 sTaskCreate(HANDLE, int, DWORD, const char*);
int	 sTaskTerminate(HANDLE);
HANDLE	 sTaskSelf(void);
int	 sTaskSuspend(HANDLE);
int	 sTaskResume(HANDLE);
int  sTaskMultiInst(int multi);
int  sTaskInfo(PTASK_INFO info, int max);
HANDLE sTaskGetMainWnd(HANDLE htask);
int sTaskGetNameEx(char *name, int max, HANDLE htask);
int sTaskGetName(char *name, int max);
int sTaskSetClass(int class);
#else
HANDLE	 TaskCreate(HANDLE, int, DWORD, const char *);
int	 TaskTerminate(HANDLE);
HANDLE	 TaskSelf(void);
int	 TaskSuspend(HANDLE);
int	 TaskResume(HANDLE);
int  TaskMultiInst(int multi);
int  TaskInfo(PTASK_INFO info, int max);
HANDLE TaskGetMainWnd(HANDLE htask)
int TaskGetNameEx(char *name, int max, HANDLE htask);
int TaskGetName(char *name, int max);
int TaskSetClass(int class);
#endif


#endif // !_TaskH
