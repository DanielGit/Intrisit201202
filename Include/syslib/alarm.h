//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//         	       闹铃管理的头文件 
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _ALARM_MAN_H
#define _ALARM_MAN_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

typedef struct _ALARM
{
	int	  Enable;		// 闹铃允许1/禁止0
	DWORD Year;			// 取值范围1900 ~ 2100, 其他值表示每年提醒
	DWORD Month;		// Bit0～Bit11有效，分别表示1～12月
	DWORD Day;			// Bit0～Bit30有效，分别表示1～31号
	DWORD Week;			// Bit0～Bit6有效，分别表示星期天、星期一至星期六
	DWORD Hour;			// Bit0～Bit23有效，分别表示0～23时
	DWORD Minute0;		// Bit0～Bit29有效，分别表示0～29分
	DWORD Minute1;		// Bit0～Bit29有效，分别表示30～59分
}ALARM;
typedef ALARM *PALARM;

int AlarmInit(void);
int AlarmReload(int isr);
void AlarmAppEnable(void);
void AlarmAppDisable(void);
int AlarmAppIsEnable(void);
void AlarmReloadBeforeDtChange(void);
void AlarmReloadAfterDtChange(void);

#if defined(STC_EXP)
HANDLE sAlarmCreate(const char *name, const char *wakeapp);
int sAlarmDestroy(HANDLE alarm, const char *name);
int sAlarmRead(HANDLE alarm, const char *name, PALARM time);
int sAlarmWrite(HANDLE alarm, const char *name, PALARM time);
#else
HANDLE AlarmCreate(const char *name, const char *wakeapp);
int AlarmDestroy(HANDLE alarm, const char *name);
int AlarmRead(HANDLE alarm, const char *name, PALARM time);
int AlarmWrite(HANDLE alarm, const char *name, PALARM time);
#endif

#endif // _ALARM_MAN_H
