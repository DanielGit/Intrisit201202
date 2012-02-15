//[PROPERTY]===========================================[PROPERTY]
//            *****   ���Ӵʵ�ƽ̨  *****
//        --------------------------------------
//                  ���������йص�����
//        --------------------------------------
//                 ��Ȩ: ��ŵ���ۿƼ�
//             ---------------------------
//                  ��   ��   ��   ʷ
//        --------------------------------------
//  �汾    ��ǰ		˵��		
//  V0.1    2008-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _CONFIG_H
#define _CONFIG_H

#if defined(WIN32)
#define CONST64(x)		(x##I64)
#pragma warning(disable: 4244 4761) 
#else
#define CONST64(x)		(x##ULL)
#endif

/////////////////////////////////////////////////////////////////////////
// ��Ʒ�ͺš��������ͺ�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_ARCH_MIPS		// �������ܹ�
#define CONFIG_ARCH_XBURST		// �������ܹ�
#define CONFIG_MCU_JZ4750L		// �������ͺ�
#define CONFIG_MAC_BDS6100		// ��Ʒ�ͺ�
#define CONFIG_SIMDISK_FNAME			"BDS6100.dsk"	// ����ģ������ļ�����
#define CONFIG_CPU_STRING				"JZ4750L"
#define CONFIG_PRODUCT_STRING			"BDS6100"


/////////////////////////////////////////////////////////////////////////
// USB��ض���
/////////////////////////////////////////////////////////////////////////
// USB����ʶ���ַ���(���Ȳ�����16�ֽ�)
#ifdef WIN32
#define CONFIG_USB_IDENTIFY_STRING		"NOAH-SIMULATOR"
#else
#define CONFIG_USB_IDENTIFY_STRING		"NOAHOSV2"
#endif
#define CONFIG_USB_IDENTIFY_CODE		0x61000000				// ����ʶ����
#define CONFIG_USB_MANUFACTURER_STRING	"WWW.FRANKLIN.COM"		// ��������
#define CONFIG_USB_PRODUCT_STRING		CONFIG_PRODUCT_STRING	// ��Ʒ����(С�ڵ���14�ֽ�)
#define CONFIG_USB_VENDOR_STRING		"FRANKLIN"				// ������(С�ڵ���8�ֽ�)


/////////////////////////////////////////////////////////////////////////
// �ڴ�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_SDRAM_SIZE			(16*1024*1024)
#define CONFIG_MMU_ENABLE		// ����MMU���
#ifdef WIN32
#undef CONFIG_MMU_ENABLE
#endif 
#define CONFIG_KERNEL_HEAP_SIZE		0x400000	// �����ں˶�ջ�ռ��С
#define CONFIG_MEM_PAGE_SIZE 		0x1000
#define CONFIG_APP_LOAD_START		0x400000	// Ӧ�ó��������ʼλ��


/////////////////////////////////////////////////////////////////////////
// ��������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWERKEY_SCANCODE	0xff7ffff7	// ���ؼ�ɨ����
#define CONFIG_BIOS_UPDATE_KEY		0x604
#define CONFIG_BIOS_TEST_KEY		0x802
#define CONFIG_BIOS_LANGUAGE	0


/////////////////////////////////////////////////////////////////////////
// ����IO������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_PWRKEY_IO			(32*1+31)		// ���ػ������IO
#define CONFIG_PWRKEY_DOWN			0				// ���ػ�������
#define CONFIG_PWRKEY_UP			1				// ���ػ�������

#define CONFIG_AMP_IO				(32*2+17)		// ����Ƭѡ����IO
#define CONFIG_AMP_ENA				1				// ��������
#define CONFIG_AMP_DIS				0				// ���Ž�ֹ

#define CONFIG_PIPO_IO				(32*2+16)		// ����������IO
#define CONFIG_PIPO_ENA				1				// ������MOS�ܴ�
#define CONFIG_PIPO_DIS				0				// ������MOS�ܹر�

//#define CONFIG_ERAPHONE_IO			(32*a+b)	// ����������IO
//#define CONFIG_ERAPHONE_INERT 		0			// �����������ƽ
//#define CONFIG_ERAPHONE_REMOVE		1			// �����γ�����ƽ

