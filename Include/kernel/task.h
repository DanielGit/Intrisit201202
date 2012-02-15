//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ��ļ�ϵͳ  *****
//        --------------------------------------
//                    �ں����ͷ�ļ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
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
	HANDLE Handle;							//����ľ��
	char   *Name;							//���������
	int    MaxLen;							//�������Ƶ���󳤶�
	int	   Handles;							//����ľ����
	int	   Threads;							//������߳���
	DWORD  AllocSize;						//������ڴ�ռ��С
	HANDLE hMainWnd;						//�����������
	int	Class;								//��������
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
