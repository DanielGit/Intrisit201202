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

#ifndef _IrqH
#define _IrqH

#include <kernel/event.h>

typedef struct _IRQ
{
	KERNEL_OBJECT Header;
	int		Vector;					// vector number
	int		(*Isr)(int);			// pointer to isr
	void	(*Ist)(int);			// pointer to ist
	DWORD	Count;					// interrupt count
	int		IstRequest;				// number of ist request
	struct _KERNEL_THREAD *Thread;	// thread id of ist
	EVENT	IstEvent;				// event for ist
}IRQ;
typedef IRQ *PIRQ;

// Return values from ISR.
#define INT_DONE	0		// success
#define INT_ERROR	1		// interrupt not handled
#define INT_CONTINUE	2	// continue processing (Request IST)

// Interrupt priority levels
#define IPL_NONE	0	// nothing
#define IPL_COMM	1	// serial, parallel
#define IPL_BLOCK	2	// FDD, IDE
#define IPL_NET		3	// network
#define IPL_DISPLAY	4	// screen
#define IPL_INPUT	5	// keyboard, mouse
#define IPL_AUDIO	6	// audio
#define IPL_USB		7	// USB, PCCARD
#define IPL_RTC		8	// RTC alarm
#define IPL_PROFILE	9	// profiling timer
#define IPL_CLOCK	10	// system clock timer
#define IPL_HIGH	11	// everything

#define NIPLS		12	// number of IPLs

// Macro to map an interrupt priority level to IST priority.
#define ISTPRIO(prio)	(PRIO_IST + (IPL_HIGH - prio))

HANDLE IrqAttach(int, int, int, int (*)(int), void (*)(int));
int	 IrqDetach(HANDLE);
void	 IrqLock(void);
void	 IrqUnlock(void);
void	 IrqHandler(int);
void	 IrqDump(char *buf, char *obj);
void	 IrqInit(void);

#endif // !_IrqH
