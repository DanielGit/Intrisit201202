//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          声音播放库操作部分函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

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
#include <config.h>

#if defined(CONFIG_MAKE_BIOS) && defined(CONFIG_MPLAYER_ENABLE)
#undef CONFIG_MPLAYER_ENABLE
#endif 

//设置最大的视频播放尺寸
#if defined(CONFIG_MCU_JZ4755)
#define WMV_DEFINITION_MOVIE_W		640			//WMV1，WMV2的限制尺寸
#define WMV_DEFINITION_MOVIE_H		480
#define FLV_DEFINITION_MOVIE_W		720			//FLV的限制尺寸
#define FLV_DEFINITION_MOVIE_H		576
#define DV_DEFINITION_MOVIE_W		640			//DV的限制尺寸
#define DV_DEFINITION_MOVIE_H		480
#define H263_DEFINITION_MOVIE_W		800			//h263的限制尺寸
#define H263_DEFINITION_MOVIE_H		480
#define H264_DEFINITION_MOVIE_W		800			//h264的限制尺寸
#define H264_DEFINITION_MOVIE_H		480
#define HEIGHT_DEFINITION_MOVIE_W	800			//高清视频的限制尺寸
#define HEIGHT_DEFINITION_MOVIE_H	480
#define NORMAL_DEFINITION_MOVIE_W	800			//普通视频的限制尺寸
#define NORMAL_DEFINITION_MOVIE_H	480
#elif defined(CONFIG_MCU_JZ4750)
#define WMV_DEFINITION_MOVIE_W		640			//WMV1，WMV2的限制尺寸
#define WMV_DEFINITION_MOVIE_H		480
#define FLV_DEFINITION_MOVIE_W		720			//FLV的限制尺寸
#define FLV_DEFINITION_MOVIE_H		576
#define DV_DEFINITION_MOVIE_W		640			//DV的限制尺寸
#define DV_DEFINITION_MOVIE_H		480
#define H263_DEFINITION_MOVIE_W		800			//h263的限制尺寸
#define H263_DEFINITION_MOVIE_H		480
#define H264_DEFINITION_MOVIE_W		800			//h264的限制尺寸
#define H264_DEFINITION_MOVIE_H		480
#define HEIGHT_DEFINITION_MOVIE_W	800			//高清视频的限制尺寸
#define HEIGHT_DEFINITION_MOVIE_H	480
#define NORMAL_DEFINITION_MOVIE_W	800			//普通视频的限制尺寸
#define NORMAL_DEFINITION_MOVIE_H	480
#else
#define WMV_DEFINITION_MOVIE_W		640			//WMV1，WMV2的限制尺寸
#define WMV_DEFINITION_MOVIE_H		480
#define FLV_DEFINITION_MOVIE_W		640			//FLV的限制尺寸
#define FLV_DEFINITION_MOVIE_H		480
#define DV_DEFINITION_MOVIE_W		640			//DV的限制尺寸
#define DV_DEFINITION_MOVIE_H		480
#define H263_DEFINITION_MOVIE_W		640			//h263的限制尺寸
#define H263_DEFINITION_MOVIE_H		480
#define H264_DEFINITION_MOVIE_W		640			//h264的限制尺寸
#define H264_DEFINITION_MOVIE_H		480
#define HEIGHT_DEFINITION_MOVIE_W	640			//高清视频的限制尺寸
#define HEIGHT_DEFINITION_MOVIE_H	480
#define NORMAL_DEFINITION_MOVIE_W	640			//普通视频的限制尺寸
#define NORMAL_DEFINITION_MOVIE_H	480
#endif


extern BYTE* pMediaPcmData;
extern DWORD nMediaPcmLen;
extern DWORD nMplayerSamplerate;
extern DWORD nMplayerChannels;
extern DWORD nMplayerDmaStart;

