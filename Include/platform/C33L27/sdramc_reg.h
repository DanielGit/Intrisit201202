/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : sdramc_reg.h
 * 	Description : S1C33L27 SDRAMC register definition file
 *
 * 	Revision History:
 *  2010/01/06   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _SDRAMC_REG_H
#define _SDRAMC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ SDRAM Initializaton Register ]  SDRAMC_INIT               0x00 */
/********************************************************************/
    union SDRAMC_INIT_tag {
        volatile struct {
            unsigned int INIREF:1;              // REF command enable for init.
            unsigned int INIPRE:1;              // PALL command enable for init.
            unsigned int INIMRS:1;              // MRS command enable for init.
            const unsigned int INIDO:1;         // SDRAM initialization status
            unsigned int SDON:1;                // SDRAM controller enable
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ SDRAM Configuration Register ]  SDRAMC_CFG                0x04 */
/********************************************************************/
    union SDRAMC_CFG_tag {
        volatile struct {
            unsigned int ADDRC:3;               // SDRAM address configuration
            unsigned int DUMMY1:1;
            unsigned int T80NS:4;               // Number of SDRAM Trc, Trfc and Txsr cycles
            unsigned int T60NS:3;               // Number of SDRAM Tras cycles
            unsigned int DUMMY2:1;
            unsigned int T24NS:2;               // Number of SDRAM Trp and Trcd cycles
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ SDRAM Refresh Control Register ]  SDRAMC_REF              0x08 */
/********************************************************************/
    union SDRAMC_REF_tag {
        volatile struct {
            unsigned int AURCO:12;              // SDRAM auto-refresh counter
            unsigned int DUMMY:4;
            unsigned int SELCO:7;               // SDRAM self-refresh ciybter
            unsigned int SELEN:1;               // SDRAM self-refresh enable
            unsigned int SCKON:1;               // SDRAM clock during self-refresh
            const unsigned int SREFDO:1;        // SDRAM self-refresh statusrcd cycles
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ SDRAM Application Configuration Register ]  SDRAMC_APP    0x10 */
/********************************************************************/
    union SDRAMC_APP_tag {
        volatile struct {
            unsigned int DUMMY:2;
            unsigned int CAS:2;                 // CAS latency setup
        } bCTL;
        volatile unsigned long ulCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define SDRAMC_BASE             (0x00302200)

#define SDRAMC_INIT             (*(union SDRAMC_INIT_tag *)(SDRAMC_BASE + 0x00)).ulCTL
#define SDRAMC_INIREF           (*(union SDRAMC_INIT_tag *)(SDRAMC_BASE + 0x00)).bCTL.INIREF
#define SDRAMC_INIPRE           (*(union SDRAMC_INIT_tag *)(SDRAMC_BASE + 0x00)).bCTL.INIPRE
#define SDRAMC_INIMRS           (*(union SDRAMC_INIT_tag *)(SDRAMC_BASE + 0x00)).bCTL.INIMRS
#define SDRAMC_INIDO            (*(union SDRAMC_INIT_tag *)(SDRAMC_BASE + 0x00)).bCTL.INIDO
#define SDRAMC_SDON             (*(union SDRAMC_INIT_tag *)(SDRAMC_BASE + 0x00)).bCTL.SDON

#define SDRAMC_CFG              (*(union SDRAMC_CFG_tag *)(SDRAMC_BASE + 0x04)).ulCTL
#define SDRAMC_ADDRC            (*(union SDRAMC_CFG_tag *)(SDRAMC_BASE + 0x04)).bCTL.ADDRC
#define SDRAMC_T80NS            (*(union SDRAMC_CFG_tag *)(SDRAMC_BASE + 0x04)).bCTL.T80NS
#define SDRAMC_T60NS            (*(union SDRAMC_CFG_tag *)(SDRAMC_BASE + 0x04)).bCTL.T60NS
#define SDRAMC_T24NS            (*(union SDRAMC_CFG_tag *)(SDRAMC_BASE + 0x04)).bCTL.T24NS

#define SDRAMC_REF              (*(union SDRAMC_REF_tag *)(SDRAMC_BASE + 0x08)).ulCTL
#define SDRAMC_AURCO            (*(union SDRAMC_REF_tag *)(SDRAMC_BASE + 0x08)).bCTL.AURCO
#define SDRAMC_SELCO            (*(union SDRAMC_REF_tag *)(SDRAMC_BASE + 0x08)).bCTL.SELCO
#define SDRAMC_SELEN            (*(union SDRAMC_REF_tag *)(SDRAMC_BASE + 0x08)).bCTL.SELEN
#define SDRAMC_SCKON            (*(union SDRAMC_REF_tag *)(SDRAMC_BASE + 0x08)).bCTL.SCKON
#define SDRAMC_SREFDO           (*(union SDRAMC_REF_tag *)(SDRAMC_BASE + 0x08)).bCTL.SREFDO

#define SDRAMC_CAS              (*(union SDRAMC_APP_tag *)(SDRAMC_BASE + 0x10)).bCTL.CAS

#ifdef __cplusplus
}
#endif
#endif                          /* _SDRAM_REG_H */

