/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : wdt_reg.h
 * 	Description : S1C33L27 WDT register definition file
 *
 * 	Revision History:
 *  2010/01/14   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _WDT_REG_H
#define _WDT_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ WDT Enable and Setup Register ] WDT_EN                 0x02    */
/********************************************************************/
    union WDT_EN_tag {
        volatile struct {
            unsigned int RESEN:1;       // WDT Reset enable
            unsigned int NMIEN:1;       // WDT NMI enable
            unsigned int DUMMY:2;
            unsigned int RUNSTP:1;      // WDT Run/Stop control
            unsigned int CLKEN:1;       // WDT clock output control
            unsigned int CLKSEL:1;      // WDT input clock select
        } bCTL;
        volatile unsigned short usCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define WDT_BASE                (0x00301000)

#define WDT_PROTECT             (*(volatile unsigned short *)(WDT_BASE+0x00))

#define WDT_EN                  (*(union WDT_EN_tag *)(WDT_BASE + 0x02)).usCTL
#define WDT_RESEN               (*(union WDT_EN_tag *)(WDT_BASE + 0x02)).bCTL.RESEN
#define WDT_NMIEN               (*(union WDT_EN_tag *)(WDT_BASE + 0x02)).bCTL.NMIEN
#define WDT_RUNSTP              (*(union WDT_EN_tag *)(WDT_BASE + 0x02)).bCTL.RUNSTP
#define WDT_CLKEN               (*(union WDT_EN_tag *)(WDT_BASE + 0x02)).bCTL.CLKEN
#define WDT_CLKSEL              (*(union WDT_EN_tag *)(WDT_BASE + 0x02)).bCTL.CLKSEL

#define WDT_CMP                 (*(volatile unsigned long *)(WDT_BASE+0x04))
#define WDT_CNT                 (*(const volatile unsigned long *)(WDT_BASE+0x08))
#define WDT_CTL                 (*(volatile unsigned char *)(WDT_BASE+0x0c))

#ifdef __cplusplus
}
#endif
#endif                          /* _WDT_REG_H */