extern BYTE _end_mplayer_data;
extern BYTE _start_mplayer_data;
extern BYTE _end_mplayer_sbss;
extern BYTE _start_mplayer_sbss;
extern BYTE _end_mplayer_bss;
extern BYTE _start_mplayer_bss;
extern void FreeMplayerMem();
extern void ClockDelay(DWORD usec);
extern void WinClipMask(RECT *rect);
extern BYTE* LcdcVideoOsdBuf(void);
extern void DacReinit(HANDLE hdac);
extern void DacWaiteDma(HANDLE hdac);
extern void SetMuteMode(char mode);
extern MEDIALIB_TYPE CheckInnerPlayerType(PAK_VFILE vfile);
extern int JzCbCheckFile(PAK_VFILE vfile);
extern DWORD TimerCount(void);

static BYTE *pMplayerDataMalloc,*pMplayerBssMalloc,*pMplayerSbssMalloc;

static PINNER_PLAYER_DRV InnerPlayerDrvList[] = 
{
	&WavDrvList,
	&Mp3DrvList,
#ifdef CONFIG_DECODE_MIDI_ENABLE
	&MidiDrvList,
#else
	NULL,
#endif
	NULL
};

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SaveMplayerVar()
{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	int data_size,bss_size,sbss_size;
	BYTE *pData;
	data_size = &_end_mplayer_data - &_start_mplayer_data;
	bss_size = &_end_mplayer_bss - &_start_mplayer_bss;
	sbss_size = &_end_mplayer_sbss - &_start_mplayer_sbss;

	if( data_size )
	{
		pMplayerDataMalloc = (BYTE*)kmalloc(data_size);
		pData = &_start_mplayer_data;
		kmemcpy(pMplayerDataMalloc,pData,data_size);
	}
	
	if( bss_size )
	{
		pMplayerBssMalloc = (BYTE*)kmalloc(bss_size);
		pData = &_start_mplayer_bss;
		kmemcpy(pMplayerBssMalloc,pData,bss_size);
	}

	if( sbss_size )
	{
		pMplayerSbssMalloc = (BYTE*)kmalloc(sbss_size);
		pData = &_start_mplayer_sbss;
		kmemcpy(pMplayerSbssMalloc,pData,sbss_size);
	}
#endif
}
////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void LoadMplayerVar()
{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	int data_size,bss_size,sbss_size;
	BYTE *pData;
	data_size = &_end_mplayer_data - &_start_mplayer_data;
	bss_size = &_end_mplayer_bss - &_start_mplayer_bss;
	sbss_size = &_end_mplayer_sbss - &_start_mplayer_sbss;

	if( data_size )
	{
		pData = &_start_mplayer_data;
		kmemcpy(pData,pMplayerDataMalloc,data_size);
	}
	
	if( bss_size )
	{
		pData = &_start_mplayer_bss;
		kmemcpy(pData,pMplayerBssMalloc,bss_size);
	}

	if( sbss_size )
	{
		pData = &_start_mplayer_sbss;
		kmemcpy(pData,pMplayerSbssMalloc,sbss_size);
	}
#endif
}

////////////////////////////////////////////////////
// 功能: 延迟N个豪秒
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void mdelay(unsigned int msec)
{
	unsigned int i;

	for (i = 0; i < msec; i++)
		ClockDelay(1000);
}

////////////////////////////////////////////////////
// 功能: 初始化音频，视频库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibInit(PAK_OBJECT obj)
{
	DWORD *pFileData,flag;
	DWORD pFileSize,i;
	PAK_VFILE File;

	//初始化定时结束标记
	obj->EndTime = 0;
	obj->StartTime = 0;

	//为了防止脑残全0视频，音频，加入容错
	File = &obj->File;
	pFileData = (DWORD*)kmalloc(1024 * sizeof( DWORD ));
	pFileSize = JzCbFileRead((BYTE*)pFileData,1,1024 * sizeof(DWORD),File);
	for( i = 0 , flag = 0 ; i < ( pFileSize / sizeof( DWORD ) ) ; i++ )
	{
		if( pFileData[i] )
			flag = 1;
	}

	kfree(pFileData);
	if( !flag )
		return -2;		//文件全0，退出

	//使用内部解码器，不用设置回调函数
	if( !obj->PlayerType )
		return 1;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	LoadMplayerVar();
	if( MediaLibCbInit(obj) < 0 )
		return -1;
	InitMplayerLib();
#endif
	return 1;
}

