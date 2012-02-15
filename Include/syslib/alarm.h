//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//         	       ��������ͷ�ļ� 
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2005-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _ALARM_MAN_H
#define _ALARM_MAN_H

#include <kernel/kernel.h>
#include <gui/wintype.h>

typedef struct _ALARM
{
	int	  Enable;		// ��������1/��ֹ0
	DWORD Year;			// ȡֵ��Χ1900 ~ 2100, ����ֵ��ʾÿ������
	DWORD Month;		// Bit0��Bit11��Ч���ֱ��ʾ1��12��
	DWORD Day;			// Bit0��Bit30��Ч���ֱ��ʾ1��31��
	DWORD Week;			// Bit0��Bit6��Ч���ֱ��ʾ�����졢����һ��������
	DWORD Hour;			// Bit0��Bit23��Ч���ֱ��ʾ0��23ʱ
	DWORD Minute0;		// Bit0��Bit29��Ч���ֱ��ʾ0��29��
	DWORD Minute1;		// Bit0��Bit29��Ч���ֱ��ʾ30��59��
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
