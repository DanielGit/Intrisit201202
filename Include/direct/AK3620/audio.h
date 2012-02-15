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

#ifndef _DIRECT_AUDIO_H
#define _DIRECT_AUDIO_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(CONFIG_MCU_AK3620) && !defined(WIN32)
#include "APU.h"
#endif

typedef struct _AUDIO_FILE
{
	int PlayOffset;			// ��������ƫ��
	int PlayLength;			// �������ݳ���
	int vOffset;			// �����ļ�/������ƫ��
	
	FILE *File;				// ý�������ļ����
	
	BYTE *uBuf;				// �û�������ָ��
	int uSize;				// �û���������С
	int uValid;				// �û�����������Ч����
	int uOffset;			// �û���������������ƫ��
	HANDLE uCb;				// �ص��������

#if defined(CONFIG_MMU_ENABLE)
	int Asid;				// ���ڶ�ȡ�û��ռ�����ʹ��
#endif	
}AUDIO_FILE;
typedef AUDIO_FILE *PAUDIO_FILE;

typedef struct _AUDIO_TASK
{
	HWND hWnd;				// ��Ƶ��Ϣ���ʹ���
	int Record;				// ��Ƶ���� 0-���� 1-¼��
	int MediaType;			// ��Ƶý������
	
	char *FileName;			// ��Ƶ�ļ�����
	BYTE *DataBuf;			// ��Ƶ���ݻ�����
	int BufSize;			// ��Ƶ��������С
	DWORD PlayOffset;		// ��Ƶ������ʼƫ��
	DWORD PlayLength;		// ��Ƶ�������ݳ���
	HANDLE hCallback;		// �ص��������
	
	int Volume;				// ��Ƶ¼������
	int Tempo;				// ��Ƶ�����ٶ�
	int Eq;					// ��Ƶ����EQģʽ
	
	DWORD Bitrate;			// ��Ƶ¼�Ʋ�����
	DWORD Samplerate;		// ��Ƶ¼�Ʋ�����
	DWORD Device;			// ¼���豸���壺MIC/LINE-IN
	
	void *Extend;			// ��������
}AUDIO_TASK;
typedef AUDIO_TASK *PAUDIO_TASK;

typedef struct _AUDIO_CTRL
{
	LIST Link;
	int Code;
	DWORD *Param;
	int Size;
	int Return;
	HANDLE hSema;
	HANDLE hMutex;
	DWORD ParamData;
}AUDIO_CTRL;
typedef AUDIO_CTRL *PAUDIO_CTRL;

typedef struct _AUDIO_INFO
{
	int Type;
	DWORD TotalTime;
	DWORD Bitrate;
	DWORD Samplerate;
}AUDIO_INFO;
typedef AUDIO_INFO *PAUDIO_INFO;

typedef struct _AUDIO_OBJECT
{
	KERNEL_OBJECT Header;	// �ں˶�����Ϣ
	LIST Link;
	int bGetInfo;
	AUDIO_TASK Task;
	AUDIO_CTRL Ctrl;		// ������Ϣ
	AUDIO_FILE File;		// ���ݻ���������ṹ��	
	AUDIO_INFO Info;		// ������Ϣ�ṹ��
}AUDIO_OBJECT;
typedef AUDIO_OBJECT *PAUDIO_OBJECT;

// ���ò����ļ�����
#define AUDIO_PLAY_TYPE_UNKNOWN			_SD_MEDIA_TYPE_UNKNOWN
#define AUDIO_PLAY_TYPE_MIDI			_SD_MEDIA_TYPE_MIDI
#define AUDIO_PLAY_TYPE_MP3				_SD_MEDIA_TYPE_MP3
#define AUDIO_PLAY_TYPE_AMR				_SD_MEDIA_TYPE_AMR
#define AUDIO_PLAY_TYPE_WAVE			_SD_MEDIA_TYPE_WAVE
#define AUDIO_PLAY_TYPE_WMA				_SD_MEDIA_TYPE_WMA
#define AUDIO_PLAY_TYPE_PCM				_SD_MEDIA_TYPE_PCM
#define AUDIO_PLAY_TYPE_MELODY			_SD_MEDIA_TYPE_MELODY
#define AUDIO_PLAY_TYPE_MP3_I			_SD_MEDIA_TYPE_MP3_I
#define AUDIO_PLAY_TYPE_MP3_II			_SD_MEDIA_TYPE_MP3_II
#define AUDIO_PLAY_TYPE_AAC				_SD_MEDIA_TYPE_AAC
#define AUDIO_PLAY_TYPE_NES				_SD_MEDIA_TYPE_NES
#define AUDIO_PLAY_TYPE_BT				_SD_MEDIA_TYPE_BT
#define AUDIO_PLAY_TYPE_SNES			_SD_MEDIA_TYPE_SNES
#define AUDIO_PLAY_TYPE_LPC_WMA			_SD_MEDIA_TYPE_LPC_WMA
#define AUDIO_PLAY_TYPE_FLAC_NATIVE		_SD_MEDIA_TYPE_FLAC_NATIVE
#define AUDIO_PLAY_TYPE_FLAC_OGG		_SD_MEDIA_TYPE_FLAC_OGG