////////////////////////////////////////////////////
// 功能: 初始化回调函数库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibCbInit(PAK_OBJECT obj)
{
	PMEDIA_TASK Task;

	//使用内部解码器，不用设置回调函数
	if( !obj->PlayerType )
		return 1;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	Task = &obj->Task;
	obj->JzAvDecoder = GetCbDecoder();
	AkCbInit(obj->JzAvDecoder,obj);
	AkCbReinit(obj->JzAvDecoder,obj);
	noah_set_lcd_size(Task->ShowRect.x,Task->ShowRect.y,Task->ShowRect.w,Task->ShowRect.h);
	if( !obj->JzAvDecoder->malloc_buf )
		return -1;
#endif
	return 1;
}

////////////////////////////////////////////////////
// 功能: 打开音频，视频库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibOpen(PAK_OBJECT obj, int info)
{
	int ret;
	MEDIALIB_TYPE player_type;
	PINNER_PLAYER_DRV pDevice;

	if( !obj->PlayerType )
	{	//使用内部解码器

		//初始化内部解码器变量
		obj->InnerPlayer.type         = MEDIALIB_UNKNOW;
		obj->InnerPlayer.status       = MEDIALIB_INIT;
		obj->InnerPlayer.cur_time     = 0;
		obj->InnerPlayer.total_time   = 0;
		obj->InnerPlayer.mp3_pcm_len  = 0;
		obj->InnerPlayer.wav_data_len = 0;
		obj->InnerPlayer.pDevice      = NULL;

		//得到内部播放器的类型
		player_type = CheckInnerPlayerType(&obj->File);
		obj->InnerPlayer.status = player_type;

		//得到内部播放器的驱动
		pDevice = InnerPlayerDrvList[player_type];
		obj->InnerPlayer.pDevice = pDevice;
		if( pDevice == NULL )
		{
			kdebug(mod_media, PRINT_ERROR, "inner player open failed, pDevice is null, player_type = %d\n",player_type);
			obj->InnerPlayer.status = MEDIALIB_UNKNOW;
			return -1;
		}

		//打开内部播放器设备
		if( pDevice->Open(obj,&obj->File) <= 0)
		{
			kdebug(mod_media, PRINT_ERROR, "inner player open failed\n");
			obj->InnerPlayer.status = MEDIALIB_UNKNOW;
			return -1;
		}
		return 1;
	}
	else
	{	//使用外部编码器
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
		ret = OpenMplayerLib(info);
		if( ret < 0 )
			kdebug(mod_media, PRINT_ERROR, "open failed flag = %d\n",ret);
		return ret;
#endif
	}
	return 1;
}

////////////////////////////////////////////////////
// 功能: 开始播放音频，视频
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibPlay(PAK_OBJECT obj)
{
	if( !obj->PlayerType )
	{	//使用外部编码器
		obj->InnerPlayer.status = MEDIALIB_PLAYING;
	}
	else
	{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
		return PLayMplayerLib();
#endif
	}
	return 0;
}

