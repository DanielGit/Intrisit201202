//[PROPERTY]===========================================[PROPERTY]
//            *****  ŵ�����۲���ϵͳV2  *****
//        --------------------------------------
//   	             
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
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
static short gVolumeSpeaker;	// ȫ����������
static char bHeadphoneIn;
static char fBeginDma;
static int  fPause;
static RESAMPLE DacDevice __attribute__((aligned(32)));		//���DMA������
static short NullBuf[DAC_PCMBUF_SIZE/sizeof(short)];
static DWORD nErrorData = 0;	//����Խ������ݸ���

extern void SetMuteMode(char mode);
extern void MediaDraveInit(char channel);
extern void OpenMediaDevice(unsigned long sample,unsigned long channle , char mode);	//��DAC/ADC
extern void CloseMediaDevice(char channel);			//�ر�DAC/ADC
extern void DmaDataToAic(int arg,unsigned int sour_addr, unsigned int len,unsigned char mode);	//DMA���ݵ�AIC
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
// ����: �趨��Ƶ����豸
// ����: 
// ���:
// ����: 
// ˵��: �����Ƿ�������
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
// ����: �趨��Ƶ����豸
// ����: 
// ���:
// ����: 
// ˵��: �����Ƿ�������
////////////////////////////////////////////////////
static int DacOutDeviceSet(void)
{
#if defined(CONFIG_ERAPHONE_IO)	
#if defined(CONFIG_AMP2_IO)	
	/////////////////////////////////////	
	// �ƻ��͵���������������
	/////////////////////////////////////

	// 1. ����Ƿ���Ƶ���룬�������������
#if defined(CONFIG_TVOUT_ENABLE)	
	if(LcdcIsTvoutOn())
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		BootGpioOut(CONFIG_AMP2_IO, CONFIG_AMP2_DIS);
		return 0;
	}
#endif

	// 2. ����Ƿ��������
	if(!BootGpioIn(CONFIG_ERAPHONE_IO))
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		BootGpioOut(CONFIG_AMP2_IO, CONFIG_AMP2_DIS);
		return 1;
	}

	// 3. ����Ƿ��ƻ������ڵ������
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
	// ֻ���ƻ�������������
	/////////////////////////////////////

	// 1. ����Ƿ���Ƶ���룬�������������
#if defined(CONFIG_TVOUT_ENABLE)	
	if(LcdcIsTvoutOn())
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		return 0;
	}
#endif

	// 2. ����Ƿ��������
	if(!BootGpioIn(CONFIG_ERAPHONE_IO))
	{
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		return 1;
	}

	// 3. �������
	BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_ENA);
	return 0;
#endif
#else
	return 0;	
#endif
}

