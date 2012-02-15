/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : sd_mmc_reg.h
 * 	Description : S1C33L27 SD_MMC register definition file
 *
 * 	Revision History:
 *  2010/01/26   Hellen Yao   New creation
 ****************************************************************************/

#ifndef _SD_MMC_REG_H
#define _SD_MMC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ SD_MMC Clock Control ] MMC_CLKCTL              0x00030_2600    */
/********************************************************************/
    union MMC_CLKCTL_tag {
        volatile struct {
            unsigned int CLKDIV:8;      // Clock divide ratio
            unsigned int DUMMY1:8;      // ~
            unsigned int CLKRUN:1;      // Clock divide run/stop control
            unsigned int DUMMY2:7;      // ~
            unsigned int CLKSTF:1;      // Clock change status flag
            unsigned int DUMMY3:7;      // ~
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ SD_MMC Function Control ] MMC_FUNCTL           0x00030_2604    */
/********************************************************************/
    union MMC_FUNCTL_tag {
        volatile struct {
            unsigned int BUSSEL:1;      // Wide bus mode enable (0:1-bit; 1:4-bit)
            unsigned int REPMD:1;       // Response mode (0:48-bit; 1:136-bit)
            unsigned int DATMD:1;       // Data transfer mode (0:Single-block; 1:Multi-block)
            unsigned int DATBYT:1;      // Data byte order
            unsigned int DUMMY1:4;      // ~

            unsigned int COMSED:1;      // Command send enable
            unsigned int RSPEN:1;       // Response receive enable
            unsigned int DATEN:1;       // Data transmit enable
            unsigned int DAREN:1;       // Data receive enable
            unsigned int BUSYEN:1;      // Wait bus enable
            unsigned int CLK8EN:1;      // Send 8 clock enable
            unsigned int DUMMY2:2;      // ~

            unsigned int SYNRST:1;      // Synch. reset enable
            unsigned int DUMMY3:7;      // ~

            unsigned int DUMMY4:8;      // ~
        } bCTL;
        volatile struct {
            unsigned int MODE:8;        // Mode Set
            unsigned int FUNC:8;        // Function Control
            unsigned int DUMMY5:8;      // ~
            unsigned int DUMMY6:8;      // ~
        } ucCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Response Reg ] MMC_RSP_R1                      0x00030_2618    */
/********************************************************************/

    /* Normal response command */
    union MMC_RSP_R1_tag {
        volatile struct {
            unsigned int ED:1;          // End bit (1)
            unsigned int CRC7:7;        // CRC7

            /* card status -------------------------------------- */
            unsigned int DUMMY1:3;      // ~
            unsigned int AKE_ERR:1;     // error in sequence of the authentication process
            unsigned int DUMMY2:1;      // ~
            unsigned int APP_CMD:1;     // the card will expect ACMD
            unsigned int DUMMY3:2;      // ~

            unsigned int READY:1;       // corresponds to buffer empty signaling on the bus
            unsigned int STATE:4;       // current state
            unsigned int ERASE:1;       // erase reset
            unsigned int ECC:1;         // card ecc disable
            unsigned int WP_ERASE:1;    // write protect erase

            unsigned int CSD_OW:1;      // CSD over write
            unsigned int DUMMY4:2;      // ~
            unsigned int ERR:1;         // error
            unsigned int CC_ERR:1;      // cc error
            unsigned int ECC_FAILED:1;  // ecc failed
            unsigned int ILL_CMD:1;     // illegal command
            unsigned int CRC_ERR:1;     // crc error

            unsigned int LOCK_F:1;      // lock unlock failed
            unsigned int LOCKED:1;      // card is locked
            unsigned int WP_VIO:1;      // write protect violation
            unsigned int ERASE_PARAM:1; // erase PARAM
            unsigned int SEQ_ERR:1;     // sequence of erase error
            unsigned int BLK_ERR:1;     // block error
            unsigned int ADD_ERR:1;     // address error
            unsigned int OUT_RANGE:1;   // out of range
            /* end ---------------------------------------------- */

            unsigned int CMDIDX:6;      // command index
            unsigned int TX:1;          // transmission bit (0)
            unsigned int ST:1;          // start bit (0)
        } bCTL;
    };

/********************************************************************/
/* [ Response Reg ] MMC_RSP_R2                      0x00030_2618    */
/********************************************************************/

    /* CID, CSD register */
    union MMC_RSP_R2_tag {
        volatile struct {
            unsigned int CID_CSD0:32;   // CID/CSD reg [31:0]
            unsigned int CID_CSD1:32;   // CID/CSD reg [63:32]
            unsigned int CID_CSD2:32;   // CID/CSD reg [95:64]
            unsigned int CID_CSD3:32;   // CID/CSD reg [127:96]
            unsigned int DUMMY:6;       // ~
            unsigned int TX:1;          // transmission bit (0)
            unsigned int ST:1;          // start bit (0)
        } bCTL;
    };

/********************************************************************/
/* [ Response Reg ] MMC_RSP_R3                      0x00030_2618    */
/********************************************************************/

    /* OCR register */
    union MMC_RSP_R3_tag {
        volatile struct {
            unsigned int ED:1;          // end bit (1)
            unsigned int DUMMY1:7;      // ~

            /* OCR register --------------------------------------- */
            unsigned int OCR_L:8;       // OCR reg
            unsigned int OCR_M:16;      // OCR reg
            unsigned int OCR_H:8;       // OCR reg
            /* end ------------------------------------------------ */

            unsigned int DUMMY2:6;      // command index
            unsigned int TX:1;          // transmission bit (0)
            unsigned int ST:1;          // start bit (0)
        } bCTL;
    };

/********************************************************************/
/* [ Response Reg ] MMC_RSP_R6                      0x00030_2618    */
/********************************************************************/

    /* Published RCA response */
    union MMC_RSP_R6_tag {
        volatile struct {
            unsigned int ED:1;          // end bit (1)
            unsigned int CRC7:7;        // CRC7

            /* card status -------------------------------------- */
            unsigned int DUMMY1:3;      // card status bit2~0
            unsigned int AKE_ERR:1;     // card status bit3: error in the sequence of the authentication process
            unsigned int DUMMY2:1;      // card status bit4
            unsigned int APP_CMD:1;     // card status bit5: interpreted as ACMD
            unsigned int DUMMY3:2;      // card status bit7~6

            unsigned int READY:1;       // card status bit8
            unsigned int CUR_STATE:4;   // card status bit12~9: the state of the card
            unsigned int ERR:1;         // card status bit19: a general or an unknown error
            unsigned int ILL_CMD:1;     // card status bit22: command not legal for the card status
            unsigned int CRC_ERR:1;     // card status bit23: the crc check of the previous command failed
            /* end ------------------------------------------------ */

            /* new published RCA[31:16] --------------------------- */
            unsigned int RCA16L:8;      // new published RCA[23:16]
            unsigned int RCA16H:8;      // new published RCA[31:24]
            /* end ------------------------------------------------ */

            unsigned int CMDIDX:6;      // command index
            unsigned int TX:1;          // transmission bit (0)
            unsigned int ST:1;          // start bit (0)
        } bCTL;
    };

/********************************************************************/
/* [ Response Reg ] MMC_RSP_R7                      0x00030_2618    */
/********************************************************************/

    /* Card I/F condition */
    union MMC_RSP_R7_tag {
        volatile struct {
            unsigned int ED:1;          // end bit (1)
            unsigned int CRC7:7;        // CRC7

            unsigned int CHKPATEN:8;    // echo-back of check pattern

            unsigned int VOLTAGE:4;     // voltage accepted
            unsigned int DUMMY1:12;     // ~

            unsigned int DUMMY2:8;      // ~

            unsigned int CMDIDX:6;      // command index
            unsigned int TX:1;          // transmission bit (0)
            unsigned int ST:1;          // start bit (0)
        } bCTL;
    };

/********************************************************************/
/* [ SD_MMC Status ] MMC_STATUS                      0x00030_262c   */
/********************************************************************/
    union MMC_STATUS_tag {
        volatile struct {
            unsigned int DATCRCFLG:1;   // Data CRC error/error token flag
            unsigned int RSPCRCFLG:1;   // Response CRC error flag
            unsigned int DATTMFLG:1;    // Data transfer timeout flag
            unsigned int RSPTMFLG:1;    // Response timeout flag
            unsigned int HFULLFLG:1;    // Receive data fifo half full flag
            unsigned int HEMPTFLG:1;    // Trnasmit data fifo half empty flag
            unsigned int CMDFLG:1;      // Command without response flag
            unsigned int RSPFLG:1;      // Command with response flag

            unsigned int BUSY_ST:1;     // Card busy status
            unsigned int CRCST:3;       // Card CRC status
            unsigned int DUMMY1:4;      // ~

            unsigned int FULLFLG:1;     // Data fifo full flag
            unsigned int EMPTFLG:1;     // Data fifo empty flag
            unsigned int DUMMY2:6;      // ~

            unsigned int DUMMY3:8;      // ~
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ SD_MMC Interrupt Control ] MMC_INT              0x00030_2630   */
/********************************************************************/
    union MMC_INT_tag {
        volatile struct {
            unsigned int DATCRCEN:1;    // Data CRC error/error token interrupt enable
            unsigned int RSPCRCEN:1;    // Response CRC error interrupt enable
            unsigned int DATTMEN:1;     // Data transfer timeout interrupt enable
            unsigned int RSPTMEN:1;     // Response timeout interrupt enable
            unsigned int RFFINTEN:1;    // Receive fifo half full interrupt enable
            unsigned int TFEINTEN:1;    // Trnasmit fifo half empty interrupt enable
            unsigned int CMDINTEN:1;    // Command without response interrupt enable
            unsigned int RSPINTEN:1;    // Command with response interrupt enable
            unsigned int DMAEN:1;       // FIFO half-empty/half-full DMA transfer enable
            unsigned int DUMMY:23;      // ~
        } bCTL;
        volatile unsigned long ulCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/

#define MMC_BASE             (0x00302600)

#define MMC_CLKCTL           (*(union MMC_CLKCTL_tag *)(MMC_BASE)).ulCTL
#define MMC_CLKDIV           (*(union MMC_CLKCTL_tag *)(MMC_BASE)).bCTL.CLKDIV
#define MMC_CLKRUN           (*(union MMC_CLKCTL_tag *)(MMC_BASE)).bCTL.CLKRUN
#define MMC_CLKSTF           (*(union MMC_CLKCTL_tag *)(MMC_BASE)).bCTL.CLKSTF

#define MMC_BUSSEL           (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.BUSSEL
#define MMC_REPMD            (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.REPMD
#define MMC_DATMD            (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.DATMD
#define MMC_DATBYT           (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.DATBYT
#define MMC_COMSED           (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.COMSED
#define MMC_RSPEN            (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.RSPEN
#define MMC_DATEN            (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.DATEN
#define MMC_DAREN            (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.DAREN
#define MMC_BUSYEN           (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.BUSYEN
#define MMC_8CLKEN           (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.CLK8EN
#define MMC_SYNRST           (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).bCTL.SYNRST

#define MMC_MODE             (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).ucCTL.MODE
#define MMC_FUNC             (*(union MMC_FUNCTL_tag *)(MMC_BASE+0x04)).ucCTL.FUNC

#define MMC_BLK              (*(volatile unsigned short*)(MMC_BASE+0x08))
#define MMC_TIMEOUT          (*(volatile unsigned long*)(MMC_BASE+0x0c))
#define MMC_CMD_IDX          (*(volatile unsigned char*)(MMC_BASE+0x10))
#define MMC_CMD_ARG          (*(volatile unsigned long*)(MMC_BASE+0x14))

#define MMC_RSP(n)           (*(volatile unsigned long*)(MMC_BASE+0x18+4*(n)))
/*------------------------------------------------------------------------------------------------------*/
#define MMC_R1_AKE_ERR       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.AKE_ERR
#define MMC_R1_APP_CMD       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.APP_CMD
#define MMC_R1_READY         (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.READY
#define MMC_R1_STATE         (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.STATE
#define MMC_R1_ERASE         (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ERASE
#define MMC_R1_ECC           (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ECC
#define MMC_R1_WP_ERASE      (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.WP_ERASE
#define MMC_R1_CSD_OW        (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.CSD_OW
#define MMC_R1_ERR           (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ERR
#define MMC_R1_CC_ERR        (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.CC_ERR
#define MMC_R1_ECC_FAILED    (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ECC_FAILED
#define MMC_R1_ILL_CMD       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ILL_CMD
#define MMC_R1_CRC_ERR       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.CRC_ERR
#define MMC_R1_LOCK_F        (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.LOCK_F
#define MMC_R1_LOCKED        (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.LOCKED
#define MMC_R1_WP_VIO        (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.WP_VIO
#define MMC_R1_ERASE_PARAM   (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ERASE_PARAM
#define MMC_R1_SEQ_ERR       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.SEQ_ERR
#define MMC_R1_BLK_ERR       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.BLK_ERR
#define MMC_R1_ADD_ERR       (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.ADD_ERR
#define MMC_R1_OUT_RANGE     (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.OUT_RANGE
#define MMC_R1_CMDIDX        (*(union MMC_RSP_R1_tag *)(MMC_BASE+0x18)).bCTL.CMDIDX
/*---------------------------------------------------------------------------------------------*/
#define MMC_R2_CID_CSD_LL    (*(union MMC_RSP_R2_tag *)(MMC_BASE+0x18)).bCTL.CID_CSD0
#define MMC_R2_CID_CSD_LH    (*(union MMC_RSP_R2_tag *)(MMC_BASE+0x18)).bCTL.CID_CSD1
#define MMC_R2_CID_CSD_HL    (*(union MMC_RSP_R2_tag *)(MMC_BASE+0x18)).bCTL.CID_CSD2
#define MMC_R2_CID_CSD_HH    (*(union MMC_RSP_R2_tag *)(MMC_BASE+0x18)).bCTL.CID_CSD3
/*---------------------------------------------------------------------------------------------*/
#define MMC_R3_OCR_L         (*(union MMC_RSP_R3_tag *)(MMC_BASE+0x18)).bCTL.OCR_L
#define MMC_R3_OCR_M         (*(union MMC_RSP_R3_tag *)(MMC_BASE+0x18)).bCTL.OCR_M
#define MMC_R3_OCR_H         (*(union MMC_RSP_R3_tag *)(MMC_BASE+0x18)).bCTL.OCR_H
/*---------------------------------------------------------------------------------------------*/
#define MMC_R6_AKE_ERR       (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.AKE_ERR
#define MMC_R6_APP_CMD       (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.APP_CMD
#define MMC_R6_READY         (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.READY
#define MMC_R6_CUR_STATE     (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.CUR_STATE
#define MMC_R6_ERR           (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.ERR
#define MMC_R6_ILL_CMD       (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.ILL_CMD
#define MMC_R6_CRC_ERR       (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.CRC_ERR
#define MMC_R6_RCA16L        (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.RCA16L
#define MMC_R6_RCA16H        (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.RCA16H
#define MMC_R6_CMDIDX        (*(union MMC_RSP_R6_tag *)(MMC_BASE+0x18)).bCTL.CMDIDX
/*---------------------------------------------------------------------------------------------*/
#define MMC_R7_CHKPATEN      (*(union MMC_RSP_R7_tag *)(MMC_BASE+0x18)).bCTL.CHKPATEN
#define MMC_R7_VOLTAGE       (*(union MMC_RSP_R7_tag *)(MMC_BASE+0x18)).bCTL.VOLTAGE
#define MMC_R7_CMDIDX        (*(union MMC_RSP_R7_tag *)(MMC_BASE+0x18)).bCTL.CMDIDX
/*---------------------------------------------------------------------------------------------*/

#define MMC_STATUS           (*(unsigned long*)(MMC_BASE+0x2c))
#define MMC_FLGREG           (*(unsigned char*)(MMC_BASE+0x2c))
#define MMC_ERRFLG           (MMC_FLGREG & 0x0f)
#define MMC_DATCRCFLG        (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.DATCRCFLG
#define MMC_RSPCRCFLG        (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.RSPCRCFLG
#define MMC_DATTMFLG         (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.DATTMFLG
#define MMC_RSPTMFLG         (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.RSPTMFLG
#define MMC_HFULLFLG         (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.HFULLFLG
#define MMC_HEMPTFLG         (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.HEMPTFLG
#define MMC_CMDFLG           (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.CMDFLG
#define MMC_RSPFLG           (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.RSPFLG
#define MMC_BUSY_ST          (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.BUSY_ST
#define MMC_CRCST            (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.CRCST
#define MMC_FULLFLG          (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.FULLFLG
#define MMC_EMPTFLG          (*(union MMC_STATUS_tag *)(MMC_BASE+0x2c)).bCTL.EMPTFLG

#define MMC_INT              (*(union MMC_INT_tag *)(MMC_BASE+0x30)).ulCTL
#define MMC_DATCRCEN         (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.DATCRCEN
#define MMC_RSPCRCEN         (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.RSPCRCEN
#define MMC_DATTMEN          (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.DATTMEN
#define MMC_RSPTMEN          (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.RSPTMEN
#define MMC_RFFINTEN         (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.RFFINTEN
#define MMC_TFEINTEN         (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.TFEINTEN
#define MMC_CMDINTEN         (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.CMDINTEN
#define MMC_RSPINTEN         (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.RSPINTEN
#define MMC_DMAEN            (*(union MMC_INT_tag *)(MMC_BASE+0x30)).bCTL.DMAEN

#define MMC_FIFODAT          (*(volatile unsigned long*)(MMC_BASE+0x50))
#define MMC_FIFODAT_ADDR     (MMC_BASE+0x50)


#ifdef __cplusplus
}
#endif
#endif                          /* _SD_MMC_REG_H */
