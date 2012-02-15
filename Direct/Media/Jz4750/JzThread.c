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
#include <kernel/sync.h>
#include <kernel/timer.h>
#include <platform/platform.h>
#include <direct/media.h>
#include <gui/winmsg.h>
#include <filesys/fs.h>

//#define	MPLAYER_DEBUG
#if defined(CONFIG_MCU_JZ4750L)
#define RECORD_FADE		1400
#elif defined(CONFIG_MCU_JZ4755)
#define RECORD_FADE		400
#else
#define RECORD_FADE		400
#endif

#if defined(CONFIG_MAC_BDS6100) || defined(CONFIG_MAC_ND800) || defined(CONFIG_MAC_ASKMI1388) || defined(CONFIG_MAC_BDS6100A)
#define RECODE_SKIP_LEN	400		//录音结束以后，清除最后的录音数据，防止录入按间声音
#else
#define RECODE_SKIP_LEN	0		//录音结束以后，清除最后的录音数据，防止录入按间声音
#endif

typedef struct
{
	HANDLE hMutex;
	PAK_OBJECT obj;
	BYTE  fPlayVideo;
	DWORD audio_frame_time;
	DWORD video_frame_time;
}VIDEO_PARAM;
typedef VIDEO_PARAM *PVIDEO_PARAM;

static WAVE_HEADER WaveFileHead = 
{
	"RIFF",
	0,
	"WAVEfmt ",		//"WAVEfmt "
	0x10,			//in bytes (16 for PCM)
	0x1,			//1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
	1,				//1=mono, 2=stereo
	8000,			//default 8k sample rate
	0,
	2,				//bits_per_samp / 8 * num_chans
	16,				//default 16bits
	"data",			//"data"
	0
};

// static unsigned long bmp_id = 0;
// extern int seek_flag;
extern void SetMoseCe(char io);
extern void SetMuteMode(char mode);
extern int LcdcVideoOsdOpen(int x,int y,int w,int h);
extern BYTE* LcdcVideoOsdBuf(void);
extern int  GetDacChannel();
extern void MxuEnable(void);
extern void WinClipMask(RECT *rect);
extern void SetVideoPosAndSize(PAK_OBJECT obj,PMEDIA_TASK task);
extern int MplayerResizeVideo(PAK_OBJECT obj,PMEDIA_TASK task);
extern void dma_cache_wback_inv(unsigned long addr, unsigned long size);
extern void MediaClearUsrScreen( PMEDIA_TASK task,BYTE format );
extern void MillinsecoundDelay(unsigned int msec);
extern int GetDacBufCountMs();
extern int syslog_set_core_mask(unsigned long mask);
extern int syslog_set_global_level(char level);

BYTE* pMediaPcmData = NULL;
DWORD nMediaPcmLen = 0;
BYTE  fPlayMedia = 0;
BYTE  fRecodeWav = 0;
////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void JzThreadGetInfo(DWORD p)
{
 	PAK_VFILE vfile;
 	PAK_OBJECT obj;
	PMEDIA_TASK task;
	int ret;
	
	// 获取打开信息
 	obj = (PAK_OBJECT)p;
 	vfile = &obj->File;
	task = &obj->Task;
	vfile = &obj->File;
#ifdef CONFIG_MAKE_BIOS
	task->ExterdType = 0;
#endif
	obj->PlayerType = task->ExterdType;

	//初始化回调函数
	if((ret = MediaLibInit(obj)) < 0 )
	{
		kdebug(mod_media, PRINT_ERROR, "MediaLibInit failed.\n");
		*obj->bReady = -1;
		if( ret == -2 )
			goto ThreadEnd1;
		else
			goto ThreadEnd;
	}

	//打开音频库
	if( MediaLibOpen(obj, 1) < 0 )
	{	//音频库打开失败
		kdebug(mod_media, PRINT_ERROR, "MediaLibOpen failed.\n");
		*obj->bReady = -1;
 		goto ThreadEnd;
	}

	//得到音频库信息
	if( MediaLibGetInfo(obj) < 0 )
	{	//得到音频库信息失败
		kdebug(mod_media, PRINT_ERROR, "MediaLibGetInfo failed.\n");
		*obj->bReady = -1;
 		goto ThreadEnd;
	}
	if( obj->Info.bHasVideo )
	{	//视频文件，需要计算视频的显示尺寸
		SetVideoPosAndSize(obj,task);
	}
	obj->bGetInfo = 0;
	*obj->bReady = 1;
	
ThreadEnd:
	MediaLibClose(obj);
ThreadEnd1:

	// 设置线程结束标志	
	if(obj->bTerminate)
		obj->bTerminate++;

#if defined(STC_EXP)
	sThreadTerminate(sThreadSelf());
#else
	ThreadTerminate(ThreadSelf());
#endif
}