// ����¼���ļ�����
#define AUDIO_REC_TYPE_WAVE				0
#define AUDIO_REC_TYPE_PCM    			101
#define AUDIO_REC_TYPE_AMR				102
#define AUDIO_REC_TYPE_UNKOWN			255

// ���ſ�����
#define AUDIO_CTRL_PLAY				0x0001		// ����
#define AUDIO_CTRL_PAUSE			0x0002		// ��ͣ
#define AUDIO_CTRL_RESUME			0x0003		// ����
#define AUDIO_CTRL_STOP				0x0004		// ֹͣ
#define AUDIO_CTRL_CLOSE			0x0005		// �ر�
#define AUDIO_CTRL_FORWARD			0x0006		// ���
#define AUDIO_CTRL_BACKWARD			0x0007		// ����
#define AUDIO_CTRL_SEEK				0x0008		// ��λ
#define AUDIO_CTRL_SETAB			0x0009		// ����AB��
#define AUDIO_CTRL_SETEQ			0x000A		// ���þ���
#define AUDIO_CTRL_DURATION			0x000B		// ����ʱ��
#define AUDIO_CTRL_VOLUME			0x000C		// ��������
#define AUDIO_CTRL_TEMPO			0x000D		// ���������ٶ�

// ״̬��ȡ������
#define AUDIO_GET_STATUS			0x0000		// ��ȡ����״̬(��ͣ�������)
#define AUDIO_GET_ENFLAG			0x0001		// ��ȡ���ſ��������־
#define AUDIO_GET_TIME				0x0011		// ��ȡ��ǰ����ʱ��
#define AUDIO_GET_ALLTIME			0x0012		// ��ȡ�ܵĲ���ʱ��
#define AUDIO_GET_ABPLAY			0x0013		// ��ȡAB����״̬
#define AUDIO_GET_ATIME				0x0014		// ��ȡA��ʱ��
#define AUDIO_GET_BTIME				0x0015		// ��ȡB��ʱ��
#define AUDIO_GET_REPEADAB			0x0016		// ��ȡ�ظ�����״̬
#define AUDIO_GET_EQ				0x0017		// ��ȡ����״̬
#define AUDIO_GET_SPEED				0x0018		// ��ȡ�ٶ�״̬
#define AUDIO_GET_VOLUME			0x0019		// ��ȡ����
#define AUDIO_GET_OFFSET			0x001a		// ��ȡ���ݶ�ȡƫ��
#define AUDIO_GET_VOLENHANCE		0x001b		// ��ȡ������ǿֵ

// ����״̬��
#define AUDIO_ST_RECORD				0x1000		// ���ڲ���״̬
#define AUDIO_ST_READY				0x0000		// ���������ھ���̬
#define AUDIO_ST_PLAY				0x0001		// ���ڲ���״̬
#define AUDIO_ST_PAUSE				0x0002		// ��ͣ״̬
#define AUDIO_ST_STOP				0x0003		// �ر�״̬
#define AUDIO_ST_FORWARD			0x0004		// ���״̬
#define AUDIO_ST_BACKWARD			0x0005		// ����״̬

#define AUDIO_LOCK()				//	MediaSrvLock()
#define AUDIO_UNLOCK()				//  MediaSrvUnlock()

// AudioCtl.c
int AudioCtlInit(void);
int AudioCtlVolumeSet(PAUDIO_OBJECT obj, int volume);
int AudioCtlPause(PAUDIO_OBJECT obj);
int AudioCtlResume(PAUDIO_OBJECT obj);
int AudioCtlSeek(PAUDIO_OBJECT obj, DWORD milliSecond);
int AudioCtlGetDuration(PAUDIO_OBJECT obj, DWORD *duration);
int AudioCtlIsPlaying(PAUDIO_OBJECT obj);
int AudioCtlPlaySpeed(PAUDIO_OBJECT obj, WORD speed);
int AudioCtlPhonePlus(void);
void AudioCtlPipo(int ena);
void AudioCtlPhonePlusCheck(int count);
void AudioCtlPowerAmp(int ena);

// AudioCallback.c
void AudioCbSetCallback(void);

// AudioThread.c
int AudioThreadInit(void);
void AudioThreadClose(PAUDIO_OBJECT obj, int code);
void AudioThreadOpen(PAUDIO_OBJECT obj);
int AudioThreadRun(void);

// AudioSrv.c
void AudioSrvInit(void);
void AudioSrvLock(void);
void AudioSrvUnlock(void);
int AudioSrvPowerOff(int mode);
int AudioSrvMode(void);
int AudioSrvInfo(void *media, void *info);
int AudioSrvDestroyNotify(PAUDIO_TASK task);
void AudioSrvProcMsg(PAUDIO_OBJECT obj);

// AudioFile.c
int AudioFileRead(PAUDIO_FILE vfile, BYTE *buffer, int count);
int AudioFileWrite(PAUDIO_FILE vfile,  void	*buffer, unsigned int count);
int AudioFileSeek(PAUDIO_FILE vfile, int offset, int origin);
int AudioFileSize(PAUDIO_FILE vfile);
int AudioFileTell(PAUDIO_FILE vfile);
int AudioFileWriteFlush(PAUDIO_FILE vfile);

#endif	// _DIRECT_AUDIO_H

