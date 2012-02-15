/*
 * jz4760cpm.h
 * JZ4760 CPM register definition
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 *
 * Author: whxu@ingenic.cn
 */

#ifndef __JZ4760CPM_H__
#define __JZ4760CPM_H__


/*
 * Clock reset and power controller module(CPM) address definition
 */
#define	CPM_BASE	0xb0000000


/*
 * CPM registers offset address definition
 */
#define CPM_CPCCR_OFFSET        (0x00)  /* rw, 32, 0x01011100 */
#define CPM_LCR_OFFSET          (0x04)  /* rw, 32, 0x000000f8 */
#define CPM_RSR_OFFSET          (0x08)  /* rw, 32, 0x???????? */
#define CPM_CPPCR0_OFFSET       (0x10)  /* rw, 32, 0x28080011 */
#define CPM_CPPSR_OFFSET        (0x14)  /* rw, 32, 0x80000000 */
#define CPM_CLKGR0_OFFSET       (0x20)  /* rw, 32, 0x3fffffe0 */
#define CPM_OPCR_OFFSET         (0x24)  /* rw, 32, 0x00001570 */
#define CPM_CLKGR1_OFFSET       (0x28)  /* rw, 32, 0x0000017f */
#define CPM_CPPCR1_OFFSET       (0x30)  /* rw, 32, 0x28080002 */
#define CPM_CPSPR_OFFSET        (0x34)  /* rw, 32, 0x???????? */
#define CPM_CPSPPR_OFFSET       (0x38)  /* rw, 32, 0x0000a5a5 */
#define CPM_USBPCR_OFFSET       (0x3c)  /* rw, 32, 0x42992198 */
#define CPM_USBRDT_OFFSET       (0x40)  /* rw, 32, 0x00000096 */
#define CPM_USBVBFIL_OFFSET     (0x44)  /* rw, 32, 0x00000080 */
#define CPM_USBCDR_OFFSET       (0x50)  /* rw, 32, 0x00000000 */
#define CPM_I2SCDR_OFFSET       (0x60)  /* rw, 32, 0x00000000 */
#define CPM_LPCDR_OFFSET        (0x64)  /* rw, 32, 0x00000000 */
#define CPM_MSCCDR_OFFSET       (0x68)  /* rw, 32, 0x00000000 */
#define CPM_UHCCDR_OFFSET       (0x6c)  /* rw, 32, 0x00000000 */
#define CPM_SSICDR_OFFSET       (0x74)  /* rw, 32, 0x00000000 */
#define CPM_CIMCDR_OFFSET       (0x7c)  /* rw, 32, 0x00000000 */
#define CPM_GPSCDR_OFFSET       (0x80)  /* rw, 32, 0x00000000 */
#define CPM_PCMCDR_OFFSET       (0x84)  /* rw, 32, 0x00000000 */
#define CPM_GPUCDR_OFFSET       (0x88)  /* rw, 32, 0x00000000 */
#define CPM_PSWC0ST_OFFSET      (0x90)  /* rw, 32, 0x00000000 */
#define CPM_PSWC1ST_OFFSET      (0x94)  /* rw, 32, 0x00000000 */
#define CPM_PSWC2ST_OFFSET      (0x98)  /* rw, 32, 0x00000000 */
#define CPM_PSWC3ST_OFFSET      (0x9c)  /* rw, 32, 0x00000000 */


/*
 * CPM registers address definition
 */
