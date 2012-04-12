//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典模拟平台  *****
//        --------------------------------------
//    	           模拟平台按键处理部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/thread.h>
#include <kernel/device.h>
#include <kernel/karch.h>
#include <direct/media.h>
#include <platform/platform.h>

//设置声音播放的DMA通道
#if defined(CONFIG_MCU_JZ4750)
#define PLAYBACK_CHANNEL		11
#define RECORD_CHANNEL			11
#elif defined(CONFIG_MCU_JZ4750L)
#define PLAYBACK_CHANNEL		3
#define RECORD_CHANNEL			3
#elif defined(CONFIG_MCU_JZ4755)
#define PLAYBACK_CHANNEL		9
#define RECORD_CHANNEL			9
#else
#define PLAYBACK_CHANNEL		5
#define RECORD_CHANNEL			5
#endif


//#define KPRINTF_DEF

#define RGADW_ADDR_BIT		( 8 )
#define RGADW_DATA_BIT		( 0 )
#define AIC_RGADW_RGWR			(1 << 16)

#define AICFR_TFTH(x)			( ( (x)/2 ) << 8 )		// 2, 4, ..., 32

#define PHYADDR(n)			(unsigned int)((n) & 0x1fffffff)
#define PHYSADDR(x)			(((unsigned int)x) & 0x1fffffff)

#if defined(CONFIG_MCU_JZ4750L) || defined(CONFIG_MCU_JZ4755) || defined(CONFIG_MCU_JZ4750)
#define INREG8(x)           ( (unsigned char)(*(volatile unsigned char * const)(x)) )
#define OUTREG8(x, y)       *(volatile unsigned char * const)(x) = (y)
#define SETREG8(x, y)       OUTREG8(x, INREG8(x)|(y))
#define CLRREG8(x, y)       OUTREG8(x, INREG8(x)&~(y))

#define INREG16(x)           ( (unsigned short)(*(volatile unsigned short * const)(x)) )
#define OUTREG16(x, y)       *(volatile unsigned short * const)(x) = (y)
#define SETREG16(x, y)      OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)      OUTREG16(x, INREG16(x)&~(y))

#define INREG32(x)           ( (unsigned int)(*(volatile unsigned int* const)(x)) )
#define OUTREG32(x, y)       *(volatile unsigned int * const)(x) = (y)
#define SETREG32(x, y)      OUTREG32(x, INREG32(x)|(y))
#define CLRREG32(x, y)      OUTREG32(x, INREG32(x)&~(y))
#endif

#define PCM_DATA_FINISH		1
#define PCM_SET_SAMPLE_RATE	2
#define PCM_SET_CHANNEL		3
#define PCM_SET_FORMAT		4
#define PCM_SET_MUTE		5
#define PCM_SET_VOL			6
#define PCM_SET_HP_VOL		7
#define PCM_SET_PAUSE		8
#define PCM_SET_PLAY		9
#define PCM_SET_REPLAY		10
#define PCM_SET_RECORD		11
#define PCM_SET_RECORD_FM	12
#define PCM_RESET			13
#define PCM_REINIT			14
#define PCM_GET_HP_VOL		15
#define PCM_GET_VOL			16
#define PCM_SET_VOLFUNCTION	17
#define PCM_GET_SAMPLE_MAX	18
#define PCM_POWER_OFF		19
#define PCM_POWER_ON		20

#define DEVICE_CLOSE_STATUS	0
#define DEVICE_OPEN_STATUS	1

extern void ClockDelay(DWORD usec);
extern void DacClearPcmData(void);
extern void AdcSetPcmData(void);

typedef struct _waveheader {
	DWORD  main_chunk;		/* 'RIFF' */
	DWORD  length;			/* filelen */
	DWORD  chunk_type;		/* 'WAVE' */

	DWORD  sub_chunk;		/* 'fmt ' */
	DWORD  sc_len;			/* length of sub_chunk, =16 */
	WORD   format;			/* should be 1 for PCM-code */
	WORD   modus;			/* 1 Mono, 2 Stereo */
	DWORD  sample_fq;		/* frequence of sample */
	DWORD  byte_p_sec;
	WORD   byte_p_spl;		/* samplesize; 1 or 2 bytes */
	WORD   bit_p_spl;		/* 8, 12 or 16 bit */ 

	DWORD  data_chunk;		/* 'data' */
	DWORD  data_length;		/* samplecount */
} WaveHeader;

//#define MAX_TEST_DATA 2*1024*1024
//static WORD test_buf[MAX_TEST_DATA];
extern DWORD TimerCount(void);
static HANDLE hPlayer = NULL;
static HANDLE hRecord = NULL;
static unsigned int	g_volume = 0;
static void dma_start(unsigned int channel, unsigned int srcAddr, unsigned int dstAddr, unsigned int count,unsigned char mode);
static unsigned char codec_reg_read(unsigned char addr);
extern void DmaDataToAic(int arg,unsigned int sour_addr, unsigned int len,unsigned char mode);
extern void EnableMediaGpio(void);
extern void dma_cache_wback_inv(unsigned long addr, unsigned long size);
extern void __dcache_inv(unsigned long addr, unsigned long size);
static DWORD interrupt_count;
static BYTE  fDeviceStatus = DEVICE_CLOSE_STATUS;
void GetDmaInfo();

