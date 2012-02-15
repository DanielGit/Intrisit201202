/*
 * linux/include/asm-mips/mach-jz4760/jz4760aic.h
 *
 * JZ4760 AIC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4760AIC_H__
#define __JZ4760AIC_H__


#define	AIC_BASE	0xB0020000
#define	ICDC_BASE	0xB0020000


/*************************************************************************
 * AIC (AC97/I2S Controller)
 *************************************************************************/
#define	AIC_FR			(AIC_BASE + 0x000)
#define	AIC_CR			(AIC_BASE + 0x004)
#define	AIC_ACCR1		(AIC_BASE + 0x008)
#define	AIC_ACCR2		(AIC_BASE + 0x00C)
#define	AIC_I2SCR		(AIC_BASE + 0x010)
#define	AIC_SR			(AIC_BASE + 0x014)
#define	AIC_ACSR		(AIC_BASE + 0x018)
#define	AIC_I2SSR		(AIC_BASE + 0x01C)
#define	AIC_ACCAR		(AIC_BASE + 0x020)
#define	AIC_ACCDR		(AIC_BASE + 0x024)
#define	AIC_ACSAR		(AIC_BASE + 0x028)
#define	AIC_ACSDR		(AIC_BASE + 0x02C)
#define	AIC_I2SDIV		(AIC_BASE + 0x030)
#define	AIC_DR			(AIC_BASE + 0x034)

#define	REG_AIC_FR		REG32(AIC_FR)
#define	REG_AIC_CR		REG32(AIC_CR)
#define	REG_AIC_ACCR1		REG32(AIC_ACCR1)
#define	REG_AIC_ACCR2		REG32(AIC_ACCR2)
#define	REG_AIC_I2SCR		REG32(AIC_I2SCR)
#define	REG_AIC_SR		REG32(AIC_SR)
#define	REG_AIC_ACSR		REG32(AIC_ACSR)
#define	REG_AIC_I2SSR		REG32(AIC_I2SSR)
#define	REG_AIC_ACCAR		REG32(AIC_ACCAR)
#define	REG_AIC_ACCDR		REG32(AIC_ACCDR)
#define	REG_AIC_ACSAR		REG32(AIC_ACSAR)
#define	REG_AIC_ACSDR		REG32(AIC_ACSDR)
#define	REG_AIC_I2SDIV		REG32(AIC_I2SDIV)
#define	REG_AIC_DR		REG32(AIC_DR)

/* AIC Controller Configuration Register (AIC_FR) */

#define	AIC_FR_RFTH_BIT	24        /* Receive FIFO Threshold */
#define	AIC_FR_RFTH(x)	( ( (x)/2 - 1 ) << AIC_FR_RFTH_BIT)		// 2, 4, ..., 32
#define	AIC_FR_RFTH_MASK	(0xf << AIC_FR_RFTH_BIT)
#define	AIC_FR_TFTH_BIT	16	   /* Transmit FIFO Threshold */
#define	AIC_FR_TFTH(x)		( ( (x)/2 ) << AIC_FR_TFTH_BIT)		// 2, 4, ..., 32
#define	AIC_FR_TFTH_MASK	(0x1f << AIC_FR_TFTH_BIT)
#define	AIC_FR_LSMP		(1 << 6)  /* Play Zero sample or last sample */
#define	AIC_FR_ICDC		(1 << 5)  /* External(0) or Internal CODEC(1) */
#define	AIC_FR_AUSEL		(1 << 4)  /* AC97(0) or I2S/MSB-justified(1) */
#define	AIC_FR_RST		(1 << 3)  /* AIC registers reset */
#define	AIC_FR_BCKD		(1 << 2)  /* I2S BIT_CLK direction, 0:input,1:output */
#define	AIC_FR_SYNCD		(1 << 1)  /* I2S SYNC direction, 0:input,1:output */
#define	AIC_FR_ENB		(1 << 0)  /* AIC enable bit */

/* AIC Controller Common Control Register (AIC_CR) */

#define AIC_CR_EN2OLD           (1 << 29)  /* Enable old style */
#define AIC_CR_PACK16           (1 << 28)  /* Output Sample data 16bit packed mode select */
#define AIC_CR_CHANNEL_BIT      24  /* Output Channel Number Select */
#define AIC_CR_CHANNEL_MASK     (0x7 << AIC_CR_CHANNEL_BIT)
  #define AIC_CR_CHANNEL_MONO   (0x0 << AIC_CR_CHANNEL_BIT)
  #define AIC_CR_CHANNEL_STEREO (0x1 << AIC_CR_CHANNEL_BIT)
  #define AIC_CR_CHANNEL_4CHNL  (0x3 << AIC_CR_CHANNEL_BIT)
  #define AIC_CR_CHANNEL_6CHNL  (0x5 << AIC_CR_CHANNEL_BIT)
  #define AIC_CR_CHANNEL_8CHNL  (0x7 << AIC_CR_CHANNEL_BIT)

#define	AIC_CR_OSS_BIT		19  /* Output Sample Size from memory (AIC V2 only) */
#define	AIC_CR_OSS_MASK		(0x7 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_8BIT	(0x0 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_16BIT	(0x1 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_18BIT	(0x2 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_20BIT	(0x3 << AIC_CR_OSS_BIT)
  #define AIC_CR_OSS_24BIT	(0x4 << AIC_CR_OSS_BIT)
#define	AIC_CR_ISS_BIT		16  /* Input Sample Size from memory (AIC V2 only) */
#define	AIC_CR_ISS_MASK		(0x7 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_8BIT	(0x0 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_16BIT	(0x1 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_18BIT	(0x2 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_20BIT	(0x3 << AIC_CR_ISS_BIT)
  #define AIC_CR_ISS_24BIT	(0x4 << AIC_CR_ISS_BIT)
#define	AIC_CR_RDMS		(1 << 15)  /* Receive DMA enable */
#define	AIC_CR_TDMS		(1 << 14)  /* Transmit DMA enable */
#define	AIC_CR_M2S		(1 << 11)  /* Mono to Stereo enable */
#define	AIC_CR_ENDSW		(1 << 10)  /* Endian switch enable */
#define	AIC_CR_AVSTSU		(1 << 9)   /* Signed <-> Unsigned toggle enable */
#define	AIC_CR_TFLUSH		(1 << 8)   /* Flush TFIFO */
#define	AIC_CR_RFLUSH		(1 << 7)   /* Flush RFIFO */
#define	AIC_CR_EROR		(1 << 6)   /* Enable ROR interrupt */
#define	AIC_CR_ETUR		(1 << 5)   /* Enable TUR interrupt */
#define	AIC_CR_ERFS		(1 << 4)   /* Enable RFS interrupt */
#define	AIC_CR_ETFS		(1 << 3)   /* Enable TFS interrupt */
#define	AIC_CR_ENLBF		(1 << 2)   /* Enable Loopback Function */
#define	AIC_CR_ERPL		(1 << 1)   /* Enable Playback Function */
#define	AIC_CR_EREC		(1 << 0)   /* Enable Record Function */