////////////////////////////////////////////////////
// 功能: 关闭音频，视频库
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibClose(PAK_OBJECT obj)
{
	PAK_VFILE vfile;
	PINNER_PLAYER_DRV pDevice;

	if( !obj->PlayerType )
	{
		pDevice = obj->InnerPlayer.pDevice;
		if( pDevice )
			pDevice->Close(obj,&obj->File);
		else
			kdebug(mod_media, PRINT_WARNING, ">>>>>>>> MediaLibClose: device is null\n");
	}
	else
	{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
		MplayerCloseLib();

		//释放mplayer使用的内存
		FreeMplayerMem();

		//释放插件文件
		vfile = (PAK_VFILE)obj->JzAvDecoder->plugstream;
		kfclose(vfile->File);
		kfree(vfile);

		kfree(obj->JzAvDecoder);
		return 1;
#endif
	}
	return 1;
}

////////////////////////////////////////////////////
// 功能: 得到当前播放文件的信息
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibGetInfo(PAK_OBJECT obj)
{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	int flag;
#endif
	PAK_VFILE vfile;
	PINNER_PLAYER_DRV pDevice;

	vfile = &obj->File;
	if( !obj->PlayerType )
	{	//内部播放器
		pDevice = obj->InnerPlayer.pDevice;
		if( pDevice )
		{
			if( pDevice->Info(obj,vfile) == 0 )
			{
				obj->Info.MediaType = MEDIALIB_UNKNOW;
				kdebug(mod_media, PRINT_ERROR, ">>>>>>>> MediaLibGetInfo: info is error\n");
				return -1;
			}
			kdebug(mod_media, PRINT_INFO, "TotalTime = %d\n",obj->Info.TotalTime);
			kdebug(mod_media, PRINT_INFO, "AudioBitrate = %d\n",obj->Info.AudioBitrate);
			kdebug(mod_media, PRINT_INFO, "AudioSamplerate = %d\n",obj->Info.AudioSamplerate);
			obj->InnerPlayer.type = obj->InnerPlayer.status;
			return 1;
		}
		else
		{
			kdebug(mod_media, PRINT_ERROR, ">>>>>>>> MediaLibGetInfo: device is null\n");
			return -1;
		}
	}
	else
	{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
		MplayerGetInfo();
		kdebug(mod_media, PRINT_INFO, "========================== media information ==========================\n");
		kmemcpy(&obj->Info,&obj->JzAvDecoder->MediaInfo,sizeof(MEDIA_INFO));
		kdebug(mod_media, PRINT_INFO, "obj = %x\n",obj);
		kdebug(mod_media, PRINT_INFO, "bHasAudio = %d\n",obj->Info.bHasAudio);
		kdebug(mod_media, PRINT_INFO, "bHasVideo = %d\n",obj->Info.bHasVideo);
		kdebug(mod_media, PRINT_INFO, "bAllowSeek = %d\n",obj->Info.bAllowSeek);
		kdebug(mod_media, PRINT_INFO, "TotalTime = %d\n",obj->Info.TotalTime);
		kdebug(mod_media, PRINT_INFO, "AudioBitrate = %d\n",obj->Info.AudioBitrate);
		kdebug(mod_media, PRINT_INFO, "AudioSamplerate = %d\n",obj->Info.AudioSamplerate);
		kdebug(mod_media, PRINT_INFO, "AudioChannels = %d\n",obj->Info.AudioChannels);
		kdebug(mod_media, PRINT_INFO, "VideoWidth = %d\n",obj->Info.VideoWidth);
		kdebug(mod_media, PRINT_INFO, "VideoHeight = %d\n",obj->Info.VideoHeight);
		kdebug(mod_media, PRINT_INFO, "VideoFps = %d\n",obj->Info.VideoFps);
		kdebug(mod_media, PRINT_INFO, "VideoBitrate = %d\n",obj->Info.VideoBitrate);
		kdebug(mod_media, PRINT_INFO, "AudioCodec = %s\n",obj->JzAvDecoder->AudioCodec);
		kdebug(mod_media, PRINT_INFO, "VideoCodec = %s\n",obj->JzAvDecoder->VideoCodec);
		kdebug(mod_media, PRINT_INFO, "=======================================================================\n");

		//初始化mplayer变量
		nMplayerSamplerate = obj->Info.AudioSamplerate;
		nMplayerChannels   = obj->Info.AudioChannels;
		nMplayerDmaStart   = 0;

		flag = 0;
		if( obj->Info.bHasVideo )
		{	//存在视频，判断视频播放尺寸是否大于1080x720，如果大于，则不进行播放，防止内存不足，导致各种情况的死机
			if( kstrcmp(obj->JzAvDecoder->VideoCodec, "ffwmv1") == 0 || kstrcmp(obj->JzAvDecoder->VideoCodec, "ffwmv2") == 0)
			{	//wmv1,wmv2软解，wmv3硬解
				if( obj->Info.VideoHeight > WMV_DEFINITION_MOVIE_H || obj->Info.VideoWidth > WMV_DEFINITION_MOVIE_W )
					flag = 1;
			}
			else if( kstrcmp(obj->JzAvDecoder->VideoCodec,"xvid") == 0 || kstrcmp(obj->JzAvDecoder->VideoCodec,"realvid") == 0 || kstrcmp(obj->JzAvDecoder->VideoCodec,"h264") == 0 )
			{
				if( obj->Info.VideoHeight > HEIGHT_DEFINITION_MOVIE_H || obj->Info.VideoWidth > HEIGHT_DEFINITION_MOVIE_W )
					flag = 1;
			}
			else if( kstrcmp(obj->JzAvDecoder->VideoCodec,"ffh264") == 0  )
			{
				if( obj->Info.VideoHeight > H264_DEFINITION_MOVIE_H || obj->Info.VideoWidth > H264_DEFINITION_MOVIE_W )
					flag = 1;
			}
			else if( kstrcmp(obj->JzAvDecoder->VideoCodec,"ffflv") == 0  )
			{
				if( obj->Info.VideoHeight > FLV_DEFINITION_MOVIE_H || obj->Info.VideoWidth > FLV_DEFINITION_MOVIE_W )
					flag = 1;
			}
			else if( kstrcmp(obj->JzAvDecoder->VideoCodec,"dv") == 0  )
			{
				if( obj->Info.VideoHeight > DV_DEFINITION_MOVIE_H || obj->Info.VideoWidth > DV_DEFINITION_MOVIE_W )
					flag = 1;
			}
			else if( kstrcmp(obj->JzAvDecoder->VideoCodec,"ffh263") == 0  )
			{
				if( obj->Info.VideoHeight > H263_DEFINITION_MOVIE_H || obj->Info.VideoWidth > H263_DEFINITION_MOVIE_W )
					flag = 1;
			}
			else
			{
				if( obj->Info.VideoHeight > NORMAL_DEFINITION_MOVIE_H || obj->Info.VideoWidth > NORMAL_DEFINITION_MOVIE_W )
					flag = 1;
			}
		}
		if( flag )
		{
			kdebug(mod_audio, PRINT_ERROR,  "VideoCodec = %s\n",obj->JzAvDecoder->VideoCodec);
			kdebug(mod_audio, PRINT_ERROR,  "video widht = %d, height = %d\n",obj->Info.VideoWidth,obj->Info.VideoHeight);
			kdebug(mod_audio, PRINT_ERROR,  "vido is to large can't play\n");
			return -1;
		}
		SetMplayerAudioSpace(obj);
		obj->Info.MediaType = 1;
#endif
	}
	return 1;
}

