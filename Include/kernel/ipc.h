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

#ifndef _IPC_H
#define _IPC_H

#include <kernel/queue.h>

typedef struct _KERNEL_PORT
{
	KERNEL_OBJECT Header;
	char	Name[CONFIG_MAX_PORTNAME];		// object name
	LIST	NameLink;						// list for name hash table
	LIST	TaskLink;						// link all objects in same task
	struct _KERNEL_TASK *Owner;				// owner task of this object
	QUEUE	SendQ;							// queue for sender threads
	QUEUE	RecvQ;							// queue for receiver threads
}KERNEL_PORT;
typedef KERNEL_PORT *PKERNEL_PORT;

// Message header
typedef struct _MESSAGE_HEADER
{
	struct _KERNEL_TASK		*Task;	// id of a sender task
	int		Code;					// message code
	int		Status;					// return status
}MESSAGE_HEADER;
typedef MESSAGE_HEADER *PMESSAGE_HEADER;

void PortInit(void);
void PortDump(char *buf, char *obj);

void MessageCleanup(struct _KERNEL_THREAD*);
void MessageCancel(PKERNEL_PORT);
void MessageInit(void);

#if defined(STC_EXP)
HANDLE sPortCreate(const char *);
HANDLE sPortLookup(const char *);
int	 sPortDestroy(HANDLE);
int	 sMessageSend(HANDLE, void *, size_t);
int	 sMessageReceive(HANDLE, void *, size_t);
int	 sMessageReply(HANDLE, void *, size_t);
#else
HANDLE PortCreate(const char *);
HANDLE PortLookup(const char *);
int	 PortDestroy(HANDLE);
int	 MessageSend(HANDLE, void *, size_t);
int	 MessageReceive(HANDLE, void *, size_t);
int	 MessageReply(HANDLE, void *, size_t);
#endif

#endif // !_IPC_H
