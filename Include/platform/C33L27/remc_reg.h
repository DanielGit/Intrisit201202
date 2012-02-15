/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : remc_reg.h
 * 	Description : S1C33L27 REMC register definition file
 *
 * 	Revision History:
 *  2010/01/14   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _REMC_REG_H
#define _REMC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ REMC Configuration Register ]  REMC_CFG                   0x00 */
/********************************************************************/
    union REMC_CFG_tag {
        volatile struct {
            unsigned int REMEN:1;       // REMC enable
            unsigned int REMMD:1;       // REMC mode select
            unsigned int DUMMY:6;
            unsigned int LCCLK:4;       // Length counter clock division ratio select
            unsigned int CGCLK:4;       // Carrier generator clock division ratio select
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ REMC Carrier Length Setup Register ]  REMC_CAR            0x02 */
/********************************************************************/
    union REMC_CAR_tag {
        volatile struct {
            unsigned int REMCH:6;       // Carrier H length setup
            unsigned int DUMMY:2;
            unsigned int REMCL:6;       // Carrier L length setup
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ REMC Length Counter Register ]  REMC_LCNT                 0x04 */
/********************************************************************/
    union REMC_LCNT_tag {
        volatile struct {
            unsigned int REMDT:1;       // Transmit/receive data
            unsigned int DUMMY:7;
            unsigned int REMLEN:8;      // Transmit/receive data length count
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ REMC Interrupot Control Register ]  REMC_INT              0x06 */
/********************************************************************/
    union REMC_INT_tag {
        volatile struct {
            unsigned int REMUIE:1;      // Underflow interrupt enable
            unsigned int REMRIE:1;      // Rising edge interrrupt enable
            unsigned int REMFIE:1;      // Falling edge interrupt enable
            unsigned int DUMMY:5;
            unsigned int REMUIF:1;      // Underflow interrupt flag
            unsigned int REMRIF:1;      // Rising edge interrrupt flag
            unsigned int REMFIF:1;      // Falling edge interrupt flag
        } bCTL;
        volatile unsigned short usCTL;
    };
/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define REMC_BASE               (0x00301500)

#define REMC_CFG                (*(union REMC_CFG_tag *)(REMC_BASE + 0x00)).usCTL
#define REMC_REMEN              (*(union REMC_CFG_tag *)(REMC_BASE + 0x00)).bCTL.REMEN
#define REMC_REMMD              (*(union REMC_CFG_tag *)(REMC_BASE + 0x00)).bCTL.REMMD
#define REMC_LCCLK              (*(union REMC_CFG_tag *)(REMC_BASE + 0x00)).bCTL.LCCLK
#define REMC_CGCLK              (*(union REMC_CFG_tag *)(REMC_BASE + 0x00)).bCTL.CGCLK

#define REMC_CAR                (*(union REMC_CAR_tag *)(REMC_BASE + 0x02)).usCTL
#define REMC_REMCH              (*(union REMC_CAR_tag *)(REMC_BASE + 0x02)).bCTL.REMCH
#define REMC_REMCL              (*(union REMC_CAR_tag *)(REMC_BASE + 0x02)).bCTL.REMCL

#define REMC_LCNT               (*(union REMC_LCNT_tag *)(REMC_BASE + 0x04)).usCTL
#define REMC_REMDT              (*(union REMC_LCNT_tag *)(REMC_BASE + 0x04)).bCTL.REMDT
#define REMC_REMLEN             (*(union REMC_LCNT_tag *)(REMC_BASE + 0x04)).bCTL.REMLEN

#define REMC_INT                (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).usCTL
#define REMC_REMUIE             (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).bCTL.REMUIE
#define REMC_REMRIE             (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).bCTL.REMRIE
#define REMC_REMFIE             (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).bCTL.REMFIE
#define REMC_REMUIF             (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).bCTL.REMUIF
#define REMC_REMRIF             (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).bCTL.REMRIF
#define REMC_REMFIF             (*(union REMC_INT_tag *)(REMC_BASE + 0x06)).bCTL.REMFIF

#ifdef __cplusplus
}
#endif
#endif                          /* _REMC_REG_H */

