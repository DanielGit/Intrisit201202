//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//                  条件编译有关的配置
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2008-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _CONFIG_H
#define _CONFIG_H

/////////////////////////////////////////////////////////////////////////
// BIOS编译
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_JTAG_ENABLE
#define CONFIG_BIOS_UPDATE_KEY	0x84
#define CONFIG_BIOS_TEST_KEY	0x6181

/////////////////////////////////////////////////////////////////////////
// 产品型号、处理器型号相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_ARCH_JANUS		// 处理器架构
#define CONFIG_MCU_AK3620		// 处理器型号
#define CONFIG_MAC_ET580		// 产品型号
#define CONFIG_USB_IDENTIFY_CODE		0xE5800000		// 机型识别码
// USB联机识别字符串(长度不超出16字节)
#ifdef WIN32
#define CONFIG_USB_IDENTIFY_STRING		"NOAH-SIMULATOR"
#else
#define CONFIG_USB_IDENTIFY_STRING		"NOAHOSV2"
#endif
#define CONFIG_SIMDISK_FNAME			"NOAHOSV2.dsk"	// 定义模拟磁盘文件名称
#define CONFIG_CPU_STRING				"AK3620"
#define CONFIG_PRODUCT_STRING			"ET580"


/////////////////////////////////////////////////////////////////////////
// 内存相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_SDRAM_SIZE			(16*1024*1024)
#define CONFIG_MMU_ENABLE		// 定义MMU情况
#ifdef WIN32
#undef CONFIG_MMU_ENABLE
#endif 
#define CONFIG_KERNEL_HEAP_SIZE		0x200000	// 定义内核堆栈空间大小
#define CONFIG_MEM_PAGE_SIZE 		0x1000
#define CONFIG_APP_LOAD_START		0x100000	// 应用程序加载起始位置


/////////////////////////////////////////////////////////////////////////
// 按键相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWERKEY_SCANCODE	0xff7ffff7	// 开关键扫描码
#define CONFIG_TEST_COMBO_KEY		0xfffdffde	// 机器测试复合键扫描码
#define CONFIG_SYSU_COMBO_KEY		0xfffdffef	// 系统升级复合键扫描码
//#define CONFIG_KEYTONE_ENABLE		// 定义是否使用按键声音模块

/////////////////////////////////////////////////////////////////////////
// LCD 相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAINLCD_XSIZE		320
#define CONFIG_MAINLCD_YSIZE		240
#define CONFIG_MAINLCD_BPP			16
#define CONFIG_MAINLCD_ROTATION		0		// 主LCD旋转角度
#define CONFIG_MAINLCD_BACKLIGHT	32		// 缺省主LCD背光级别

/////////////////////////////////////////////////////////////////////////
// NAND FLASH相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_NAND_SIZE			2048UL			// NAND FLASH大小(M)
#define CONFIG_NAND_SECTOR_SIZE		0x200UL			// NAND FLASH扇区大小定义
#define CONFIG_NAND_PAGE_SIZE		0x1000UL		// NAND FLASH页大小定义
#define CONFIG_NAND_BLOCK_SIZE		(0x1000*128UL)	// NAND FLASH块大小定义

#define CONFIG_NAND_BIOS_SIZE		4		// BIOS映象文件最大大小
#define CONFIG_NAND_SYSTEM_SIZE		4		// 程序映象文件最大大小
#define CONFIG_NAND_BOOT_SIZE		(CONFIG_NAND_SYSTEM_SIZE+CONFIG_NAND_BIOS_SIZE+2)

#define CONFIG_NAND_ISOFS_SIZE		0UL		// ISO文件系统(光盘文件系统)大小(M)
#define CONFIG_NAND_ROMFS_SIZE		350UL	// ROM文件系统大小(M)
#define CONFIG_NAND_SYSCFG_SIZE		0UL		// 系统配置数据区大小(M)	
#define CONFIG_NAND_RAMBK_SIZE		0UL		// 内存备份数据区大小(M)	

#define CONFIG_NAND_FATFS0_SIZE		1550UL	// D盘文件系统大小(M)
#define CONFIG_NAND_FATFS1_SIZE		4UL		// F盘(隐藏FAT文件系统)大小(M)
#define CONFIG_NAND_RESERVE0_SIZE	0UL		// 暂未定义	
#define CONFIG_NAND_RESERVE1_SIZE	0UL		// 暂未定义	

