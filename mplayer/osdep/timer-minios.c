// Precise timer routines for LINUX  (C) LGB & A'rpi/ASTRAL
#include "mplaylib.h"
#include "mplayertm.h"
#include "config.h"

const char *timer_name =  "sleep minios";
extern void BUFF_TimeDly(unsigned int tm);
extern int sthread_id;
int usec_sleep(int usec_delay)
{
	BUFF_TimeDly(usec_delay);
	return 0; 
}

// Returns current time in microseconds
unsigned int GetTimer(void){
	unsigned int dm[2];
	JZ_GetTimer_20ms(dm);
  return (dm[1]*20000+dm[0]);
}  

// Returns current time in milliseconds
unsigned int GetTimerMS(void){
	unsigned int dm[2];
	JZ_GetTimer_20ms(dm);
  return (dm[1]*20 + dm[0]/1000);
}  

static unsigned int RelativeTime = 0;

// Returns time spent between now and last call in seconds
float GetRelativeTime(void){
  unsigned int t,r;
  t= GetTimer();
  r = t - RelativeTime;
  RelativeTime = t;
  return (float)r* 0.000001F;
}

// Initialize timer, must be called at least once at start
void InitTimer(void){
	JZ_StartTimer();
	
  GetRelativeTime();
}


