/*
 * linux/include/asm-mips/mach-jz4760/jz4760cim.h
 *
 * JZ4760 CIM register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4760CIM_H__
#define __JZ4760CIM_H__


#define	CIM_BASE	0xB3060000


//sxyzhang add 2010-12-2
#define	CIMCFG_EXT_VSYNC	( 1 << 8 )	// In CCIR656 Mode Only 
#define SHIFT_DF	( 16 )		// bit16 - bit17
#define CIMCFG_DF_MASK	( 0x3 << SHIFT_DF)

#define	CIMCFG_DF_RESERVED		( 0x0 << SHIFT_DF )
#define	CIMCFG_DF_YCbCr444		( 0x1 << SHIFT_DF )
#define	CIMCFG_DF_YCbCr422		( 0x2 << SHIFT_DF )
#define	CIMCFG_DF_ITU656_YCbCr422	( 0x3 << SHIFT_DF )


#define SHIFT_ORDER	( 18 )		// bit18 - bit19
#define CIMCFG_ORDER_MASK	( 0x3 << SHIFT_ORDER )

#define	CIMCFG_ORDER_YCbCr	( 0x0 << SHIFT_ORDER )
#define	CIMCFG_ORDER_YCrCb	( 0x1 << SHIFT_ORDER )
#define	CIMCFG_ORDER_CbCrY	( 0x2 << SHIFT_ORDER )
#define	CIMCFG_ORDER_CrCbY	( 0x3 << SHIFT_ORDER )

#define	CIMCFG_BYPASS		( 1 << 2 )

//sxyzhang add over


/*************************************************************************
 * CIM
 *************************************************************************/
#define	A_CIMCFG		(CIM_BASE + 0x0000)
#define	A_CIMCR			(CIM_BASE + 0x0004)
#define	A_CIMST			(CIM_BASE + 0x0008)
#define	A_CIMIID		(CIM_BASE + 0x000C)

#ifndef JZ4760B
#define	A_CIMRXFIFO		(CIM_BASE + 0x0010)
#endif 

#define	A_CIMDA			(CIM_BASE + 0x0020)
#define	A_CIMFA			(CIM_BASE + 0x0024)
#define	A_CIMFID		(CIM_BASE + 0x0028)
#define	A_CIMCMD		(CIM_BASE + 0x002C)
#define	A_CIMSIZE		(CIM_BASE + 0x0030)
#define	A_CIMOFFSET		(CIM_BASE + 0x0034)
//#define	A_CIMRAM_ADDR		(CIM_BASE + 0x1000)

#ifdef JZ4760B
#define CIM_YFA			(CIM_BASE + 0x0038)
#define CIM_YCMD		(CIM_BASE + 0x003C)
#define CIM_CBFA		(CIM_BASE + 0x0040)
#define CIM_CBCMD		(CIM_BASE + 0x0044)
#define CIM_CRFA		(CIM_BASE + 0x0048)
#define CIM_CRCMD		(CIM_BASE + 0x004C)
#define CIM_CTRL2		(CIM_BASE + 0x0050)
#endif 


#define	REG_CIM_CFG		REG32(A_CIMCFG)
#define	REG_CIM_CTRL	REG32(A_CIMCR)
#define	REG_CIM_STATE	REG32(A_CIMST)
#define	REG_CIM_IID		REG32(A_CIMIID)

#ifndef JZ4760B
#define	REG_CIM_RXFIFO	REG32(A_CIMRXFIFO)
#endif 

#define	REG_CIM_DA		REG32(A_CIMDA)
#define	REG_CIM_FA		REG32(A_CIMFA)
#define	REG_CIM_FID		REG32(A_CIMFID)
#define	REG_CIM_CMD		REG32(A_CIMCMD)
#define	REG_CIM_SIZE	REG32(A_CIMSIZE)
#define	REG_CIM_OFFSET	REG32(A_CIMOFFSET)

#ifdef JZ4760B

#define REG_CIM_YFA		REG32(CIM_YFA)
#define REG_CIM_YCMD		REG32(CIM_YCMD)
#define REG_CIM_CBFA		REG32(CIM_CBFA)
#define REG_CIM_CBCMD		REG32(CIM_CBCMD)
#define REG_CIM_CRFA		REG32(CIM_CRFA)
#define REG_CIM_CRCMD		REG32(CIM_CRCMD)
#define	REG_CIM_CTRL2		REG32(CIM_CTRL2)

