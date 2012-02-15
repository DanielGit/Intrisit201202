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

#ifndef _DIRECT_AK_H
#define _DIRECT_AK_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

// �˽ṹ�������ǰ����Ƶ�ṹ�壬����Խṹ�����κ��޸�!!!
typedef struct _AUDIO_INFO
{
	int Type;
	DWORD TotalTime;
	DWORD Bitrate;
	DWORD Samplerate;
}AUDIO_INFO;
typedef AUDIO_INFO *PAUDIO_INFO;

// �˽ṹ�������ǰ����Ƶ�ṹ�壬����Խṹ�����κ��޸�!!!
typedef struct _VIDEO_INFO
{
	int Type;
	int MovieWidth;
	int MovieHeight;
	int FPS;
	DWORD TotalTime;
	DWORD VideoBitrate;
	DWORD AudioBitrate;
}VIDEO_INFO;
typedef VIDEO_INFO *PVIDEO_INFO;

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


typedef struct _AK_FILE
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
}AK_VFILE;
typedef AK_VFILE *PAK_VFILE;

typedef struct _AK_CTRL
{
	LIST Link;
	int Code;
	DWORD *Param;
	int Size;
	int Return;
	HANDLE hSema;
	HANDLE hMutex;
	DWORD ParamData;
}AK_CTRL;
typedef AK_CTRL *PAK_CTRL;

typedef struct _AK_OBJECT
{
	KERNEL_OBJECT Header;	// �ں˶�����Ϣ
	LIST Link;
	char bGetInfo;
	char bClose;			// 1. ��������˳�  2. �����쳣�˳�
	char bTerminate;		// 1. �û������˳�  2. �����˳�
	char bRecPause;
	int *bReady;
	HANDLE hMedia;		// ���ſ���
	HANDLE hDac;		// DAC�豸���
	MEDIA_TASK Task;
	AK_CTRL Ctrl;		// ������Ϣ
	AK_VFILE File;		// ���ݻ���������ṹ��	
	MEDIA_INFO Info;	// ������Ϣ�ṹ��
	DWORD RecDuration;	// ¼�����ʱ����Ϣ
}AK_OBJECT;
typedef AK_OBJECT *PAK_OBJECT;

// ���ò����ļ�����
#define AK_PLAY_TYPE_UNKNOWN		0
#define AK_PLAY_TYPE_MIDI			1
#define AK_PLAY_TYPE_MP3			2
#define AK_PLAY_TYPE_AMR			3
#define AK_PLAY_TYPE_WAVE			4
#define AK_PLAY_TYPE_WMA			5
#define AK_PLAY_TYPE_PCM			6
#define AK_PLAY_TYPE_MELODY			7
#define AK_PLAY_TYPE_MP3_I			8
#define AK_PLAY_TYPE_MP3_II			9
#define AK_PLAY_TYPE_AAC			10
#define AK_PLAY_TYPE_NES			11
#define AK_PLAY_TYPE_BT				12
#define AK_PLAY_TYPE_SNES			13
#define AK_PLAY_TYPE_LPC_WMA		14
#define AK_PLAY_TYPE_FLAC_NATIVE	15
#define AK_PLAY_TYPE_FLAC_OGG		16

// ����¼���ļ�����
#define AK_REC_TYPE_WAVE			0
#define AK_REC_TYPE_PCM    			101
#define AK_REC_TYPE_AMR				102
#define AK_REC_TYPE_MP3				103
#define AK_REC_TYPE_UNKOWN			255

// ���ſ�����
#define AK_CTRL_PLAY			0x0001		// ����
#define AK_CTRL_PAUSE			0x0002		// ��ͣ
#define AK_CTRL_RESUME			0x0003		// ����
#define AK_CTRL_STOP			0x0004		// ֹͣ
#define AK_CTRL_CLOSE			0x0005		// �ر�
#define AK_CTRL_FORWARD			0x0006		// ���
#define AK_CTRL_BACKWARD		0x0007		// ����
#define AK_CTRL_SEEK			0x0008		// ��λ
#define AK_CTRL_SETAB			0x0009		// ����AB��
#define AK_CTRL_SETEQ			0x000A		// ���þ���
#define AK_CTRL_DURATION		0x000B		// ����ʱ��
#define AK_CTRL_VOLUME			0x000C		// ��������
#define AK_CTRL_TEMPO			0x000D		// ���������ٶ�

// ״̬��ȡ������
#define AK_GET_STATUS			0x0000		// ��ȡ����״̬(��ͣ�������)
#define AK_GET_ENFLAG			0x0001		// ��ȡ���ſ��������־
#define AK_GET_TIME				0x0011		// ��ȡ��ǰ����ʱ��
#define AK_GET_ALLTIME			0x0012		// ��ȡ�ܵĲ���ʱ��
#define AK_GET_ABPLAY			0x0013		// ��ȡAB����״̬
#define AK_GET_ATIME			0x0014		// ��ȡA��ʱ��
#define AK_GET_BTIME			0x0015		// ��ȡB��ʱ��
#define AK_GET_REPEADAB			0x0016		// ��ȡ�ظ�����״̬
#define AK_GET_EQ				0x0017		// ��ȡ����״̬
#define AK_GET_SPEED			0x0018		// ��ȡ�ٶ�״̬
#define AK_GET_VOLUME			0x0019		// ��ȡ����
#define AK_GET_OFFSET			0x001a		// ��ȡ���ݶ�ȡƫ��
#define AK_GET_VOLENHANCE		0x001b		// ��ȡ������ǿֵ

// ����״̬��
#define AK_ST_RECORD			0x1000		// ���ڲ���״̬
#define AK_ST_READY				0x0000		// ���������ھ���̬
#define AK_ST_PLAY				0x0001		// ���ڲ���״̬
#define AK_ST_PAUSE				0x0002		// ��ͣ״̬
#define AK_ST_STOP				0x0003		// �ر�״̬
#define AK_ST_FORWARD			0x0004		// ���״̬
#define AK_ST_BACKWARD			0x0005		// ����״̬

void MediaSrvLock(void);
void MediaSrvUnlock(void);
#define AK_LOCK()				//	MediaSrvLock()
#define AK_UNLOCK()				//  MediaSrvUnlock()

// AkCtl.c
int AkCtlInit(void);
int AkCtlVolumeSet(PAK_OBJECT obj, int volume);
int AkCtlPause(PAK_OBJECT obj);
int AkCtlResume(PAK_OBJECT obj);
int AkCtlSeek(PAK_OBJECT obj, DWORD milliSecond);
int AkCtlGetDuration(PAK_OBJECT obj, DWORD *duration);
int AkCtlIsPlaying(PAK_OBJECT obj);
int AkCtlPlaySpeed(PAK_OBJECT obj, int speed);
int AkCtlForward(PAK_OBJECT obj);
int AkCtlBackward(PAK_OBJECT obj);
int AkCtlEq(PAK_OBJECT obj, int eq);

// AkCallback.c
void AkCbSetCallback(void);
void AkCbInit(T_MEDIALIB_CB *cb);
void AkCbFuncInit(T_MEDIALIB_INIT_CB *init_cb);
int AkCbFileRead(PAK_VFILE vfile, BYTE *buffer, DWORD count);
int AkCbFileWrite(PAK_VFILE vfile, void *buffer, unsigned int count);
int AkCbFileWriteFlush(PAK_VFILE vfile);
int AkCbFileSeek(PAK_VFILE vfile, int offset, int origin);
int AkCbFileTell(PAK_VFILE vfile);

// AkThread.c
int AkThreadInit(void);
void AkThreadClose(PAK_OBJECT obj, int code);
int AkThreadOpen(PAK_OBJECT obj);
int AkThreadRun(PAK_OBJECT obj);

// AkSrv.c
void AkSrvInit(void);
void AkSrvLock(void);
void AkSrvUnlock(void);
int AkSrvPowerOff(int mode);
int AkSrvMode(void);
int AkSrvInfo(void *media, void *info, int type);
int AkSrvDestroyNotify(PMEDIA_TASK task);
int AkSrvProcMsg(PAK_OBJECT obj);

void AkShow(BYTE *data, int src_w, int src_h, RECT *dst_r, int dst_deg);
void AkShowOpen(void);
void AkShowClose(void);

HANDLE DacOpen(void);
int DacClose(HANDLE hdac);
int DacWrite(HANDLE hdac, short *src, int len);
int DacWriteEnd(HANDLE hdac, int terminate);
int DacSetSamplerate(HANDLE hdac, int samprate, int chs);
int DacSetVolume(HANDLE hdac, int volume);
int DacGetStatus(HANDLE hdac, int pause);
int DacSetStatus(HANDLE hdac, int pause);
int DacGetVolume(HANDLE hdac);
int DacSetTempo(HANDLE hdac, int tempo);

HANDLE AdcOpen(DWORD samprate, DWORD chs);
int AdcClose(HANDLE hadc);
int AdcRead(HANDLE hadc, short *src, int len);
int AdcSetVolume(HANDLE hadc, int volume);
int AdcGetStatus(HANDLE hadc, int pause);
int AdcSetStatus(HANDLE hadc, int pause);
int AdcGetVolume(HANDLE hadc);
int AdcSetEq(HANDLE hadc, int filter);

#endif	// _DIRECT_AK_H