////////////////////////////////////////////////////
// 功能: 得到当前播放状态
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
MEDIALIB_STATUS MediaLibGetStatus(PAK_OBJECT obj)
{
	if( !obj->PlayerType )
		return obj->InnerPlayer.status;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	return GetMplayerStatus();
#endif
	return 0;
}

////////////////////////////////////////////////////
// 功能: 暂停播放 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibPause(PAK_OBJECT obj)
{
	if( !obj->PlayerType )
	{
		obj->InnerPlayer.status = MEDIALIB_PAUSE;
		DacSetStatus(obj->hDac,1);
	}
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	else
	{
		obj->JzAvDecoder->fIpuEnable = 1;
		DacSetStatus(obj->hDac,1);
		WinClipMask(&obj->PauseRect);
		SetMplayerPause();
	}
#endif
	mdelay(100);
	return 1;
}

////////////////////////////////////////////////////
// 功能: 继续播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibResume(PAK_OBJECT obj)
{
	mdelay(100);
	if( !obj->PlayerType )
	{
		obj->InnerPlayer.status = MEDIALIB_PLAYING;
		DacSetStatus(obj->hDac,0);
	}
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	else
	{
		SetMplayerResume();
		WinClipMask(&obj->Task.ShowRect);
		DacSetStatus(obj->hDac,0);
		obj->JzAvDecoder->fIpuEnable = 0;
	}
#endif
	return 1;
}