#define CIM_CFG_EEOFEN			(1 << 31)
#define CIM_CFG_EXP			(1 << 30)

#define CIM_CFG_RXF_TRIG_BIT		24
#define CIM_CFG_RXF_TRIG_MASK		(0x3f << CIM_CFG_RXF_TRIG_BIT)

#define CIM_CFG_BW_BIT			22
#define CIM_CFG_BW_MASK			(0x3 << CIM_CFG_BW_BIT)

#define CIM_CFG_SEP			(1 << 20)
#endif 

#define	CIM_CFG_ORDER_BIT	18
#define	CIM_CFG_ORDER_MASK	(0x3 << CIM_CFG_ORDER_BIT)
  #define CIM_CFG_ORDER_0	  (0x0 << CIM_CFG_ORDER_BIT) 	/* Y0CbY1Cr; YCbCr */
  #define CIM_CFG_ORDER_1	  (0x1 << CIM_CFG_ORDER_BIT)	/* Y0CrY1Cb; YCrCb */
  #define CIM_CFG_ORDER_2	  (0x2 << CIM_CFG_ORDER_BIT)	/* CbY0CrY1; CbCrY */
  #define CIM_CFG_ORDER_3	  (0x3 << CIM_CFG_ORDER_BIT)	/* CrY0CbY1; CrCbY */
#define	CIM_CFG_DF_BIT		16
#define	CIM_CFG_DF_MASK		  (0x3 << CIM_CFG_DF_BIT)
  #define CIM_CFG_DF_YUV444	  (0x1 << CIM_CFG_DF_BIT) 	/* YCbCr444 */
  #define CIM_CFG_DF_YUV422	  (0x2 << CIM_CFG_DF_BIT)	/* YCbCr422 */
  #define CIM_CFG_DF_ITU656	  (0x3 << CIM_CFG_DF_BIT)	/* ITU656 YCbCr422 */
#define	CIM_CFG_INV_DAT		(1 << 15)
#define	CIM_CFG_VSP		(1 << 14) /* VSYNC Polarity:0-rising edge active,1-falling edge active */
#define	CIM_CFG_HSP		(1 << 13) /* HSYNC Polarity:0-rising edge active,1-falling edge active */
#define	CIM_CFG_PCP		(1 << 12) /* PCLK working edge: 0-rising, 1-falling */
#define	CIM_CFG_DMA_BURST_TYPE_BIT	10
#define	CIM_CFG_DMA_BURST_TYPE_MASK	(0x3 << CIM_CFG_DMA_BURST_TYPE_BIT)
  #define	CIM_CFG_DMA_BURST_INCR4		(0 << CIM_CFG_DMA_BURST_TYPE_BIT)
  #define	CIM_CFG_DMA_BURST_INCR8		(1 << CIM_CFG_DMA_BURST_TYPE_BIT)	/* Suggested */
  #define	CIM_CFG_DMA_BURST_INCR16	(2 << CIM_CFG_DMA_BURST_TYPE_BIT)	/* Suggested High speed AHB*/
  
  #ifdef JZ4760B
  
  #define	CIM_CFG_DMA_BURST_INCR32	(3 << CIM_CFG_DMA_BURST_TYPE_BIT)
  
  #endif
   
#define	CIM_CFG_DUMMY_ZERO	(1 << 9)
#define	CIM_CFG_EXT_VSYNC	(1 << 8)	/* Only for ITU656 Progressive mode */

#ifdef JZ4760B
#define	CIM_CFG_LM			(1 << 7)	/* Only for ITU656 Progressive mode */
#define	CIM_CFG_FP			(1 << 3)	/* Only for ITU656 Progressive mode */
#endif 

