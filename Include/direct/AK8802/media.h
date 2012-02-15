//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          直接音频播放控制函数部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DIRECT_AK_H
#define _DIRECT_AK_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

// 此结构体兼容以前的音频结构体，请勿对结构体做任何修改!!!
typedef struct _AUDIO_INFO
{
	int Type;
	DWORD TotalTime;
	DWORD Bitrate;
	DWORD Samplerate;
}AUDIO_INFO;
typedef AUDIO_INFO *PAUDIO_INFO;

// 此结构体兼容以前的视频结构体，请勿对结构体做任何修改!!!
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
	DWORD Version;			// 版本
	HWND hWnd;				// 视频消息发送窗体
	int Record;				// 视频任务 0-播放 1-录制
	int MediaType;			// 视频媒体类型
	
	char *FileName;			// 视频文件名称
	BYTE *DataBuf;			// 视频数据缓冲区
	int BufSize;			// 视频缓冲区大小
	DWORD PlayOffset;		// 视频播放起始偏移
	DWORD PlayLength;		// 视频播放数据长度
	HANDLE hCallback;		// 回调函数句柄
	
	RECT ShowRect;			// 视频显示矩形
	int Volume;				// 音频录放音量
	int Tempo;				// 音频播放速度
	int Eq;					// 音频播放EQ模式
	int Duration;			// 录制时间
	int Position;			// 播放时间
	int Degree;				// 视频显示角度

	DWORD ABitrate;			// 音频录制波特率
	DWORD ASamplerate;		// 音频录制采样率
	DWORD AChannels;		// 音频录制声道数
	DWORD VBitrate;			// 视频录制波特率
	DWORD Device;			// 录放设备定义：MIC/LINE-IN
	
	int ExterdType;			// 附加数据类型
	int ExtendSize;			// 附加数据大小
	void *Extend;			// 附加数据
}MEDIA_TASK;
typedef MEDIA_TASK *PMEDIA_TASK;


typedef struct _AK_FILE
{
	int PlayOffset;			// 播放数据偏移
	int PlayLength;			// 播放数据长度
	int vOffset;			// 虚拟文件/缓冲区偏移
	
	FILE *File;				// 媒体数据文件句柄
	
	BYTE *uBuf;				// 用户缓冲区指针
	int uSize;				// 用户缓冲区大小
	int uValid;				// 用户缓冲区中有效数据
	int uOffset;			// 用户缓冲区数据虚拟偏移
	HANDLE uCb;				// 回调函数句柄

#if defined(CONFIG_MMU_ENABLE)
	int Asid;				// 用于读取用户空间数据使用
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
	KERNEL_OBJECT Header;	// 内核对象信息
	LIST Link;
	char bGetInfo;
	char bClose;			// 1. 播放完毕退出  2. 播放异常退出
	char bTerminate;		// 1. 用户主动退出  2. 被迫退出
	char bRecPause;
	int *bReady;
	HANDLE hMedia;		// 播放库句柄
	HANDLE hDac;		// DAC设备句柄
	MEDIA_TASK Task;
	AK_CTRL Ctrl;		// 控制信息
	AK_VFILE File;		// 数据缓冲区管理结构体	
	MEDIA_INFO Info;	// 数据信息结构体
	DWORD RecDuration;	// 录音相关时间信息
}AK_OBJECT;
typedef AK_OBJECT *PAK_OBJECT;

// 设置播放文件类型
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

// 设置录音文件类型
#define AK_REC_TYPE_WAVE			0
#define AK_REC_TYPE_PCM    			101
#define AK_REC_TYPE_AMR				102
#define AK_REC_TYPE_MP3				103
#define AK_REC_TYPE_UNKOWN			255

// 播放控制码
#define AK_CTRL_PLAY			0x0001		// 播放
#define AK_CTRL_PAUSE			0x0002		// 暂停
#define AK_CTRL_RESUME			0x0003		// 继续
#define AK_CTRL_STOP			0x0004		// 停止
#define AK_CTRL_CLOSE			0x0005		// 关闭
#define AK_CTRL_FORWARD			0x0006		// 快进
#define AK_CTRL_BACKWARD		0x0007		// 快退
#define AK_CTRL_SEEK			0x0008		// 定位
#define AK_CTRL_SETAB			0x0009		// 设置AB点
#define AK_CTRL_SETEQ			0x000A		// 设置均衡
#define AK_CTRL_DURATION		0x000B		// 返回时间
#define AK_CTRL_VOLUME			0x000C		// 音量控制
#define AK_CTRL_TEMPO			0x000D		// 设置语音速度

// 状态获取控制码
#define AK_GET_STATUS			0x0000		// 获取播放状态(暂停、快进等)
#define AK_GET_ENFLAG			0x0001		// 获取播放控制允许标志
#define AK_GET_TIME				0x0011		// 获取当前播放时间
#define AK_GET_ALLTIME			0x0012		// 获取总的播放时间
#define AK_GET_ABPLAY			0x0013		// 获取AB播放状态
#define AK_GET_ATIME			0x0014		// 获取A点时间
#define AK_GET_BTIME			0x0015		// 获取B点时间
#define AK_GET_REPEADAB			0x0016		// 获取重复播放状态
#define AK_GET_EQ				0x0017		// 获取均衡状态
#define AK_GET_SPEED			0x0018		// 获取速度状态
#define AK_GET_VOLUME			0x0019		// 获取音量
#define AK_GET_OFFSET			0x001a		// 获取数据读取偏移
#define AK_GET_VOLENHANCE		0x001b		// 获取音量增强值

// 播放状态码
#define AK_ST_RECORD			0x1000		// 正在播放状态
#define AK_ST_READY				0x0000		// 播放器处于就绪态
#define AK_ST_PLAY				0x0001		// 正在播放状态
#define AK_ST_PAUSE				0x0002		// 暂停状态
#define AK_ST_STOP				0x0003		// 关闭状态
#define AK_ST_FORWARD			0x0004		// 快进状态
#define AK_ST_BACKWARD			0x0005		// 快退状态

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

