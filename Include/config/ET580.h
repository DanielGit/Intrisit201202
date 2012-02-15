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

/////////////////////////////////////////////////////////////////////////
// BIOS����
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_JTAG_ENABLE
#define CONFIG_BIOS_UPDATE_KEY	0x84
#define CONFIG_BIOS_TEST_KEY	0x6181

/////////////////////////////////////////////////////////////////////////
// ��Ʒ�ͺš��������ͺ�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_ARCH_JANUS		// �������ܹ�
#define CONFIG_MCU_AK3620		// �������ͺ�
#define CONFIG_MAC_ET580		// ��Ʒ�ͺ�
#define CONFIG_USB_IDENTIFY_CODE		0xE5800000		// ����ʶ����
// USB����ʶ���ַ���(���Ȳ�����16�ֽ�)
#ifdef WIN32
#define CONFIG_USB_IDENTIFY_STRING		"NOAH-SIMULATOR"
#else
#define CONFIG_USB_IDENTIFY_STRING		"NOAHOSV2"
#endif
#define CONFIG_SIMDISK_FNAME			"NOAHOSV2.dsk"	// ����ģ������ļ�����
#define CONFIG_CPU_STRING				"AK3620"
#define CONFIG_PRODUCT_STRING			"ET580"


/////////////////////////////////////////////////////////////////////////
// �ڴ�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_SDRAM_SIZE			(16*1024*1024)
#define CONFIG_MMU_ENABLE		// ����MMU���
#ifdef WIN32
#undef CONFIG_MMU_ENABLE
#endif 
#define CONFIG_KERNEL_HEAP_SIZE		0x200000	// �����ں˶�ջ�ռ��С
#define CONFIG_MEM_PAGE_SIZE 		0x1000
#define CONFIG_APP_LOAD_START		0x100000	// Ӧ�ó��������ʼλ��


/////////////////////////////////////////////////////////////////////////
// ��������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWERKEY_SCANCODE	0xff7ffff7	// ���ؼ�ɨ����
#define CONFIG_TEST_COMBO_KEY		0xfffdffde	// �������Ը��ϼ�ɨ����
#define CONFIG_SYSU_COMBO_KEY		0xfffdffef	// ϵͳ�������ϼ�ɨ����
//#define CONFIG_KEYTONE_ENABLE		// �����Ƿ�ʹ�ð�������ģ��

/////////////////////////////////////////////////////////////////////////
// LCD ����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAINLCD_XSIZE		320
#define CONFIG_MAINLCD_YSIZE		240
#define CONFIG_MAINLCD_BPP			16
#define CONFIG_MAINLCD_ROTATION		0		// ��LCD��ת�Ƕ�
#define CONFIG_MAINLCD_BACKLIGHT	32		// ȱʡ��LCD���⼶��

/////////////////////////////////////////////////////////////////////////
// NAND FLASH����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_NAND_SIZE			2048UL			// NAND FLASH��С(M)
#define CONFIG_NAND_SECTOR_SIZE		0x200UL			// NAND FLASH������С����
#define CONFIG_NAND_PAGE_SIZE		0x1000UL		// NAND FLASHҳ��С����
#define CONFIG_NAND_BLOCK_SIZE		(0x1000*128UL)	// NAND FLASH���С����

#define CONFIG_NAND_BIOS_SIZE		4		// BIOSӳ���ļ�����С
#define CONFIG_NAND_SYSTEM_SIZE		4		// ����ӳ���ļ�����С
#define CONFIG_NAND_BOOT_SIZE		(CONFIG_NAND_SYSTEM_SIZE+CONFIG_NAND_BIOS_SIZE+2)

#define CONFIG_NAND_ISOFS_SIZE		0UL		// ISO�ļ�ϵͳ(�����ļ�ϵͳ)��С(M)
#define CONFIG_NAND_ROMFS_SIZE		350UL	// ROM�ļ�ϵͳ��С(M)
#define CONFIG_NAND_SYSCFG_SIZE		0UL		// ϵͳ������������С(M)	
#define CONFIG_NAND_RAMBK_SIZE		0UL		// �ڴ汸����������С(M)	

#define CONFIG_NAND_FATFS0_SIZE		1550UL	// D���ļ�ϵͳ��С(M)
#define CONFIG_NAND_FATFS1_SIZE		4UL		// F��(����FAT�ļ�ϵͳ)��С(M)
#define CONFIG_NAND_RESERVE0_SIZE	0UL		// ��δ����	
#define CONFIG_NAND_RESERVE1_SIZE	0UL		// ��δ����	