extern void BootGpioSet(DWORD pin, int in);
extern int BootGpioIn(DWORD pin);
extern void BootGpioOut(DWORD pin, int data);
////////////////////////////////////////////////////
// 功能: 延迟N个豪秒
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void dma_init(unsigned int channel, char mode)
{
	unsigned int group, data;
#ifdef KPRINTF_DEF
	kprintf("dma init channle = %d\n",channel);
#endif
	group = channel / HALF_DMA_NUM;

	SETREG32(A_DMA_DMAC(group), (DMAC_DMA_EN | DMAC_FAST_AIC) );
	SETREG32(A_DMA_DCKE(group), (1 << (channel  - group * HALF_DMA_NUM)) );

	OUTREG32(A_DMA_DCS(channel), DCS_NDES );
	if (mode)
	{
		data = DCM_SAI | DCM_SP_32BIT | DCM_DP_16BIT | DCM_TSZ_16BYTE | DCM_RDIL_IGN | DCM_TRANS_INTR_EN;
		OUTREG32(A_DMA_DCM(channel), data);
		OUTREG32(A_DMA_DRT(channel), DRT_AIC_TX);
	}
	else
	{
		data = DCM_DAI | DCM_SP_16BIT | DCM_DP_32BIT | DCM_TSZ_16BYTE | DCM_RDIL_IGN;
		OUTREG32(A_DMA_DCM(channel), data);
		OUTREG32(A_DMA_DRT(channel), DRT_AIC_RX);
	}
}

////////////////////////////////////////////////////
// 功能: 延迟N个豪秒
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void dma_start(unsigned int channel, unsigned int srcAddr, unsigned int dstAddr, unsigned int count, unsigned char mode)
{
#ifdef KPRINTF_DEF
	kprintf("dma channle = %d\n",channel);
	kprintf("source = %x, destion = %x, count = %x\n",srcAddr,dstAddr,count*16);
#endif
	OUTREG32(A_DMA_DSA(channel), srcAddr);				// DMA数据地址
	OUTREG32(A_DMA_DTA(channel), dstAddr);				// DMA目标地址
	OUTREG32(A_DMA_DTC(channel), count / 16);			// 传送DMA的数据组数，当前设置1组为16个数据
	SETREG32(A_DMA_DCS(channel), DCS_CTE);				// 开始DMA数据传送

	//判断是否允许DMA中断
	if( mode )
		InterruptUnmask(IRQ_DMA_0 + channel, 0);		// 允许DMA结束后，自动产生中断
}

////////////////////////////////////////////////////
// 功能: 得到当前DMA传送的数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
DWORD GetDmaCount()
{
	return (REG_DMAC_DTCR(PLAYBACK_CHANNEL) & 0x0ffffff) * 16;
}

////////////////////////////////////////////////////
// 功能: 读取DMA状态
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void GetDmaInfo()
{
	unsigned int channel;
	channel = PLAYBACK_CHANNEL;
	for(channel= 0; channel < 4 ; channel++)
	{
		kprintf("DMA CHANNEL = %d\n",channel);
		kprintf("status  = %x, count = %x\n",INREG32(A_DMA_DCM(channel)),INREG32(A_DMA_DTC(channel)));
		kprintf("control = %x,irq = %x\n",INREG32(A_DMA_DCS(channel)),INREG32(A_DMA_DIRQP(channel/6)));
		kprintf("source addr = %x, destion addr = %x\n",INREG32(A_DMA_DSA(channel)),INREG32(A_DMA_DTA(channel)));
		kprintf("DRT = %x, DMAC = %x, DCKE = %x\n",INREG32(A_DMA_DRT(channel)),INREG32(A_DMA_DMAC(0)),INREG32(A_DMA_DCKE(0)));
	}
	kprintf("\nDMA interrupt count = %d\n",interrupt_count);
	kprintf("aic register = %x\n",INREG32(A_CPM_CLKGR));
	kprintf("dma register = %x\n\n",INREG32(INTC_IMR));

	kprintf("======== aic status ========\n");
	kprintf("AIC I2S/MSB-justified Control Register I2SCR = %x\n",REG_AIC_I2SCR);
	kprintf("AIC Controller FIFO Status Register AICSR = %x\n",REG_AIC_SR);
	kprintf("AIC AC-link Status Register ACSR = %x\n",REG_AIC_ACSR);
	kprintf("AIC I2S/MSB-justified Status Register I2SSR = %x\n\n",REG_AIC_I2SSR);

	kprintf("======== codec status ========\n");
	kprintf("Audio Interface Control, Software Write = %x\n",codec_reg_read(A_CODEC_AICR));
	kprintf("Control Register 1 = %x\n",codec_reg_read(A_CODEC_CR1));
	kprintf("Control Register 2 = %x\n",codec_reg_read(A_CODEC_CR2));
	kprintf("Control Clock Register 1 = %x\n",codec_reg_read(A_CODEC_CCR1));
	kprintf("Control Clock Register 2 = %x\n",codec_reg_read(A_CODEC_CCR2));
	kprintf("Power Mode Register 1 = %x\n",codec_reg_read(A_CODEC_PMR1));
	kprintf("Power Mode Register 2 = %x\n",codec_reg_read(A_CODEC_PMR2));
	kprintf("Control Ramp Register = %x\n",codec_reg_read(A_CODEC_CRR));
	kprintf("Interrupt Control Register = %x\n",codec_reg_read(A_CODEC_ICR));
	kprintf("Interrupt Flag Register = %x\n",codec_reg_read(A_CODEC_IFR));
	kprintf("Control Gain Register 1 = %x\n",codec_reg_read(A_CODEC_CGR1));
	kprintf("Control Gain Register 2 = %x\n",codec_reg_read(A_CODEC_CGR2));
	kprintf("Control Gain Register 3 = %x\n",codec_reg_read(A_CODEC_CGR3));
	kprintf("Control Gain Register 4 = %x\n",codec_reg_read(A_CODEC_CGR4));
	kprintf("Control Gain Register 5 = %x\n",codec_reg_read(A_CODEC_CGR5));
	kprintf("Control Gain Register 6 = %x\n",codec_reg_read(A_CODEC_CGR6));
	kprintf("Control Gain Register 7 = %x\n",codec_reg_read(A_CODEC_CGR7));
	kprintf("Control Gain Register 8 = %x\n",codec_reg_read(A_CODEC_CGR8));
	kprintf("Control Gain Register 9 = %x\n",codec_reg_read(A_CODEC_CGR9));
	kprintf("Control Gain Register 10 = %x\n",codec_reg_read(A_CODEC_CGR10));
}
////////////////////////////////////////////////////
// 功能: 延迟N个豪秒
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void MillinsecoundDelay(unsigned int msec)
{
	unsigned int i;

	for (i = 0; i < msec; i++)
		ClockDelay(1000);
}

