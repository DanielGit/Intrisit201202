//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//       	        时间日期处理函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-9      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>

#ifndef _DT_H
#define _DT_H

// 日期结构定义
typedef struct
{
	WORD year;		// 取值范围1900 ~ 2100
	BYTE month;		// 取值范围1~12
	BYTE day;		// 取值范围1~31
}DATE;
typedef DATE *PDATE;

// 时间结构
typedef struct
{
	BYTE hour;		// 取值范围0~23
	BYTE minute;	// 取值范围0~59
	BYTE second;	// 取值范围0~59
}TIME;
typedef TIME *PTIME;

// 闹铃时间结构
typedef struct
{
	WORD year;		// 取值范围1900 ~ 2100
	BYTE month;		// 取值范围1~12
	BYTE day;		// 取值范围1~31
	BYTE hour;		// 取值范围0~23
	BYTE minute;	// 取值范围0~59
}ALARM_DT;
typedef ALARM_DT *PALARM_DT;


#if defined(STC_EXP)
int sRtcGetDate(DATE *date);
int sRtcGetTime(TIME *time);
int sRtcGetAlarm(ALARM_DT *alarm);
int sRtcSetDate(DATE date);
int sRtcSetTime(TIME time);
int sRtcSetAlarm(ALARM_DT alarm);
int sRtcAlarmEna(void);
int sRtcAlarmDis(void);
int sRtcAlarmDrvEna(void);
int sRtcAlarmDrvDis(void);
#else
int RtcGetDate(DATE *date);
int RtcGetTime(TIME *time);
int RtcGetAlarm(ALARM_DT *alarm);
int RtcSetDate(DATE date);
int RtcSetTime(TIME time);
int RtcSetAlarm(ALARM_DT alarm);
int RtcAlarmDrvEna(void);
int RtcAlarmDrvDis(void);
#endif

#endif
