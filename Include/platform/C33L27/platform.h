#ifndef _C33L27_H
#define _C33L27_H

#define BreakPoint()		
#define IMODE_EDGE	0		// edge trigger
#define IMODE_LEVEL	1		// level trigger

#define KernArea(addr)		1
#define UserArea(addr)		1

#define REG8(addr)	(*(volatile unsigned char *)(addr))
#define REG16(addr)	(*(volatile unsigned short *)(addr))
#define REG32(addr)	(*(volatile unsigned long *)(addr))

enum
{
	P00 = 0,
	P01,
	P02,
	P03,
	P04,
	P05,
	P06,
	P07,

	P10,
	P11,
	P12,
	P13,
	P14,
	P15,
	P16,
	P17,
	
	P20,
	P21,
	PU22,
	PU23,
	PU24,
	PU25,
	PU26,
	PU27,

	P30,
	P31,
	P32,
	P33,
	P34,
	P35,
	P36,
	PU37,

	P40,
	P41,
	P42,
	PU43,
	PU44,
	PU45,
	PU46,
	PU47,
	
	P50,
	P51,
	P52,
	P53,
	P54,
	P55,
	P56,
	PU57,

	P60,
	PU61,
	PU62,
	PU63,
	PU64,
	PU65,
	PU66,
	PU67,

	P70,
	P71,
	P72,
	P73,
	P74,
	P75,
	P76,
	P77,

	P80,
	P81,
	P82,
	P83,
	PU84,
	PU85,
	PU86,
	PU87,

	P90,
	P91,
	P92,
	P93,
	P94,
	P95,
	P96,
	P97,

	PA0,
	PA1,
	PA2,
	PA3,
	PA4,
	PA5,
	PA6,
	PA7,

	PB0,
	PB1,
	PB2,
	PB3,
	PB4,
	PB5,
	PB6,
	PB7,

	PC0,
	PC1,
	PC2,
	PC3,
	PC4,
	PC5,
	PC6,
	PC7,

	PD0,
	PD1,
	PD2,
	PD3,
	PD4,
	PD5,
	PD6,
	PD7,

	PE0,
	PE1,
	PE2,
	PE3,
	PE4,
	PE5,
	PUE6,
	PUE7,

	PF0,
	PF1,
	PF2,
	PF3,
	PF4,
	PUF5,
	PUF6,
	PUF7
};

enum
{
	T16A_CLK = 0,
	TCLK,
	NANDIF_CLK,
	REMC_CLK,
	ADC_CLK,
	PORT_CLK,
	SDMMC_CLK,
	I2S_CLK,
	ITC_CLK,
	DMAC_CLK,
	HIF_CLK,
	LCDC_SAPB_CLK,
	LCDC_AHB_CLK,
	LCDC_CLK,
	USBREG_CLK,
	USB_CLK,
	BCU_CLK,
	PSC1_2_CLK,
	T8F_CLK,
	UART_CLK,
	T16P_CLK,
	USI_CLK,
	USIL_CLK,
	MISC_CLK,
	IVRAM_ARB_CLK,
	A3RAM_CLK,
	RTC_SAPB_CLK,
	WDT_CLK
};

enum
{
	IRQ_RESET = 0,
	IRQ_RES0,
	IRQ_EXT,
	IRQ_UND,
	IRQ_RES1,
	IRQ_RES2,
	IRQ_MISALIGN,
	IRQ_NMI,
	IRQ_RES3,
	IRQ_RES4,
	IRQ_RES5,
	IRQ_RES6,
	IRQ_SOFT0,
	IRQ_SOFT1,
	IRQ_SOFT2,
	IRQ_SOFT3,
	IRQ_PORT0,
	IRQ_PORT1,
	IRQ_KEY0,
	IRQ_KEY1,
	IRQ_DMAC02,
	IRQ_DMAC13,
	IRQ_DMAC46,
	IRQ_DMAC57,
	IRQ_T16A0,
	IRQ_T16A1,
	IRQ_T16P0,
	IRQ_T16P1,
	IRQ_T16P2,
	IRQ_T16P3,
	IRQ_T8F024,
	IRQ_T8F135,
	IRQ_AD,
	IRQ_RTC,
	IRQ_USI_UART,
	IRQ_USI1,
	IRQ_USI2,
	IRQ_USIL,
	IRQ_LCDC,
	IRQ_SDMMC,
	IRQ_REMC,
	IRQ_IIS0,
	IRQ_IIS1,
	IRQ_HIF,
	IRQ_USB,
	NIRQS
};

/*
MISC registers
(MISC)
(8-bit device)
*/
#define REG_MISC_RTCWT	0x300010 //RTC Wait Control Register Configure RTC access cycles
#define REG_MISC_USB	0x300012 //USB Configuration Register Enable USB interrupt, snooze, and configure access cycle
#define REG_MISC_RAMWT	0x300014 //Internal RAM Wait Control Register Configure internal RAM access cycles
#define REG_MISC_BOOT	0x300016 //Boot Register Indicate/set boot conditions
#define REG_MISC_TRACE	0x300018 //
#define REG_MISC_PROT0	0x300020 //Misc Protect Register 0 Enable/disable Misc register write protection
#define REG_MISC_PROT1	0x300022 //Misc Protect Register 1
#define REG_MISC_RAMCFG	0x300024 //RAM Configuration Register Configure internal RAM
#define MISCBIT_RTCWT	0
#define MISCBIT_USBINTEN	6
#define MISCBIT_USBSNZ	5
#define MISCBIT_USBWT	0
#define MISCBIT_CALWT	3
#define MISCBIT_A3WT	1
#define MISCBIT_A0WT	0
#define MISCBIT_BOOT	4
#define MISCBIT_BOOTEN	1
#define MISCBIT_LUTCFG	3
#define MISCBIT_RAMCFG	0
#define MISCBIT_TRACE	0


/*
Clock
management
unit (CMU)
(8-bit device)
*/
#define REG_CMU_OSCSEL	0x300100 //Clock Source Select Register Select system clock source
#define REG_CMU_OSCCTL	0x300101 //Oscillation Control Register Control oscillators
#define REG_CMU_LCLKDIV	0x300103 //LCDC Clock Division Ratio Select Register Set LCDC_CLK frequency
#define REG_CMU_SYSCLKDIV	0x300105 //System Clock Division Ratio Select Register Set system clock frequency
#define REG_CMU_CMUCLK	0x300106 //CMU_CLK Select Register Select CMU_CLK output clock
#define REG_CMU_PLLINDIV	0x300107 //PLL Input Clock Division Ratio Select Register Set PLL input clock frequency
#define REG_CMU_PLLCTL0	0x300108 //PLL Control Register 0 Set PLL multiplication rate and enable PLL
#define REG_CMU_PLLCTL1	0x300109 //PLL Control Register 1 Set PLL parameters
#define REG_CMU_PLLCTL2	0x30010a //PLL Control Register 2
#define REG_CMU_SSCG0	0x30010c //SSCG Macro Control Register 0 Enable SSCG
#define REG_CMU_SSCG1	0x30010d //SSCG Macro Control Register 1 Set SSCG parameters
#define REG_CMU_PROTECT	0x300110 //CMU Write Protect Register Enable/disable CMU register write protection
#define REG_CMU_CLKCTL0	0x300114 //Clock Control Register 0 Control clock supply to peripheral/bus modules
#define REG_CMU_CLKCTL1	0x300115 //Clock Control Register 1
#define REG_CMU_CLKCTL2	0x300116 //Clock Control Register 2
#define REG_CMU_CLKCTL3	0x300117 //Clock Control Register 3
#define REG_CMU_CLKCTL4	0x300118 //Clock Control Register 4
#define CMUBIT_CLKSEL	0
#define CMUBIT_OSC3WT	4
#define CMUBIT_OSC1EN	1
#define CMUBIT_OSC3EN	0
#define CMUBIT_LCLKDIV	0
#define CMUBIT_SYSCLKDIV	0
#define CMUBIT_CMU_CLKSEL	0
#define CMUBIT_PLLINDIV	0
#define CMUBIT_PLLN	4
#define CMUBIT_PLLV	2
#define CMUBIT_PLLPOWR	0
#define CMUBIT_PLLVC	4
#define CMUBIT_PLLRS	0
#define CMUBIT_PLLCS	6
#define CMUBIT_PLLBYP	5
#define CMUBIT_PLLCP	0
#define CMUBIT_SSMCON	0
#define CMUBIT_SSMCITM	4
#define CMUBIT_SSMCIDT	0
#define CMUBIT_T16A_CKE	3
#define CMUBIT_TCLK_CKE	2
#define CMUBIT_TCLK_SEL	0
#define CMUBIT_NANDIF_CKE	7
#define CMUBIT_REMC_CKE	6
#define CMUBIT_ADC_CKE	5
#define CMUBIT_PORT_CKE	4
#define CMUBIT_SDMMC_CKE	3
#define CMUBIT_I2S_CKE	2
#define CMUBIT_ITC_CKE	1
#define CMUBIT_DMAC_CKE	0
#define CMUBIT_HIF_CKE	7
#define CMUBIT_LCDC_SAPB_CKE	6
#define CMUBIT_LCDC_AHB_CKE	5
#define CMUBIT_LCDC_CKE	4
#define CMUBIT_USBREG_CKE	3
#define CMUBIT_USB_CKE	2
#define CMUBIT_BCU_CKE	1
#define CMUBIT_BCLK_CKE	0
#define CMUBIT_PSC1_2_CKE	7
#define CMUBIT_T8F_CKE	6
#define CMUBIT_UART_CKE	5
#define CMUBIT_T16P_CKE	4
#define CMUBIT_USI_CKE	1
#define CMUBIT_USIL_CKE	0
#define CMUBIT_MISC_CKE	4
#define CMUBIT_IVRAM_ARB_CKE	3
#define CMUBIT_A3RAM_CKE	2
#define CMUBIT_RTC_SAPB_CKE	1
#define CMUBIT_WDT_CKE	0

