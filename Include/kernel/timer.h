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


#ifndef _TimerH
#define _TimerH

#include <kernel/event.h>

typedef struct _TIMER
{
	LIST	Link;				// linkage on timer chain
	int		Active;				// true if active
	DWORD	Expire;				// expire time (ticks)
	DWORD	Interval;			// time interval
	EVENT	Event;				// event for this timer
	void	(*Func)(void *);	// function to call
	void	*Arg;				// function argument
}TIMER;
typedef TIMER *PTIMER;


// Macro to compare two timer counts.
// time_after() returns true if a is after b.
#define TimeAfter(a,b)		((DWORD)(a) > (DWORD)(b))
#define TimeBefore(a,b)		TimeAfter(b,a)

#define TimeAfterEq(a,b)	((DWORD)(a) >= (DWORD)(b))
#define TimeBeforeEq(a,b)	TimeAfterEq(b,a)

// Macro to convert milliseconds and tick.
#define Msec2Tick(ms)	(((ms) >= 0x20000) ? \
				 ((ms) / 1000UL) * CONFIG_HZ : ((ms) * CONFIG_HZ) / 1000UL)

#define Tick2Msec(tick)	(((tick) * 1000) / CONFIG_HZ)


void	 TimerCallout(PTIMER, void (*)(void *), void *, DWORD);
void	 TimerStop(PTIMER);
DWORD	 TimerDelay(DWORD);
void	 TimerCleanup(struct _KERNEL_THREAD*);
int	 TimerHook(void (*)(int));
void	 TimerTick(void);
DWORD	 TimerCount(void);
void	 TimerDump(char *buf, char *obj);
void	 TimerInit(void);

#if defined(STC_EXP)
int	 sTimerAlarm(DWORD, DWORD *);
int	 sTimerSleep(DWORD, DWORD *);
int	 sTimerPeriodic(HANDLE hth, DWORD, DWORD);
int	 sTimerWait(void);
#else
int	 TimerAlarm(DWORD, DWORD *);
int	 TimerSleep(DWORD, DWORD *);
int	 TimerPeriodic(HANDLE hth, DWORD, DWORD);
int	 TimerWait(void);
#endif

#endif // !_TimerH
