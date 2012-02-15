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

#include <kernel/kernel.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/device.h>
#include <kernel/karch.h>
#include <platform/platform.h>
#include <direct/media.h>

extern int DacInit(void);
extern int AdcInit(void);
extern int DacSetTempo(HANDLE hdac, int tempo);
extern int AdcSetEq(HANDLE hadc, int filter);
extern void SetMuteMode(char mode);
extern int MplayerResizeVideo(PAK_OBJECT obj,PMEDIA_TASK task);
extern void MillinsecoundDelay(unsigned int msec);
extern void SetPowerAmplifier(char io);
extern int GetDacChannel(void);
//int seek_flag = 0;
////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlInit(void)
{	
	// ADC & DAC设备初始化
	AdcInit();
	DacInit();
	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlVolumeSet(PAK_OBJECT obj, int volume)
{
	obj->Task.Volume = volume;
	if(obj->Task.Record)
		return AdcSetVolume(obj->hDac, volume);
	return DacSetVolume(obj->hDac, volume);
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlPause(PAK_OBJECT obj)
{
	// 检查是否为录音状态
	if(obj->Task.Record)
	{
		obj->bRecPause = 1;
		return 0;
	}

	MediaLibPause(obj);

	obj->bPlayerPause = 1;
	// 如果好几路声音在播放, 不能静音
	// 注意GetDacChannel()函数里面使用了互斥量,
	// 而JzCtlPause()被调用时也有使用互斥量, 小心死锁
	if(GetDacChannel() <= 1)
	{
		SetPowerAmplifier(0);
		MillinsecoundDelay(20);
		SetMuteMode(0);				//关闭声音
	}
	return 0;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlForward(PAK_OBJECT obj)
{
	return -1;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlBackward(PAK_OBJECT obj)
{
	return -1;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlResume(PAK_OBJECT obj)
{
	// 检查是否为录音状态
	if(obj->Task.Record)
	{
		obj->bRecPause = 0;
		return 0;
	}

	MediaLibResume(obj);
	obj->bPlayerPause = 0;
	// 无论有多少路声音在播放, 总是要打开声音的.
	SetMuteMode(1);		//打开声音通道
	MillinsecoundDelay(20);
	SetPowerAmplifier(1);
	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlSeek(PAK_OBJECT obj, DWORD milliSecond)
{
 	MEDIALIB_STATUS player_status;
	
	// 检查是否为录音状态
	if(obj->Task.Record)
		return -1;
	
	// 检查是否允许seek动作
	if(!obj->Info.bAllowSeek)
	{
		kdebug(mod_media, PRINT_ERROR, "the media unsupport seeking!\n");
		return -1;
	}
 
 	// 暂停播放
 	player_status = MediaLibGetStatus(obj);
	if(player_status == MEDIALIB_SEEK)
		return 0;

	if((player_status == MEDIALIB_PLAYING) || (player_status == MEDIALIB_PAUSE))
	{
		MediaLibSeek(obj, milliSecond);
	}
	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlGetDuration(PAK_OBJECT obj, DWORD *duration)
{
	int ret;
	
	// 检查是否为录音状态
	if(obj->Task.Record)
	{
		*duration = obj->RecDuration;
		return 0;
	}

	ret = obj->RecDuration;
	if(ret >= 0)
	{
		*duration = ret;
		return 0;
	}
	return -1;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlIsPlaying(PAK_OBJECT obj)
{
	// 检查是否为录音状态
	if(obj->Task.Record)
	{
		if(obj->bRecPause)
			return 1;
		return 0;
	}

	if( obj->bPlayerPause )
		return 1;

	if( MediaLibGetStatus(obj) == MEDIALIB_PLAYING )
		return 1;
 	return 0;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlPlaySpeed(PAK_OBJECT obj, int tempo)
{
	if(obj->Task.Record)
		return -1;
	return DacSetTempo(obj->hDac, tempo);
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlEq(PAK_OBJECT obj, int eq)
{
	if(obj->Task.Record)
		return AdcSetEq(obj->hDac, eq);
	return -1;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlResize(PAK_OBJECT obj)
{
	kdebug(mod_media, PRINT_INFO, "resize: x = %d, y = %d, w = %d, h = %d\n",
		obj->Task.ShowRect.x,obj->Task.ShowRect.y,obj->Task.ShowRect.w,obj->Task.ShowRect.h);
	return MplayerResizeVideo(obj,&obj->Task);
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlGetTotalTime(PAK_OBJECT obj, DWORD* totaltime)
{
	*totaltime = obj->Info.TotalTime;
	kdebug(mod_media, PRINT_INFO, "get total time : %d\n",*totaltime);
	return 1;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCtlSetAB(PAK_OBJECT obj,DWORD start, DWORD end)
{
	int ret;

	kprintf("JzCtlSetAB: start = %d, end = %d\n",start,end);
	if( end <= start )
		return -1;

	ret = -1;
	ret = JzCtlSeek(obj,start);
	if(  ret < 0 )
	{
		kprintf("JzCtlSetAB: seek error,ret = %d\n",ret);
		return -1;
	}
	obj->EndTime = end;
	return ret;
}
