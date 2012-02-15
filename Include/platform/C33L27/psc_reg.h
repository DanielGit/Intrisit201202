/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : psc_reg.h
 * 	Description : S1C33L27 PSC register definition file
 *
 * 	Revision History:
 *  2010/01/13   Joan Lin     New creation
 ****************************************************************************/

#ifndef _PSC_REG_H
#define _PSC_REG_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/
/********************************************************************/
/* [ Prescaler Ch.0 Control Register ]  PSC0_CTL               0x00 */
/********************************************************************/
    union PSC0_CTL_tag {
        volatile struct {
            unsigned int PRUN:1;        /* Prescaler run/stop control       */
            unsigned int PRUND:1;       /* Prescaler run/stop in debug mode */
            unsigned int DUMMY:6;       /*                                  */
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ Prescaler Ch.1 Control Register ]  PSC1_CTL               0x01 */
/********************************************************************/
    union PSC1_CTL_tag {
        volatile struct {
            unsigned int PRUN:1;        /* Prescaler run/stop control       */
            unsigned int PRUND:1;       /* Prescaler run/stop in debug mode */
            unsigned int DUMMY:6;       /*                                  */
        } bCTL;
        volatile unsigned char ucCTL;
    };


/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define PSC_BASE       (0x00300e00)

/* PSC */
#define PSC_CTL0        (*(union PSC0_CTL_tag *)(PSC_BASE + 0x00)).ucCTL
#define PSC_PRUN0       (*(union PSC0_CTL_tag *)(PSC_BASE + 0x00)).bCTL.PRUN
#define PSC_PRUND0      (*(union PSC0_CTL_tag *)(PSC_BASE + 0x00)).bCTL.PRUND

#define PSC_CTL12       (*(union PSC1_CTL_tag *)(PSC_BASE + 0x01)).ucCTL
#define PSC_PRUN12      (*(union PSC1_CTL_tag *)(PSC_BASE + 0x01)).bCTL.PRUN
#define PSC_PRUND12     (*(union PSC1_CTL_tag *)(PSC_BASE + 0x01)).bCTL.PRUND

#ifdef __cplusplus
}
#endif
#endif                          /* _PSC_REG_H */
