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


#ifndef _EVENT_H
#define _EVENT_H

#include <kernel/queue.h>

// Event for sleep/wakeup
typedef struct _EVENT
{
	QUEUE	SleepQ;		// queue for waiting thread
	char	*Name;		// pointer to event name string
}EVENT;
typedef EVENT *PEVENT;

// Macro to initialize event statically
#define EVENT_INIT(event, evt_name) \
			{ {&(event).SleepQ, &(event).SleepQ}, evt_name}

// Macro to initialize event dynamically
#define EventInit(event, evt_name) \
    do { QueueInit(&(event)->SleepQ); (event)->Name = evt_name; } while (0)

#define EventWaiting(event)   (!QueueEmpty(&(event)->SleepQ))

#endif // !_EVENT_H
