#ifndef _AUTOBOT_H
#define _AUTOBOT_H

#include <config.h>
#include <sys/types.h>

struct message_key_t
{
	unsigned short msg;
	unsigned short size;
	unsigned long event;
	unsigned long key;
};

struct message_pen_t
{
	unsigned short msg;
	unsigned short size;
	unsigned long event;
	unsigned long x;
	unsigned long y;
};

#define MESSAGE_KEY		1
#define MESSAGE_PEN		2

#define AUTO_TEST_IDLE		0
#define AUTO_TEST_PLAY		1
#define AUTO_TEST_RECORD	2
#define AUTO_TEST_ABORT		4

void AutoTestPlayStart(void);
void AutoTestPlayEnd(void);
void AutoTestRecordStart(void);
void AutoTestRecordEnd(void);
int AutoTestGetStatus(void);
void AutoTestSetStatus(int status);
int AutoTestTimeOut(unsigned long stamp);
unsigned long AutoTestGetTime(void);
void AutoTestTick(void);
void AutoTestAbortNotify(void);

#if defined(STC_EXP)
int sATRecordStart(char *path);
int sATRecordEnd(void);
int sATPlayEnd(void);
int sATPlayStart(char *path);
int sATGetRepeatCounter(void);
int sATWriteMessage(void *mbuf, unsigned long stamp);
int sATReadMessage(void *mbuf, unsigned long *stamp);
#else
int ATRecordStart(char *path);
int ATRecordEnd(void);
int ATPlayEnd(void);
int ATPlayStart(char *path);
int ATGetRepeatCounter(void);
int ATWriteMessage(void *mbuf, unsigned long stamp);
int ATReadMessage(void *mbuf, unsigned long *stamp);
#endif

#endif
