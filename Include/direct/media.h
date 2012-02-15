//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	          ֱ����Ƶ���ſ��ƺ�������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DIRECT_MEDIA_H
#define _DIRECT_MEDIA_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(WIN32)
#include "./Win32/media.h"
#endif

#if defined(CONFIG_MCU_AK3620) && !defined(WIN32)
#include "./AK3620/Audio.h"
#include "./AK3620/Vedio.h"
#endif

#if defined(CONFIG_MCU_AK8802) && !defined(WIN32)
#include "./AK8802/media_player_lib.h"
#include "./AK8802/media.h"
#endif

#if (defined(CONFIG_MCU_JZ4750) || defined(CONFIG_MCU_JZ4750L) || defined(CONFIG_MCU_JZ4755) || defined(CONFIG_MCU_JZ4760B)) && !defined(WIN32)
#include "./Jz4750/media.h"
#include "./Jz4750/mplayer_head.h"
#include "./Jz4750/ipu_app.h"
#endif

#define INNER_POWAMP_ENABLE			// �����Ƿ�ʹ���ڲ�����
//#define DA_OPEN_ALWAYS				// �����Ƿ�һֱ��DA�豸

#define MEDIA_TYPE_AUDIO		0x010000
#define MEDIA_TYPE_VEDIO		0x020000
#define MEDIA_TYPE_VIDEO		MEDIA_TYPE_VEDIO
#define MEDIA_TYPE_NES			0x040000
#define MEDIA_TYPE_JPEG			0x080000
#define MEDIA_TYPE_FLASH		0x100000

// ��Ƶ������ϢID����
#define WM_MEDIA_OPEN				0x1100		// ý���豸��ʱ��
#define WM_MEDIA_CLOSE				0x1101		// ý���豸�ر�ʱ��
#define WM_MEDIA_TIME				0x1102		// ý���豸ʱ�䷵��
#define WM_MEDIA_SETA				0x1103		// ý���豸����A����Ϣ
#define WM_MEDIA_SETB				0x1104		// ý���豸����B����Ϣ
#define WM_MEDIA_STARTAB			0x1105		// ý���豸��ʼA-B����
#define WM_MEDIA_QUITAB				0x1106		// ý���豸�˳�A-B����
#define WM_MEDIA_SUSPEND			0x1107		// ý���豸������
#define WM_MEDIA_RESUME				0x1108		// ý���豸�˳�����
#define WM_MEDIA_TERMINATE			0x1109		// �û�����AUDIO_CTL_CLOSE���͵���Ϣ
#define WM_MEDIA_LOWBAT				0x110a		// �͵�ѹ�����������˳�
#define WM_MEDIA_VOLUME_CHANGED     0x110a		// ����ֵ��ϵͳ�ı�

// ״̬��ȡ������
#define MEDIA_CTRL_PLAY				0x0001		// ����
#define MEDIA_CTRL_PAUSE			0x0002		// ��ͣ
#define MEDIA_CTRL_RESUME			0x0003		// ����
#define MEDIA_CTRL_STOP				0x0004		// ֹͣ
#define MEDIA_CTRL_CLOSE			0x0005		// �ر�
#define MEDIA_CTRL_FORWARD			0x0006		// ���
#define MEDIA_CTRL_BACKWARD			0x0007		// ����
#define MEDIA_CTRL_SEEK				0x0008		// ��λ
#define MEDIA_CTRL_SETAB			0x0009		// ����AB��
#define MEDIA_CTRL_SETEQ			0x000A		// ���þ���
#define MEDIA_CTRL_DURATION			0x000B		// ����ʱ��
#define MEDIA_CTRL_VOLUME			0x000C		// ��������
#define MEDIA_CTRL_TEMPO			0x000D		// ���������ٶ�
#define MEDIA_CTRL_RECT				0x000E		// ��ʾ����
#define MEDIA_CTRL_ISPLAY			0x000F		// ����Ƿ񲥷�
#define MEDIA_CTRL_TOTALTIME		0x0010		// �õ���ǰ���ŵ���ʱ��
#define MEDIA_CTRL_OVOLUME_SET		0x0100		// ȫ�������������
#define MEDIA_CTRL_OVOLUME_GET		0x0200		// ȫ�����������ȡ
#define MEDIA_CTRL_IVOLUME_SET		0x0300		// ȫ��������������
#define MEDIA_CTRL_IVOLUME_GET		0x0400		// ȫ������������ȡ
#define MEDIA_CTRL_HEADPHONE_IN		0x0500		
#define MEDIA_CTRL_MIDI_DIRECT		0x0600		// MIDI��ɫ���ַ
#define MEDIA_CTRL_HEADPHONE_VOL_SET	0x0100	// ȫ�������������
#define MEDIA_CTRL_HEADPHONE_VOL_GET	0x0200	// ȫ�����������ȡ
#define MEDIA_CTRL_SPEAKER_VOL_SET		0x0101	// ȫ�������������
#define MEDIA_CTRL_SPEAKER_VOL_GET		0x0201	// ȫ�����������ȡ

