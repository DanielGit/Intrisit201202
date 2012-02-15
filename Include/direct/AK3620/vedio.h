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

#ifndef _DIRECT_VEDIO_H
#define _DIRECT_VEDIO_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(CONFIG_MCU_AK3620) && !defined(WIN32)
#include "movie_player_lib.h"
#include "VedioPortShow.h"
#include "VedioPortCam.h"
#include "VedioPortFs.h"
#include "VedioPortMem.h"
#include "VedioPortShow.h"
#endif

typedef struct _VEDIO_FILE
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
	int Asid;				// 用于读取用户空间使用
#endif	
}VEDIO_FILE;
typedef VEDIO_FILE *PVEDIO_FILE;

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

typedef struct _VEDIO_CTRL
{
	LIST Link;
	int Code;
	DWORD *Param;
	int Size;
	int Return;
	HANDLE hSema;
	HANDLE hMutex;
	DWORD ParamData;
}VEDIO_CTRL;
typedef VEDIO_CTRL *PVEDIO_CTRL;

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

typedef struct _VEDIO_OBJECT
{
	KERNEL_OBJECT Header;	// 内核对象信息
	LIST Link;
	int bGetInfo;
#if defined(CONFIG_MMU_ENABLE)
	int Asid;				// 用于读取用户空间使用
#endif	
	VEDIO_TASK Task;
	VEDIO_CTRL Ctrl;		// 控制信息
	VEDIO_FILE File;		// 数据缓冲区管理结构体
	VEDIO_INFO Info;		// 数据信息结构体
}VEDIO_OBJECT;
typedef VEDIO_OBJECT *PVEDIO_OBJECT;

#define VEDIO_LOCK()		//	MediaSrvLock()
#define VEDIO_UNLOCK()		//  MediaSrvUnlock()

// VedioCtl.c
int VedioCtlInit(void);
int VedioCtlVolumeSet(PVEDIO_OBJECT obj, int volume);
int VedioCtlPause(PVEDIO_OBJECT obj);
int VedioCtlResume(PVEDIO_OBJECT obj);
int VedioCtlSeek(PVEDIO_OBJECT obj, DWORD milliSecond);
int VedioCtlGetDuration(PVEDIO_OBJECT obj, DWORD *duration);
int VedioCtlIsPlaying(PVEDIO_OBJECT obj);
int VedioCtlPhonePlus(void);
void VedioCtlPipo(int ena);
int VedioCtlForward(PVEDIO_OBJECT obj);
int VedioCtlBackward(PVEDIO_OBJECT obj);

// VedioThread.c
int VedioThreadInit(void);
void VedioThreadClose(PVEDIO_OBJECT obj, int code);
void VedioThreadOpen(PVEDIO_OBJECT obj);
int VedioThreadRun(void);

// VedioSrv.c
void VedioSrvInit(void);
int VedioSrvDestroyNotify(PVEDIO_TASK task);
int VedioSrvPowerOff(int mode);
int VedioSrvMode(void);
void VedioSrvRefreshFrame(BYTE *srcImg, short src_width, short src_height);
void VedioSrvProcMsg(PVEDIO_OBJECT obj);
int VedioSrvInfo(void *media, void *info);

#endif	// _DIRECT_VEDIO_H