#define CPM_CPCCR        (CPM_BASE + CPM_CPCCR_OFFSET)
#define CPM_LCR          (CPM_BASE + CPM_LCR_OFFSET)
#define CPM_RSR          (CPM_BASE + CPM_RSR_OFFSET)
#define CPM_CPPCR0       (CPM_BASE + CPM_CPPCR0_OFFSET)
#define CPM_CPPSR        (CPM_BASE + CPM_CPPSR_OFFSET)
#define CPM_CLKGR0       (CPM_BASE + CPM_CLKGR0_OFFSET)
#define CPM_OPCR         (CPM_BASE + CPM_OPCR_OFFSET)
#define CPM_CLKGR1       (CPM_BASE + CPM_CLKGR1_OFFSET)
#define CPM_CPPCR1       (CPM_BASE + CPM_CPPCR1_OFFSET)
#define CPM_CPSPR        (CPM_BASE + CPM_CPSPR_OFFSET)
#define CPM_CPSPPR       (CPM_BASE + CPM_CPSPPR_OFFSET)
#define CPM_USBPCR       (CPM_BASE + CPM_USBPCR_OFFSET)
#define CPM_USBRDT       (CPM_BASE + CPM_USBRDT_OFFSET)
#define CPM_USBVBFIL     (CPM_BASE + CPM_USBVBFIL_OFFSET)
#define CPM_USBCDR       (CPM_BASE + CPM_USBCDR_OFFSET)
#define CPM_I2SCDR       (CPM_BASE + CPM_I2SCDR_OFFSET)
#define CPM_LPCDR        (CPM_BASE + CPM_LPCDR_OFFSET)
#define CPM_MSCCDR       (CPM_BASE + CPM_MSCCDR_OFFSET)
#define CPM_UHCCDR       (CPM_BASE + CPM_UHCCDR_OFFSET)
#define CPM_SSICDR       (CPM_BASE + CPM_SSICDR_OFFSET)
#define CPM_CIMCDR       (CPM_BASE + CPM_CIMCDR_OFFSET)
#define CPM_GPSCDR       (CPM_BASE + CPM_GPSCDR_OFFSET)
#define CPM_PCMCDR       (CPM_BASE + CPM_PCMCDR_OFFSET)
#define CPM_GPUCDR       (CPM_BASE + CPM_GPUCDR_OFFSET)
#define CPM_PSWC0ST      (CPM_BASE + CPM_PSWC0ST_OFFSET)
#define CPM_PSWC1ST      (CPM_BASE + CPM_PSWC1ST_OFFSET)
#define CPM_PSWC2ST      (CPM_BASE + CPM_PSWC2ST_OFFSET)
#define CPM_PSWC3ST      (CPM_BASE + CPM_PSWC3ST_OFFSET)


/*
 * CPM registers common define
 */

/* Clock control register(CPCCR) */
#define CPCCR_ECS               BIT31
#define CPCCR_MEM               BIT30
#define CPCCR_CE                BIT22
#define CPCCR_PCS               BIT21

#ifndef JZ4760B

#define CPCCR_SDIV_LSB          24
#define CPCCR_SDIV_MASK         BITS_H2L(27, CPCCR_SDIV_LSB)

#endif 

#define CPCCR_H2DIV_LSB         16
#define CPCCR_H2DIV_MASK        BITS_H2L(19, CPCCR_H2DIV_LSB)

#define CPCCR_MDIV_LSB          12
#define CPCCR_MDIV_MASK         BITS_H2L(15, CPCCR_MDIV_LSB)

#define CPCCR_PDIV_LSB		8
#define CPCCR_PDIV_MASK         BITS_H2L(11, CPCCR_PDIV_LSB)

#define CPCCR_HDIV_LSB		4
#define CPCCR_HDIV_MASK         BITS_H2L(7, CPCCR_HDIV_LSB)

#define CPCCR_CDIV_LSB		0
#define CPCCR_CDIV_MASK         BITS_H2L(3, CPCCR_CDIV_LSB)

/* Low power control register(LCR) */
#define LCR_PDAHB1              BIT30
#define LCR_VBATIR              BIT29
#define LCR_PDGPS               BIT28
#define LCR_PDAHB1S             BIT26
#define LCR_PDGPSS              BIT24
#define LCR_DOZE                BIT2

#define LCR_PST_LSB             8
#define LCR_PST_MASK            BITS_H2L(19, LCR_PST_LSB)

#define LCR_DUTY_LSB            3
#define LCR_DUTY_MASK           BITS_H2L(7, LCR_DUTY_LSB)

#define LCR_LPM_LSB             0
#define LCR_LPM_MASK            BITS_H2L(1, LCR_LPM_LSB)
#define LCR_LPM_IDLE            (0x0 << LCR_LPM_LSB)
#define LCR_LPM_SLEEP           (0x1 << LCR_LPM_LSB)