// ��ý�岥�����ݻ�ȡ/д��ص�����
typedef int (*MEIDA_CALLBACK)(DWORD, int, int, int);
// �������أ���д���ֽ���
// HANDLE hmedia: ý����
// int bRead: 1-������ 0-д����
// int offset: ��д���ݻ�����ƫ��
// int count: ��д�����ֽ���

typedef HANDLE (*MEDIA_FUNC_CREATE)(void*);
typedef int (*MEDIA_FUNC_DESTROY)(HANDLE);
typedef int (*MEDIA_FUNC_OPEN)(HANDLE);
typedef int (*MEDIA_FUNC_CLOSE)(HANDLE, int);
typedef int (*MEDIA_FUNC_CTRL)(HANDLE, int, DWORD*, int*);
typedef int (*MEDIA_FUNC_INFO)(void*, void*, int);

typedef struct _MEDIA_CALLBACK
{
	MEDIA_FUNC_CREATE MediaCreate;
	MEDIA_FUNC_DESTROY MediaDestroy;
	MEDIA_FUNC_OPEN MediaOpen;
	MEDIA_FUNC_CLOSE MediaClose;
	MEDIA_FUNC_CTRL MediaCtrl;
}MEDIA_CALLBACK;
typedef MEDIA_CALLBACK *PMEDIA_CALLBACK;

typedef struct _MEDIA_OBJECT
{
	KERNEL_OBJECT Header;		// �ں˶�����Ϣ
	LIST Link;
	HANDLE hTask;
	int Type;					// ��ý����������(��Ƶ����Ƶ����Ϸ��Flash...)
	int Mode;					// ����ģʽ
	void *Media;
	char *MediaInfo;
	DWORD Device;				// ʹ���豸
	MEDIA_CALLBACK Cb;
}MEDIA_OBJECT;
typedef MEDIA_OBJECT *PMEDIA_OBJECT;

typedef struct _MEDIA_CBPARAM
{
	int bRead;
	int Offset;
	int Count;
	int Return;
}MEDIA_CBPARAM;
typedef MEDIA_CBPARAM *PMEDIA_CBPARAM;

#define MEDIA_DEVICE_AUDIO		(1<<0)		// ��Ƶ�豸
#define MEDIA_DEVICE_VEDIO		(1<<1)		// ��Ƶ��ͼ���豸
#define MEDIA_DEVICE_VIDEO		MEDIA_DEVICE_VEDIO		// ��Ƶ��ͼ���豸

// ��Ƶ������ռģʽ����
#define MEDIA_MODE_NORMAL			0x0000		// ��ͨ����ģʽ
#define MEDIA_MODE_WAIT				0x0001		// �ȴ�����
#define MEDIA_MODE_PREEMPTIVE		0x0002		// ��ռʽ����ģʽ

void MediaDeviceInit(void);
int MediaDeviceLock(PMEDIA_OBJECT obj);
int MediaDeviceUnlock(PMEDIA_OBJECT obj);
int MediaDeviceCreate(DWORD device);
PMEDIA_OBJECT MediaDevicePoll(DWORD device);

void MediaSrvInit(void);
void MediaSrvLock(void);
void MediaSrvUnlock(void);
int MediaSrvPowerOff(int mode);
int MediaSrvMode(void);
int MediaSrvRegistCallback(int type, DWORD device, PMEDIA_CALLBACK callback);
int MediaSrvDestroyNotify(void *media);

#if defined(STC_EXP)
HANDLE sMediaSrvCreate(void* media, char *name, int type, int preempt);
int sMediaSrvDestroy(HANDLE hmedia);
int sMediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size);
int sMediaSrvGetName(HANDLE hmedia, char *name, int max);
int sMediaSrvInfo(void *media, void *info, int type);
int sMediaSrvMaster(char *name, int max, int type);
#else
HANDLE MediaSrvCreate(void* media, char *name, int type, int preempt);
int MediaSrvDestroy(HANDLE hmedia);
int MediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size);
int MediaSrvGetName(HANDLE hmedia, char *name, int max);
int MediaSrvInfo(void *media, void *info, int type);
int MediaSrvMaster(char *name, int max, int type);
#endif

#endif	// _DIRECT_MEDIA_H