////////////////////////////////////////////////////
// 功能: codec 寄存器读出
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static unsigned char codec_reg_read(unsigned char addr)
{
	AIC_RW_CODEC_START();
	OUTREG16(AIC_RGADW, (addr << RGADW_ADDR_BIT));

	return (INREG8(AIC_RGDAT));
}

////////////////////////////////////////////////////
// 功能: codec 寄存器写入
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void codec_reg_write(unsigned char addr, unsigned char data)
{
	unsigned int	temp;

	temp = (addr << RGADW_ADDR_BIT) | (data << RGADW_DATA_BIT);

	AIC_RW_CODEC_START();
	OUTREG32(AIC_RGADW, temp);
	OUTREG32(AIC_RGADW, (temp | AIC_RGADW_RGWR));

	AIC_RW_CODEC_STOP();
}

////////////////////////////////////////////////////
// 功能: codec 寄存器设置
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void codec_reg_set(unsigned char addr, unsigned char data)
{
	unsigned int	temp = addr << RGADW_ADDR_BIT;

	AIC_RW_CODEC_START();
	OUTREG16(AIC_RGADW, temp);
	temp |= (INREG8(AIC_RGDAT) | data);
	OUTREG32(AIC_RGADW, temp);
	OUTREG32(AIC_RGADW, (temp | AIC_RGADW_RGWR));

	AIC_RW_CODEC_STOP();
}

////////////////////////////////////////////////////
// 功能: codec 寄存器清除
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void codec_reg_clear(unsigned char addr, unsigned char data)
{
	unsigned int	temp = addr << RGADW_ADDR_BIT;

	AIC_RW_CODEC_START();
	OUTREG16(AIC_RGADW, temp);
	temp |= (INREG8(AIC_RGDAT) & (~data));
	OUTREG32(AIC_RGADW, temp);
	OUTREG32(AIC_RGADW, (temp | AIC_RGADW_RGWR));

	AIC_RW_CODEC_STOP();
}

////////////////////////////////////////////////////
// 功能: 利用DMA播放数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void set_volume_reg(unsigned long arg)
{
	if (arg == 0)
	{
		codec_reg_write(A_CODEC_CGR8, GOL(31));
		codec_reg_write(A_CODEC_CGR9, GOR(31));
		codec_reg_write(A_CODEC_CGR1, GODR(15) | GODL(15));
		codec_reg_set(A_CODEC_CR1, DAC_MUTE);
		return;
	}

	codec_reg_clear(A_CODEC_CR1, DAC_MUTE);

	if (arg >= 8)
	{
		codec_reg_write(A_CODEC_CGR1, GODR(0) | GODL(0));
		codec_reg_write(A_CODEC_CGR8, GOL(31 - (arg - 8)));
		codec_reg_write(A_CODEC_CGR9, GOR(31 - (arg - 8)));
	}
	else
	{
		codec_reg_write(A_CODEC_CGR1, GODR(15 - arg * 2) | GODL(15 - arg * 2));
		codec_reg_write(A_CODEC_CGR8, GOL(31));
		codec_reg_write(A_CODEC_CGR9, GOR(31));
	}
}

////////////////////////////////////////////////////
// 功能: 利用DMA播放数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static unsigned char get_volume_reg(void)
{
	unsigned char cgr1 = codec_reg_read(A_CODEC_CGR1) & 0x0F;
	unsigned char cgr8 = codec_reg_read(A_CODEC_CGR8) & 0x1F;

	return (31 - cgr8 + ((15 - cgr1) >> 1));
}

////////////////////////////////////////////////////
// 功能: 利用DMA播放数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int intr_handler_playback_dma(int arg)
{
	int group,channle;
	unsigned int stat,dmac;

	//清除DMA标记
#ifdef KPRINTF_DEF
	kprintf("play music interrupt happend\n");
#endif
	channle = arg - IRQ_DMA_0;
	group   = (arg - IRQ_DMA_0) / HALF_DMA_NUM;

	__dmac_channel_ack_irq( group , channle - group * HALF_DMA_NUM );

	stat = INREG32(A_DMA_DCS(channle));
	dmac = INREG32(A_DMA_DMAC(group));

#ifdef KPRINTF_DEF
	if( stat & DCS_AR )
		kprintf("dcs addr error\n");
	if( stat & DCS_HLT)
		kprintf("dcs halt error\n");
	if( dmac & DMAC_ADDR_ERR )
		kprintf("dmac addr error\n");
	if( dmac & DMAC_HALT )
		kprintf("dmac halt error\n");
#endif

	if (stat & DCS_TT)
	{
		CLRREG32(A_DMA_DMAC(group), (DMAC_ADDR_ERR | DMAC_HALT) );
		CLRREG32(A_DMA_DCS(channle), (DCS_AR | DCS_HLT | DCS_CTE | DCS_CT | DCS_TT));
		DacClearPcmData();		//清空PCM数据
		interrupt_count++;
	}
	else
		kdebug(mod_media, PRINT_INFO, "stat & DCS_TT == 0\n");

	return INT_DONE;
}