/* AIC Controller AC-link Control Register 1 (AIC_ACCR1) */

#define	AIC_ACCR1_RS_BIT	16          /* Receive Valid Slots */
#define	AIC_ACCR1_RS_MASK	(0x3ff << AIC_ACCR1_RS_BIT)
  #define AIC_ACCR1_RS_SLOT12	  (1 << 25) /* Slot 12 valid bit */
  #define AIC_ACCR1_RS_SLOT11	  (1 << 24) /* Slot 11 valid bit */
  #define AIC_ACCR1_RS_SLOT10	  (1 << 23) /* Slot 10 valid bit */
  #define AIC_ACCR1_RS_SLOT9	  (1 << 22) /* Slot 9 valid bit, LFE */
  #define AIC_ACCR1_RS_SLOT8	  (1 << 21) /* Slot 8 valid bit, Surround Right */
  #define AIC_ACCR1_RS_SLOT7	  (1 << 20) /* Slot 7 valid bit, Surround Left */
  #define AIC_ACCR1_RS_SLOT6	  (1 << 19) /* Slot 6 valid bit, PCM Center */
  #define AIC_ACCR1_RS_SLOT5	  (1 << 18) /* Slot 5 valid bit */
  #define AIC_ACCR1_RS_SLOT4	  (1 << 17) /* Slot 4 valid bit, PCM Right */
  #define AIC_ACCR1_RS_SLOT3	  (1 << 16) /* Slot 3 valid bit, PCM Left */
#define	AIC_ACCR1_XS_BIT	0          /* Transmit Valid Slots */
#define	AIC_ACCR1_XS_MASK	(0x3ff << AIC_ACCR1_XS_BIT)
  #define AIC_ACCR1_XS_SLOT12	  (1 << 9) /* Slot 12 valid bit */
  #define AIC_ACCR1_XS_SLOT11	  (1 << 8) /* Slot 11 valid bit */
  #define AIC_ACCR1_XS_SLOT10	  (1 << 7) /* Slot 10 valid bit */
  #define AIC_ACCR1_XS_SLOT9	  (1 << 6) /* Slot 9 valid bit, LFE */
  #define AIC_ACCR1_XS_SLOT8	  (1 << 5) /* Slot 8 valid bit, Surround Right */
  #define AIC_ACCR1_XS_SLOT7	  (1 << 4) /* Slot 7 valid bit, Surround Left */
  #define AIC_ACCR1_XS_SLOT6	  (1 << 3) /* Slot 6 valid bit, PCM Center */
  #define AIC_ACCR1_XS_SLOT5	  (1 << 2) /* Slot 5 valid bit */
  #define AIC_ACCR1_XS_SLOT4	  (1 << 1) /* Slot 4 valid bit, PCM Right */
  #define AIC_ACCR1_XS_SLOT3	  (1 << 0) /* Slot 3 valid bit, PCM Left */

/* AIC Controller AC-link Control Register 2 (AIC_ACCR2) */

#define	AIC_ACCR2_ERSTO		(1 << 18) /* Enable RSTO interrupt */
#define	AIC_ACCR2_ESADR		(1 << 17) /* Enable SADR interrupt */
#define	AIC_ACCR2_ECADT		(1 << 16) /* Enable CADT interrupt */
#define	AIC_ACCR2_OASS_BIT	8  /* Output Sample Size for AC-link */
#define	AIC_ACCR2_OASS_MASK	(0x3 << AIC_ACCR2_OASS_BIT)
  #define AIC_ACCR2_OASS_20BIT	  (0 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 20-bit */
  #define AIC_ACCR2_OASS_18BIT	  (1 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 18-bit */
  #define AIC_ACCR2_OASS_16BIT	  (2 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 16-bit */
  #define AIC_ACCR2_OASS_8BIT	  (3 << AIC_ACCR2_OASS_BIT) /* Output Audio Sample Size is 8-bit */
#define	AIC_ACCR2_IASS_BIT	6  /* Output Sample Size for AC-link */
#define	AIC_ACCR2_IASS_MASK	(0x3 << AIC_ACCR2_IASS_BIT)
  #define AIC_ACCR2_IASS_20BIT	  (0 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 20-bit */
  #define AIC_ACCR2_IASS_18BIT	  (1 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 18-bit */
  #define AIC_ACCR2_IASS_16BIT	  (2 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 16-bit */
  #define AIC_ACCR2_IASS_8BIT	  (3 << AIC_ACCR2_IASS_BIT) /* Input Audio Sample Size is 8-bit */
#define	AIC_ACCR2_SO		(1 << 3)  /* SDATA_OUT output value */
#define	AIC_ACCR2_SR		(1 << 2)  /* RESET# pin level */
#define	AIC_ACCR2_SS		(1 << 1)  /* SYNC pin level */
#define	AIC_ACCR2_SA		(1 << 0)  /* SYNC and SDATA_OUT alternation */

/* AIC Controller I2S/MSB-justified Control Register (AIC_I2SCR) */

#define	AIC_I2SCR_RFIRST	(1 << 17) /* Send R channel first in stereo mode */
#define	AIC_I2SCR_SWLH   	(1 << 16) /* Switch LR channel in 16bit-packed stereo mode */ 
#define	AIC_I2SCR_STPBK		(1 << 12) /* Stop BIT_CLK for I2S/MSB-justified */
#define	AIC_I2SCR_WL_BIT	1  /* Input/Output Sample Size for I2S/MSB-justified */
#define	AIC_I2SCR_WL_MASK	(0x7 << AIC_I2SCR_WL_BIT)
  #define AIC_I2SCR_WL_24BIT	  (0 << AIC_I2SCR_WL_BIT) /* Word Length is 24 bit */
  #define AIC_I2SCR_WL_20BIT	  (1 << AIC_I2SCR_WL_BIT) /* Word Length is 20 bit */
  #define AIC_I2SCR_WL_18BIT	  (2 << AIC_I2SCR_WL_BIT) /* Word Length is 18 bit */
  #define AIC_I2SCR_WL_16BIT	  (3 << AIC_I2SCR_WL_BIT) /* Word Length is 16 bit */
  #define AIC_I2SCR_WL_8BIT	  (4 << AIC_I2SCR_WL_BIT) /* Word Length is 8 bit */

