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
	BYTE FileType[4];		// �ļ�����

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

typedef struct _Jz47_AV_Decoder
{
// display	
	unsigned char *lcd_frame_buffer;
	int		lcd_width, lcd_height, lcd_line_length;
	int		UsrLcdWidth,UsrLcdHeight,UsrLcdPosX,UsrLcdPosY;
	int		kprintf_enable;		//=0,����ӡDEBUG��Ϣ��=1����ӡDEBUG��Ϣ
	char	AudioCodec[64];		//audio ����
	char	VideoCodec[64];		//video ����
	char	FileName[256];		//�ļ���
	int OutFormat;

// file operation
  void *stream, *plugstream;
	long (*os_ftell)(void *stream);
	long (*os_fread)(void *ptr, long size, long nmemb, void *stream);
	long (*os_fwrite)(void *ptr, long size, long nmemb, void *stream);
	long (*os_fseek) (void *stream, long offset, int whence);
	void (*os_msleep) (unsigned long msec);

//audio control
  int   (*os_audio_init)      (int rate,int channels,int format,int flags);
  void  (*os_audio_uninit)    (int immed);
  void  (*os_audio_reset)     (void);
  void  (*os_audio_pause)     (void);
  void  (*os_audio_resume)    (void);
  int   (*os_audio_get_space) (void);
  int   (*os_audio_play)      (void* data,int len,int flags);
  float (*os_audio_get_delay) (void);
  int   (*os_audio_control)   (int cmd,void *arg);
  void  (*os_open_video_osd)  (int w, int h);
  int	(*os_audio_long_jump) (void* stream, jmp_buf buf, int ret_val);
  
// malloc usage	
	unsigned char *malloc_buf;
	unsigned int malloc_size;

//media info
	MEDIA_INFO 	MediaInfo;
	DWORD		AudioSpace;
	BYTE		fIpuEnable;
	BYTE		fMplayerInit;
	jmp_buf		AudioJumpBuf;
	BYTE		*pJzDecoderBuf;
}Jz47_AV_Decoder;
typedef Jz47_AV_Decoder *PJz47_AV_Decoder;

// �ڲ�������ʹ�ýṹ��
typedef struct _INNER_PLAYER
{
	int		status;				//�ڲ���������ǰ��״̬
	int		type;				//�ڲ����������ŵ�����
	DWORD	cur_time;			//�ڲ��������������ĵ�ǰʱ��
	DWORD	total_time;			//�ڲ����������������ܹ�ʱ��

	DWORD mp3_pcm_len;			//MP3�ļ��Ѿ���ѹ��PCM������
	DWORD	wav_data_len;		//WAV�ļ����ݳ��ȣ�WAV�ļ������ʱ����һЩ�������ݣ�����BOBO����

	struct _INNER_PLAYER_DRV* pDevice;	//�ڲ�����������
}INNER_PLAYER;

typedef struct _AK_OBJECT
{
	KERNEL_OBJECT Header;			// �ں˶�����Ϣ
	LIST Link;
	char bGetInfo;
	char bClose;					// 1. ��������˳�  2. �����쳣�˳�
	char bTerminate;				// 1. �û������˳�  2. �����˳�
	char bRecPause;
	int *bReady;
	HANDLE hMedia;					// ���ſ���
	HANDLE hDac;					// DAC�豸���
	MEDIA_TASK Task;
	AK_CTRL Ctrl;					// ������Ϣ
	AK_VFILE File;					// ���ݻ���������ṹ��	
	MEDIA_INFO Info;				// ������Ϣ�ṹ��
	DWORD RecDuration;				// ¼�����ʱ����Ϣ
	INNER_PLAYER InnerPlayer;
	PJz47_AV_Decoder JzAvDecoder;
	int  bPlayerPause;
	RECT PauseRect;					// ��ͣʱ������Ҫˢ��VIDEO����Ļ����
	int	PlayerType;					// ����������
									// = 0,��ʾʹ���ڲ�������
									// = 1,��ʾʹ���ⲿ������
	int StartTime;					// ��ʱ��ʼλ��
	int EndTime;					// ��ʱ�������
	DWORD nSkipData;				// ����N�������Ժ��ڲ�������(SEEK�����POPO������Ҫȥ�������)
}AK_OBJECT;
typedef AK_OBJECT *PAK_OBJECT;

