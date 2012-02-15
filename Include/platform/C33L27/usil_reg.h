/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : usil_reg.h
 * 	Description : S1C33L27 USIL register definition file
 *
 * 	Revision History:
 *  2010/01/29   Jackie Zhu   New Creation
 ****************************************************************************/
#ifndef _USIL_REG_H
#define _USIL_REG_H

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************/
/* [ LCD SPI Configure Register ]  USIL_LSCFG               0x280    */
/********************************************************************/
    union USIL_LSCFG_tag {
        volatile struct {
            unsigned int LSCMDEN:1;     // CMD Enable
            unsigned int LSCMD:1;       // CMD Bit
            unsigned int LSCPOL:1;      // Clock polarity select
            unsigned int LSCPHA:1;      // Clock phase select
            unsigned int DUMMY:4;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ LCD SPI IE Register ]  USIL_LSIE                       0x281    */
/********************************************************************/
    union USIL_LSIE_tag {
        volatile struct {
            unsigned int LSTDIE:1;      // Tx Interrupt Enable
            unsigned int DUMMY:7;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ LCD SPI IF Register ]  USIL_LSIF                       0x282    */
/********************************************************************/
    union USIL_LSIF_tag {
        volatile struct {
            unsigned int LSTDIF:1;      // Trans-buffer Empty Flag
            unsigned int LSBSY:1;       // SPI Master Busy
            unsigned int DUMMY:7;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ LCD SPI Mode Set Register ]  USIL_LSDCFG              0x28f    */
/********************************************************************/
    union USIL_LSDCFG_tag {
        volatile struct {
            unsigned int LSDMOD:2;      // LCD SPI Mode                    */
            unsigned int LS18DFM:2;     // LCD SPI 18bpp data format mode  */
            unsigned int DUMMY:4;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ Parallel LCD Access CFG Register ]  USIL_LPCFG         0x290    */
/********************************************************************/
    union USIL_LPCFG_tag {
        volatile struct {
            unsigned int LPRD:1;        // Read Trigger
            unsigned int LPCMD:1;       // CMD Bit
            unsigned int LPSRDEN:1;     // Successive Read Enable
            unsigned int DUMMY:5;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ Parallel LCD Interrupt Enable Register ]  USIL_LPIE    0x291    */
/********************************************************************/
    union USIL_LPIE_tag {
        volatile struct {
            unsigned int LPWRIE:1;      // Write interrupt enable
            unsigned int LPRDIE:1;      // Read interrupt enable
            unsigned int DUMMY:6;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ Parallel LCD Interrupt Flag Register ]  USIL_LPIF      0x292    */
/********************************************************************/
    union USIL_LPIF_tag {
        volatile struct {
            unsigned int LPWRIF:1;      // Write interrupt flag
            unsigned int LPRDIF:1;      // Read interrupt flag
            unsigned int LPBSY:1;       // Busy Flag
            unsigned int DUMMY:5;
        } bCTL;
        volatile unsigned char ucCTL;
    };
/********************************************************************/
/* [ Parallel LCD Access Timing Register ]  USIL_LPAC       0x29f    */
/********************************************************************/
    union USIL_LPAC_tag {
        volatile struct {
            unsigned int LPWT:4;   // Wait Cycle
            unsigned int LPST:2;   // Setup Cycle
            unsigned int LPHD:2;   // Hold Cycle
        } bCTL;
        volatile unsigned char ucCTL;
    };
/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define USIL_BASE                (0x00300400)

#define USIL_LSCFG               (*(union USIL_LSCFG_tag *)(USIL_BASE+0x280)).ucCTL
#define USIL_LSCMDEN             (*(union USIL_LSCFG_tag *)(USIL_BASE+0x280)).bCTL.LSCMDEN
#define USIL_LSCMD               (*(union USIL_LSCFG_tag *)(USIL_BASE+0x280)).bCTL.LSCMD
#define USIL_LSCPOL              (*(union USIL_LSCFG_tag *)(USIL_BASE+0x280)).bCTL.LSCPOL
#define USIL_LSCPHA              (*(union USIL_LSCFG_tag *)(USIL_BASE+0x280)).bCTL.LSCPHA

#define USIL_LSIE                (*(union USIL_LSIE_tag *)(USIL_BASE+0x281)).ucCTL
#define USIL_LSTDIE              (*(union USIL_LSIE_tag *)(USIL_BASE+0x281)).bCTL.LSTDIE

#define USIL_LSIF                (*(union USIL_LSIF_tag *)(USIL_BASE+0x282)).ucCTL
#define USIL_LSTDIF              (*(union USIL_LSIF_tag *)(USIL_BASE+0x282)).bCTL.LSTDIF
#define USIL_LSBSY               (*(union USIL_LSIF_tag *)(USIL_BASE+0x282)).bCTL.LSBSY

#define USIL_LSDCFG              (*(union USIL_LSDCFG_tag *)(USIL_BASE+0x28f)).ucCTL
#define USIL_LSMOD               (*(union USIL_LSDCFG_tag *)(USIL_BASE+0x28f)).bCTL.LSMOD
#define USIL_LS18DFM             (*(union USIL_LSDCFG_tag *)(USIL_BASE+0x28f)).bCTL.LS18DFM

#define USIL_LPCFG               (*(union USIL_LPCFG_tag *)(USIL_BASE+0x290)).ucCTL
#define USIL_LPRD                (*(union USIL_LPCFG_tag *)(USIL_BASE+0x290)).bCTL.LPRD
#define USIL_LPCD                (*(union USIL_LPCFG_tag *)(USIL_BASE+0x290)).bCTL.LPCD
#define USIL_LPSRDEN             (*(union USIL_LPCFG_tag *)(USIL_BASE+0x290)).bCTL.LPSRDEN

#define USIL_LPIE                (*(union USIL_LPIE_tag *)(USIL_BASE+0x291)).ucCTL
#define USIL_LPWRIE              (*(union USIL_LPIE_tag *)(USIL_BASE+0x291)).bCTL.LPWRIE
#define USIL_LPRDIE              (*(union USIL_LPIE_tag *)(USIL_BASE+0x291)).bCTL.LPRDIE

#define USIL_LPIF                (*(union USIL_LPIF_tag *)(USIL_BASE+0x292)).ucCTL
#define USIL_LPWRIF              (*(union USIL_LPIF_tag *)(USIL_BASE+0x292)).bCTL.LPWRIF
#define USIL_LPRDIF              (*(union USIL_LPIF_tag *)(USIL_BASE+0x292)).bCTL.LPRDIF
#define USIL_LPBSY               (*(union USIL_LPIF_tag *)(USIL_BASE+0x292)).bCTL.LPBSY

#define USIL_LPAC                (*(union USIL_LPAC_tag *)(USIL_BASE+0x29f)).ucCTL
#define USIL_LPWT                (*(union USIL_LPAC_tag *)(USIL_BASE+0x29f)).bCTL.LPWT
#define USIL_LPST                (*(union USIL_LPAC_tag *)(USIL_BASE+0x29f)).bCTL.LPST
#define USIL_LPHD                (*(union USIL_LPAC_tag *)(USIL_BASE+0x29f)).bCTL.LPHD

/****************************************************************************
 * 	Function prototypes
 ****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif                          /* USI_REG_H */
