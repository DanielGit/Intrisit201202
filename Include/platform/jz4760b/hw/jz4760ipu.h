/*
 * linux/include/asm-mips/mach-jz4760/jz4760ipu.h
 *
 * JZ4760 IPU register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4760IPU_H__
#define __JZ4760IPU_H__


#define	IPU_BASE	0xB3080000
//#define ipu_vbase	0xB3080000

/*ipu registers*/
#define IPU_CTRL				(IPU_BASE + 0x0)   /* IPU Control Register */
#define IPU_STATUS				(IPU_BASE + 0x4)   /* IPU Status Register */
#define IPU_D_FMT				(IPU_BASE + 0x8)   /* Data Format Register */
#define IPU_Y_ADDR				(IPU_BASE + 0xc)   /* Input Y or YUV422 Packaged Data Address Register */
#define IPU_U_ADDR				(IPU_BASE + 0x10)  /* Input U Data Address Register */
#define IPU_V_ADDR				(IPU_BASE + 0x14)  /* Input V Data Address Register */
#define IPU_IN_FM_GS			(IPU_BASE + 0x18)  /* Input Geometric Size Register */
#define IPU_Y_STRIDE			(IPU_BASE + 0x1c)  /* Input Y Data Line Stride Register */
#define IPU_UV_STRIDE			(IPU_BASE + 0x20)  /* Input UV Data Line Stride Register */
#define IPU_OUT_ADDR			(IPU_BASE + 0x24)  /* Output Frame Start Address Register */
#define IPU_OUT_GS				(IPU_BASE + 0x28)  /* Output Geometric Size Register */
#define IPU_OUT_STRIDE			(IPU_BASE + 0x2c)  /* Output Data Line Stride Register */
#define IPU_RSZ_COEF_INDEX		(IPU_BASE + 0x30)  /* Resize Coefficients Table Index Register */
#define IPU_CSC_C0_COEF			(IPU_BASE + 0x34)  /* CSC C0 Coefficient Register */
#define IPU_CSC_C1_COEF			(IPU_BASE + 0x38)  /* CSC C1 Coefficient Register */
#define IPU_CSC_C2_COEF 		(IPU_BASE + 0x3c)  /* CSC C2 Coefficient Register */
#define IPU_CSC_C3_COEF 		(IPU_BASE + 0x40)  /* CSC C3 Coefficient Register */
#define IPU_CSC_C4_COEF 		(IPU_BASE + 0x44)  /* CSC C4 Coefficient Register */
#define IPU_HRSZ_LUT_BASE 		(IPU_BASE + 0x48)  /* Horizontal Resize Coefficients Look Up Table Register group */
#define IPU_VRSZ_LUT_BASE 		(IPU_BASE + 0x4c)  /* Virtical Resize Coefficients Look Up Table Register group */
#define IPU_CSC_OFSET_PARA		(IPU_BASE + 0x50)  /* CSC Offset Parameter Register */
#define IPU_SRC_TLB_ADDR		(IPU_BASE + 0x54)  /* base address of src Y Physical address map table Register */
#define IPU_DEST_TLB_ADDR		(IPU_BASE + 0x58)  /* base address of dst Physical address map table Register */
#define IPU_TLB_MONITOR			(IPU_BASE + 0x60)  /* TLB Monitor Register */
#define IPU_ADDR_CTRL			(IPU_BASE + 0x64)  /* IPU address set Register */
#define IPU_Y_ADDR_N			(IPU_BASE + 0x84)  /* src Y base address of next frame Register */
#define IPU_U_ADDR_N			(IPU_BASE + 0x88)  /* src U base address of next frame Register */
#define IPU_V_ADDR_N			(IPU_BASE + 0x8c)  /* src V base address of next frame Register */
#define IPU_OUT_ADDR_N			(IPU_BASE + 0x90)  /* result frame base address of next frame Register */
#define IPU_SRC_TLB_ADDR_N		(IPU_BASE + 0x94)  /* base address of src Y Physical address map table for next frame Register*/
#define IPU_DEST_TLB_ADDR_N		(IPU_BASE + 0x98)  /* base address of dst Physical address map table for next frame Register */
#define IPU_TLB_CTRL			(IPU_BASE + 0x68)  /* TLB Control Register*/
//qhliu