// �豸���������ṹ��
typedef struct _INNER_PLAYER_DRV
{
	int (*Open)(PAK_OBJECT , PAK_VFILE);
	int	(*Close)(PAK_OBJECT , PAK_VFILE);
	int (*Info)(PAK_OBJECT , PAK_VFILE);
	int (*Seek)(PAK_OBJECT, int);
	int (*GetPcmData)(PAK_OBJECT , BYTE* , int);
}INNER_PLAYER_DRV;
typedef INNER_PLAYER_DRV *PINNER_PLAYER_DRV;


typedef struct WAVE_HEADER 
{
    signed char  riff[4];			// "RIFF"
    signed long  file_size;		// in bytes
    signed char  wavefmt_[8];		// "WAVE"
    signed long  chunk_size;		// in bytes (16 for PCM)
    signed short format_tag;		// 1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
    signed short num_chans;		    // 1=mono, 2=stereo
    signed long  sample_rate;
    signed long  bytes_per_sec;
    signed short bytes_per_samp;	// 2=16-bit mono, 4=16-bit stereo
    signed short bits_per_samp;
    signed char  data[4];			// "data"
    signed long  data_length;		// in bytes
}WAVE_HEADER;

typedef struct _MP3_INFO
{
	int MediaType;
	DWORD TotalTime;
	
	int AudioCodec;
	int AudioBitrate;
	int AudioSamplerate;
	int AudioChannels;
}MP3_INFO;

typedef enum
{
	MEDIALIB_INIT,
	MEDIALIB_END,
	MEDIALIB_PLAYING,
	MEDIALIB_PAUSE,
	MEDIALIB_STOP,
	MEDIALIB_ERR,
	MEDIALIB_SEEK
}MEDIALIB_STATUS;

typedef enum
{
	MEDIALIB_WAV,
	MEDIALIB_MP3,
	MEDIALIB_MIDI,
	MEDIALIB_UNKNOW
}MEDIALIB_TYPE;

// ����¼���ļ�����
#define AK_REC_TYPE_WAVE			0
#define AK_REC_TYPE_PCM    			101
#define AK_REC_TYPE_AMR				102
#define AK_REC_TYPE_MP3				103
#define AK_REC_TYPE_UNKOWN			255

void MediaSrvLock(void);
void MediaSrvUnlock(void);
#define JZ_LOCK()				//	MediaSrvLock()
#define JZ_UNLOCK()				//  MediaSrvUnlock()

// JzCtl.c
int JzCtlInit(void);
int JzCtlVolumeSet(PAK_OBJECT obj, int volume);
int JzCtlPause(PAK_OBJECT obj);
int JzCtlResume(PAK_OBJECT obj);
int JzCtlSeek(PAK_OBJECT obj, DWORD milliSecond);
int JzCtlGetDuration(PAK_OBJECT obj, DWORD *duration);
int JzCtlIsPlaying(PAK_OBJECT obj);
int JzCtlPlaySpeed(PAK_OBJECT obj, int speed);
int JzCtlForward(PAK_OBJECT obj);
int JzCtlBackward(PAK_OBJECT obj);
int JzCtlEq(PAK_OBJECT obj, int eq);
int JzCtlResize(PAK_OBJECT obj);
int JzCtlGetTotalTime(PAK_OBJECT obj,DWORD* totaltime);
int JzCtlSetAB(PAK_OBJECT obj,DWORD start, DWORD end);