/* Reset status register(RSR) */
#define RSR_P0R         BIT2
#define RSR_WR          BIT1
#define RSR_PR          BIT0

/* PLL control register 0(CPPCR0) */
#define CPPCR0_LOCK             BIT15   /* LOCK0 bit */
#define CPPCR0_ENLOCK           BIT14
#define CPPCR0_PLLS             BIT10
#define CPPCR0_PLLBP            BIT9
#define CPPCR0_PLLEN            BIT8

#define CPPCR0_PLLM_LSB         24
#define CPPCR0_PLLM_MASK        BITS_H2L(30, CPPCR0_PLLM_LSB)

#define CPPCR0_PLLN_LSB         18
#define CPPCR0_PLLN_MASK        BITS_H2L(21, CPPCR0_PLLN_LSB)

#define CPPCR0_PLLOD_LSB        16
#define CPPCR0_PLLOD_MASK       BITS_H2L(17, CPPCR0_PLLOD_LSB)

#define CPPCR0_PLLST_LSB        0
#define CPPCR0_PLLST_MASK       BITS_H2L(7, CPPCR0_PLLST_LSB)

/* PLL switch and status register(CPPSR) */
#define CPPSR_PLLOFF            BIT31
#define CPPSR_PLLBP             BIT30
#define CPPSR_PLLON             BIT29
#define CPPSR_PS                BIT28
#define CPPSR_FS                BIT27
#define CPPSR_CS                BIT26
#define CPPSR_SM                BIT2
#define CPPSR_PM                BIT1
#define CPPSR_FM                BIT0

/* Clock gate register 0(CGR0) */

#ifndef JZ4760B
#define CLKGR0_EMC              BIT31
#else
#define CLKGR0_AHB_MON          BIT31
#endif
 
#define CLKGR0_DDR              BIT30
#define CLKGR0_IPU              BIT29
#define CLKGR0_LCD              BIT28
#define CLKGR0_TVE              BIT27
#define CLKGR0_CIM              BIT26
#define CLKGR0_MDMA             BIT25
#define CLKGR0_UHC              BIT24
#define CLKGR0_MAC              BIT23
#define CLKGR0_GPS              BIT22
#define CLKGR0_DMAC             BIT21
#define CLKGR0_SSI2             BIT20
#define CLKGR0_SSI1             BIT19
#define CLKGR0_UART3            BIT18
#define CLKGR0_UART2            BIT17
#define CLKGR0_UART1            BIT16
#define CLKGR0_UART0            BIT15
#define CLKGR0_SADC             BIT14
#define CLKGR0_KBC              BIT13
#define CLKGR0_MSC2             BIT12
#define CLKGR0_MSC1             BIT11
#define CLKGR0_OWI              BIT10
#define CLKGR0_TSSI             BIT9
#define CLKGR0_AIC              BIT8
#define CLKGR0_SCC              BIT7
#define CLKGR0_I2C1             BIT6
#define CLKGR0_I2C0             BIT5
#define CLKGR0_SSI0             BIT4
#define CLKGR0_MSC0             BIT3
#define CLKGR0_OTG              BIT2
#define CLKGR0_BCH              BIT1
#define CLKGR0_NEMC             BIT0

/* Oscillator and power control register(OPCR) */
#define OPCR_OTGPHY_ENABLE      BIT7    /* SPENDN bit */
#define OPCR_GPSEN              BIT6
#define OPCR_UHCPHY_DISABLE     BIT5    /* SPENDH bit */
#define OPCR_O1SE               BIT4
#define OPCR_PD                 BIT3
#define OPCR_ERCS               BIT2

#define OPCR_O1ST_LSB           8
#define OPCR_O1ST_MASK          BITS_H2L(15, OPCR_O1ST_LSB)

/* Clock gate register 1(CGR1) */
#define CLKGR1_GPU              BIT9
#define CLKGR1_PCM              BIT8
#define CLKGR1_AHB1             BIT7
#define CLKGR1_CABAC            BIT6
#define CLKGR1_SRAM             BIT5
#define CLKGR1_DCT              BIT4
#define CLKGR1_ME               BIT3
#define CLKGR1_DBLK             BIT2
#define CLKGR1_MC               BIT1
#define CLKGR1_BDMA             BIT0

