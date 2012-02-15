/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : t16p_reg.h
 * 	Description : S1C33L27 T16P register definition file
 *
 * 	Revision History:
 *  2010/01/27   Joan Lin   New creation
 ****************************************************************************/

#ifndef _T16P_REG_H
#define _T16P_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/

/********************************************************************/
/* [ T16P volume control register ]  T16P_VOL_CTL        REG[0x06]  */
/********************************************************************/
    union T16P_VOL_CTL_tag {
        volatile struct {
            unsigned int VOLSEL:7;      /* T16P Volume level select                 */
            unsigned int VOLBPS:1;      /* T16P Volume control enable               */
        } bCTL;
        volatile unsigned short usCTL;
    };

/********************************************************************/
/* [ T16P control register ]  T16P_CTL                   REG[0x08]  */
/********************************************************************/
    union T16P_CTL_tag {
        volatile struct {
            unsigned int DUMMY1:1;      /*                                          */
            unsigned int PRESET:1;      /* T16P reset                               */
            unsigned int DUMMY2:1;      /*                                          */
            unsigned int CLKSEL:1;      /* Input clock select (1:Ext; 0:Int)        */
            unsigned int INITOL:1;      /* Initial output level select (1:H; 0:L)   */
            unsigned int DUMMY3:1;      /*                                          */
            unsigned int SELFM:1;       /* Fine mode select (1:Fine; 0:Normal)      */
            unsigned int CHSYNC:1;      /* Channel sync enable                      */

            unsigned int SPLTMD:2;      /* Split mode select                        */
            unsigned int SGNSEL:1;      /* Data format select (1:Signed;0:Unsigned) */
            unsigned int RESSEL:1;      /* Data resolution select (1:16bit; 0:8bit) */
            unsigned int BCNT:4;        /* B match count (0~15)                     */
        } bCTL;
        volatile unsigned short usCTL;
    };

/**********************************************************************/
/* [ T16P interrupt control register ] T16P_INT            REG[0x0e]  */
/**********************************************************************/
    union T16P_INT_tag {
        volatile struct {
            unsigned int INTAEN:1;      /* A Match Interrupt Enable                 */
            unsigned int INTBEN:1;      /* B Match Interrupt Enable                 */
            unsigned int INTBEEN:1;     /* Buffer Empty Interrupt Enable            */
            unsigned int DUMMY:5;       /*                                          */
            unsigned int INTAF:1;       /* A Match Interrupt Flag                   */
            unsigned int INTBF:1;       /* B Match Interrupt Flag                   */
            unsigned int BUFEF:1;       /* Buffer Empty Interrupt Flag              */
        } bCTL;
        volatile unsigned short usCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define T16P_BASE              (0x00301200)

/* Channel 0 */

#define T16P_CH0_A             (*(volatile unsigned short *)(T16P_BASE+0x00))      /* T16P Compare A Buffer */
#define T16P_CH0_B             (*(volatile unsigned short *)(T16P_BASE+0x02))      /* T16P Compare B Buffer */
#define T16P_CH0_CNT_DATA      (*(volatile unsigned short *)(T16P_BASE+0x04))      /* T16P Counter Data Reg */

#define T16P_CH0_VOL_CTL       (*(union T16P_VOL_CTL_tag *)(T16P_BASE+0x06)).usCTL
#define T16P_CH0_VOLSEL        (*(union T16P_VOL_CTL_tag *)(T16P_BASE+0x06)).bCTL.VOLSEL
#define T16P_CH0_VOLBPS        (*(union T16P_VOL_CTL_tag *)(T16P_BASE+0x06)).bCTL.VOLBPS

#define T16P_CH0_CTL           (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).usCTL
#define T16P_CH0_PRESET        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.PRESET
#define T16P_CH0_CLKSEL        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.CLKSEL
#define T16P_CH0_INITOL        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.INITOL
#define T16P_CH0_SELFM         (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.SELFM
#define T16P_CH0_CHSYNC        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.CHSYNC
#define T16P_CH0_SPLTMD        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.SPLTMD
#define T16P_CH0_SGNSEL        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.SGNSEL
#define T16P_CH0_RESSEL        (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.RESSEL
#define T16P_CH0_BCNT          (*(union T16P_CTL_tag *)(T16P_BASE+0x08)).bCTL.BCNT

#define T16P_CH0_PRUN          (*(volatile unsigned char *)(T16P_BASE+0x0a))       /* T16P run/stop control */
#define T16P_CH0_CLKDIV        (*(volatile unsigned char *)(T16P_BASE+0x0c))       /* T16P internal clock control */

#define T16P_CH0_INT           (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).usCTL
#define T16P_CH0_INTAEN        (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).bCTL.INTAEN
#define T16P_CH0_INTBEN        (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).bCTL.INTBEN
#define T16P_CH0_INTBEEN       (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).bCTL.INTBEEN
#define T16P_CH0_INTAF         (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).bCTL.INTAF
#define T16P_CH0_INTBF         (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).bCTL.INTBF
#define T16P_CH0_BUFEF         (*(union T16P_INT_tag *)(T16P_BASE+0x0e)).bCTL.BUFEF

/* Channel 1 */
#define T16P_CH1_A             (*(volatile unsigned short *)(T16P_BASE+0x10))      /* T16P Compare A Buffer */
#define T16P_CH1_B             (*(volatile unsigned short *)(T16P_BASE+0x12))      /* T16P Compare B Buffer */
#define T16P_CH1_CNT_DATA      (*(volatile unsigned short *)(T16P_BASE+0x14))      /* T16P Counter Data Reg */

#define T16P_CH1_VOL_CTL       (*(union T16P_VOL_CTL_tag *)(T16P_BASE+0x16)).usCTL
#define T16P_CH1_VOLSEL        (*(union T16P_VOL_CTL_tag *)(T16P_BASE+0x16)).bCTL.VOLSEL
#define T16P_CH1_VOLBPS        (*(union T16P_VOL_CTL_tag *)(T16P_BASE+0x16)).bCTL.VOLBPS

#define T16P_CH1_CTL           (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).usCTL
#define T16P_CH1_PRESET        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.PRESET
#define T16P_CH1_CLKSEL        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.CLKSEL
#define T16P_CH1_INITOL        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.INITOL
#define T16P_CH1_SELFM         (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.SELFM
#define T16P_CH1_CHSYNC        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.CHSYNC
#define T16P_CH1_SPLTMD        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.SPLTMD
#define T16P_CH1_SGNSEL        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.SGNSEL
#define T16P_CH1_RESSEL        (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.RESSEL
#define T16P_CH1_BCNT          (*(union T16P_CTL_tag *)(T16P_BASE+0x18)).bCTL.BCNT

#define T16P_CH1_PRUN          (*(volatile unsigned char *)(T16P_BASE+0x1a))       /* T16P run/stop control */
#define T16P_CH1_CLKDIV        (*(volatile unsigned char *)(T16P_BASE+0x1c))       /* T16P internal clock control */

#define T16P_CH1_INT           (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).usCTL
#define T16P_CH1_INTAEN        (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).bCTL.INTAEN
#define T16P_CH1_INTBEN        (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).bCTL.INTBEN
#define T16P_CH1_INTBEEN       (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).bCTL.INTBEEN
#define T16P_CH1_INTAF         (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).bCTL.INTAF
#define T16P_CH1_INTBF         (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).bCTL.INTBF
#define T16P_CH1_BUFEF         (*(union T16P_INT_tag *)(T16P_BASE+0x1e)).bCTL.BUFEF

#ifdef __cplusplus
}
#endif
#endif                          /* _T16P_REG_H */