#define	CIM_CFG_PACK_BIT	4
#define	CIM_CFG_PACK_MASK	(0x7 << CIM_CFG_PACK_BIT)
  #define CIM_CFG_PACK_0	  (0 << CIM_CFG_PACK_BIT) /* 11 22 33 44 0xY0CbY1Cr */
  #define CIM_CFG_PACK_1	  (1 << CIM_CFG_PACK_BIT) /* 22 33 44 11 0xCbY1CrY0 */
  #define CIM_CFG_PACK_2	  (2 << CIM_CFG_PACK_BIT) /* 33 44 11 22 0xY1CrY0Cb */
  #define CIM_CFG_PACK_3	  (3 << CIM_CFG_PACK_BIT) /* 44 11 22 33 0xCrY0CbY1 */
  #define CIM_CFG_PACK_4	  (4 << CIM_CFG_PACK_BIT) /* 44 33 22 11 0xCrY1CbY0 */
  #define CIM_CFG_PACK_5	  (5 << CIM_CFG_PACK_BIT) /* 33 22 11 44 0xY1CbY0Cr */
  #define CIM_CFG_PACK_6	  (6 << CIM_CFG_PACK_BIT) /* 22 11 44 33 0xCbY0CrY1 */
  #define CIM_CFG_PACK_7	  (7 << CIM_CFG_PACK_BIT) /* 11 44 33 22 0xY0CrY1Cb */
#define	CIM_CFG_BYPASS_BIT	2
#define	CIM_CFG_BYPASS_MASK	(1 << CIM_CFG_BYPASS_BIT)
  #define CIM_CFG_BYPASS	  (1 << CIM_CFG_BYPASS_BIT)
#define	CIM_CFG_DSM_BIT		0
#define	CIM_CFG_DSM_MASK	(0x3 << CIM_CFG_DSM_BIT)
  #define CIM_CFG_DSM_CPM	  (0 << CIM_CFG_DSM_BIT) /* CCIR656 Progressive Mode */
  #define CIM_CFG_DSM_CIM	  (1 << CIM_CFG_DSM_BIT) /* CCIR656 Interlace Mode */
  #define CIM_CFG_DSM_GCM	  (2 << CIM_CFG_DSM_BIT) /* Gated Clock Mode */

/* CIM Control Register  (CIM_CTRL) */
#define	CIM_CTRL_EEOF_LINE_BIT	20
#define	CIM_CTRL_EEOF_LINE_MASK	(0xfff << CIM_CTRL_EEOF_LINE_BIT)
#define	CIM_CTRL_FRC_BIT	16
#define	CIM_CTRL_FRC_MASK	(0xf << CIM_CTRL_FRC_BIT)
  #define CIM_CTRL_FRC_1	  (0x0 << CIM_CTRL_FRC_BIT) /* Sample every frame */
  #define CIM_CTRL_FRC_2	  (0x1 << CIM_CTRL_FRC_BIT) /* Sample 1/2 frame */
  #define CIM_CTRL_FRC_3	  (0x2 << CIM_CTRL_FRC_BIT) /* Sample 1/3 frame */
  #define CIM_CTRL_FRC_4	  (0x3 << CIM_CTRL_FRC_BIT) /* Sample 1/4 frame */
  #define CIM_CTRL_FRC_5	  (0x4 << CIM_CTRL_FRC_BIT) /* Sample 1/5 frame */
  #define CIM_CTRL_FRC_6	  (0x5 << CIM_CTRL_FRC_BIT) /* Sample 1/6 frame */
  #define CIM_CTRL_FRC_7	  (0x6 << CIM_CTRL_FRC_BIT) /* Sample 1/7 frame */
  #define CIM_CTRL_FRC_8	  (0x7 << CIM_CTRL_FRC_BIT) /* Sample 1/8 frame */
  #define CIM_CTRL_FRC_9	  (0x8 << CIM_CTRL_FRC_BIT) /* Sample 1/9 frame */
  #define CIM_CTRL_FRC_10	  (0x9 << CIM_CTRL_FRC_BIT) /* Sample 1/10 frame */
  #define CIM_CTRL_FRC_11	  (0xA << CIM_CTRL_FRC_BIT) /* Sample 1/11 frame */
  #define CIM_CTRL_FRC_12	  (0xB << CIM_CTRL_FRC_BIT) /* Sample 1/12 frame */
  #define CIM_CTRL_FRC_13	  (0xC << CIM_CTRL_FRC_BIT) /* Sample 1/13 frame */
  #define CIM_CTRL_FRC_14	  (0xD << CIM_CTRL_FRC_BIT) /* Sample 1/14 frame */
  #define CIM_CTRL_FRC_15	  (0xE << CIM_CTRL_FRC_BIT) /* Sample 1/15 frame */
  #define CIM_CTRL_FRC_16	  (0xF << CIM_CTRL_FRC_BIT) /* Sample 1/16 frame */