////////////////////////////////////////////////////
// 功能: 停止播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibStop(PAK_OBJECT obj)
{
	if( !obj->PlayerType )
		obj->InnerPlayer.status = MEDIALIB_STOP;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	else
		SetMplayerStop();
#endif
	return 1;
}

////////////////////////////////////////////////////
// 功能: SEEK到指定时间播放
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MediaLibSeek(PAK_OBJECT obj, unsigned long ms)
{
	PINNER_PLAYER_DRV pDevice;
	DWORD start;

	start = TimerCount();
	if( !obj->PlayerType )
	{
		pDevice = obj->InnerPlayer.pDevice;
		obj->InnerPlayer.status = MEDIALIB_SEEK;
		mdelay(10);
		pDevice->Seek(obj,ms);
		mdelay(10);
		obj->InnerPlayer.status = MEDIALIB_PLAYING;
	}
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	else
	{
		kdebug(mod_media, PRINT_INFO, ">>>>>>>> seek to %dms <<<<<<<<\n",ms);
		SetMplayerPause();
		mdelay(10);
//		DacWriteEnd(obj->hDac,0);
		SetMplayerSeek(ms);
//		DacWaiteDma(obj->hDac);
		SetMplayerResume();
		mdelay(100);
	}
#endif
	kprintf("seek used time : %d\n",TimerCount() - start);
	return 1;
}

////////////////////////////////////////////////////
// 功能: 得到音频数据的PCM数据
// 输入: 
// 输出:
// 返回: PCM数据长度
// 说明: 
////////////////////////////////////////////////////
int MediaLibGetPcm(PAK_OBJECT obj,BYTE* pcm,int size)
{
	PAK_VFILE vfile;
	PINNER_PLAYER_DRV pDevice;

	if( !obj->PlayerType )
	{
		pDevice = obj->InnerPlayer.pDevice;
		if( pDevice )
			return pDevice->GetPcmData(obj,pcm,size);
		else
		{
			kdebug(mod_media, PRINT_ERROR, ">>>>>>>> MediaLibGetPcm: device is null\n");
			return -1;
		}
	}
	else
	{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
		int ret,res;
		vfile = (PAK_VFILE)obj->JzAvDecoder->stream;
		res = GetMplayerDataLib();
		if( JzCbCheckFile(vfile) < 0 )
		{
			kdebug(mod_media, PRINT_ERROR, "audio file error\n");
			SetMplayerError();
			return -1;
		}
		ret = nMediaPcmLen & (~0x80000000);
		if(nMediaPcmLen == 0x80000000)
			return -1;
		if(nMediaPcmLen & 0x80000000)
			nMediaPcmLen = 0x80000000;
		return ret;
#endif
	}
	return 0;
}

