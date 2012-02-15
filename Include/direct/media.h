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

#ifndef _DIRECT_MEDIA_H
#define _DIRECT_MEDIA_H

#include <config.h>
#include <kernel/kernel.h>
#include <gui/wintype.h>

#if defined(WIN32)
#include "./Win32/media.h"
#endif

#if defined(CONFIG_MCU_AK3620) && !defined(WIN32)
#include "./AK3620/Audio.h"
#include "./AK3620/Vedio.h"
#endif

#if defined(CONFIG_MCU_AK8802) && !defined(WIN32)
#include "./AK8802/media_player_lib.h"
#include "./AK8802/media.h"
#endif

#if (defined(CONFIG_MCU_JZ4750) || defined(CONFIG_MCU_JZ4750L) || defined(CONFIG_MCU_JZ4755) || defined(CONFIG_MCU_JZ4760B)) && !defined(WIN32)
#include "./Jz4750/media.h"
#include "./Jz4750/mplayer_head.h"
#include "./Jz4750/ipu_app.h"
#endif

#define INNER_POWAMP_ENABLE			// 定义是否使用内部功放
//#define DA_OPEN_ALWAYS				// 定义是否一直打开DA设备

#define MEDIA_TYPE_AUDIO		0x010000
#define MEDIA_TYPE_VEDIO		0x020000
#define MEDIA_TYPE_VIDEO		MEDIA_TYPE_VEDIO
#define MEDIA_TYPE_NES			0x040000
#define MEDIA_TYPE_JPEG			0x080000
#define MEDIA_TYPE_FLASH		0x100000

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
#define WM_MEDIA_VOLUME_CHANGED     0x110a		// 音量值被系统改变

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
#define MEDIA_CTRL_HEADPHONE_IN		0x0500		
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

typedef HANDLE (*MEDIA_FUNC_CREATE)(void*);
typedef int (*MEDIA_FUNC_DESTROY)(HANDLE);
typedef int (*MEDIA_FUNC_OPEN)(HANDLE);
typedef int (*MEDIA_FUNC_CLOSE)(HANDLE, int);
typedef int (*MEDIA_FUNC_CTRL)(HANDLE, int, DWORD*, int*);
typedef int (*MEDIA_FUNC_INFO)(void*, void*, int);

typedef struct _MEDIA_CALLBACK
{
	MEDIA_FUNC_CREATE MediaCreate;
	MEDIA_FUNC_DESTROY MediaDestroy;
	MEDIA_FUNC_OPEN MediaOpen;
	MEDIA_FUNC_CLOSE MediaClose;
	MEDIA_FUNC_CTRL MediaCtrl;
}MEDIA_CALLBACK;
typedef MEDIA_CALLBACK *PMEDIA_CALLBACK;

typedef struct _MEDIA_OBJECT
{
	KERNEL_OBJECT Header;		// 内核对象信息
	LIST Link;
	HANDLE hTask;
	int Type;					// 多媒体数据类型(音频、视频、游戏、Flash...)
	int Mode;					// 播放模式
	void *Media;
	char *MediaInfo;
	DWORD Device;				// 使用设备
	MEDIA_CALLBACK Cb;
}MEDIA_OBJECT;
typedef MEDIA_OBJECT *PMEDIA_OBJECT;

typedef struct _MEDIA_CBPARAM
{
	int bRead;
	int Offset;
	int Count;
	int Return;
}MEDIA_CBPARAM;
typedef MEDIA_CBPARAM *PMEDIA_CBPARAM;

#define MEDIA_DEVICE_AUDIO		(1<<0)		// 音频设备
#define MEDIA_DEVICE_VEDIO		(1<<1)		// 视频、图像设备
#define MEDIA_DEVICE_VIDEO		MEDIA_DEVICE_VEDIO		// 视频、图像设备

// 音频播放抢占模式定义
#define MEDIA_MODE_NORMAL			0x0000		// 普通播放模式
#define MEDIA_MODE_WAIT				0x0001		// 等待播放
#define MEDIA_MODE_PREEMPTIVE		0x0002		// 抢占式播放模式

void MediaDeviceInit(void);
int MediaDeviceLock(PMEDIA_OBJECT obj);
int MediaDeviceUnlock(PMEDIA_OBJECT obj);
int MediaDeviceCreate(DWORD device);
PMEDIA_OBJECT MediaDevicePoll(DWORD device);

void MediaSrvInit(void);
void MediaSrvLock(void);
void MediaSrvUnlock(void);
int MediaSrvPowerOff(int mode);
int MediaSrvMode(void);
int MediaSrvRegistCallback(int type, DWORD device, PMEDIA_CALLBACK callback);
int MediaSrvDestroyNotify(void *media);

#if defined(STC_EXP)
HANDLE sMediaSrvCreate(void* media, char *name, int type, int preempt);
int sMediaSrvDestroy(HANDLE hmedia);
int sMediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size);
int sMediaSrvGetName(HANDLE hmedia, char *name, int max);
int sMediaSrvInfo(void *media, void *info, int type);
int sMediaSrvMaster(char *name, int max, int type);
#else
HANDLE MediaSrvCreate(void* media, char *name, int type, int preempt);
int MediaSrvDestroy(HANDLE hmedia);
int MediaSrvCtrl(HANDLE hmedia, int ctl, DWORD *param, int *size);
int MediaSrvGetName(HANDLE hmedia, char *name, int max);
int MediaSrvInfo(void *media, void *info, int type);
int MediaSrvMaster(char *name, int max, int type);
#endif

#endif	// _DIRECT_MEDIA_H

