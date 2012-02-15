//
// Copyright (c) Ingenic Semiconductor Co., Ltd. 2007.
//

#ifndef __JZ4750LDMA_H__
#define __JZ4750LDMA_H__

//--------------------------------------------------------------------------
// DMA Address Definition
//--------------------------------------------------------------------------
#define DMA_PHYS_ADDR			( 0x13020000 )
#define DMA_BASE_U_VIRTUAL		( 0xB3020000 )

//--------------------------------------------------------------------------
// DMA Registers Offset Definition
//--------------------------------------------------------------------------

#define DMA_DSA_OFFSET			( 0x00 )        // 32, R/W, DMA source address register 
#define DMA_DTA_OFFSET			( 0x04 )        // 32, R/W, DMA destination address register 
#define DMA_DTC_OFFSET			( 0x08 )        // 32, R/W, DMA transfer count register 
#define DMA_DRT_OFFSET			( 0x0C )        // 32, R/W, DMA request source register
#define DMA_DCS_OFFSET			( 0x10 )        // 32, R/W, DMA channel control/status register
#define DMA_DCM_OFFSET			( 0x14 )        // 32, R/W, DMA command register
#define DMA_DDA_OFFSET			( 0x18 )        // 32, R/W, DMA descriptor address register

#define DMA_DMAC_OFFSET			( 0x00 )		// 32, R/W, DMA control Register.
#define DMA_DIRQP_OFFSET		( 0x04 )		// 32, R/W, DMA interrupt pending Register.
#define DMA_DDR_OFFSET			( 0x08 )		// 32, R/W, DMA doorbell Register.
#define DMA_DDRS_OFFSET			( 0x0C )		// 32, R/W, DMA doorbell set Register.
#define DMA_DCKE_OFFSET			( 0x10 )		// 32, R/W, DMA clock enable Register.

//--------------------------------------------------------------------------
// DMA Registers Address Definition
//--------------------------------------------------------------------------
#define A_DMA_DSA(x)				( DMA_BASE_U_VIRTUAL + DMA_DSA_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DTA(x)				( DMA_BASE_U_VIRTUAL + DMA_DTA_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) )
#define A_DMA_DTC(x)				( DMA_BASE_U_VIRTUAL + DMA_DTC_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) )  
#define A_DMA_DRT(x)				( DMA_BASE_U_VIRTUAL + DMA_DRT_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DCS(x)				( DMA_BASE_U_VIRTUAL + DMA_DCS_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DCM(x)				( DMA_BASE_U_VIRTUAL + DMA_DCM_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
#define A_DMA_DDA(x)				( DMA_BASE_U_VIRTUAL + DMA_DDA_OFFSET + 0x20*((x)%6) + 0x100*((x)/6) ) 
                    				
#define A_DMA_DSD(x)				( DMA_BASE_U_VIRTUAL + 0xc0 + 0x4*(x) + 0x100*((x)/6) )
 
#define A_DMA_DMAC(x)				( DMA_BASE_U_VIRTUAL + DMA_DMAC_OFFSET  + 0x100*((x)+3) )
#define A_DMA_DIRQP(x)				( DMA_BASE_U_VIRTUAL + DMA_DIRQP_OFFSET + 0x100*((x)+3) )
#define A_DMA_DDR(x)				( DMA_BASE_U_VIRTUAL + DMA_DDR_OFFSET  + 0x100*((x)+3) )
#define A_DMA_DDRS(x)				( DMA_BASE_U_VIRTUAL + DMA_DDRS_OFFSET + 0x100*((x)+3) ) 
#define A_DMA_DCKE(x)				( DMA_BASE_U_VIRTUAL + DMA_DCKE_OFFSET + 0x100*((x)+3) )

//------------------------------------------------------------------------------
// Register DRT Register field descriptions
//------------------------------------------------------------------------------

#define DRT_DREQn			( 0 )
#define DRT_NAND			( 1 )
#define DRT_BCH_EN			( 2 )
#define DRT_BCH_DE			( 3 )
#define	DRT_AUTO			( 8 )
#define DRT_TSSI			( 9 )
#define	DRT_UART3_TX		( 14 )
#define	DRT_UART3_RX		( 15 )
#define	DRT_UART2_TX		( 16 )
#define	DRT_UART2_RX		( 17 )
#define	DRT_UART1_TX		( 18 )
#define	DRT_UART1_RX		( 19 )
#define	DRT_UART0_TX		( 20 )
#define	DRT_UART0_RX		( 21 )
#define	DRT_SSI_TX			( 22 )
#define	DRT_SSI_RX			( 23 )
#define	DRT_AIC_TX			( 24 )
#define	DRT_AIC_RX			( 25 )
#define	DRT_MSC0_TX			( 26 )
#define	DRT_MSC0_RX			( 27 )
#define	DRT_TCU				( 28 )
#define	DRT_SADC			( 29 )
#define	DRT_MSC1_TX			( 30 )
#define	DRT_MSC1_RX			( 31 )
#define	DRT_SSI1_TX			( 32 )
#define	DRT_SSI1_RX			( 33 )
#define	DRT_PM_TX			( 34 )
#define	DRT_PM_RX			( 35 )