////////////////////////////////////////////////////
// 功能: 得到音频数据的PCM数据
// 输入: 
// 输出:
// 返回: PCM数据长度
// 说明: 
////////////////////////////////////////////////////
int MediaLibGetYuv(PAK_OBJECT obj)
{
	int ret;
	PAK_VFILE vfile;

	if( !obj->PlayerType )
		return 0;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	vfile = (PAK_VFILE)obj->JzAvDecoder->stream;
	if( JzCbCheckFile(vfile) < 0 )
	{
		kdebug(mod_media, PRINT_ERROR, "video file error\n");
		SetMplayerError();
		return -1;
	}
	ret = GetMplayerVideoDataLib();
	return ret;
#else
	return 0;
#endif
}

////////////////////////////////////////////////////
// 功能: 得到当前音频播放的时间，用来与VIDEO同步
// 输入: 
// 输出:
// 返回: 当前音频播放的时间
// 说明: 
////////////////////////////////////////////////////
int MediaLibGetAudioCurTime(PAK_OBJECT obj)
{
	if( !obj->PlayerType )
		return obj->InnerPlayer.cur_time;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	return GetMplayerCurTime();
#endif
	return 0;
}

////////////////////////////////////////////////////
// 功能: 设置视频的显示位置以及大小
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetVideoPosAndSize(PAK_OBJECT obj,PMEDIA_TASK task)
{
	int video_x,video_y,video_width,video_height;
	int win_width,win_height,x,y;

	//video原始尺寸
	video_width  = obj->Info.VideoWidth;
	video_height = obj->Info.VideoHeight;

	//得到窗口的尺寸
	x = video_x = task->ShowRect.x;
	y = video_y = task->ShowRect.y;
	win_width   = task->ShowRect.w;
	win_height  = task->ShowRect.h;
	kdebug(mod_media, PRINT_INFO, "Usr Set: x = %d, y = %d, w = %d, h = %d\n",x,y,win_width,win_height);
	kdebug(mod_media, PRINT_INFO, "Video info: w = %d, h = %d\n",video_width,video_height);

	//宽度，高度为0，则自动默认为屏幕宽度，高度
	win_width  = ( win_width == 0 ) ? CONFIG_MAINLCD_XSIZE : win_width;
	win_height = ( win_height == 0 ) ? CONFIG_MAINLCD_YSIZE : win_height;

	//宽度，高度 为-1，表示自动设置成原始尺寸
	win_width  = ( win_width == -1 ) ? video_width : win_width;
	win_height = ( win_height == -1 ) ? video_height : win_height;

	//视频的放大倍数不能超过2倍
	win_width  = ((video_width * 2) < win_width) ? video_width * 2 : win_width;
	win_height = ((video_height * 2) < win_height) ? video_height * 2 : win_height;

	//视频的放大倍数不能小于2倍
	win_width  = ((video_width / 32) > win_width) ? video_width / 32 : win_width;
	win_height = ((video_height / 32) > win_height) ? video_height / 32 : win_height;

	//x,y = -1，屏幕居中显示
	task->ShowRect.x = (x == -1) ? (CONFIG_MAINLCD_XSIZE - win_width) / 2 : x;
	task->ShowRect.y = (y == -1) ? (CONFIG_MAINLCD_YSIZE - win_height) / 2 : y;

	//如果设置区域大于显示区域，则居中显示
	task->ShowRect.x = (task->ShowRect.w > win_width) ? (task->ShowRect.x + (task->ShowRect.w - win_width) / 2) : task->ShowRect.x;
	task->ShowRect.y = (task->ShowRect.h > win_height) ? (task->ShowRect.y + (task->ShowRect.h - win_height) / 2) : task->ShowRect.y;
	
	task->ShowRect.w      = win_width;
	task->ShowRect.h      = win_height;

	kdebug(mod_media, PRINT_INFO, "media info: x = %d, y = %d, width = %d, height = %d\n",
		task->ShowRect.x,task->ShowRect.y,task->ShowRect.w,task->ShowRect.h);

}
////////////////////////////////////////////////////
// 功能: 清空用户设置的区域
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void MediaClearUsrScreen( PMEDIA_TASK task,BYTE format )
{
	int   w,h;
	int   bpp,i;
	RECT  cr;
	BYTE *pScreen,*pos;

#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	pScreen = LcdcVideoOsdBuf();
	cr.x = task->ShowRect.x;
	cr.y = task->ShowRect.y;
	cr.w = task->ShowRect.w;
	cr.h = task->ShowRect.h;
	kdebug(mod_media, PRINT_INFO, "clean rect = (%d,%d,%d,%d)\n",cr.x,cr.y,cr.w,cr.h);

	if( task->ShowRect.w == 0 || task->ShowRect.h == 0 )
		return;

	if( format == CONFIG_MPLAYER_OUT_FORMAT32 )
		bpp = 4;
	else if( format == CONFIG_MPLAYER_OUT_FORMAT16 )
		bpp = 2;
	else
		return;

	kdebug(mod_media, PRINT_INFO, "bpp = %d\n",bpp);
	if( task->ShowRect.w != CONFIG_MAINLCD_XSIZE )
	{	// 宽度不是屏幕宽度
		for( h = task->ShowRect.y ; h < (task->ShowRect.y + task->ShowRect.h) ; h++ )
		{
			if( h >= CONFIG_MAINLCD_YSIZE)
				break;

			for( w = task->ShowRect.x ; w < (task->ShowRect.x + task->ShowRect.w) ; w++)
			{
				if( w >= CONFIG_MAINLCD_XSIZE )
					break;

				pos = pScreen + (h * CONFIG_MAINLCD_XSIZE + w) * bpp;
				for( i = 0 ; i < bpp ; i++ )
					*pos++ = 0;
			}
		}
	}
	else
	{	// 宽度为屏幕宽度
		if( task->ShowRect.h == CONFIG_MAINLCD_YSIZE)
			kmemset(pScreen,0,CONFIG_MAINLCD_XSIZE * CONFIG_MAINLCD_YSIZE * bpp );
		else
		{
			for( h = task->ShowRect.y ; h < (task->ShowRect.y + task->ShowRect.h) ; h++ )
			{
				if( h >= CONFIG_MAINLCD_YSIZE)
					break;

				pos = pScreen + (h * CONFIG_MAINLCD_XSIZE) * bpp;
				kmemset(pos,0,CONFIG_MAINLCD_XSIZE * bpp);
			}
		}
	}
#endif
}