#define	CIM_CTRL_DMA_EEOF	(0x1 << 15)	/* Enable EEOF interrupt */
#define	CIM_CTRL_WIN_EN		(0x1 << 14)
#define	CIM_CTRL_VDDM		(0x1 << 13) /* VDD interrupt enable */
#define	CIM_CTRL_DMA_SOFM	(0x1 << 12)
#define	CIM_CTRL_DMA_EOFM	(0x1 << 11)
#define	CIM_CTRL_DMA_STOPM	(0x1 << 10)
#define	CIM_CTRL_RXF_TRIGM	(0x1 << 9)
#define	CIM_CTRL_RXF_OFM	(0x1 << 8)
#define	CIM_CTRL_DMA_SYNC	(0x1 << 7)	/*when change DA, do frame sync */

#ifndef JZ4760B

#define	CIM_CTRL_RXF_TRIG_BIT	3
#define	CIM_CTRL_RXF_TRIG_MASK	(0xf << CIM_CTRL_RXF_TRIG_BIT) /* trigger value = (n+1)*burst_type */
#else
#define CIM_CTRL_H_SYNC         (1 << 6) /*Enable horizental sync when CIMCFG.SEP is 1*/       

#define	CIM_CTRL_PPW_BIT	3
#define	CIM_CTRL_PPW_MASK	(0x3 << CIM_CTRL_PPW_BIT)
#endif 

#define	CIM_CTRL_DMA_EN		(0x1 << 2) /* Enable DMA */
#define	CIM_CTRL_RXF_RST	(0x1 << 1) /* RxFIFO reset */
#define	CIM_CTRL_ENA		(0x1 << 0) /* Enable CIM */


#ifdef JZ4760B

#define CIM_CTRL2_OPG_BIT	4
#define CIM_CTRL2_OPG_MASK	(0x3 << CIM_CTRL2_OPG_BIT)
#define CIM_CTRL2_OPE		(1 << 2)
#define CIM_CTRL2_EME		(1 << 1)
#define CIM_CTRL2_APM		(1 << 0)

/* CIM State Register  (CIM_STATE) */
#define CIM_STATE_CR_RF_OF	(1 << 27)
#define CIM_STATE_CR_RF_TRIG	(1 << 26)
#define CIM_STATE_CR_RF_EMPTY	(1 << 25)
#define CIM_STATE_CB_RF_OF	(1 << 19)
#define CIM_STATE_CB_RF_TRIG	(1 << 18)
#define CIM_STATE_CB_RF_EMPTY	(1 << 17)
#define CIM_STATE_Y_RF_OF	(1 << 11)
#define CIM_STATE_Y_RF_TRIG	(1 << 10)
#define CIM_STATE_Y_RF_EMPTY	(1 << 9)
#endif 
/* CIM State Register  (CIM_STATE) */
#define	CIM_STATE_DMA_EEOF	(0x1 << 7) /* DMA Line EEOf irq */
#define	CIM_STATE_DMA_SOF	(0x1 << 6) /* DMA start irq */
#define	CIM_STATE_DMA_EOF	(0x1 << 5) /* DMA end irq */
#define	CIM_STATE_DMA_STOP	(0x1 << 4) /* DMA stop irq */
#define	CIM_STATE_RXF_OF	(0x1 << 3) /* RXFIFO over flow irq */
#define	CIM_STATE_RXF_TRIG	(0x1 << 2) /* RXFIFO triger meet irq */
#define	CIM_STATE_RXF_EMPTY	(0x1 << 1) /* RXFIFO empty irq */
#define	CIM_STATE_VDD		(0x1 << 0) /* CIM disabled irq */

