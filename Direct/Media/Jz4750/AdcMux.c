//[PROPERTY]===========================================[PROPERTY]
//			*****  ŵ�����۲���ϵͳV2  *****
//		--------------------------------------
//					 
//		--------------------------------------
//				 ��Ȩ: ��ŵ���ۿƼ�
//			 ---------------------------
//				  ��   ��   ��   ʷ
//		--------------------------------------
//  �汾	��ǰ		˵��		
//  V0.1	2008-11	  Init.			 Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]


#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/device.h>
#include <kernel/timer.h>
#include <kernel/sched.h>
#include <kernel/callback.h>


#define ADC_PCMBUF_SIZE			1024 * 2
#define MAX_PCMBUFS				16

#define ADC_BUF_READ			0
#define ADC_BUF_READING			1
#define ADC_BUF_WRITE			2
#define ADC_BUF_WRITEING		3

#define ADC_RECORD_INIT			0
#define ADC_RECORD_PAUSE		1
#define ADC_RECORD_STOP			2
#define ADC_RECORD_PLAY			3
#define ADC_RECORD_END			4

#define QCOEF 28
#define Q_PREP_COEF (1<<QCOEF)
#define Q16 (1<<16) 
 
typedef struct
{
	int ly2;
	int ly1;
	int lx0;
	int lx1;

	int ry2;
	int ry1;
	int rx0;
	int rx1;
}PRE_PROCESS;
typedef PRE_PROCESS *PPRE_PROCESS;
 

typedef struct _ADC_DEVICE
{
	LIST Link;
	int Samprate;
	int Channel;
	int Volume;
	int Pause;
	int bFilter;

	PRE_PROCESS PreProcess;

	int dummy;
	short Buf[MAX_PCMBUFS][ADC_PCMBUF_SIZE/sizeof(short)];
	BYTE BufFlag[MAX_PCMBUFS];
	BYTE ReadBuf;
	BYTE WriteBuf;
	int ReadOffset;
}ADC_DEVICE;

typedef ADC_DEVICE *PADC_DEVICE;
static HANDLE hAdcMutex;
static HANDLE hAdcSema;
static int record_status;
static LIST AdcList;
static ADC_DEVICE AdcDevice __attribute__((aligned(32)));
static short NullBuf[ADC_PCMBUF_SIZE/sizeof(short)];
static DWORD nRecordLoop;

extern void CloseMediaDevice(char channel);			//�ر�DAC/ADC
extern void MediaDraveInit(char channel);
extern void OpenMediaDevice(unsigned long sample,unsigned long channle , char mode);	//��DAC/ADC
extern void DmaDataToAic(int arg,unsigned int sour_addr, unsigned int len,unsigned char mode);	//DMA���ݵ�AIC
extern void DestroyMediaDevoice(char mode);
extern void SetMoseCe(char io);
extern void SetPowerAmplifier(char io);
extern void __dcache_inv(unsigned long addr, unsigned long size);
static void StartDmaPcmTrans(int mute, unsigned int sour);

extern DWORD ClockGetPllValue(void);

#define     ADC2_INTERFACE_EN   (1 << 0)
//ANALOG_CONTROL_REG1
#define     PD_REF              (1 << 0)
#define     PULL_VCM            (1 << 23)
#define     PD_VCM              (1 << 24)

//CLK_DIV_REG2
#define     ADC1_DIV            2
#define     ADC2_DIV            4
#define     ADC1_CLK_EN         (1 << 3)
#define     ADC2_CLK_EN         (1 << 12)
#define     ADC1_RST            (1 << 22)
#define     ADC2_RST            (1 << 23)
#define     ADC2_GATE		    (1 << 25)

//#define     ADC1_PD             (1 << 29)

//ADC2 MODE CONFIGURE REGISTER
#define WORD_LENGTH_MASK (0xF << 8)
#define I2S_EN			(1 << 4)
#define CH_POLARITY_SEL (1 << 3)
#define HOST_RD_INT_EN	(1 << 2)
#define L2_EN			(1 << 1)
#define ADC2_CTRL_EN	(1 <<0)