/*************************************************************************
 * IPU (Image Processing Unit)
 *************************************************************************/
 #if 0
#define IPU_V_BASE		0xB3080000
#define IPU_P_BASE		0x13080000

/* Register offset */
#define REG_CTRL		0x0  /* IPU Control Register */
#define REG_STATUS		0x4  /* IPU Status Register */
#define REG_D_FMT		0x8  /* Data Format Register */
#define REG_Y_ADDR		0xc  /* Input Y or YUV422 Packaged Data Address Register */
#define REG_U_ADDR		0x10 /* Input U Data Address Register */
#define REG_V_ADDR		0x14 /* Input V Data Address Register */
#define REG_IN_FM_GS		0x18 /* Input Geometric Size Register */
#define REG_Y_STRIDE		0x1c /* Input Y Data Line Stride Register */
#define REG_UV_STRIDE		0x20 /* Input UV Data Line Stride Register */
#define REG_OUT_ADDR		0x24 /* Output Frame Start Address Register */
#define REG_OUT_GS		0x28 /* Output Geometric Size Register */
#define REG_OUT_STRIDE		0x2c /* Output Data Line Stride Register */
#define REG_RSZ_COEF_INDEX	0x30 /* Resize Coefficients Table Index Register */
#define REG_CSC_CO_COEF		0x34 /* CSC C0 Coefficient Register */
#define REG_CSC_C1_COEF		0x38 /* CSC C1 Coefficient Register */
#define REG_CSC_C2_COEF 	0x3c /* CSC C2 Coefficient Register */
#define REG_CSC_C3_COEF 	0x40 /* CSC C3 Coefficient Register */
#define REG_CSC_C4_COEF 	0x44 /* CSC C4 Coefficient Register */
#define HRSZ_LUT_BASE 		0x48 /* Horizontal Resize Coefficients Look Up Table Register group */
#define VRSZ_LUT_BASE 		0x4c /* Virtical Resize Coefficients Look Up Table Register group */
#define REG_CSC_OFSET_PARA	0x50 /* CSC Offset Parameter Register */
#define REG_Y_PHY_T_ADDR	0x54 /* Input Y Physical Table Address Register */
#define REG_U_PHY_T_ADDR	0x58 /* Input U Physical Table Address Register */
#define REG_V_PHY_T_ADDR	0x5c /* Input V Physical Table Address Register */
#define REG_OUT_PHY_T_ADDR	0x60 /* Output Physical Table Address Register */
#endif
/* REG_CTRL: IPU Control Register */
#define IPU_CE_SFT	0x0
#define IPU_CE_MSK	0x1
#define IPU_RUN_SFT	0x1
#define IPU_RUN_MSK	0x1
#define HRSZ_EN_SFT	0x2
#define HRSZ_EN_MSK	0x1
#define VRSZ_EN_SFT	0x3
#define VRSZ_EN_MSK	0x1
#define CSC_EN_SFT	0x4
#define CSC_EN_MSK	0x1
#define FM_IRQ_EN_SFT	0x5
#define FM_IRQ_EN_MSK	0x1
#define IPU_RST_SFT	0x6
#define IPU_RST_MSK	0x1
#define H_SCALE_SFT	0x8
#define H_SCALE_MSK	0x1
#define V_SCALE_SFT	0x9
#define V_SCALE_MSK	0x1
#define PKG_SEL_SFT	0xA
#define PKG_SEL_MSK	0x1
#define LCDC_SEL_SFT	0xB
#define LCDC_SEL_MSK	0x1
#define SPAGE_MAP_SFT	0xC
#define SPAGE_MAP_MSK	0x1
#define DPAGE_SEL_SFT	0xD
#define DPAGE_SEL_MSK	0x1
#define DISP_SEL_SFT	0xE
#define DISP_SEL_MSK	0x1
#define FIELD_CONF_EN_SFT 15
#define FIELD_CONF_EN_MSK 1
#define FIELD_SEL_SFT	16
#define FIELD_SEL_MSK	1
#define DFIX_SEL_SFT	17
#define DFIX_SEL_MSK	1
//qhliu
#define ZOOM_SEL_SFT	18
#define ZOOM_SEL_MSK	1
#define BURST_SEL_SFT	19
#define BURST_SEL_MSK	1
#define ADDR_SEL_SFT	20
#define ADDR_SEL_MSK	1
#define CONF_MODE_SFT	21
#define CONF_MODE_MSK	1
#define BUS_OPT_SFT		22
#define BUS_OPT_MSK	1
//qhliu

