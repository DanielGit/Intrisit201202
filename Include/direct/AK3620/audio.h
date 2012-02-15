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
}AUDIO_FILE;
typedef AUDIO_FILE *PAUDIO_FILE;

typedef struct _AUDIO_TASK
{
	HWND hWnd;				// 音频消息发送窗体
	int Record;				// 音频任务 0-播放 1-录音
	int MediaType;			// 音频媒体类型
	
	char *FileName;			// 音频文件名称
	BYTE *DataBuf;			// 音频数据缓冲区
	int BufSize;			// 音频缓冲区大小
	DWORD PlayOffset;		// 音频播放起始偏移
	DWORD PlayLength;		// 音频播放数据长度
	HANDLE hCallback;		// 回调函数句柄
	
	int Volume;				// 音频录放音量
	int Tempo;				// 音频播放速度
	int Eq;					// 音频播放EQ模式
	
	DWORD Bitrate;			// 音频录制波特率
	DWORD Samplerate;		// 音频录制采样率
	DWORD Device;			// 录放设备定义：MIC/LINE-IN
	
	void *Extend;			// 附加数据
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
	KERNEL_OBJECT Header;	// 内核对象信息
	LIST Link;
	int bGetInfo;
	AUDIO_TASK Task;
	AUDIO_CTRL Ctrl;		// 控制信息
	AUDIO_FILE File;		// 数据缓冲区管理结构体	
	AUDIO_INFO Info;		// 数据信息结构体
}AUDIO_OBJECT;
typedef AUDIO_OBJECT *PAUDIO_OBJECT;

// 设置播放文件类型
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

// 设置录音文件类型
#define AUDIO_REC_TYPE_WAVE				0
#define AUDIO_REC_TYPE_PCM    			101
#define AUDIO_REC_TYPE_AMR				102
#define AUDIO_REC_TYPE_UNKOWN			255

// 播放控制码
#define AUDIO_CTRL_PLAY				0x0001		// 播放
#define AUDIO_CTRL_PAUSE			0x0002		// 暂停
#define AUDIO_CTRL_RESUME			0x0003		// 继续
#define AUDIO_CTRL_STOP				0x0004		// 停止
#define AUDIO_CTRL_CLOSE			0x0005		// 关闭
#define AUDIO_CTRL_FORWARD			0x0006		// 快进
#define AUDIO_CTRL_BACKWARD			0x0007		// 快退
#define AUDIO_CTRL_SEEK				0x0008		// 定位
#define AUDIO_CTRL_SETAB			0x0009		// 设置AB点
#define AUDIO_CTRL_SETEQ			0x000A		// 设置均衡
#define AUDIO_CTRL_DURATION			0x000B		// 返回时间
#define AUDIO_CTRL_VOLUME			0x000C		// 音量控制
#define AUDIO_CTRL_TEMPO			0x000D		// 设置语音速度

// 状态获取控制码
#define AUDIO_GET_STATUS			0x0000		// 获取播放状态(暂停、快进等)
#define AUDIO_GET_ENFLAG			0x0001		// 获取播放控制允许标志
#define AUDIO_GET_TIME				0x0011		// 获取当前播放时间
#define AUDIO_GET_ALLTIME			0x0012		// 获取总的播放时间
#define AUDIO_GET_ABPLAY			0x0013		// 获取AB播放状态
#define AUDIO_GET_ATIME				0x0014		// 获取A点时间
#define AUDIO_GET_BTIME				0x0015		// 获取B点时间
#define AUDIO_GET_REPEADAB			0x0016		// 获取重复播放状态
#define AUDIO_GET_EQ				0x0017		// 获取均衡状态
#define AUDIO_GET_SPEED				0x0018		// 获取速度状态
#define AUDIO_GET_VOLUME			0x0019		// 获取音量
#define AUDIO_GET_OFFSET			0x001a		// 获取数据读取偏移
#define AUDIO_GET_VOLENHANCE		0x001b		// 获取音量增强值

// 播放状态码
#define AUDIO_ST_RECORD				0x1000		// 正在播放状态
#define AUDIO_ST_READY				0x0000		// 播放器处于就绪态
#define AUDIO_ST_PLAY				0x0001		// 正在播放状态
#define AUDIO_ST_PAUSE				0x0002		// 暂停状态
#define AUDIO_ST_STOP				0x0003		// 关闭状态
#define AUDIO_ST_FORWARD			0x0004		// 快进状态
#define AUDIO_ST_BACKWARD			0x0005		// 快退状态

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

