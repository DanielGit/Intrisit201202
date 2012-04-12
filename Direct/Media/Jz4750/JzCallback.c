//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	            音频处理器回调函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/callback.h>
#include <kernel/device.h>
#include <kernel/thread.h>
#include <platform/platform.h>
#include <direct/media.h>
#include <config.h>

#ifndef WIN32

#if defined(CONFIG_MAKE_BIOS) && defined(CONFIG_MPLAYER_ENABLE)
#undef CONFIG_MPLAYER_ENABLE
#endif 

#if defined(CONFIG_MAC_ND800) || defined(CONFIG_MAC_BDS7100) || defined(CONFIG_MAC_ASKMI1388)
#define MPLAYE_MALLOC_SIZE (11 * 1024 * 1024)
#else
#define MPLAYE_MALLOC_SIZE (20 * 1024 * 1024)
#endif

#define MPLAYE_ONLY_MEDIA_MALLOC_SIZE (2 * 1024 * 1024)	//MPLAYER只播放音频使用的内存大小

#define MEDIA_GET_DATA_LEN	(1*1024)
#define	MEDIA_LIB_DIRECT	"\\\\DEVICE\\MRESFILE" 
#define MEMORY_ALIGN_ORDER 12

extern BYTE* pMediaPcmData;
extern DWORD nMediaPcmLen;
extern DWORD nMplayerDelay;
extern DWORD nMplayerDmaStart;

extern void ClockDelay(DWORD usec);
extern BYTE* LcdcVideoOsdBuf(void);
extern void SetMplayerEnd(void);
extern int GetDacBufCount();
extern int GetDacSpaceCount();
extern void GetMplayerResampleSize(DWORD len);
extern void MplayerWaiteDmaEnd();

extern void __icache_invalidate_all(void);
extern void __dcache_writeback_all(void);