////////////////////////////////////////////////////
// 功能: 音频初始化
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static int intr_handler_record_dma(int arg)
{
	unsigned int channle = arg - IRQ_DMA_0;
	unsigned int group = arg / HALF_DMA_NUM;

#ifdef KPRINTF_DEF
	kprintf("recorde interrupt happend\n");
#endif
	CLRREG32(A_DMA_DMAC(group),  (DMAC_ADDR_ERR | DMAC_HALT) );
	CLRREG32(A_DMA_DCS(channle), (DCS_CTE | DCS_CT | DCS_TT | DCS_AR | DCS_HLT));

	AdcSetPcmData();		//读出PCM数据
	return INT_DONE;
}

////////////////////////////////////////////////////
// 功能: 音频初始化
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static void jz_audio_reset(void)
{
	CLRREG32(AIC_CR, AIC_CR_ERPL | AIC_CR_EREC);
	SETREG32(AIC_CR, AIC_CR_FLUSH_FIFO);
	OUTREG32(AIC_SR, 0x00000000);

}

////////////////////////////////////////////////////
// 功能: 音频初始化
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
static inline void set_sample_rate(unsigned int rate)
{
	unsigned int val;

	if (rate == 44100)
		val = 2;
	else if (rate == 48000)
		val = 1;
	else if (rate == 32000)
		val = 3;
	else if (rate == 22050)
		val = 5;
	else if (rate == 11025)
		val = 8;
	else if (rate == 8000)
		val = 10;
	else if (rate == 96000)
		val = 0;
	else if (rate == 24000)
		val = 4;
	else if (rate == 16000)
		val = 6;
	else if (rate == 12000)
		val = 7;
	else if (rate == 9600)
		val = 9;
	else
	{
#ifdef KPRINTF_DEF
		kprintf("Invalid Sample Rate[%d], set to 8000\n", rate);
#endif
		val = 10;
	}	
	codec_reg_clear(A_CODEC_CCR2, DFREQ(val) | AFREQ(val));
	codec_reg_write(A_CODEC_CCR2, DFREQ(val) | AFREQ(val));
}