//ANALOG_CONTROL_REG1
#define     PD_ADC2             (1 << 1)
#define     PD_ADC3             (1 << 2)
#define     LINE_IN             (2 << 3)
#define     MIC_IN              (4 << 3)
#define     NO_INPUT            (7 << 3)
#define     LIN_GAIN_L1         (0 << 6)
#define     LIN_GAIN_L2         (1 << 6)
#define     LIN_GAIN_L3         (2 << 6)
#define     LIN_GAIN_L4         (3 << 6)
#define     LIN_GAIN_CLR        (3 << 6)
#define     MIC_GAIN_L0         (0 << 8)
#define     MIC_GAIN_L1         (1 << 8)
#define     MIC_GAIN_L2         (2 << 8)
#define     MIC_GAIN_L3         (3 << 8)
#define     MIC_GAIN_CLR        (3 << 8)
#define     PD_MIC              (1 << 10)
#define     PD_LINE_L_R         (3 << 11)
#define     BYPASS_LINE	        (1 << 23)
#define     VCM3_CTRL	        (1 << 22)
#define     ADC2_LIM            (1 << 25)



//ANALOG_CONTROL_REG2
#define     ADC1_INTERFACE_EN   (1 << 8)
#define     BAT_EN              (1 << 9)
#define     KEY_EN              (1 << 10)
#define     ADC_OSR             12
#define     OSR_MODE            (1 << 12)
#ifdef SUPPORT_SDCARD
#define     START_WAIT          (0x1f << 0)
#define     KEY_THRESHOLD       (0x20 << 17)
#else
#define     START_WAIT          (0x3f << 0)
#define     KEY_THRESHOLD       (0x20 << 17)
#endif
//ADC2_INTERFACE_CTRL_REG
#define     ADC_WORD_LAN        (0xf << 7)

//#define SAVE_ADC_FILE
#ifdef SAVE_ADC_FILE
static DWORD nAdcBufIdx;
static BYTE  AdcBuf[5*1024*1024];
#endif

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
void AdcPreProcessInit(PRE_PROCESS *s)
{
	s->ly2 = 0;
	s->ly1 = 0;
	s->lx0 = 0;
	s->lx1 = 0;

	s->ry2 = 0;
	s->ry1 = 0;
	s->rx0 = 0;
	s->rx1 = 0;
}
 
