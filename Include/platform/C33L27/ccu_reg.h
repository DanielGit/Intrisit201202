/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : ccu_reg.h
 * 	Description : S1C33L27 CCU register definition file
 *
 * 	Revision History:
 *  2010/01/05   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _CCU_REG_H
#define _CCU_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ Cache Configuration Register ]  CCU_CFG                   0x00 */
/********************************************************************/
    union CCU_CFG_tag {
        volatile struct {
            unsigned int DC:1;          // Data cache enable
            unsigned int IC:1;          // Instruction cache enable
            unsigned int SBRK:1;        // Software break enable
            unsigned int DUMMY:5;
            unsigned int WBEN:1;        // Write buffer enable
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Cache Area Select Register ]  CCU_AREA                    0x04 */
/********************************************************************/
    union CCU_AREA_tag {
        volatile struct {
            unsigned int ARDC:3;          // Data cache area select
            unsigned int DUMMY:1;
            unsigned int ARIC:3;          // Instruction cache area select
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Cache Lock Register ]  CCU_LK                             0x08 */
/********************************************************************/
    union CCU_LK_tag {
        volatile struct {
            unsigned int LKPRI0:1;      // Interrupt level 0 cache-lock enable
            unsigned int LKPRI1:1;      // Interrupt level 1 cache-lock enable
            unsigned int LKPRI2:1;      // Interrupt level 2 cache-lock enable
            unsigned int LKPRI3:1;      // Interrupt level 3 cache-lock enable
            unsigned int LKPRI4:1;      // Interrupt level 4 cache-lock enable
            unsigned int LKPRI5:1;      // Interrupt level 5 cache-lock enable
            unsigned int LKPRI6:1;      // Interrupt level 6 cache-lock enable
            unsigned int LKPRI7:1;      // Interrupt level 7 cache-lock enable
        } bCTL;
        volatile unsigned long ulCTL;
    };

/********************************************************************/
/* [ Cache Status Register ]  CCU_STAT                         0x0c */
/********************************************************************/
    union CCU_STAT_tag {
        volatile const struct {
            unsigned int DCS:1;        // Data cache operating status
            unsigned int ICS:1;        // Instruction cache operating status
            unsigned int DCLKS:1;      // Data cache lock status
            unsigned int ICLKS:1;      // Instruction cache lock status
        } bCTL;
        volatile const unsigned long ulCTL;
    };

/********************************************************************/
/* [ Cache Status Register ]  CCU_WB_STAT                      0x18 */
/********************************************************************/
    union CCU_WB_STAT_tag {
        volatile const struct {
            unsigned int DUMMY:8;
            unsigned int WBEMPTY:1;       // Write buffer empty flag
            unsigned int WEFINISH:1;      // Write-finish flag
        } bCTL;
        volatile const unsigned long ulCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define CCU_BASE            (0x00302300)

#define CCU_CFG             (*(union CCU_CFG_tag *)(CCU_BASE + 0x00)).ulCTL
#define CCU_DC              (*(union CCU_CFG_tag *)(CCU_BASE + 0x00)).bCTL.DC
#define CCU_IC              (*(union CCU_CFG_tag *)(CCU_BASE + 0x00)).bCTL.IC
#define CCU_SBRK            (*(union CCU_CFG_tag *)(CCU_BASE + 0x00)).bCTL.SBRK
#define CCU_WBEN            (*(union CCU_CFG_tag *)(CCU_BASE + 0x00)).bCTL.WBEN

#define CCU_AREA            (*(union CCU_AREA_tag *)(CCU_BASE + 0x04)).ulCTL
#define CCU_ARDC            (*(union CCU_AREA_tag *)(CCU_BASE + 0x04)).bCTL.ARDC
#define CCU_ARIC            (*(union CCU_AREA_tag *)(CCU_BASE + 0x04)).bCTL.ARIC

#define CCU_LK              (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).ulCTL
#define CCU_LKPRI0          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI0
#define CCU_LKPRI1          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI1
#define CCU_LKPRI2          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI2
#define CCU_LKPRI3          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI3
#define CCU_LKPRI4          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI4
#define CCU_LKPRI5          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI5
#define CCU_LKPRI6          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI6
#define CCU_LKPRI7          (*(union CCU_LK_tag *)(CCU_BASE + 0x08)).bCTL.LKPRI7

#define CCU_STAT            (*(union CCU_STAT_tag *)(CCU_BASE + 0x0c)).ulCTL
#define CCU_DCS             (*(union CCU_STAT_tag *)(CCU_BASE + 0x0c)).bCTL.DCS
#define CCU_ICS             (*(union CCU_STAT_tag *)(CCU_BASE + 0x0c)).bCTL.ICS
#define CCU_DCLKS           (*(union CCU_STAT_tag *)(CCU_BASE + 0x0c)).bCTL.DCLKS
#define CCU_ICLKS           (*(union CCU_STAT_tag *)(CCU_BASE + 0x0c)).bCTL.ICLKS

#define CCU_WB_STAT         (*(union CCU_WB_STAT_tag *)(CCU_BASE + 0x18)).ulCTL
#define CCU_WBEMPTY         (*(union CCU_WB_STAT_tag *)(CCU_BASE + 0x18)).bCTL.WBEMPTY
#define CCU_WEFINISH        (*(union CCU_WB_STAT_tag *)(CCU_BASE + 0x18)).bCTL.WEFINISH


#ifdef __cplusplus
}
#endif
#endif                          /* _CCU_REG_H */
