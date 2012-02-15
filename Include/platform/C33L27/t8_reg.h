/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : t8_reg.h
 * 	Description : S1C33L27 T8 register definition file
 *
 * 	Revision History:
 *  2010/01/29   Joan Lin     New creation
 ****************************************************************************/

#ifndef _T8_REG_H
#define _T8_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/

/********************************************************************/
/* [ T8F control register ]  T8F_CTL                        0x06    */
/********************************************************************/
    union T8F_CTL_tag {
        volatile struct {
            unsigned int PRUN:1;        /* 8-bit Timer Run/Stop Control Bit           */
            unsigned int PRESET:1;      /* 8-bit Timer Reset Bit                      */
            unsigned int DUMMY1:2;      /*                                            */
            unsigned int TRMD:1;        /* 8-bit Repeat Mode (1: one-shot; 0: repeat) */
            unsigned int DUMMY2:3;      /*                                            */
            unsigned int TFMD:4;        /* Fine Mode Type                             */
            unsigned int DUMMY3:4;      /*                                            */
        } bCTL;
        volatile unsigned short usCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define T8F_BASE       (0x00301100)

/* T8F: n=0, 1, 2, 3 */
/* T8 : n=4, 5, 6, 7 */

#define T8F_CLK(n)     (*(volatile unsigned char *)(T8F_BASE + 0x10*(n)))       /* Input Clock Select */
#define T8F_TR(n)      (*(volatile unsigned char *)(T8F_BASE + 0x10*(n) + 0x2)) /* Reload Data Reg.  */
#define T8F_TC(n)      (*(volatile const unsigned char *)(T8F_BASE + 0x10*(n) + 0x4))   /* Counter Data Reg. */

#define T8F_CTL(n)     (*(volatile unsigned short *)(T8F_BASE + 0x10*(n) + 0x6))
#define T8F_PRUN(n)    (*(union T8F_CTL_tag *)(T8F_BASE + 0x10*(n) + 0x6)).bCTL.PRUN
#define T8F_PRESET(n)  (*(union T8F_CTL_tag *)(T8F_BASE + 0x10*(n) + 0x6)).bCTL.PRESET
#define T8F_TRMD(n)    (*(union T8F_CTL_tag *)(T8F_BASE + 0x10*(n) + 0x6)).bCTL.TRMD
#define T8F_TFMD(n)    (*(union T8F_CTL_tag *)(T8F_BASE + 0x10*(n) + 0x6)).bCTL.TFMD    /* Only valid for T8F */

#define T8F_IF(n)      (*(volatile unsigned char *)(T8F_BASE + 0x10*(n) + 0x8))
#define T8F_IE(n)      (*(volatile unsigned char *)(T8F_BASE + 0x10*(n) + 0x9))

#ifdef __cplusplus
}
#endif
#endif                          /* _T8_REG_H */
