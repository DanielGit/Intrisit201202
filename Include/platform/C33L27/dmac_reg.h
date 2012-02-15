/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : dmac_reg.h
 * 	Description : S1C33L27 DMAC register definition file
 *
 * 	Revision History:
 *  2010/01/06   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _DMAC_REG_H
#define _DMAC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ DMAC Channel Enable Register ]   DMAC_CH_EN               0x00 */
/********************************************************************/
    union DMAC_CH_EN_tag {
        volatile struct {
            unsigned int DMAON0:1;           // DMAC Ch.0 enable
            unsigned int DMAON1:1;           // DMAC Ch.1 enable
            unsigned int DMAON2:1;           // DMAC Ch.2 enable
            unsigned int DMAON3:1;           // DMAC Ch.3 enable
            unsigned int DMAON4:1;           // DMAC Ch.4 enable
            unsigned int DMAON5:1;           // DMAC Ch.5 enable
            unsigned int DMAON6:1;           // DMAC Ch.6 enable
            unsigned int DMAON7:1;           // DMAC Ch.7 enable
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ DMAC Interrupt Enable Register ]       DMAC_IE            0x08 */
/********************************************************************/
    union DMAC_IE_tag {
        volatile struct {
            unsigned int DMAIE0:1;           // DMAC Ch.0 interrupt enable
            unsigned int DMAIE1:1;           // DMAC Ch.1 interrupt enable
            unsigned int DMAIE2:1;           // DMAC Ch.2 interrupt enable
            unsigned int DMAIE3:1;           // DMAC Ch.3 interrupt enable
            unsigned int DMAIE4:1;           // DMAC Ch.4 interrupt enable
            unsigned int DMAIE5:1;           // DMAC Ch.5 interrupt enable
            unsigned int DMAIE6:1;           // DMAC Ch.6 interrupt enable
            unsigned int DMAIE7:1;           // DMAC Ch.7 interrupt enable
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ DMAC Trigger Select Register ]     DMAC_TRG_SEL           0x0c */
/********************************************************************/
    union DMAC_TRG_SEL_tag {
        volatile struct {
            unsigned int TRG_SEL0:2;           // DMAC Ch.0 trigger select
            unsigned int TRG_SEL1:2;           // DMAC Ch.1 trigger select
            unsigned int TRG_SEL2:2;           // DMAC Ch.2 trigger select
            unsigned int TRG_SEL3:2;           // DMAC Ch.3 trigger select
            unsigned int TRG_SEL4:2;           // DMAC Ch.4 trigger select
            unsigned int TRG_SEL5:2;           // DMAC Ch.5 trigger select
            unsigned int TRG_SEL6:2;           // DMAC Ch.6 trigger select
            unsigned int TRG_SEL7:2;           // DMAC Ch.7 trigger select
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ DMAC Trigger Flag Register ]    DMAC_TRG_FLG             0x10  */
/********************************************************************/
    union DMAC_TRG_FLG_tag {
        volatile struct {
            unsigned int TRG0:1;           // DMAC Ch.0 software trigger/trigger status
            unsigned int TRG1:1;           // DMAC Ch.1 software trigger/trigger status
            unsigned int TRG2:1;           // DMAC Ch.2 software trigger/trigger status
            unsigned int TRG3:1;           // DMAC Ch.3 software trigger/trigger status
            unsigned int TRG4:1;           // DMAC Ch.4 software trigger/trigger status
            unsigned int TRG5:1;           // DMAC Ch.5 software trigger/trigger status
            unsigned int TRG6:1;           // DMAC Ch.6 software trigger/trigger status
            unsigned int TRG7:1;           // DMAC Ch.7 software trigger/trigger status
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ DMAC End-of-Transfer Flag Register ]  DMAC_END_FLG       0x14  */
/********************************************************************/
    union DMAC_END_FLG_tag {
        volatile struct {
            unsigned int ENDF0:1;           // DMAC Ch.0 end-of-transfer flag
            unsigned int ENDF1:1;           // DMAC Ch.1 end-of-transfer flag
            unsigned int ENDF2:1;           // DMAC Ch.2 end-of-transfer flag
            unsigned int ENDF3:1;           // DMAC Ch.3 end-of-transfer flag
            unsigned int ENDF4:1;           // DMAC Ch.4 end-of-transfer flag
            unsigned int ENDF5:1;           // DMAC Ch.5 end-of-transfer flag
            unsigned int ENDF6:1;           // DMAC Ch.6 end-of-transfer flag
            unsigned int ENDF7:1;           // DMAC Ch.7 end-of-transfer flag
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ DMAC Running Status Register ]        DMAC_RUN_STAT      0x18  */
/********************************************************************/
    union DMAC_RUN_STAT_tag {
        volatile const struct {
            unsigned int RUN0:1;           // DMAC Ch.0 running status
            unsigned int RUN1:1;           // DMAC Ch.1 running status
            unsigned int RUN2:1;           // DMAC Ch.2 running status
            unsigned int RUN3:1;           // DMAC Ch.3 running status
            unsigned int RUN4:1;           // DMAC Ch.4 running status
            unsigned int RUN5:1;           // DMAC Ch.5 running status
            unsigned int RUN6:1;           // DMAC Ch.6 running status
            unsigned int RUN7:1;           // DMAC Ch.7 running status
        } bCTL;
        volatile const unsigned long ulCTL;
    };

/********************************************************************/
/* [ DMAC Pause Status Register ]      DMAC_PAUSE_STAT        0x1c  */
/********************************************************************/
    union DMAC_PAUSE_STAT_tag {
        volatile const struct {
            unsigned int PAUSE0:1;           // DMAC Ch.0 pause status
            unsigned int PAUSE1:1;           // DMAC Ch.1 pause status
            unsigned int PAUSE2:1;           // DMAC Ch.2 pause status
            unsigned int PAUSE3:1;           // DMAC Ch.3 pause status
            unsigned int PAUSE4:1;           // DMAC Ch.4 pause status
            unsigned int PAUSE5:1;           // DMAC Ch.5 pause status
            unsigned int PAUSE6:1;           // DMAC Ch.6 pause status
            unsigned int PAUSE7:1;           // DMAC Ch.7 pause status
        } bCTL;
        volatile const unsigned long ulCTL;
    };
/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define DMAC_BASE               (0x00302100)

#define DMAC_CH_EN              (*(union DMAC_CH_EN_tag *)(DMAC_BASE + 0x00)).ulCTL
#define DMAC_DMAON(n)           (*(union DMAC_CH_EN_tag *)(DMAC_BASE + 0x00)).bCTL.DMAON##n

#define DMAC_TBL_BASE           (*(volatile unsigned long *)(DMAC_BASE+0x04))

#define DMAC_IE                 (*(union DMAC_IE_tag *)(DMAC_BASE + 0x08)).ulCTL
#define DMAC_DMAIE(n)           (*(union DMAC_IE_tag *)(DMAC_BASE + 0x08)).bCTL.DMAIE##n

#define DMAC_TRG_SEL            (*(union DMAC_TRG_SEL_tag *)(DMAC_BASE + 0x0c)).ulCTL
#define DMAC_TRG_SELx(n)        (*(union DMAC_TRG_SEL_tag *)(DMAC_BASE + 0x0c)).bCTL.TRG_SEL##n

#define DMAC_TRG_FLG            (*(union DMAC_TRG_FLG_tag *)(DMAC_BASE + 0x10)).ulCTL
#define DMAC_TRG(n)             (*(union DMAC_TRG_FLG_tag *)(DMAC_BASE + 0x10)).bCTL.TRG##n

#define DMAC_END_FLG            (*(union DMAC_END_FLG_tag *)(DMAC_BASE + 0x14)).ulCTL
#define DMAC_ENDF(n)            (*(union DMAC_END_FLG_tag *)(DMAC_BASE + 0x14)).bCTL.ENDF##n

#define DMAC_RUN_STAT           (*(union DMAC_RUN_STAT_tag *)(DMAC_BASE + 0x18)).ulCTL
#define DMAC_RUN(n)             (*(union DMAC_RUN_STAT_tag *)(DMAC_BASE + 0x18)).bCTL.RUN##n

#define DMAC_PAUSE_STAT         (*(union DMAC_PAUSE_STAT_tag *)(DMAC_BASE + 0x1c)).ulCTL
#define DMAC_PAUSE(n)           (*(union DMAC_PAUSE_STAT_tag *)(DMAC_BASE + 0x1c)).bCTL.PAUSE##n

#ifdef __cplusplus
}
#endif
#endif                          /* _DMAC_REG_H */

