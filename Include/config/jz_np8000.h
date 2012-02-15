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
#define CONFIG_MCU_JZ4760B		// �������ͺ�
#define CONFIG_MAC_NP8000		// ��Ʒ�ͺ�
#define CONFIG_SIMDISK_FNAME			"NP8000.dsk"	// ����ģ������ļ�����
#define CONFIG_CPU_STRING				"JZ4760B"
#define CONFIG_PRODUCT_STRING			"NP8000"


/////////////////////////////////////////////////////////////////////////
// USB��ض���
/////////////////////////////////////////////////////////////////////////
// USB����ʶ���ַ���(���Ȳ�����16�ֽ�)
#ifdef WIN32
#define CONFIG_USB_IDENTIFY_STRING		"NOAH-SIMULATOR"
#else
#define CONFIG_USB_IDENTIFY_STRING		"NOAHOSV2"
#endif
#define CONFIG_USB_IDENTIFY_CODE		0xD800000A				// ����ʶ����
#define CONFIG_USB_MANUFACTURER_STRING	"WWW.NOAHEDU.COM"		// ��������
#define CONFIG_USB_PRODUCT_STRING		CONFIG_PRODUCT_STRING	// ��Ʒ����(С�ڵ���14�ֽ�)
#define CONFIG_USB_VENDOR_STRING		"NOAHEDU"				// ������(С�ڵ���8�ֽ�)

/////////////////////////////////////////////////////////////////////////
// �����豸
/////////////////////////////////////////////////////////////////////////
//#define	CONFIG_USB_BOOT				//��USB����,����USB�����޸�ϵͳ
//#define	CONFIG_NAND_BOOT			//��NAND����
#define	CONFIG_INAND_BOOT				//��INAND����

/////////////////////////////////////////////////////////////////////////
// �ڴ�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_SDRAM_SIZE			(4*32*1024*1024)
#define CONFIG_MMU_ENABLE			// ����MMU���
#ifdef WIN32
#undef CONFIG_MMU_ENABLE
#endif 
#define CONFIG_KERNEL_HEAP_SIZE		0x800000	// �����ں˶�ջ�ռ��С
#define CONFIG_MEM_PAGE_SIZE 		0x1000
#define CONFIG_APP_LOAD_START		0x400000	// Ӧ�ó��������ʼλ��


/////////////////////////////////////////////////////////////////////////
// ��������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWERKEY_SCANCODE	0xff7ffff7	// ���ؼ�ɨ����
#define CONFIG_BIOS_UPDATE_KEY		0x51		// Z��
#define CONFIG_BIOS_TEST_KEY		0x25		// Y��


/////////////////////////////////////////////////////////////////////////
// ����IO������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_PWRKEY_IO			(32*0+30)		// ���ػ������IO
#define CONFIG_PWRKEY_DOWN			0				// ���ػ�������
#define CONFIG_PWRKEY_UP			1				// ���ػ�������

#define BOARD_VER					0// 1��ES 1.0 �� 0��ES 1.1
#if BOARD_VER
#define CONFIG_AMP_IO				(32*4+7)		// ����Ƭѡ����IO
#define CONFIG_AMP_ENA				1				// ��������
#define CONFIG_AMP_DIS				0				// ���Ž�ֹ

#define CONFIG_PIPO_IO				(32*4+6)		// ����������IO
#define CONFIG_PIPO_ENA				1				// ������MOS�ܴ�
#define CONFIG_PIPO_DIS				0				// ������MOS�ܹر�

#define CONFIG_ERAPHONE_IO			(32*4+13)		// ����������IO
#define CONFIG_ERAPHONE_INERT 		0				// �����������ƽ
#define CONFIG_ERAPHONE_REMOVE		1				// �����γ�����ƽ

#define CONFIG_TFPWR_IO				(32*3+12)		// TF��Դ����
#define CONFIG_TFPWR_ON				0				// TF��Դ�򿪿��Ƶ�ƽ
#define CONFIG_TFPWR_OFF			1				// TF��Դ�رտ��Ƶ�ƽ

#define CONFIG_TFDET_IO				(32*3+13)		// TF��������
#define CONFIG_TFDET_INSERT			0				// TF���������ƽ
#define CONFIG_TFDET_REMOVE			1				// TF���γ�����ƽ

#define CONFIG_CHARGE_IO			(32*0+15)		// �����IO
#define CONFIG_CHARGE_ON			0				// �����м���ƽ
#define CONFIG_CHARGE_FULL 			1				// ���������ƽ

