//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//       	        ʱ�����ڴ�����
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2005-9      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>

#ifndef _DT_H
#define _DT_H

// ���ڽṹ����
typedef struct
{
	WORD year;		// ȡֵ��Χ1900 ~ 2100
	BYTE month;		// ȡֵ��Χ1~12
	BYTE day;		// ȡֵ��Χ1~31
}DATE;
typedef DATE *PDATE;

// ʱ��ṹ
typedef struct
{
	BYTE hour;		// ȡֵ��Χ0~23
	BYTE minute;	// ȡֵ��Χ0~59
	BYTE second;	// ȡֵ��Χ0~59
}TIME;
typedef TIME *PTIME;

// ����ʱ��ṹ
typedef struct
{
	WORD year;		// ȡֵ��Χ1900 ~ 2100
	BYTE month;		// ȡֵ��Χ1~12
	BYTE day;		// ȡֵ��Χ1~31
	BYTE hour;		// ȡֵ��Χ0~23
	BYTE minute;	// ȡֵ��Χ0~59
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