////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int AdcPreProcess(PRE_PROCESS *st,  short *signal, short lg)
{
    short i, x2;
    int L_tmp;
 
	int a1 = (int)(1.906005859*Q_PREP_COEF+0.5);
	int a2 = (int)(-0.911376953*Q_PREP_COEF+0.5);
	int b0 = (int)(0.4636230465*2*Q_PREP_COEF+0.5);
	int b1 = (int)(-0.92724705*2*Q_PREP_COEF+0.5);
    int b2 = (int)(0.4636234515*2*Q_PREP_COEF+0.5);
    for (i = 0; i < lg; i+=2) 
	{
		x2 = st->lx1;
		st->lx1 = st->lx0;
		st->lx0 = signal[i];
		L_tmp = ((unsigned long long)st->ly1 * a1) >>(16 + QCOEF - 16) ;
		L_tmp += ((unsigned long long)st->ly2 * a2) >>(16 + QCOEF - 16) ;
		L_tmp += ((unsigned long long)st->lx0 * b0) >>(0 + QCOEF - 16);
		L_tmp += ((unsigned long long)st->lx1 * b1) >>(0 + QCOEF - 16);
		L_tmp += ((unsigned long long)x2 * b2) >>(0 + QCOEF - 16);
		signal[i] = L_tmp >> 16;
		st->ly2 = st->ly1;
		st->ly1 = L_tmp;
    }

    for (i = 1; i < lg; i+=2) 
	{
		x2 = st->rx1;
		st->rx1 = st->rx0;
		st->rx0 = signal[i];
		L_tmp = ((unsigned long long)st->ry1 * a1) >>(16 + QCOEF - 16) ;
		L_tmp += ((unsigned long long)st->ry2 * a2) >>(16 + QCOEF - 16) ;
		L_tmp += ((unsigned long long)st->rx0 * b0) >>(0 + QCOEF - 16);
		L_tmp += ((unsigned long long)st->rx1 * b1) >>(0 + QCOEF - 16);
		L_tmp += ((unsigned long long)x2 * b2) >>(0 + QCOEF - 16);
		signal[i] = L_tmp >> 16;
		st->ry2 = st->ry1;
		st->ry1 = L_tmp;
    }
    return 0;
}
 

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static int AdcDebias(short *signal, short lg)
{
	int i;
	int offset=0;
	
	for(i=0; i<lg; i+=2)
		offset += signal[i];
	offset /= (lg>>1) ;
	for(i=0; i<lg; i+=2)
		signal[i] -= offset; 

	for(i=1; i<lg; i+=2)
		offset += signal[i];
	offset /= (lg>>1);
	for(i=1; i<lg; i+=2)
		signal[i] -= offset; 
    return 0;
	AdcDebias(signal,lg);
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void AdcDeviceOpen(DWORD samplerate, int chs)
{
	MediaDraveInit(0);
	kdebug(mod_audio, PRINT_INFO, "samplerate = %d, chs = %d\n",samplerate,chs);
	OpenMediaDevice(samplerate,chs,1);

	//��ʼ����ǰ¼��״̬
	record_status = ADC_RECORD_INIT;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void AdcDeviceClose(void)
{
	//��ʼ����ǰ¼��״̬
	record_status = ADC_RECORD_END;

	//�ر���������
	CloseMediaDevice(0);

	//ɾ��¼���ж�������
	DestroyMediaDevoice(1);
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
static void AdcMicrophoneOpen(int gain)
{
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcInit(void)
{
	int i;

	// ȫ�ֱ�����ʼ��
	hAdcMutex = kMutexCreate();
	hAdcSema = kSemaCreate(0);
	ListInit(&AdcList);
	
	//��ʼ���ṹ������
	kmemset(&AdcDevice,0,sizeof(ADC_DEVICE));

	//��ʼ������
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		AdcDevice.BufFlag[i] = ADC_BUF_WRITE;

	//��ʼ������������GPIO�˿�
	SetMoseCe(0);
	SetPowerAmplifier(0);
	return 0;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
HANDLE AdcOpen(DWORD samprate, DWORD chs)
{
	int i;
	PADC_DEVICE adc;

	if((samprate != 8000) && (samprate != 11025) && 
			(samprate != 16000) && (samprate != 22050) && 
			(samprate != 24000) &&
			(samprate != 32000) && (samprate != 44100))
	{
		return NULL;
	}
	if((chs != 1) && (chs != 2))
		return NULL;

	adc = kmalloc(sizeof(ADC_DEVICE));
	if(adc == NULL)
		return NULL;

	// ��ʼ���豸����
	kMutexWait(hAdcMutex);
	kmemset(adc, 0x00, sizeof(ADC_DEVICE));
	adc->Pause = 1;
	adc->Channel = chs;
	adc->Samprate = samprate;
	adc->Volume = 99;
	adc->ReadOffset = 0;
	adc->ReadBuf = 0;
	adc->WriteBuf = 0;
	nRecordLoop = 0;
#ifdef SAVE_ADC_FILE
	nAdcBufIdx = 0;
#endif
	for( i = 0 ; i < MAX_PCMBUFS ; i++ )
		adc->BufFlag[i] = ADC_BUF_READ;
	
	adc->bFilter = 0;
	AdcPreProcessInit(&adc->PreProcess);
	ListInit(&adc->Link);

	// ��鵱ǰ�豸�Ƿ��Ѿ���
	if(ListEmpty(&AdcList))
	{
		// ��DA�豸
		AdcDeviceOpen(samprate, chs);
		
		// �򿪶����豸
		AdcMicrophoneOpen(1);

		// �ͷ��豸���ź�
		kSemaRelease(hAdcSema);
	}
	ListInsert(&AdcList, &adc->Link);
	kMutexRelease(hAdcMutex);
	return(HANDLE)adc;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcClose(HANDLE hadc)
{
	PADC_DEVICE adc;

	kMutexWait(hAdcMutex);
	adc =(PADC_DEVICE)hadc;
	if(adc)
	{
		ListRemove(&adc->Link);
		if(ListEmpty(&AdcList))
		{
			// �ر�DA�豸
			AdcDeviceClose();
		}
#ifdef SAVE_ADC_FILE
		{
			HANDLE fp;
			fp = kfopen("d:\\record.bin","w+b");
			kfwrite(AdcBuf,1,nAdcBufIdx,fp);
			kfclose(fp);

		}
#endif
		kfree(adc);
		kMutexRelease(hAdcMutex);
		return 0;
	}
	kMutexRelease(hAdcMutex);
	return -1;
}
////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcRead(HANDLE hadc, short *dst, int len)
{
	PADC_DEVICE adc,pAdcDevice;
	short *src;
	volatile BYTE *flag;
	int rsize;
	int offset;
	int len_bak;
	short volume;
	
	// ��ȡ�豸����
	adc =(PADC_DEVICE)hadc;
	if(adc == NULL)
		return -1;
	
	// ���ݵ���˫����������ʵ�ʶ�ȡ���ݳ���
	len_bak = len;
	if(adc->Channel == 1)
		len *= 2;
	offset = adc->ReadOffset;
	volume = adc->Volume * 30;
	
	if( record_status == ADC_RECORD_INIT )
		StartDmaPcmTrans(1,0);		//����DMA��ȡ����

	record_status = ADC_RECORD_PLAY;
	pAdcDevice = &AdcDevice;
	while(len > 0)
	{
		// ��ȡд������ָ��
		src = pAdcDevice->Buf[pAdcDevice->ReadBuf] + offset / sizeof(short);
		flag = &pAdcDevice->BufFlag[pAdcDevice->ReadBuf];

		// ��ȡ��д�ֽ�����
		rsize =(ADC_PCMBUF_SIZE - offset);
		rsize = (rsize > len) ? len : rsize;

		// �ȴ���ǰ��������д
		while(*flag != ADC_BUF_READ)
			sTimerSleep(10, NULL);

		//�����˲�����
//		AdcDebias(src, ADC_PCMBUF_SIZE/sizeof(short));		//��������FADEʱ�������ȥ����ƫ�ƴ�����
		if(pAdcDevice->bFilter)
			AdcPreProcess(&pAdcDevice->PreProcess, src, ADC_PCMBUF_SIZE/sizeof(short));

		// д�����ݵ�Cache������������ת������������
		if(adc->Channel == 2)
		{
			int i;
			int tmp;
			
			i = rsize / sizeof(short);
			while(i--)
			{
				tmp = ((*src++) * volume) / 1024;
				if(tmp >= 32767)
					tmp = 32767;
				else if(tmp <= -32767)
					tmp = -32767;
				*dst++ = (short)tmp;
			}
		}
		else
		{
			int i;
			int tmp;
			
			i = rsize / (sizeof(short) * 2);
			while(i--)
			{
				tmp = ((*src++) * volume) / 1024;
				if(tmp >= 32767)
					tmp = 32767;
				else if(tmp <= -32767)
					tmp = -32767;
				*dst++ = tmp;
				src++;
			}
		}

		// ׼��ѭ������
		len -= rsize;
		offset += rsize;
		if(offset >= ADC_PCMBUF_SIZE)
		{
			offset = 0;
			if(++pAdcDevice->ReadBuf == MAX_PCMBUFS )
				pAdcDevice->ReadBuf = 0;
			*flag = ADC_BUF_WRITE;
		}
		adc->ReadOffset = offset;
	}
	return len_bak;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcSetVolume(HANDLE hadc, int volume)
{
	PADC_DEVICE adc;
	int ret;
	
	// ����������
	adc =(PADC_DEVICE)hadc;
	if(adc == NULL)
		return -1;
	if(volume >= 100)
		volume = 99;
	else if(volume < 0)
		volume = 0;
		
	// ������������	
	adc =(PADC_DEVICE)hadc;
	ret = adc->Volume;
	adc->Volume = volume;
	return ret;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcGetVolume(HANDLE hadc)
{
	PADC_DEVICE adc;

	adc =(PADC_DEVICE)hadc;
	if(adc == NULL)
		return -1;
	return adc->Volume;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcSetStatus(HANDLE hadc, int pause)
{
	PADC_DEVICE adc;
	int ret;
	
	// ����������
	adc =(PADC_DEVICE)hadc;
	if(adc == NULL)
		return -1;
		
	// ������������	
	ret = adc->Pause;
	if(pause)
		adc->Pause = 1;
	else
		adc->Pause = 0;
	return ret;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcGetStatus(HANDLE hadc, int pause)
{
	PADC_DEVICE adc;
	
	adc =(PADC_DEVICE)hadc;
	if(adc == NULL)
		return -1;
	return adc->Pause;
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
int AdcSetEq(HANDLE hadc, int filter)
{
	PADC_DEVICE adc;
	int ret;
	
	// ����������
	adc =(PADC_DEVICE)hadc;
	if(adc == NULL)
		return -1;
		
	// ������������	
	ret = adc->bFilter;
	if(!filter)
		adc->bFilter = 1;
	else
		adc->bFilter = 0;
	return ret;
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
	//�ж��Ƿ��ܴ�������
	if(!mute)
	{	//����PCM����
//		kprintf("get record data len = %d\n",ADC_PCMBUF_SIZE);
		DmaDataToAic(1,(unsigned int)sour, ADC_PCMBUF_SIZE,1);
	}
	else
	{
		kdebug(mod_audio, PRINT_WARNING, "lost record data\n");
		DmaDataToAic(1,(unsigned int)NullBuf, ADC_PCMBUF_SIZE,1);
	}
}

////////////////////////////////////////////////////
// ����: 
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
void AdcSetPcmData(void)
{
	PADC_DEVICE pAdcDevice;
	short *pTemp;

	//�ж��Ƿ���ڲ����豸
	if( ListEmpty(&AdcList) )
		return;

	switch( record_status )
	{
	case ADC_RECORD_INIT:
	case ADC_RECORD_PAUSE:
	case ADC_RECORD_STOP:
		StartDmaPcmTrans(1,0);
		break;
	case ADC_RECORD_END:
		break;
	case ADC_RECORD_PLAY:
		pAdcDevice = &AdcDevice;
		switch( pAdcDevice->BufFlag[pAdcDevice->WriteBuf])
		{
		case ADC_BUF_READ:
		case ADC_BUF_READING:
			//��ǰBUFΪ��BUF������д������
			StartDmaPcmTrans(1,0);
			break;
		case ADC_BUF_WRITE:
			pTemp = pAdcDevice->Buf[pAdcDevice->WriteBuf];
			//__dcache_inv((unsigned long)pTemp,ADC_PCMBUF_SIZE);

#ifdef SAVE_ADC_FILE
			if( pTemp[0] == pTemp[1] && pTemp[0] == -1 )
			{
				kprintf("======== begin of adc have zero: ======== \n");
				kprintf("pAdcDevice->WriteBuf = %d , nRecordLoop = %d\n",pAdcDevice->WriteBuf,nRecordLoop);
			}
			else if( pTemp[(ADC_PCMBUF_SIZE / 2) - 2] ==  pTemp[(ADC_PCMBUF_SIZE / 2) - 1] && pTemp[(ADC_PCMBUF_SIZE / 2) - 2] == -1 )
			{
				kprintf("======== end fo adc have zero: ========\n");
				kprintf("pAdcDevice->WriteBuf = %d , nRecordLoop = %d\n",pAdcDevice->WriteBuf,nRecordLoop);
			}
			else if( pTemp[(ADC_PCMBUF_SIZE / 2) - 2] ==  pTemp[(ADC_PCMBUF_SIZE / 2) - 1] && pTemp[(ADC_PCMBUF_SIZE / 2) - 2] == 0x5a5a )
			{
				kprintf("======== end fo adc have 0x5a5a: ========\n");
				kprintf("pAdcDevice->WriteBuf = %d , nRecordLoop = %d\n",pAdcDevice->WriteBuf,nRecordLoop);
			}

			kmemcpy(&AdcBuf[nAdcBufIdx],(BYTE*)pTemp,ADC_PCMBUF_SIZE);
			nAdcBufIdx += ADC_PCMBUF_SIZE;
			if( nAdcBufIdx >= 4 * 1024 * 1024 )
				nAdcBufIdx = 4 * 1024 * 1024;
#endif

			//BUFд����������õ�ǰBUF�ɶ���ͬʱд��һ��BUF
			pAdcDevice->BufFlag[pAdcDevice->WriteBuf] = ADC_BUF_READ;
			if( ++pAdcDevice->WriteBuf == MAX_PCMBUFS )
			{
				pAdcDevice->WriteBuf = 0;
				nRecordLoop++;
			}

			//����һ��BUFд������
			if( pAdcDevice->BufFlag[pAdcDevice->WriteBuf] == ADC_BUF_WRITE )
			{
				StartDmaPcmTrans(0,(unsigned int)pAdcDevice->Buf[pAdcDevice->WriteBuf]);
//				pAdcDevice->BufFlag[pAdcDevice->WriteBuf] = ADC_BUF_WRITEING;        
			}
			else
				StartDmaPcmTrans(1,0);
			break;
		}
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////
// ����: ADC���Գ���
// ����: 
// ���:
// ����: 
// ˵��: 
////////////////////////////////////////////////////
// #define FILE_BLOCK_SIZE ADC_PCMBUF_SIZE * 4
// void test_adc_max()
// {
// 	char *pBuf;
// 	HANDLE fp;
// 	HANDLE hmux;
// 	DWORD file_len,num;
// 
// 	kprintf("begin adc test\n");
// 	fp = kfopen("D:\\record.bin","w+b");
// 	if( fp <= 0 )
// 		return;
// 
// 	pBuf = (char*)kmalloc(FILE_BLOCK_SIZE);
// 	
// 	hmux = AdcOpen(16000,2);
// 	kprintf("adc open hmux = %x\n",hmux);
// 	
// 	file_len = FILE_BLOCK_SIZE * 128;
// 	while(file_len > 0)
// 	{
// 		num = AdcRead(hmux,(short*)pBuf,FILE_BLOCK_SIZE );
// 		kfwrite(pBuf,1,FILE_BLOCK_SIZE,fp);
// 		file_len -= num;
// 	}
// 
// 	AdcClose(hmux);
// 	kfclose(fp);
// 	kprintf("record finish\n");
// }
