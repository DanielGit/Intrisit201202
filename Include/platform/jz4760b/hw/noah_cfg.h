/********************** BEGIN LICENSE BLOCK **********************************
 *
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co., Ltd. 2008. All rights reserved.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * http://www.ingenic.cn
 *
 ********************** END LICENSE BLOCK ************************************
 *
 *  Author:  <whxu@ingenic.cn>
 *
 *  Create:   2008-10-13, by whxu
 *
 *  Maintain:   2008-10-28, by jbyu
 *
 *  Maintain:   2008-11-04, by whxu, add macro DEVICE_CLOCK for apus v1.1
 *
 *  Maintain:   2008-11-29, by whxu, add LCD GPIO configuration
 *
 ****************************************************************************/

#ifndef __ASM_JZ4760_LEPUS_H__
#define __ASM_JZ4760_LEPUS_H__
/*======================================================================
 * Frequencies of on-board oscillators
 */
#define OSC_CLOCK		12000000	/* Main extal freq:	12 MHz */
#define JZ_EXTAL		OSC_CLOCK
#define JZ_EXTAL2		32768		/* RTC extal freq:	32.768 KHz */
#define DEVICE_CLOCK		12000000	// The clock for i2c, uart, tcu, aic, udc,
						// owi and sadc(Unit: Hz)
//----------------------------------------------------------------------------
//	CPU Clock Configuration
//----------------------------------------------------------------------------
//#define CFG_CPU_SPEED		360000000	// Unit: Hz
#define CFG_CPU_SPEED		528000000
//#define CFG_CPU_SPEED		600000000

#include "./jz4760_common.h"

#ifndef JZ4760B
 //Default CPCCR Configuration, C:H:H2:P:M:S = 1:2:4:4:4:4
#define CPCCR_CFG	( (0 << CPCCR_CDIV_LSB)		\
					| (1 << CPCCR_HDIV_LSB)		\
					| (3 << CPCCR_H2DIV_LSB)	\
					| (3 << CPCCR_PDIV_LSB)		\
					| (3 << CPCCR_MDIV_LSB)		\
					| (3 << CPCCR_SDIV_LSB)		\
					| CPCCR_MEM			\
					| CPCCR_CE			\
					| CPCCR_PCS)

#define	CPPCR_CFG	( CPCCR_M_N_OD					\
					| (0x20 << CPPCR0_PLLST_LSB)	\
					| CPPCR0_PLLEN)

#else
#define CPCCR_CFG	( (0 << CPCCR_CDIV_LSB)				\
					| (1 << CPCCR_HDIV_LSB)		\
					| (3 << CPCCR_H2DIV_LSB)	\
					| (3 << CPCCR_PDIV_LSB)		\
					| (3 << CPCCR_MDIV_LSB) 	\
					| CPCCR_MEM			\
					| CPCCR_CE			\
					| CPCCR_PCS)

#define	CPPCR_CFG	( CPCCR_M_N_OD					\
					| (0x20 << CPPCR0_PLLST_LSB)	\
					| CPPCR0_PLLEN)
#endif 
//----------------------------------------------------------------------------
//	Debug Configuration
//----------------------------------------------------------------------------
#define	DEBUG_UART_BASE			UART1_BASE
#define DEBUG_UART_BAUDRATE		57600


//----------------------------------------------------------------------------
//	Cache Configurations
//----------------------------------------------------------------------------
#define CFG_DCACHE_SIZE		(16 * 1024)		// Unit: Byte
#define CFG_ICACHE_SIZE		(16 * 1024)		// Unit: Byte
#define CFG_CACHELINE_SIZE	(32)


//----------------------------------------------------------------------------
//	Sadc Configurations
//----------------------------------------------------------------------------
#define R1			300
#define R2			100  //kohm
#define RESOLUTION		((R1+R2)*2500*1000/(4096*R2))

#define BAT_SAMPLE_TO_VOLTAGE(dat)	((dat * RESOLUTION) / 1000)

/*
	       VBAT
               |
	       R1
	       |______detect point
	       |
	       R2
	       |
	       GND

*/