/* PLL control register 1(CPPCR1) */
#define CPPCR1_P1SCS            BIT15
#define CPPCR1_PLL1EN           BIT7
#define CPPCR1_PLL1S            BIT6
#define CPPCR1_LOCK             BIT2    /* LOCK1 bit */
#define CPPCR1_PLL1OFF          BIT1
#define CPPCR1_PLL1ON           BIT0

#define CPPCR1_PLL1M_LSB        24
#define CPPCR1_PLL1M_MASK       BITS_H2L(30, CPPCR1_PLL1M_LSB)

#define CPPCR1_PLL1N_LSB        18
#define CPPCR1_PLL1N_MASK       BITS_H2L(21, CPPCR1_PLL1N_LSB)

#define CPPCR1_PLL1OD_LSB       16
#define CPPCR1_PLL1OD_MASK      BITS_H2L(17, CPPCR1_PLL1OD_LSB)

#define CPPCR1_P1SDIV_LSB       9
#define CPPCR1_P1SDIV_MASK      BITS_H2L(14, CPPCR1_P1SDIV_LSB)

/* CPM scratch pad protected register(CPSPPR) */
#define CPSPPR_CPSPR_WRITABLE   (0x00005a5a)

/* OTG parameter control register(USBPCR) */
#define USBPCR_USB_MODE         BIT31
#define USBPCR_AVLD_REG         BIT30
#define USBPCR_INCRM            BIT27   /* INCR_MASK bit */
#define USBPCR_CLK12_EN         BIT26
#define USBPCR_COMMONONN        BIT25
#define USBPCR_VBUSVLDEXT       BIT24
#define USBPCR_VBUSVLDEXTSEL    BIT23
#define USBPCR_POR              BIT22
#define USBPCR_SIDDQ            BIT21
#define USBPCR_OTG_DISABLE      BIT20
#define USBPCR_TXPREEMPHTUNE    BIT6

#define USBPCR_IDPULLUP_LSB             28   /* IDPULLUP_MASK bit */
#define USBPCR_IDPULLUP_MASK            BITS_H2L(29, USBPCR_USBPCR_IDPULLUP_LSB)

#define USBPCR_COMPDISTUNE_LSB          17
#define USBPCR_COMPDISTUNE_MASK         BITS_H2L(19, USBPCR_COMPDISTUNE_LSB)

#define USBPCR_OTGTUNE_LSB              14
#define USBPCR_OTGTUNE_MASK             BITS_H2L(16, USBPCR_OTGTUNE_LSB)

#define USBPCR_SQRXTUNE_LSB             11
#define USBPCR_SQRXTUNE_MASK            BITS_H2L(13, USBPCR_SQRXTUNE_LSB)

#define USBPCR_TXFSLSTUNE_LSB           7
#define USBPCR_TXFSLSTUNE_MASK          BITS_H2L(10, USBPCR_TXFSLSTUNE_LSB)

#define USBPCR_TXRISETUNE_LSB           4
#define USBPCR_TXRISETUNE_MASK          BITS_H2L(5, USBPCR_TXRISETUNE_LSB)

#define USBPCR_TXVREFTUNE_LSB           0
#define USBPCR_TXVREFTUNE_MASK          BITS_H2L(3, USBPCR_TXVREFTUNE_LSB)

/* OTG reset detect timer register(USBRDT) */
#define USBRDT_VBFIL_LD_EN      BIT25
#define USBRDT_IDDIG_EN         BIT24
#define USBRDT_IDDIG_REG        BIT23

#define USBRDT_USBRDT_LSB       0
#define USBRDT_USBRDT_MASK      BITS_H2L(22, USBRDT_USBRDT_LSB)

/* OTG PHY clock divider register(USBCDR) */
#define USBCDR_UCS              BIT31
#define USBCDR_UPCS             BIT30

#define USBCDR_OTGDIV_LSB       0       /* USBCDR bit */
#define USBCDR_OTGDIV_MASK      BITS_H2L(5, USBCDR_OTGDIV_LSB)