//------------------------------------------------------------------------------
// Register DCS Register field descriptions
//------------------------------------------------------------------------------
#define	DCS_NDES			( 1 << 31 )
#define	DCS_DES8			( 1 << 30 )
#define	DCS_CDOA			( 1 << 16 )
#define DCS_BERR			( 1 << 7 )
#define	DCS_INV				( 1 << 6 )
#define	DCS_AR				( 1 << 4 )
#define	DCS_TT				( 1 << 3 )
#define	DCS_HLT				( 1 << 2 )
#define	DCS_CT				( 1 << 1 )
#define	DCS_CTE				( 1 << 0 )


//------------------------------------------------------------------------------
// Register DCM Register field descriptions
//------------------------------------------------------------------------------
#define DCM_EACKS			( 1 << 31 )
#define DCM_EACKM			( 1 << 30 )
#define DCM_ERDM_LL			( 0 << 28 )
#define DCM_ERDM_FE			( 1 << 28 )
#define DCM_ERDM_HL			( 2 << 28 )
#define DCM_ERDM_RE			( 3 << 28 )

#define DCM_BLAST			( 1 << 25 )

#define	DCM_SAI				( 1 << 23 )
#define	DCM_DAI				( 1 << 22 )

#define	DCM_RDIL_IGN		( 0 << 16 )
#define	DCM_RDIL_UNIT2		( 1 << 16 )
#define	DCM_RDIL_UNIT4		( 2 << 16 )
#define	DCM_RDIL_UNIT8		( 3 << 16 )
#define	DCM_RDIL_UNIT12		( 4 << 16 )
#define	DCM_RDIL_UNIT16		( 5 << 16 )
#define	DCM_RDIL_UNIT20		( 6 << 16 )
#define	DCM_RDIL_UNIT24		( 7 << 16 )
#define	DCM_RDIL_UNIT28		( 8 << 16 )
#define	DCM_RDIL_UNIT32		( 9 << 16 )
#define	DCM_RDIL_UNIT48		( 10 << 16 )
#define	DCM_RDIL_UNIT60		( 11 << 16 )
#define	DCM_RDIL_UNIT64		( 12 << 16 )
#define	DCM_RDIL_UNIT124		( 13 << 16 )
#define	DCM_RDIL_UNIT128		( 14 << 16 )
#define	DCM_RDIL_UNIT200		( 15 << 16 )

#define DCM_SP_MASK				( 3 << 14 )
#define	DCM_SP_32BIT			( 0 << 14 )
#define	DCM_SP_08BIT			( 1 << 14 )
#define	DCM_SP_16BIT			( 2 << 14 )

#define DCM_DP_MASK					( 3 << 12 )
#define	DCM_DP_32BIT				( 0 << 12 )
#define	DCM_DP_08BIT				( 1 << 12 )
#define	DCM_DP_16BIT				( 2 << 12 )

#define DCM_TSZ_MASK			( 0x7 << 8)
#define	DCM_TSZ_32BIT			( 0 << 8 )
#define	DCM_TSZ_08BIT			( 1 << 8 )
#define	DCM_TSZ_16BIT			( 2 << 8 )
#define	DCM_TSZ_16BYTE			( 3 << 8 )
#define	DCM_TSZ_32BYTE			( 4 << 8 )

#define	DCM_BLOCK_MODE			( 1 << 7 )
#define	DCM_SINGLE_MODE			( 0 << 7 )
#define DCD_STRIDE_EN			( 1 << 5 )
#define	DCM_DESC_VALID			( 1 << 4 )
#define	DCM_DESC_VALID_MODE		( 1 << 3 )
#define	DCM_VALID_INTR_EN		( 1 << 2 )
#define	DCM_TRANS_INTR_EN		( 1 << 1 )
#define	DCM_LINK_EN				( 1 << 0 )

//------------------------------------------------------------------------------
// Register DMAC field descriptions
//------------------------------------------------------------------------------
#define DMAC_FAST_MSC		( 1 << 31 )
#define DMAC_FAST_SSI		( 1 << 30 )
#define DMAC_FAST_TSSI		( 1 << 29 )
#define DMAC_FAST_UART		( 1 << 28 )
#define DMAC_FAST_AIC		( 1 << 27 )

#define	DMAC_PM_0			( 0 << 8 )
#define	DMAC_PM_1			( 1 << 8 )
#define	DMAC_PM_2			( 2 << 8 )
#define	DMAC_PM_3			( 3 << 8 )