////////////////////////////////////////////////////
// 功能: 音频初始化
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
int pcm_ioctl(unsigned int cmd, unsigned long arg)
{
	unsigned int data, temp, aicfr, aiccr;
	switch (cmd)
	{
		case PCM_SET_SAMPLE_RATE:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_SAMPLE_RATE,arg = %d\n", arg);
#endif
			set_sample_rate(arg);
			break;

		case PCM_SET_CHANNEL:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_CHANNEL,arg = %d.\n", arg);
#endif
			if (arg == 2)
				__aic_disable_mono2stereo();		//CLRREG32(AIC_CR, AICCR_M2S);
			else if (arg == 1)
				__aic_enable_mono2stereo();			//SETREG32(AIC_CR, AICCR_M2S);
			break;
			
		case PCM_SET_FORMAT:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_FORMAT, arg = %d.\n",arg);
#endif
			// Config for Playback
			aicfr = INREG32(AIC_FR) & (~AIC_FR_TFTH_MASK);
			aiccr = INREG32(AIC_CR) & (~AIC_CR_OSS_MASK);
			temp = INREG32(A_DMA_DCM(PLAYBACK_CHANNEL)) & (~DCM_DP_MASK);
			if (arg == 16)
			{
				aicfr |= AICFR_TFTH(16);
				temp |= DCM_DP_16BIT;
				aiccr &= ~AIC_CR_AVSTSU;
				aiccr |= AIC_CR_OSS_16BIT;
			}
			else if (arg == 8)
			{
				aicfr |= AICFR_TFTH(16);
				temp |= DCM_DP_08BIT;
				aiccr |= AIC_CR_AVSTSU;
				aiccr |= AIC_CR_OSS_8BIT;
			}
			OUTREG32(AIC_FR, aicfr);
			OUTREG32(AIC_CR, aiccr);
			OUTREG32(A_DMA_DCM(PLAYBACK_CHANNEL), temp);
			
			// Config for Recorder
			temp = INREG32(A_DMA_DCM(RECORD_CHANNEL)) & (~DCM_DP_MASK);
			if (arg == 16)
				temp |= DCM_DP_16BIT;
			else if (arg == 8)
				temp |= DCM_DP_08BIT;

			OUTREG32(A_DMA_DCM(RECORD_CHANNEL), temp);
			break;
			
		case PCM_SET_MUTE:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_MUTE, arg = %d.\n", arg);
#endif
			if ((g_volume == 0) || arg == 1)
			{
				codec_reg_set(A_CODEC_CR1, DAC_MUTE);
			}
			else
			{
				codec_reg_clear(A_CODEC_CR1, DAC_MUTE);
			}
			break;
			
		case PCM_SET_VOL:
		case PCM_SET_HP_VOL:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_HP_VOL, val = %d.\n", arg);
#endif
			{
				int RegVolume = 0, VolDif = 0, JumpCoun = 0;
				data = arg > 31 ? 31 : arg;
				g_volume = data;
				RegVolume = get_volume_reg();
				VolDif = RegVolume > data ? (RegVolume - data) : (data - RegVolume);
#ifdef KPRINTF_DEF
				kprintf("set volume: voldif = %d,data = %d\n",VolDif,data);
#endif

				if (VolDif > 7)
				{
					for (JumpCoun = 0; JumpCoun <=  data; JumpCoun++)
					{
						set_volume_reg(JumpCoun);
						//MillinsecoundDelay(10);
					}
				}
				else
				{
					set_volume_reg(data);
				}
			}
			break;
			
		case PCM_SET_PAUSE:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_PAUSE\n");
#endif
			MillinsecoundDelay(10);

			CLRREG32(AIC_CR, AIC_CR_ERPL);
			CLRREG32(A_DMA_DCS(PLAYBACK_CHANNEL), DCS_AR | DCS_HLT | DCS_CTE | DCS_CT);
			SETREG32(AIC_CR, AIC_CR_FLUSH_FIFO);
			OUTREG32(AIC_SR, 0x00000000);
			break;

		case PCM_SET_PLAY:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_PLAY\n");
#endif
			codec_reg_clear(A_CODEC_PMR1, SB_ADC);
			jz_audio_reset();
			MillinsecoundDelay(10);
			codec_reg_clear(A_CODEC_CR1, HP_DIS);
			codec_reg_set(A_CODEC_PMR1, SB_LIN);

			codec_reg_clear(A_CODEC_PMR1, SB_DAC);
			codec_reg_set(A_CODEC_PMR1, SB_ADC);

			SETREG32(AIC_CR, AIC_CR_ERPL);
			break;

		case PCM_SET_REPLAY:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_REPLAY\n");
#endif
			CLRREG32(AIC_CR, AIC_CR_EREC);
			CLRREG32(A_DMA_DCS(RECORD_CHANNEL), DCS_AR | DCS_HLT | DCS_CTE | DCS_CT);
			codec_reg_clear(A_CODEC_CR2, ADC_HPF);
			codec_reg_set(A_CODEC_CR1, SB_MICBIAS);
			codec_reg_clear(A_CODEC_CR1, BYPASS);
			codec_reg_set(A_CODEC_CR1, DACSEL);
			codec_reg_write(A_CODEC_CR3, 0xC0);
			codec_reg_clear(A_CODEC_PMR2, GIM);

			pcm_ioctl(PCM_SET_PLAY, 0);
			break;

		case PCM_SET_RECORD:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_RECORD\n");
#endif
			jz_audio_reset();
			codec_reg_clear(A_CODEC_CR1, SB_MICBIAS);
			codec_reg_write(A_CODEC_CR3, 0x40);
			codec_reg_set(A_CODEC_PMR2, GIM);
			codec_reg_set(A_CODEC_PMR1, SB_LIN);
			codec_reg_set(A_CODEC_CR2, ADC_HPF);
			codec_reg_clear(A_CODEC_PMR1, SB_ADC);
			
			codec_reg_write(A_CODEC_CGR10, 0x88);
			break;

		case PCM_SET_RECORD_FM:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_SET_RECORD_FM\n");
#endif
			jz_audio_reset();
			codec_reg_clear(A_CODEC_CR1, DACSEL);
			codec_reg_set(A_CODEC_CR2, ADC_HPF);
			codec_reg_write(A_CODEC_CGR10, 0xff);
			break;

		case PCM_RESET:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_RESET\n");
#endif
			CLRREG32(A_DMA_DCS(PLAYBACK_CHANNEL), DCS_AR | DCS_HLT | DCS_CTE | DCS_CT);
			break;

		case PCM_GET_HP_VOL:
			return get_volume_reg();

		case PCM_GET_VOL:
			return get_volume_reg();

		case PCM_POWER_OFF:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_POWER_OFF\n");
#endif
			codec_reg_set(A_CODEC_CR1, DAC_MUTE);

			return 1;

		case PCM_POWER_ON:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl, PCM_POWER_ON\n");
#endif
			codec_reg_clear(A_CODEC_CR1, DAC_MUTE);

			return 1;

		default:
#ifdef KPRINTF_DEF
			kprintf("pcm_ioctl:Unsupported I/O command: %08x\n", cmd);
#endif
			return -1;
	}
	
	return 0;
}


////////////////////////////////////////////////////
// 功能: 打开或者关闭声音
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMuteMode(char mode)
{
	if( mode )
		codec_reg_clear(A_CODEC_CR1, DAC_MUTE);
	else
		codec_reg_set(A_CODEC_CR1, DAC_MUTE);
}


////////////////////////////////////////////////////
// 功能: 设置功放状态
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetPowerAmplifier(char io)
{
#if defined(CONFIG_ERAPHONE_IO)
	if( !BootGpioIn(CONFIG_ERAPHONE_IO) )
		io = 0;
#endif

#if defined(CONFIG_AMP_CE2_IO)
	//存在2个功放
	BootGpioSet(CONFIG_AMP_CE2_IO,0);
	BootGpioSet(CONFIG_AMP_IO,0);
	if( io )
	{	//打开功放
		if( BootGpioIn(CONFIG_PALM_DETECT_IO) == CONFIG_PALM_IN )
		{	//掌机插入状态,大功放开启，小功放关闭
			BootGpioOut(CONFIG_AMP_CE2_IO, CONFIG_AMP_CE2_ENA);
			BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
		}
		else
		{	//掌机拔出状态,大功放关闭，小功放开启
			BootGpioOut(CONFIG_AMP_CE2_IO, CONFIG_AMP_CE2_DIS);
			BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_ENA);
		}
	}
	else
	{	//关闭功放
		BootGpioOut(CONFIG_AMP_CE2_IO, CONFIG_AMP_CE2_DIS);
		BootGpioOut(CONFIG_AMP_IO, CONFIG_AMP_DIS);
	}