/*
Interrupt
controller (ITC)
(8-bit device)
*/
#define REG_ITC_PINT0_LV	0x300210 //Port Input 0 Interrupt Level Register Set port input 0 interrupt level
#define REG_ITC_PINT1_LV	0x300211 //Port Input 1 Interrupt Level Register Set port input 1 interrupt level
#define REG_ITC_KINT0_LV	0x300212 //Key Input 0 Interrupt Level Register Set key input 0 interrupt level
#define REG_ITC_KINT1_LV	0x300213 //Key Input 1 Interrupt Level Register Set key input 1 interrupt level
#define REG_ITC_DMA02_LV	0x300214 //DMAC Ch.0/2 Interrupt Level Register Set DMAC Ch.0 and 2 interrupt levels
#define REG_ITC_DMA13_LV	0x300215 //DMAC Ch.1/3 Interrupt Level Register Set DMAC Ch.1 and 3 interrupt levels
#define REG_ITC_DMA46_LV	0x300216 //DMAC Ch.4/6 Interrupt Level Register Set DMAC Ch.4 and 6 interrupt levels
#define REG_ITC_DMA57_LV	0x300217 //DMAC Ch.5/7 Interrupt Level Register Set DMAC Ch.5 and 7 interrupt levels
#define REG_ITC_T16P0_LV	0x300218 //T16P Ch.0 Interrupt Level Register Set T16P Ch.0 interrupt level
#define REG_ITC_T16P1_LV	0x300219 //T16P Ch.1 Interrupt Level Register Set T16P Ch.1 interrupt level
#define REG_ITC_T16A0_LV	0x30021a //T16A6 Ch.0 Interrupt Level Register Set T16A6 Ch.0 interrupt level
#define REG_ITC_T16A1_LV	0x30021b //T16A6 Ch.1 Interrupt Level Register Set T16A6 Ch.1 interrupt level
#define REG_ITC_T16A2_LV	0x30021c //T16A6 Ch.2 Interrupt Level Register Set T16A6 Ch.2 interrupt level
#define REG_ITC_T16A3_LV	0x30021d //T16A6 Ch.3 Interrupt Level Register Set T16A6 Ch.3 interrupt level
#define REG_ITC_T8F024_LV	0x30021e //T8F Ch.0/2/4 Interrupt Level Register Set T8F Ch.0, 2, and 4 interrupt levels
#define REG_ITC_T8F135_LV	0x30021f //T8F Ch.1/3/5 Interrupt Level Register Set T8F Ch.1, 3, and 5 interrupt levels
#define REG_ITC_ADC10_LV	0x300220 //ADC10 Interrupt Level Register Set ADC10 interrupt level
#define REG_ITC_RTC_LV	0x300221 //RTC Interrupt Level Register Set RTC interrupt level
#define REG_ITC_USI0_LV	0x300222 //USI Ch.0/UART Interrupt Level Register Set USI Ch.0 and UART interrupt levels
#define REG_ITC_USI1_LV	0x300223 //USI Ch.1 Interrupt Level Register Set USI Ch.1 interrupt level
#define REG_ITC_USI2_LV	0x300224 //USI Ch.2 Interrupt Level Register Set USI Ch.2 interrupt level
#define REG_ITC_USIL_LV	0x300225 //USIL Interrupt Level Register Set USIL interrupt level
#define REG_ITC_LCDC_LV	0x300226 //LCDC Interrupt Level Register Set LCDC interrupt level
#define REG_ITC_SDMMC_LV	0x300227 //SD_MMC Interrupt Level Register Set SD_MMC interrupt level
#define REG_ITC_REMC_LV	0x300228 //REMC Interrupt Level Register Set REMC interrupt level
#define REG_ITC_I2SOUT_LV	0x300229 //I2S Output Interrupt Level Register Set I2S output channel interrupt level
#define REG_ITC_I2SIN_LV	0x30022a //I2S Input Interrupt Level Register Set I2S input channel interrupt level
#define REG_ITC_HIF_LV	0x30022b //HIF Interrupt Level Register Set HIF interrupt level
#define REG_ITC_USB_LV	0x30022c //USB Interrupt Level Register Set USB interrupt level

/*
GPIO & port
MUX
(GPIO/PMUX)
(8-bit device)
*/
#define REG_GPIO_P0_DAT	0x300300 //P0 Port Data Register P0 port input/output data
#define REG_GPIO_P0_IOC	0x300301 //P0 Port I/O Control Register Control P0 port input/output direction
#define REG_GPIO_P1_DAT	0x300302 //P1 Port Data Register P1 port input/output data
#define REG_GPIO_P1_IOC	0x300303 //P1 Port I/O Control Register Control P1 port input/output direction
#define REG_GPIO_P2_DAT	0x300304 //P2 Port Data Register P2 port input/output data
#define REG_GPIO_P2_IOC	0x300305 //P2 Port I/O Control Register Control P2 port input/output direction
#define REG_GPIO_P3_DAT	0x300306 //P3 Port Data Register P3 port input/output data
#define REG_GPIO_P3_IOC	0x300307 //P3 Port I/O Control Register Control P3 port input/output direction
#define REG_GPIO_P4_DAT	0x300308 //P4 Port Data Register P4 port input/output data
#define REG_GPIO_P4_IOC	0x300309 //P4 Port I/O Control Register Control P4 port input/output direction
#define REG_GPIO_P5_DAT	0x30030a //P5 Port Data Register P5 port input/output data
#define REG_GPIO_P5_IOC	0x30030b //P5 Port I/O Control Register Control P5 port input/output direction
#define REG_GPIO_P6_DAT	0x30030c //P6 Port Data Register P6 port input/output data
#define REG_GPIO_P6_IOC	0x30030d //P6 Port I/O Control Register Control P6 port input/output direction
#define REG_GPIO_P7_DAT	0x30030e //P7 Port Data Register P7 port input data
#define REG_GPIO_P8_DAT	0x300310 //P8 Port Data Register P8 port input/output data
#define REG_GPIO_P8_IOC	0x300311 //P8 Port I/O Control Register Control P8 port input/output direction
#define REG_GPIO_P9_DAT	0x300312 //P9 Port Data Register P9 port input/output data
#define REG_GPIO_P9_IOC	0x300313 //P9 Port I/O Control Register Control P9 port input/output direction
#define REG_GPIO_PA_DAT	0x300314 //PA Port Data Register PA port input/output data
#define REG_GPIO_PA_IOC	0x300315 //PA Port I/O Control Register Control PA port input/output direction
#define REG_GPIO_PB_DAT	0x300316 //PB Port Data Register PB port input/output data
#define REG_GPIO_PB_IOC	0x300317 //PB Port I/O Control Register Control PB port input/output direction
#define REG_GPIO_PC_DAT	0x300318 //PC Port Data Register PC port input/output data
#define REG_GPIO_PC_IOC	0x300319 //PC Port I/O Control Register Control PC port input/output direction
#define REG_GPIO_PD_DAT	0x30031a //PD Port Data Register PD port input/output data
#define REG_GPIO_PD_IOC	0x30031b //PD Port I/O Control Register Control PD port input/output direction
#define REG_GPIO_PE_DAT	0x30031c //PE Port Data Register PE port input/output data
#define REG_GPIO_PE_IOC	0x30031d //PE Port I/O Control Register Control PE port input/output direction
#define REG_GPIO_PF_DAT	0x30031e //PF Port Data Register PF port input/output data
#define REG_GPIO_PF_IOC	0x30031f //PF Port I/O Control Register Control PF port input/output direction
#define REG_GPIO_FPT01_SEL	0x300340 //FPT0每1 Interrupt Port Select Register Select ports used for FPT0每1 interrupts
#define REG_GPIO_FPT23_SEL	0x300341 //FPT2每3 Interrupt Port Select Register Select ports used for FPT2每3 interrupts
#define REG_GPIO_FPT45_SEL	0x300342 //FPT4每5 Interrupt Port Select Register Select ports used for FPT4每5 interrupts
#define REG_GPIO_FPT67_SEL	0x300343 //FPT6每7 Interrupt Port Select Register Select ports used for FPT6每7 interrupts
#define REG_GPIO_FPT03_POL	0x300344 //FPT0每3 Interrupt Polarity Select Register Select input signal polarity for FPT0每3 interrupts
#define REG_GPIO_FPT47_POL	0x300345 //FPT4每7 Interrupt Polarity Select Register Select input signal polarity for FPT4每7 interrupts
#define REG_GPIO_FPT03_MOD	0x300346 //FPT0每3 Interrupt Mode Select Register Select edge/level mode for FPT0每3 interrupts
#define REG_GPIO_FPT47_MOD	0x300347 //FPT4每7 Interrupt Mode Select Register Select edge/level mode for FPT4每7 interrupts
#define REG_GPIO_FPT03_MSK	0x300348 //FPT0每3 Interrupt Mask Register Enable/disable FPT0每3 interrupts
#define REG_GPIO_FPT47_MSK	0x300349 //FPT4每7 Interrupt Mask Register Enable/disable FPT4每7 interrupts
#define REG_GPIO_FPT03_FLG	0x30034a //FPT0每3 Interrupt Flag Register Indicate FPT0每3 interrupt cause status
#define REG_GPIO_FPT47_FLG	0x30034b //FPT4每7 Interrupt Flag Register Indicate FPT4每7 interrupt cause status
#define REG_GPIO_FPT01_CHAT	0x30034c //FPT0每1 Interrupt Chattering Filter Control Register Control FPT0每1 chattering filter
#define REG_GPIO_FPT23_CHAT	0x30034d //FPT2每3 Interrupt Chattering Filter Control Register Control FPT2每3 chattering filter
#define REG_GPIO_FPT45_CHAT	0x30034e //FPT4每5 Interrupt Chattering Filter Control Register Control FPT4每5 chattering filter
#define REG_GPIO_FPT67_CHAT	0x30034f //FPT6每7 Interrupt Chattering Filter Control Register Control FPT6每7 chattering filter
#define REG_GPIO_DMA	0x300350 //Port DMA Trigger Source Select Register Select port DMA trigger source
#define REG_GPIO_FPK0_07_SEL	0x300360 //FPK000每007 Interrupt Select Register Select ports used for FPK0 interrupts
#define REG_GPIO_FPK0_8F_SEL	0x300361 //FPK008每015 Interrupt Select Register
#define REG_GPIO_FPK1_07_SEL	0x300362 //FPK100每107 Interrupt Select Register Select ports used for FPK1 interrupts
#define REG_GPIO_FPK1_8F_SEL	0x300363 //FPK108每115 Interrupt Select Register
#define REG_GPIO_FPK01_POL	0x300370 //FPK0每1 Interrupt Polarity Select Register Select input signal polarity for FPK0每1 interrupts
#define REG_GPIO_FPK01_MOD	0x300371 //FPK0每1 Interrupt Mode Select Register Select edge/level mode for FPK0每1 interrupts
#define REG_GPIO_FPK01_MSK	0x300372 //FPK0每1 Interrupt Mask Register Enable/disable FPK0每1 interrupts
#define REG_GPIO_FPK01_FLG	0x300373 //FPK0每1 Interrupt Flag Register Indicate FPK0每1 interrupt cause status
#define REG_GPIO_FPK01_CHAT	0x300374 //FPK0每1 Interrupt Chattering Filter Control Register Control FPK0每1 chattering filter
#define REG_PMUX_P0_03	0x300800 //P0[3:0] Port Function Select Register Select P0[3:0] port functions
#define REG_PMUX_P0_47	0x300801 //P0[7:4] Port Function Select Register Select P0[7:4] port functions
#define REG_PMUX_P1_03	0x300802 //P1[3:0] Port Function Select Register Select P1[3:0] port functions
#define REG_PMUX_P1_47	0x300803 //P1[7:4] Port Function Select Register Select P1[7:4] port functions
#define REG_PMUX_P2_01	0x300804 //P2[1:0] Port Function Select Register Select P2[1:0] port functions
#define REG_PMUX_P3_03	0x300806 //P3[3:0] Port Function Select Register Select P3[3:0] port functions
#define REG_PMUX_P3_46	0x300807 //P3[6:4] Port Function Select Register Select P3[6:4] port functions
#define REG_PMUX_P4_02	0x300808 //P4[2:0] Port Function Select Register Select P4[2:0] port functions
#define REG_PMUX_P5_03	0x30080a //P5[3:0] Port Function Select Register Select P5[3:0] port functions
#define REG_PMUX_P5_46	0x30080b //P5[6:4] Port Function Select Register Select P5[6:4] port functions
#define REG_PMUX_P6_0	0x30080c //P60 Port Function Select Register Select P60 port function
#define REG_PMUX_P7_03	0x30080e //P7[3:0] Port Function Select Register Select P7[3:0] port functions
#define REG_PMUX_P7_47	0x30080f //P7[7:4] Port Function Select Register Select P7[7:4] port functions
#define REG_PMUX_P8_03	0x300810 //P8[3:0] Port Function Select Register Select P8[3:0] port functions
#define REG_PMUX_P9_03	0x300812 //P9[3:0] Port Function Select Register Select P9[3:0] port functions
#define REG_PMUX_P9_47	0x300813 //P9[7:4] Port Function Select Register Select P9[7:4] port functions
#define REG_PMUX_PA_03	0x300814 //PA[3:0] Port Function Select Register Select PA[3:0] port functions
#define REG_PMUX_PA_47	0x300815 //PA[7:4] Port Function Select Register Select PA[7:4] port functions
#define REG_PMUX_PB_03	0x300816 //PB[3:0] Port Function Select Register Select PB[3:0] port functions
#define REG_PMUX_PB_47	0x300817 //PB[7:4] Port Function Select Register Select PB[7:4] port functions
#define REG_PMUX_PC_03	0x300818 //PC[3:0] Port Function Select Register Select PC[3:0] port functions
#define REG_PMUX_PC_47	0x300819 //PC[7:4] Port Function Select Register Select PC[7:4] port functions
#define REG_PMUX_PD_03	0x30081a //PD[3:0] Port Function Select Register Select PD[3:0] port functions
#define REG_PMUX_PD_47	0x30081b //PD[7:4] Port Function Select Register Select PD[7:4] port functions
#define REG_PMUX_PE_03	0x30081c //PE[3:0] Port Function Select Register Select PE[3:0] port functions
#define REG_PMUX_PE_45	0x30081d //PE[5:4] Port Function Select Register Select PE[5:4] port functions
#define REG_PMUX_PF_03	0x30081e //PF[3:0] Port Function Select Register Select PF[3:0] port functions
#define REG_PMUX_PF_4	0x30081f //PF4 Port Function Select Register Select PF4 port function
#define REG_GPIO_P0_PUP	0x300820 //P0 Port Pull-up Control Register Enable/disable P0 port pull-up resistors
#define REG_GPIO_P1_PUP	0x300821 //P1 Port Pull-up Control Register Enable/disable P1 port pull-up resistors
#define REG_GPIO_P2_PUP	0x300822 //P2 Port Pull-up Control Register Enable/disable P2 port pull-up resistors
#define REG_GPIO_P3_PUP	0x300823 //P3 Port Pull-up Control Register Enable/disable P3 port pull-up resistors
#define REG_GPIO_P4_PUP	0x300824 //P4 Port Pull-up Control Register Enable/disable P4 port pull-up resistors
#define REG_GPIO_P5_PUP	0x300825 //P5 Port Pull-up Control Register Enable/disable P5 port pull-up resistors
#define REG_GPIO_P6_PUP	0x300826 //P6 Port Pull-up Control Register Enable/disable P6 port pull-up resistors
#define REG_GPIO_P7_PUP	0x300827 //P7 Port Pull-up Control Register Enable/disable P7 port pull-up resistors
#define REG_GPIO_P8_PUP	0x300828 //P8 Port Pull-up Control Register Enable/disable P8 port pull-up resistors
#define REG_GPIO_P9_PUP	0x300829 //P9 Port Pull-up Control Register Enable/disable P9 port pull-up resistors
#define REG_GPIO_PA_PUP	0x30082a //PA Port Pull-up Control Register Enable/disable PA port pull-up resistors
#define REG_GPIO_PB_PUP	0x30082b //PB Port Pull-up Control Register Enable/disable PB port pull-up resistors
#define REG_GPIO_PD_PUP	0x30082d //PD Port Pull-up Control Register Enable/disable PD port pull-up resistors
#define REG_GPIO_PE_PUP	0x30082e //PE Port Pull-up Control Register Enable/disable PE port pull-up resistors
#define REG_GPIO_PF_PUP	0x30082f //PF Port Pull-up Control Register Enable/disable PF port pull-up resistors
#define REG_GPIO_BUS_DRV	0x300830 //Bus Drive Control Register Set external data and address bus signals to low
#define REG_GPIO_FILTER	0x30083e //Port Noise Filter Control Register Enable/disable port input noise filter
#define REG_GPIO_PROTECT	0x30083f //GPIO/PMUX Write Protect Register Enable/disable write protection for PMUX, GPIO_BUS_DRV, and GPIO_Px_PUP registers