/***wyang***/
#define CIMCR_RXF_TRIG_1BURST 	( 0x0 << 3 )
#define CLKGR_STOP_CIM			( 0x1 << 26)
#define CPCCR_CHANGE_EN			( 0x1 << 22)

/* CIM DMA Command Register (CIM_CMD) */

#define	CIM_CMD_SOFINT		(1 << 31) /* enable DMA start irq */
#define	CIM_CMD_EOFINT		(1 << 30) /* enable DMA end irq */
#define	CIM_CMD_EEOFINT		(1 << 29) /* enable DMA EEOF irq */
#define	CIM_CMD_STOP		(1 << 28) /* enable DMA stop irq */
#define	CIM_CMD_OFRCV		(1 << 27) /* enable recovery when TXFiFo overflow */
#define	CIM_CMD_LEN_BIT		0
#define	CIM_CMD_LEN_MASK	(0xffffff << CIM_CMD_LEN_BIT)

/* CIM Window-Image Size Register  (CIM_SIZE) */
#define	CIM_SIZE_LPF_BIT	16 /* Lines per freame for csc output image */
#define	CIM_SIZE_LPF_MASK	(0x1fff << CIM_SIZE_LPF_BIT)
#define	CIM_SIZE_PPL_BIT	0 /* Pixels per line for csc output image, should be an even number */
#define	CIM_SIZE_PPL_MASK	(0x1fff << CIM_SIZE_PPL_BIT)

/* CIM Image Offset Register  (CIM_OFFSET) */
#define	CIM_OFFSET_V_BIT	16 /* Vertical offset */
#define	CIM_OFFSET_V_MASK	(0xfff << CIM_OFFSET_V_BIT)
#define	CIM_OFFSET_H_BIT	0 /* Horizontal offset, should be an enen number */
#define	CIM_OFFSET_H_MASK	(0xfff << CIM_OFFSET_H_BIT) /*OFFSET_H should be even number*/

#define	CIMOFFSET(v_offset,h_offset)		((v_offset << 16)|(h_offset))
#define	CIMCR_FRC_1OF(n)		( ((n) - 1) << 16 )

#ifdef JZ4760B
#define	CIM_YCMD_SOFINT		(1 << 31) /* enable DMA start irq */
#define	CIM_YCMD_EOFINT		(1 << 30) /* enable DMA end irq */
#define	CIM_YCMD_EEOFINT	(1 << 29) /* enable DMA EEOF irq */
#define	CIM_YCMD_STOP		(1 << 28) /* enable DMA stop irq */
#define	CIM_YCMD_OFRCV		(1 << 27) /* enable recovery when TXFiFo overflow */
#define	CIM_YCMD_LEN_BIT	0
#define	CIM_YCMD_LEN_MASK	(0xffffff << CIM_YCMD_LEN_BIT)

#define	CIM_CBCMD_LEN_BIT	0
#define	CIM_CBCMD_LEN_MASK	(0xffffff << CIM_CBCMD_LEN_BIT)

#define	CIM_CRCMD_LEN_BIT	0
#define	CIM_CRCMD_LEN_MASK	(0xffffff << CIM_CRCMD_LEN_BIT)
#endif 

#ifndef __MIPS_ASSEMBLER

/***************************************************************************
 * CIM
 ***************************************************************************/

#define __cim_enable()	( REG_CIM_CTRL |= CIM_CTRL_ENA )
#define __cim_disable()	( REG_CIM_CTRL &= ~CIM_CTRL_ENA )

#ifdef JZ4760B
#define __cim_enable_sep() (REG_CIM_CFG |= CIM_CFG_SEP)
#define __cim_disable_sep() (REG_CIM_CFG &= ~CIM_CFG_SEP)
#endif 

/* n = 0, 1, 2, 3 */
#define __cim_set_input_data_stream_order(n)				\
	do {								\
		REG_CIM_CFG &= ~CIM_CFG_ORDER_MASK;			\
		REG_CIM_CFG |= ((n)<<CIM_CFG_ORDER_BIT)&CIM_CFG_ORDER_MASK; \
	} while (0)

#define __cim_input_data_format_select_RGB()	\
	do {					\
		REG_CIM_CFG &= ~CIM_CFG_DF_MASK;	\
		REG_CIM_CFG |= CIM_CFG_DF_RGB;	\
	} while (0)

