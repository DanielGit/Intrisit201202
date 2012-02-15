//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//   	             
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2008-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/device.h>
#include <kernel/timer.h>
#include <kernel/sched.h>
#include <kernel/callback.h>
#include <direct/medialib/resample.h>

#if defined(CONFIG_MAC_BDS6100) || defined(CONFIG_MAC_ND800) || defined(CONFIG_MAC_ASKMI1388) || defined(CONFIG_MAC_BDS6100A)
#define DAC_PCMBUF_SIZE			(1024*2)
#else
#define DAC_PCMBUF_SIZE			(1024*2)
#endif

#define DAC_SAMPLE_RATIO		48000
#define FILTER_PCMBUF_SIZE		(1024*2)
#define MAX_PCMBUFS			32

#define WAOLA_PCM_BUF_SIZE		(FILTER_PCMBUF_SIZE * 4)

#define DAC_BUF_READ			0
#define DAC_BUF_READING			1
#define DAC_BUF_WRITE			2

#define INIT_DMA_TRANS		0
#define PAUSE_DMA_TRANS		1
#define STOP_DMA_TRANS		2
#define END_DMA_TRANS		3
#define RUNING_DMA_TRANS	4

typedef struct _RESAMPLE
{
	short Buf[MAX_PCMBUFS][DAC_PCMBUF_SIZE/sizeof(short)];
	BYTE BufFlag[MAX_PCMBUFS];
	BYTE ReadBuf;
	BYTE WriteBuf;
	BYTE bPass;

	int WriteOffset;
	int LFilterQ[4];
	int RFilterQ[4];
}RESAMPLE;
typedef RESAMPLE *PRESAMPLE;

typedef struct _DAC_DEVICE
{
	LIST Link;
	int Samprate;
	int Channel;
	int Volume;
	int Tempo;
	int Pause;
	int fThreadStart;

	HANDLE   hWsola;
	short*   WsolaBuf;

	HANDLE   hSample;
	RESAMPLE Resample;
	short*   pSamplerate;
	DWORD    SamplerateSize;
}DAC_DEVICE;
typedef DAC_DEVICE *PDAC_DEVICE;
static HANDLE hDacMutex;
static HANDLE hDacSema;
static LIST DacList;
static short gVolumeSpeaker;	// 全局喇叭音量
static char bHeadphoneIn;
static char fBeginDma;
static int  fPause;
static RESAMPLE DacDevice __attribute__((aligned(32)));		//存放DMA数据链
static short NullBuf[DAC_PCMBUF_SIZE/sizeof(short)];
static DWORD nErrorData = 0;	//出现越界的数据个数

extern void SetMuteMode(char mode);
extern void MediaDraveInit(char channel);
extern void OpenMediaDevice(unsigned long sample,unsigned long channle , char mode);	//打开DAC/ADC
extern void CloseMediaDevice(char channel);			//关闭DAC/ADC
extern void DmaDataToAic(int arg,unsigned int sour_addr, unsigned int len,unsigned char mode);	//DMA数据到AIC
extern void DestroyMediaDevoice(char mode);
extern void DacDestorySamplerate(HANDLE hdac);
extern void SetMoseCe(char io);
extern void SetPowerAmplifier(char io);
extern void GetDmaInfo();
extern void BootGpioOut(DWORD pin, int data);
extern void DacDestoryWsola(HANDLE hdac);
extern int BootGpioIn(DWORD pin);
extern HANDLE WsolaCreate(DWORD sample_rate, DWORD channel_count, DWORD options, int rate);
extern int WsolaDestroy(HANDLE hwsola);
extern int WsolaConvert(HANDLE hwsola, PAUDIO_FILTER filter);
extern DWORD GetDmaCount();
extern int GetDacChannel();
extern void MediaSysInit();
extern void MillinsecoundDelay(unsigned int msec);
static void DacWaiteDmaEnd();

extern int DacGetGloalVolume();

extern DWORD TimerCount(void);

//#define DAC_SAVE_PCM
#ifdef DAC_SAVE_PCM
static DWORD dac_offset = 0;
static BYTE dac_buf[5 * 1024 * 1024];
static DWORD dac_source_offset = 0;
static BYTE dac_source_buf[3*1024*1024];
#endif
////////////////////////////////////////////////////
// 功能: 设定音频输出设备
// 输入: 
// 输出:
// 返回: 
// 说明: 返回是否插入耳机
////////////////////////////////////////////////////
static void PrintfDacStatus(PRESAMPLE presample)
{
	int i;

	kprintf("dac flag buf:");
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		kprintf("%d,", presample->BufFlag[i]);
	kprintf("\n");
	kprintf("read buf idx = %d, write buf idx = %d\n",presample->ReadBuf,presample->WriteBuf);
	return;
	PrintfDacStatus(presample);
}

////////////////////////////////////////////////////
// 功能: 设定音频输出设备
// 输入: 
// 输出:
// 返回: 
// 说明: 返回是否插入耳机
////////////////////////////////////////////////////
static int DacOutDeviceSet(void)
{
#if defined(CONFIG_ERAPHONE_IO)	
#if defined(CONFIG_AMP2_IO)	
	/////////////////////////////////////	
	// 掌机和点读板存在输出的情况
	/////////////////////////////////////

	// 1. 检查是否视频接入，有则输出到耳机
#if defined(CONFIG_TVOUT_ENABLE)	
	if(LcdcIsTvoutOn())
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		BootGpioOut(CONFIG_AMP2_IO, CONFIG_AMP2_DIS);
		return 0;
	}
