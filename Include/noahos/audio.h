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


#ifndef _AUDIO_H
#define _AUDIO_H

// 设置播放文件类型
#define AUDIO_PLAY_TYPE_UNKNOWN			0
#define AUDIO_PLAY_TYPE_MIDI			1
#define AUDIO_PLAY_TYPE_MP3				2
#define AUDIO_PLAY_TYPE_AMR				3
#define AUDIO_PLAY_TYPE_WAVE			4
#define AUDIO_PLAY_TYPE_WMA				5
#define AUDIO_PLAY_TYPE_PCM				6
#define AUDIO_PLAY_TYPE_MELODY			7
#define AUDIO_PLAY_TYPE_MP3_I			8
#define AUDIO_PLAY_TYPE_MP3_II			9
#define AUDIO_PLAY_TYPE_AAC				10
#define AUDIO_PLAY_TYPE_NES				11
#define AUDIO_PLAY_TYPE_BT				12
#define AUDIO_PLAY_TYPE_SNES			13
#define AUDIO_PLAY_TYPE_LPC_WMA			14
#define AUDIO_PLAY_TYPE_FLAC_NATIVE		15
#define AUDIO_PLAY_TYPE_FLAC_OGG		16

// 设置录音文件类型
#define AUDIO_REC_TYPE_WAVE				0
#define AUDIO_REC_TYPE_PCM    			101
#define AUDIO_REC_TYPE_AMR				102
#define AUDIO_REC_TYPE_MP3				103
#define AUDIO_REC_TYPE_UNKOWN			255

typedef struct _AUDIO_INFO
{
	int Type;
	DWORD TotalTime;
	DWORD Bitrate;
	DWORD Samplerate;
}AUDIO_INFO;
typedef AUDIO_INFO *PAUDIO_INFO;

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

#endif //_AUDIO_H

