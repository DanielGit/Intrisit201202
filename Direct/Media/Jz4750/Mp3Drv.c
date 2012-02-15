//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          mp3内部播放器函数接口
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2009-5      Init.             taoqh
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

////////////////////////////////////////////////////
// 功能: MP3解码回调函数
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int Mp3CallBackProc(DWORD id,BYTE* buf, int size)
{
	PAK_VFILE vfile;
	DWORD offset;
	vfile = (PAK_VFILE)id;
	if( buf )
		return JzCbFileRead(buf,size,1,vfile);
	else
	{
		offset = kftell(vfile->File);
		if( size )
			JzCbFileSeek(vfile,size,SEEK_CUR);
		else
			JzCbFileSeek(vfile,0,SEEK_SET);
		return offset;
	}
}

////////////////////////////////////////////////////
// 功能: 得到midi数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int OpenMp3Drv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	//mp3解码器
	obj->hMedia = (HANDLE)Mp3DecodeCreate(&obj->File);
	if( obj->hMedia == NULL )
	{
		kdebug(mod_audio, PRINT_ERROR, "media lib mp3 decode create failed\n");
		return 0;
	}
	Mp3DecodeSetCallback((HANDLE)obj->hMedia,(void*)Mp3CallBackProc,(DWORD)&obj->File);
	obj->InnerPlayer.mp3_pcm_len = 0;
	obj->InnerPlayer.cur_time = 0;
	return 1;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int GetMp3Info(PAK_OBJECT obj, PAK_VFILE vfile)
{
	MP3_INFO  Info;

	JzCbFileSeek(vfile,0,SEEK_SET);
	if( GetMp3PlayTime(&Info,Mp3CallBackProc,(DWORD)vfile,30,vfile->PlayLength) )	//GetMp3DecodeTime(obj->hMedia,&Info) ==》精确算MP3播放时间，但是速度太慢，1次要2~3秒
	{																				//GetMp3PlayTime(obj,10) ==》估计MP3播放时间，如果是CBR格式，准确，VBR不准确
		obj->Info.AudioCodec      = 2;
		obj->Info.VideoCodec      = 0;
		obj->Info.AudioBitrate    = Info.AudioBitrate;
		obj->Info.AudioSamplerate = Info.AudioSamplerate;
		obj->Info.AudioChannels   = Info.AudioChannels;
		obj->Info.TotalTime       = Info.TotalTime;
		obj->Info.bHasAudio       = 1;
		obj->Info.bAllowSeek      = 1;
		obj->Info.bHasVideo       = 0;
		if( obj->Info.TotalTime )
			obj->Info.MediaType       = MEDIALIB_MP3;
		JzCbFileSeek(vfile,0,SEEK_SET);
		return 1;
	}
	JzCbFileSeek(vfile,0,SEEK_SET);
	return 0;
}


////////////////////////////////////////////////////
// 功能: 得到MP3数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int GetMp3PcmData(PAK_OBJECT obj, BYTE* pcm, int size )
{
	int len;
	int ret;
	int tm;

	if( (ret = Mp3DecodeFrame(obj->hMedia) ) < 0 )
	{
		obj->InnerPlayer.status = MEDIALIB_END;
		return -1;
	}

	len = size / sizeof(short);
	Mp3DecodeGetPcm(obj->hMedia,(short*)pcm,&len);

	tm = ((len+obj->InnerPlayer.mp3_pcm_len) * 1000) / (obj->Info.AudioSamplerate * obj->Info.AudioChannels * sizeof(short));
	obj->InnerPlayer.cur_time += tm;
	obj->InnerPlayer.mp3_pcm_len = len+obj->InnerPlayer.mp3_pcm_len - (tm * obj->Info.AudioSamplerate * obj->Info.AudioChannels * sizeof(short)) / 1000;
	return len;
}

////////////////////////////////////////////////////
// 功能: 关闭mp3数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int CloseMp3Drv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	Mp3DecodeDestroy(obj->hMedia);
	return 1;
}

////////////////////////////////////////////////////
// 功能: seek mp3
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int Mp3Seek(PAK_OBJECT obj, int ms)
{
	kdebug(mod_audio, PRINT_INFO, "Mp3Seek: ms = %d\n",ms);
	obj->InnerPlayer.cur_time = Mp3DecodeSeek(obj->hMedia,ms);
	obj->InnerPlayer.mp3_pcm_len = 0;
	return 1;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
INNER_PLAYER_DRV Mp3DrvList = {
	OpenMp3Drv,
	CloseMp3Drv,
	GetMp3Info,
	Mp3Seek,
	GetMp3PcmData
};
