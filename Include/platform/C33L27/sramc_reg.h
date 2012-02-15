/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : sramc_reg.h
 * 	Description : S1C33L27 SRAMC register definition file
 *
 * 	Revision History:
 *  2010/01/06   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _SRAMC_REG_H
#define _SRAMC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [SRAMC #CE4~#CE11 Timing Config. reg] pSRAMC_TIM_CFG 0x0030_2220 */
/********************************************************************/
    union SRAMC_TMG_tag {
        volatile struct {
            unsigned int WAIT_CE4:4;    //#CE4 static wait cycle (0~15 cycle)
            unsigned int HOLD_CE4:2;    //#CE4 hold cycle (1~4 cycle)
            unsigned int SETUP_CE4:2;   //#CE4 setup cycle (1~4 cycle)

            unsigned int WAIT_CE5:4;    //#CE5 static wait cycle (0~15 cycle)
            unsigned int HOLD_CE5:2;    //#CE5 hold cycle (1~4 cycle)
            unsigned int SETUP_CE5:2;   //#CE5 setup cycle (1~4 cycle)

            unsigned int DUMMY:8;

            unsigned int WAIT_CE7:4;    //#CE7 static wait cycle (0~15 cycle)
            unsigned int HOLD_CE7:2;    //#CE7 hold cycle (1~4 cycle)
            unsigned int SETUP_CE7:2;   //#CE7 setup cycle (1~4 cycle)

            unsigned int WAIT_CE8:4;    //#CE8 static wait cycle (0~15 cycle)
            unsigned int HOLD_CE8:2;    //#CE8 hold cycle (1~4 cycle)
            unsigned int SETUP_CE8:2;   //#CE8 setup cycle (1~4 cycle)

            unsigned int WAIT_CE9:4;    //#CE9 static wait cycle (0~15 cycle)
            unsigned int HOLD_CE9:2;    //#CE9 hold cycle (1~4 cycle)
            unsigned int SETUP_CE9:2;   //#CE9 setup cycle (1~4 cycle)

            unsigned int WAIT_CE10:4;   //#CE10 static wait cycle (0~15 cycle)
            unsigned int HOLD_CE10:2;   //#CE10 hold cycle (1~4 cycle)
            unsigned int SETUP_CE10:2;  //#CE10 setup cycle (1~4 cycle)

        } bCTL;

    };


/********************************************************************/
/* [SRAMC #CE4~#CE11 Device Config. reg] SRAMC_TYPE     0x0030_2228 */
/********************************************************************/
    union SRAMC_TYPE_tag {
        volatile struct {
            unsigned int TYPE_CE4:2;   //#CE4 Device Type (00: 16-bit A0, 01: 16-bit BSL, 1x: 8-bit)
            unsigned int TYPE_CE5:2;   //~
            unsigned int DUMMY:2;      //~
            unsigned int TYPE_CE7:2;   //~
            unsigned int TYPE_CE8:2;   //~
            unsigned int TYPE_CE9:2;   //~
            unsigned int TYPE_CE10:2;  //~
        } bCTL;
        volatile unsigned long ulCTL;
    };


/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define SRAMC_BASE              (0x00302220)

#define SRAMC_TMG47             (*(volatile unsigned long *)(SRAMC_BASE+0x00))
#define SRAMC_TMG810            (*(volatile unsigned long *)(SRAMC_BASE+0x04))

#define SRAMC_CEWAIT(n)         (*(union SRAMC_TMG_tag *)(SRAMC_BASE+0x00)).bCTL.WAIT_CE##n
#define SRAMC_CEHOLD(n)         (*(union SRAMC_TMG_tag *)(SRAMC_BASE+0x00)).bCTL.HOLD_CE##n
#define SRAMC_CESETUP(n)        (*(union SRAMC_TMG_tag *)(SRAMC_BASE+0x00)).bCTL.SETUP_CE##n

#define SRAMC_TYPE              (*(union SRAMC_TYPE_tag *)(SRAMC_BASE+0x08)).ulCTL
#define SRAMC_CETYPE(n)         (*(union SRAMC_TYPE_tag *)(SRAMC_BASE+0x08)).bCTL.TYPE_CE##n


#ifdef __cplusplus
}
#endif
#endif                          /* _SRAMC_REG_H */