#define CONFIG_NAND_LOGIC_SIZE		(CONFIG_NAND_ISOFS_SIZE+CONFIG_NAND_ROMFS_SIZE+ \
			CONFIG_NAND_SYSCFG_SIZE+CONFIG_NAND_RAMBK_SIZE+ \
			CONFIG_NAND_FATFS0_SIZE+CONFIG_NAND_FATFS1_SIZE+ \
			CONFIG_NAND_RESERVE0_SIZE+CONFIG_NAND_RESERVE1_SIZE)

#define CONFIG_NAND_IDT_OFFSET		0x1f8	// ��¼ϵͳ����С������λ��
#define CONFIG_NAND_LOGO_OFFSET		0x1f0	// ��¼LOGOӦ�ó�����Ϣ������λ��


/////////////////////////////////////////////////////////////////////////
// RTC��λ���ȱʡ���ڡ�ʱ��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DEFAULT_YEAR		2009
#define CONFIG_DEFAULT_MONTH	1
#define CONFIG_DEFAULT_DAY		1
#define CONFIG_DEFAULT_HOUR		12
#define CONFIG_DEFAULT_MINUTE	0
#define CONFIG_DEFAULT_SECOND	0


/////////////////////////////////////////////////////////////////////////
// ����Ӳ������������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_USB_CHARGE		// �����Ƿ�����USB���г��
#define CONFIG_USB_LINK_AUTO	// �����Ƿ�USB�Զ�����
#define CONFIG_WATCHDOG_ENABLE	// ���Ź�����
#define CONFIG_SD_ENABLE		// �����Ƿ�ʹ��SD���豸
#define CONFIG_RECORDER_ENABLE	// �����Ƿ�¼���豸

/////////////////////////////////////////////////////////////////////////
// �ں�����������붨��
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAX_TASKNAME	512		// �����������(����·��)
#define CONFIG_MAX_DEVNAME	12		// ����豸����
#define CONFIG_MAX_PORTNAME	16		// ���˿�����

#define CONFIG_HZ			100		// �ں�TICKʱ��Ƶ��
#define CONFIG_TIME_SLICE	20		// Context�л���С����

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
#define CONFIG_TOUCH_ENABLE				// �����Ƿ�ʹ�ô�����


/////////////////////////////////////////////////////////////////////////
// �ļ�ϵͳ����������붨��
/////////////////////////////////////////////////////////////////////////
// ʹ��UNICOD����ת������
#define CONFG_CHARSET_GBK_ENABLE		0	
//#define CONFG_CHARSET_BIG5_ENABLE		1
//#define CONFG_CHARSET_KSC5601_ENABLE	2
//#define CONFG_CHARSET_SHIFTJIS_ENABLE	3
#define CONFG_CHARSET_ISO1_ENABLE		4
//#define CONFG_CHARSET_ISO2_ENABLE		5
//#define CONFG_CHARSET_ISO5_ENABLE		6
//#define CONFG_CHARSET_ISO7_ENABLE		7
#define CONFG_CHARSET_ISO9_ENABLE		8

#define CONFIG_FS_CHARSET		CONFG_CHARSET_ISO9_ENABLE	//�ļ�ϵͳʹ���ַ���
#if defined(WIN32) && defined(VC_DEBUG)
#define CONFIG_SHOW_SECRET_DISK		// �����Ƿ���ʾ���ص�FAT��
#endif

/////////////////////////////////////////////////////////////////////////
// �������������������붨��
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_DECODE_MP3_ENABLE		// �����Ƿ�ʹ�����MP3������
//#define CONFIG_DECODE_NSP_ENABLE		// �����Ƿ�ʹ�����NSP������
//#define CONFIG_DECODE_WAV_ENABLE		// �����Ƿ�ʹ�����MS-ADPCM������
//#define CONFIG_DECODE_MIDI_ENABLE		// �����Ƿ�ʹ��MIDI

//#define CONFIG_AUDIO_MUX_ENABLE			// �����Ƿ�ʹ�ö�ͨ����Ƶ���

/////////////////////////////////////////////////////////////////////////
// ������ض���
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_TTF_ENABLE				// ֧��TTF����
#define CONFIG_GB2312_RAMFONT			// ��GB2312����������C��
//#define CONFIG_MAKE_BIOS


#endif  // _CONFIG_H
