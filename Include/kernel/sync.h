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


#ifndef _SYNC_H
#define _SYNC_H

#include <kernel/event.h>
#include <kernel/thread.h>
#include <kernel/task.h>

//�����������ͺ궨��
#define COND_OR			0
#define COND_AND		1


typedef struct _MSG_HEADER
{
	struct _KERNEL_TASK	*Task;		// id of send task
	int	Code;						// message code
	int	Status;						// return status
}MSG_HEADER;
typedef MSG_HEADER *PMSG_HEADER;

//�ź����ṹ�嶨��
typedef struct _KERNEL_SEMA
{
	KERNEL_OBJECT Header;
	struct _KERNEL_TASK	*Task;		// owner task
	EVENT	Event;					// event
	int		Value;					// current value
}KERNEL_SEMA;
typedef KERNEL_SEMA *PKERNEL_SEMA;

//�������ṹ�嶨��
typedef struct _KERNEL_MUTEX
{
	KERNEL_OBJECT Header;
	struct _KERNEL_TASK		*Task;	// owner task
	struct _KERNEL_THREAD	*Owner;	// owner thread locking this mutex
	EVENT	Event;					// event
	LIST	Link;					// linkage on locked mutex list
	int		LockCount;				// counter for recursive lock
}KERNEL_MUTEX;
typedef KERNEL_MUTEX *PKERNEL_MUTEX;

//�������ṹ�嶨��
typedef struct _KERNEL_COND
{
	KERNEL_OBJECT Header;
	struct _KERNEL_TASK		*Task;	// owner task
	struct _KERNEL_THREAD	*Owner;	// owner thread locking this mutex
	EVENT	Event;					// event
	DWORD	CondFlag;				//����ֵ
	DWORD	CondMask;				//��������
	int		CondTrig;				//�������������� COND_OR/COND_AND
}KERNEL_COND;
typedef KERNEL_COND *PKERNEL_COND;


// maximum value for semaphore.
#define MAXSEMVAL		((DWORD)((~0u) >> 1))

void MutexCleanup(struct _KERNEL_THREAD *);

HANDLE kSemaCreate(DWORD);
int kSemaDestroy(HANDLE);
int kSemaWait(HANDLE, DWORD);
int kSemaTry(HANDLE);
int kSemaRelease(HANDLE);
int kSemaValue(HANDLE);

HANDLE kMutexCreate(void);
int kMutexDestroy(HANDLE);
int kMutexWait(HANDLE);
int kMutexTry(HANDLE);
int kMutexRelease(HANDLE);

HANDLE kCondCreate(int);
int kCondDestroy(HANDLE);
int kCondWait(HANDLE, int);
int kCondRelease(HANDLE, DWORD);
int kCondBroadcast(HANDLE);

#if defined(STC_EXP)
HANDLE sSemaCreate(DWORD);
int sSemaDestroy(HANDLE);
int sSemaWait(HANDLE, DWORD);
int sSemaTry(HANDLE);
int sSemaRelease(HANDLE);
int sSemaValue(HANDLE);

HANDLE sMutexCreate(void);
int sMutexDestroy(HANDLE);
int sMutexWait(HANDLE);
int sMutexTry(HANDLE);
int sMutexRelease(HANDLE);

HANDLE sCondCreate(int);
int sCondDestroy(HANDLE);
int sCondWait(HANDLE, int);
int sCondRelease(HANDLE, DWORD);
#else
extern HANDLE SemaCreate(DWORD);
int SemaDestroy(HANDLE);
int SemaWait(HANDLE, DWORD);
int SemaTry(HANDLE);
int SemaRelease(HANDLE);
int SemaValue(HANDLE);

HANDLE MutexCreate(void);
int MutexDestroy(HANDLE);
int MutexWait(HANDLE);
int MutexTry(HANDLE);
int MutexRelease(HANDLE);

HANDLE CondCreate(int);
int CondDestroy(HANDLE);
int CondWait(HANDLE, int);
int CondRelease(HANDLE, DWORD);
#endif

#endif // !_SYNC_H