#else
	//只有一个功放
#if defined(CONFIG_AMP_IO)
	BootGpioSet(CONFIG_AMP_IO,0);
	if(io)
		BootGpioOut(CONFIG_AMP_IO,CONFIG_AMP_ENA);
	else
		BootGpioOut(CONFIG_AMP_IO,CONFIG_AMP_DIS);
#endif
#endif
}


////////////////////////////////////////////////////
// 功能: 设置mos管状态
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void SetMoseCe(char io)
{
#if defined(CONFIG_PIPO_IO)
	BootGpioSet(CONFIG_PIPO_IO,0);
	if(io)
		BootGpioOut(CONFIG_PIPO_IO,CONFIG_PIPO_ENA);
	else
		BootGpioOut(CONFIG_PIPO_IO,CONFIG_PIPO_DIS);
#endif
}

////////////////////////////////////////////////////
// 功能: 关闭codec设备
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void CloseMediaCodecDevice()
{
#ifdef CODEC_ALWAYS_OPEN
	SetMoseCe(0);
#endif
	kdebug(mod_media, PRINT_INFO, "close meida device\n");
	fDeviceStatus = DEVICE_CLOSE_STATUS;

	//close codec
	codec_reg_write(A_CODEC_CGR8, 0x1f);
	codec_reg_write(A_CODEC_CGR9, 0x1f);
	codec_reg_write(A_CODEC_CGR1, 0xff);
	codec_reg_write(A_CODEC_CGR10, 0);

	codec_reg_set(A_CODEC_PMR1, SB_OUT);
	codec_reg_set(A_CODEC_PMR2, SB_SLEEP);
	codec_reg_set(A_CODEC_PMR2, SB);
	codec_reg_set(A_CODEC_PMR1, SB_MIX | SB_DAC | SB_ADC);
}

////////////////////////////////////////////////////
// 功能: 打开codec设备
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void OpenMediaCodecDevice()
{
	unsigned char pmr1;
	pmr1 = codec_reg_read(A_CODEC_PMR1);
	kdebug(mod_media, PRINT_INFO, "pmr1 = %x\n",pmr1);
#ifdef CODEC_ALWAYS_OPEN
	if( codec_reg_read(A_CODEC_PMR1) & 0x90)
	{	//CODEC处于开的状态，不需要重新打开
		if( fDeviceStatus == DEVICE_OPEN_STATUS )
		{
			fDeviceStatus = DEVICE_OPEN_STATUS;
			kdebug(mod_media, PRINT_INFO, "codec is already open\n");
			return;
		}
		goto OPEN_MEDIA_CODEC_DEVICE;
	}
	else	//CODEC处于关闭状态，必须重新打开CODEC
	{
		kdebug(mod_media, PRINT_ERROR, "codec is close, need to open codec\n");
		goto OPEN_MEDIA_CODEC_DEVICE;
	}
#endif
	if( fDeviceStatus == DEVICE_OPEN_STATUS )
	{
		kdebug(mod_media, PRINT_INFO, "codec is already open\n");
		return;
	}
#ifdef CODEC_ALWAYS_OPEN
OPEN_MEDIA_CODEC_DEVICE:
	SetMoseCe(1);//0 //修改复位开机声音小的问题。
#endif
	kdebug(mod_media, PRINT_INFO, "open media device\n");
	fDeviceStatus = DEVICE_OPEN_STATUS;

	//codec寄存器设置
	codec_reg_write(A_CODEC_AICR, 0x0F);
	codec_reg_write(A_CODEC_TR1,0x00);	// 0x02);

	codec_reg_write(A_CODEC_IFR, 0xFF);		//Write 1 to Update of the flag
	codec_reg_write(A_CODEC_ICR, 0x3F);
	codec_reg_write(A_CODEC_CR2, 0x60);
	codec_reg_write(A_CODEC_CR3, 0xC0);		//??
	codec_reg_write(A_CODEC_CGR8, 0x1f);	//??
	codec_reg_write(A_CODEC_CGR9, 0x1f);	//??
	codec_reg_write(A_CODEC_CGR1, 0xff);
	codec_reg_write(A_CODEC_CGR10, 0);

	codec_reg_clear(A_CODEC_PMR2, SB);
	MillinsecoundDelay(10);
	codec_reg_clear(A_CODEC_PMR2, SB_SLEEP);
	codec_reg_clear(A_CODEC_PMR1, SB_DAC);
	codec_reg_clear(A_CODEC_PMR1, SB_ADC);
	codec_reg_clear(A_CODEC_PMR2, SB_MC);
	codec_reg_clear(A_CODEC_PMR1, SB_MIX);
	codec_reg_clear(A_CODEC_PMR1, SB_OUT);
	MillinsecoundDelay(20);
	// 关闭音量开关
	codec_reg_set(A_CODEC_CR1, DAC_MUTE);
}