#define AIC_I2SCR_ESCLK		(1 << 4)

#define	AIC_I2SCR_AMSL		(1 << 0) /* 0:I2S, 1:MSB-justified */

/* AIC Controller FIFO Status Register (AIC_SR) */

#define	AIC_SR_RFL_BIT		24  /* Receive FIFO Level */
#define	AIC_SR_RFL_MASK		(0x3f << AIC_SR_RFL_BIT)
#define	AIC_SR_TFL_BIT		8   /* Transmit FIFO level */
#define	AIC_SR_TFL_MASK		(0x3f << AIC_SR_TFL_BIT)
#define	AIC_SR_ROR		(1 << 6) /* Receive FIFO Overrun */
#define	AIC_SR_TUR		(1 << 5) /* Transmit FIFO Underrun */
#define	AIC_SR_RFS		(1 << 4) /* Receive FIFO Service Request */
#define	AIC_SR_TFS		(1 << 3) /* Transmit FIFO Service Request */

/* AIC Controller AC-link Status Register (AIC_ACSR) */

#define	AIC_ACSR_SLTERR		(1 << 21) /* Slot Error Flag */
#define	AIC_ACSR_CRDY		(1 << 20) /* External CODEC Ready Flag */
#define	AIC_ACSR_CLPM		(1 << 19) /* External CODEC low power mode flag */
#define	AIC_ACSR_RSTO		(1 << 18) /* External CODEC regs read status timeout */
#define	AIC_ACSR_SADR		(1 << 17) /* External CODEC regs status addr and data received */
#define	AIC_ACSR_CADT		(1 << 16) /* Command Address and Data Transmitted */

/* AIC Controller I2S/MSB-justified Status Register (AIC_I2SSR) */

#define	AIC_I2SSR_BSY		(1 << 2)  /* AIC Busy in I2S/MSB-justified format */

/* AIC Controller AC97 codec Command Address Register (AIC_ACCAR) */

#define	AIC_ACCAR_CAR_BIT	0
#define	AIC_ACCAR_CAR_MASK	(0xfffff << AIC_ACCAR_CAR_BIT)

/* AIC Controller AC97 codec Command Data Register (AIC_ACCDR) */

#define	AIC_ACCDR_CDR_BIT	0
#define	AIC_ACCDR_CDR_MASK	(0xfffff << AIC_ACCDR_CDR_BIT)

/* AIC Controller AC97 codec Status Address Register (AIC_ACSAR) */

#define	AIC_ACSAR_SAR_BIT	0
#define	AIC_ACSAR_SAR_MASK	(0xfffff << AIC_ACSAR_SAR_BIT)

/* AIC Controller AC97 codec Status Data Register (AIC_ACSDR) */

#define	AIC_ACSDR_SDR_BIT	0
#define	AIC_ACSDR_SDR_MASK	(0xfffff << AIC_ACSDR_SDR_BIT)

/* AIC Controller I2S/MSB-justified Clock Divider Register (AIC_I2SDIV) */

#define	AIC_I2SDIV_DIV_BIT	0
#define	AIC_I2SDIV_DIV_MASK	(0x7f << AIC_I2SDIV_DIV_BIT)
  #define AIC_I2SDIV_BITCLK_3072KHZ	(0x0C << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 3.072MHz */
  #define AIC_I2SDIV_BITCLK_2836KHZ	(0x0D << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 2.836MHz */
  #define AIC_I2SDIV_BITCLK_1418KHZ	(0x1A << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 1.418MHz */
  #define AIC_I2SDIV_BITCLK_1024KHZ	(0x24 << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 1.024MHz */
  #define AIC_I2SDIV_BITCLK_7089KHZ	(0x34 << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 708.92KHz */
  #define AIC_I2SDIV_BITCLK_512KHZ	(0x48 << AIC_I2SDIV_DIV_BIT) /* BIT_CLK of 512.00KHz */



/*************************************************************************
 * ICDC (Internal CODEC)
 *************************************************************************/

#define	ICDC_CKCFG	  (ICDC_BASE + 0x00a0)  /* Clock Configure Register */
#define	ICDC_RGADW	  (ICDC_BASE + 0x00a4)  /* internal register access control */
#define	ICDC_RGDATA	  (ICDC_BASE + 0x00a8)  /* internal register data output */

#define	REG_ICDC_CKCFG		REG32(ICDC_CKCFG)
#define	REG_ICDC_RGADW		REG32(ICDC_RGADW)
#define	REG_ICDC_RGDATA		REG32(ICDC_RGDATA)

/* ICDC Clock Configure Register */
#define	ICDC_CKCFG_CKRDY	(1 << 1) 
#define	ICDC_CKCFG_SELAD	(1 << 0)

/* ICDC internal register access control Register */
#define ICDC_RGADW_RGWR         (1 << 16)
#define ICDC_RGADW_RGADDR_BIT   8
#define	ICDC_RGADW_RGADDR_MASK	(0x7f << ICDC_RGADW_RGADDR_BIT)
#define ICDC_RGADW_RGDIN_BIT    0
#define	ICDC_RGADW_RGDIN_MASK	(0xff << ICDC_RGADW_RGDIN_BIT)

/* ICDC internal register data output Register */
#define ICDC_RGDATA_IRQ         (1 << 8)
#define ICDC_RGDATA_RGDOUT_BIT  0
#define ICDC_RGDATA_RGDOUT_MASK (0xff << ICDC_RGDATA_RGDOUT_BIT)

/*************************************************************************
 * SPDIF INTERFACE in AIC Controller
 *************************************************************************/

#define SPDIF_ENA               (AIC_BASE + 0x080) 
#define SPDIF_CTRL              (AIC_BASE + 0x084)
#define SPDIF_STATE             (AIC_BASE + 0x088)
#define SPDIF_CFG1              (AIC_BASE + 0x08c)
#define SPDIF_CFG2              (AIC_BASE + 0x090)
#define SPDIF_FIFO              (AIC_BASE + 0x094)

#define REG_SPDIF_ENA           REG32(SPDIF_ENA) 
#define REG_SPDIF_CTRL          REG32(SPDIF_CTRL)
#define REG_SPDIF_STATE         REG32(SPDIF_STATE)
#define REG_SPDIF_CFG1          REG32(SPDIF_CFG1)
#define REG_SPDIF_CFG2          REG32(SPDIF_CFG2)
#define REG_SPDIF_FIFO          REG32(SPDIF_FIFO)

/* SPDIF Enable Register (SPDIF_ENA) */