/*
USI Ch.0
(8-bit device)
*/
#define REG_USI_GCFG0	0x300400 //USI Ch.0 Global Configuration Register Set interface and MSB/LSB modes
#define REG_USI_TD0	0x300401 //USI Ch.0 Transmit Data Buffer Register Transmit data buffer
#define REG_USI_RD0	0x300402 //USI Ch.0 Receive Data Buffer Register Receive data buffer
#define REG_USI_UCFG0	0x300440 //USI Ch.0 UART Mode Configuration Register Set UART transfer conditions
#define REG_USI_UIE0	0x300441 //USI Ch.0 UART Mode Interrupt Enable Register Enable/disable UART interrupts
#define REG_USI_UIF0	0x300442 //USI Ch.0 UART Mode Interrupt Flag Register Indicate UART interrupt cause status
#define REG_USI_SCFG0	0x300450 //USI Ch.0 SPI Master/Slave Mode Configuration Register Set SPI transfer conditions
#define REG_USI_SIE0	0x300451 //USI Ch.0 SPI Master/Slave Mode Interrupt Enable Register Enable/disable SPI interrupts
#define REG_USI_SIF0	0x300452 //USI Ch.0 SPI Master/Slave Mode Interrupt Flag Register Indicate SPI interrupt cause status
#define REG_USI_SMSK0	0x30045f //USI Ch.0 SPI Master/Slave Mode Receive Data Mask Register Set SPI receive data mask
#define REG_USI_IMTG0	0x300460 //USI Ch.0 I2C Master Mode Trigger Register Start I2C master operations
#define REG_USI_IMIE0	0x300461 //USI Ch.0 I2C Master Mode Interrupt Enable Register Enable/disable I2C master interrupts
#define REG_USI_IMIF0	0x300462 //USI Ch.0 I2C Master Mode Interrupt Flag Register Indicate I2C master interrupt cause status
#define REG_USI_ISTG0	0x300470 //USI Ch.0 I2C Slave Mode Trigger Register Start I2C slave operations
#define REG_USI_ISIE0	0x300471 //USI Ch.0 I2C Slave Mode Interrupt Enable Register Enable/disable I2C slave interrupts
#define REG_USI_ISIF0	0x300472 //USI Ch.0 I2C Slave Mode Interrupt Flag Register Indicate I2C slave interrupt cause status
#define USIBIT_LSBFST	3
#define USIBIT_USIMOD	0
#define USIBIT_UCHLN	3
#define USIBIT_USTPB	2
#define USIBIT_UPMD	1
#define USIBIT_UPREN	0
#define USIBIT_UEIE	2
#define USIBIT_URDIE	1
#define USIBIT_UTDIE	0
#define USIBIT_URBSY	6
#define USIBIT_UTBSY	5
#define USIBIT_UPEIF	4
#define USIBIT_USEIF	3
#define USIBIT_UOEIF	2
#define USIBIT_URDIF	1
#define USIBIT_UTDIF	0

/*
USI Ch.1
(8-bit device)
*/
#define REG_USI_GCFG1	0x300500 //USI Ch.1 Global Configuration Register Set interface and MSB/LSB modes
#define REG_USI_TD1	0x300501 //USI Ch.1 Transmit Data Buffer Register Transmit data buffer
#define REG_USI_RD1	0x300502 //USI Ch.1 Receive Data Buffer Register Receive data buffer
#define REG_USI_UCFG1	0x300540 //USI Ch.1 UART Mode Configuration Register Set UART transfer conditions
#define REG_USI_UIE1	0x300541 //USI Ch.1 UART Mode Interrupt Enable Register Enable/disable UART interrupts
#define REG_USI_UIF1	0x300542 //USI Ch.1 UART Mode Interrupt Flag Register Indicate UART interrupt cause status
#define REG_USI_SCFG1	0x300550 //USI Ch.1 SPI Master/Slave Mode Configuration Register Set SPI transfer conditions
#define REG_USI_SIE1	0x300551 //USI Ch.1 SPI Master/Slave Mode Interrupt Enable Register Enable/disable SPI interrupts
#define REG_USI_SIF1	0x300552 //USI Ch.1 SPI Master/Slave Mode Interrupt Flag Register Indicate SPI interrupt cause status
#define REG_USI_SMSK1	0x30055f //USI Ch.1 SPI Master/Slave Mode Receive Data Mask Register Set SPI receive data mask
#define REG_USI_IMTG1	0x300560 //USI Ch.1 I2C Master Mode Trigger Register Start I2C master operations
#define REG_USI_IMIE1	0x300561 //USI Ch.1 I2C Master Mode Interrupt Enable Register Enable/disable I2C master interrupts
#define REG_USI_IMIF1	0x300562 //USI Ch.1 I2C Master Mode Interrupt Flag Register Indicate I2C master interrupt cause status
#define REG_USI_ISTG1	0x300570 //USI Ch.1 I2C Slave Mode Trigger Register Start I2C slave operations
#define REG_USI_ISIE1	0x300571 //USI Ch.1 I2C Slave Mode Interrupt Enable Register Enable/disable I2C slave interrupts
#define REG_USI_ISIF1	0x300572 //USI Ch.1 I2C Slave Mode Interrupt Flag Register Indicate I2C slave interrupt cause status