////////////////////////////////////////////////////
// ����: ��ȡ���Ż�����ָ��
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: �����ݴ�ŵ�DMA������
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int SavePcmData( short* pcm )
{
	PRESAMPLE pResample;

	pResample = &DacDevice;

	if( pResample->BufFlag[pResample->WriteBuf] == DAC_BUF_WRITE )
	{	//����д������,������д��DMA����
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
// ����: ��DMA�е���������I2S
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void StartDmaPcmTrans(int mute, unsigned int sour)
{
	PRESAMPLE pResample;
	static DWORD mute_data;

	pResample = &DacDevice;

	//�ж��Ƿ��ܴ�������
	if(!mute)
	{	
		//����PCM����
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
		//���Ϳյ�����
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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

		// �ȴ�DAC�豸����
		if(!bopen)
		{
			kSemaWait(hDacSema, 0);
			bopen = 1;
			pcm = (short*)kmalloc(DAC_PCMBUF_SIZE);
		}

		// ���DAC�Ƿ��Ѿ��ر�
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

		// ��黺�����Ƿ��ȡ
		if(pcm == NULL)
		{
			kMutexRelease(hDacMutex);
			sTimerSleep(1000, NULL);
			continue;
		}

		// ��ȡ�������ݵ���ʱ������
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
				break;		// ���ϳ�4ͨ��
		}

		// ����Ƿ������Ч����
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
			// ǿ��ͬ���ϳ�����. ע��:
			// 1. ��·����, ��������ʱ���Ῠ, ���Ǻϳɲ��ſ�, ��������Ϊ����·����s
			// �������ݵ� ���� ��ͬ��ɵ�. ������һ·�ܿ�, ����һ·����,
			// �ͻ��� ���� ���һ·���ϳ�����, ���� ���� ����һ·������.
			// 2. ���������ǿ�ƿ��һ·�ȴ�����һ·.
			// 3. ����������ͣ��������ֹͣ���������������, ����Ҫͬ��.
			for(i = 0; i < chs; i++)
			{
				//kprintf("src[%d]:0x%08x, Pause:%d, fThreadStart:%d, vol[%d]:%d\n", i, src[i], dac[i]->Pause, dac[i]->fThreadStart, i, vol[i]);
				// ������, ����ͣ, �Ѿ���ʼ�ϳ�, ����������ֹͣ�Ĺ�����
				if( !src[i] && !dac[i]->Pause && dac[i]->fThreadStart == 1 )
				{
					break;
				}
			}

			// ��ʾ��ĳ·����û������, ���ǲ���ʾ��·�����Ῠ.
			if(i != chs)
			{
				int buf_cnt = 0;
				PRESAMPLE presample;

				// ��DAC��������л��ж��ٸ� ������ �Ļ���.
				presample = &DacDevice;
				for( j = 0 ; j < MAX_PCMBUFS ; j++ )
				{
					if( presample->BufFlag[j] == DAC_BUF_READ )
						buf_cnt++;
				}

				// ���DAC������, ��Ϊ �п��� �����������������, ���Ժϳ���
				// ���ݵ�����. ������û�����ݵ���·����һ���Ῠ,
				// ��ʹ��·�������� �Ѿ�����(������) �����ݲ����ǲ��Ῠ��.
				// DAC�����, ��ô˯��, �ȴ�û�����ݵ���·����.
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

		// �ϳɶ�ͨ����Ƶ����
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
		else	// ����ͨ����Ƶ����
		{
			short volume;

			volume = vol[0];
			for(i=0; i<DAC_PCMBUF_SIZE/sizeof(short); i++)
				pcm[i] = (short)((src[0][i] * volume) / 1024);
		}

		DacOutDeviceSet();

		// д����Ƶ���ݵ�DMA������
		if(SavePcmData(pcm))
		{
			// ���û�������ȡ���
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void DacDeviceOpen(void)
{
	int i;

	MediaDraveInit(1);

	// ���������ʼ��������������, ��pop noiseû��ϵ
	OpenMediaDevice(DAC_SAMPLE_RATIO,2,0);

	//��ʼ����ǰDMA״̬ΪINIT״̬
	fBeginDma = INIT_DMA_TRANS;

	//��ʼ���ṹ������
	kmemset(&DacDevice,0,sizeof(RESAMPLE));

	//��ʼ������
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		DacDevice.BufFlag[i] = DAC_BUF_WRITE;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void DacDeviceClose(void)
{
	//��ʼ����ǰDMA״̬ΪEND״̬
	fBeginDma = END_DMA_TRANS;

	//�ر���������
	CloseMediaDevice(1);

	//ɾ�������ж�������
	DestroyMediaDevoice(0);
}


////////////////////////////////////////////////////
// ����: �򿪶���
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void DacHeadphoneOpen(void)
{
	return;
}


////////////////////////////////////////////////////
// ����: �رն���
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void DacHeadphoneClose(void)
{
	return;
}


////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacInit(void)
{
	int i;
	// ȫ�ֱ�����ʼ��
	hDacMutex = kMutexCreate();
	hDacSema = kSemaCreate(0);
	ListInit(&DacList);
	gVolumeSpeaker = 99;
	bHeadphoneIn = 0;
#ifdef DAC_SAVE_PCM
	dac_offset = 0;
	dac_source_offset = 0;
#endif

	//��ʼ���ṹ������
	kmemset(&DacDevice,0,sizeof(RESAMPLE));
	kmemset((BYTE*)NullBuf,0xff,DAC_PCMBUF_SIZE);

	//��ʼ������
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		DacDevice.BufFlag[i] = DAC_BUF_WRITE;

	// ����DAC�豸д�߳�
	KernelThread(PRIO_USER-11, DacWriteThread, 0, 0);


#if defined(CONFIG_ERAPHONE_IO)
	__gpio_as_input(CONFIG_ERAPHONE_IO);
	__gpio_enable_pull(CONFIG_ERAPHONE_IO);
#endif	

	//��ʼ������������GPIO�˿�
	SetMoseCe(0);

	//D�๦�ſ���ȥ�����������⴦��
#if defined(CONFIG_MAC_BDS6100) || defined(CONFIG_MAC_ND800) || defined(CONFIG_MAC_ASKMI1388) || defined(CONFIG_MAC_BDS6100A) || defined(CONFIG_MAC_NP7000) || defined( CONFIG_MAC_NP2300 )
	SetPowerAmplifier(1);
	SetPowerAmplifier(0);
#else
	SetPowerAmplifier(0);
#endif
	return 0;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
HANDLE DacOpen(void)
{
	PDAC_DEVICE dac;
	PLIST head;
	PLIST n;
	int open_devs;
	int i;

	// ��ȡ���豸����
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
	// ��ʼ���豸����
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
	// ��鵱ǰ�豸�Ƿ��Ѿ���
	if(ListEmpty(&DacList))
	{
		// �����򿪹���, mos�ܺͿ�dac�Ĺ���Ӧ����ListEmpty()==true�������.
		// ���ǵ�����·����, ��������һ·����ͣ״̬, ��Ϊ��ͣ��ʱ��ر��˹��ź�dac,
		// ������Ҫ�ٴδ򿪹��ź�dac, ���Ǹĳɲ����ж�����ڴ�, ���ڶ�������ϳ�,
		// �൱�ڶ��һ��dac�͹���, Ӧ��û��Ӱ��.
#if defined(CONFIG_MAC_NP7000) || defined(CONFIG_MAC_NP2300)
		MillinsecoundDelay(250);

		SetMoseCe(1);
#else
		SetPowerAmplifier(1);
#endif
		// ��DA�豸
		DacDeviceOpen();

		// �򿪶����豸
		DacHeadphoneOpen();

		//����DMA���ݴ��ͣ�ͬʱֻ���;�������
		StartDmaPcmTrans(1,0);

		// �ͷ��豸���ź�
		kSemaRelease(hDacSema);

		ListInsert(&DacList, &dac->Link);

		//ֻ�е�һ�δ������豸����Ҫȥ��BOBO��
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
		//��ֹһ��������ͣʱ������һ�������޷����ŵ�����
		SetMuteMode(1);
		ListInsert(&DacList, &dac->Link);
	}


	kMutexRelease(hDacMutex);
	return (HANDLE)dac;
}


////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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

			//�رչ���
			SetPowerAmplifier(0);

			MillinsecoundDelay(20);

			SetMoseCe(0);

			// �رն����豸
			DacHeadphoneClose();		

			// �ر�DA�豸
			DacDeviceClose();

			// û������������б�����.
			MillinsecoundDelay(20);
		}

		DacDestorySamplerate(dac);	//�ͷ�resample����
		DacDestoryWsola(hdac);		//�ͷ�wsola����
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int DacPcmQueue(short *dst, short *src, int *outsize, int *insize, int chs)
{
	short pcm;
	int rsize, wsize;
	int cpysize;

	// �ֲ�������ʼ��
	rsize = *insize;
	wsize = *outsize;

	// ����Ƿ�Ϊ����������
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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

	// ��ȡ�豸����
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
		{	//��������
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
					// �ز�������
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
			// �ز�������
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

		// �Ѿ���Filter�������д��PCM������
		offset = presample->WriteOffset;
		while(pcm_in)
		{
			// ��ȡд������ָ��
			dst = presample->Buf[presample->WriteBuf] + offset / sizeof(short);
			flag = &presample->BufFlag[presample->WriteBuf];

			// ��ȡ��д�ֽ�����
			if(offset != 0)
				wsize = (DAC_PCMBUF_SIZE - offset);
			else
				wsize = DAC_PCMBUF_SIZE;
			rsize = pcm_in;

			// �ȴ���ǰ��������д
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

			// д�����ݵ�BUF
			DacPcmQueue(dst, pcm_src, &wsize, &rsize, dac->Channel);

			// ׼��ѭ������
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
// ����: д������ݣ����뻺����ָ��
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void DacWaitWriteEnd(PRESAMPLE presample)
{
	int i;
	int delay;

	kdebug(mod_audio, PRINT_INFO, "DacWaitWriteEnd: start\n");
	// �����Ƿ�·���߶�·��������, ����Ҫ�ȴ������е����ݲ������.
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
// ����: д������ݣ����뻺����ָ��
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
void DacWaiteDma(HANDLE hdac)
{
	DWORD count,delay;
	PDAC_DEVICE dac;

	// ��ȡ�豸����
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
// ����: д������ݣ����뻺����ָ��
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacWriteEnd(HANDLE hdac, int terminate)
{
	PDAC_DEVICE dac;
	PRESAMPLE presample;
	short *dst;
	volatile BYTE *flag;
	int offset;

	// ��ȡ�豸����
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;
	if( terminate )
		return 0;

	// ��ȡд������ָ��
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
				// ���ñ���
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
		// �������������	
		kmemset(dst, 0x00, DAC_PCMBUF_SIZE-offset);

		// ���ñ���
		kMutexWait(hDacMutex);
		if(++presample->WriteBuf == MAX_PCMBUFS)
			presample->WriteBuf = 0;
		*flag = DAC_BUF_READ;
		presample->WriteOffset = 0;
		kMutexRelease(hDacMutex);
	}

	// ���������ϳ�ʱ��ǿ��ͬ��, ��ʾ��������Ļ�������������������.
	dac->fThreadStart = 2;

	// �ȴ��������е�PCM������
	DacWaitWriteEnd(presample);
	
	return DAC_PCMBUF_SIZE-offset;
}

////////////////////////////////////////////////////
// ����:	���÷����ٶ�
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacSetTempo(HANDLE hdac, int tempo)
{
	PDAC_DEVICE dac;
	int buf_size;

	// ����������
	dac = (PDAC_DEVICE)hdac;
	if((dac == NULL) || (tempo > 10) || (tempo < -5))
		return -1;

	// ����Ƿ�����
	kMutexWait(hDacMutex);
	dac->Tempo = tempo;	

	//ɾ��wsola
	DacDestoryWsola(hdac);

	if(!tempo)
	{
		kMutexRelease(hDacMutex);
		return 0;	
	}

	// ��������Wsola�Ļ�����
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacSetVolume(HANDLE hdac, int volume)
{
	PDAC_DEVICE dac;
	int ret;

	// ����������
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return -1;
	if(volume >= 100)
		volume = 99;
	else if(volume < 0)
		volume = 0;

	// ������������	
	dac = (PDAC_DEVICE)hdac;
	ret = dac->Volume;
	dac->Volume = volume;
	return ret;
}


////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacGetStatus(HANDLE hdac, int pause)
{
	return fPause;
}


////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacGetGloalVolume()
{
	return (int)gVolumeSpeaker;
}


////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacGetOwners(void)
{
	PLIST head;
	PLIST n;
	int owners;

	// ��ȡ���豸����
	kMutexWait(hDacMutex);
	head = &DacList;
	owners = 0;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
		owners++;
	kMutexRelease(hDacMutex);

	return owners;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int DacSetSamplerate(HANDLE hdac, int samprate, int chs)
{
	PDAC_DEVICE dac;
	int buf_size;

	// ����������
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
// ����:
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
void DacDestorySamplerate(HANDLE hdac)
{
	PDAC_DEVICE dac;

	// ����������
	dac = (PDAC_DEVICE)hdac;
	if(dac == NULL)
		return;

	if( dac->pSamplerate != NULL )
		kfree( dac->pSamplerate );

	ResampleDestroy(dac->hSample);
}

////////////////////////////////////////////////////
// ����:
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
void DacDestoryWsola(HANDLE hdac)
{
	PDAC_DEVICE dac;

	// ����������
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
// ����: ��鵱ǰ�м�������ͨ��
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int GetDacChannel(void)
{
	PLIST head, n;
	int chs;
	// ��ȡ�������ݵ���ʱ������
	kMutexWait(hDacMutex);
	head = &DacList;
	chs = 0;
	for(n=ListFirst(head); n!=head; n=ListNext(n))
	{
		if(++chs == 4)
			break;		// ���ϳ�4ͨ��
	}
	kMutexRelease(hDacMutex);
	return chs;
}

////////////////////////////////////////////////////
// ����: �õ�BUF��ʣ�����ݵ�ʱ��
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: �õ�BUF��ʣ�����ݵ�ʱ��
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: �ȴ�DMA����
// ����: 
// ���:
// ����: 
// ˵��: 
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
// ����: ���Գ���
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
void DacClearPcmData()
{
	PRESAMPLE presample;

	//�ж��Ƿ���ڲ����豸
	if( ListEmpty(&DacList) )
	{
		kdebug(mod_audio, PRINT_WARNING, "dac list is null\n");
		return;
	}


	switch( fBeginDma )
	{
		case INIT_DMA_TRANS:	//��ʼ���׶Σ����ݴ��ڲ�ȷ��״̬���ž���
		case PAUSE_DMA_TRANS:	//��ͣ״̬���ž���
		case STOP_DMA_TRANS:	//ֹͣ״̬���ž���
			StartDmaPcmTrans(1,0);
			break;
		case END_DMA_TRANS:		//������ɣ�ֹͣ�����µ�DMA
			break;
		case RUNING_DMA_TRANS:
			//�����������ţ�DMA������������
			presample = &DacDevice;
			//		PrintfDacStatus(presample);
			switch(presample->BufFlag[presample->ReadBuf])
			{
				case DAC_BUF_WRITE:
					//��ǰ��ȡBUF��û�����ݣ�ֱ�ӷž���
					StartDmaPcmTrans(1,0);
					break;
				case DAC_BUF_READ:
					//��ǰ����BUF�������ݣ���ʼ�������ݣ�ͬʱ��BUF���Զ����READING
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