/*
 * 采样电压(mv) = voltage_sample * (( R1 + R2 ) * 2500 * 1000 / (4096 * R2)) / 1000
 * voltage_sample	: 采样值.
 * 2500			: 电压最大测量值.
 * 4096			: 12bit adc.
 * R1, R2		: 分压电阻.
 * 1000			: 先把mv 换成 uv, 最后再转换成mv, 提高整型运算的精度.
 */

#if 1
/*--------------------------------------------------------------------------------
 * DDR2 info
 */
/* Chip Select */
#define DDR_CS1EN 0 // CSEN : whether a ddr chip exists 0 - un-used, 1 - used
#define DDR_CS0EN 1
#define DDR_DW32 1 /* 0 - 16-bit data width, 1 - 32-bit data width */

/* DDR2 paramters */
#define DDR_ROW 13 /* ROW : 12 to 14 row address */
#define DDR_COL 10 /* COL :  8 to 10 column address */
#define DDR_BANK8 0 /* Banks each chip: 0-4bank, 1-8bank */
#define DDR_CL 3 /* CAS latency: 1 to 7 */

/*
 * DDR2 controller timing1 register
 */
#if 0
#define DDR_tRAS 45 /*tRAS: ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP 8 /* 7.5ns READ to PRECHARGE command period. */
#define DDR_tRP 42 /* tRP: PRECHARGE command period to the same bank */
#define DDR_tRCD 42 /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC 60 /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD 8   /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR 15 /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR 2 /* unit: tCK. WRITE to READ command delay. */
#else
#define DDR_tRAS 45  /* tRAS: ACTIVE to PRECHARGE command period to the same bank. */
#define DDR_tRTP 8   /* 7.5ns READ to PRECHARGE command period. */
#define DDR_tRP  15  /* tRP: PRECHARGE command period to the same bank */
#define DDR_tRCD 15  /* ACTIVE to READ or WRITE command period to the same bank. */
#define DDR_tRC  60  /* ACTIVE to ACTIVE command period to the same bank.*/
#define DDR_tRRD 10  /* ACTIVE bank A to ACTIVE bank B command period. */
#define DDR_tWR  15  /* WRITE Recovery Time defined by register MR of DDR2 memory */
#define DDR_tWTR 8  /* WRITE to READ command delay. */
#endif
/*
 * DDR2 controller timing2 register
 */
#define DDR_tRFC 128 /* ns,  AUTO-REFRESH command period. */
#define DDR_tMINSR 6 /* Minimum Self-Refresh / Deep-Power-Down */
#define DDR_tXP 2 /* EXIT-POWER-DOWN to next valid command period: 1 to 8 tCK. */
#define DDR_tMRD 2 /* unit: tCK. Load-Mode-Register to next valid command period: 1 to 4 tCK */

/*
 * DDR2 controller refcnt register
 */
#define DDR_tREFI	        7800	/* Refresh period: ns */

#define DDR_CLK_DIV		1	/* Clock Divider. auto refresh
					 * cnt_clk = memclk/(16*(2^DDR_CLK_DIV))
					 */

#else
//----------------------------------------------------------------------------
//	SDRAM Configurations
//----------------------------------------------------------------------------
// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
//#define SDRAM_TREF		15625	/* Refresh period: 4096 refresh cycles/64ms */
#define SDRAM_TREF		7812	/* Refresh period: 8192 refresh cycles/64ms */

#define	SDRAM_DATA_BUS_WIDTH	32
#define	SDRAM_ROW_ADDR_WIDTH	13
#define	SDRAM_COL_ADDR_WIDTH	9
#define	SDRAM_BANK_NUM		4
#define	SDRAM_CAS_LATENCY	3

#endif

#if 0
//----------------------------------------------------------------------------
//	GPIO Configurations
//----------------------------------------------------------------------------<<<<<<< .mine
#define GPIO_UDC_DETE_PIN		(GPIO_GROUP_E + 19)//cma0707




#define MMC0_POWER_PIN			(GPIO_GROUP_F + 9)
#define MMC0_CD_PIN			(GPIO_GROUP_B + 22)
#define MMC0_SD_WP			(GPIO_GROUP_F + 4)
#define CARD_INSERT_VALUE0		GPIO_LOW_LEVEL