/*
USIL
(8-bit device)
*/
#define REG_USIL_GCFG	0x300600 //USIL Global Configuration Register Set interface and MSB/LSB modes
#define REG_USIL_TD	0x300601 //USIL Transmit Data Buffer Register Transmit data buffer
#define REG_USIL_RD	0x300602 //USIL Receive Data Buffer Register Receive data buffer
#define REG_USIL_UCFG	0x300640 //USIL UART Mode Configuration Register Set UART transfer conditions
#define REG_USIL_UIE	0x300641 //USIL UART Mode Interrupt Enable Register Enable/disable UART interrupts
#define REG_USIL_UIF	0x300642 //USIL UART Mode Interrupt Flag Register Indicate UART interrupt cause status
#define REG_USIL_SCFG	0x300650 //USIL SPI Master/Slave Mode Configuration Register Set SPI transfer conditions
#define REG_USIL_SIE	0x300651 //USIL SPI Master/Slave Mode Interrupt Enable Register Enable/disable SPI interrupts
#define REG_USIL_SIF	0x300652 //USIL SPI Master/Slave Mode Interrupt Flag Register Indicate SPI interrupt cause status
#define REG_USIL_SMSK	0x30065f //USIL SPI Master/Slave Mode Receive Data Mask Register Set SPI receive data mask
#define REG_USIL_IMTG	0x300660 //USIL I2C Master Mode Trigger Register Start I2C master operations
#define REG_USIL_IMIE	0x300661 //USIL I2C Master Mode Interrupt Enable Register Enable/disable I2C master interrupts
#define REG_USIL_IMIF	0x300662 //USIL I2C Master Mode Interrupt Flag Register Indicate I2C master interrupt cause status
#define REG_USIL_ISTG	0x300670 //USIL I2C Slave Mode Trigger Register Start I2C slave operations
#define REG_USIL_ISIE	0x300671 //USIL I2C Slave Mode Interrupt Enable Register Enable/disable I2C slave interrupts
#define REG_USIL_ISIF	0x300672 //USIL I2C Slave Mode Interrupt Flag Register Indicate I2C slave interrupt cause status
#define REG_USIL_LSCFG	0x300680 //USIL LCD SPI Mode Configuration Register Set LCD SPI transfer conditions
#define REG_USIL_LSIE	0x300681 //USIL LCD SPI Mode Interrupt Enable Register Enable/disable LCD SPI interrupts
#define REG_USIL_LSIF	0x300682 //USIL LCD SPI Mode Interrupt Flag Register Indicate LCD SPI interrupt cause status
#define REG_USIL_LSDCFG	0x30068f //USIL LCD SPI Mode Data Configuration Register Select display data format
#define REG_USIL_LPCFG	0x300690 //USIL LCD Parallel I/F Mode Configuration Register Set LCD parallel interface conditions
#define REG_USIL_LPIE	0x300691 //USIL LCD Parallel I/F Mode Interrupt Enable Register Enable/disable LCD parallel interface interrupts
#define REG_USIL_LPIF	0x300692 //USIL LCD Parallel I/F Mode Interrupt Flag Register Indicate LCD parallel interface interrupt cause status
#define REG_USIL_LPAC	0x30069f //USIL LCD Parallel I/F Mode Access Timing Register Set LCD parallel interface access timing parameters

/*
USI Ch.2
(8-bit device)
*/
#define REG_USI_GCFG2	0x300700 //USI Ch.2 Global Configuration Register Set interface and MSB/LSB modes
#define REG_USI_TD2	0x300701 //USI Ch.2 Transmit Data Buffer Register Transmit data buffer
#define REG_USI_RD2	0x300702 //USI Ch.2 Receive Data Buffer Register Receive data buffer
#define REG_USI_UCFG2	0x300740 //USI Ch.2 UART Mode Configuration Register Set UART transfer conditions
#define REG_USI_UIE2	0x300741 //USI Ch.2 UART Mode Interrupt Enable Register Enable/disable UART interrupts
#define REG_USI_UIF2	0x300742 //USI Ch.2 UART Mode Interrupt Flag Register Indicate UART interrupt cause status
#define REG_USI_SCFG2	0x300750 //USI Ch.2 SPI Master/Slave Mode Configuration Register Set SPI transfer conditions
#define REG_USI_SIE2	0x300751 //USI Ch.2 SPI Master/Slave Mode Interrupt Enable Register Enable/disable SPI interrupts
#define REG_USI_SIF2	0x300752 //USI Ch.2 SPI Master/Slave Mode Interrupt Flag Register Indicate SPI interrupt cause status
#define REG_USI_SMSK2	0x30075f //USI Ch.2 SPI Master/Slave Mode Receive Data Mask Register Set SPI receive data mask
#define REG_USI_IMTG2	0x300760 //USI Ch.2 I2C Master Mode Trigger Register Start I2C master operations
#define REG_USI_IMIE2	0x300761 //USI Ch.2 I2C Master Mode Interrupt Enable Register Enable/disable I2C master interrupts
#define REG_USI_IMIF2	0x300762 //USI Ch.2 I2C Master Mode Interrupt Flag Register Indicate I2C master interrupt cause status
#define REG_USI_ISTG2	0x300770 //USI Ch.2 I2C Slave Mode Trigger Register Start I2C slave operations
#define REG_USI_ISIE2	0x300771 //USI Ch.2 I2C Slave Mode Interrupt Enable Register Enable/disable I2C slave interrupts
#define REG_USI_ISIF2	0x300772 //USI Ch.2 I2C Slave Mode Interrupt Flag Register Indicate I2C slave interrupt cause status

/*
Host Interface
(HIF)
(8- or 16-bit device)
*/

/*
Real-Time
Clock (RTC)
(8-bit device)
*/
#define REG_RTC_INTSTAT	0x300a00 //RTC Interrupt Status Register Indicate RTC interrupt cause status
#define REG_RTC_INTMODE	0x300a01 //RTC Interrupt Mode Register Set RTC interrupt modes
#define REG_RTC_CNTL0	0x300a02 //RTC Control 0 Register Control RTC
#define REG_RTC_CNTL1	0x300a03 //RTC Control 1 Register
#define REG_RTC_SEC	0x300a04 //RTC Second Register Second counter data
#define REG_RTC_MIN	0x300a05 //RTC Minute Register Minute counter data
#define REG_RTC_HOUR	0x300a06 //RTC Hour Register Hour counter data
#define REG_RTC_DAY	0x300a07 //RTC Day Register Day counter data
#define REG_RTC_MONTH	0x300a08 //RTC Month Register Month counter data
#define REG_RTC_YEAR	0x300a09 //RTC Year Register Year counter data
#define REG_RTC_WEEK	0x300a0a //RTC Days of Week Register Days of week counter data
#define REG_RTC_WAKEUP	0x300a0f //RTC Wakeup Configuration Register Set RTC wakeup conditions
#define RTCBIT_RTCIRQ	0
#define RTCBIT_RTCT	2
#define RTCBIT_RTCIMD	1
#define RTCBIT_RTCIEN	0
#define RTCBIT_RTCRST	0
#define RTCBIT_RTCSTP	1
#define RTCBIT_RTCADJ	2
#define RTCBIT_RTC24H	4
#define RTCBIT_RTCHLD	0
#define RTCBIT_RTCBSY	1
#define RTCBIT_RTCRDHLD	2
#define RTCBIT_RTCSL	0
#define RTCBIT_RTCSH	4
#define RTCBIT_RTCMIL	0
#define RTCBIT_RTCMIH	4
#define RTCBIT_RTCHL	0
#define RTCBIT_RTCHH	4
#define RTCBIT_RTCAP	6
#define RTCBIT_RTCDL	0
#define RTCBIT_RTCDH	4
#define RTCBIT_RTCMOL	0
#define RTCBIT_RTCMOH	4
#define RTCBIT_RTCYL	0
#define RTCBIT_RTCYH	4
#define RTCBIT_RTCWK	0
#define RTCBIT_WUP_CTL	1
#define RTCBIT_WUP_POL	0

/*
BBRAM
(8-bit device)
*/
#define REG_BBRAM_0		0x300b00 //BBRAM byte 0
#define REG_BBRAM_1		0x300b00 //BBRAM byte 1
#define REG_BBRAM_2		0x300b00 //BBRAM byte 2
#define REG_BBRAM_3		0x300b00 //BBRAM byte 3
#define REG_BBRAM_4		0x300b00 //BBRAM byte 4
#define REG_BBRAM_5		0x300b00 //BBRAM byte 5
#define REG_BBRAM_6		0x300b00 //BBRAM byte 6
#define REG_BBRAM_7		0x300b00 //BBRAM byte 7
#define REG_BBRAM_8		0x300b00 //BBRAM byte 8
#define REG_BBRAM_9		0x300b00 //BBRAM byte 9
#define REG_BBRAM_10	0x300b00 //BBRAM byte 10
#define REG_BBRAM_11	0x300b00 //BBRAM byte 11
#define REG_BBRAM_12	0x300b00 //BBRAM byte 12
#define REG_BBRAM_13	0x300b00 //BBRAM byte 13
#define REG_BBRAM_14	0x300b00 //BBRAM byte 14
#define REG_BBRAM_15	0x300b00 //BBRAM byte 15

