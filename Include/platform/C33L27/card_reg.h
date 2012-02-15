/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : card_reg.h
 * 	Description : S1C33L27 card register definition file
 *
 * 	Revision History:
 *  2010/01/14   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _CARD_REG_H
#define _CARD_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ CARD EDC Trigger Register ]  CARD_EDC_TRG                 0x00 */
/********************************************************************/
    union CARD_EDC_TRG_tag {
        volatile struct {
            unsigned int RST:1;                 // EDC RESET
            unsigned int END:1;                 // 10-symbol EDC end trigger
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ CARD_EDC_CTRL_REGISTER ]   CARD_EDC_CTRL                  0x01 */
/********************************************************************/
    union CARD_EDC_CTRL_tag {
        volatile struct {
            unsigned int EN:1;                 // EDC enable
            unsigned int MOD:1;                // EDC operation mode select
            unsigned int SLT:1;                // EDC correction mode select
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ CARD EDC Flag Register ] CARD_EDC_FLG                     0x10 */
/********************************************************************/
    union CARD_EDC_FLG_tag {
        volatile struct {
            unsigned int EER:1;                  // EDC error flag
            unsigned int DUMMY:3;
            unsigned int BSY:2;                  // 10-symbol EDC busy flag
        } bCTL;
        volatile unsigned char ucCTL;
    };


/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define CARD_BASE               (0x00300f00)

#define CARD_EDC_TRG            (*(union CARD_EDC_TRG_tag *)(CARD_BASE + 0x00)).ucCTL
#define CARD_RST                (*(union CARD_EDC_TRG_tag *)(CARD_BASE + 0x00)).bCTL.RST
#define CARD_END                (*(union CARD_EDC_TRG_tag *)(CARD_BASE + 0x00)).bCTL.END

#define CARD_EDC_CTRL           (*(union CARD_EDC_CTRL_tag *)(CARD_BASE + 0x01)).ucCTL
#define CARD_EN                 (*(union CARD_EDC_CTRL_tag *)(CARD_BASE + 0x01)).bCTL.EN
#define CARD_MOD                (*(union CARD_EDC_CTRL_tag *)(CARD_BASE + 0x01)).bCTL.MOD
#define CARD_SLT                (*(union CARD_EDC_CTRL_tag *)(CARD_BASE + 0x01)).bCTL.SLT

#define CARD_EDC_RGN            (*(volatile unsigned char *)(CARD_BASE + 0x02))

#define CARD_EDC_FLG            (*(union CARD_EDC_FLG_tag *)(CARD_BASE + 0x10)).ucCTL
#define CARD_EER                (*(union CARD_EDC_FLG_tag *)(CARD_BASE + 0x10)).bCTL.EER
#define CARD_BSY                (*(union CARD_EDC_FLG_tag *)(CARD_BASE + 0x10)).bCTL.BSY

#define CARD_EDC10_COD(n)       (*(volatile unsigned char *)(CARD_BASE + 0x20 + (n)))
#define CARD_EDC2_COD(n)        (*(volatile unsigned char *)(CARD_BASE + 0x40 + (n)))

#ifdef __cplusplus
}
#endif
#endif                          /* _CARD_REG_H */

