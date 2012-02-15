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
#include <gui/wintype.h>


#ifndef _VEDIO_H
#define _VEDIO_H

typedef struct _VEDIO_INFO
{
	int Type;
	int MovieWidth;
	int MovieHeight;
	int FPS;
	DWORD TotalTime;
	DWORD VedioBitrate;
	DWORD AudioBitrate;
}VEDIO_INFO;
typedef VEDIO_INFO *PVEDIO_INFO;

typedef struct _VEDIO_TASK
{
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
	int Eq;					// 音频播放EQ模式

	DWORD ABitrate;			// 音频录制波特率
	DWORD ASamplerate;		// 音频录制采样率
	DWORD VBitrate;			// 视频录制波特率
	DWORD Device;			// 录放设备定义：MIC/LINE-IN
	
	void *Extend;			// 附加数据
}VEDIO_TASK;
typedef VEDIO_TASK *PVEDIO_TASK;


#endif //_VEDIO_H

