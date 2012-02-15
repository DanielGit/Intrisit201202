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

#ifndef _DIRECT_FFMPEG_H
#define _DIRECT_FFMPEG_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#include <ffmpeg/libavutil/avstring.h>
#include <ffmpeg/libavformat/avformat.h>
#include <ffmpeg/libswscale/swscale.h>
#include <ffmpeg/libavcodec/opt.h>


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


typedef struct _FFMPEG_FILE
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
}FFMPEG_VFILE;
typedef FFMPEG_VFILE *PFFMPEG_VFILE;

typedef struct _FFMPEG_CTRL
{
	LIST Link;
	int Code;
	DWORD *Param;
	int Size;
	int Return;
	HANDLE hSema;
	HANDLE hMutex;
	DWORD ParamData;
}FFMPEG_CTRL;
typedef FFMPEG_CTRL *PFFMPEG_CTRL;


typedef struct _FFMPEG_OBJECT
{
	KERNEL_OBJECT Header;	// �ں˶�����Ϣ
	LIST Link;
	char bGetInfo;
	char bRecPause;
	char bClose;
	char bTerminate;
	int *bReady;
	HANDLE hMedia;		// ���ſ���
	HANDLE hDac;		// DAC�豸���
	MEDIA_TASK Task;
	FFMPEG_CTRL Ctrl;		// ������Ϣ
	FFMPEG_VFILE File;		// ���ݻ���������ṹ��	
	MEDIA_INFO Info;	// ������Ϣ�ṹ��
	
	// ¼���������
	DWORD CurTime;
	
	// FFMPEG���
	int paused;
	int last_paused;
	int seek_req;
	int seek_flags;
	int64_t seek_pos;
	AVFormatContext *ic;
	
	int audio_stream;
	AVStream *audio_st;
	
	int video_stream;
	AVStream *video_st;
}FFMPEG_OBJECT;
typedef FFMPEG_OBJECT *PFFMPEG_OBJECT;

// ���ò����ļ�����
#define FFMPEG_PLAY_TYPE_UNKNOWN		0
#define FFMPEG_PLAY_TYPE_MIDI			1
#define FFMPEG_PLAY_TYPE_MP3			2
#define FFMPEG_PLAY_TYPE_AMR			3
#define FFMPEG_PLAY_TYPE_WAVE			4
#define FFMPEG_PLAY_TYPE_WMA			5
#define FFMPEG_PLAY_TYPE_PCM			6
#define FFMPEG_PLAY_TYPE_MELODY			7
#define FFMPEG_PLAY_TYPE_MP3_I			8
#define FFMPEG_PLAY_TYPE_MP3_II			9
#define FFMPEG_PLAY_TYPE_AAC			10
#define FFMPEG_PLAY_TYPE_NES			11
#define FFMPEG_PLAY_TYPE_BT				12
#define FFMPEG_PLAY_TYPE_SNES			13
#define FFMPEG_PLAY_TYPE_LPC_WMA		14
#define FFMPEG_PLAY_TYPE_FLAC_NATIVE	15
#define FFMPEG_PLAY_TYPE_FLAC_OGG		16

// ����¼���ļ�����
#define FFMPEG_REC_TYPE_WAVE			0
#define FFMPEG_REC_TYPE_PCM    			101
#define FFMPEG_REC_TYPE_AMR				102
#define FFMPEG_REC_TYPE_MP3				103
#define FFMPEG_REC_TYPE_UNKOWN			255

// ���ſ�����
#define FFMPEG_CTRL_PLAY			0x0001		// ����
#define FFMPEG_CTRL_PAUSE			0x0002		// ��ͣ
#define FFMPEG_CTRL_RESUME			0x0003		// ����
#define FFMPEG_CTRL_STOP			0x0004		// ֹͣ
#define FFMPEG_CTRL_CLOSE			0x0005		// �ر�
#define FFMPEG_CTRL_FORWARD			0x0006		// ���
#define FFMPEG_CTRL_BACKWARD		0x0007		// ����
#define FFMPEG_CTRL_SEEK			0x0008		// ��λ
#define FFMPEG_CTRL_SETAB			0x0009		// ����AB��
#define FFMPEG_CTRL_SETEQ			0x000A		// ���þ���
#define FFMPEG_CTRL_DURATION		0x000B		// ����ʱ��
#define FFMPEG_CTRL_VOLUME			0x000C		// ��������
#define FFMPEG_CTRL_TEMPO			0x000D		// ���������ٶ�

// ״̬��ȡ������
#define FFMPEG_GET_STATUS			0x0000		// ��ȡ����״̬(��ͣ�������)
#define FFMPEG_GET_ENFLAG			0x0001		// ��ȡ���ſ��������־
#define FFMPEG_GET_TIME				0x0011		// ��ȡ��ǰ����ʱ��
#define FFMPEG_GET_ALLTIME			0x0012		// ��ȡ�ܵĲ���ʱ��
#define FFMPEG_GET_ABPLAY			0x0013		// ��ȡAB����״̬
#define FFMPEG_GET_ATIME			0x0014		// ��ȡA��ʱ��
#define FFMPEG_GET_BTIME			0x0015		// ��ȡB��ʱ��
#define FFMPEG_GET_REPEADAB			0x0016		// ��ȡ�ظ�����״̬
#define FFMPEG_GET_EQ				0x0017		// ��ȡ����״̬
#define FFMPEG_GET_SPEED			0x0018		// ��ȡ�ٶ�״̬
#define FFMPEG_GET_VOLUME			0x0019		// ��ȡ����
#define FFMPEG_GET_OFFSET			0x001a		// ��ȡ���ݶ�ȡƫ��
#define FFMPEG_GET_VOLENHANCE		0x001b		// ��ȡ������ǿֵ

// ����״̬��
#define FFMPEG_ST_RECORD			0x1000		// ���ڲ���״̬
#define FFMPEG_ST_READY				0x0000		// ���������ھ���̬
#define FFMPEG_ST_PLAY				0x0001		// ���ڲ���״̬
#define FFMPEG_ST_PAUSE				0x0002		// ��ͣ״̬
#define FFMPEG_ST_STOP				0x0003		// �ر�״̬
#define FFMPEG_ST_FORWARD			0x0004		// ���״̬
#define FFMPEG_ST_BACKWARD			0x0005		// ����״̬

#define FFMPEG_LOCK()				//	MediaSrvLock()
#define FFMPEG_UNLOCK()				//  MediaSrvUnlock()

// FfmpegCtl.c
int FfmpegCtlInit(void);
int FfmpegCtlVolumeSet(PFFMPEG_OBJECT obj, int volume);
int FfmpegCtlPause(PFFMPEG_OBJECT obj);
int FfmpegCtlResume(PFFMPEG_OBJECT obj);
int FfmpegCtlSeek(PFFMPEG_OBJECT obj, DWORD milliSecond);
int FfmpegCtlGetDuration(PFFMPEG_OBJECT obj, DWORD *duration);
int FfmpegCtlIsPlaying(PFFMPEG_OBJECT obj);
int FfmpegCtlPlaySpeed(PFFMPEG_OBJECT obj, WORD speed);
int FfmpegCtlForward(PFFMPEG_OBJECT obj);
int FfmpegCtlBackward(PFFMPEG_OBJECT obj);

// FfmpegCallback.c
void FfmpegCbSetCallback(void);
void FfmpegCbInit(void);
int FfmpegCbFileRead(PFFMPEG_VFILE vfile, BYTE *buffer, int count);
int FfmpegCbFileWrite(PFFMPEG_VFILE vfile, void *buffer, int count);
int FfmpegCbFileWriteFlush(PFFMPEG_VFILE vfile);
int FfmpegCbFileSeek(PFFMPEG_VFILE vfile, int offset, int origin);
int FfmpegCbFileTell(PFFMPEG_VFILE vfile);

// FfmpegThread.c
int FfmpegThreadInit(void);
void FfmpegThreadClose(PFFMPEG_OBJECT obj, int code);
void FfmpegThreadOpen(PFFMPEG_OBJECT obj);
int FfmpegThreadRun(PFFMPEG_OBJECT obj);

// FfmpegSrv.c
void FfmpegSrvInit(void);
void FfmpegSrvLock(void);
void FfmpegSrvUnlock(void);
int FfmpegSrvPowerOff(int mode);
int FfmpegSrvMode(void);
int FfmpegSrvInfo(void *media, void *info, int type);
int FfmpegSrvDestroyNotify(PMEDIA_TASK task);
void FfmpegSrvProcMsg(PFFMPEG_OBJECT obj);

void FfmpegShow(DWORD *data, RECT *dst_r, int dst_deg);
void FfmpegShowOpen(void);
void FfmpegShowClose(void);

HANDLE DacOpen(void);
int DacClose(HANDLE hdac);
int DacWrite(HANDLE hdac, short *src, int len);
int DacSetSamplerate(HANDLE hdac, int samprate, int chs);
int DacSetVolume(HANDLE hdac, int volume);
int DacGetStatus(HANDLE hdac, int pause);
int DacSetStatus(HANDLE hdac, int pause);
int DacGetVolume(HANDLE hdac);

HANDLE AdcOpen(DWORD samprate, DWORD chs);
int AdcClose(HANDLE hdac);
int AdcRead(HANDLE hdac, short *src, int len);
int AdcSetVolume(HANDLE hdac, int volume);
int AdcGetStatus(HANDLE hdac, int pause);
int AdcSetStatus(HANDLE hdac, int pause);
int AdcGetVolume(HANDLE hdac);

#endif	// _DIRECT_FFMPEG_H