#define CONFIG_EXTPWR_IO			(32*0+12)		// �ⲿ������IO
#define CONFIG_EXTPWR_ON			1				// �ⲿ������м���ƽ
#define CONFIG_EXTPWR_OFF 			0				// �ⲿ����ȡ������ƽ

#define CONFIG_USB_IO				(32*0+13)		// USB���¼��IO
#define CONFIG_USB_INSERT			1				// USB���²������ƽ
#define CONFIG_USB_REMOVE			0				// USB���°γ�����ƽ

#define CONFIG_USB_HOST_POWER_IO	(32*0+9)		// USB HOST����IO
#define CONFIG_USB_HOST_POWER_ON	1				// USB HOST���⹩��
#define CONFIG_USB_HOST_POWER_OFF	0				// USB HOST�����⹩��
#else
// ES 1.1
#define CONFIG_AMP_IO				(32*4+0)		// ����Ƭѡ����IO
#define CONFIG_AMP_ENA				1				// ��������
#define CONFIG_AMP_DIS				0				// ���Ž�ֹ

#define CONFIG_PIPO_IO				(32*3+12)		// ����������IO
#define CONFIG_PIPO_ENA				1				// ������MOS�ܴ�
#define CONFIG_PIPO_DIS				0				// ������MOS�ܹر�

#define CONFIG_ERAPHONE_IO			(32*4+25)		// ����������IO
#define CONFIG_ERAPHONE_INERT 		0				// �����������ƽ
#define CONFIG_ERAPHONE_REMOVE		1				// �����γ�����ƽ

#define CONFIG_TFPWR_IO				(32*4+27)		// TF��Դ����
#define CONFIG_TFPWR_ON				0				// TF��Դ�򿪿��Ƶ�ƽ
#define CONFIG_TFPWR_OFF			1				// TF��Դ�رտ��Ƶ�ƽ

#define CONFIG_TFDET_IO				(32*3+13)		// TF��������
#define CONFIG_TFDET_INSERT			0				// TF���������ƽ
#define CONFIG_TFDET_REMOVE			1				// TF���γ�����ƽ

#define CONFIG_CHARGE_IO			(32*0+26)		// �����IO
#define CONFIG_CHARGE_ON			0				// �����м���ƽ
#define CONFIG_CHARGE_FULL 			1				// ���������ƽ

#define CONFIG_CHARGE_PROG_IO		(32*4+12)		// ����������IO
#define CONFIG_CHARGE_PROG_LIMIT	0				// ������������100MA

#define CONFIG_EXTPWR_IO			(32*0+28)		// �ⲿ������IO
#define CONFIG_EXTPWR_ON			1				// �ⲿ������м���ƽ
#define CONFIG_EXTPWR_OFF 			0				// �ⲿ����ȡ������ƽ

#define CONFIG_USB_IO				(32*4+7)		// USB���¼��IO
#define CONFIG_USB_INSERT			1				// USB���²������ƽ
#define CONFIG_USB_REMOVE			0				// USB���°γ�����ƽ

#define CONFIG_USB_HOST_POWER_IO	(32*0+29)		// USB HOST����IO
#define CONFIG_USB_HOST_POWER_ON	1				// USB HOST���⹩��
#define CONFIG_USB_HOST_POWER_OFF	0				// USB HOST�����⹩��

#endif
/////////////////////////////////////////////////////////////////////////
// LCD ����������붨��
/////////////////////////////////////////////////////////////////////////
#if 1
#define CONFIG_MAINLCD_XSIZE		1024
#define CONFIG_MAINLCD_YSIZE		600
#define CONFIG_MAINLCD_BPP			32
#else
#define CONFIG_MAINLCD_XSIZE		800
#define CONFIG_MAINLCD_YSIZE		480
#define CONFIG_MAINLCD_BPP			16
#endif
#define CONFIG_MAINLCD_ROTATION		0		// ��LCD��ת�Ƕ�

#define CONFIG_LCD_DISPLAY_SYNC

/////////////////////////////////////////////////////////////////////////
// NAND FLASH����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_NAND_BIOS_SIZE		6UL				// BIOSӳ���ļ�����С
#define CONFIG_NAND_SYSTEM_SIZE		16UL			// ����ӳ���ļ�����С

#define CONFIG_NAND_BOOT_SIZE		(CONFIG_NAND_SYSTEM_SIZE+CONFIG_NAND_BIOS_SIZE+4)
#define CONFIG_NAND_ISOFS_SIZE		CONST64(0)	// ISO�ļ�ϵͳ(�����ļ�ϵͳ)��С(M)
#define CONFIG_NAND_ROMFS_SIZE		CONST64(0)		// ROM�ļ�ϵͳ��С(M)
#define CONFIG_NAND_SYSCFG_SIZE		CONST64(4)		// ϵͳ������������С(M)	
#define CONFIG_NAND_RAMBK_SIZE		CONST64(0)		// �ڴ汸����������С(M)	
#define CONFIG_NAND_FATFS1_SIZE		CONST64(1024)	// F��(����FAT�ļ�ϵͳ)��С(M)