/*
USB function
controller
(USB)
(8-bit device)
*/
#define REG_MainIntStat	0x300c00 //Main Interrupt Status Register Indicate main interrupt status
#define REG_SIE_IntStat	0x300c01 //SIE Interrupt Status Register Indicate SIE interrupt status
#define REG_EPrIntStat	0x300c02 //EPr Interrupt Status Register Indicate EPr interrupt status
#define REG_DMA_IntStat	0x300c03 //DMA Interrupt Status Register Indicate DMA interrupt status
#define REG_FIFO_IntStat	0x300c04 //FIFO Interrupt Status Register Indicate FIFO interrupt status
#define REG_EP0IntStat	0x300c07 //EP0 Interrupt Status Register Indicate EP0 interrupt status
#define REG_EPaIntStat	0x300c08 //EPa Interrupt Status Register Indicate EPa interrupt status
#define REG_EPbIntStat	0x300c09 //EPb Interrupt Status Register Indicate EPb interrupt status
#define REG_EPcIntStat	0x300c0a //EPc Interrupt Status Register Indicate EPc interrupt status
#define REG_EPdIntStat	0x300c0b //EPd Interrupt Status Register Indicate EPd interrupt status
#define REG_MainIntEnb	0x300c10 //Main Interrupt Enable Register Enable main interrupts
#define REG_SIE_IntEnb	0x300c11 //SIE Interrupt Enable Register Enable SIE interrupts
#define REG_EPrIntEnb	0x300c12 //EPr Interrupt Enable Register Enable EPr interrupts
#define REG_DMA_IntEnb	0x300c13 //DMA Interrupt Enable Register Enable DMA interrupts
#define REG_FIFO_IntEnb	0x300c14 //FIFO Interrupt Enable Register Enable FIFO interrupts
#define REG_EP0IntEnb	0x300c17 //EP0 Interrupt Enable Register Enable EP0 interrupts
#define REG_EPaIntEnb	0x300c18 //EPa Interrupt Enable Register Enable EPa interrupts
#define REG_EPbIntEnb	0x300c19 //EPb Interrupt Enable Register Enable EPb interrupts
#define REG_EPcIntEnb	0x300c1a //EPc Interrupt Enable Register Enable EPc interrupts
#define REG_EPdIntEnb	0x300c1b //EPd Interrupt Enable Register Enable EPd interrupts
#define REG_RevisionNum	0x300c20 //Revision Number Register Indicate revision number of USB controller
#define REG_USB_Control	0x300c21 //USB Control Register Control USB conditions
#define REG_USB_Status	0x300c22 //USB Status Register Indicate USB status
#define REG_XcvrControl	0x300c23 //Xcvr Control Register Control transceiver macro
#define REG_USB_Test	0x300c24 //USB Test Register Set USB test mode
#define REG_EPnControl	0x300c25 //Endpoint Control Register Clear all FIFOs and set NAK/STALL bits
#define REG_EPrFIFO_Clr	0x300c26 //EPr FIFO Clear Register Clear each FIFO
#define REG_FrameNumber_H	0x300c2e //Frame Number High Register Frame number
#define REG_FrameNumber_L	0x300c2f //Frame Number Low Register
#define REG_EP0Setup_0	0x300c30 //EP0 Setup 0 Register EP0 setup data (BmRequestType)
#define REG_EP0Setup_1	0x300c31 //EP0 Setup 1 Register EP0 setup data (BRequest)
#define REG_EP0Setup_2	0x300c32 //EP0 Setup 2 Register EP0 setup data (low-order Wvalue bits)
#define REG_EP0Setup_3	0x300c33 //EP0 Setup 3 Register EP0 setup data (high-order Wvalue bits)
#define REG_EP0Setup_4	0x300c34 //EP0 Setup 4 Register EP0 setup data (low-order WIndex bits)
#define REG_EP0Setup_5	0x300c35 //EP0 Setup 5 Register EP0 setup data (high-order WIndex bits)
#define REG_EP0Setup_6	0x300c36 //EP0 Setup 6 Register EP0 setup data (low-order WLength bits)
#define REG_EP0Setup_7	0x300c37 //EP0 Setup 7 Register EP0 setup data (high-order WLength bits)
#define REG_USB_Address	0x300c38 //USB Address Register Set USB address
#define REG_EP0Control	0x300c39 //EP0 Control Register Set up EP0
#define REG_EP0ControlIN	0x300c3a //EP0 Control In Register Set EP0 IN transaction conditions
#define REG_EP0ControlOUT	0x300c3b //EP0 Control Out Register Set EP0 OUT transaction conditions
#define REG_EP0MaxSize	0x300c3f //EP0 Max Packet Size Register Set the EP0 max packet size
#define REG_EPaControl	0x300c40 //EPa Control Register Set up EPa
#define REG_EPbControl	0x300c41 //EPb Control Register Set up EPb
#define REG_EPcControl	0x300c42 //EPc Control Register Set up EPc
#define REG_EPdControl	0x300c43 //EPd Control Register Set up EPd
#define REG_EPaMaxSize_H	0x300c50 //EPa Max Packet Size High Register Set EPa max packet size
#define REG_EPaMaxSize_L	0x300c51 //EPa Max Packet Size Low Register
#define REG_EPaConfig_0	0x300c52 //EPa Configuration 0 Register Configure EPa
#define REG_EPaConfig_1	0x300c53 //EPa Configuration 1 Register
#define REG_EPbMaxSize_H	0x300c54 //EPb Max Packet Size High Register Set EPb max packet size
#define REG_EPbMaxSize_L	0x300c55 //EPb Max Packet Size Low Register
#define REG_EPbConfig_0	0x300c56 //EPb Configuration 0 Register Configure EPb
#define REG_EPbConfig_1	0x300c57 //EPb Configuration 1 Register
#define REG_EPcMaxSize_H	0x300c58 //EPc Max Packet Size High Register Set EPc max packet size
#define REG_EPcMaxSize_L	0x300c59 //EPc Max Packet Size Low Register
#define REG_EPcConfig_0	0x300c5a //EPc Configuration 0 Register Configure EPc
#define REG_EPcConfig_1	0x300c5b //EPc Configuration 1 Register
#define REG_EPdMaxSize_H	0x300c5c //EPd Max Packet Size High Register Set EPd max packet size
#define REG_EPdMaxSize_L	0x300c5d //EPd Max Packet Size Low Register
#define REG_EPdConfig_0	0x300c5e //EPd Configuration 0 Register Configure EPd
#define REG_EPdConfig_1	0x300c5f //EPd Configuration 1 Register
#define REG_EPaStartAdrs_H	0x300c70 //EPa FIFO Start Address High Register Set FIFO start address for EPa
#define REG_EPaStartAdrs_L	0x300c71 //EPa FIFO Start Address Low Register
#define REG_EPbStartAdrs_H	0x300c72 //EPb FIFO Start Address High Register Set FIFO start address for EPb
#define REG_EPbStartAdrs_L	0x300c73 //EPb FIFO Start Address Low Register
#define REG_EPcStartAdrs_H	0x300c74 //EPc FIFO Start Address High Register Set FIFO start address for EPc
#define REG_EPcStartAdrs_L	0x300c75 //EPc FIFO Start Address Low Register
#define REG_EPdStartAdrs_H	0x300c76 //EPd FIFO Start Address High Register Set FIFO start address for EPd
#define REG_EPdStartAdrs_L	0x300c77 //EPd FIFO Start Address Low Register
#define REG_CPU_JoinRd	0x300c80 //CPU Join FIFO Read Register Set up FIFO data read conditions
#define REG_CPU_JoinWr	0x300c81 //CPU Join FIFO Write Register Set up FIFO data write conditions
#define REG_EnEPnFIFO	0x300c82 //_Access EPn FIFO Access Enable Register Enable CPU_JoinRd and CPU_JoinWr registers
#define REG_EPnFIFOforCPU	0x300c83 //EPn FIFO for CPU Register EPn FIFO for accessing by CPU
#define REG_EPnRdRemain_H	0x300c84 //EPn FIFO Read Remain High Register Indicate remained data quantity in FIFO
#define REG_EPnRdRemain_L	0x300c85 //EPn FIFO Read Remain Low Register
#define REG_EPnWrRemain_H	0x300c86 //EPn FIFO Write High Register Indicate free space capacity in FIFO
#define REG_EPnWrRemain_L	0x300c87 //EPn FIFO Write Low Register
#define REG_DescAdrs_H	0x300c88 //Descriptor Address High Register Specify FIFO start address for the descriptor
#define REG_DescAdrs_L	0x300c89 //Descriptor Address Low Register reply function
#define REG_DescSize_H	0x300c8a //Descriptor Size High Register Specify number of data for the descriptor reply
#define REG_DescSize_L	0x300c8b //Descriptor Size Low Register function
#define REG_DescDoor	0x300c8f //Descriptor Door Register Read/write descriptors
#define REG_DMA_FIFO_Control	0x300c90 //DMA FIFO Control Register Control DMA FIFO
#define REG_DMA_Join	0x300c91 //DMA Join FIFO Enable endpoint to perform DMA transfer
#define REG_DMA_Control	0x300c92 //DMA Control Register Control DMA transfer and indicate DMA status
#define REG_DMA_Config_0	0x300c94 //DMA Configuration 0 Register Configure DMA interface signals
#define REG_DMA_Config_1	0x300c95 //DMA Configuration 1 Register Set DMA interface operating modes
#define REG_DMA_Latency	0x300c97 //DMA Latency Register Set data transfer latency
#define REG_DMA_Remain_H	0x300c98 //DMA FIFO Remain High Register Indicate remained data quantity in FIFO or free
#define REG_DMA_Remain_L	0x300c99 //DMA FIFO Remain Low Register space capacity in FIFO
#define REG_DMA_Count_HH	0x300c9c //DMA Transfer Byte Counter High/High Register Specify/indicate data length in DMA transfer
#define REG_DMA_Count_HL	0x300c9d //DMA Transfer Byte Counter High/Low Register
#define REG_DMA_Count_LH	0x300c9e //DMA Transfer Byte Counter Low/High Register
#define REG_DMA_Count_LL	0x300c9f //DMA Transfer Byte Counter Low/Low Register

/*
Prescaler
(PSC)
(8-bit device)
*/
#define REG_PSC_CTL0	0x300e00 //PSC Ch.0 Control Register Control prescaler Ch.0
#define REG_PSC_CTL12	0x300e01 //PSC Ch.1/2 Control Register Control prescalers Ch.1 and 2
#define PSCBIT_PRUND	1
#define PSCBIT_PRUN	0

/*
UART
(with IrDA)
(8-bit device)
*/
#define REG_UART_ST	0x300e10 //UART Status Register Indicates transfer, buffer and error statuses.
#define REG_UART_TXD	0x300e11 //UART Transmit Data Register Transmit data
#define REG_UART_RXD	0x300e12 //UART Receive Data Register Receive data
#define REG_UART_MOD	0x300e13 //UART Mode Register Sets transfer data format.
#define REG_UART_CTL	0x300e14 //UART Control Register Controls data transfer.
#define REG_UART_EXP	0x300e15 //UART Expansion Register Sets IrDA mode.
#define UARTBIT_FER	6
#define UARTBIT_PER	5
#define UARTBIT_OER	4
#define UARTBIT_RD2B	3
#define UARTBIT_TRBS	2
#define UARTBIT_RDRY	1
#define UARTBIT_TDBE	0
#define UARTBIT_CHLN	4
#define UARTBIT_PREN	3
#define UARTBIT_PMD	2
#define UARTBIT_STPB	1
#define UARTBIT_SSCK	0
#define UARTBIT_REIEN	6
#define UARTBIT_RIEN	5
#define UARTBIT_TIEN	4
#define UARTBIT_RBFI	1
#define UARTBIT_RXEN	0
#define UARTBIT_IRCLK	4
#define UARTBIT_IRMD	0