#endif

	// 2. 检查是否耳机接入
	if(!BootGpioIn(CONFIG_ERAPHONE_IO))
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		BootGpioOut(CONFIG_AMP2_IO, CONFIG_AMP2_DIS);
		return 1;
	}

	// 3. 检查是否掌机放置在点读盒中
	if(BootGpioIn(GPIO_PALM_DETECT))
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		BootGpioOut(CONFIG_AMP2_IO, CONFIG_AMP2_ENA);
	}
	else
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_ENA);
		BootGpioOut(CONFIG_AMP2_IO, CONFIG_AMP2_DIS);
	}
	return 0;

#else
	/////////////////////////////////////	
	// 只有掌机存在输出的情况
	/////////////////////////////////////

	// 1. 检查是否视频接入，有则输出到耳机
#if defined(CONFIG_TVOUT_ENABLE)	
	if(LcdcIsTvoutOn())
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		return 0;
	}
#endif

	// 2. 检查是否耳机接入
	if(!BootGpioIn(CONFIG_ERAPHONE_IO))
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		return 1;
	}

	// 3. 喇叭输出
	BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_ENA);
	return 0;
#endif
#else
	return 0;	
#endif
}

////////////////////////////////////////////////////
// 功能: 获取播放缓冲区指针
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static short *DacGetPcmBuf(PDAC_DEVICE dac)
{
	PRESAMPLE presample;

	presample = &dac->Resample;
	if(presample->BufFlag[presample->ReadBuf] != DAC_BUF_READ)
		return NULL;
	return presample->Buf[presample->ReadBuf];
}



////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacGetPcmOk(PDAC_DEVICE dac)
{
	PRESAMPLE presample;

	presample = &dac->Resample;
	presample->BufFlag[presample->ReadBuf] = DAC_BUF_WRITE;

	if(++presample->ReadBuf == MAX_PCMBUFS)
		presample->ReadBuf = 0;
}

////////////////////////////////////////////////////
// 功能: 把数据存放到DMA缓冲中
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int SavePcmData( short* pcm )
{
	PRESAMPLE pResample;

	pResample = &DacDevice;

	if( pResample->BufFlag[pResample->WriteBuf] == DAC_BUF_WRITE )
	{	//可以写入数据,将数据写入DMA数据
		kmemcpy((BYTE*)pResample->Buf[pResample->WriteBuf] , (BYTE*)pcm, DAC_PCMBUF_SIZE);
		pResample->BufFlag[pResample->WriteBuf] = DAC_BUF_READ;
		if( ++pResample->WriteBuf == MAX_PCMBUFS )
			pResample->WriteBuf = 0;
		return 1;
	}
	// 	else 
	// 		kprintf("save pcm data failed\n");
	return 0;
}