////////////////////////////////////////////////////
// 功能: 设置视频的显示位置以及大小
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int MplayerResizeVideo(PAK_OBJECT obj,PMEDIA_TASK task)
{
	RECT cr;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	if( obj->Info.bHasVideo )
	{	//视频文件，需要计算视频的显示尺寸
		MediaLibPause(obj);

		//保存用户定义显示区域
		cr.x = task->ShowRect.x;
		cr.y = task->ShowRect.y;
		cr.w = task->ShowRect.w;
		cr.h = task->ShowRect.h;

		//根据用户显示区域，重新计算VIDEO实际显示区域
		SetVideoPosAndSize(obj,task);

		//设置VIDEO实际区域大小
		AkCbReinit(obj->JzAvDecoder,obj);

		//修改IPU显示区域
		noah_set_lcd_size(task->ShowRect.x,task->ShowRect.y,task->ShowRect.w,task->ShowRect.h);

		//读出用户设置区域
		task->ShowRect.x = cr.x;
		task->ShowRect.y = cr.y;
		task->ShowRect.w = cr.w;
		task->ShowRect.h = cr.h;

		//开始重新播放，并且按照用户定义区域设置剪切区域
		MediaLibResume(obj);

		//清除用户定义区域
		MediaClearUsrScreen(task,obj->JzAvDecoder->OutFormat);
		return 1;
	}
	else
	{
		//清除用户定义区域
		WinClipMask(&obj->Task.ShowRect);
		MediaClearUsrScreen(task,obj->JzAvDecoder->OutFormat);
	}
#endif
	return 0;
}