////////////////////////////////////////////////////
// 功能: 关闭声音设备
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void CloseMediaDevice(char channel)
{
	int arg;
	int time;
	arg = ((channel == 0) ? RECORD_CHANNEL : PLAYBACK_CHANNEL );

	//等待DMA结束
	time = 0;
	while( INREG32(A_DMA_DTC(arg)) )
	{
#ifdef KPRINTF_DEF
		kprintf("close media device : count = %x\n",INREG32(A_DMA_DTC(arg)));
#endif
		sTimerSleep(10, NULL);

		//加入延迟处理，防止死锁
		time++;
		if( time > 10 )
		{
			kdebug(mod_media, PRINT_WARNING, "close media device timer out\n");
			break;
		}
	}

	//stop dma
	CLRREG32(A_DMA_DCS(arg), DCS_AR | DCS_HLT | DCS_CTE | DCS_CT);

	//close aic
	CLRREG32(AIC_CR, AIC_CR_ERPL);
	SETREG32(AIC_CR, AIC_CR_FLUSH_FIFO);
	OUTREG32(AIC_SR, 0x00000000);
#ifndef	CODEC_ALWAYS_OPEN
	CloseMediaCodecDevice();
#endif
}
////////////////////////////////////////////////////
// 功能: 开打声音设备
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void OpenMediaDevice(unsigned long sample,unsigned long channle , char mode)
{
	//统一设置成48000，双声道，16BIT PCM模式
	pcm_ioctl(PCM_SET_SAMPLE_RATE,sample);
	pcm_ioctl(PCM_SET_CHANNEL,channle);
	pcm_ioctl(PCM_SET_FORMAT,16);

	if( !mode )
	{
 		pcm_ioctl(PCM_SET_REPLAY,0);
		if( hPlayer == NULL )
			hPlayer = IrqAttach(IRQ_DMA_0 + PLAYBACK_CHANNEL, IPL_AUDIO, 0, intr_handler_playback_dma, NULL);
	}
	else
	{
		pcm_ioctl(PCM_SET_RECORD,0);
		if( hRecord == NULL )
			hRecord = IrqAttach(IRQ_DMA_0 + RECORD_CHANNEL, IPL_AUDIO, 0, intr_handler_record_dma, NULL);
	}
	
	if( !mode )
	{
#ifdef CONFIG_MAC_BDS6100A
		pcm_ioctl(PCM_SET_VOL,28);
#else
		pcm_ioctl(PCM_SET_VOL,24);
#endif
	}
	else
		pcm_ioctl(PCM_SET_VOL,0);
}

////////////////////////////////////////////////////
// 功能: 开打声音设备
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void MediaSysInit()
{
	// Start AIC and DMA Clock
	// Clock gate Bits. Controls the clock supplies to some peripherals. If set,
	// clock supplies to associated devices are stopped, and registers of the
	// device cannot be accessed also
	//CLRREG32(A_CPM_CLKGR, CLKGR_STOP_AIC_PCLK | CLKGR_STOP_DMAC);
	__cpm_start_aic1();			//clear CPM_CLKGR_AIC1
	__cpm_start_aic2();			//clear CPM_CLKGR_AIC2
	__cpm_start_dmac();			//clear CPM_CLKGR_DMAC

	// I2S device clock divider Register (I2SCDR) is a 32-bit read/write register that specifies the divider of
	// I2S device clock . This register is initialized to 0x00000004 only by any reset. Only word access can
	// be used on I2SCDR
	//OUTREG32(A_CPM_I2SCDR, 0x00);
	REG_CPM_I2SCDR = 0x00;

	// change enable. If CE is 1, writes on CDIV, HDIV, PDIV, MDIV, UDIV,
	// PXDIV or LDIV will start a frequency changing sequence immediately.
	// When CE is 0, writes on CDIV, HDIV, PDIV, MDIV, UDIV, PXDIV and
	// LDIV will not start a frequency changing sequence immediately. The
	// division ratio is actually updated in PLL multiple ratio changing sequence
	// or PLL Disable Sequence.
	// 0: Division ratios are updated in PLL multiple ratio changing sequence or
	// PLL Disable Sequence
	// 1: Division ratios are updated immediately
	//SETREG32(A_CPM_CPCCR, CPCCR_CHANGE_EN);
	__cpm_enable_pll_change();

	// I2SCR contains bits to control BIT_CLK stop, audio sample size, I2S or MSB-justified selection in
	// I2S/MSB-justified. It is valid only in I2S/MSB-justified format.
	//OUTREG32(A_AIC_I2SCR, 0x00);
	REG_AIC_I2SCR = 0;

	// AIC Init
	// AICFR contains bits to control FIFO threshold, AC-link or I2S/MSB-justified selection, AIC reset,
	// master/slave selection, and AIC enable.
	// Play ZERO sample when TX FIFO underflow or Play last sample when TX FIFO underflow
	REG_AIC_FR = AIC_FR_LSMP;
	// Internal CODEC
	//OUTREG32(A_AIC_AICFR, AICFR_CODEC_INT | AICFR_LSMP_LAST_SAMPLE);
	__aic_internal_codec();
	
	// 0 Select AC-link format
	// 1 Select I2S/MSB-justified format
	//SETREG32(A_AIC_AICFR, AICFR_I2S);
	__aic_select_i2s();

	//Enable SYSCLK output. When this bit is 1, the SYSCLK outputs to chip
	//outside is enabled. Else, the clock is disabled.
	__i2s_select_i2s();			//	OUTREG32(A_AIC_I2SCR, I2SCR_I2S);
										

	// SETREG32(A_AIC_AICFR, AICFR_BCKD_IN | AICFR_SYNCD_IN);
	__i2s_as_slave();
	
	// SETREG32(A_AIC_AICFR, AICFR_ENABLE);
	__aic_enable();
	
	// AIC Configure
	
	//SETREG32(A_AIC_AICFR, AICFR_RFTH(16) | AICFR_TFTH(24));
	// Receive FIFO threshold for interrupt or DMA request. The RFTH valid
	// value is 0 ~ 15
	__i2s_set_receive_trigger((16/2) - 1);
	// Transmit FIFO threshold for interrupt or DMA request
	__i2s_set_transmit_trigger(24/2);

	//OUTREG32(A_AIC_AICCR, AICCR_OSS_16BIT | AICCR_ISS_16BIT);
	// Output Sample Size. These bits reflect output sample data size from
	// memory or register. The data sizes supported are: 8, 16, 18, 20 and 24
	// bits. The sample data is LSB-justified in memory/register
	__i2s_set_oss_sample_size(16);
	// Input Sample Size. These bits reflect input sample data size to memory or
	// register. The data sizes supported are: 8, 16, 18, 20 and 24 bits. The
	// sample data is LSB-justified in memory/register.	
	__i2s_set_iss_sample_size(16);
	
	
	//SETREG32(A_AIC_AICCR, AICCR_RDMS | AICCR_TDMS | AICCR_FLUSH_FIFO);
	// Transmit DMA enable. This bit is used to enable or disable the DMA
	// during transmit audio data
	__i2s_enable_transmit_dma();
	// Receive DMA enable. This bit is used to enable or disable the DMA during
	// receiving audio data
	__i2s_enable_receive_dma();

	//SETREG32(AIC_CR, AIC_CR_FLUSH_FIFO);
	// JZ4750 D8 == FIFO Flush. Write 1 to this bit flush transmit/receive FIFOs to empty.Writing 0 to this bit has no effect and this bit is always reading 0.
	// JZ4755 D7 == Receive FIFO Flush. Write 1 to this bit flush receive FIFOs to empty
	//        D8 == Transmit FIFO Flush. Write 1 to this bit flush transmit FIFOs to empty.
//	__aic_flush_fifo();
	REG_AIC_CR |= (1<<7);
	
	// I2SCR contains bits to control BIT_CLK stop, audio sample size, I2S or MSB-justified selection in
	// I2S/MSB-justified. It is valid only in I2S/MSB-justified format.
	OUTREG32(A_AIC_AICSR, 0x00000000);

	//CLRREG32(A_AIC_AICFR, AICFR_LSMP_LAST_SAMPLE);
	__aic_clr_esclk();

#ifdef KPRINTF_DEF
	kprintf("AICFR[0x%x]\n", INREG32(AIC_FR));
	kprintf("AICCR[0x%x]\n", INREG32(AIC_CR));
	kprintf("AICSR[0x%x]\n", INREG32(AIC_SR));
#endif
	fDeviceStatus = DEVICE_CLOSE_STATUS;
	OpenMediaCodecDevice();
}