#define CONFIG_NAND_LOGIC_SIZE		(CONFIG_NAND_ISOFS_SIZE+CONFIG_NAND_ROMFS_SIZE+ \
			CONFIG_NAND_SYSCFG_SIZE+CONFIG_NAND_RAMBK_SIZE+ \
			CONFIG_NAND_FATFS0_SIZE+CONFIG_NAND_FATFS1_SIZE+ \
			CONFIG_NAND_RESERVE0_SIZE+CONFIG_NAND_RESERVE1_SIZE)

#define CONFIG_NAND_IDT_OFFSET		0x1f8	// 记录系统包大小的数据位置
#define CONFIG_NAND_LOGO_OFFSET		0x1f0	// 记录LOGO应用程序信息的数据位置


/////////////////////////////////////////////////////////////////////////
// RTC复位后的缺省日期、时间
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DEFAULT_YEAR		2009
#define CONFIG_DEFAULT_MONTH	1
#define CONFIG_DEFAULT_DAY		1
#define CONFIG_DEFAULT_HOUR		12
#define CONFIG_DEFAULT_MINUTE	0
#define CONFIG_DEFAULT_SECOND	0


/////////////////////////////////////////////////////////////////////////
// 其他硬件相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_USB_CHARGE		// 定义是否允许USB进行充电
#define CONFIG_USB_LINK_AUTO	// 定义是否USB自动联机
#define CONFIG_WATCHDOG_ENABLE	// 看门狗允许
#define CONFIG_SD_ENABLE		// 定义是否使用SD卡设备
#define CONFIG_RECORDER_ENABLE	// 定义是否录音设备

/////////////////////////////////////////////////////////////////////////
// 内核相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAX_TASKNAME	512		// 最大任务名称(包括路径)
#define CONFIG_MAX_DEVNAME	12		// 最大设备名称
#define CONFIG_MAX_PORTNAME	16		// 最大端口名称

#define CONFIG_HZ			100		// 内核TICK时钟频率
#define CONFIG_TIME_SLICE	20		// Context切换最小周期

#ifdef WIN32
#define WIN32_STACK_SIZE	0x8000
#else
#define WIN32_STACK_SIZE	0
#endif
#define CONFIG_KSTACK_SIZE	(4096+WIN32_STACK_SIZE)	// 内核线程内核堆栈大小
#define CONFIG_USTACK_SIZE	(4096+WIN32_STACK_SIZE)	// 确实线程用户堆栈大小
#define CONFIG_USER_PRIO	200		// 用户线程确实优先级

#define	CONFIG_ARG_MAX		255		// 创建进程时，最大参数字节数
#define	CONFIG_PATH_MAX		256		// 创建进程时，最大命令字节数



/////////////////////////////////////////////////////////////////////////
// 窗体系统相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_TOUCH_ENABLE				// 定义是否使用触摸屏


/////////////////////////////////////////////////////////////////////////
// 文件系统相关条件编译定义
/////////////////////////////////////////////////////////////////////////
// 使用UNICOD编码转换表定义
#define CONFG_CHARSET_GBK_ENABLE		0	
//#define CONFG_CHARSET_BIG5_ENABLE		1
//#define CONFG_CHARSET_KSC5601_ENABLE	2
//#define CONFG_CHARSET_SHIFTJIS_ENABLE	3
#define CONFG_CHARSET_ISO1_ENABLE		4
//#define CONFG_CHARSET_ISO2_ENABLE		5
//#define CONFG_CHARSET_ISO5_ENABLE		6
//#define CONFG_CHARSET_ISO7_ENABLE		7
#define CONFG_CHARSET_ISO9_ENABLE		8

#define CONFIG_FS_CHARSET		CONFG_CHARSET_ISO9_ENABLE	//文件系统使用字符集
#if defined(WIN32) && defined(VC_DEBUG)
#define CONFIG_SHOW_SECRET_DISK		// 定义是否显示隐藏的FAT盘
#endif

/////////////////////////////////////////////////////////////////////////
// 声音编解码相关条件编译定义
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_DECODE_MP3_ENABLE		// 定义是否使用软件MP3解码器
//#define CONFIG_DECODE_NSP_ENABLE		// 定义是否使用软件NSP解码器
//#define CONFIG_DECODE_WAV_ENABLE		// 定义是否使用软件MS-ADPCM解码器
//#define CONFIG_DECODE_MIDI_ENABLE		// 定义是否使用MIDI

//#define CONFIG_AUDIO_MUX_ENABLE			// 定义是否使用多通道音频输出

/////////////////////////////////////////////////////////////////////////
// 字体相关定义
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_TTF_ENABLE				// 支持TTF字体
#define CONFIG_GB2312_RAMFONT			// 把GB2312字体点阵放入C盘
//#define CONFIG_MAKE_BIOS


#endif  // _CONFIG_H