#define __cim_input_data_format_select_YUV444()		\
	do {						\
		REG_CIM_CFG &= ~CIM_CFG_DF_MASK;		\
		REG_CIM_CFG |= CIM_CFG_DF_YUV444;	\
	} while (0)

#define __cim_input_data_format_select_YUV422()		\
	do {						\
		REG_CIM_CFG &= ~CIM_CFG_DF_MASK;		\
		REG_CIM_CFG |= CIM_CFG_DF_YUV422;	\
	} while (0)

#define __cim_input_data_format_select_ITU656()		\
	do {						\
		REG_CIM_CFG &= ~CIM_CFG_DF_MASK;		\
		REG_CIM_CFG |= CIM_CFG_DF_ITU656;	\
	} while (0)

#define __cim_input_data_inverse()	( REG_CIM_CFG |= CIM_CFG_INV_DAT )
#define __cim_input_data_normal()	( REG_CIM_CFG &= ~CIM_CFG_INV_DAT )

#define __cim_vsync_active_low()	( REG_CIM_CFG |= CIM_CFG_VSP )
#define __cim_vsync_active_high()	( REG_CIM_CFG &= ~CIM_CFG_VSP )

#define __cim_hsync_active_low()	( REG_CIM_CFG |= CIM_CFG_HSP )
#define __cim_hsync_active_high()	( REG_CIM_CFG &= ~CIM_CFG_HSP )

#define __cim_sample_data_at_pclk_falling_edge() \
	( REG_CIM_CFG |= CIM_CFG_PCP )
#define __cim_sample_data_at_pclk_rising_edge() \
	( REG_CIM_CFG &= ~CIM_CFG_PCP )

#define __cim_enable_dummy_zero()	( REG_CIM_CFG |= CIM_CFG_DUMMY_ZERO )
#define __cim_disable_dummy_zero()	( REG_CIM_CFG &= ~CIM_CFG_DUMMY_ZERO )

#define __cim_select_external_vsync()	( REG_CIM_CFG |= CIM_CFG_EXT_VSYNC )
#define __cim_select_internal_vsync()	( REG_CIM_CFG &= ~CIM_CFG_EXT_VSYNC )

