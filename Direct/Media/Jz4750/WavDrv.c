//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	          wav内部播放器函数接口
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

#define MP3_OVER_DATA_LEN	4096
////////////////////////////////////////////////////
// 功能:  判断是否是MP3文件
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static BYTE CheckIsMp3File(PAK_VFILE vfile)
{
	int i,flag;
	BYTE *head_buf;
	DWORD len;

	head_buf = (BYTE*)kmalloc( MP3_OVER_DATA_LEN );
	//读取头数据
	JzCbFileSeek(vfile,0,SEEK_SET);
	len = JzCbFileRead(head_buf,MP3_OVER_DATA_LEN,1 ,vfile );

	flag = 0;
	for( i = 0 ; i < len ; i++ )
	{
		if(head_buf[i] == 0xff && (head_buf[i+1] & 0xe0) == 0xe0 )
		{
			flag = 1;
			break;
		}
	}

	kfree(head_buf);

	if( flag )
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
MEDIALIB_TYPE CheckInnerPlayerType(PAK_VFILE vfile)
{
	MEDIALIB_TYPE ret;
	BYTE head_buf[32];

	//读取头数据
	JzCbFileSeek(vfile,0,SEEK_SET);
	if( JzCbFileRead(head_buf,32,1 ,vfile ) != 32 )
		return MEDIALIB_UNKNOW;

	//初始化播放类型
	ret = MEDIALIB_UNKNOW;

	//判断是否是MIDI文件
	if( head_buf[0] == 'M' && head_buf[1] == 'T' && head_buf[2] == 'h' && head_buf[3] == 'd' )
		ret = MEDIALIB_MIDI;

	//判断是否是WAV文件
	else if( (head_buf[0] == 'R' && head_buf[1] == 'I' && head_buf[2] == 'F' && head_buf[3] == 'F')
		&& (head_buf[8] == 'W' && head_buf[9] == 'A' && head_buf[10] == 'V' && head_buf[11] == 'E' && head_buf[12] == 'f' && head_buf[13] == 'm' && head_buf[14] == 't') )
		ret = MEDIALIB_WAV;

	else if( head_buf[0] == 'I' && head_buf[1] == 'D' && head_buf[2] == '3' )
		ret = MEDIALIB_MP3;
	else if( CheckIsMp3File(vfile) )
		ret = MEDIALIB_MP3;

	//重定位
	JzCbFileSeek(vfile,0,SEEK_SET);
	return ret;
}

////////////////////////////////////////////////////
// 功能: 得到midi数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int OpenWavDrv(PAK_OBJECT obj, PAK_VFILE vfile)
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
static int GetWavInfo(PAK_OBJECT obj, PAK_VFILE vfile)
{
	WAVE_HEADER head_buf;

	//读取WAV 文件头
	JzCbFileSeek(vfile,0,SEEK_SET);
	if( JzCbFileRead((BYTE*)&head_buf,sizeof(WAVE_HEADER),1,vfile ) != sizeof(WAVE_HEADER) )
		return 0;

	//判断头是否正确
	if( !(head_buf.riff[0] == 'R' && head_buf.riff[1] == 'I' && head_buf.riff[2] == 'F' && head_buf.riff[3] == 'F') )
	{
		kdebug(mod_audio, PRINT_ERROR, "head riff error\n");
		JzCbFileSeek(vfile,0,SEEK_SET);
		return 0;
	}

	//判断wave fmt
	if( !(head_buf.wavefmt_[0] == 'W' && head_buf.wavefmt_[1] == 'A' && head_buf.wavefmt_[2] == 'V' && head_buf.wavefmt_[3] == 'E' && head_buf.wavefmt_[4] == 'f' && head_buf.wavefmt_[5] == 'm' && head_buf.wavefmt_[6] == 't') )
	{
		kdebug(mod_audio, PRINT_ERROR, "head wav fmt error\n");
		JzCbFileSeek(vfile,0,SEEK_SET);
		return 0;
	}

	obj->Info.AudioSamplerate = head_buf.sample_rate;
	obj->Info.AudioChannels   = head_buf.num_chans;
	obj->Info.MediaType       = MEDIALIB_WAV;
	obj->Info.TotalTime       = (head_buf.data_length / (head_buf.sample_rate * head_buf.num_chans * sizeof(short)) ) * 1000;
	obj->Info.AudioBitrate    = head_buf.bytes_per_sec;
	obj->Info.bHasAudio       = 1;
	obj->Info.bAllowSeek      = 1;
	obj->Info.bHasVideo       = 0;
	obj->Info.AudioCodec      = 1;
	obj->Info.VideoCodec      = 0;
	obj->InnerPlayer.wav_data_len = head_buf.data_length;
	JzCbFileSeek(vfile,0,SEEK_SET);
	return 1;
}


////////////////////////////////////////////////////
// 功能: 得到WAV数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int GetWavPcmData(PAK_OBJECT obj, BYTE* pcm, int size)
{
	DWORD offset;
	int wlen;
	PAK_VFILE vfile;

	vfile = &obj->File;

	//直接跳过WAV的头
	offset = JzCbFileTell(vfile);
	if( offset < sizeof(WAVE_HEADER))
	{
		offset = 0;
		JzCbFileSeek(vfile,sizeof(WAVE_HEADER),SEEK_SET);
	}
	else
		offset -= sizeof(WAVE_HEADER);

	// 获取当前音频解码时间
	if( offset >= obj->InnerPlayer.wav_data_len)
	{
		obj->InnerPlayer.status = MEDIALIB_END;
		return -1;
	}
	obj->InnerPlayer.cur_time = ( offset / (obj->Info.AudioSamplerate * obj->Info.AudioChannels * sizeof(short)) ) * 1000;

	//读出数据
	wlen = JzCbFileRead((BYTE*)pcm,size,1,vfile);

	if( wlen > size )
	{
		obj->InnerPlayer.status = MEDIALIB_ERR;
		return -1;
	}
	else if( wlen == 0 )
	{
		obj->InnerPlayer.status = MEDIALIB_END;
		return -1;
	}


	//去掉WAV数据后面的无用数据，防止BOBO音产生
	offset += wlen;
	if( offset > obj->InnerPlayer.wav_data_len )
		wlen -= offset - obj->InnerPlayer.wav_data_len;

	return wlen;
}

////////////////////////////////////////////////////
// 功能: 关闭WAV数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int CloseWavDrv(PAK_OBJECT obj, PAK_VFILE vfile)
{
	return 1;
}

////////////////////////////////////////////////////
// 功能: wav mp3
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int WavSeek(PAK_OBJECT obj, int ms)
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
INNER_PLAYER_DRV WavDrvList = {
	OpenWavDrv,
	CloseWavDrv,
	GetWavInfo,
	WavSeek,
	GetWavPcmData
};