#define CONFIG_NAND_ALLOC_SIZE		(CONFIG_NAND_ISOFS_SIZE+CONFIG_NAND_ROMFS_SIZE+ \
										CONFIG_NAND_SYSCFG_SIZE+CONFIG_NAND_RAMBK_SIZE+ \
										CONFIG_NAND_FATFS1_SIZE)

#define CONFIG_NAND_IDT_OFFSET		0x1f8	// ��¼ϵͳ����С������λ��
#define CONFIG_NAND_LOGO_OFFSET		0x1f0	// ��¼LOGOӦ�ó�����Ϣ������λ��
//#define CONFIG_NAND_MRES_OFFSET		0x1e8	// ��¼��ý����Դ��


/////////////////////////////////////////////////////////////////////////
// RTC��λ���ȱʡ���ڡ�ʱ��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DEFAULT_YEAR		2012
#define CONFIG_DEFAULT_MONTH	1
#define CONFIG_DEFAULT_DAY		1
#define CONFIG_DEFAULT_HOUR		12
#define CONFIG_DEFAULT_MINUTE	0
#define CONFIG_DEFAULT_SECOND	0


/////////////////////////////////////////////////////////////////////////
// ����Ӳ������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWON_BATVAL		350
//#define CONFIG_USB_CHARGE		// �����Ƿ�����USB���г��
//#define CONFIG_CHARGE_WAKEUP	// ��绽��
#define CONFIG_USB_LINK_AUTO	// �����Ƿ�USB�Զ�����
#define CONFIG_WATCHDOG_ENABLE	// ���Ź�����
#define CONFIG_SD_ENABLE		// �����Ƿ�ʹ��SD���豸
#define CONFIG_RECORDER_ENABLE	// �����Ƿ�¼���豸
//#define CONFIG_ISO9660_SUPPORT	// ����֧��ISO�ļ�ϵͳ

/////////////////////////////////////////////////////////////////////////
// �ں�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAX_TASKNAME	512		// �����������(����·��)
#define CONFIG_MAX_DEVNAME	12		// ����豸����
#define CONFIG_MAX_PORTNAME	16		// ���˿�����

#define CONFIG_HZ			50		// �ں�TICKʱ��Ƶ��
#define CONFIG_TIME_SLICE	10		// Context�л���С����

#ifdef WIN32
#define WIN32_STACK_SIZE	0x8000
#else
#define WIN32_STACK_SIZE	0
#endif
#define CONFIG_KSTACK_SIZE	(4096+WIN32_STACK_SIZE)	// �ں��߳��ں˶�ջ��С
#define CONFIG_USTACK_SIZE	(64*1024+WIN32_STACK_SIZE)	// ȷʵ�߳��û���ջ��С
#define CONFIG_USER_PRIO	200		// �û��߳�ȷʵ���ȼ�

#define	CONFIG_ARG_MAX		255		// ��������ʱ���������ֽ���
#define	CONFIG_PATH_MAX		256		// ��������ʱ����������ֽ���

#define CONFIG_BIOS_LANGUAGE	1	// ����BIOS����

/////////////////////////////////////////////////////////////////////////
// ����ϵͳ����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_TOUCH_ENABLE				// �����Ƿ�ʹ�ô�����
#define CONFIG_USBHOST_ENABLE			// �����Ƿ�ʹ��USBHOST
#define CONFIG_MOUSE_ENABLE				// �����Ƿ�ʹ�����

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

#define CONFIG_FS_CHARSET		CONFG_CHARSET_GBK_ENABLE	//�ļ�ϵͳʹ���ַ���
#if defined(WIN32)
#define CONFIG_SHOW_SECRET_DISK		// �����Ƿ���ʾ���ص�FAT��
#endif

#define CONFIG_CFG_RAMFILE_SIZE			(8*1024)	// �����ڴ������ļ���С

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

#define CONFIG_MPLAYER_ENABLE			// �����Ƿ�ʹ��MPLayer��Ƶ��

/////////////////////////////////////////////////////////////////////////
// ������ض���
/////////////////////////////////////////////////////////////////////////
#define CONFIG_TTF_ENABLE				// ֧��TTF����
#define CONFIG_GB2312_RAMFONT			// ��GB2312����������C��
//#define CONFIG_MAKE_BIOS


#endif  // _CONFIG_H