/*
Card interface
(CARD)
(8-bit device)
*/
#define REG_CARD_ECC_TRG	0x300f00 //ECC Trigger Register Resets the hardware accelerator registers and terminates 10-symbol ECC.
#define REG_CARD_ECC_CTRL	0x300f01 //ECC Control Register Sets the operating mode and enables operation.
#define REG_CARD_ECC_RGN	0x300f02 //ECC Region Register Selects the card area.
#define REG_CARD_ECC_FLG	0x300f10 //ECC Flag Register Indicates error detection status.
#define REG_CARD_ECC10_COD0	0x300f20 //10-Symbol ECC Code Register 0 10-symbol ECC RS code
#define REG_CARD_ECC10_COD1	0x300f21 //10-Symbol ECC Code Register 1
#define REG_CARD_ECC10_COD2	0x300f22 //10-Symbol ECC Code Register 2
#define REG_CARD_ECC10_COD3	0x300f23 //10-Symbol ECC Code Register 3
#define REG_CARD_ECC10_COD4	0x300f24 //10-Symbol ECC Code Register 4
#define REG_CARD_ECC10_COD5	0x300f25 //10-Symbol ECC Code Register 5
#define REG_CARD_ECC10_COD6	0x300f26 //10-Symbol ECC Code Register 6
#define REG_CARD_ECC10_COD7	0x300f27 //10-Symbol ECC Code Register 7
#define REG_CARD_ECC10_COD8	0x300f28 //10-Symbol ECC Code Register 8
#define REG_CARD_ECC10_COD9	0x300f29 //10-Symbol ECC Code Register 9
#define REG_CARD_ECC10_COD10	0x300f2a //10-Symbol ECC Code Register 10
#define REG_CARD_ECC10_COD11	0x300f2b //10-Symbol ECC Code Register 11
#define REG_CARD_ECC10_COD12	0x300f2c //10-Symbol ECC Code Register 12
#define REG_CARD_ECC10_COD13	0x300f2d //10-Symbol ECC Code Register 13
#define REG_CARD_ECC10_COD14	0x300f2e //10-Symbol ECC Code Register 14
#define REG_CARD_ECC10_COD15	0x300f2f //10-Symbol ECC Code Register 15
#define REG_CARD_ECC10_COD16	0x300f30 //10-Symbol ECC Code Register 16
#define REG_CARD_ECC10_COD17	0x300f31 //10-Symbol ECC Code Register 17
#define REG_CARD_ECC10_COD18	0x300f32 //10-Symbol ECC Code Register 18
#define REG_CARD_ECC10_COD19	0x300f33 //10-Symbol ECC Code Register 19
#define REG_CARD_ECC10_COD20	0x300f34 //10-Symbol ECC Code Register 20
#define REG_CARD_ECC10_COD21	0x300f35 //10-Symbol ECC Code Register 21
#define REG_CARD_ECC10_COD22	0x300f36 //10-Symbol ECC Code Register 22
#define REG_CARD_ECC10_COD23	0x300f37 //10-Symbol ECC Code Register 23
#define REG_CARD_ECC10_COD24	0x300f38 //10-Symbol ECC Code Register 24
#define REG_CARD_ECC2_COD0	0x300f40 //2-Symbol ECC Code Register 0 2-symbol ECC RS code
#define REG_CARD_ECC2_COD1	0x300f41 //2-Symbol ECC Code Register 1 2-symbol ECC RS code
#define REG_CARD_ECC2_COD2	0x300f42 //2-Symbol ECC Code Register 2 2-symbol ECC RS code
#define REG_CARD_ECC2_COD3	0x300f43 //2-Symbol ECC Code Register 3 2-symbol ECC RS code
#define CARDBIT_END	1
#define CARDBIT_RST	0
#define CARDBIT_SLT	2
#define CARDBIT_MOD	1
#define CARDBIT_EN	0
#define CARDBIT_RGN	0
#define CARDBIT_BSY	4
#define CARDBIT_ERR	0

/*
Watchdog timer
(WDT)
(16-bit device)
*/
#define REG_WD_PROTECT	0x301000 //WDT Write Protect Register Enable WDT register write protection
#define REG_WD_EN	0x301002 //WDT Enable and Setup Register Configure and start watchdog timer
#define REG_WD_CMP_L	0x301004 //WDT Comparison Data L Register Comparison data
#define REG_WD_CMP_H	0x301006 //WDT Comparison Data H Register
#define REG_WD_CNT_L	0x301008 //WDT Count Data L Register Watchdog timer counter data
#define REG_WD_CNT_H	0x30100a //WDT Count Data H Register
#define REG_WD_CTL	0x30100c //WDT Control Register Reset watchdog timer

/*
Fine mode 8-bit
timer (T8F)
Ch.0
(16-bit device)
*/
#define REG_T8F_CLK0	0x301100 //T8F Ch.0 Input Clock Select Register Select prescaler output clock
#define REG_T8F_TR0	0x301102 //T8F Ch.0 Reload Data Register Set reload data
#define REG_T8F_TC0	0x301104 //T8F Ch.0 Counter Data Register Counter data
#define REG_T8F_CTL0	0x301106 //T8F Ch.0 Control Register Set timer mode and start/stop timer
#define REG_T8F_INT0	0x301108 //T8F Ch.0 Interrupt Control Register Control interrupt
/*
Fine mode 8-bit
timer (T8F)
Ch.1
(16-bit device)
*/
#define REG_T8F_CLK1	0x301110 //T8F Ch.1 Input Clock Select Register Select prescaler output clock
#define REG_T8F_TR1	0x301112 //T8F Ch.1 Reload Data Register Set reload data
#define REG_T8F_TC1	0x301114 //T8F Ch.1 Counter Data Register Counter data
#define REG_T8F_CTL1	0x301116 //T8F Ch.1 Control Register Set timer mode and start/stop timer
#define REG_T8F_INT1	0x301118 //T8F Ch.1 Interrupt Control Register Control interrupt
/*
Fine mode 8-bit
timer (T8F)
Ch.2
(16-bit device)
*/
#define REG_T8F_CLK2	0x301120 //T8F Ch.2 Input Clock Select Register Select prescaler output clock
#define REG_T8F_TR2	0x301122 //T8F Ch.2 Reload Data Register Set reload data
#define REG_T8F_TC2	0x301124 //T8F Ch.2 Counter Data Register Counter data
#define REG_T8F_CTL2	0x301126 //T8F Ch.2 Control Register Set timer mode and start/stop timer
#define REG_T8F_INT2	0x301128 //T8F Ch.2 Interrupt Control Register Control interrupt
/*
Fine mode 8-bit
timer (T8F)
Ch.3
(16-bit device)
*/
#define REG_T8F_CLK3	0x301130 //T8F Ch.3 Input Clock Select Register Select prescaler output clock
#define REG_T8F_TR3	0x301132 //T8F Ch.3 Reload Data Register Set reload data
#define REG_T8F_TC3	0x301134 //T8F Ch.3 Counter Data Register Counter data
#define REG_T8F_CTL3	0x301136 //T8F Ch.3 Control Register Set timer mode and start/stop timer
#define REG_T8F_INT3	0x301138 //T8F Ch.3 Interrupt Control Register Control interrupt
/*
Fine mode 8-bit
timer (T8F)
Ch.4
(16-bit device)
*/
#define REG_T8F_CLK4	0x301140 //T8F Ch.4 Input Clock Select Register Select prescaler output clock
#define REG_T8F_TR4	0x301142 //T8F Ch.4 Reload Data Register Set reload data
#define REG_T8F_TC4	0x301144 //T8F Ch.4 Counter Data Register Counter data
#define REG_T8F_CTL4	0x301146 //T8F Ch.4 Control Register Set timer mode and start/stop timer
#define REG_T8F_INT4	0x301148 //T8F Ch.4 Interrupt Control Register Control interrupt
/*
Fine mode 8-bit
timer (T8F)
Ch.5
(16-bit device)
*/
#define REG_T8F_CLK5	0x301150 //T8F Ch.5 Input Clock Select Register Select prescaler output clock
#define REG_T8F_TR5	0x301152 //T8F Ch.5 Reload Data Register Set reload data
#define REG_T8F_TC5	0x301154 //T8F Ch.5 Counter Data Register Counter data
#define REG_T8F_CTL5	0x301156 //T8F Ch.5 Control Register Set timer mode and start/stop timer
#define REG_T8F_INT5	0x301158 //T8F Ch.5 Interrupt Control Register Control interrupt
#define T8FBIT_DF	0
#define T8FBIT_TFMD	8
#define T8FBIT_TRMD	4
#define T8FBIT_PRESER	1
#define T8FBIT_PRUN	0
#define T8FBIT_T8FIE	8
#define T8FBIT_T8FIF	0

/*
16-bit PWM
timer (T16A6)
Ch.0
(16-bit device)
*/
#define REG_T16A_CTL0	0x301160 //T16A6 Ch.0 Counter Control Register Control counter
#define REG_T16A_TC0	0x301162 //T16A6 Ch.0 Counter Data Register Counter data
#define REG_T16A_CCCTL0	0x301164 //T16A6 Ch.0 Comparator/Capture Control Register Control comparator/capture block and TOUT
#define REG_T16A_CCA0	0x301166 //T16A6 Ch.0 Comparator/Capture A Data Register Compare A/capture A data
#define REG_T16A_CCB0	0x301168 //T16A6 Ch.0 Comparator/Capture B Data Register Compare B/capture B data
#define REG_T16A_IEN0	0x30116a //T16A6 Ch.0 Comparator/Capture Interrupt Enable Register Enable/disable T16A6 interrupts
#define REG_T16A_IFLG0	0x30116c //T16A6 Ch.0 Comparator/Capture Interrupt Flag Register Indicate T16A6 interrupt cause status
#define T16ABIT_DMASEL	12
#define T16ABIT_CLKS	8
#define T16ABIT_BUSY	7
#define T16ABIT_T16SEL	4
#define T16ABIT_CBUFEN	3
#define T16ABIT_TMMD	2
#define T16ABIT_PRESET	1
#define T16ABIT_PRUN	0
#define T16ABIT_CAPBTRG	14
#define T16ABIT_TOUTBMD	12
#define T16ABIT_TOUTBINV	9
#define T16ABIT_CCBMD	8
#define T16ABIT_CAPATRG	6
#define T16ABIT_TOUTAMD	4
#define T16ABIT_TOUTAINV	1
#define T16ABIT_CCAMD	0
#define T16ABIT_CAPBOWIE	5
#define T16ABIT_CAPAOWIE	4
#define T16ABIT_CAPBIE	3
#define T16ABIT_CAPAIE	2
#define T16ABIT_CBIE	1
#define T16ABIT_CAIE	0
#define T16ABIT_CAPBOWIF	5
#define T16ABIT_CAPAOWIF	4
#define T16ABIT_CAPBIF	3
#define T16ABIT_CAPAIF	2
#define T16ABIT_CBIF	1
#define T16ABIT_CAIF	0

/*
16-bit PWM
timer (T16A6)
Ch.1
(16-bit device)
*/
#define REG_T16A_CTL1	0x301170 //T16A6 Ch.1 Counter Control Register Control counter
#define REG_T16A_TC1	0x301172 //T16A6 Ch.1 Counter Data Register Counter data
#define REG_T16A_CCCTL1	0x301174 //T16A6 Ch.1 Comparator/Capture Control Register Control comparator/capture block and TOUT
#define REG_T16A_CCA1	0x301176 //T16A6 Ch.1 Comparator/Capture A Data Register Compare A/capture A data
#define REG_T16A_CCB1	0x301178 //T16A6 Ch.1 Comparator/Capture B Data Register Compare B/capture B data
#define REG_T16A_IEN1	0x30117a //T16A6 Ch.1 Comparator/Capture Interrupt Enable Register Enable/disable T16A6 interrupts
#define REG_T16A_IFLG1	0x30117c //T16A6 Ch.1 Comparator/Capture Interrupt Flag Register Indicate T16A6 interrupt cause status

/*
16-bit PWM
timer (T16A6)
Ch.2
(16-bit device)
*/
#define REG_T16A_CTL2	0x301180 //T16A6 Ch.2 Counter Control Register Control counter
#define REG_T16A_TC2	0x301182 //T16A6 Ch.2 Counter Data Register Counter data
#define REG_T16A_CCCTL2	0x301184 //T16A6 Ch.2 Comparator/Capture Control Register Control comparator/capture block and TOUT
#define REG_T16A_CCA2	0x301186 //T16A6 Ch.2 Comparator/Capture A Data Register Compare A/capture A data
#define REG_T16A_CCB2	0x301188 //T16A6 Ch.2 Comparator/Capture B Data Register Compare B/capture B data
#define REG_T16A_IEN2	0x30118a //T16A6 Ch.2 Comparator/Capture Interrupt Enable Register Enable/disable T16A6 interrupts
#define REG_T16A_IFLG2	0x30118c //T16A6 Ch.2 Comparator/Capture Interrupt Flag Register Indicate T16A6 interrupt cause status