////////////////////////////////////////////////////
// 功能: 开打声音设备
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void MediaDraveInit(char channel)
{
#ifdef KPRINTF_DEF
	kprintf("pcm_init start!\n");
#endif

#ifndef	CODEC_ALWAYS_OPEN
	MediaSysInit();
#else
	kdebug(mod_media, PRINT_INFO, "fDeviceStatus = %d, pmr1 = %d\n",fDeviceStatus,codec_reg_read(A_CODEC_PMR1));
	if(fDeviceStatus == DEVICE_CLOSE_STATUS || (codec_reg_read(A_CODEC_PMR1) & 0x90) )
	{
		MediaSysInit();
	}
	else
	{
		//GetDmaInfo();
#ifdef KPRINTF_DEF
		kprintf("codec is open\n");
#endif
	}
#endif

	//DMA初始化
	if( channel )
	{
		dma_init(PLAYBACK_CHANNEL,1);
	}
	else
	{
		dma_init(RECORD_CHANNEL,0);
	}

	interrupt_count = 0;
#ifdef KPRINTF_DEF
	kprintf("===============================================\n  pcm_init finished!  \n===============================================\n");
#endif
}

////////////////////////////////////////////////////
// 功能: 删除media建立的中断向量表
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void DestroyMediaDevoice(char mode)
{
	if( !mode )
	{
		IrqDetach(hPlayer);
		hPlayer = NULL;
	}
	else
	{
		IrqDetach(hRecord);
		hRecord = NULL;
	}
}

////////////////////////////////////////////////////
// 功能: 触发DMA中断，传送数据
// 输入: 
// 输出:
// 返回: 
// 说明: 
////////////////////////////////////////////////////
void DmaDataToAic(int arg,unsigned int sour_addr, unsigned int len,unsigned char mode)
{

	if( !arg )
	{	//放音DMA
		dma_cache_wback_inv(sour_addr, len);
		CLRREG32(AIC_SR, AIC_SR_TUR);
		SETREG32(AIC_CR, AIC_CR_ERPL);
		dma_start(PLAYBACK_CHANNEL,	PHYADDR(sour_addr), PHYADDR(AIC_DR),len,mode);

		if (INREG32(AIC_SR) & AIC_SR_TUR)
			CLRREG32(AIC_SR, AIC_SR_TUR);
	}
	else
	{	//录音DMA
		__dcache_inv((unsigned long)sour_addr, len);
		SETREG32(AIC_CR, AIC_CR_EREC);
		dma_start(RECORD_CHANNEL, PHYADDR(AIC_DR), PHYADDR(sour_addr),len,mode);

		if (INREG32(A_AIC_AICSR) & AICSR_ROR)
			CLRREG32(A_AIC_AICSR, AICSR_ROR);
	}
}
