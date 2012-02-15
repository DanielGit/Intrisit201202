// Precise timer routines for LINUX  (C) LGB & A'rpi/ASTRAL



#include "mplaylib.h"
#include "mplayertm.h"
#include "config.h"

const char *timer_name =  "sleep minios";
extern void BUFF_TimeDly(unsigned int tm);
/*
	 depend jz4750->time.c 
*/
#define DIV_TIMER  3

extern int sthread_id;
int usec_sleep(int usec_delay)
{

//	F("%d\n",usec_delay);
	//if(usec_delay < 1000) usec_delay = 1000;
	BUFF_TimeDly(usec_delay);
	
}

// Returns current time in microseconds
unsigned int GetTimer(void){
	unsigned int dm;
	dm = Get_PerformanceCounter();
  return (dm / DIV_TIMER);
}  

// Returns current time in milliseconds
unsigned int GetTimerMS(void){
	unsigned int dm;
	dm = Get_PerformanceCounter();
  return (dm/(1000 * DIV_TIMER));
}  

static unsigned int RelativeTime = 0;

// Returns time spent between now and last call in seconds
float GetRelativeTime(void){
  unsigned int t,r;
  t= Get_PerformanceCounter();
  //F("%d\n",t);
 
  r = t - RelativeTime;
  RelativeTime = t; 
  if(r > 0x7fffffff)
  	r = 0xffffffff - r;
  r = r / DIV_TIMER;
  //F("%s %d\n",__FUNCTION__,r);
  return (float)r* 0.000001;
}

// Initialize timer, must be called at least once at start
void InitTimer(void){
  Init_PerformanceCounter();
  GetRelativeTime();
	//F("%s\n",__FUNCTION__);
}


#if 0
void timer_main(){
  float t=0;
  int t1;
  JZ_StartTimer();
  InitTimer();
  t1 = GetTimerMS();
  while(1){ 
  	GetTimer();
    
  	t+=GetRelativeTime();
  	os_TimeDelay(100);
  	t1 = GetTimerMS();
  	printf("time= %d %d\r\n",(int)t,t1); 
  }
}
#endif