/*
16-bit PWM
timer (T16A6)
Ch.3
(16-bit device)
*/
#define REG_T16A_CTL3	0x301190 //T16A6 Ch.3 Counter Control Register Control counter
#define REG_T16A_TC3	0x301192 //T16A6 Ch.3 Counter Data Register Counter data
#define REG_T16A_CCCTL3	0x301194 //T16A6 Ch.3 Comparator/Capture Control Register Control comparator/capture block and TOUT
#define REG_T16A_CCA3	0x301196 //T16A6 Ch.3 Comparator/Capture A Data Register Compare A/capture A data
#define REG_T16A_CCB3	0x301198 //T16A6 Ch.3 Comparator/Capture B Data Register Compare B/capture B data
#define REG_T16A_IEN3	0x30119a //T16A6 Ch.3 Comparator/Capture Interrupt Enable Register Enable/disable T16A6 interrupts
#define REG_T16A_IFLG3	0x30119c //T16A6 Ch.3 Comparator/Capture Interrupt Flag Register Indicate T16A6 interrupt cause status


/*
16-bit audio
PWM timer
(T16P) Ch.0
(16-bit device)
*/
#define REG_T16P_A0	0x301200 //T16P Ch.0 Compare A Buffer Register Compare A data
#define REG_T16P_B0	0x301202 //T16P Ch.0 Compare B Buffer Register Compare B data
#define REG_T16P_CNT_DATA0	0x301204 //T16P Ch.0 Counter Data Register Counter data
#define REG_T16P_VOL_CTL0	0x301206 //T16P Ch.0 Volume Control Register Enable volume control and set volume level
#define REG_T16P_CTL0	0x301208 //T16P Ch.0 Control Register Set timer operating conditions
#define REG_T16P_RUN0	0x30120a //T16P Ch.0 Running Control Register Start/stop timer
#define REG_T16P_CLK0	0x30120c //T16P Ch.0 Internal Clock Control Register Select internal count clock
#define REG_T16P_INT0	0x30120e //T16P Ch.0 Interrupt Control Register Control T16P interrupts

/*
16-bit audio
PWM timer
(T16P) Ch.1
(16-bit device)
*/
#define REG_T16P_A1	0x301210 //T16P Ch.1 Compare A Buffer Register Compare A data
#define REG_T16P_B1	0x301212 //T16P Ch.1 Compare B Buffer Register Compare B data
#define REG_T16P_CNT_DATA1	0x301214 //T16P Ch.1 Counter Data Register Counter data
#define REG_T16P_VOL_CTL1	0x301216 //T16P Ch.1 Volume Control Register Enable volume control and set volume level
#define REG_T16P_CTL1	0x301218 //T16P Ch.1 Control Register Set timer operating conditions
#define REG_T16P_RUN1	0x30121a //T16P Ch.1 Running Control Register Start/stop timer
#define REG_T16P_CLK1	0x30121c //T16P Ch.1 Internal Clock Control Register Select internal count clock
#define REG_T16P_INT1	0x30121e //T16P Ch.1 Interrupt Control Register Control T16P interrupts

/*
A/D converter
(ADC10)
(16-bit device)
*/
#define REG_ADC10_ADD	0x301300 //A/D Conversion Result Register A/D converted data
#define REG_ADC10_TRG	0x301302 //A/D Trigger/Channel Select Register Set start/end channels and conversion mode
#define REG_ADC10_CTL	0x301304 //A/D Control/Status Register Control A/D converter and indicate conversion status
#define REG_ADC10_CLK	0x301306 //A/D Clock Control Register Control A/D converter clock

/*
Remote
controller
(REMC)
(16-bit device)
*/
#define REG_REMC_CFG	0x301500 //REMC Configuration Register Control clock and data transfer
#define REG_REMC_CAR	0x301502 //REMC Carrier Length Setup Register Set carrier H/L section lengths
#define REG_REMC_LCNT	0x301504 //REMC Length Counter Register Set transmit/receive data length
#define REG_REMC_INT	0x301506 //REMC Interrupt Control Register Control REMC interrupts

/*
LCD controller
(LCDC)
(32-bit device)
*/
#define REG_LCDC_INT	0x302000 //LCDC Interrupt Enable Register Enable/disable LCDC interrupts
#define REG_LCDC_PSAVE	0x302004 //Status and Power Save Configuration Register Indicate LCDC status and set power save mode
#define REG_LCDC_HDISP	0x302010 //Horizontal Display Register Set horizontal display period
#define REG_LCDC_VDISP	0x302014 //Vertical Display Register Set vertical display period
#define REG_LCDC_MODR	0x302018 //MOD Rate Register Set MOD rate
#define REG_LCDC_HDPS	0x302020 //Horizontal Display Start Position Register Set horizontal display start position for TFT
#define REG_LCDC_VDPS	0x302024 //Vertical Display Start Position Register Set vertical display start position for TFT
#define REG_LCDC_FPLINE	0x302028 //FPLINE Pulse Setting Register Configure FPLINE pulse for TFT
#define REG_LCDC_FPFR	0x30202c //FPFRAME Pulse Setting Register Configure FPFRAME pulse for TFT
#define REG_LCDC_FPFROFS	0x302030 //FPFRAME Pulse Offset Register Adjust FPLINE pulse timings for TFT
#define REG_LCDC_TFTSO	0x302040 //TFT Special Output Register Set TFT control signals
#define REG_LCDC_TFT_CTL1	0x302044 //TFT_CTL1 Pulse Register Set TFT_CTL1 pulse timings
#define REG_LCDC_TFT_CTL0	0x302048 //TFT_CTL0 Pulse Register Set TFT_CTL0 pulse timings
#define REG_LCDC_TFT_CTL2	0x30204c //TFT_CTL2 Register Set TFT_CTL2 signal timing
#define REG_LCDC_RLDCTL	0x302050 //LCDC Reload Control Register Control reloading
#define REG_LCDC_RLDADR	0x302054 //LCDC Reload Table Base Address Register Set reload table base address
#define REG_LCDC_DISPMOD	0x302060 //LCDC Display Mode Register Set display conditions
#define REG_LCDC_MAINADR	0x302070 //Main Window Display Start Address Register Set main window display start address
#define REG_LCDC_MAINOFS	0x302074 //Main Screen Address Offset Register Set virtual main screen width
#define REG_LCDC_SUBADR	0x302080 //Sub-window Display Start Address Register Set sub-window display start address
#define REG_LCDC_SUBOFS	0x302084 //Sub-screen Address Offset Register Set virtual sub-screen width
#define REG_LCDC_SUBSP	0x302088 //Sub-window Start Position Register Set sub-window start position
#define REG_LCDC_SUBEP	0x30208c //Sub-window End Position Register Set sub-window end position
#define REG_LCDC_MLUT0	0x302090 //Monochrome Look-up Table Register 0 Monochrome look-up table data entries 0每7
#define REG_LCDC_MLUT1	0x302094 //Monochrome Look-up Table Register 1 Monochrome look-up table data entries 8每15

/*
DMA controller
(DMAC)
(32-bit device)
*/
#define REG_DMAC_CH_EN	0x302100 //DMAC Channel Enable Register Enable DMAC channels
#define REG_DMAC_TBL_BASE	0x302104 //DMAC Control Table Base Address Register Set control table base address
#define REG_DMAC_IE	0x302108 //DMAC Interrupt Enable Register Enable/disable DMAC interrupts
#define REG_DMAC_TRG_SEL	0x30210c //DMAC Trigger Select Register Select trigger sources
#define REG_DMAC_TRG_FLG	0x302110 //DMAC Trigger Flag Register Control software trigger and indicate trigger status
#define REG_DMAC_END_FLG	0x302114 //DMAC End-of-Transfer Flag Register Indicate DMA completed channels
#define REG_DMAC_RUN_STAT	0x302118 //DMAC Running Status Register Indicates running channel
#define REG_DMAC_PAUSE_STAT	0x30211c //DMAC Pause Status Register Indicate DMA suspended channels

/*
SDRAM
controller
(SDRAMC)
(32-bit device)
*/
#define REG_SDRAMC_INIT	0x302200 //SDRAM Initialization Register Enable SDRAMC and control SDRAM initialization
#define REG_SDRAMC_CFG	0x302204 //SDRAM Configuration Register Set SDRAM size and timing parameters
#define REG_SDRAMC_REF	0x302208 //SDRAM Refresh Control Register Control SDRAM refresh
#define REG_SDRAMC_APP	0x302210 //SDRAM Application Configuration Register Set CAS latency
#define SDRAMCBIT_SDON	4
#define SDRAMCBIT_INIDO	3
#define SDRAMCBIT_INIMRS	2
#define SDRAMCBIT_INIPEF	1
#define SDRAMCBIT_INIREF	0
#define SDRAMCBIT_T24NS	12
#define SDRAMCBIT_T60NS	8
#define SDRAMCBIT_T80NS	4
#define SDRAMCBIT_ADDRC	0
#define SDRAMCBIT_SREFDO	25
#define SDRAMCBIT_SCKON	24
#define SDRAMCBIT_SELEN	23
#define SDRAMCBIT_SELCO	16
#define SDRAMCBIT_AURCO	0
#define SDRAMCBIT_CAS	2

/*
SRAM
controller
(SRAMC)
(32-bit device)
*/
#define REG_SRAMC_TMG47	0x302220 //#CE[7:4] Access Timing Configuration Register Set #CE[7:4] access conditions
#define REG_SRAMC_TMG810	0x302224 //#CE[10:8] Access Timing Configuration Register Set #CE[10:8] access conditions
#define REG_SRAMC_TYPE	0x302228 //#CE[10:4] Device Configuration Register Set #CE[10:4] device types
#define SRAMCBIT_CE7SETUP	30
#define SRAMCBIT_CE7HOLD	28
#define SRAMCBIT_CE7WAIT	24
#define SRAMCBIT_CE5SETUP	14
#define SRAMCBIT_CE5HOLD	12
#define SRAMCBIT_CE5WAIT	8
#define SRAMCBIT_CE4SETUP	6
#define SRAMCBIT_CE4HOLD	4
#define SRAMCBIT_CE4WAIT	0
#define SRAMCBIT_CE10SETUP	22
#define SRAMCBIT_CE10HOLD	20
#define SRAMCBIT_CE10WAIT	16
#define SRAMCBIT_CE9SETUP	14
#define SRAMCBIT_CE9HOLD	12
#define SRAMCBIT_CE9WAIT	8
#define SRAMCBIT_CE8SETUP	6
#define SRAMCBIT_CE8HOLD	4
#define SRAMCBIT_CE8WAIT	0
#define SRAMCBIT_CE10TYPE	12
#define SRAMCBIT_CE9TYPE	10
#define SRAMCBIT_CE8TYPE	8
#define SRAMCBIT_CE7TYPE	6
#define SRAMCBIT_CE5TYPE	2
#define SRAMCBIT_CE4TYPE	0