////////////////////////////////////////////////////
// 功能: 把DMA中的数据送入I2S
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void StartDmaPcmTrans(int mute, unsigned int sour)
{
	PRESAMPLE pResample;
	static DWORD mute_data;

	pResample = &DacDevice;

	//判断是否能传递数据
	if(!mute)
	{	
		//传送PCM数据
		DmaDataToAic(0,(unsigned int)sour, DAC_PCMBUF_SIZE,1);
#ifdef DAC_SAVE_PCM
		kmemcpy(&dac_buf[dac_offset],(BYTE*)sour,DAC_PCMBUF_SIZE);
		dac_offset += DAC_PCMBUF_SIZE;
		if( dac_offset >= 4 * 1024 * 1024 )
			dac_offset = 0;
#endif
		mute_data = 0;
	} else
	{	
		//传送空的数据
		DmaDataToAic(0, (unsigned int)NullBuf, DAC_PCMBUF_SIZE,1);
#ifdef DAC_SAVE_PCM
		kmemcpy(&dac_buf[dac_offset],(BYTE*)NullBuf,DAC_PCMBUF_SIZE);
		dac_offset += DAC_PCMBUF_SIZE;
		if( dac_offset >= 4 * 1024 * 1024 )
			dac_offset = 0;
#endif
		if( !fPause )
			kdebug(mod_audio, PRINT_INFO, "MUTE: %d-%d\n", ++mute_data, fBeginDma);
	}
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacWriteThread(DWORD p)
{
	int bopen;
	char play_flag;
	short *pcm;
	short gvol;

	pcm = NULL;
	bopen = 0;
	gvol = 0;
	for(;;)
	{	
		PLIST head, n;
		short *src[4];
		short vol[4];
		PDAC_DEVICE dac[4];
		int chs;
		int i, j;
		int tmp;

		// 等待DAC设备创建
		if(!bopen)
		{
			kSemaWait(hDacSema, 0);
			bopen = 1;
			pcm = (short*)kmalloc(DAC_PCMBUF_SIZE);
		}

		// 检查DAC是否已经关闭
		kMutexWait(hDacMutex);
		if(ListEmpty(&DacList))
		{
			if(pcm)
			{
				kfree(pcm);
				pcm = NULL;
			}
			bopen = 0;
			kMutexRelease(hDacMutex);
			continue;
		}

		// 检查缓冲区是否获取
		if(pcm == NULL)
		{
			kMutexRelease(hDacMutex);
			sTimerSleep(1000, NULL);
			continue;
		}

		// 获取采样数据到临时缓冲区
		head = &DacList;
		chs = play_flag = 0;
		for(n=ListFirst(head); n!=head; n=ListNext(n))
		{
			dac[chs] = ListEntry(n, DAC_DEVICE, Link);
			src[chs] = DacGetPcmBuf(dac[chs]);
			vol[chs] = dac[chs]->Volume * DacGetGloalVolume()  / 10;

			if( dac[chs]->fThreadStart )
				play_flag = 1;
			if(++chs == 4)
				break;		// 最多合成4通道
		}

		// 检查是否存在有效数据
		for(i=0; i<chs; i++)
		{
			if(src[i])
				break;
		}

		if( play_flag == 0 )
		{
			kMutexRelease(hDacMutex);
			sTimerSleep(20, NULL);
			continue;
		}

		if(i == chs )
		{
			kMutexRelease(hDacMutex);
			sTimerSleep(20, NULL);
			continue;
		}

		if(chs > 1)
		{
			// 强制同步合成声音. 注意:
			// 1. 两路声音, 单独播放时不会卡, 但是合成播放卡, 可能是因为这两路声音s
			// 产生数据的 速率 不同造成的. 当其中一路很快, 另外一路很慢,
			// 就会以 速率 快的一路来合成声音, 导致 速率 慢的一路声音卡.
			// 2. 解决方法是强制快的一路等待慢的一路.
			// 3. 但是声音暂停和声音在停止过程中这两种情况, 不需要同步.
			for(i = 0; i < chs; i++)
			{
				//kprintf("src[%d]:0x%08x, Pause:%d, fThreadStart:%d, vol[%d]:%d\n", i, src[i], dac[i]->Pause, dac[i]->fThreadStart, i, vol[i]);
				// 无数据, 非暂停, 已经开始合成, 声音不是在停止的过程中
				if( !src[i] && !dac[i]->Pause && dac[i]->fThreadStart == 1 )
				{
					break;
				}
			}

			// 表示有某路声音没有数据, 但是不表示这路声音会卡.
			if(i != chs)
			{
				int buf_cnt = 0;
				PRESAMPLE presample;

				// 求DAC输出缓冲中还有多少个 有数据 的缓冲.
				presample = &DacDevice;
				for( j = 0 ; j < MAX_PCMBUFS ; j++ )
				{
					if( presample->BufFlag[j] == DAC_BUF_READ )
						buf_cnt++;
				}

				// 如果DAC缓冲少, 认为 有可能 会造成所有声音都卡, 所以合成有
				// 数据的声音. 其结果是没有数据的那路声音一定会卡,
				// 即使这路声音本身 已经存有(缓冲中) 的数据播放是不会卡的.
				// DAC缓冲多, 那么睡眠, 等待没有数据的那路声音.
				if(buf_cnt <= 11)
				{
					//kprintf("bufcount:%d\n", buf_cnt);
				}
				else
				{
					//kprintf("bufcount:%d\n", buf_cnt);
					kMutexRelease(hDacMutex);
					sTimerSleep(10, NULL);
					continue;
				}
			}
		}

		// 合成多通道音频数据
		if(chs > 1)
		{
			for(i=0; i<DAC_PCMBUF_SIZE/sizeof(short); i++)
			{
				tmp = 0;
				for(j=0; j<chs; j++)
				{
					if(src[j] && vol[j])
						tmp += src[j][i] * vol[j];
				}
				tmp /= 1024;
				if(tmp >= 32767)
				{
					pcm[i] = 32767;
					nErrorData++;
				}
				else if(tmp < -32767)
				{
					pcm[i] = -32767;
					nErrorData++;
				}
				else
					pcm[i] = (short)tmp;
			}
		}
		else	// 处理单通道音频数据
		{
			short volume;

			volume = vol[0];
			for(i=0; i<DAC_PCMBUF_SIZE/sizeof(short); i++)
				pcm[i] = (short)((src[0][i] * volume) / 1024);
		}

		DacOutDeviceSet();

		// 写入音频数据到DMA处理缓冲
		if(SavePcmData(pcm))
		{
			// 设置缓冲区读取完毕
			for(i=0; i<chs; i++)
			{
				if(src[i] != NULL)
					DacGetPcmOk(dac[i]);
			}

			if( fBeginDma == INIT_DMA_TRANS )
			{
				fBeginDma = RUNING_DMA_TRANS;
			}
			kMutexRelease(hDacMutex);
		}
		else
		{
			kMutexRelease(hDacMutex);
			sTimerSleep(10, NULL);
		}
	}
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacDeviceOpen(void)
{
	int i;

	MediaDraveInit(1);

	// 这个函数初始化波特率与声道, 与pop noise没关系
	OpenMediaDevice(DAC_SAMPLE_RATIO,2,0);

	//初始化当前DMA状态为INIT状态
	fBeginDma = INIT_DMA_TRANS;

	//初始化结构体数据
	kmemset(&DacDevice,0,sizeof(RESAMPLE));

	//初始化缓冲
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		DacDevice.BufFlag[i] = DAC_BUF_WRITE;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacDeviceClose(void)
{
	//初始化当前DMA状态为END状态
	fBeginDma = END_DMA_TRANS;

	//关闭声音驱动
	CloseMediaDevice(1);

	//删除放音中断向量表
	DestroyMediaDevoice(0);
}


////////////////////////////////////////////////////
// 功能: 打开耳机
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacHeadphoneOpen(void)
{
	return;
}


////////////////////////////////////////////////////
// 功能: 关闭耳机
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacHeadphoneClose(void)
{
	return;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacInit(void)
{
	int i;
	// 全局变量初始化
	hDacMutex = kMutexCreate();
	hDacSema = kSemaCreate(0);
	ListInit(&DacList);
	gVolumeSpeaker = 99;
	bHeadphoneIn = 0;
#ifdef DAC_SAVE_PCM
	dac_offset = 0;
	dac_source_offset = 0;
#endif

	//初始化结构体数据
	kmemset(&DacDevice,0,sizeof(RESAMPLE));
	kmemset((BYTE*)NullBuf,0xff,DAC_PCMBUF_SIZE);

	//初始化缓冲
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		DacDevice.BufFlag[i] = DAC_BUF_WRITE;

	// 创建DAC设备写线程
	KernelThread(PRIO_USER-11, DacWriteThread, 0, 0);


#if defined(CONFIG_ERAPHONE_IO)
	__gpio_as_input(CONFIG_ERAPHONE_IO);
	__gpio_enable_pull(CONFIG_ERAPHONE_IO);
#endif	

	//初始化播放声音的GPIO端口
	SetMoseCe(0);

	//D类功放开机去喀嚓音的特殊处理
#if defined(CONFIG_MAC_BDS6100) || defined(CONFIG_MAC_ND800) || defined(CONFIG_MAC_ASKMI1388) || defined(CONFIG_MAC_BDS6100A) || defined(CONFIG_MAC_NP7000) || defined( CONFIG_MAC_NP2300 )
	SetPowerAmplifier(1);
	SetPowerAmplifier(0);
#else
	SetPowerAmplifier(0);
#endif
	return 0;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
HANDLE DacOpen(void)
{
	PDAC_DEVICE dac;
	PLIST head;
	PLIST n;
	int open_devs;
	int i;

	// 获取打开设备数量
	kMutexWait(hDacMutex);
	head = &DacList;
	open_devs = 0;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
		open_devs++;

	if(open_devs == 4)
	{
		kdebug(mod_audio, PRINT_WARNING, "no dac for open\n");
		kMutexRelease(hDacMutex);
		return NULL;
	}

	dac = kmalloc(sizeof(DAC_DEVICE));
	if(dac == NULL)
	{
		kdebug(mod_audio, PRINT_ERROR, "open dac malloc failed\n");
		kMutexRelease(hDacMutex);
		return NULL;
	}
	// 初始化设备对象
	kmemset(dac, 0x00, sizeof(DAC_DEVICE));

	dac->Channel  = 0;
	dac->Samprate = 0;
	dac->Volume   = 0;
	dac->hWsola   = NULL;
	dac->WsolaBuf = NULL;
	dac->pSamplerate = NULL;
	for(i=0; i<MAX_PCMBUFS; i++)
		dac->Resample.BufFlag[i] = DAC_BUF_WRITE;
	dac->Resample.WriteOffset = 0;
	dac->Resample.ReadBuf = 0;
	dac->Resample.WriteBuf = 0;
	nErrorData = 0;
	ListInit(&dac->Link);

#ifdef DAC_SAVE_PCM
	dac_offset = 0;
	dac_source_offset = 0;
#endif
	// 检查当前设备是否已经打开
	if(ListEmpty(&DacList))
	{
		// 本来打开功放, mos管和开dac的工作应该在ListEmpty()==true里面进行.
		// 但是当打开两路声音, 而且其中一路在暂停状态, 因为暂停的时候关闭了功放和dac,
		// 所以需要再次打开功放和dac, 于是改成不在判断语句内打开, 对于多个声音合成,
		// 相当于多打开一次dac和功放, 应该没有影响.
#if defined(CONFIG_MAC_NP7000) || defined(CONFIG_MAC_NP2300)
		MillinsecoundDelay(250);

		SetMoseCe(1);
#else
		SetPowerAmplifier(1);
#endif
		// 打开DA设备
		DacDeviceOpen();

		// 打开耳机设备
		DacHeadphoneOpen();

		//开启DMA数据传送，同时只传送静音数据
		StartDmaPcmTrans(1,0);

		// 释放设备打开信号
		kSemaRelease(hDacSema);

		ListInsert(&DacList, &dac->Link);

		//只有第一次打开声音设备才需要去掉BOBO音
#if defined(CONFIG_MAC_NP7000) || defined(CONFIG_MAC_NP2300)
		MillinsecoundDelay(250);

		SetPowerAmplifier(1);
#else
		MillinsecoundDelay(120);

		SetMoseCe(1);

		MillinsecoundDelay(130);

		SetMuteMode(1);
#endif
	}
	else
	{
		//防止一个声音暂停时，另外一个声音无法播放的问题
		SetMuteMode(1);
		ListInsert(&DacList, &dac->Link);
	}


	kMutexRelease(hDacMutex);
	return (HANDLE)dac;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacClose(HANDLE hdac)
{
	PDAC_DEVICE dac;

	kMutexWait(hDacMutex);
	dac = (PDAC_DEVICE)hdac;
	kdebug(mod_audio, PRINT_INFO, "dac error data number = %d\n",nErrorData);
	if(dac)
	{
		ListRemove(&dac->Link);
		if(ListEmpty(&DacList))
		{
			MillinsecoundDelay(20);

			SetMuteMode(0);

			//关闭功放
			SetPowerAmplifier(0);

			MillinsecoundDelay(20);

			SetMoseCe(0);

			// 关闭耳机设备
			DacHeadphoneClose();		

			// 关闭DA设备
			DacDeviceClose();

			// 没有这个耳机会有爆破音.
			MillinsecoundDelay(20);
		}

		DacDestorySamplerate(dac);	//释放resample数据
		DacDestoryWsola(hdac);		//释放wsola数据
#ifdef DAC_SAVE_PCM
		{
			HANDLE fp;
			fp = kfopen("d:\\pcm.bin","w+b");
			kfwrite(dac_buf,1,dac_offset,fp);
			kfclose(fp);

			fp = kfopen("d:\\source.bin","w+b");
			kfwrite(dac_source_buf,1,dac_source_offset,fp);
			kfclose(fp);
		}
#endif
		kfree(dac);
		kMutexRelease(hDacMutex);
		return 0;
	}
	nErrorData = 0;
	kMutexRelease(hDacMutex);
	return -1;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int DacPcmQueue(short *dst, short *src, int *outsize, int *insize, int chs)
{
	short pcm;
	int rsize, wsize;
	int cpysize;

	// 局部变量初始化
	rsize = *insize;
	wsize = *outsize;

	// 检查是否为单声道数据
	if(chs == 1)
	{
		while((rsize>0) && (wsize>0))
		{
			pcm = *src++;
			*dst++ = pcm;
			*dst++ = pcm;
			wsize -= 2*sizeof(short);
			rsize -= sizeof(short);
		}
		*outsize -= wsize;
		*insize -= rsize;
		return *insize;
	}

	cpysize = (rsize > wsize) ? wsize : rsize;
	kmemcpy(dst, src, cpysize);
	*insize = cpysize;
	*outsize = cpysize;
	return cpysize;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacWrite(HANDLE hdac, short *src, int len)
{
	PDAC_DEVICE dac;
	AUDIO_FILTER Filter;
	PRESAMPLE presample;
	short *dst,*pcm_src;
	volatile BYTE *flag;
	int wsize, rsize,error_delay;
	int offset,fltr_in;
	int len_bak,pcm_in,wsola_in,start_flag;
	DWORD nWsolaSize;

	// 获取设备对象
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;

	presample = &dac->Resample;
	start_flag = dac->fThreadStart;

	len_bak = len;
	dst = NULL;
	offset = 0;
	wsize = 0;
	rsize = 0;
	pcm_src = src;

#if defined(DAC_SAVE_PCM)
	kmemcpy(&dac_source_buf[dac_source_offset],(BYTE*)src,len);
	dac_source_offset += len;
	if( dac_source_offset >= 2 * 1024 * 1024 )
		dac_source_offset = 0;
#endif

	while(len)
	{
		if( dac->WsolaBuf && dac->hWsola )
		{	//调节语速
			wsola_in = (len > FILTER_PCMBUF_SIZE) ? FILTER_PCMBUF_SIZE : len;
			Filter.iBuf = src;
			Filter.iSize = wsola_in;
			Filter.oBuf = dac->WsolaBuf;
			Filter.oSize = WAOLA_PCM_BUF_SIZE;
			WsolaConvert(dac->hWsola,&Filter);
			nWsolaSize = Filter.oSize;

			pcm_in = 0;
			if( nWsolaSize )
			{
				if( dac->Samprate != DAC_SAMPLE_RATIO )
				{
					// 重采样处理
					Filter.iBuf  = dac->WsolaBuf;
					Filter.iSize = nWsolaSize;
					Filter.oBuf  = dac->pSamplerate;
					Filter.oSize = (4*FILTER_PCMBUF_SIZE*DAC_SAMPLE_RATIO)/dac->Samprate+512;	//((DAC_SAMPLE_RATIO * fltr_in) / dac->Samprate) & 0xfffe;	//
					pcm_in = Resample(dac->hSample,&Filter);
					pcm_src = dac->pSamplerate;
				}
				else
				{
					pcm_in  = nWsolaSize;
					pcm_src = dac->WsolaBuf;
				}
			}

			len -= wsola_in;
			src += wsola_in/sizeof(short);
		}
		else if( dac->Samprate != DAC_SAMPLE_RATIO)
		{
			// 重采样处理
			fltr_in = (len > FILTER_PCMBUF_SIZE) ? FILTER_PCMBUF_SIZE : len;
			Filter.iBuf  = src;
			Filter.iSize = fltr_in;
			Filter.oBuf  = dac->pSamplerate;
			Filter.oSize = (4*FILTER_PCMBUF_SIZE*DAC_SAMPLE_RATIO)/dac->Samprate+512;	//((DAC_SAMPLE_RATIO * fltr_in) / dac->Samprate) & 0xfffe;	//
			pcm_in = Resample(dac->hSample,&Filter);
			pcm_src = dac->pSamplerate;
			len -= fltr_in;
			src += fltr_in/sizeof(short);
		}
		else
		{
			pcm_src = src;
			pcm_in = len;
			len = 0;
		}

		// 把经过Filter后的数据写入PCM缓冲区
		offset = presample->WriteOffset;
		while(pcm_in)
		{
			// 获取写缓冲区指针
			dst = presample->Buf[presample->WriteBuf] + offset / sizeof(short);
			flag = &presample->BufFlag[presample->WriteBuf];

			// 获取可写字节数据
			if(offset != 0)
				wsize = (DAC_PCMBUF_SIZE - offset);
			else
				wsize = DAC_PCMBUF_SIZE;
			rsize = pcm_in;

			// 等待当前缓冲区可写
			error_delay = 0;
			while((*flag != DAC_BUF_WRITE) )
			{
				sTimerSleep(10, NULL);
				error_delay++;
				if( error_delay >= 1000 )
				{
					kdebug(mod_audio, PRINT_ERROR, "have't action for dma 10S\n");
					GetDmaInfo();
					SetMuteMode(0);
					if( error_delay > 1005 )
						return 0;
				}
			}

			// 写入数据到BUF
			DacPcmQueue(dst, pcm_src, &wsize, &rsize, dac->Channel);

			// 准备循环条件
			pcm_in -= rsize;
			pcm_src += rsize/sizeof(short);
			offset += wsize;
			if(offset == DAC_PCMBUF_SIZE)
			{
				kMutexWait(hDacMutex);
				offset = 0;
				if(++presample->WriteBuf == MAX_PCMBUFS)
					presample->WriteBuf = 0;
				dac->fThreadStart = 1;
				*flag = DAC_BUF_READ;
				kMutexRelease(hDacMutex);
			}
			presample->WriteOffset = offset;
		}	
	}


	if( start_flag == 0 && dac->fThreadStart == 1 )
	{
		kprintf("dac write sleep\n");
		sTimerSleep(20, NULL);
	}
	return len_bak;
}


////////////////////////////////////////////////////
// 功能: 写入空数据，补齐缓冲区指针
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacWaitWriteEnd(PRESAMPLE presample)
{
	int i;
	int delay;

	kdebug(mod_audio, PRINT_INFO, "DacWaitWriteEnd: start\n");
	// 无论是否单路或者多路声音播放, 都需要等待缓冲中的数据播放完毕.
	/*
	if( GetDacChannel() != 1 )
		return;
		*/

	delay = 0;
	for(;;)
	{
		for(i=0; i<MAX_PCMBUFS; i++)
		{
			if(presample->BufFlag[i] != DAC_BUF_WRITE)
				break;
		}
		if(i == MAX_PCMBUFS)
		{
			break;
		}
		sTimerSleep(40,0);

		delay++;
		if( delay >= 12 )
			break;
	}

	kdebug(mod_audio, PRINT_INFO, "DacWaitWriteEnd: middle\n");
	if( GetDacChannel() == 1 )
	{
		DacWaiteDmaEnd();
		kdebug(mod_audio, PRINT_INFO, "DacWaitWriteEnd: end\n");
	}
}

////////////////////////////////////////////////////
// 功能: 写入空数据，补齐缓冲区指针
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void DacWaiteDma(HANDLE hdac)
{
	DWORD count,delay;
	PDAC_DEVICE dac;

	// 获取设备对象
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return;

	delay = 0;
	while(1)
	{
		delay++;
		count = GetDmaCount();
		if( count >= 200 || delay >= 50000 )
			break;
	}
}

////////////////////////////////////////////////////
// 功能: 写入空数据，补齐缓冲区指针
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacWriteEnd(HANDLE hdac, int terminate)
{
	PDAC_DEVICE dac;
	PRESAMPLE presample;
	short *dst;
	volatile BYTE *flag;
	int offset;

	// 获取设备对象
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;
	if( terminate )
		return 0;

	// 获取写缓冲区指针
	presample = &dac->Resample;

	offset = presample->WriteOffset;
	if( dac->Samprate != DAC_SAMPLE_RATIO )
	{
		AUDIO_FILTER Filter;
		int size;
		int wsize, rsize;
		int error_delay;
		short *src;
		Filter.iBuf  = NULL;
		Filter.iSize = 0;
		Filter.oBuf  = dac->pSamplerate;
		Filter.oSize = (4*FILTER_PCMBUF_SIZE*DAC_SAMPLE_RATIO)/dac->Samprate+512;
		size = Resample(dac->hSample,&Filter);
		kdebug(mod_audio, PRINT_INFO, "last resample size:%d\n", size);
		src = dac->pSamplerate;

		while(size)
		{
			wsize = (DAC_PCMBUF_SIZE - offset);
			rsize = size;
			dst = presample->Buf[presample->WriteBuf] + offset / sizeof(short);
			flag = &presample->BufFlag[presample->WriteBuf];

			error_delay = 0;
			while((*flag != DAC_BUF_WRITE) )
			{
				sTimerSleep(10, NULL);
				error_delay++;
				if( error_delay >= 1000 )
				{
					kdebug(mod_audio, PRINT_ERROR, "have't action for dma 10S\n");
					GetDmaInfo();
					SetMuteMode(0);
					if( error_delay > 1005 )
						return 0;
				}
			}

			DacPcmQueue(dst, src, &wsize, &rsize, dac->Channel);

			size -= rsize;
			src += rsize / sizeof(short);
			offset += wsize;

			if(offset == DAC_PCMBUF_SIZE)
			{
				// 设置变量
				kMutexWait(hDacMutex);
				if(++presample->WriteBuf == MAX_PCMBUFS)
					presample->WriteBuf = 0;
				*flag = DAC_BUF_READ;
				kMutexRelease(hDacMutex);
				offset = 0;
			}
			presample->WriteOffset = offset;
		}
	}

	if(offset != 0)
	{
		dst = presample->Buf[presample->WriteBuf] + offset / sizeof(short);
		flag = &presample->BufFlag[presample->WriteBuf];
		// 清除缓冲区内容	
		kmemset(dst, 0x00, DAC_PCMBUF_SIZE-offset);

		// 设置变量
		kMutexWait(hDacMutex);
		if(++presample->WriteBuf == MAX_PCMBUFS)
			presample->WriteBuf = 0;
		*flag = DAC_BUF_READ;
		presample->WriteOffset = 0;
		kMutexRelease(hDacMutex);
	}

	// 用于声音合成时的强制同步, 表示这个声音的缓冲数量不会再增加了.
	dac->fThreadStart = 2;

	// 等待缓冲区中的PCM输出完毕
	DacWaitWriteEnd(presample);
	
	return DAC_PCMBUF_SIZE-offset;
}

////////////////////////////////////////////////////
// 功能:	设置发音速度
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacSetTempo(HANDLE hdac, int tempo)
{
	PDAC_DEVICE dac;
	int buf_size;

	// 输入参数检查
	dac = (PDAC_DEVICE)hdac;
	if((dac == NULL) || (tempo > 10) || (tempo < -5))
		return -1;

	// 检查是否申请
	kMutexWait(hDacMutex);
	dac->Tempo = tempo;	

	//删除wsola
	DacDestoryWsola(hdac);

	if(!tempo)
	{
		kMutexRelease(hDacMutex);
		return 0;	
	}

	// 申请用于Wsola的缓冲区
	buf_size = WAOLA_PCM_BUF_SIZE;
	dac->WsolaBuf = (short*)kmalloc(buf_size * sizeof(short));
	if(dac->WsolaBuf == NULL)
	{
		kMutexRelease(hDacMutex);
		return -1;
	}
	dac->hWsola = WsolaCreate(dac->Samprate,dac->Channel,0,dac->Tempo);
	if(dac->hWsola == NULL)
	{
		kfree(dac->WsolaBuf);
		dac->WsolaBuf = NULL;
		kMutexRelease(hDacMutex);
		return -1;
	}
	kMutexRelease(hDacMutex);
	return 0;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacSetVolume(HANDLE hdac, int volume)
{
	PDAC_DEVICE dac;
	int ret;

	// 输入参数检查
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;
	if(volume >= 100)
		volume = 99;
	else if(volume < 0)
		volume = 0;

	// 设置音量参数	
	dac = (PDAC_DEVICE)hdac;
	ret = dac->Volume;
	dac->Volume = volume;
	return ret;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacGetVolume(HANDLE hdac)
{
	PDAC_DEVICE dac;

	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;
	return dac->Volume;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacSetStatus(HANDLE hdac, int pause)
{
	int ret;
	PDAC_DEVICE dac;
	ret = fPause;
	fPause = pause;

	dac = (PDAC_DEVICE)hdac;
	if(dac != NULL)
		dac->Pause = pause;

	return ret;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacGetStatus(HANDLE hdac, int pause)
{
	return fPause;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacSetGloalVolume(int vol, int speak)
{
	int ret;

	if(vol < 0)
		vol = 0;
	if(vol > 99)
		vol  = 99;
	ret = gVolumeSpeaker;
	gVolumeSpeaker = (short)vol;
	return ret;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacGetGloalVolume()
{
	return (int)gVolumeSpeaker;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacIsHeadphoneIn(void)
{
#if defined(CONFIG_ERAPHONE_IO)
	if(__gpio_get_pin(CONFIG_ERAPHONE_IO) == CONFIG_ERAPHONE_INERT)
		return 1;
	return 0;
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
int DacGetOwners(void)
{
	PLIST head;
	PLIST n;
	int owners;

	// 获取打开设备数量
	kMutexWait(hDacMutex);
	head = &DacList;
	owners = 0;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
		owners++;
	kMutexRelease(hDacMutex);

	return owners;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int DacSetSamplerate(HANDLE hdac, int samprate, int chs)
{
	PDAC_DEVICE dac;
	int buf_size;

	// 输入参数检查
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;

	if((samprate != 8000) && (samprate != 11025) && 
			(samprate != 16000) && (samprate != 22050) && 
			(samprate != 24000) && (samprate != 32000) && 
			(samprate != 44100) && (samprate != 48000))
		return -1;

	if((chs != 1) && (chs != 2))
		return -1;

	dac->hSample = ResampleCreate(chs,samprate,DAC_SAMPLE_RATIO,0);
	if( dac->hSample == NULL )
		return -1;

	dac->Samprate = samprate;
	dac->Channel = chs;
	dac->SamplerateSize = 0;
	buf_size = (4*FILTER_PCMBUF_SIZE*DAC_SAMPLE_RATIO)/samprate+512;
	if( samprate != DAC_SAMPLE_RATIO)
	{
		dac->pSamplerate = kmalloc( buf_size );
		if( dac->pSamplerate == NULL )
			return -1;
		dac->SamplerateSize = buf_size;
	}
	else
		dac->pSamplerate = NULL;

	return 1;
}

////////////////////////////////////////////////////
// 功能:
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void DacDestorySamplerate(HANDLE hdac)
{
	PDAC_DEVICE dac;

	// 输入参数检查
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return;

	if( dac->pSamplerate != NULL )
		kfree( dac->pSamplerate );

	ResampleDestroy(dac->hSample);
}

////////////////////////////////////////////////////
// 功能:
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void DacDestoryWsola(HANDLE hdac)
{
	PDAC_DEVICE dac;

	// 输入参数检查
	dac = (PDAC_DEVICE)hdac;
	if( dac == NULL )
		return ;

	if(dac->hWsola)
	{
		WsolaDestroy(dac->hWsola);
		dac->hWsola = NULL;
	}

	if(dac->WsolaBuf)
	{
		kfree(dac->WsolaBuf);
		dac->WsolaBuf = NULL;
	}
}

////////////////////////////////////////////////////
// 功能: 检查当前有几个声音通道
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int GetDacChannel(void)
{
	PLIST head, n;
	int chs;
	// 获取采样数据到临时缓冲区
	kMutexWait(hDacMutex);
	head = &DacList;
	chs = 0;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
	{
		if(++chs == 4)
			break;		// 最多合成4通道
	}
	kMutexRelease(hDacMutex);
	return chs;
}

////////////////////////////////////////////////////
// 功能: 得到BUF里剩余数据的时间
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int GetDacBufCount()
{
	int i;
	int len;
	PLIST head, n;
	PDAC_DEVICE dac;
	PRESAMPLE presample;

	len = 0;
	presample = &DacDevice;
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
	{
		if( presample->BufFlag[i] == DAC_BUF_READ )
			len +=DAC_PCMBUF_SIZE;
		else if( presample->BufFlag[i] == DAC_BUF_READING )
			len += GetDmaCount();
	}

	head = &DacList;
	if( head )
	{
		n=ListFirst(head);
		dac = ListEntry(n, DAC_DEVICE, Link);
		if( dac )
		{
			presample = &dac->Resample;
			for( i = 0 ; i < MAX_PCMBUFS ; i++ )
			{
				if( presample->BufFlag[i] == DAC_BUF_READ )
					len +=DAC_PCMBUF_SIZE;
			}
		}
	}

	return ( (len*1000) / (DAC_SAMPLE_RATIO*2*2) );
}

////////////////////////////////////////////////////
// 功能: 得到BUF里剩余数据的时间
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int GetDacBufCountMs()
{
	int i;
	int len;
	PRESAMPLE presample;

	len = 0;
	presample = &DacDevice;
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
	{
		if( presample->BufFlag[i] == DAC_BUF_READ )
			len +=DAC_PCMBUF_SIZE;
	}

	return ( (len*1000) / (DAC_SAMPLE_RATIO*2*2) );
}

////////////////////////////////////////////////////
// 功能: 等待DMA结束
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void DacWaiteDmaEnd()
{
	int i,time;
	PRESAMPLE presample;

	if( ListEmpty(&DacList) )
		return;

	kdebug(mod_audio, PRINT_INFO, "DacWaiteDmaEnd start\n");
	presample = &DacDevice;
	time = 0;
	while(1)
	{

		for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		{
			if( presample->BufFlag[i] != DAC_BUF_WRITE )
				break;
		}
		if( i == MAX_PCMBUFS )
			break;

		sTimerSleep(10,NULL);
		time++;
		if( time > 40 )
			break;
		if( GetDacChannel() != 1 )
			break;
	}

	kdebug(mod_audio, PRINT_INFO, "DacWaiteDmaEnd end\n");
	return;
}

////////////////////////////////////////////////////
// 功能: 测试程序
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void DacClearPcmData()
{
	PRESAMPLE presample;

	//判断是否存在播放设备
	if( ListEmpty(&DacList) )
	{
		kdebug(mod_audio, PRINT_WARNING, "dac list is null\n");
		return;
	}


	switch( fBeginDma )
	{
		case INIT_DMA_TRANS:	//初始化阶段，数据处于不确定状态，放静音
		case PAUSE_DMA_TRANS:	//暂停状态，放静音
		case STOP_DMA_TRANS:	//停止状态，放静音
			StartDmaPcmTrans(1,0);
			break;
		case END_DMA_TRANS:		//播放完成，停止触发新的DMA
			break;
		case RUNING_DMA_TRANS:
			//声音正常播放，DMA数据正常播放
			presample = &DacDevice;
			//		PrintfDacStatus(presample);
			switch(presample->BufFlag[presample->ReadBuf])
			{
				case DAC_BUF_WRITE:
					//当前读取BUF，没有数据，直接放静音
					StartDmaPcmTrans(1,0);
					break;
				case DAC_BUF_READ:
					//当前数据BUF，有数据，开始播放数据，同时把BUF属性定义成READING
					StartDmaPcmTrans(0,(unsigned int)presample->Buf[presample->ReadBuf]);
					presample->BufFlag[presample->ReadBuf] = DAC_BUF_READING;
					break;
				case DAC_BUF_READING:
					presample->BufFlag[presample->ReadBuf] = DAC_BUF_WRITE;
					if(++presample->ReadBuf == MAX_PCMBUFS )
						presample->ReadBuf = 0;

					if( presample->BufFlag[presample->ReadBuf] == DAC_BUF_READ )
					{
						StartDmaPcmTrans(0,(unsigned int)presample->Buf[presample->ReadBuf]);
						presample->BufFlag[presample->ReadBuf] = DAC_BUF_READING;
					}
					else
						StartDmaPcmTrans(1,0);
					break;
			}
			break;
	}
}