/* I2S device clock divider register(I2SCDR) */
#define I2SCDR_I2CS             BIT31
#define I2SCDR_I2PCS            BIT30

#define I2SCDR_I2SDIV_LSB       0       /* I2SCDR bit */
#define I2SCDR_I2SDIV_MASK      BITS_H2L(8, I2SCDR_I2SDIV_LSB)

/* LCD pix clock divider register(LPCDR) */
#define LPCDR_LSCS              BIT31
#define LPCDR_LTCS              BIT30	/*LCD TV Encoder or Panel pix clock Selection*/
#define LPCDR_LPCS              BIT29	/*LCD pix clock selection*/

#define LPCDR_PIXDIV_LSB        0       /* LPCDR bit */
#define LPCDR_PIXDIV_MASK       BITS_H2L(10, LPCDR_PIXDIV_LSB)

/* MSC clock divider register(MSCCDR) */
#define MSCCDR_MCS              BIT31

#define MSCCDR_MSCDIV_LSB       0       /* MSCCDR bit */
#define MSCCDR_MSCDIV_MASK      BITS_H2L(5, MSCCDR_MSCDIV_LSB)

/* UHC device clock divider register(UHCCDR) */
#define UHCCDR_UHPCS            BIT31

#define UHCCDR_UHCDIV_LSB       0       /* UHCCDR bit */
#define UHCCDR_UHCDIV_MASK      BITS_H2L(3, UHCCDR_UHCDIV_LSB)

/* SSI clock divider register(SSICDR) */
#define SSICDR_SCS              BIT31

#define SSICDR_SSIDIV_LSB       0       /* SSICDR bit */
#define SSICDR_SSIDIV_MASK      BITS_H2L(5, SSICDR_SSIDIV_LSB)

/* CIM mclk clock divider register(CIMCDR) */
#define CIMCDR_CIMDIV_LSB       0       /* CIMCDR bit */
#define CIMCDR_CIMDIV_MASK      BITS_H2L(7, CIMCDR_CIMDIV_LSB)

/* GPS clock divider register(GPSCDR) */
#define GPSCDR_GPCS             BIT31

#define GPSCDR_GPSDIV_LSB       0       /* GPSCDR bit */
#define GSPCDR_GPSDIV_MASK      BITS_H2L(3, GPSCDR_GPSDIV_LSB)

/* PCM device clock divider register(PCMCDR) */
#define PCMCDR_PCMS             BIT31
#define PCMCDR_PCMPCS           BIT30

#define PCMCDR_PCMDIV_LSB       0       /* PCMCDR bit */
#define PCMCDR_PCMDIV_MASK      BITS_H2L(8, PCMCDR_PCMDIV_LSB)

/* GPU clock divider register */
#define GPUCDR_GPCS             BIT31
#define GPUCDR_GPUDIV_LSB       0       /* GPUCDR bit */
#define GPUCDR_GPUDIV_MASK      BITS_H2L(2, GPUCDR_GPUDIV_LSB)


#ifndef __MIPS_ASSEMBLER

#define REG_CPM_CPCCR           REG32(CPM_CPCCR)
#define REG_CPM_RSR             REG32(CPM_RSR)
#define REG_CPM_CPPCR0          REG32(CPM_CPPCR0)
#define REG_CPM_CPPSR           REG32(CPM_CPPSR)
#define REG_CPM_CPPCR1          REG32(CPM_CPPCR1)
#define REG_CPM_CPSPR           REG32(CPM_CPSPR)
#define REG_CPM_CPSPPR          REG32(CPM_CPSPPR)
#define REG_CPM_USBPCR          REG32(CPM_USBPCR)
#define REG_CPM_USBRDT          REG32(CPM_USBRDT)
#define REG_CPM_USBVBFIL        REG32(CPM_USBVBFIL)
#define REG_CPM_USBCDR          REG32(CPM_USBCDR)
#define REG_CPM_I2SCDR          REG32(CPM_I2SCDR)
#define REG_CPM_LPCDR           REG32(CPM_LPCDR)
#define REG_CPM_MSCCDR          REG32(CPM_MSCCDR)
#define REG_CPM_UHCCDR          REG32(CPM_UHCCDR)
#define REG_CPM_SSICDR          REG32(CPM_SSICDR)
#define REG_CPM_CIMCDR          REG32(CPM_CIMCDR)
#define REG_CPM_GPSCDR          REG32(CPM_GPSCDR)
#define REG_CPM_PCMCDR          REG32(CPM_PCMCDR)
#define REG_CPM_GPUCDR          REG32(CPM_GPUCDR)

