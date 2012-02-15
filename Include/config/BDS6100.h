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

#if defined(WIN32)
#define CONST64(x)		(x##I64)
#pragma warning(disable: 4244 4761) 
#else
#define CONST64(x)		(x##ULL)
#endif

/////////////////////////////////////////////////////////////////////////
// 产品型号、处理器型号相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_ARCH_MIPS		// 处理器架构
#define CONFIG_ARCH_XBURST		// 处理器架构
#define CONFIG_MCU_JZ4750L		// 处理器型号
#define CONFIG_MAC_BDS6100		// 产品型号
#define CONFIG_SIMDISK_FNAME			"BDS6100.dsk"	// 定义模拟磁盘文件名称
#define CONFIG_CPU_STRING				"JZ4750L"
#define CONFIG_PRODUCT_STRING			"BDS6100"


/////////////////////////////////////////////////////////////////////////
// USB相关定义
/////////////////////////////////////////////////////////////////////////
// USB联机识别字符串(长度不超出16字节)
#ifdef WIN32
#define CONFIG_USB_IDENTIFY_STRING		"NOAH-SIMULATOR"
#else
#define CONFIG_USB_IDENTIFY_STRING		"NOAHOSV2"
#endif
#define CONFIG_USB_IDENTIFY_CODE		0x61000000				// 机型识别码
#define CONFIG_USB_MANUFACTURER_STRING	"WWW.FRANKLIN.COM"		// 制作厂商
#define CONFIG_USB_PRODUCT_STRING		CONFIG_PRODUCT_STRING	// 产品名称(小于等于14字节)
#define CONFIG_USB_VENDOR_STRING		"FRANKLIN"				// 销售商(小于等于8字节)


/////////////////////////////////////////////////////////////////////////
// 内存相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_SDRAM_SIZE			(16*1024*1024)
#define CONFIG_MMU_ENABLE		// 定义MMU情况
#ifdef WIN32
#undef CONFIG_MMU_ENABLE
#endif 
#define CONFIG_KERNEL_HEAP_SIZE		0x400000	// 定义内核堆栈空间大小
#define CONFIG_MEM_PAGE_SIZE 		0x1000
#define CONFIG_APP_LOAD_START		0x400000	// 应用程序加载起始位置


/////////////////////////////////////////////////////////////////////////
// 按键相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWERKEY_SCANCODE	0xff7ffff7	// 开关键扫描码
#define CONFIG_BIOS_UPDATE_KEY		0x604
#define CONFIG_BIOS_TEST_KEY		0x802
#define CONFIG_BIOS_LANGUAGE	0


/////////////////////////////////////////////////////////////////////////
// 控制IO口相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_PWRKEY_IO			(32*1+31)		// 开关机键检测IO
#define CONFIG_PWRKEY_DOWN			0				// 开关机键按下
#define CONFIG_PWRKEY_UP			1				// 开关机键弹起

#define CONFIG_AMP_IO				(32*2+17)		// 功放片选控制IO
#define CONFIG_AMP_ENA				1				// 功放允许
#define CONFIG_AMP_DIS				0				// 功放禁止

#define CONFIG_PIPO_IO				(32*2+16)		// 波波音控制IO
#define CONFIG_PIPO_ENA				1				// 波波音MOS管打开
#define CONFIG_PIPO_DIS				0				// 波波音MOS管关闭

//#define CONFIG_ERAPHONE_IO			(32*a+b)	// 耳机插入检测IO
//#define CONFIG_ERAPHONE_INERT 		0			// 耳机插入检测电平
//#define CONFIG_ERAPHONE_REMOVE		1			// 耳机拔出检测电平

#define CONFIG_TFPWR_IO				(32*2+23)		// TF电源控制
#define CONFIG_TFPWR_ON				0				// TF电源打开控制电平
#define CONFIG_TFPWR_OFF			1				// TF电源关闭控制电平

#define CONFIG_TFDET_IO				(32*2+24)		// TF卡插入检测
#define CONFIG_TFDET_INSERT			1				// TF卡插入检测电平
#define CONFIG_TFDET_REMOVE			0				// TF卡拔出检测电平

#define CONFIG_CHARGE_IO			(32*2+18)		// 充电检测IO
#define CONFIG_CHARGE_ON			0				// 充电进行检测电平
#define CONFIG_CHARGE_FULL 			1				// 充电满检测电平

#define CONFIG_EXTPWR_IO			(32*2+25)		// 外部供电检测IO
#define CONFIG_EXTPWR_ON			1				// 外部供电进行检测电平
#define CONFIG_EXTPWR_OFF 			0				// 外部供电取消检测电平

#define CONFIG_USB_IO				(32*2+25)		// USB电缆检测IO
#define CONFIG_USB_INSERT			1				// USB电缆插入检测电平
#define CONFIG_USB_REMOVE			0				// USB电缆拔出检测电平


/////////////////////////////////////////////////////////////////////////
// LCD 相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAINLCD_XSIZE		480
#define CONFIG_MAINLCD_YSIZE		320
#define CONFIG_MAINLCD_BPP			1
#define CONFIG_MAINLCD_ROTATION		0		// 主LCD旋转角度

/////////////////////////////////////////////////////////////////////////
// NAND FLASH相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_NAND_BIOS_SIZE		6UL				// BIOS映象文件最大大小
#define CONFIG_NAND_SYSTEM_SIZE		6UL				// 程序映象文件最大大小