/* n=0-7 */
#define __cim_set_data_packing_mode(n) 		\
do {						\
	REG_CIM_CFG &= ~CIM_CFG_PACK_MASK;	\
	REG_CIM_CFG |= (CIM_CFG_PACK_##n);	\
} while (0)


#ifndef JZ4760B
#define __cim_enable_bypass_func() 	(REG_CIM_CFG |= CIM_CFG_BYPASS)
#define __cim_disable_bypass_func() 	(REG_CIM_CFG &= ~CIM_CFG_BYPASS_MASK)

#else

#define __cim_enable_bypass_func() 	(REG_CIM_CFG &= ~CIM_CFG_BYPASS)
#define __cim_disable_bypass_func() 	(REG_CIM_CFG |= CIM_CFG_BYPASS)

#endif 

#define __cim_enable_ccir656_progressive_mode()	\
do {						\
	REG_CIM_CFG &= ~CIM_CFG_DSM_MASK;	\
	REG_CIM_CFG |= CIM_CFG_DSM_CPM;		\
} while (0)

#define __cim_enable_ccir656_interlace_mode()	\
do {						\
	REG_CIM_CFG &= ~CIM_CFG_DSM_MASK;	\
	REG_CIM_CFG |= CIM_CFG_DSM_CIM;		\
} while (0)

#define __cim_enable_gated_clock_mode()		\
do {						\
	REG_CIM_CFG &= ~CIM_CFG_DSM_MASK;	\
	REG_CIM_CFG |= CIM_CFG_DSM_GCM;		\
} while (0)

#define __cim_enable_nongated_clock_mode()	\
do {						\
	REG_CIM_CFG &= ~CIM_CFG_DSM_MASK;	\
	REG_CIM_CFG |= CIM_CFG_DSM_NGCM;	\
} while (0)

/* sclk:system bus clock
 * mclk: CIM master clock
 */
 
 #ifndef JZ4760B
 
#define __cim_set_master_clk(sclk, mclk)			\
do {								\
	REG_CIM_CTRL &= ~CIM_CTRL_MCLKDIV_MASK;				\
	REG_CIM_CTRL |= (((sclk)/(mclk) - 1) << CIM_CTRL_MCLKDIV_BIT);	\
} while (0)

#endif 

/* n=1-16 */
#define __cim_set_frame_rate(n) 		\
do {						\
	REG_CIM_CTRL &= ~CIM_CTRL_FRC_MASK; 	\
	REG_CIM_CTRL |= CIM_CTRL_FRC_##n;	\
} while (0)

#define __cim_enable_size_func() \
	( REG_CIM_CTRL |= CIM_CTRL_WIN_EN)
#define __cim_disable_size_func() \
	( REG_CIM_CTRL &= ~CIM_CTRL_WIN_EN )

#define __cim_enable_vdd_intr() \
	( REG_CIM_CTRL |= CIM_CTRL_VDDM )
#define __cim_disable_vdd_intr() \
	( REG_CIM_CTRL &= ~CIM_CTRL_VDDM )

#define __cim_enable_sof_intr() \
	( REG_CIM_CTRL |= CIM_CTRL_DMA_SOFM )
#define __cim_disable_sof_intr() \
	( REG_CIM_CTRL &= ~CIM_CTRL_DMA_SOFM )

#define __cim_enable_eof_intr() \
	( REG_CIM_CTRL |= CIM_CTRL_DMA_EOFM )
#define __cim_disable_eof_intr() \
	( REG_CIM_CTRL &= ~CIM_CTRL_DMA_EOFM )

#define __cim_enable_eeof_intr() \
	( REG_CIM_CTRL |= CIM_CTRL_DMA_EEOFM )
#define __cim_disable_eeof_intr() \
	( REG_CIM_CTRL &= ~CIM_CTRL_DMA_EEOFM )

#define __cim_enable_stop_intr() \
	( REG_CIM_CTRL |= CIM_CTRL_DMA_STOPM )
#define __cim_disable_stop_intr() \
	( REG_CIM_CTRL &= ~CIM_CTRL_DMA_STOPM )

#define __cim_enable_trig_intr() \
	( REG_CIM_CTRL |= CIM_CTRL_RXF_TRIGM )
#define __cim_disable_trig_intr() \
	( REG_CIM_CTRL &= ~CIM_CTRL_RXF_TRIGM )

#define __cim_enable_rxfifo_overflow_intr()	\
	( REG_CIM_CTRL |= CIM_CTRL_RXF_OFM )
#define __cim_disable_rxfifo_overflow_intr()	\
	( REG_CIM_CTRL &= ~CIM_CTRL_RXF_OFM )

#ifndef JZ4760B

/* n=4,8,12,16,20,24,28,32 */
#define __cim_set_rxfifo_trigger(n) 		\
do {						\
	REG_CIM_CTRL &= ~CIM_CTRL_RXF_TRIG_MASK; 	\
	REG_CIM_CTRL |= CIM_CTRL_RXF_TRIG_##n;		\
} while (0)
#define __cim_enable_fast_mode() 	( REG_CIM_CTRL |= CIM_CTRL_FAST_MODE )
#define __cim_disable_fast_mode() 	( REG_CIM_CTRL &= ~CIM_CTRL_FAST_MODE )
#define __cim_use_normal_mode() 	__cim_disable_fast_mode()

#endif 

#define __cim_enable_dma()   ( REG_CIM_CTRL |= CIM_CTRL_DMA_EN )
#define __cim_disable_dma()  ( REG_CIM_CTRL &= ~CIM_CTRL_DMA_EN )
#define __cim_reset_rxfifo() ( REG_CIM_CTRL |= CIM_CTRL_RXF_RST )
#define __cim_unreset_rxfifo() ( REG_CIM_CTRL &= ~CIM_CTRL_RXF_RST )
#ifdef JZ4760B

/* cim control2 */
#define __cim_enable_priority_control()		( REG_CIM_CTRL2 |= CIM_CTRL2_OPE)
#define __cim_disable_priority_control()	( REG_CIM_CTRL2 &= ~CIM_CTRL2_OPE)
#define __cim_enable_auto_priority()		( REG_CIM_CTRL2 |= CIM_CTRL2_APM)
#define __cim_disable_auto_priority()		( REG_CIM_CTRL2 &= ~CIM_CTRL2_APM)
#define __cim_enable_emergency()		( REG_CIM_CTRL2 |= CIM_CTRL2_EME)
#define __cim_disable_emergency()		( REG_CIM_CTRL2 &= ~CIM_CTRL2_EME);
/* 0, 1, 2, 3
 ** 0: highest priority
 ** 3: lowest priority
 ** 1 maybe best for SEP=1
 ** 3 maybe best for SEP=0
 **/
#define __cim_set_opg(n)				\
	do {								\
		REG_CIM_CTRL2 &= ~CIM_CTRL2_OPG_MASK;			\
		REG_CIM_CTRL2 |= ((n) << CIM_CTRL2_OPG_BIT) & CIM_CTRL2_OPG_MASK; \
	} while (0)
#endif 
#define __cim_clear_state()   	     ( REG_CIM_STATE = 0 )

#define __cim_disable_done()   	     ( REG_CIM_STATE & CIM_STATE_VDD )
#define __cim_rxfifo_empty()   	     ( REG_CIM_STATE & CIM_STATE_RXF_EMPTY )
#define __cim_rxfifo_reach_trigger() ( REG_CIM_STATE & CIM_STATE_RXF_TRIG )
#define __cim_rxfifo_overflow()      ( REG_CIM_STATE & CIM_STATE_RXF_OF )
#define __cim_clear_rxfifo_overflow() ( REG_CIM_STATE &= ~CIM_STATE_RXF_OF )
#define __cim_dma_stop()   	     ( REG_CIM_STATE & CIM_STATE_DMA_STOP )
#define __cim_dma_eof()   	     ( REG_CIM_STATE & CIM_STATE_DMA_EOF )
#define __cim_dma_sof()   	     ( REG_CIM_STATE & CIM_STATE_DMA_SOF )

#define __cim_get_iid()   	     ( REG_CIM_IID )
#define __cim_get_fid()   	     ( REG_CIM_FID )

#ifndef JZ4760B

#define __cim_get_image_data()       ( REG_CIM_RXFIFO )

#endif 


#define __cim_get_dma_cmd()          ( REG_CIM_CMD )

#define __cim_set_da(a)              ( REG_CIM_DA = (a) )

#define __cim_set_line(a) 	( REG_CIM_SIZE = (REG_CIM_SIZE&(~CIM_SIZE_LPF_MASK))|((a)<<CIM_SIZE_LPF_BIT) )
#define __cim_set_pixel(a) 	( REG_CIM_SIZE = (REG_CIM_SIZE&(~CIM_SIZE_PPL_MASK))|((a)<<CIM_SIZE_PPL_BIT) )
#define __cim_get_line() 	((REG_CIM_SIZE&CIM_SIZE_LPF_MASK)>>CIM_SIZE_LPF_BIT)
#define __cim_get_pixel() 	((REG_CIM_SIZE&CIM_SIZE_PPL_MASK)>>CIM_SIZE_PPL_BIT)

#define __cim_set_v_offset(a) 	( REG_CIM_OFFSET = (REG_CIM_OFFSET&(~CIM_OFFSET_V_MASK)) | ((a)<<CIM_OFFSET_V_BIT) )
#define __cim_set_h_offset(a) 	( REG_CIM_OFFSET = (REG_CIM_OFFSET&(~CIM_OFFSET_H_MASK)) | ((a)<<CIM_OFFSET_H_BIT) )
#define __cim_get_v_offset() 	((REG_CIM_OFFSET&CIM_OFFSET_V_MASK)>>CIM_OFFSET_V_BIT)
#define __cim_get_h_offset() 	((REG_CIM_OFFSET&CIM_OFFSET_H_MASK)>>CIM_OFFSET_H_BIT)

#define	CIMOFFSET(v_offset,h_offset)		((v_offset << 16)|(h_offset))
#define	CIMCR_FRC_1OF(n)		( ((n) - 1) << 16 )

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4760CIM_H__ */