#define REG_CPM_PSWC0ST         REG32(CPM_PSWC0ST)
#define REG_CPM_PSWC1ST         REG32(CPM_PSWC1ST)
#define REG_CPM_PSWC2ST         REG32(CPM_PSWC2ST)
#define REG_CPM_PSWC3ST         REG32(CPM_PSWC3ST)

#define REG_CPM_LCR             REG32(CPM_LCR)
#define REG_CPM_CLKGR0          REG32(CPM_CLKGR0)
#define REG_CPM_OPCR            REG32(CPM_OPCR)
#define REG_CPM_CLKGR1          REG32(CPM_CLKGR1)
#define REG_CPM_CLKGR           REG32(CPM_CLKGR0)

//=====================================================================
/* PLL output frequency */

#define __cpm_get_pllm() \
	((REG_CPM_CPPCR0 & CPPCR0_PLLM_MASK) >> CPPCR0_PLLM_LSB)
#define __cpm_get_plln() \
	((REG_CPM_CPPCR0 & CPPCR0_PLLN_MASK) >> CPPCR0_PLLN_LSB)
#define __cpm_get_pllod() \
	((REG_CPM_CPPCR0 & CPPCR0_PLLOD_MASK) >> CPPCR0_PLLOD_LSB)

#define __cpm_get_pll1m() \
	((REG_CPM_CPPCR1 & CPPCR1_PLL1M_MASK) >> CPPCR1_PLL1M_LSB)
#define __cpm_get_pll1n() \
	((REG_CPM_CPPCR1 & CPPCR1_PLL1N_MASK) >> CPPCR1_PLL1N_LSB)
#define __cpm_get_pll1od() \
	((REG_CPM_CPPCR1 & CPPCR1_PLL1OD_MASK) >> CPPCR1_PLL1OD_LSB)

#define __cpm_get_cdiv() \
	((REG_CPM_CPCCR & CPCCR_CDIV_MASK) >> CPCCR_CDIV_LSB)
#define __cpm_get_hdiv() \
	((REG_CPM_CPCCR & CPCCR_HDIV_MASK) >> CPCCR_HDIV_LSB)
#define __cpm_get_h2div() \
	((REG_CPM_CPCCR & CPCCR_H2DIV_MASK) >> CPCCR_H2DIV_LSB)
#define __cpm_get_pdiv() \
	((REG_CPM_CPCCR & CPCCR_PDIV_MASK) >> CPCCR_PDIV_LSB)
#define __cpm_get_mdiv() \
	((REG_CPM_CPCCR & CPCCR_MDIV_MASK) >> CPCCR_MDIV_LSB)
#define __cpm_get_sdiv() \
	((REG_CPM_CPCCR & CPCCR_SDIV_MASK) >> CPCCR_SDIV_LSB)

#define __cpm_get_mscdiv() \
	((REG_CPM_MSCCDR & MSCCDR_MSCDIV_MASK) >> MSCCDR_MSCDIV_LSB)
#define __cpm_set_mscdiv(v) \
	(REG_CPM_MSCCDR = (REG_CPM_MSCCDR & ~MSCCDR_MSCDIV_MASK) | ((v) << (MSCCDR_MSCDIV_LSB)) | MSCCDR_MCS)

#define __cpm_enable_pll_change()	(REG_CPM_CPCCR |= CPCCR_CE)