/*
Cache
controller
(CCU)
(32-bit device)
*/
#define REG_CCU_CFG	0x302300 //Cache Configuration Register Enable instruction and data caches
#define REG_CCU_AREA	0x302304 //Cacheable Area Select Register Select cacheable areas
#define REG_CCU_LK	0x302308 //Cache Lock Register Configure cache lock function
#define REG_CCU_STAT	0x30230c //Cache Status Register Indicate cache statuses
#define REG_CCU_WB_STAT	0x302318 //Cache Write Buffer Status Register Indicate write buffer status
#define CCUBIT_WBEN	8
#define CCUBIT_SBRK	2
#define CCUBIT_IC	1
#define CCUBIT_DC	0
#define CCUBIT_ARIC	4
#define CCUBIT_ARDC	0
#define CCUBIT_ICLKS	3
#define CCUBIT_DCLKS	2
#define CCUBIT_ICS	1
#define CCUBIT_DCS	0
#define CCUBIT_WEFINISH	9
#define CCUBIT_WBEMPTY	8


/*
I2S
(32-bit device)
*/
#define REG_I2S_CTL0	0x302400 //I2S Ch.0 Control Register Control I2S Ch.0 output
#define REG_I2S_CTL1	0x302404 //I2S Ch.1 Control Register Control I2S Ch.1 input
#define REG_I2S_DV_MCLK	0x302408 //I2S Master Clock Division ratio Register Configure master clock
#define REG_I2S_DV_BCLK	0x30240c //I2S Audio Clock Division ratio Register Configure audio clock
#define REG_I2S_START	0x302410 //I2S Start/Stop Register Control/indicate I2S start/stop status
#define REG_I2S_STATUS	0x302414 //I2S FIFO Status Register Indicate FIFO status
#define REG_I2S_INT	0x302418 //I2S Interrupt Control Register Control I2S interrupts
#define REG_I2S_MODE	0x30241c //I2S Mode Select Register Set DMA and pin modes
#define REG_I2S_FIFO0	0x302420 //I2S Ch.0 FIFO Register I2S Ch.0 output data
#define REG_I2S_FIFO0_H	0x302422
#define REG_I2S_FIFO1	0x302430 //I2S Ch.1 FIFO Register I2S Ch.1 input data
#define REG_I2S_FIFO1_H	0x302432

/*
SD/MMC
interface
(SD_MMC)
(32-bit device)
*/
#define REG_MMC_CLKCTL	0x302600 //SD_MMC Clock Control Register Control MMC clock
#define REG_MMC_FUNCTL	0x302604 //SD_MMC Function Control Register Control data transfer
#define REG_MMC_BLK	0x302608 //SD_MMC Block Size Setting Register Set block size
#define REG_MMC_TIMEOUT	0x30260c //SD_MMC Data/Busy Timeout Period Setting Register Set data/busy timeout
#define REG_MMC_CMD_IDX	0x302610 //SD_MMC Command Index Register Set command index
#define REG_MMC_CMD_ARG	0x302614 //SD_MMC Command Argument Register Set command argument
#define REG_MMC_RSP0	0x302618 //SD_MMC Response Register 0 Response code[31:0]
#define REG_MMC_RSP1	0x30261c //SD_MMC Response Register 1 Response code[63:32]
#define REG_MMC_RSP2	0x302620 //SD_MMC Response Register 2 Response code[95:64]
#define REG_MMC_RSP3	0x302624 //SD_MMC Response Register 3 Response code[127:96]
#define REG_MMC_RSP4	0x302628 //SD_MMC Response Register 4 Response code[135:128]
#define REG_MMC_STATUS	0x30262c //SD_MMC Status Register Indicate status
#define REG_MMC_INT	0x302630 //SD_MMC Interrupt Control Register Control interrupts and DMA transfer
#define REG_MMC_DAT	0x302650 //SD_MMC Data Transfer FIFO Register Transmit/receive data

#include "33l27_reg.h"

void BootGpioSet(DWORD pin, int in);
void BootGpioOut(DWORD pin, int data);
int BootGpioIn(DWORD pin);
void BootGpioPull(DWORD pin, int pullup);
void BootGpioInt(DWORD pin, int ena, int polH);
void BootGpioFunc(DWORD pin, int func);

void DMAC_Init(void);
void DMAC_En(unsigned long ulChs);
void DMAC_Dis(unsigned long ulChs);
void DMAC_DisInt(unsigned long ulChs);
void DMAC_EnInt(unsigned long ulChs);
void DMAC_TrgMode(unsigned long ulChs, unsigned char ucMode);
void DMAC_SoftTrg(unsigned long ulChs);
unsigned char DMAC_GetTrgStatus(void);
void DMAC_ClrEndFlg(unsigned long ulChs);
unsigned char DMAC_GetEndStatus(void);
unsigned char DMAC_GetRunStatus(void);
unsigned char DMAC_GetPauseStatus(void);
void DMAC_Setup(unsigned long channel, unsigned long pbw, 
		unsigned long reload, unsigned long mode,
		unsigned long enable, unsigned long dsinc,
		unsigned long srinc, unsigned long unit,
		unsigned long st, unsigned long count,
		unsigned long srcaddr, unsigned long dstaddr,
		unsigned long pbasel, unsigned long pbaseh);

void DMAC_ReloadSetup(unsigned long channel, unsigned long pbw, 
		unsigned long reload, unsigned long mode,
		unsigned long enable, unsigned long dsinc,
		unsigned long srinc, unsigned long unit,
		unsigned long st, unsigned long count,
		unsigned long srcaddr, unsigned long dstaddr,
		unsigned long pbasel, unsigned long pbaseh);

void DMAC_SetupEn(unsigned long channel, unsigned long enable);

void BootClockInit(void);
void ClockPclkCtrl(int clk, int enable);
int ClockGetSysClk(void);
DWORD MsDelay(DWORD minisecond);
DWORD UsDelay(DWORD us);
int ClockSwitch(int mode);
void ClockDetect(void);

void BootTimer8Init(int ti, unsigned long freq, int inten);
void BootTimer8Run(int ti);
void BootTimer8Stop(int ti);
void BootTimer8FindMode(int ti, int insert);

void BootTimer16Init(int ti, unsigned long freq);
void BootTimer16CompValue(int ti, WORD a, WORD b);
void BootTimer16IntSetup(int ti, int type, int enable);
void BootTimer16IntFlagClr(int ti, int type);
void BootTimer16Run(int ti);
void BootTimer16Stop(int ti);

int BootUartInit(int dlen, int sbit, int parity, int bd, int inten);

void BootCacheArea(int area);
void BootCacheFlushAll(void);
void BootCacheEnable(void);
void BootCacheDisable(void);

void MachineIdle(void);
int MachineStandby(int param);
void MachinePowerOff(int param);
DWORD MachineRtcValue(void);

void IrqUnmask(int vector, int level);
void IrqSetup(int vector, int mode);
void IrqAck(int vector);
void IrqMask(int vector);
int IrqVector(void);

#define EnterCpuIrq()		\
	/* in user-mode or kernel-mode */	\
	asm("xcmp	%r15, 0x10000000");	\
	asm("jrne	1f");	\
	/* save %r0~%r15*/	\
	asm("pushn	%r15");	\
	asm("jp		0f");	\
	asm("1:");	\
	asm("push	%r15");	\
	/* %r0, %r1 as temporary reg */	\
	asm("pushn	%r1");	\
	/* this is a little complicated */	\
	/* switch %dp(%r15) at 1st */	\
	asm("xld.w	%r15, 0x10000000");	\
	/* %usp -> %r1 */	\
	asm("ld.w	%r1, %sp");	\
	/* %ksp = %r0 = gCurThread->KStackTop */	\
	asm("xld.w	%r0, [gCurThread]");	\
	asm("xld.w	%r0, [%r0+184]");	\
	asm("ld.w	%sp, %r0");	\
	/* save %ret-pc */	\
	asm("xld.w	%r0, [%r1+16]");	\
	asm("push	%r0");	\
	/* save %psr */	\
	asm("xld.w	%r0, [%r1+12]");	\
	asm("push	%r0");	\
	/* save %sp(point to the stack before interrupt) */	\
	asm("ld.w	%r0, %r1");	\
	asm("add	%r0, 20");	\
	asm("push	%r0");	\
	/* save %dp */	\
	asm("xld.w	%r0, [%r1+8]");	\
	asm("push	%r0");	\
	/* save %r14~%r0 */	\
	asm("xld.w	%r0, [%r1]");	\
	asm("xld.w	%r1, [%r1+4]");	\
	asm("pushn	%r14");	\
	asm("0:");	\
	asm("pushs	%ahr");	\
	/* _IrqNesting++ */	\
	asm("xld.w	%r0, _IrqNesting");	\
	asm("xld.w	%r1, [%r0]");	\
	asm("add	%r1, 1");	\
	asm("ld.w	[%r0], %r1");	\
	asm("cmp	%r1, 1");	\
	asm("jrne	2f");	\
	asm("xcall	SchedLock");	\
	asm("2:");


#define ExitCpuIrq()	\
	/* _IrqNesting-- */	\
	asm("xld.w	%r0, _IrqNesting");	\
	asm("xld.w	%r1, [%r0]");	\
	asm("sub	%r1, 1");	\
	asm("ld.w	[%r0], %r1");	\
	/* in irq nesting, then jump */	\
	asm("jrne	0f");	\
	/* schedule */	\
	asm("xcall	SchedUnlock");	\
	/* get %dp(r15) */	\
	asm("xld.w	%r0, [%sp+68]");\
	asm("xcmp	%r0, 0x10000000");\
	asm("jreq	0f");\
	asm("pops	%ahr");	\
	/* get %r14~%r0 */	\
	asm("popn	%r14");	\
	/* get %sp & %sp = %usp */	\
	asm("xld.w	%r0, [%sp+4]");	\
	asm("ld.w	%r1, %sp");	\
	asm("ld.w	%sp, %r0");	\
	/* get ret-pc & push */	\
	asm("xld.w	%r0, [%r1+12]");	\
	asm("push	%r0");	\
	/* get %psr & push */	\
	asm("xld.w	%r0, [%r1+8]");	\
	asm("push	%r0");	\
	/* get %dp & push */	\
	asm("xld.w	%r0, [%r1]");	\
	asm("push	%r0");	\
	/* re-get %r0, %r1 */	\
	asm("xsub	%r1, 60");	\
	asm("xld.w	%r0, [%r1+4]");	\
	asm("push	%r0");	\
	asm("xld.w	%r0, [%r1]");	\
	asm("push	%r0");	\
	asm("popn	%r1");	\
	/* pop %dp */	\
	asm("pop	%r15");	\
	asm("reti");	\
	asm("0:");	\
	asm("pops	%ahr");	\
	asm("popn	%r15");	\
	asm("reti");

#endif