/* REG_STATUS: IPU Status Register */
#define OUT_END_SFT	0x0
#define OUT_END_MSK	0x1
#define FMT_ERR_SFT	0x1
#define FMT_ERR_MSK	0x1
#define SIZE_ERR_SFT	0x2
#define SIZE_ERR_MSK	0x1

/* D_FMT: Data Format Register */
#define IN_FMT_SFT	0x0
#define IN_FMT_MSK 	0x3
#define IN_OFT_SFT 	0x2
#define IN_OFT_MSK 	0x3
#define YUV_PKG_OUT_SFT	0x10
#define YUV_PKG_OUT_MSK	0x7
#define OUT_FMT_SFT 	0x13
#define OUT_FMT_MSK 	0x3
#define RGB_OUT_OFT_SFT	0x15
#define RGB_OUT_OFT_MSK	0x7
#define RGB888_FMT_SFT	0x18
#define RGB888_FMT_MSK	0x1

/* IN_FM_GS: Input Geometric Size Register */
#define IN_FM_H_SFT	0x0
#define IN_FM_H_MSK	0xFFF
#define IN_FM_W_SFT	0x10
#define IN_FM_W_MSK	0xFFF

/* Y_STRIDE: Input Y Data Line Stride Register */
#define Y_S_SFT		0x0
#define Y_S_MSK		0x3FFF

/* UV_STRIDE: Input UV Data Line Stride Register */
#define V_S_SFT		0x0
#define V_S_MSK		0x1FFF
#define U_S_SFT 	0x10
#define U_S_MSK		0x1FFF

/* OUT_GS: Output Geometric Size Register */
#define OUT_FM_H_SFT	0x0
#define OUT_FM_H_MSK	0x1FFF
#define OUT_FM_W_SFT	0x10
#define OUT_FM_W_MSK	0x7FFF

/* OUT_STRIDE: Output Data Line Stride Register */
#define OUT_S_SFT	0x0
#define OUT_S_MSK	0xFFFF

/* RSZ_COEF_INDEX: Resize Coefficients Table Index Register */
#define VE_IDX_SFT	0x0
#define VE_IDX_MSK	0x1F
#define HE_IDX_SFT	0x10
#define HE_IDX_MSK	0x1F

/* CSC_CX_COEF: CSC CX Coefficient Register */
#define CX_COEF_SFT	0x0
#define CX_COEF_MSK	0xFFF

/* HRSZ_LUT_BASE, VRSZ_LUT_BASE: Resize Coefficients Look Up Table Register group */
#define LUT_LEN		20

#define OUT_N_SFT	0x0
#define OUT_N_MSK	0x1
#define IN_N_SFT	0x1
#define IN_N_MSK	0x1
#define W_COEF_SFT	0x2
#define W_COEF_MSK	0x3FF

/* CSC_OFSET_PARA: CSC Offset Parameter Register */
#define CHROM_OF_SFT	0x10
#define CHROM_OF_MSK	0xFF
#define LUMA_OF_SFT	0x00
#define LUMA_OF_MSK	0xFF


#define W_CUBE_COEF0_SFT 0x6
#define W_CUBE_COEF0_MSK 0x7FF
#define W_CUBE_COEF1_SFT 0x11
#define W_CUBE_COEF1_MSK 0x7FF


#ifndef __MIPS_ASSEMBLER




#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4760IPU_H__ */