static __inline__ unsigned int __cpm_get_pllout(void)
{
	unsigned long m, n, no, pllout;
	unsigned long cppcr = REG_CPM_CPPCR0;
	unsigned long od[4] = {1, 2, 4, 8};
	if ((cppcr & CPPCR0_PLLEN) && (!(cppcr & CPPCR0_PLLBP))) {
		m = __cpm_get_pllm() * 2;
		n = __cpm_get_plln();
		no = od[__cpm_get_pllod()];
		pllout = ((JZ_EXTAL) * m / (n * no));
	} else
		pllout = JZ_EXTAL;
	return pllout;
}
#if 0
/* PLL output frequency */
static __inline__ unsigned int __cpm_get_pll1out(void)
{
	unsigned long m, n, no, pllout;
	unsigned long cppcr1 = REG_CPM_CPPCR1;
	unsigned long od[4] = {1, 2, 4, 8};
	if (cppcr1 & CPPCR1_PLL1EN)
	{
		m = __cpm_get_pll1m() * 2;
		n = __cpm_get_pll1n();
		no = od[__cpm_get_pll1od()];
		if (cppcr1 & CPPCR1_P1SCS)
			pllout = ((__cpm_get_pllout()) * m / (n * no));
		else
			pllout = ((JZ_EXTAL) * m / (n * no));
			
	} else
		pllout = JZ_EXTAL;
	return pllout;
}

/* PLL output frequency for MSC/I2S/LCD/USB */
static __inline__ unsigned int __cpm_get_pllout2(void)
{
	if (REG_CPM_CPCCR & CPCCR_PCS)
		return __cpm_get_pllout();
	else
		return __cpm_get_pllout()/2;
}

/* CPU core clock */
static __inline__ unsigned int __cpm_get_cclk(void)
{
	int div[] = {1, 2, 3, 4, 6, 8};

	return __cpm_get_pllout() / div[__cpm_get_cdiv()];
}

/* AHB system bus clock */
static __inline__ unsigned int __cpm_get_hclk(void)
{
	int div[] = {1, 2, 3, 4, 6, 8};

	return __cpm_get_pllout() / div[__cpm_get_hdiv()];
}
#endif
/* Memory bus clock */
static __inline__ unsigned int __cpm_get_mclk(void)
{
	int div[] = {1, 2, 3, 4, 6, 8};

	return __cpm_get_pllout() / div[__cpm_get_mdiv()];
}

#if 0
/* APB peripheral bus clock */
static __inline__ unsigned int __cpm_get_pclk(void)
{
	int div[] = {1, 2, 3, 4, 6, 8};

	return __cpm_get_pllout() / div[__cpm_get_pdiv()];
}

/* AHB1 module clock */
static __inline__ unsigned int __cpm_get_h2clk(void)
{
	int div[] = {1, 2, 3, 4, 6, 8};

	return __cpm_get_pllout() / div[__cpm_get_h2div()];
}
#endif
//=====================================================================

typedef enum __CLKDIV
{
	CPM_CDIV 	= CPCCR_CDIV_LSB,
	CPM_H0DIV = CPCCR_HDIV_LSB,
	CPM_PDIV 	= CPCCR_PDIV_LSB,
	CPM_MDIV 	= CPCCR_MDIV_LSB,
	CPM_H1DIV = CPCCR_H2DIV_LSB,
} CLKDIV, *PCLKDIV;

typedef enum {
        CGM_NEMC = 0,
        CGM_BCH  = 1,
        CGM_OTG  = 2,
        CGM_MSC0 = 3,
        CGM_SSI0 = 4,
        CGM_I2C0 = 5,
        CGM_I2C1 = 6,
        CGM_SCC  = 7,
        CGM_AIC         = 8,
        CGM_TSSI        = 9,
        CGM_OWI         = 10,
        CGM_MSC1        = 11,
        CGM_MSC2        = 12,
        CGM_KBC         = 13,
        CGM_SADC        = 14,
        CGM_UART0       = 15,
        CGM_UART1 = 16,
        CGM_UART2 = 17,
        CGM_UART3 = 18,
        CGM_SSI1  = 19,
        CGM_SSI2  = 20,
        CGM_DMAC  = 21,
        CGM_GPS   = 22,
        CGM_MAC   = 23,
        CGM_UHC         = 24,
        CGM_MDMA        = 25,
        CGM_CIM         = 26,
        CGM_TVE         = 27,
        CGM_LCD         = 28,
        CGM_IPU         = 29,
        CGM_DDR         = 30,
        CGM_EMC         = 31,
        CGM_BDMA  = 32 + 0,
        CGM_MC    = 32 + 1,
        CGM_DBLK  = 32 + 2,
        CGM_ME    = 32 + 3,
        CGM_DCT   = 32 + 4,
        CGM_SRAM  = 32 + 5,
        CGM_CABAC = 32 + 6,
        CGM_AHB1  = 32 + 7,
        CGM_PCM         = 32 + 8,
        CGM_GPU         = 32 + 9,
        CGM_ALL_MODULE,
} clock_gate_module;


