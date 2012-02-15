//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          midi内部播放器函数接口
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
#include <direct/medialib/MidiCfg.h>
#include <direct/medialib/MidiDecodec.h>

#ifdef CONFIG_DECODE_MIDI_ENABLE

#define MIDI_PCM_BUF_SIZE	4096 * 8

extern void* JzCbMalloc(DWORD size);
extern void* JzCbFree(void* mem);

static BYTE *pMidiWtbDir=NULL;
////////////////////////////////////////////////////
// 功能: 设置音色库路径
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
void SetMidiWtdDirect(BYTE *param, int size)
{
	if(pMidiWtbDir)
		kfree( pMidiWtbDir );

	size++;
	pMidiWtbDir = (BYTE*)kmalloc(size);
	if( !pMidiWtbDir )
		return ;

	kmemset(pMidiWtbDir,0,size);
	kmemcpy(pMidiWtbDir,param,size);
	kdebug(mod_audio, PRINT_INFO, "kernel midi twb = %s\n",pMidiWtbDir);
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
static  int MidiReadCallback(HANDLE file,unsigned char* buf, int rsize)
{
	int ret;
	ret = JzCbFileRead(buf,rsize,1,file);
	if( ret <= 0 )
		ret = 0;
	return ret;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
static void MidiPcmCallback(HANDLE hMedia,void *id, void *buf, int bytes)
{
	PMIDI_DECODEC hMidi;
	int i;
	int tmp;
	short *pMidiData,*pSource;
	hMidi = (PMIDI_DECODEC)hMedia;

	pSource   = (short*)buf;
	pMidiData = (short*)&hMidi->pMidiPcmBuf[hMidi->nMidiPcmSize];
	for( i = 0 ; i < (bytes / sizeof(short)) ; i++ )
	{
		tmp = (*pSource++) << 1;
		if(tmp >= 32767)
			*pMidiData++ = 32767;
		else if(tmp < -32767)
			*pMidiData++ = -32767;
		else
			*pMidiData++ = (short)tmp;
	}
//	kmemcpy(&hMidi->pMidiPcmBuf[hMidi->nMidiPcmSize],(BYTE*)buf,bytes);
	if( (hMidi->nMidiPcmSize + bytes) >= MIDI_PCM_BUF_SIZE )
	{
		kdebug(mod_audio, PRINT_WARNING, "================ ALARM!!!! ================\n");
		kdebug(mod_audio, PRINT_WARNING, "            midi buf is to small\n");
		kdebug(mod_audio, PRINT_WARNING, "            please tell taoqh\n");
	}
	else
		hMidi->nMidiPcmSize += (DWORD)bytes;
}

////////////////////////////////////////////////////
// 功能: 得到midi数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int OpenMidiDrv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	int32 events;
	int32 samples;
	PMIDI_DECODEC hMidi;

	//创建midi
	hMidi = (PMIDI_DECODEC)MidiDrvCrteate();
	obj->hMedia = (HANDLE)hMidi;

	//初始化内存管理函数
	hMidi->MidiFree   = JzCbFree;
	hMidi->MidiMalloc = JzCbMalloc;
	hMidi->midi_pcm_callback = MidiPcmCallback;
	hMidi->midi_callback     = MidiReadCallback;
	hMidi->midi_file         = vfile;
	hMidi->midi_buf_remain   = 0;

	if( init_instruments(hMidi,(char*)pMidiWtbDir) < 0 )
	{
		kdebug(mod_audio, PRINT_ERROR, "init_instruments failed\n");
 		return -1;
	}
 
 	hMidi->event=read_midi_file(hMidi,&events, &samples);
	if(hMidi->event == NULL)
	{
		kdebug(mod_audio, PRINT_ERROR, "hMidi->event failed\n");
		return -1;
	}
 	
 	load_instruments(hMidi,(char*)pMidiWtbDir);
 	
	hMidi->nMidiPcmSize = 0;
	hMidi->pMidiPcmBuf = (BYTE*)kmalloc(MIDI_PCM_BUF_SIZE);

	init_midi_play(hMidi,hMidi->event,events,samples);

	return 1;
}

////////////////////////////////////////////////////
// 功能: 得到midi信息
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int GetMidiInfo(PAK_OBJECT obj, PAK_VFILE vfile)
{
	BYTE head_buf[8];
	PMIDI_DECODEC hMidi;
	hMidi = (PMIDI_DECODEC)obj->hMedia;

	//读取WAV 文件头
	JzCbFileSeek(vfile,0,SEEK_SET);
	if( JzCbFileRead((BYTE*)&head_buf,4,1,vfile ) != 4 )
		return 0;

	//判断头是否正确
	if( !(head_buf[0] == 'M' && head_buf[1] == 'T' && head_buf[2] == 'h' && head_buf[3] == 'd') )
	{
		head_buf[4] = 0;
		kdebug(mod_audio, PRINT_ERROR, "midi error, head = %s\n",head_buf);
		JzCbFileSeek(vfile,0,SEEK_SET);
		return 0;
	}

	JzCbFileSeek(vfile,0,SEEK_SET);
	obj->Info.AudioSamplerate = DEFAULT_RATE;
	obj->Info.AudioChannels   = 1;
	obj->Info.MediaType       = MEDIALIB_MIDI;
	obj->Info.TotalTime       = get_midi_file_time(hMidi,vfile,MidiReadCallback);
	kdebug(mod_audio, PRINT_INFO, "obj->Info.TotalTime = %d\n",obj->Info.TotalTime);
	obj->Info.AudioBitrate    = 0;
	obj->Info.bHasAudio       = 1;
	obj->Info.bAllowSeek      = 1;
	obj->Info.bHasVideo       = 0;
	obj->Info.AudioCodec      = 1;
	obj->Info.VideoCodec      = 0;
	JzCbFileSeek(vfile,0,SEEK_SET);

	return 1;
}

////////////////////////////////////////////////////
// 功能: 得到midi数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int GetMidiPcmData(PAK_OBJECT obj, BYTE* pcm, int size )
{
	DWORD ret;
	PMIDI_DECODEC hMidi;
	hMidi = (PMIDI_DECODEC)obj->hMedia;


	if( get_midi_pcm(hMidi) < 0 )
		obj->InnerPlayer.status = MEDIALIB_END;

	if( hMidi->nMidiPcmSize > size )
	{
		kdebug(mod_audio, PRINT_WARNING, "================ ALARM!!!! ================\n");
		kdebug(mod_audio, PRINT_WARNING, "            pcm buf is to small\n");
		kdebug(mod_audio, PRINT_WARNING, "            please tell taoqh\n");
		hMidi->nMidiPcmSize = size;
	}
	kmemcpy(pcm,hMidi->pMidiPcmBuf,hMidi->nMidiPcmSize);
	ret = hMidi->nMidiPcmSize;
	hMidi->nMidiPcmSize = 0;
	return ret;
}

////////////////////////////////////////////////////
// 功能: 关闭midi数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int CloseMidiDrv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	PMIDI_DECODEC hMidi;
	hMidi = (PMIDI_DECODEC)obj->hMedia;

	if( hMidi->pMidiPcmBuf )
		kfree(hMidi->pMidiPcmBuf);
	hMidi->pMidiPcmBuf  = NULL;
	hMidi->nMidiPcmSize = 0;

	free_instruments(hMidi);
	
	free_midi_file(hMidi,hMidi->event);

	MidiDrvDestory(hMidi);
	return 1;
}

////////////////////////////////////////////////////
// 功能: seek midi
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int MidiSeek(PAK_OBJECT obj, int ms)
{
	return 1;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
INNER_PLAYER_DRV MidiDrvList = {
	OpenMidiDrv,
	CloseMidiDrv,
	GetMidiInfo,
	MidiSeek,
	GetMidiPcmData
};

#endif	//CONFIG_DECODE_MIDI_ENABLE