#define CONFIG_TFPWR_IO				(32*2+23)		// TF��Դ����
#define CONFIG_TFPWR_ON				0				// TF��Դ�򿪿��Ƶ�ƽ
#define CONFIG_TFPWR_OFF			1				// TF��Դ�رտ��Ƶ�ƽ

#define CONFIG_TFDET_IO				(32*2+24)		// TF��������
#define CONFIG_TFDET_INSERT			1				// TF���������ƽ
#define CONFIG_TFDET_REMOVE			0				// TF���γ�����ƽ

#define CONFIG_CHARGE_IO			(32*2+18)		// �����IO
#define CONFIG_CHARGE_ON			0				// �����м���ƽ
#define CONFIG_CHARGE_FULL 			1				// ���������ƽ

#define CONFIG_EXTPWR_IO			(32*2+25)		// �ⲿ������IO
#define CONFIG_EXTPWR_ON			1				// �ⲿ������м���ƽ
#define CONFIG_EXTPWR_OFF 			0				// �ⲿ����ȡ������ƽ

#define CONFIG_USB_IO				(32*2+25)		// USB���¼��IO
#define CONFIG_USB_INSERT			1				// USB���²������ƽ
#define CONFIG_USB_REMOVE			0				// USB���°γ�����ƽ


/////////////////////////////////////////////////////////////////////////
// LCD ����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAINLCD_XSIZE		480
#define CONFIG_MAINLCD_YSIZE		320
#define CONFIG_MAINLCD_BPP			1
#define CONFIG_MAINLCD_ROTATION		0		// ��LCD��ת�Ƕ�

/////////////////////////////////////////////////////////////////////////
// NAND FLASH����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_NAND_BIOS_SIZE		6UL				// BIOSӳ���ļ�����С
#define CONFIG_NAND_SYSTEM_SIZE		6UL				// ����ӳ���ļ�����С

#define CONFIG_NAND_BOOT_SIZE		(CONFIG_NAND_SYSTEM_SIZE+CONFIG_NAND_BIOS_SIZE+4)
#define CONFIG_NAND_ISOFS_SIZE		CONST64(1350)		// ISO�ļ�ϵͳ(�����ļ�ϵͳ)��С(M)
#define CONFIG_NAND_ROMFS_SIZE		CONST64(256)	// ROM�ļ�ϵͳ��С(M)
#define CONFIG_NAND_SYSCFG_SIZE		CONST64(4)		// ϵͳ������������С(M)	
#define CONFIG_NAND_RAMBK_SIZE		CONST64(0)		// �ڴ汸����������С(M)	
#define CONFIG_NAND_FATFS1_SIZE		CONST64(4)		// F��(����FAT�ļ�ϵͳ)��С(M)

#define CONFIG_NAND_ALLOC_SIZE		(CONFIG_NAND_ISOFS_SIZE+CONFIG_NAND_ROMFS_SIZE+ \
										CONFIG_NAND_SYSCFG_SIZE+CONFIG_NAND_RAMBK_SIZE+ \
										CONFIG_NAND_FATFS1_SIZE)

#define CONFIG_NAND_IDT_OFFSET		0x1f8	// ��¼ϵͳ����С������λ��
#define CONFIG_NAND_LOGO_OFFSET		0x1f0	// ��¼LOGOӦ�ó�����Ϣ������λ��


/////////////////////////////////////////////////////////////////////////
// RTC��λ���ȱʡ���ڡ�ʱ��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DEFAULT_YEAR		2011
#define CONFIG_DEFAULT_MONTH	1
#define CONFIG_DEFAULT_DAY		1
#define CONFIG_DEFAULT_HOUR		12
#define CONFIG_DEFAULT_MINUTE	0
#define CONFIG_DEFAULT_SECOND	0


/////////////////////////////////////////////////////////////////////////
// ����Ӳ������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWON_BATVAL		350
#define CONFIG_USB_CHARGE		// �����Ƿ�����USB���г��
#define CONFIG_USB_LINK_AUTO	// �����Ƿ�USB�Զ�����
#define CONFIG_WATCHDOG_ENABLE	// ���Ź�����
#define CONFIG_SD_ENABLE		// �����Ƿ�ʹ��SD���豸
#define CONFIG_RECORDER_ENABLE	// �����Ƿ�¼���豸
#define CONFIG_ISO9660_SUPPORT	// ����֧��ISO�ļ�ϵͳ
#define CONFIG_BUZZER_ENABLE	// �����Ƿ�ʹ�÷���Ƭ