#define MMC1_POWER_PIN			(GPIO_GROUP_E + 9)
#define MMC1_CD_PIN			(GPIO_GROUP_A + 28)
#define MMC1_SD_WP			(UNUSED_GPIO_PIN)
#define CARD_INSERT_VALUE1		GPIO_LOW_LEVEL

#if (MMCTYPE == 1)
#define MMC_BIT_MODE			MMC_4BIT_MODE
#elif (MMCTYPE == 2)
//#define MMC_BIT_MODE			MMC_1BIT_MODE
#define MMC_BIT_MODE			MMC_4BIT_MODE
#endif


// Keypad
#define AD_KEY_INT_PIN			(GPIO_GROUP_E + 8)

#define	KEY_VOL_DECR_PIN		(GPIO_GROUP_D + 17)
#define	KEY_VOL_INCR_PIN		(GPIO_GROUP_D + 19)
#define	KEY_AUD_PREV_PIN		(GPIO_GROUP_C + 29)
#define	KEY_AUD_NEXT_PIN		(GPIO_GROUP_D + 27)
#define	KEY_MENU_PIN			(GPIO_GROUP_C + 31)

#define	KEY_DOWN_VALUE			( GPIO_LOW_LEVEL )
// The Special pin Key Down Value is NOT the KEY_DOWN_VALUE
#define	KEY_SPECIAL_PIN			(0)


// LCD
#define	LCD_POWER_PIN			(GPIO_GROUP_B+ 31)
#define	LCD_POWER_ON_LEVEL		(GPIO_HIGH_LEVEL)

#define	LCD_RESET_PIN			(GPIO_GROUP_B+ 31)//(UNUSED_GPIO_PIN)
#define	LCD_RESET_LEVEL			(GPIO_LOW_LEVEL)

#define	LCD_STANDBY_PIN			(GPIO_GROUP_F + 6)
#define	LCD_STANDBY_LEVEL		(GPIO_LOW_LEVEL)

#define	LCD_BACKLIGHT_PWM_CHN		(1)
//#define LCD_BACKLIGHT_PIN		(GPIO_GROUP_E + 1)
//#define LCD_BACKLIGHT_LEVEL		(GPIO_HIGH_LEVEL)

// SLCD
//#define SLCD_CS_PIN			(GPIO_GROUP_D + 22)
//#define SLCD_CS_LEVEL			(GPIO_LOW_LEVEL)

//#define SLCD_RESET_PIN 		(UNUSED_GPIO_PIN)
//#define SLCD_RESET_LEVEL		(GPIO_LOW_LEVEL)

//#define SLCD_BACKLIGHT_PWM_CHN	(2)

//Camera
#define CAM_RESET_PIN			(GPIO_GROUP_B+26) 
#define CAM_STANDBY_PIN			(GPIO_GROUP_B+27)  

// TV
#define TV_OUT_MODE_CVBS		0x00aa
#define TV_OUT_MODE_SVIDEO		0x00bb
#define TV_OUT_MODE_3DIV		0x00cc
#define TV_VIDEO_OUT_MODE		TV_OUT_MODE_CVBS
//#define TV_VIDEO_OUT_MODE		TV_OUT_MODE_SVIDEO

// Battery Monitor
//#define DC_DETE_PIN			(GPIO_GROUP_F + 11)
#define	DC_DETE_LEVEL			(GPIO_HIGH_LEVEL)

#define	CHARG_STAT_PIN			(GPIO_GROUP_D + 29)  //cma0707
#define	CHARG_STAT_LEVEL		(GPIO_LOW_LEVEL)

#define MTV_POWER_PIN			( GPIO_GROUP_A + 16 )
#define MTV_RESET_PIN			( GPIO_GROUP_A + 17 )
#define MTV_IRQ_PIN			( GPIO_GROUP_E + 18 )
#define MTV_CS_PIN			( GPIO_GROUP_E + 19 )
#define SMDDATAIO			( GPIO_GROUP_A + 22 )
#define SMDRESETIO		    	( GPIO_GROUP_A + 23 )
#define PWM_SMD			 	( 3 )
#endif

#endif	// __APUS_CFG_H__
