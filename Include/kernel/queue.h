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
//  V0.1    2007-4      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#ifndef _QUEUE_H
#define _QUEUE_H

typedef struct _QUEUE
{
	struct _QUEUE *Next;
	struct _QUEUE *Prev;
}QUEUE;
typedef QUEUE *PQUEUE;

#define QueueInit(head)		((head)->Next = (head)->Prev = (head))
#define QueueEmpty(head)	((head)->Next == (head))
#define QueueNext(q)		((q)->Next)
#define QueuePrev(q)		((q)->Prev)
#define QueueFirst(head)	((head)->Next)
#define QueueLast(head)		((head)->Prev)
#define QueueEnd(head,q)	((q) == (head))
#define QueueEntry(q, type, member) \
    ((type *)((char *)(q) - (unsigned long)(&((type *)0)->member)))

void	 EnQueue(PQUEUE, PQUEUE);
PQUEUE	 DeQueue(PQUEUE);
void	 QueueInsert(PQUEUE, PQUEUE);
void	 QueueRemove(PQUEUE);

#endif // !_QUEUE_H

