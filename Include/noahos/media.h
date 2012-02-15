//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//       	        编码解码器部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
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

#define MEDIA_TYPE_AUDIO		0x10000
#define MEDIA_TYPE_VEDIO		0x20000
#define MEDIA_TYPE_VIDEO		MEDIA_TYPE_VEDIO
#define MEDIA_TYPE_NES			0x30000
#define MEDIA_TYPE_JPEG			0x40000
#define MEDIA_TYPE_FLASH		0x50000

// 音频播放消息ID定义
#define WM_MEDIA_OPEN				0x1100		// 媒体设备打开时间
#define WM_MEDIA_CLOSE				0x1101		// 媒体设备关闭时间
#define WM_MEDIA_TIME				0x1102		// 媒体设备时间返回
#define WM_MEDIA_SETA				0x1103		// 媒体设备设置A点信息
#define WM_MEDIA_SETB				0x1104		// 媒体设备设置B点信息
#define WM_MEDIA_STARTAB			0x1105		// 媒体设备开始A-B播放
#define WM_MEDIA_QUITAB				0x1106		// 媒体设备退出A-B播放
#define WM_MEDIA_SUSPEND			0x1107		// 媒体设备被挂起
#define WM_MEDIA_RESUME				0x1108		// 媒体设备退出挂起
#define WM_MEDIA_TERMINATE			0x1109		// 用户调用AUDIO_CTL_CLOSE后发送的消息
#define WM_MEDIA_LOWBAT				0x110a		// 低电压、结束播放退出
#define WM_MEDIA_VOLUME_CHANGED     0x110b		// 音量值被系统改变

// 音频播放抢占模式定义
#define MEDIA_MODE_NORMAL			0x0000		// 普通播放模式
#define MEDIA_MODE_WAIT				0x0001		// 等待播放
#define MEDIA_MODE_PREEMPTIVE		0x0002		// 抢占式播放模式

// 状态获取控制码
#define MEDIA_CTRL_PLAY				0x0001		// 播放
#define MEDIA_CTRL_PAUSE			0x0002		// 暂停
#define MEDIA_CTRL_RESUME			0x0003		// 继续
#define MEDIA_CTRL_STOP				0x0004		// 停止
#define MEDIA_CTRL_CLOSE			0x0005		// 关闭
#define MEDIA_CTRL_FORWARD			0x0006		// 快进
#define MEDIA_CTRL_BACKWARD			0x0007		// 快退
#define MEDIA_CTRL_SEEK				0x0008		// 定位
#define MEDIA_CTRL_SETAB			0x0009		// 设置AB点
#define MEDIA_CTRL_SETEQ			0x000A		// 设置均衡
#define MEDIA_CTRL_DURATION			0x000B		// 返回时间
#define MEDIA_CTRL_VOLUME			0x000C		// 音量控制
#define MEDIA_CTRL_TEMPO			0x000D		// 设置语音速度
#define MEDIA_CTRL_RECT				0x000E		// 显示矩形
#define MEDIA_CTRL_ISPLAY			0x000F		// 检查是否播放
#define MEDIA_CTRL_TOTALTIME		0x0010		// 得到当前播放的总时间
#define MEDIA_CTRL_OVOLUME_SET		0x0100		// 全局输出音量设置
#define MEDIA_CTRL_OVOLUME_GET		0x0200		// 全局输出音量获取
#define MEDIA_CTRL_IVOLUME_SET		0x0300		// 全局输入音量设置
#define MEDIA_CTRL_IVOLUME_GET		0x0400		// 全局输入音量获取
#define MEDIA_CTRL_HEADPHONE_IN		0x0500		// 
#define MEDIA_CTRL_MIDI_DIRECT		0x0600		// MIDI音色库地址
#define MEDIA_CTRL_HEADPHONE_VOL_SET	0x0100	// 全局输出音量设置
#define MEDIA_CTRL_HEADPHONE_VOL_GET	0x0200	// 全局输出音量获取
#define MEDIA_CTRL_SPEAKER_VOL_SET		0x0101	// 全局输出音量设置
#define MEDIA_CTRL_SPEAKER_VOL_GET		0x0201	// 全局输出音量获取

// 多媒体播放数据获取/写入回调函数
typedef int (*MEIDA_CALLBACK)(DWORD, int, int, int);
// 函数返回：读写的字节数
// HANDLE hmedia: 媒体句柄
// int bRead: 1-读操作 0-写操作
// int offset: 读写数据缓冲区偏移
// int count: 读写数据字节数

HANDLE MediaSrvCallbackCreate(MEIDA_CALLBACK callback, DWORD id);
HANDLE MediaSrvCreate(void* media, char *name, int type, int preempt);
int MediaSrvDestroy(HANDLE hmedia);
int MediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size);
int MediaSrvGetName(HANDLE hmedia, char *name, int max);
int MediaSrvInfo(void *media, void *info, int type);
int MediaSrvMaster(char *name, int max, int type);


#endif //_MEDIA_H

