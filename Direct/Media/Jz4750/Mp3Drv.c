//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//    	          mp3�ڲ������������ӿ�
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
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
// ����: MP3����ص�����
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: �õ�midi����
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int OpenMp3Drv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	//mp3������
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int GetMp3Info(PAK_OBJECT obj, PAK_VFILE vfile)
{
	MP3_INFO  Info;

	JzCbFileSeek(vfile,0,SEEK_SET);
	if( GetMp3PlayTime(&Info,Mp3CallBackProc,(DWORD)vfile,30,vfile->PlayLength) )	//GetMp3DecodeTime(obj->hMedia,&Info) ==����ȷ��MP3����ʱ�䣬�����ٶ�̫����1��Ҫ2~3��
	{																				//GetMp3PlayTime(obj,10) ==������MP3����ʱ�䣬�����CBR��ʽ��׼ȷ��VBR��׼ȷ
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
// ����: �õ�MP3����
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: �ر�mp3����
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int CloseMp3Drv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	Mp3DecodeDestroy(obj->hMedia);
	return 1;
}

////////////////////////////////////////////////////
// ����: seek mp3
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int Mp3Seek(PAK_OBJECT obj, int ms)
{
	kdebug(mod_audio, PRINT_INFO, "Mp3Seek: ms = %d\n",ms);
	obj->InnerPlayer.cur_time = Mp3DecodeSeek(obj->hMedia,ms);
	obj->InnerPlayer.mp3_pcm_len = 0;
	return 1;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
INNER_PLAYER_DRV Mp3DrvList = {
	OpenMp3Drv,
	CloseMp3Drv,
	GetMp3Info,
	Mp3Seek,
	GetMp3PcmData
};