#define	DMAC_HALT			( 1 << 3 )
#define	DMAC_ADDR_ERR		( 1 << 2 )
#define	DMAC_INT_EN			( 1 << 1 )
#define	DMAC_DMA_EN			( 1 << 0 )

//------------------------------------------------------------------------------
// Register DIRQP field descriptions
//------------------------------------------------------------------------------
#define DIRQP_CIRQ5			( 1 << 5 )
#define DIRQP_CIRQ4			( 1 << 4 )
#define DIRQP_CIRQ3			( 1 << 3 )
#define DIRQP_CIRQ2			( 1 << 2 )
#define DIRQP_CIRQ1			( 1 << 1 )
#define DIRQP_CIRQ0			( 1 << 0 )

//------------------------------------------------------------------------------
// Register DCKE field descriptions
//------------------------------------------------------------------------------
#define DMAC_DCKE_DCKE5		( 1 << 5 )
#define DMAC_DCKE_DCKE4		( 1 << 4 )
#define DMAC_DCKE_DCKE3		( 1 << 3 )
#define DMAC_DCKE_DCKE2		( 1 << 2 )
#define DMAC_DCKE_DCKE1		( 1 << 1 )
#define DMAC_DCKE_DCKE0		( 1 << 0 )
//------------------------------------------------------------------------------
// Descriptor DES0 structure descriptions
//------------------------------------------------------------------------------
#define DES_EACKS			( 1 << 31 )
#define DES_EACKM			( 1 << 30 )

#define DES_ERDM_MASK		( 3 << 28)
#define DES_ERDM_LL			( 0 << 28 )
#define DES_ERDM_FE			( 1 << 28 )
#define DES_ERDM_HL			( 2 << 28 )
#define DES_ERDM_RE			( 3 << 28 )

#define DES_EOPM			( 1 << 27 )
#define DES_BLAST			( 1 << 25 )
#define	DES_SAI				( 1 << 23 )
#define	DES_DAI				( 1 << 22 )

#define DES_RDIL_MASK		( 0xF << 16)
#define	DES_RDIL_IGN		( 0 << 16 )
#define	DES_RDIL_UNIT2		( 1 << 16 )
#define	DES_RDIL_UNIT4		( 2 << 16 )
#define	DES_RDIL_UNIT8		( 3 << 16 )
#define	DES_RDIL_UNIT12		( 4 << 16 )
#define	DES_RDIL_UNIT16		( 5 << 16 )
#define	DES_RDIL_UNIT20		( 6 << 16 )
#define	DES_RDIL_UNIT24		( 7 << 16 )
#define	DES_RDIL_UNIT28		( 8 << 16 )
#define	DES_RDIL_UNIT32		( 9 << 16 )
#define	DES_RDIL_UNIT48		( 10 << 16 )
#define	DES_RDIL_UNIT60		( 11 << 16 )
#define	DES_RDIL_UNIT64		( 12 << 16 )
#define	DES_RDIL_UNIT124	( 13 << 16 )
#define	DES_RDIL_UNIT128	( 14 << 16 )
#define	DES_RDIL_UNIT200	( 15 << 16 )

#define DES_SP_MASK			( 3 << 14)
#define	DES_SP_32BIT		( 0 << 14 )
#define	DES_SP_08BIT		( 1 << 14 )
#define	DES_SP_16BIT		( 2 << 14 )

#define DES_DP_MASK				( 3 << 12)
#define	DES_DP_32BIT			( 0 << 12 )
#define	DES_DP_08BIT			( 1 << 12 )
#define	DES_DP_16BIT			( 2 << 12 )

#define DES_TSZ_MASK			( 7 << 8)
#define	DES_TSZ_32BIT			( 0 << 8 )
#define	DES_TSZ_08BIT			( 1 << 8 )
#define	DES_TSZ_16BIT			( 2 << 8 )
#define	DES_TSZ_16BYTE			( 3 << 8 )
#define	DES_TSZ_32BYTE			( 4 << 8 )

#define	DES_TM					( 1 << 7 )
#define DCD_STRIDE_EN			( 1 << 5 )
#define	DES_DESC_VALID			( 1 << 4 )
#define	DES_DESC_VALID_MODE		( 1 << 3 )
#define	DES_VALID_INT_EN		( 1 << 2 )
#define	DES_TRANS_INT_EN		( 1 << 1 )
#define	DES_LINK_EN				( 1 << 0 )

#define	DMA_CHANNEL_OFFSET		( 0x20 )
#define	DMA_CHANNEL_NUM			( 6 )
#define DMA_CONTROLLER_OFFSET	( 0X100 )
#define DMA_CONTROLLER_NUM		( 2 )
//#define NUM_DMA (DMA_CONTROLLER_NUM*DMA_CHANNEL_NUM)

   
#endif // __JZ4725DMA_H__
