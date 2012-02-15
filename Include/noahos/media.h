//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//       	        �������������
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2006-3      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <config.h>
#include <noahos.h>
#include <libc/stdlib.h>
#include <noahos/audio.h>
#include <noahos/vedio.h>


#ifndef _MEDIA_H
#define _MEDIA_H

typedef struct _MEDIA_INFO
{
	int MediaType;
	char bHasAudio;
	char bHasVideo;
	char bAllowSeek;
	DWORD TotalTime;
	
	int AudioCodec;
	int AudioBitrate;
	int AudioSamplerate;
	int AudioChannels;

	int VideoCodec;
	int VideoWidth;
	int VideoHeight;
	int VideoFps;
	int VideoBitrate;
}MEDIA_INFO;
typedef MEDIA_INFO *PMEDIA_INFO;


typedef struct _MEDIA_TASK
{
	DWORD Version;			// �汾
	HWND hWnd;				// ��Ƶ��Ϣ���ʹ���
	int Record;				// ��Ƶ���� 0-���� 1-¼��
	int MediaType;			// ��Ƶý������
	
	char *FileName;			// ��Ƶ�ļ�����
	BYTE *DataBuf;			// ��Ƶ���ݻ�����
	int BufSize;			// ��Ƶ��������С
	DWORD PlayOffset;		// ��Ƶ������ʼƫ��
	DWORD PlayLength;		// ��Ƶ�������ݳ���
	HANDLE hCallback;		// �ص��������
	
	RECT ShowRect;			// ��Ƶ��ʾ����
	int Volume;				// ��Ƶ¼������
	int Tempo;				// ��Ƶ�����ٶ�
	int Eq;					// ��Ƶ����EQģʽ
	int Duration;			// ¼��ʱ��
	int Position;			// ����ʱ��
	int Degree;				// ��Ƶ��ʾ�Ƕ�

	DWORD ABitrate;			// ��Ƶ¼�Ʋ�����
	DWORD ASamplerate;		// ��Ƶ¼�Ʋ�����
	DWORD AChannels;		// ��Ƶ¼��������
	DWORD VBitrate;			// ��Ƶ¼�Ʋ�����
	DWORD Device;			// ¼���豸���壺MIC/LINE-IN
	
	int ExterdType;			// ������������
	int ExtendSize;			// �������ݴ�С
	void *Extend;			// ��������
}MEDIA_TASK;
typedef MEDIA_TASK *PMEDIA_TASK;

#define MEDIA_TYPE_AUDIO		0x10000
#define MEDIA_TYPE_VEDIO		0x20000
#define MEDIA_TYPE_VIDEO		MEDIA_TYPE_VEDIO
#define MEDIA_TYPE_NES			0x30000
#define MEDIA_TYPE_JPEG			0x40000
#define MEDIA_TYPE_FLASH		0x50000

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
#define WM_MEDIA_VOLUME_CHANGED     0x110b		// ����ֵ��ϵͳ�ı�

// ��Ƶ������ռģʽ����
#define MEDIA_MODE_NORMAL			0x0000		// ��ͨ����ģʽ
#define MEDIA_MODE_WAIT				0x0001		// �ȴ�����
#define MEDIA_MODE_PREEMPTIVE		0x0002		// ��ռʽ����ģʽ

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
#define MEDIA_CTRL_HEADPHONE_IN		0x0500		// 
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

HANDLE MediaSrvCallbackCreate(MEIDA_CALLBACK callback, DWORD id);
HANDLE MediaSrvCreate(void* media, char *name, int type, int preempt);
int MediaSrvDestroy(HANDLE hmedia);
int MediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size);
int MediaSrvGetName(HANDLE hmedia, char *name, int max);
int MediaSrvInfo(void *media, void *info, int type);
int MediaSrvMaster(char *name, int max, int type);


#endif //_MEDIA_H