static BYTE* MplayerUsrMem;
static DWORD MplayerAudioSpace;
static DWORD MplayerAudioLenFlag;
static DWORD MPlayerAllocMemSize;
static DWORD MplayerDelayCount = 0;
////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCbCheckFile(PAK_VFILE vfile)
{
	if(vfile->File)
		return kferror(vfile->File);
	else
		return 0;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCbFileRead(BYTE *buffer, DWORD count, DWORD size, PAK_VFILE vfile)
{
	int rsize;
	int remain;
	BYTE *dst;
	int rtotal;
	
	// 检查是否从文件中读取数据
	vfile = (PAK_VFILE)((DWORD)vfile|0x80000000);
	count = count * size;
	if(vfile->File)
	{
		// 获取剩余可读数据量
		remain = vfile->PlayLength - vfile->vOffset;
		if(!remain)
			return 0;
		
		// 读取数据
		rsize = (remain > count) ? count : remain;
		rsize = kfread(buffer, sizeof(char), rsize, vfile->File);
		vfile->vOffset += rsize;
		return rsize;
	}
		
	// 检查是否直接从用户缓冲区中读取数据
	if(!vfile->uCb)
	{
		int roffset;

		// 根据当前偏移，设置用户缓冲区偏移
		roffset = vfile->vOffset + vfile->PlayOffset;
		if((roffset < 0) || (roffset >= vfile->uSize))
			return -1;
		
		// 获取可读数据量
		remain = vfile->uSize - roffset;
		rsize = (remain > count) ? count : remain;
		remain = vfile->PlayLength - vfile->vOffset;
		rsize = (remain > rsize) ? rsize : remain;
		if(!rsize)
			return 0;
			
		// 读取数据
#if defined(CONFIG_MMU_ENABLE)
		ThreadSetAsid(vfile->Asid);
		kmemcpy(buffer, vfile->uBuf+roffset, rsize);
		ThreadResetAsid();
#else		
		kmemcpy(buffer, vfile->uBuf+roffset, rsize);
#endif
		vfile->vOffset += rsize;
		return rsize;
	}

	// 通过回调函数读取数据
	dst = buffer;
	rtotal = 0;
	while(count > 0)
	{
		int roffset;
		
		// 检查用户缓冲区中数据是否有效
		roffset = vfile->vOffset - vfile->uOffset;
		if((roffset >= vfile->uValid) || (roffset < 0) || !vfile->uValid)
		{
			// 重新读取有效数据
			remain = vfile->PlayLength - vfile->vOffset;
			rsize = (vfile->uSize > remain) ? remain : vfile->uSize;
			if(rsize)
			{
				MEDIA_CBPARAM param;

				param.bRead = 1;
				param.Offset = vfile->vOffset+vfile->PlayOffset;
				param.Count = rsize;
				param.Return = 0;
				
				// 执行回调函数
				if(KernCallbackFunc(vfile->uCb, &param) < 0)
					return -1;
				rsize = param.Return;
				if(rsize > 0)
				{
					vfile->uValid = rsize;
					vfile->uOffset = vfile->vOffset;
				}
				else
					break;
			}
		}
		
		// 获取当前可读数据量
		roffset = vfile->vOffset - vfile->uOffset;
		remain = vfile->uValid - roffset;
		rsize = (count > remain) ? remain : count;
		if(!rsize)
			break;
		
		// 读取数据到缓冲区
#if defined(CONFIG_MMU_ENABLE)
		ThreadSetAsid(vfile->Asid);
		kmemcpy(dst, vfile->uBuf + roffset, rsize);
		ThreadResetAsid();
#else		
		kmemcpy(dst, vfile->uBuf + roffset, rsize);
#endif
		dst += rsize;
		count -= rsize;
		rtotal += rsize;
		vfile->vOffset += rsize;
	}
	return rtotal;
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCbFileWrite(void *buffer, unsigned int count, unsigned int size, PAK_VFILE vfile)
{
	int wsize;
	int remain;
	int wtotal;
	BYTE *src;
	
	// 检查是否从文件中读取数据
	count = count * size;
	vfile = (PAK_VFILE)((DWORD)vfile|0x80000000);
	if(vfile->File)
	{
		// 获取剩余可写数据量
		remain = vfile->PlayLength - vfile->vOffset;
		if(!remain)
			return 0;
		
		// 写入数据
		wsize = (remain > count) ? count : remain;
		wsize = kfwrite(buffer, sizeof(char), wsize, vfile->File);
		vfile->vOffset += wsize;
		return wsize;
	}
		
	// 检查是否直接写入数据到用户缓冲区中
	if(!vfile->uCb)
	{
		int woffset;

		// 根据当前偏移，设置用户缓冲区偏移
		woffset = vfile->vOffset + vfile->PlayOffset;
		if((woffset < 0) || (woffset >= vfile->uSize))
			return -1;
		
		// 获取剩余可写数据量
		remain = vfile->uSize - woffset;
		wsize = (remain > count) ? count : remain;
		remain = vfile->PlayLength - vfile->vOffset;
		wsize = (remain > wsize) ? wsize : remain;
		if(!wsize)
			return 0;
			
		// 写入数据
#if defined(CONFIG_MMU_ENABLE)
		ThreadSetAsid(vfile->Asid);
		kmemcpy(vfile->uBuf + woffset, buffer, wsize);
		ThreadResetAsid();
#else		
		kmemcpy(vfile->uBuf + woffset, buffer, wsize);
#endif
		vfile->vOffset += wsize;
		return wsize;
	}

	// 通过回调函数读取数据
	src = buffer;
	wtotal = 0;
	while(count > 0)
	{
		int woffset;
		
		// 检查需要Flush用户缓冲区中的数据
		woffset = vfile->vOffset - vfile->uOffset;
		if((woffset >= vfile->uValid) || (woffset < 0) || (vfile->uValid == vfile->uSize))
		{
			if(vfile->uValid)
			{
				MEDIA_CBPARAM param;
				
				param.bRead = 0;
				param.Offset = vfile->uOffset + vfile->PlayOffset;
				param.Count = vfile->uValid;
				param.Return = 0;
					
				// 执行回调函数
				if(KernCallbackFunc(vfile->uCb, &param) < 0)
					return -1;
				if(param.Return != vfile->uValid)
					return -1;
			
				vfile->uOffset = vfile->vOffset;
				vfile->uValid = 0;
			}
		}
		
		// 获取当前写入数据量
		woffset = vfile->vOffset - vfile->uOffset;
		remain = vfile->uSize - woffset;
		wsize = (remain > count) ? count : remain;
		remain = vfile->PlayLength - vfile->vOffset;
		wsize = (remain > wsize) ? wsize : remain;
		if(!wsize)
			break;
		
		// 读取数据到缓冲区
#if defined(CONFIG_MMU_ENABLE)
		ThreadSetAsid(vfile->Asid);
		kmemcpy(vfile->uBuf + woffset, src, wsize);
		ThreadResetAsid();
#else		
		kmemcpy(vfile->uBuf + woffset, src, wsize);
#endif
		src += wsize;
		vfile->uValid += wsize;
		vfile->vOffset += wsize;
		count -= wsize;
		wtotal += wsize;
	}
	return wtotal;
}



////////////////////////////////////////////////////
// 功能: Flush缓冲区中数据
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCbFileWriteFlush(PAK_VFILE vfile)
{
	vfile = (PAK_VFILE)((DWORD)vfile|0x80000000);
	if(vfile->File || !vfile->uCb)
		return 0;
		
	// 检查缓冲区中是否有数据
	if(vfile->uValid)
	{
		MEDIA_CBPARAM param;
		
		param.bRead = 0;
		param.Offset = vfile->uOffset + vfile->PlayOffset;
		param.Count = vfile->uValid;
		param.Return = 0;
			
		// 执行回调函数
		if(KernCallbackFunc(vfile->uCb, &param) < 0)
			return -1;
	
		vfile->uValid = 0;
		vfile->uOffset = vfile->vOffset;
		return param.Return;
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
int JzCbFileSeek(PAK_VFILE vfile, int offset, int origin)
{
	int voffset;
	
	// 设置虚拟文件指针
	vfile = (PAK_VFILE)((DWORD)vfile|0x80000000);
	voffset = vfile->vOffset;
	switch (origin)
	{
	case 0:
		if(offset > vfile->PlayLength)
			return -1;
		voffset = offset;
		break;
	case 1:
		if(((voffset + (int)offset) > vfile->PlayLength)
			|| ((voffset + (int)offset) < 0))
		{
			return -1;
		}
		voffset += offset;
		break;
	case 2:
		if((offset > 0) || (((int)offset + vfile->PlayLength) < 0))
			return -1;
		voffset = vfile->PlayLength + offset;
		break;
	default:
		return -1;
	}
	
	// 实际文件搜索
	if(vfile->File)
	{
		if(kfseek(vfile->File, voffset+vfile->PlayOffset, SEEK_SET) < 0)
			return -1;
	}
	
	// 保存新的偏移值
	//seek 越界处理
	if( voffset < 0 )
		voffset = 0;
	vfile->vOffset = voffset;
	return voffset;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int JzCbFileTell(PAK_VFILE vfile)
{
	vfile = (PAK_VFILE)((DWORD)vfile|0x80000000);
	return vfile->vOffset;
}

int PPagePhySpace(DWORD addr);
void PPageFreePhy(DWORD *pentry, int pages);
int PPageAllocPhy(DWORD *pentry, int pages);
void BootCleanInvalidateAll(void);
////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void* JzCbMalloc(DWORD size)
{
	DWORD pages;
	DWORD *pentry;
	DWORD addr;
	
	// 检查是否小内存块
	if(size < 32*1024)
		return kmalloc(size);
	
	// 大内存块，则直接申请连续页内存
	size += 2 * sizeof(DWORD);
	pages = (size + 4095) / 4096;
	pentry = kmalloc(sizeof(DWORD)*pages);
	if(pentry == NULL)
	{
		kdebug(mod_media, PRINT_ERROR, "JzCbMalloc malloc failed\n");
		return NULL;
	}
	kmemset(pentry, 0x00, sizeof(DWORD)*pages);
	SchedLock();
	if(PPageAllocPhy(pentry, pages) < 0)
	{
		SchedUnlock();
		kdebug(mod_media, PRINT_ERROR, "JzCbMalloc malloc error\n");
		kfree(pentry);
		return NULL;
	}
	SchedUnlock();
	addr = *pentry;
	addr &= 0x0fffffff;
	addr |= 0x80000000;
	*(DWORD*)(addr) = (DWORD)pentry;
	*(DWORD*)(addr+sizeof(DWORD)) = pages;	
	return (void*)(addr+2*sizeof(DWORD));
}


////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void* JzCbFree(void* mem)
{
	DWORD t;
	
	if(!mem)
		return NULL;
	t = (DWORD)mem;
	t |= 0x80000000;
	
	// 检查是否为页块空间
	if(PPagePhySpace(t))
	{
		DWORD pages;
		DWORD pentry;
		
		// 内容同步
		__dcache_writeback_all();
		__icache_invalidate_all();
		
		// 释放空间
		pentry = *(DWORD*)(t-2*sizeof(DWORD));
		pages = *(DWORD*)(t-sizeof(DWORD));
		SchedLock();
		PPageFreePhy((DWORD*)pentry, pages);
		SchedUnlock();
		kfree((void*)pentry);
		return NULL;
	}
	
	kfree((void*)t);
	return NULL;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static PAK_VFILE GetPluginFile()
{
	int fsize;
	PAK_VFILE vfile;

	vfile = (PAK_VFILE)kmalloc(sizeof(AK_VFILE));
	if( vfile == 0)
	{
		kdebug(mod_media, PRINT_ERROR, "open plugin file malloc error\n");
		return 0;
	}

	//初始化vfile
	kmemset(vfile,0,sizeof(AK_VFILE));

	// 打开文件
	vfile->File = kfopen(MEDIA_LIB_DIRECT, "rb");
	if(vfile->File == NULL)
	{
		kdebug(mod_media, PRINT_ERROR, "plugin file open failed\n");
		return 0;
	}
	
	// 获取文件大小
	fsize = kfsize(vfile->File);
	kfseek(vfile->File, 0, SEEK_SET);
	
	// 初始化缓冲参数
	vfile->PlayLength = fsize;
	vfile->PlayOffset = 0;
	vfile->vOffset = 0;
	return vfile;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static void AkCbDelay(DWORD tick)
{
	MplayerDelayCount += tick;
	sTimerSleep(MplayerDelayCount, NULL);
	MplayerDelayCount = MplayerDelayCount % 10;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static void AkCbOpenVideoOsd(int w, int h)
{
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int os_audio_get_space()
{
	return GetDacSpaceCount();
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int null_func ()
{
	return 0;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int return_func_1()
{
	return 1;
	return_func_1();
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int return_func_ff()
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
static int os_audio_init()
{
	nMplayerDmaStart = 0;
	return 1;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static float os_audio_get_delay()
{
	int len;
	float time;
	len = GetDacBufCount();
	time = ((float)len) / 1000.0;
	return time;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static int os_audio_play(void* data,int len,int flags)
{
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	kmemcpy((BYTE*)pMediaPcmData,(BYTE*)data,len);
	nMediaPcmLen = len;
	GetMplayerResampleSize(len);
	if(flags)
		nMediaPcmLen |= 0x80000000;
	return len;
#else
	return 0;
#endif	
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
static void os_audio_resume()
{
	MplayerWaiteDmaEnd();
	nMplayerDelay = 0;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
int GetMplayerMem(int mode)
{
	int i;

	if( mode == 2 )
		MPlayerAllocMemSize = MPLAYE_ONLY_MEDIA_MALLOC_SIZE;	//只播放音频
	else
		MPlayerAllocMemSize = MPLAYE_MALLOC_SIZE;				//音频，视频均播放

	for( i =  0 ; i < 3 ; i++ )
	{
		MplayerUsrMem = (BYTE*)JzCbMalloc(MPlayerAllocMemSize);
		if(MplayerUsrMem)
			return 0;
		MPlayerAllocMemSize = (MPlayerAllocMemSize >> 1) + (MPlayerAllocMemSize >> 2);
		kdebug(mod_media, PRINT_WARNING, "mplayer malloc failed,beigin malloc %x memory\n",MPlayerAllocMemSize);
	}
	kdebug(mod_media, PRINT_ERROR, "mplayer malloc failed\n");
	return 1;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void FreeMplayerMem()
{
	if( MplayerUsrMem )
		JzCbFree(MplayerUsrMem);
	MplayerUsrMem = NULL;
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void SetMplayerAudioSpace(PAK_OBJECT obj)
{
	PMEDIA_INFO			pMediaInfo;
	PJz47_AV_Decoder	pAvDecoder;
	pAvDecoder = obj->JzAvDecoder;
	pMediaInfo = &pAvDecoder->MediaInfo;

	if(!pMediaInfo->bHasVideo)
	{
		MplayerAudioSpace = MEDIA_GET_DATA_LEN;
	}
	else
	{
		if(pMediaInfo->AudioSamplerate < 44100)
			MplayerAudioSpace = MEDIA_GET_DATA_LEN;
		else
			MplayerAudioSpace = MEDIA_GET_DATA_LEN * 2;
		if(pMediaInfo->VideoWidth > 480)
			MplayerAudioSpace *= 2;
	}
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void JzLongJmp(void* stream,jmp_buf buf, int ret_val)
{
	PAK_VFILE File;
	File = (PAK_VFILE)stream;
	if( JzCbCheckFile(File) >= 0 )
		return;
	klongjmp(buf,ret_val);
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
PJz47_AV_Decoder GetCbDecoder()
{
	return (PJz47_AV_Decoder)kmalloc(sizeof(Jz47_AV_Decoder));
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void get_mplayer_plugin()
{
	HANDLE fp;
	fp = kfopen(MEDIA_LIB_DIRECT,"rb");
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void AkCbReinit(PJz47_AV_Decoder cb, PAK_OBJECT obj )
{
	PAK_VFILE File;
	PMEDIA_TASK Task; 

	File = &obj->File;
	Task = &obj->Task;
	
	cb->UsrLcdPosX  = Task->ShowRect.x;
	cb->UsrLcdPosY  = Task->ShowRect.y;
	cb->UsrLcdWidth = Task->ShowRect.w;
	cb->UsrLcdHeight= Task->ShowRect.h;

	//初始化传送文件名
	kmemset(cb->FileName,0,256);

	//得到文件名的长度，以及文件名开始的位置
	cb->FileName[0] = 'a';
	cb->FileName[1] = '.';
	cb->FileName[2] = File->FileType[0];
	cb->FileName[3] = File->FileType[1];
	cb->FileName[4] = File->FileType[2];
	cb->FileName[5] = File->FileType[3];
	cb->FileName[6] = 0;

	cb->stream = (void*)File;
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	//初始化回调函数指针
	noah_os_init(cb);
#endif
}

////////////////////////////////////////////////////
// 功能:
// 输入:
// 输出:
// 返回:
// 说明:
////////////////////////////////////////////////////
void AkCbInit(PJz47_AV_Decoder cb, PAK_OBJECT obj)
{
	PMEDIA_TASK Task; 

	Task = &obj->Task;
	GetMplayerMem(Task->ExterdType);

	cb->lcd_width = CONFIG_MAINLCD_XSIZE;
	cb->lcd_height = CONFIG_MAINLCD_YSIZE;
	cb->lcd_line_length = CONFIG_MAINLCD_XSIZE * CONFIG_MAINLCD_BPP / 8;
#if (CONFIG_MAINLCD_BPP==32)
	cb->OutFormat = CONFIG_MPLAYER_OUT_FORMAT32;
#else
	cb->OutFormat = CONFIG_MPLAYER_OUT_FORMAT16;
#endif
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	cb->lcd_frame_buffer = LcdcVideoOsdBuf();
#else
	cb->lcd_frame_buffer = NULL;
#endif

	cb->os_ftell = (void*)JzCbFileTell;
	cb->os_fread = (void*)JzCbFileRead;
	cb->os_fwrite = (void*)JzCbFileWrite;
	cb->os_fseek  = (void*)JzCbFileSeek;
	cb->plugstream = GetPluginFile();
	cb->pJzDecoderBuf = MplayerUsrMem;
	cb->malloc_buf = ((unsigned int)cb->pJzDecoderBuf + (1 << MEMORY_ALIGN_ORDER) - 1) & ~((1 << MEMORY_ALIGN_ORDER) - 1);  
	
	kdebug(mod_media, PRINT_INFO, "++++ cb->malloc_buf = 0x%08x ++++++\n", cb->malloc_buf);
	
	cb->malloc_size = MPlayerAllocMemSize - (1 << MEMORY_ALIGN_ORDER);
	
	// sleep func callback
	MplayerDelayCount = 0;
	cb->os_msleep  = (void *)AkCbDelay;	//sTimerSleep;
	
	// audio playing functions
	cb->os_audio_init =   (void *)os_audio_init;			/* int init(int rate,int channels,int format,int flags), return 1 if succes, else return 0.*/
	cb->os_audio_uninit = (void *)null_func;				/* void uninit(int immed) */
	cb->os_audio_reset =  (void *)null_func;				/* void reset(void) */
	cb->os_audio_pause =  (void *)null_func;				/* void audio_pause(void) */
	cb->os_audio_resume = (void *)os_audio_resume;			/* void audio_resume(void) */
	cb->os_audio_get_space = (void *)os_audio_get_space;	/* int get_space(void) */
	cb->os_audio_play =  (void *)os_audio_play;				/* int play(void* data,int len,int flags), flags = 1 is end chunk */
	cb->os_audio_get_delay = (void *)os_audio_get_delay;	/* float get_delay(void) */
	cb->os_audio_control  = (void *)return_func_ff;			/* int control(int cmd,void *arg), return -1 is not support */
	cb->os_open_video_osd = (void *)AkCbOpenVideoOsd;

	cb->os_audio_long_jump = (void *)JzLongJmp;
	cb->fMplayerInit       = 0;

	cb->fIpuEnable     = 0;		//是否可以进行IPU
	cb->kprintf_enable = 0;		//是否打印mplayer中的信息

	MplayerAudioLenFlag = 0;	//
#if defined(CONFIG_MPLAYER_ENABLE) && !defined(CONFIG_MAKE_BIOS)
	//初始化回调函数指针
	noah_os_init(cb);
#endif
}

#endif