////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void AkThreadVideoPlay(DWORD p)
{
	PVIDEO_PARAM video = (PVIDEO_PARAM)p;
	PAK_OBJECT obj;
	DWORD pre_audio_time;
	DWORD audio_time;
	DWORD frame_time;
	DWORD delay_time;
	DWORD totle_time;
	DWORD totle_fps;
	DWORD back_time;
	DWORD back_count;
	RECT *show_rect;
	int pts;
	int pass_frame;
	MEDIALIB_STATUS player_status;
			
	frame_time = 0;
	audio_time = 0;
	pre_audio_time = 0;
	pass_frame = 0;
	delay_time = 0;
	totle_time = 0;
	totle_fps  = 0;
	video->fPlayVideo = 1;
	frame_time = TimerCount();
	obj = video->obj;
	MxuEnable();

	show_rect = &obj->Task.ShowRect;
	WinClipMask(show_rect);
	while(!obj->bClose && !obj->bTerminate)
	{
		kMutexWait(video->hMutex);

		// 获取音频时间	
		audio_time = obj->RecDuration;
		
		// 矫正同步时间
		{
			DWORD tick;
			tick = TimerCount();
			pts  = MediaLibGetYuv(obj);
			totle_time += TimerCount() - tick;
			totle_fps++;
		}
		
//		kprintf("%d,%d\n",pts,audio_time);
		if(pts < 0)
		{
			player_status = MediaLibGetStatus(obj);
			if (MEDIALIB_ERR == player_status)
			{
				kdebug(mod_video, PRINT_ERROR, "video decode error!\n");
				obj->bClose = 2;
				kMutexRelease(video->hMutex);
				break;
			}
			else if(MEDIALIB_PAUSE == player_status)
			{
				kMutexRelease(video->hMutex);
				
				// 等待退出暂停状态
				do
				{
					sTimerSleep(100, NULL);
					kMutexWait(video->hMutex);
					player_status = MediaLibGetStatus(obj);
					kMutexRelease(video->hMutex);
				}while((MEDIALIB_PAUSE == player_status) && !obj->bClose && !obj->bTerminate);
				
				// 重新设置系数				
				frame_time = 0;
				audio_time = 0;
				pre_audio_time = 0;
				frame_time = TimerCount();
				
				// 继续解压循环
				continue;
			}
			else
			{
				obj->bClose = 1;
				kMutexRelease(video->hMutex);
				break;
			}
		}
		player_status = MediaLibGetStatus(obj);
		if(player_status == MEDIALIB_ERR)
		{
			kdebug(mod_video, PRINT_ERROR, "video decode error!\n");
			obj->bClose = 2;
			kMutexRelease(video->hMutex);
			break;
		}

		kMutexRelease(video->hMutex);

		// 获取Sleep时间
		if((DWORD)pts > audio_time && player_status == MEDIALIB_PLAYING )
		{
			delay_time += (DWORD)pts-audio_time;
			if( delay_time >= 50 )
			{	//延迟时间大于100ms,防止seek以后，VIDEO立即执行到SEEK点，PTS正确，但是
				//音频时间没有马上到SEEK点，延后，导致停止时间过长，VIDEO卡住
//				kprintf("pts = %d,video delay = %d\n",(DWORD)pts,delay_time);
				back_time  = delay_time;
				back_count = 0;
				while(1)
				{
					kMutexWait(video->hMutex);
					player_status = MediaLibGetStatus(obj);
					audio_time = obj->RecDuration;
					kMutexRelease(video->hMutex);

					if( obj->bClose || obj->bTerminate )
						break;

					if( (DWORD)pts <= audio_time )
					{	//延迟结束
						delay_time = 0;
						break;
					}

					//因为VIDEO开始IPU以后，才会对AUDIO进行解琐，所以防止VIDEO等待时间中，AUDIO由于没有解琐，
					//而导致obj->RecDuration无变化，导致死循环
					if( 50 * back_count >= back_time )
						break;

					delay_time = (DWORD)pts - audio_time;
//					kprintf("video fac delay = %d,audio_time = %d, cur_time = %d\n",delay_time,audio_time,cur_time);
					if( delay_time > 50 )
					{
						sTimerSleep(50, NULL);
						back_count++;
					}
					else
					{
						sTimerSleep(delay_time, NULL);
						delay_time = delay_time %10;
						break;
					}
				}
			}
			else
			{	//小于100ms的延迟，直接延迟
				sTimerSleep(delay_time, NULL);
				delay_time = delay_time % 10;
			}
		}

		// 等待退出暂停状态
		while( MEDIALIB_PAUSE == player_status && !obj->bClose && !obj->bTerminate)
		{
			sTimerSleep(100, NULL);

			kMutexWait(video->hMutex);
			player_status = MediaLibGetStatus(obj);
			kMutexRelease(video->hMutex);
		}
	}
	WinClipMask(NULL);
	kdebug(mod_video, PRINT_INFO, "totle_fps = %d, total_time = %d\n",totle_fps,totle_time);
	kdebug(mod_video, PRINT_INFO, "video thread close\n");
	video->fPlayVideo = 0;
#if defined(STC_EXP)
	sThreadTerminate(sThreadSelf());
#else
	ThreadTerminate(ThreadSelf());
#endif
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void JzThreadPlayFile(DWORD p)
{
	PAK_VFILE vfile;
	PAK_OBJECT obj;
	PMEDIA_TASK task;
	VIDEO_PARAM video;
	MEDIALIB_STATUS player_status;
	MESSAGE msg;
	int cur_time;
	int duration;
	int msgid;
	int ret,jmp_ret;
	int max_wsize;
	int max_time;
	short *pcm,*pData;
	int fade,begin_time;
 	int obj_del = 1;
	int wlen;
	int block_num;
	DWORD init_start_time,init_end_time;

#ifdef MPLAYER_PRINTF_ENABEL
	syslog_set_global_level(PRINT_INFO);
#endif

	// 获取打开信息
	obj = (PAK_OBJECT)p;
	task = &obj->Task;
	vfile = &obj->File;
	init_start_time = TimerCount();
	kdebug(mod_audio, PRINT_INFO, "mplayer init player time = %d\n",init_start_time);
	obj->PlayerType = task->ExterdType;
#ifdef CONFIG_MAKE_BIOS
	task->ExterdType = 0;
#endif
	pcm          = NULL;
	obj->hDac    = NULL;
	video.hMutex = NULL;

	//初始化回调函数
	if( (ret = MediaLibInit(obj)) < 0 )
	{
		kdebug(mod_audio, PRINT_ERROR, "MediaLibInit failed\n");
		*obj->bReady = -1;
		if( ret != -2 )
			MediaLibClose(obj);
 		goto ThreadEnd;
	}
	kdebug(mod_audio, PRINT_INFO, "mplayer MediaLibInit end = %d\n",TimerCount());

	if( task->ExterdType )
	{
		//防止SD卡播放时，拔出SD卡，导致MPLAYER在初始化阶段死机
		obj->JzAvDecoder->fMplayerInit = 1;
		kdebug(mod_audio, PRINT_INFO, "set jump error addr\n");
		jmp_ret = ksetjmp(obj->JzAvDecoder->AudioJumpBuf);
		if( jmp_ret == 1 )
		{
			kdebug(mod_audio, PRINT_ERROR, "audio error, jump to end\n");
			*obj->bReady = -1;
			if( pcm )
				kfree(pcm);
			if( obj->hDac )
				DacClose(obj->hDac);
			if(video.hMutex)
				kMutexDestroy(video.hMutex);
			MediaLibClose(obj);
			goto ThreadEnd;
		}
	}

	//打开音频库
	if( MediaLibOpen(obj, 0) < 0 )
	{	//音频库打开失败
		kdebug(mod_audio, PRINT_ERROR, "MediaLibOpen failed\n");
		*obj->bReady = -1;
		MediaLibClose(obj);
 		goto ThreadEnd;
	}
	kdebug(mod_audio, PRINT_INFO, "mplayer MediaLibOpen end = %d\n",TimerCount());

	//得到音频库信息
	if( MediaLibGetInfo(obj) < 0 )
	{	//得到音频库信息失败
		kdebug(mod_audio, PRINT_ERROR, "MediaLibGetInfo failed\n");
		*obj->bReady = -1;
		MediaLibClose(obj);
 		goto ThreadEnd;
	}

	if( !obj->Info.bHasAudio  )
	{	//没有音频数据，直接退出
		*obj->bReady = -1;
		MediaLibClose(obj);
 		goto ThreadEnd;
	}

	if(obj->Info.bHasVideo)
	{
		//有视频，但是播放区域为空，直接退出（MP3里面播放改名的RMVB等。。。。）
		if( !(task->ShowRect.w  && task->ShowRect.h) )
		{
			*obj->bReady = -1;
			MediaLibClose(obj);
 			goto ThreadEnd;
		}
	}
	kdebug(mod_audio, PRINT_INFO, "mplayer MediaLibGetInfo end = %d\n",TimerCount());

	if( obj->PlayerType )
	{	//设置视频的显示区域以及位置
		if( task->ShowRect.w  && task->ShowRect.h )
			MplayerResizeVideo(obj,task);
		else
		{
			kdebug(mod_audio, PRINT_WARNING, "can't play video\n");
			obj->JzAvDecoder->fIpuEnable = 1;
		}
	}
	else
	{
		obj->StartTime = task->ShowRect.x;
		obj->EndTime   = task->ShowRect.y;
	}

	//申请播放PCM的内存
	block_num = 8192 * 4;
	pcm = (short*)kmalloc(block_num);
	if( pcm == NULL )
	{
		kdebug(mod_audio, PRINT_ERROR, "memory lose pcm == null\n");
		*obj->bReady = -1;
		MediaLibClose(obj);
		goto ThreadEnd;
	}
	kdebug(mod_audio, PRINT_INFO, "mplayer kmalloc pcm end = %d\n",TimerCount());

	//MPLAYER播放器使用,用来设置从MPLAYER读到的PCM数据以及长度
	if( obj->PlayerType )
	{	//外部播放器使用变量
		pMediaPcmData = (BYTE*)pcm;
		nMediaPcmLen  = 0;
	}

	// 打开DAC设备，并设置DAC设备采样率、声道
	*obj->bReady = 1;
	obj->hDac = DacOpen();
	if(obj->hDac == NULL)
	{
		kdebug(mod_audio, PRINT_ERROR, "dac open error\n");
		kfree(pcm);
		MediaLibClose(obj);
		goto ThreadEnd;
	}
	if(DacSetSamplerate(obj->hDac, obj->Info.AudioSamplerate, obj->Info.AudioChannels) < 0)
	{
		kdebug(mod_audio, PRINT_ERROR, "set samplerate error\n");
		kfree(pcm);
		DacClose(obj->hDac);
		MediaLibClose(obj);
		goto ThreadEnd;
	}

	kdebug(mod_audio, PRINT_INFO, "mplayer dacopen end = %d\n",TimerCount());
	DacSetTempo(obj->hDac, obj->Task.Tempo);
	DacSetVolume(obj->hDac, obj->Task.Volume);
	kdebug(mod_audio, PRINT_INFO, "volume = %d, tempo=%d\n",obj->Task.Volume, obj->Task.Tempo);


	// 计算每次写入PCM数据最大值(1K的整数倍)
	max_wsize =  50 * (obj->Info.AudioSamplerate * obj->Info.AudioChannels * sizeof(short)) / 1000;
	max_wsize = ((max_wsize + 1023) / 1024) * 1024;
	max_time = (max_wsize * 1000) / (obj->Info.AudioSamplerate * obj->Info.AudioChannels * sizeof(short));

	begin_time = MediaLibPlay(obj);
	if( begin_time < 0 )
	{
		kdebug(mod_audio, PRINT_ERROR, "MediaLibPlay failed\n");
		if(video.hMutex)
			kMutexDestroy(video.hMutex);
		*obj->bReady = -1;
		DacClose(obj->hDac);
		MediaLibClose(obj);
		kfree(pcm);
		goto ThreadEnd;
	}

	// 设置视频输出
	if (obj->Info.bHasVideo)
	{
		// 启动视频解码线程
		video.obj = obj;
		video.fPlayVideo = 1;
		video.hMutex = kMutexCreate();
		video.audio_frame_time = 0;
		video.video_frame_time = 0;
		kdebug(mod_audio, PRINT_INFO, "create video thread\n");
		KernelThread(PRIO_USER+2, AkThreadVideoPlay, (DWORD)&video, 0x8000);
	}
	else
	{
		video.obj = obj;
		video.hMutex = NULL;
		video.fPlayVideo = 0;
		video.audio_frame_time = 0;
		video.video_frame_time = 0;
	}
		
	// 发送WM_MEDIA_OPEN消息
	if(task->hWnd)
	{
		msg.hWnd = task->hWnd;
		msg.Data.v = 0;
		msg.hWndSrc = 0;
		msg.MsgId = WM_MEDIA_OPEN;
		msg.Param = 0;
#if defined(STC_EXP)
		sGuiMsgPutMsg(&msg);
#else
		GuiMsgPutMsg(&msg);
#endif
	}

	// 循环解压并播放
	fade = 0;
	duration = 0;
	fPlayMedia = 1;
	obj->RecDuration = 0;

	if( task->ExterdType )
		obj->JzAvDecoder->fMplayerInit = 0;
	init_end_time = TimerCount();
	kdebug(mod_audio, PRINT_INFO, "mplayer start play time = %d\n",init_end_time);
	kdebug(mod_audio, PRINT_INFO, "playe init time = %dms\n",(init_end_time-init_start_time)*10);
	kdebug(mod_audio,PRINT_INFO,"obj->StartTime = %d, Obj->EndTime = %d\n",obj->StartTime,obj->EndTime);
	while(!obj->bClose && !obj->bTerminate)
	{
		// 解码音频数据
		if(video.hMutex)
			kMutexWait(video.hMutex);

		//得到PCM数据
		player_status = MediaLibGetStatus(obj);
		wlen = MediaLibGetPcm(obj,(BYTE*)pcm,block_num);
//		kprintf("wlen = %d\n",wlen);
		if((wlen<0) || (player_status==MEDIALIB_END) || (player_status==MEDIALIB_ERR))
		{
			DacWriteEnd(obj->hDac, 0);
			if(MEDIALIB_ERR == player_status)
			{
				kdebug(mod_media, PRINT_ERROR, "media lib error!\n");
				obj->bClose = 2;
			}
			else
				obj->bClose = 1;
			if(video.hMutex)
				kMutexRelease(video.hMutex);
			kdebug(mod_audio, PRINT_INFO, "wlen = %d, player_status = %d\n",wlen,player_status);
			break;
		}

		// 获取当前音频解码时间
		cur_time = MediaLibGetAudioCurTime(obj);
//		kprintf("cur time = %d\n",cur_time);
		if( obj->StartTime )
		{
			MediaLibSeek(obj,obj->StartTime);
			obj->StartTime = 0;
			wlen = 0;
		}

		if( obj->EndTime && cur_time >= obj->EndTime )
		{	//定时播放，如果播放时间到，则直接播放结束
			DacWriteEnd(obj->hDac, 0);
			obj->bClose = 1;
			if(video.hMutex)
				kMutexRelease(video.hMutex);
			kprintf("play seek end: cur time = %d, end time = %d\n",cur_time,obj->EndTime);
			break;
		}

		if(video.hMutex)
			kMutexRelease(video.hMutex);

		//开始播放时，wlen = 0,此时需要VIDEO线程运行，同时给AUDIO解锁
		if( wlen == 0 && obj->Info.bHasVideo )
			sTimerSleep(20, NULL);
		if(GetDacBufCountMs() > 200 && obj->Info.bHasVideo )
			sTimerSleep(40, NULL);

		// 输出音频数据到DA
		pData = pcm;
		obj->RecDuration = cur_time;
 		while(wlen > 0)
		{
			DWORD wsize;
			int ctl_code;

			// 写入PCM数据到DAC设备
			wsize = (wlen > max_wsize) ? max_wsize : wlen;
			if(fade)
			{
				int s;
				short pcms;
				pcms = wsize / sizeof(short);
				for(s=0; s<pcms; s++)
				{
					int tmp;
					tmp = (int)pData[s];
					tmp = (tmp * (pcms - s)) / pcms;
					pData[s] = (short)tmp;
				}
				DacWrite(obj->hDac, pData, wsize);
				DacWriteEnd(obj->hDac, 0);
				break;
			}
			else
			{
				if( DacWrite(obj->hDac, pData, wsize) == 0 )
				{	//出错，返回值为0
					kdebug(mod_audio, PRINT_ERROR, "dac write is 0 \n");
					obj->bClose = 1;
					break;
				}
			}

			// 提高当前时间精度
			if( wsize == max_wsize )
				obj->RecDuration += max_time;
			else
				obj->RecDuration += (wsize * 1000) / (obj->Info.AudioSamplerate * obj->Info.AudioChannels * sizeof(short));

			pData += wsize / sizeof(short);
			wlen -= wsize;
			
			// 控制动作处理
			if(video.hMutex)
				kMutexWait(video.hMutex);
			ctl_code = JzSrvProcMsg(obj);
			player_status = MediaLibGetStatus(obj);
			if(video.hMutex)
				kMutexRelease(video.hMutex);

 			// 等待退出暂停状态
			while( MEDIALIB_PAUSE == player_status && !obj->bClose && !obj->bTerminate)
			{
				sTimerSleep(100, NULL);

				if(video.hMutex)
					kMutexWait(video.hMutex);
				ctl_code = JzSrvProcMsg(obj);
				player_status = MediaLibGetStatus(obj);
				if(video.hMutex)
					kMutexRelease(video.hMutex);
			}
			
			// 检查是否退出
			if(obj->bTerminate)
			{
// 				if(fade == 0 && wlen == 0)
// 				{
// 					wlen = MediaLibGetPcm(obj,(BYTE*)pcm,block_num);
// 					pData = pcm;
// 					if(wlen < 0)
// 					{
// 						wlen = 0;
// 					}
// 				}
				kdebug(mod_audio, PRINT_INFO, "fade len:%d\n", wlen);
				fade = 1;
			}
 		}
	}

	//如果是强行退出，则直接关闭MOS管，方式喀嚓声
	if( obj->bTerminate && GetDacChannel() <= 1 )
	{
		kdebug(mod_audio, PRINT_INFO, "terminate media, close mute mos\n");
	}

	init_start_time = TimerCount();
	kdebug(mod_audio, PRINT_INFO, "close meidia start : %d\n",init_start_time);

	// 是否用于视频的互斥量
	if(video.hMutex)
	{	//等待视频结束
		while( 1 )
		{
			if( !video.fPlayVideo )
				break;
			sTimerSleep(10, NULL);
		}
		kMutexDestroy(video.hMutex);
	}

	// 关闭DAC设备
	DacClose(obj->hDac);
	MediaLibClose(obj);
	kfree(pcm);

	init_end_time = TimerCount();
	kdebug(mod_audio, PRINT_INFO, "close meidia end : %d - %dms\n",init_end_time, (init_end_time - init_start_time)*10);
ThreadEnd:
	fPlayMedia = 0;
	WinClipMask(NULL);

	//MPLAYER播放器使用,用来设置从MPLAYER读到的PCM数据以及长度
	if( obj->PlayerType )
	{	//外部播放器使用变量
		pMediaPcmData = NULL;
		nMediaPcmLen  = 0;
	}

	// 设置线程结束标志	
	msgid = 0;
	if(obj->bTerminate || (obj->bClose == 2))
	{	
		msgid = WM_MEDIA_TERMINATE;
		msg.MsgId = msgid;
		msg.hWnd = task->hWnd;
	}
	else if(obj->bClose)
	{	
		msgid = WM_MEDIA_CLOSE;
		msg.MsgId = msgid;
		msg.hWnd = task->hWnd;
	}
	else
	{
		// 未启动播放即结束
		msgid = 0;
		*obj->bReady = -1;
	}
	
	// 设置返回消息参数	
	if((obj->bTerminate == 2))
		msg.Data.v = 1;
	else if(obj->bClose == 2)
		msg.Data.v = 2;
	else
		msg.Data.v = 0;
	
	// 删除对象
	if(obj->bTerminate == 1)
		obj_del = 0;
	else
		JzSrvDestroyNotify(task);
	
	// 发送WM_MEDIA_CLOSE消息
	if(msgid)
	{
		msg.hWndSrc = 0;
		msg.Param = 0;
	#if defined(STC_EXP)
		sGuiMsgPutMsg(&msg);
	#else
		GuiMsgPutMsg(&msg);
	#endif
	}
	
	// 增加Terminate标记，方便退出
	if(!obj_del)
		obj->bTerminate++;

	kdebug(mod_audio, PRINT_INFO, "medialib close finish\n");
#ifdef MPLAYER_PRINTF_ENABEL
	syslog_set_global_level(PRINT_NOTICE);
#endif
#if defined(STC_EXP)
	sThreadTerminate(sThreadSelf());
#else
	ThreadTerminate(ThreadSelf());
#endif
}


extern void LcdcStandbyOff(void);
extern void LcdcStandbyOn(void);
////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void JzThreadRecordFile(DWORD p)
{
	PAK_OBJECT obj;
	PAK_VFILE vfile;
	PMEDIA_TASK task;
	MESSAGE msg;
	int recsize;
	int samp_size;
	void *buf_in;
	BYTE *fade_buf;
	int msgid;
	int wsize,samp_time;
	int obj_del = 1;
	int fade_len;
	int fade_record = RECORD_FADE;

	// 获取打开信息
	obj = (PAK_OBJECT)p;
	task = &obj->Task;
	vfile = &obj->File;
	if(!task->AChannels)
		task->AChannels = 1;
	if(!task->ASamplerate)
		task->ASamplerate = 8000;
	
	// 写入头信息
	samp_size = 8192 * 2;
	msgid = 0;
	WaveFileHead.bytes_per_samp = task->AChannels * 2;
	WaveFileHead.bytes_per_sec = WaveFileHead.bytes_per_samp * task->ASamplerate;
	WaveFileHead.num_chans = task->AChannels;
	WaveFileHead.sample_rate = task->ASamplerate;
	if( task->MediaType == AK_REC_TYPE_WAVE)
	{
		if(JzCbFileWrite(&WaveFileHead, sizeof(WAVE_HEADER),1,vfile) != sizeof(WAVE_HEADER))
		{
			*obj->bReady = -1;
			goto ThreadEnd;
		}
	}

	// 打开音频编码库
	buf_in = kmalloc(samp_size);
	if(buf_in == NULL)
	{
		*obj->bReady = -1;
		goto ThreadEnd;
	}

	// 打开ADC设备
	obj->hDac = AdcOpen(task->ASamplerate, task->AChannels);
	if(obj->hDac == NULL)
	{
		kdebug(mod_audio, PRINT_ERROR, "ADC device open failed! CH: %d, %dHz\n", task->AChannels, task->ASamplerate);
		kfree(buf_in);
		*obj->bReady = -1;
		goto ThreadEnd;
	}
	AdcSetVolume(obj->hDac, obj->Task.Volume);
	AdcSetEq(obj->hDac, obj->Task.Eq);

	// 发送WM_MEDIA_OPEN消息
	if(task->hWnd)
	{
		msg.hWnd = task->hWnd;
		msg.Data.v = 0;
		msg.hWndSrc = 0;
		msg.MsgId = WM_MEDIA_OPEN;
		msg.Param = 0;
	#if defined(STC_EXP)
		sGuiMsgPutMsg(&msg);
	#else
		GuiMsgPutMsg(&msg);
	#endif
	}
	
	// 等待条件，写入数据
	*obj->bReady = 1;
	recsize = 0;
	fRecodeWav = 1;
	while(!obj->bClose && !obj->bTerminate)
	{
		int i;
		
		// 从DA读取PCM数据
		for(i=0; i<8; i++)
		{
			JzSrvProcMsg(obj);
			wsize = AdcRead(obj->hDac, (short*)((DWORD)buf_in+(i*samp_size)/8), samp_size/8);
			samp_time = (samp_size * 1000 / 8) / (task->ASamplerate * task->AChannels * sizeof(short)) ;

			if( fade_record )
			{	//fade时，不进行时间计算，递减fade时间
				if( fade_record > samp_time )
					fade_record -= samp_time;
				else
					fade_record = 0;
			}
			else	//fade结束，可以进行时间计算
				obj->RecDuration += samp_time;
		}

		if( !fade_record )
			recsize += samp_size;
		
		// 检查录音时间是否结束
		if(obj->Task.Duration && (obj->RecDuration >= obj->Task.Duration))
		{
			recsize -= samp_size;
			obj->bClose = 1;
			break;
		}

		// 直接把PCM数据写入文件
		if( !fade_record )
		{
			wsize = JzCbFileWrite(buf_in, samp_size,1,vfile);
			if(wsize != samp_size)
			{
				if(wsize > 0)
					recsize -= samp_size - wsize;
				else
					recsize -= samp_size;
				obj->bClose = 2;
				break;
			}
		}

		// 处理控制消息
		JzSrvProcMsg(obj);
		while(obj->bRecPause && !obj->bClose && !obj->bTerminate)
		{
			JzSrvProcMsg(obj);
			sTimerSleep(100, NULL);
		}
	}

	//fade时，不保存文件
	if( !fade_record )
	{
		fade_len = (task->ASamplerate * task->AChannels * sizeof(short) * RECODE_SKIP_LEN) / 1000;
		if( task->MediaType == AK_REC_TYPE_WAVE)
		{
			// 录音结束，写入头/尾信息
			JzCbFileSeek(vfile, 0, SEEK_SET);
			WaveFileHead.file_size = recsize + sizeof(WaveFileHead) - 8;
			WaveFileHead.data_length = recsize;
			JzCbFileWrite(&WaveFileHead, sizeof(WAVE_HEADER),1,vfile);

			if( vfile->PlayLength > (fade_len + sizeof(WaveFileHead)) && fade_len )
			{
				fade_buf = (BYTE*)kmalloc(fade_len);
				kmemset(fade_buf,0,fade_len);
				JzCbFileSeek(vfile, recsize + sizeof(WaveFileHead) - fade_len , SEEK_SET);
				JzCbFileWrite(fade_buf, fade_len,1,vfile);
				kfree(fade_buf);
				kdebug(mod_audio, PRINT_INFO, "recode fade: skip wav len = %d\n",fade_len);
			}
		}
		else
		{
			if( vfile->PlayLength > fade_len && fade_len )
			{
				fade_buf = (BYTE*)kmalloc(fade_len);
				kmemset(fade_buf,0,fade_len);
				JzCbFileSeek(vfile, recsize - fade_len, SEEK_SET);
				JzCbFileWrite(fade_buf, fade_len,1,vfile);
				kfree(fade_buf);
				kdebug(mod_audio, PRINT_INFO, "recode fade: skip pcm len = %d\n",fade_len);
			}
		}

	}
	JzCbFileWriteFlush(vfile);
	fRecodeWav = 0;
	
	// 线程结束, 关闭文件和设备
	AdcClose(obj->hDac);

	// 设置线程结束标志	
	if(obj->bTerminate || (obj->bClose == 2))
	{	
		msgid = WM_MEDIA_TERMINATE;
		msg.MsgId = msgid;
		msg.hWnd = task->hWnd;
	}
	else if(obj->bClose)
	{	
		msgid = WM_MEDIA_CLOSE;
		msg.MsgId = msgid;
		msg.hWnd = task->hWnd;
	}
	else
	{
		// 非正常结束
		msgid = 0;
		*obj->bReady = -1;
	}
	
	// 设置返回消息参数	
	if(obj->bTerminate == 2)
		msg.Data.v = 1;
	else if(obj->bClose == 2)
		msg.Data.v = 2;
	else
		msg.Data.v = 0;
	
	// 删除对象
	if(obj->bTerminate == 1)
		obj_del = 0;
	else
		JzSrvDestroyNotify(task);
	
	// 发送WM_MEDIA_CLOSE消息
	if(msgid)
	{
		msg.hWndSrc = 0;
		msg.Param = 0;
	#if defined(STC_EXP)
		sGuiMsgPutMsg(&msg);
	#else
		GuiMsgPutMsg(&msg);
	#endif
	}

	if(buf_in)
		kfree(buf_in);
ThreadEnd:	
	// 设置线程结束标志	
	if(!obj_del)
		obj->bTerminate++;

#if defined(STC_EXP)
	sThreadTerminate(sThreadSelf());
#else
	ThreadTerminate(ThreadSelf());
#endif
}



////////////////////////////////////////////////////
// 功能: 启动播放线程
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int JzThreadOpen(PAK_OBJECT obj)
{
	int thread_ready;
	DWORD stack_size;
	
	// 初始化变量
	thread_ready = 0;
	obj->bTerminate = 0;
	obj->bClose = 0;
	stack_size  = 0x8000;
	if( obj->Task.ExterdType )
		stack_size *= 2;
	obj->bReady = &thread_ready;
	
	// 启动线程
	if(obj->Task.Record)
	{
		if(!obj->bGetInfo)
			KernelThread(PRIO_USER-10, JzThreadRecordFile, (DWORD)obj, stack_size);
	}
	else
	{
		if(obj->bGetInfo)
			KernelThread(PRIO_USER-10, JzThreadGetInfo, (DWORD)obj, stack_size);
		else
			KernelThread(PRIO_USER-10, JzThreadPlayFile, (DWORD)obj, stack_size);
	}
	
	// 等待线程处理
	while(!thread_ready)
	{
#if defined(STC_EXP)
		sTimerSleep(100, NULL);
#else
		TimerSleep(100, NULL);
#endif		
	}
	if(thread_ready < 0)
		return -1;
	return 0;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void JzThreadClose(PAK_OBJECT obj, int code)
{
	// 等待线程退出
	if(!code)
		code = 1;
	obj->bTerminate = code;

// 	if( GetDacChannel() == 1 )
// 	{
// 		kprintf("JzThreadClose close\n");
// 		SetMoseCe(0);				//关闭声音，防止出现BOBO音
// 	}
	while(obj->bTerminate == code)
	{
		// 等待
#if defined(STC_EXP)
		sTimerSleep(100, NULL);
#else
		TimerSleep(100, NULL);
#endif		
	}
}