/////////////////////////////////////////////////////////////////////////
// �ں�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAX_TASKNAME	512		// �����������(����·��)
#define CONFIG_MAX_DEVNAME	12		// ����豸����
#define CONFIG_MAX_PORTNAME	16		// ���˿�����

#define CONFIG_HZ			100		// �ں�TICKʱ��Ƶ��
#define CONFIG_TIME_SLICE	10		// Context�л���С����

#ifdef WIN32
#define WIN32_STACK_SIZE	0x8000
#else
#define WIN32_STACK_SIZE	0
#endif
#define CONFIG_KSTACK_SIZE	(4096+WIN32_STACK_SIZE)	// �ں��߳��ں˶�ջ��С
#define CONFIG_USTACK_SIZE	(4096+WIN32_STACK_SIZE)	// ȷʵ�߳��û���ջ��С
#define CONFIG_USER_PRIO	200		// �û��߳�ȷʵ���ȼ�

#define	CONFIG_ARG_MAX		255		// ��������ʱ���������ֽ���
#define	CONFIG_PATH_MAX		256		// ��������ʱ����������ֽ���


/////////////////////////////////////////////////////////////////////////
// ����ϵͳ����������붨��
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_TOUCH_ENABLE				// �����Ƿ�ʹ�ô�����
#define CONFIG_LCD_GRAY					// �����Ƿ��Ǻڰ׻ҽ�


/////////////////////////////////////////////////////////////////////////
// �ļ�ϵͳ����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_FTL_V2					// �����Ƿ�ʹ�õڶ���FTL
#define CONFIG_BURN_IMGV2				// �����Ƿ�ʹ�õڶ�����¼ӳ��
#define CONFIG_INAND_SUPPORT			// �����Ƿ�֧��INAND

// ʹ��UNICOD����ת������
#define CONFG_CHARSET_GBK_ENABLE		0	
//#define CONFG_CHARSET_BIG5_ENABLE		1
//#define CONFG_CHARSET_KSC5601_ENABLE	2
//#define CONFG_CHARSET_SHIFTJIS_ENABLE	3
#define CONFG_CHARSET_ISO1_ENABLE		4
//#define CONFG_CHARSET_ISO2_ENABLE		5
//#define CONFG_CHARSET_ISO5_ENABLE		6
//#define CONFG_CHARSET_ISO7_ENABLE		7
//#define CONFG_CHARSET_ISO9_ENABLE		8

#define CONFIG_FS_CHARSET		CONFG_CHARSET_ISO1_ENABLE	//�ļ�ϵͳʹ���ַ���
#if defined(WIN32) && defined(VC_DEBUG)
#define CONFIG_SHOW_SECRET_DISK		// �����Ƿ���ʾ���ص�FAT��
#endif

#define CONFIG_CFG_RAMFILE_SIZE			(2*1024)	// �����ڴ������ļ���С

/////////////////////////////////////////////////////////////////////////
// �������������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DECODE_MP3_ENABLE		// �����Ƿ�ʹ�����MP3������
#define CONFIG_DECODE_NSP_ENABLE		// �����Ƿ�ʹ�����NSP������
#define CONFIG_DECODE_WAV_ENABLE		// �����Ƿ�ʹ�����MS-ADPCM������
#define CONFIG_DECODE_MIDI_ENABLE		// �����Ƿ�ʹ��MIDI

#define CONFIG_AUDIO_MUX_ENABLE			// �����Ƿ�ʹ�ö�ͨ����Ƶ���
#define CONFIG_DIRECT_WAVEOUT			// �����Ƿ�ʹ��ֱ��WAVE����豸
#define CONFIG_DIRECT_OSD				// �����Ƿ�ʹ��ֱ��OSD�豸

/////////////////////////////////////////////////////////////////////////
// ������ض���
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_TTF_ENABLE				// ֧��TTF����
#define CONFIG_GB2312_RAMFONT			// ��GB2312����������C��
//#define CONFIG_MAKE_BIOS


#endif  // _CONFIG_H