#define SPDIF_ENA_SPEN		(1 << 0)  /* Enable or disable the SPDIF transmitter */

/* SPDIF Control Register (SPDIF_CTRL) */

#define SPDIF_CTRL_DMAEN        (1 << 15)
#define SPDIF_CTRL_DTYPE        (1 << 14)
#define SPDIF_CTRL_SIGN         (1 << 13)
#define SPDIF_CTRL_INVALID      (1 << 12)
#define SPDIF_CTRL_RST          (1 << 11)
#define SPDIF_CTRL_SPDIFI2S     (1 << 10)
#define SPDIF_CTRL_MTRIG        (1 << 1)
#define SPDIF_CTRL_MFFUR        (1 << 0)

/* SPDIF Configure 1 Register (SPDIF_CFG1) */

#define SPDIF_CFG1_INITLVL      (1 << 17)
#define SPDIF_CFG1_ZROVLD       (1 << 16)

#define SPDIF_CFG1_TRIG_BIT     12
#define SPDIF_CFG1_TRIG_MASK    (0x3 << SPDIF_CFG1_TRIG_BIT)
  #define SPDIF_CFG1_TRIG_4     (0x0 << SPDIF_CFG1_TRIG_BIT)
  #define SPDIF_CFG1_TRIG_8     (0x1 << SPDIF_CFG1_TRIG_BIT)
  #define SPDIF_CFG1_TRIG_16    (0x2 << SPDIF_CFG1_TRIG_BIT)
  #define SPDIF_CFG1_TRIG_32    (0x3 << SPDIF_CFG1_TRIG_BIT)

#define SPDIF_CFG1_SRCNUM_BIT     8
#define SPDIF_CFG1_SRCNUM_MASK    (0xf << SPDIF_CFG1_SRCNUM_BIT)

#define SPDIF_CFG1_CH1NUM_BIT     4
#define SPDIF_CFG1_CH1NUM_MASK    (0xf << SPDIF_CFG1_CH1NUM_BIT)

#define SPDIF_CFG1_CH2NUM_BIT     0
#define SPDIF_CFG1_CH2NUM_MASK    (0xf << SPDIF_CFG1_CH2NUM_BIT)

/* SPDIF Configure 2 Register (SPDIF_CFG2) */

#define SPDIF_CFG2_FS_BIT       26
#define SPDIF_CFG2_FS_MASK      (0xf << SPDIF_CFG2_FS_BIT)
  #define SPDIF_CFG2_FS_44K     (0x0 << SPDIF_CFG2_FS_BIT) /* 44.1kHz */
  #define SPDIF_CFG2_FS_48K     (0x2 << SPDIF_CFG2_FS_BIT)
  #define SPDIF_CFG2_FS_32K     (0x3 << SPDIF_CFG2_FS_BIT)
  #define SPDIF_CFG2_FS_96K     (0xa << SPDIF_CFG2_FS_BIT)
  #define SPDIF_CFG2_FS_192K    (0xe << SPDIF_CFG2_FS_BIT)

#define SPDIF_CFG2_ORGFRQ_BIT   22
#define SPDIF_CFG2_ORGFRQ_MASK  (0xf << SPDIF_CFG2_ORGFRQ_BIT)
  #define SPDIF_CFG2_ORGFRQ_44K     (0xf << SPDIF_CFG2_ORGFRQ_BIT) /* 44.1kHz */
  #define SPDIF_CFG2_ORGFRQ_48K     (0xd << SPDIF_CFG2_ORGFRQ_BIT)
  #define SPDIF_CFG2_ORGFRQ_32K     (0xc << SPDIF_CFG2_ORGFRQ_BIT)
  #define SPDIF_CFG2_ORGFRQ_96K     (0x5 << SPDIF_CFG2_ORGFRQ_BIT)
  #define SPDIF_CFG2_ORGFRQ_192K    (0x1 << SPDIF_CFG2_ORGFRQ_BIT)

#define SPDIF_CFG2_SAMWL_BIT    19
#define SPDIF_CFG2_SAMWL_MASK  (0x7 << SPDIF_CFG2_SAMWL_BIT)

#define SPDIF_CFG2_MAXWL        (1 << 18)

#define SPDIF_CFG2_CLKACU_BIT   16
#define SPDIF_CFG2_CLKACU_MASK  (0x3 << SPDIF_CFG2_CLKACU_BIT)
  #define SPDIF_CFG2_CLKACU_LVL2     (0x0 << SPDIF_CFG2_CLKACU_BIT)
  #define SPDIF_CFG2_CLKACU_LVL1     (0x1 << SPDIF_CFG2_CLKACU_BIT)
  #define SPDIF_CFG2_CLKACU_LVL3     (0x2 << SPDIF_CFG2_CLKACU_BIT)
  #define SPDIF_CFG2_CLKACU_NOTMAT   (0x3 << SPDIF_CFG2_CLKACU_BIT)

#define SPDIF_CFG2_CATCODE_BIT   8
#define SPDIF_CFG2_CATCODE_MASK  (0xff << SPDIF_CFG2_CATCODE_BIT)

#define SPDIF_CFG2_CHMD_BIT   6
#define SPDIF_CFG2_CHMD_MASK  (0x3 << SPDIF_CFG2_CHMD_BIT)
  #define SPDIF_CFG2_CHMD_MOD0     (0x0 << SPDIF_CFG2_CHMD_BIT)

#define SPDIF_CFG2_PRE        (1 << 3)
#define SPDIF_CFG2_COPYN      (1 << 2)
#define SPDIF_CFG2_AUDION     (1 << 1)
#define SPDIF_CFG2_CONPRO     (1 << 0)


#ifndef __MIPS_ASSEMBLER

/***************************************************************************
 * AIC (AC'97 & I2S Controller)
 ***************************************************************************/

#define __aic_enable()		( REG_AIC_FR |= AIC_FR_ENB )
#define __aic_disable()		( REG_AIC_FR &= ~AIC_FR_ENB )

#define __aic_select_ac97()	( REG_AIC_FR &= ~AIC_FR_AUSEL )
#define __aic_select_i2s()	( REG_AIC_FR |= AIC_FR_AUSEL )

#define __aic_play_zero()	( REG_AIC_FR &= ~AIC_FR_LSMP )
#define __aic_play_lastsample()	( REG_AIC_FR |= AIC_FR_LSMP )

#define __i2s_as_master()	( REG_AIC_FR |= AIC_FR_BCKD | AIC_FR_SYNCD )
#define __i2s_as_slave()	( REG_AIC_FR &= ~(AIC_FR_BCKD | AIC_FR_SYNCD) )
#define __aic_reset_status()          ( REG_AIC_FR & AIC_FR_RST )