#define __CGU_CLOCK_BASE__      0x1000

typedef enum {
        /* Clock source is pll0 */
        CGU_CCLK = __CGU_CLOCK_BASE__ + 0,
        CGU_HCLK,
        CGU_PCLK,
        CGU_MCLK,
        CGU_H2CLK,
        CGU_SCLK,

        /* Clock source is exclk, pll0 or pll0/2 */
        CGU_MSCCLK,
        CGU_SSICLK,

        /* Clock source is pll0 or pll0/2 */
        CGU_CIMCLK,

        /* Clock source is exclk, pll0, pll0/2 or pll1 */
        CGU_TVECLK,

        /* Clock source is pll0 */
        CGU_LPCLK,

        /* Clock source is exclk, exclk/2, pll0, pll0/2 or pll1 */
        CGU_I2SCLK,
        CGU_PCMCLK,
        CGU_OTGCLK,

        /* Clock source is pll0, pll0/2 or pll1 */
        CGU_UHCCLK,
        CGU_GPSCLK,
        CGU_GPUCLK,

        /* Clock source is exclk or exclk/2 */
        CGU_UARTCLK,
        CGU_SADCCLK,

        /* Clock source is exclk */
        CGU_TCUCLK,

	/* Clock source is external rtc clock */
	CGU_RTCCLK,

        CGU_CLOCK_MAX,
} cgu_clock;

/*
 * JZ4760 clocks structure
 */
typedef struct {
	unsigned int cclk;	/* CPU clock				*/
	unsigned int hclk;	/* System bus clock: AHB0,AHB1		*/
	unsigned int h1clk;	/* For compatible, the same as h1clk	*/
	unsigned int h2clk;	/* System bus clock: AHB2		*/
	unsigned int pclk;	/* Peripheral bus clock			*/
	unsigned int mclk;	/* EMC or DDR controller clock		*/
	unsigned int sclk;	/* NEMC controller clock		*/
	unsigned int cko;	/* SDRAM or DDR clock			*/
	unsigned int pixclk;	/* LCD pixel clock			*/
	unsigned int tveclk;	/* TV encoder 27M  clock		*/
	unsigned int cimmclk;	/* Clock output from CIM module		*/
	unsigned int cimpclk;	/* Clock input to CIM module		*/
	unsigned int gpuclk;	/* GPU clock				*/
	unsigned int gpsclk;	/* GPS clock				*/
	unsigned int i2sclk;	/* I2S codec clock			*/
	unsigned int bitclk;	/* AC97 bit clock			*/
	unsigned int pcmclk;	/* PCM clock				*/
	unsigned int mscclk;	/* MSC clock				*/
	unsigned int ssiclk;	/* SSI clock				*/
	unsigned int tssiclk;	/* TSSI clock				*/
	unsigned int otgclk;	/* USB OTG clock			*/
	unsigned int uhcclk;	/* USB UHCI clock			*/
	unsigned int extalclk;	/* EXTAL clock for
				   UART,I2C,TCU,USB2.0-PHY,AUDIO CODEC	*/
	unsigned int rtcclk;	/* RTC clock for CPM,INTC,RTC,TCU,WDT	*/
} jz_clocks_t;


void cpm_start_clock(clock_gate_module module_name);
void cpm_stop_clock(clock_gate_module module_name);

unsigned int cpm_set_clock(cgu_clock clock_name, unsigned int clock_hz);
unsigned int cpm_get_clock(cgu_clock clock_name);
unsigned int cpm_get_pllout(void);

void cpm_uhc_phy(unsigned int en);

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4760CPM_H__ */