#define CONFIG_NAND_BOOT_SIZE		(CONFIG_NAND_SYSTEM_SIZE+CONFIG_NAND_BIOS_SIZE+4)
#define CONFIG_NAND_ISOFS_SIZE		CONST64(1350)		// ISO文件系统(光盘文件系统)大小(M)
#define CONFIG_NAND_ROMFS_SIZE		CONST64(256)	// ROM文件系统大小(M)
#define CONFIG_NAND_SYSCFG_SIZE		CONST64(4)		// 系统配置数据区大小(M)	
#define CONFIG_NAND_RAMBK_SIZE		CONST64(0)		// 内存备份数据区大小(M)	
#define CONFIG_NAND_FATFS1_SIZE		CONST64(4)		// F盘(隐藏FAT文件系统)大小(M)

#define CONFIG_NAND_ALLOC_SIZE		(CONFIG_NAND_ISOFS_SIZE+CONFIG_NAND_ROMFS_SIZE+ \
										CONFIG_NAND_SYSCFG_SIZE+CONFIG_NAND_RAMBK_SIZE+ \
										CONFIG_NAND_FATFS1_SIZE)

#define CONFIG_NAND_IDT_OFFSET		0x1f8	// 记录系统包大小的数据位置
#define CONFIG_NAND_LOGO_OFFSET		0x1f0	// 记录LOGO应用程序信息的数据位置


/////////////////////////////////////////////////////////////////////////
// RTC复位后的缺省日期、时间
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DEFAULT_YEAR		2011
#define CONFIG_DEFAULT_MONTH	1
#define CONFIG_DEFAULT_DAY		1
#define CONFIG_DEFAULT_HOUR		12
#define CONFIG_DEFAULT_MINUTE	0
#define CONFIG_DEFAULT_SECOND	0


/////////////////////////////////////////////////////////////////////////
// 其他硬件相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_POWON_BATVAL		350
#define CONFIG_USB_CHARGE		// 定义是否允许USB进行充电
#define CONFIG_USB_LINK_AUTO	// 定义是否USB自动联机
#define CONFIG_WATCHDOG_ENABLE	// 看门狗允许
#define CONFIG_SD_ENABLE		// 定义是否使用SD卡设备
#define CONFIG_RECORDER_ENABLE	// 定义是否录音设备
#define CONFIG_ISO9660_SUPPORT	// 定义支持ISO文件系统
#define CONFIG_BUZZER_ENABLE	// 定义是否使用蜂鸣片


/////////////////////////////////////////////////////////////////////////
// 内核相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_MAX_TASKNAME	512		// 最大任务名称(包括路径)
#define CONFIG_MAX_DEVNAME	12		// 最大设备名称
#define CONFIG_MAX_PORTNAME	16		// 最大端口名称

#define CONFIG_HZ			100		// 内核TICK时钟频率
#define CONFIG_TIME_SLICE	10		// Context切换最小周期

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
//#define CONFIG_TOUCH_ENABLE				// 定义是否使用触摸屏
#define CONFIG_LCD_GRAY					// 定义是否是黑白灰阶


/////////////////////////////////////////////////////////////////////////
// 文件系统相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_FTL_V2					// 定义是否使用第二版FTL
#define CONFIG_BURN_IMGV2				// 定义是否使用第二版烧录映像
#define CONFIG_INAND_SUPPORT			// 定义是否支持INAND

// 使用UNICOD编码转换表定义
#define CONFG_CHARSET_GBK_ENABLE		0	
//#define CONFG_CHARSET_BIG5_ENABLE		1
//#define CONFG_CHARSET_KSC5601_ENABLE	2
//#define CONFG_CHARSET_SHIFTJIS_ENABLE	3
#define CONFG_CHARSET_ISO1_ENABLE		4
//#define CONFG_CHARSET_ISO2_ENABLE		5
//#define CONFG_CHARSET_ISO5_ENABLE		6
//#define CONFG_CHARSET_ISO7_ENABLE		7
//#define CONFG_CHARSET_ISO9_ENABLE		8

#define CONFIG_FS_CHARSET		CONFG_CHARSET_ISO1_ENABLE	//文件系统使用字符集
#if defined(WIN32) && defined(VC_DEBUG)
#define CONFIG_SHOW_SECRET_DISK		// 定义是否显示隐藏的FAT盘
#endif

#define CONFIG_CFG_RAMFILE_SIZE			(2*1024)	// 定义内存配置文件大小

/////////////////////////////////////////////////////////////////////////
// 声音编解码相关条件编译定义
/////////////////////////////////////////////////////////////////////////
#define CONFIG_DECODE_MP3_ENABLE		// 定义是否使用软件MP3解码器
#define CONFIG_DECODE_NSP_ENABLE		// 定义是否使用软件NSP解码器
#define CONFIG_DECODE_WAV_ENABLE		// 定义是否使用软件MS-ADPCM解码器
#define CONFIG_DECODE_MIDI_ENABLE		// 定义是否使用MIDI

#define CONFIG_AUDIO_MUX_ENABLE			// 定义是否使用多通道音频输出
#define CONFIG_DIRECT_WAVEOUT			// 定义是否使用直接WAVE输出设备
#define CONFIG_DIRECT_OSD				// 定义是否使用直接OSD设备

/////////////////////////////////////////////////////////////////////////
// 字体相关定义
/////////////////////////////////////////////////////////////////////////
//#define CONFIG_TTF_ENABLE				// 支持TTF字体
#define CONFIG_GB2312_RAMFONT			// 把GB2312字体点阵放入C盘
//#define CONFIG_MAKE_BIOS


#endif  // _CONFIG_H