// JzCallback.c
void JzCbSetCallback(void);
//void JzCbInit(T_MEDIALIB_CB *cb);
//void JzCbFuncInit(T_MEDIALIB_INIT_CB *init_cb);
int JzCbFileRead( BYTE *buffer, DWORD count,DWORD size,PAK_VFILE vfile);
int JzCbFileWrite(void *buffer, unsigned int count, unsigned int size, PAK_VFILE vfile);
int JzCbFileWriteFlush(PAK_VFILE vfile);
int JzCbFileSeek(PAK_VFILE vfile, int offset, int origin);
int JzCbFileTell(PAK_VFILE vfile);
PJz47_AV_Decoder GetCbDecoder();
void AkCbReinit(PJz47_AV_Decoder cb, PAK_OBJECT obj );
void AkCbInit(PJz47_AV_Decoder cb, PAK_OBJECT obj);
void SetMplayerAudioSpace(PAK_OBJECT obj);

// JzThread.c
int JzThreadInit(void);
void JzThreadClose(PAK_OBJECT obj, int code);
int JzThreadOpen(PAK_OBJECT obj);
int JzThreadRun(PAK_OBJECT obj);

// JzSrv.c
void JzSrvInit(void);
void JzSrvLock(void);
void JzSrvUnlock(void);
int JzSrvPowerOff(int mode);
int JzSrvMode(void);
int JzSrvInfo(void *media, void *info, int type);
int JzSrvDestroyNotify(PMEDIA_TASK task);
int JzSrvProcMsg(PAK_OBJECT obj);

void JzShow(BYTE *data, int src_w, int src_h, RECT *dst_r, int dst_deg);
void JzShowOpen(void);
void JzShowClose(void);

// DacMux.c
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

// AdcMux.c
HANDLE AdcOpen(DWORD samprate, DWORD chs);
int AdcClose(HANDLE hadc);
int AdcRead(HANDLE hadc, short *src, int len);
int AdcSetVolume(HANDLE hadc, int volume);
int AdcGetStatus(HANDLE hadc, int pause);
int AdcSetStatus(HANDLE hadc, int pause);
int AdcGetVolume(HANDLE hadc);
int AdcSetEq(HANDLE hadc, int filter);

// JzMediaLib.c
int MediaLibInit();
int MediaLibCbInit(PAK_OBJECT obj);
int MediaLibOpen(PAK_OBJECT obj, int info);
int MediaLibPlay(PAK_OBJECT obj);
int MediaLibClose(PAK_OBJECT obj);
int MediaLibGetInfo(PAK_OBJECT obj);
MEDIALIB_STATUS MediaLibGetStatus(PAK_OBJECT obj);
int MediaLibPause(PAK_OBJECT obj);
int MediaLibResume(PAK_OBJECT obj);
int MediaLibStop(PAK_OBJECT obj);
int MediaLibSeek(PAK_OBJECT obj, unsigned long ms);
int MediaLibGetPcm(PAK_OBJECT obj,BYTE* pcm,int size);
int MediaLibGetYuv(PAK_OBJECT obj);
int MediaLibGetAudioCurTime(PAK_OBJECT obj);

int Mp3DecodeFrame(HANDLE hmp3);
int Mp3DecodeSeek(HANDLE hmp3, DWORD ms);
HANDLE Mp3DecodeCreate(PAK_VFILE vfile);
int Mp3DecodeDestroy(HANDLE hmp3);
int Mp3DecodeGetPcm(HANDLE hmp3, short *buf, int *max);
int Mp3DecodeSetCallback(HANDLE hmp3, void *callback, DWORD id);
int GetMp3PlayTime(MP3_INFO* Info, void *callback, DWORD id,int estimate,DWORD file_size);
DWORD GetMp3DecodeTime(HANDLE hMedia, PAK_OBJECT obj);

extern INNER_PLAYER_DRV WavDrvList;
extern INNER_PLAYER_DRV Mp3DrvList;
extern INNER_PLAYER_DRV MidiDrvList;

#endif	// _DIRECT_AK_H