#define __aic_reset()                                   \
do {                                                    \
        REG_AIC_FR |= AIC_FR_RST;                       \
} while(0)


#define __aic_set_transmit_trigger(n) 			\
do {							\
	REG_AIC_FR &= ~AIC_FR_TFTH_MASK;		\
	REG_AIC_FR |= ((n) << AIC_FR_TFTH_BIT);		\
} while(0)

#define __aic_set_receive_trigger(n) 			\
do {							\
	REG_AIC_FR &= ~AIC_FR_RFTH_MASK;		\
	REG_AIC_FR |= ((n) << AIC_FR_RFTH_BIT);		\
} while(0)

#define __aic_enable_oldstyle() ( REG_AIC_CR |= AIC_CR_EN2OLD )
#define __aic_enable_newstyle() ( REG_AIC_CR &= ~AIC_CR_EN2OLD )
#define __aic_enable_pack16()   ( REG_AIC_CR |= AIC_CR_PACK16 )
#define __aic_enable_unpack16() ( REG_AIC_CR &= ~AIC_CR_PACK16) 

/* n = AIC_CR_CHANNEL_MONO,AIC_CR_CHANNEL_STEREO ... */
#define __aic_out_channel_select(n)                    \
do {                                                   \
	REG_AIC_CR &= ~AIC_CR_CHANNEL_MASK;            \
        REG_AIC_CR |= ((n) << AIC_CR_CHANNEL_BIT );			       \
} while(0)

#define __aic_enable_record()	( REG_AIC_CR |= AIC_CR_EREC )
#define __aic_disable_record()	( REG_AIC_CR &= ~AIC_CR_EREC )
#define __aic_enable_replay()	( REG_AIC_CR |= AIC_CR_ERPL )
#define __aic_disable_replay()	( REG_AIC_CR &= ~AIC_CR_ERPL )
#define __aic_enable_loopback()	( REG_AIC_CR |= AIC_CR_ENLBF )
#define __aic_disable_loopback() ( REG_AIC_CR &= ~AIC_CR_ENLBF )

#define __aic_flush_tfifo()	( REG_AIC_CR |= AIC_CR_TFLUSH )
#define __aic_unflush_tfifo()	( REG_AIC_CR &= ~AIC_CR_TFLUSH )
#define __aic_flush_rfifo()	( REG_AIC_CR |= AIC_CR_RFLUSH )
#define __aic_unflush_rfifo()	( REG_AIC_CR &= ~AIC_CR_RFLUSH )

#define __aic_enable_transmit_intr() \
  ( REG_AIC_CR |= (AIC_CR_ETFS | AIC_CR_ETUR) )
#define __aic_disable_transmit_intr() \
  ( REG_AIC_CR &= ~(AIC_CR_ETFS | AIC_CR_ETUR) )
#define __aic_enable_receive_intr() \
  ( REG_AIC_CR |= (AIC_CR_ERFS | AIC_CR_EROR) )
#define __aic_disable_receive_intr() \
  ( REG_AIC_CR &= ~(AIC_CR_ERFS | AIC_CR_EROR) )

#define __aic_enable_transmit_dma()  ( REG_AIC_CR |= AIC_CR_TDMS )
#define __aic_disable_transmit_dma() ( REG_AIC_CR &= ~AIC_CR_TDMS )
#define __aic_enable_receive_dma()   ( REG_AIC_CR |= AIC_CR_RDMS )
#define __aic_disable_receive_dma()  ( REG_AIC_CR &= ~AIC_CR_RDMS )

#define __aic_enable_mono2stereo()   ( REG_AIC_CR |= AIC_CR_M2S )
#define __aic_disable_mono2stereo()  ( REG_AIC_CR &= ~AIC_CR_M2S )
#define __aic_enable_byteswap()      ( REG_AIC_CR |= AIC_CR_ENDSW )
#define __aic_disable_byteswap()     ( REG_AIC_CR &= ~AIC_CR_ENDSW )
#define __aic_enable_unsignadj()     ( REG_AIC_CR |= AIC_CR_AVSTSU )
#define __aic_disable_unsignadj()    ( REG_AIC_CR &= ~AIC_CR_AVSTSU )

#define AC97_PCM_XS_L_FRONT   	AIC_ACCR1_XS_SLOT3
#define AC97_PCM_XS_R_FRONT   	AIC_ACCR1_XS_SLOT4
#define AC97_PCM_XS_CENTER    	AIC_ACCR1_XS_SLOT6
#define AC97_PCM_XS_L_SURR    	AIC_ACCR1_XS_SLOT7
#define AC97_PCM_XS_R_SURR    	AIC_ACCR1_XS_SLOT8
#define AC97_PCM_XS_LFE       	AIC_ACCR1_XS_SLOT9

#define AC97_PCM_RS_L_FRONT   	AIC_ACCR1_RS_SLOT3
#define AC97_PCM_RS_R_FRONT   	AIC_ACCR1_RS_SLOT4
#define AC97_PCM_RS_CENTER    	AIC_ACCR1_RS_SLOT6
#define AC97_PCM_RS_L_SURR    	AIC_ACCR1_RS_SLOT7
#define AC97_PCM_RS_R_SURR    	AIC_ACCR1_RS_SLOT8
#define AC97_PCM_RS_LFE       	AIC_ACCR1_RS_SLOT9

#define __ac97_set_xs_none()	( REG_AIC_ACCR1 &= ~AIC_ACCR1_XS_MASK )
#define __ac97_set_xs_mono() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_XS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_XS_R_FRONT;				\
} while(0)
#define __ac97_set_xs_stereo() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_XS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_XS_L_FRONT | AC97_PCM_XS_R_FRONT;	\
} while(0)

/* In fact, only stereo is support now. */ 
#define __ac97_set_rs_none()	( REG_AIC_ACCR1 &= ~AIC_ACCR1_RS_MASK )
#define __ac97_set_rs_mono() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_RS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_RS_R_FRONT;				\
} while(0)
#define __ac97_set_rs_stereo() 						\
do {									\
	REG_AIC_ACCR1 &= ~AIC_ACCR1_RS_MASK;				\
	REG_AIC_ACCR1 |= AC97_PCM_RS_L_FRONT | AC97_PCM_RS_R_FRONT;	\
} while(0)

#define __ac97_warm_reset_codec()		\
 do {						\
	REG_AIC_ACCR2 |= AIC_ACCR2_SA;		\
	REG_AIC_ACCR2 |= AIC_ACCR2_SS;		\
	udelay(2);				\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SS;		\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SA;		\
 } while (0)

