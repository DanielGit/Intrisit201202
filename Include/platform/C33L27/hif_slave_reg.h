/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : hif_slave_reg.h
 * 	Description : S1C33L27 HIF Slave register definition file
 *
 * 	Revision History:
 *  2010/01/14   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _HIF_SLAVE_REG_H
#define _HIF_SLAVE_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************/
/* [ HIF Extended Control Register ]  HIF_EXTCTRL              0x01 */
/********************************************************************/
    union HIF_EXTCTRL_tag {
        volatile struct {
            unsigned int DUMMY:1;
            unsigned int HINT_MD:1;     // host interrupt mode select
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/*	[	HIF Semaphore register x]       HIF_SMPHx	      0x08~0x0f */
/********************************************************************/
union HIF_SMPHx_tag {
    volatile struct {
        unsigned int SMPHx:1;
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*	[	HIF Flag Register 0]       HIF_FLAG0	               0x18 */
/********************************************************************/
union HIF_FLAG0_tag {
    volatile struct {
        unsigned int SINT:1;        // slave interrupt flag
        unsigned int SFLAG0:1;      // slave flag 0
        unsigned int SFLAG1:1;      // slave flag 1
        unsigned int SFLAG2:1;      // slave flag 2
        unsigned int SFLAG3:1;      // slave flag 3
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*	[	HIF Flag Register 1]       HIF_FLAG1	               0x19 */
/********************************************************************/
union HIF_FLAG1_tag {
    volatile struct {
        unsigned int HINT:1;        // host interrupt flag
        unsigned int HFLAG0:1;      // host flag 0
        unsigned int HFLAG1:1;      // host flag 1
        unsigned int HFLAG2:1;      // host flag 2
    } bCTL;
    volatile unsigned char ucCTL;
};
/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define HIF_BASE                (0x00300900)

#define HIF_SLAVE_HINT_MD       (*(union HIF_EXTCTRL_tag *)(HIF_BASE+0x1)).bCTL.HINT_MD

#define HIF_SLAVE_SMPH(n)       (*(union HIF_SMPHx_tag *)(HIF_BASE+0x8+n)).bCTL.SMPHx

#define HIF_SLAVE_SCRATCH(n)    (*(volatile unsigned char *)(HIF_BASE+0x10+n))

#define HIF_SLAVE_FLAG0         (*(union HIF_FLAG0_tag *)(HIF_BASE + 0x18)).ulCTL
#define HIF_SLAVE_SINT          (*(union HIF_FLAG0_tag *)(HIF_BASE + 0x18)).bCTL.SINT
#define HIF_SLAVE_SFLAG0        (*(union HIF_FLAG0_tag *)(HIF_BASE + 0x18)).bCTL.SFLAG0
#define HIF_SLAVE_SFLAG1        (*(union HIF_FLAG0_tag *)(HIF_BASE + 0x18)).bCTL.SFLAG1
#define HIF_SLAVE_SFLAG2        (*(union HIF_FLAG0_tag *)(HIF_BASE + 0x18)).bCTL.SFLAG2
#define HIF_SLAVE_SFLAG3        (*(union HIF_FLAG0_tag *)(HIF_BASE + 0x18)).bCTL.SFLAG3

#define HIF_SLAVE_FLAG1         (*(union HIF_FLAG1_tag *)(HIF_BASE + 0x19)).ulCTL
#define HIF_SLAVE_HINT          (*(union HIF_FLAG1_tag *)(HIF_BASE + 0x19)).bCTL.HINT
#define HIF_SLAVE_HFLAG0        (*(union HIF_FLAG1_tag *)(HIF_BASE + 0x19)).bCTL.HFLAG0
#define HIF_SLAVE_HFLAG1        (*(union HIF_FLAG1_tag *)(HIF_BASE + 0x19)).bCTL.HFLAG1
#define HIF_SLAVE_HFLAG2        (*(union HIF_FLAG1_tag *)(HIF_BASE + 0x19)).bCTL.HFLAG2


#ifdef __cplusplus
}
#endif
#endif                          /* _HIF_SLAVE_REG_H */