#define __ac97_cold_reset_codec()		\
 do {						\
	REG_AIC_ACCR2 |=  AIC_ACCR2_SR;		\
	udelay(2);				\
	REG_AIC_ACCR2 &= ~AIC_ACCR2_SR;		\
 } while (0)

/* n=8,16,18,20 */
#define __ac97_set_iass(n) \
 ( REG_AIC_ACCR2 = (REG_AIC_ACCR2 & ~AIC_ACCR2_IASS_MASK) | AIC_ACCR2_IASS_##n##BIT )
#define __ac97_set_oass(n) \
 ( REG_AIC_ACCR2 = (REG_AIC_ACCR2 & ~AIC_ACCR2_OASS_MASK) | AIC_ACCR2_OASS_##n##BIT )

/* This bit should only be set in 2 channels configuration */
#define __i2s_send_rfirst()   ( REG_AIC_I2SCR |= AIC_I2SCR_RFIRST )  /* RL */
#define __i2s_send_lfirst()   ( REG_AIC_I2SCR &= ~AIC_I2SCR_RFIRST ) /* LR */

/* This bit should only be set in 2 channels configuration and 16bit-packed mode */
#define __i2s_switch_lr()     ( REG_AIC_I2SCR |= AIC_I2SCR_SWLH )
#define __i2s_unswitch_lr()   ( REG_AIC_I2SCR &= ~AIC_I2SCR_SWLH )

#define __i2s_select_i2s()            ( REG_AIC_I2SCR &= ~AIC_I2SCR_AMSL )
#define __i2s_select_msbjustified()   ( REG_AIC_I2SCR |= AIC_I2SCR_AMSL )

/* n=8,16,18,20,24 */
/*#define __i2s_set_sample_size(n) \
 ( REG_AIC_I2SCR |= (REG_AIC_I2SCR & ~AIC_I2SCR_WL_MASK) | AIC_I2SCR_WL_##n##BIT )*/

#define __i2s_out_channel_select(n) __aic_out_channel_select(n)

#define __i2s_set_oss_sample_size(n) \
 ( REG_AIC_CR = (REG_AIC_CR & ~AIC_CR_OSS_MASK) | AIC_CR_OSS_##n##BIT )
#define __i2s_set_iss_sample_size(n) \
 ( REG_AIC_CR = (REG_AIC_CR & ~AIC_CR_ISS_MASK) | AIC_CR_ISS_##n##BIT )

#define __i2s_stop_bitclk()   ( REG_AIC_I2SCR |= AIC_I2SCR_STPBK )
#define __i2s_start_bitclk()  ( REG_AIC_I2SCR &= ~AIC_I2SCR_STPBK )

#define __aic_transmit_request()  ( REG_AIC_SR & AIC_SR_TFS )
#define __aic_receive_request()   ( REG_AIC_SR & AIC_SR_RFS )
#define __aic_transmit_underrun() ( REG_AIC_SR & AIC_SR_TUR )
#define __aic_receive_overrun()   ( REG_AIC_SR & AIC_SR_ROR )

#define __aic_clear_errors()      ( REG_AIC_SR &= ~(AIC_SR_TUR | AIC_SR_ROR) )

#define __aic_get_transmit_resident() \
  ( (REG_AIC_SR & AIC_SR_TFL_MASK) >> AIC_SR_TFL_BIT )
#define __aic_get_receive_count() \
  ( (REG_AIC_SR & AIC_SR_RFL_MASK) >> AIC_SR_RFL_BIT )

#define __ac97_command_transmitted()     ( REG_AIC_ACSR & AIC_ACSR_CADT )
#define __ac97_status_received()         ( REG_AIC_ACSR & AIC_ACSR_SADR )
#define __ac97_status_receive_timeout()  ( REG_AIC_ACSR & AIC_ACSR_RSTO )
#define __ac97_codec_is_low_power_mode() ( REG_AIC_ACSR & AIC_ACSR_CLPM )
#define __ac97_codec_is_ready()          ( REG_AIC_ACSR & AIC_ACSR_CRDY )
#define __ac97_slot_error_detected()     ( REG_AIC_ACSR & AIC_ACSR_SLTERR )
#define __ac97_clear_slot_error()        ( REG_AIC_ACSR &= ~AIC_ACSR_SLTERR )

#define __i2s_is_busy()         ( REG_AIC_I2SSR & AIC_I2SSR_BSY )

#define CODEC_READ_CMD	        (1 << 19)
#define CODEC_WRITE_CMD	        (0 << 19)
#define CODEC_REG_INDEX_BIT     12
#define CODEC_REG_INDEX_MASK	(0x7f << CODEC_REG_INDEX_BIT)	/* 18:12 */
#define CODEC_REG_DATA_BIT      4
#define CODEC_REG_DATA_MASK	(0x0ffff << 4)	/* 19:4 */

#define __ac97_out_rcmd_addr(reg) 					\
do { 									\
    REG_AIC_ACCAR = CODEC_READ_CMD | ((reg) << CODEC_REG_INDEX_BIT); 	\
} while (0)

#define __ac97_out_wcmd_addr(reg) 					\
do { 									\
    REG_AIC_ACCAR = CODEC_WRITE_CMD | ((reg) << CODEC_REG_INDEX_BIT); 	\
} while (0)

#define __ac97_out_data(value) 						\
do { 									\
    REG_AIC_ACCDR = ((value) << CODEC_REG_DATA_BIT); 			\
} while (0)

#define __ac97_in_data() \
 ( (REG_AIC_ACSDR & CODEC_REG_DATA_MASK) >> CODEC_REG_DATA_BIT )

#define __ac97_in_status_addr() \
 ( (REG_AIC_ACSAR & CODEC_REG_INDEX_MASK) >> CODEC_REG_INDEX_BIT )

#define __i2s_set_sample_rate(i2sclk, sync) \
  ( REG_AIC_I2SDIV = ((i2sclk) / (4*64)) / (sync) )

#define __aic_write_tfifo(v)  ( REG_AIC_DR = (v) )
#define __aic_read_rfifo()    ( REG_AIC_DR )

#define __aic_internal_codec()  ( REG_AIC_FR |= AIC_FR_ICDC ) 
#define __aic_external_codec()  ( REG_AIC_FR &= ~AIC_FR_ICDC )

//
// Define next ops for AC97 compatible
//

#define AC97_ACSR	AIC_ACSR

#define __ac97_enable()		__aic_enable(); __aic_select_ac97()
#define __ac97_disable()	__aic_disable()
#define __ac97_reset()		__aic_reset()

#define __ac97_set_transmit_trigger(n)	__aic_set_transmit_trigger(n)
#define __ac97_set_receive_trigger(n)	__aic_set_receive_trigger(n)

#define __ac97_enable_record()		__aic_enable_record()
#define __ac97_disable_record()		__aic_disable_record()
#define __ac97_enable_replay()		__aic_enable_replay()
#define __ac97_disable_replay()		__aic_disable_replay()
#define __ac97_enable_loopback()	__aic_enable_loopback()
#define __ac97_disable_loopback()	__aic_disable_loopback()

#define __ac97_enable_transmit_dma()	__aic_enable_transmit_dma()
#define __ac97_disable_transmit_dma()	__aic_disable_transmit_dma()
#define __ac97_enable_receive_dma()	__aic_enable_receive_dma()
#define __ac97_disable_receive_dma()	__aic_disable_receive_dma()

#define __ac97_transmit_request()	__aic_transmit_request()
#define __ac97_receive_request()	__aic_receive_request()
#define __ac97_transmit_underrun()	__aic_transmit_underrun()
#define __ac97_receive_overrun()	__aic_receive_overrun()

#define __ac97_clear_errors()		__aic_clear_errors()

#define __ac97_get_transmit_resident()	__aic_get_transmit_resident()
#define __ac97_get_receive_count()	__aic_get_receive_count()

#define __ac97_enable_transmit_intr()	__aic_enable_transmit_intr()
#define __ac97_disable_transmit_intr()	__aic_disable_transmit_intr()
#define __ac97_enable_receive_intr()	__aic_enable_receive_intr()
#define __ac97_disable_receive_intr()	__aic_disable_receive_intr()

#define __ac97_write_tfifo(v)		__aic_write_tfifo(v)
#define __ac97_read_rfifo()		__aic_read_rfifo()

//
// Define next ops for I2S compatible
//

#define I2S_ACSR	AIC_I2SSR

#define __i2s_enable()		 __aic_enable(); __aic_select_i2s()
#define __i2s_disable()		__aic_disable()
#define __i2s_reset()		__aic_reset()

#define __i2s_set_transmit_trigger(n)	__aic_set_transmit_trigger(n)
#define __i2s_set_receive_trigger(n)	__aic_set_receive_trigger(n)

#define __i2s_enable_record()		__aic_enable_record()
#define __i2s_disable_record()		__aic_disable_record()
#define __i2s_enable_replay()		__aic_enable_replay()
#define __i2s_disable_replay()		__aic_disable_replay()
#define __i2s_enable_loopback()		__aic_enable_loopback()
#define __i2s_disable_loopback()	__aic_disable_loopback()

#define __i2s_enable_transmit_dma()	__aic_enable_transmit_dma()
#define __i2s_disable_transmit_dma()	__aic_disable_transmit_dma()
#define __i2s_enable_receive_dma()	__aic_enable_receive_dma()
#define __i2s_disable_receive_dma()	__aic_disable_receive_dma()

#define __i2s_transmit_request()	__aic_transmit_request()
#define __i2s_receive_request()		__aic_receive_request()
#define __i2s_transmit_underrun()	__aic_transmit_underrun()
#define __i2s_receive_overrun()		__aic_receive_overrun()

#define __i2s_clear_errors()		__aic_clear_errors()

#define __i2s_get_transmit_resident()	__aic_get_transmit_resident()
#define __i2s_get_receive_count()	__aic_get_receive_count()

#define __i2s_enable_transmit_intr()	__aic_enable_transmit_intr()
#define __i2s_disable_transmit_intr()	__aic_disable_transmit_intr()
#define __i2s_enable_receive_intr()	__aic_enable_receive_intr()
#define __i2s_disable_receive_intr()	__aic_disable_receive_intr()

#define __i2s_write_tfifo(v)		__aic_write_tfifo(v)
#define __i2s_read_rfifo()		__aic_read_rfifo()

#define __i2s_reset_codec()			\
 do {						\
 } while (0)


/*************************************************************************
 * SPDIF INTERFACE in AIC Controller
 *************************************************************************/

#define __spdif_enable()        ( REG_SPDIF_ENA |= SPDIF_ENA_SPEN )
#define __spdif_disable()       ( REG_SPDIF_ENA &= ~SPDIF_ENA_SPEN )

#define __spdif_enable_transmit_dma()     ( REG_SPDIF_CTRL |= SPDIF_CTRL_DMAEN )
#define __spdif_disable_transmit_dma()    ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_DMAEN )
#define __spdif_enable_dtype()            ( REG_SPDIF_CTRL |= SPDIF_CTRL_DTYPE )
#define __spdif_disable_dtype()           ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_DTYPE )
#define __spdif_enable_sign()             ( REG_SPDIF_CTRL |= SPDIF_CTRL_SIGN )
#define __spdif_disable_sign()            ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_SIGN )
#define __spdif_enable_invalid()          ( REG_SPDIF_CTRL |= SPDIF_CTRL_INVALID )
#define __spdif_disable_invalid()         ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_INVALID )
#define __spdif_enable_reset()            ( REG_SPDIF_CTRL |= SPDIF_CTRL_RST )
#define __spdif_select_spdif()            ( REG_SPDIF_CTRL |= SPDIF_CTRL_SPDIFI2S )
#define __spdif_select_i2s()              ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_SPDIFI2S )
#define __spdif_enable_MTRIGmask()        ( REG_SPDIF_CTRL |= SPDIF_CTRL_MTRIG )
#define __spdif_disable_MTRIGmask()       ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_MTRIG )
#define __spdif_enable_MFFURmask()        ( REG_SPDIF_CTRL |= SPDIF_CTRL_MFFUR )
#define __spdif_disable_MFFURmask()       ( REG_SPDIF_CTRL &= ~SPDIF_CTRL_MFFUR )

#define __spdif_enable_initlvl_high()     ( REG_SPDIF_CFG1 |=  SPDIF_CFG1_INITLVL )
#define __spdif_enable_initlvl_low()      ( REG_SPDIF_CFG1 &=  ~SPDIF_CFG1_INITLVL )
#define __spdif_enable_zrovld_invald()    ( REG_SPDIF_CFG1 |=  SPDIF_CFG1_ZROVLD )
#define __spdif_enable_zrovld_vald()      ( REG_SPDIF_CFG1 &=  ~SPDIF_CFG1_ZROVLD )

/* 0, 1, 2, 3 */
#define __spdif_set_transmit_trigger(n)			\
do {							\
	REG_SPDIF_CFG1 &= ~SPDIF_CFG1_TRIG_MASK;	\
	REG_SPDIF_CFG1 |= ((n) << SPDIF_CFG1_TRIG_BIT );	\
} while(0)

/* 1 ~ 15 */
#define __spdif_set_srcnum(n)    			\
do {							\
	REG_SPDIF_CFG1 &= ~SPDIF_CFG1_SRCNUM_MASK;	\
	REG_SPDIF_CFG1 |= ((n) << SPDIF_CFG1_SRCNUM_BIT);	\
} while(0)

/* 1 ~ 15 */
#define __spdif_set_ch1num(n)    			\
do {							\
	REG_SPDIF_CFG1 &= ~SPDIF_CFG1_CH1NUM_MASK;	\
	REG_SPDIF_CFG1 |= ((n) << SPDIF_CFG1_CH1NUM_BIT);	\
} while(0)

/* 1 ~ 15 */
#define __spdif_set_ch2num(n)    			\
do {							\
	REG_SPDIF_CFG1 &= ~SPDIF_CFG1_CH2NUM_MASK;	\
	REG_SPDIF_CFG1 |= ((n) << SPDIF_CFG1_CH2NUM_BIT);	\
} while(0)

/* 0x0, 0x2, 0x3, 0xa, 0xe */
#define __spdif_set_fs(n)				\
do {							\
	REG_SPDIF_CFG2 &= ~SPDIF_CFG2_FS_MASK;   	\
	REG_SPDIF_CFG2 |= ((n) << SPDIF_CFG2_FS_BIT);	\
} while(0)

/* 0xd, 0xc, 0x5, 0x1 */
#define __spdif_set_orgfrq(n)				\
do {							\
	REG_SPDIF_CFG2 &= ~SPDIF_CFG2_ORGFRQ_MASK;   	\
	REG_SPDIF_CFG2 |= ((n) << SPDIF_CFG2_ORGFRQ_BIT);	\
} while(0)

/* 0x1, 0x6, 0x2, 0x4, 0x5 */
#define __spdif_set_samwl(n)				\
do {							\
	REG_SPDIF_CFG2 &= ~SPDIF_CFG2_SAMWL_MASK;   	\
	REG_SPDIF_CFG2 |= ((n) << SPDIF_CFG2_SAMWL_BIT);	\
} while(0)

#define __spdif_enable_samwl_24()    ( REG_SPDIF_CFG2 |= SPDIF_CFG2_MAXWL )
#define __spdif_enable_samwl_20()    ( REG_SPDIF_CFG1 &= ~SPDIF_CFG2_MAXWL )

/* 0x1, 0x1, 0x2, 0x3 */
#define __spdif_set_clkacu(n)				\
do {							\
	REG_SPDIF_CFG2 &= ~SPDIF_CFG2_CLKACU_MASK;   	\
	REG_SPDIF_CFG2 |= ((n) << SPDIF_CFG2_CLKACU_BIT);	\
} while(0)

/* see IEC60958-3 */
#define __spdif_set_catcode(n)				\
do {							\
	REG_SPDIF_CFG2 &= ~SPDIF_CFG2_CATCODE_MASK;   	\
	REG_SPDIF_CFG2 |= ((n) << SPDIF_CFG2_CATCODE_BIT);	\
} while(0)

/* n = 0x0, */
#define __spdif_set_chmode(n)				\
do {							\
	REG_SPDIF_CFG2 &= ~SPDIF_CFG2_CHMD_MASK;   	\
	REG_SPDIF_CFG2 |= ((n) << SPDIF_CFG2_CHMD_BIT);	\
} while(0)

#define __spdif_enable_pre()       ( REG_SPDIF_CFG2 |= SPDIF_CFG2_PRE )
#define __spdif_disable_pre()      ( REG_SPDIF_CFG2 &= ~SPDIF_CFG2_PRE )
#define __spdif_enable_copyn()     ( REG_SPDIF_CFG2 |= SPDIF_CFG2_COPYN )
#define __spdif_disable_copyn()    ( REG_SPDIF_CFG2 &= ~SPDIF_CFG2_COPYN )
/* audio sample word represents linear PCM samples */
#define __spdif_enable_audion()    ( REG_SPDIF_CFG2 &= ~SPDIF_CFG2_AUDION )
/* udio sample word used for other purpose */
#define __spdif_disable_audion()   ( REG_SPDIF_CFG2 |= SPDIF_CFG2_AUDION )
#define __spdif_enable_conpro()    ( REG_SPDIF_CFG2 &= ~SPDIF_CFG2_CONPRO )
#define __spdif_disable_conpro()   ( REG_SPDIF_CFG2 |= SPDIF_CFG2_CONPRO )

/***************************************************************************
 * ICDC
 ***************************************************************************/
#define __i2s_internal_codec()         __aic_internal_codec()
#define __i2s_external_codec()         __aic_external_codec()

#define __icdc_clk_ready()             ( REG_ICDC_CKCFG & ICDC_CKCFG_CKRDY )
#define __icdc_sel_adc()               ( REG_ICDC_CKCFG |= ICDC_CKCFG_SELAD )
#define __icdc_sel_dac()               ( REG_ICDC_CKCFG &= ~ICDC_CKCFG_SELAD )

#define __icdc_set_rgwr()              ( REG_ICDC_RGADW |= ICDC_RGADW_RGWR )
#define __icdc_clear_rgwr()            ( REG_ICDC_RGADW &= ~ICDC_RGADW_RGWR )
#define __icdc_rgwr_ready()            ( REG_ICDC_RGADW & ICDC_RGADW_RGWR )

#define	AIC_RW_CODEC_START()		while (INREG32(ICDC_RGADW) & ICDC_RGADW_RGWR)
#define	AIC_RW_CODEC_STOP()		while (INREG32(ICDC_RGADW) & ICDC_RGADW_RGWR)

#define __icdc_set_addr(n)				\
do {          						\
	REG_ICDC_RGADW &= ~ICDC_RGADW_RGADDR_MASK;	\
	REG_ICDC_RGADW |= (n) << ICDC_RGADW_RGADDR_BIT;	\
} while(0)

#define __icdc_set_cmd(n)				\
do {          						\
	REG_ICDC_RGADW &= ~ICDC_RGADW_RGDIN_MASK;	\
	REG_ICDC_RGADW |= (n) << ICDC_RGADW_RGDIN_BIT;	\
} while(0)

#define __icdc_irq_pending()            ( REG_ICDC_RGDATA & ICDC_RGDATA_IRQ )
#define __icdc_get_value()              ( REG_ICDC_RGDATA & ICDC_RGDATA_RGDOUT_MASK )



#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4760AIC_H__ */
