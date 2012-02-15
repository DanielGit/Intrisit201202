/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : t16a_reg.h
 * 	Description : S1C33L27 T16A register definition file
 *
 * 	Revision History:
 *  2010/01/29   Joan Lin     New creation
 ****************************************************************************/

#ifndef _T16A_REG_H
#define _T16A_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/

/********************************************************************/
/* [ T16A counter control register ]  T16A_CTL              0x00    */
/********************************************************************/
    union T16A_CTL_tag {
        volatile struct {
            unsigned int PRUN:1;        /* 16-bit Timer Run/Stop Control Bit */
            unsigned int PRESET:1;      /* 16-bit Timer Reset Bit            */
            unsigned int TMMD:1;        /* 16-bit up-Counter Mode            */
            unsigned int CBUFEN:1;      /* Comparison Data Buffer Enable     */
            unsigned int T16SEL:2;      /* Using T16A chx counter (0,1,2,3)  */
            unsigned int DUMMY1:1;      /*                                   */
            unsigned int BUSY:1;        /* Busy signal                       */

            unsigned int CLKS:4;        /* Counter Clock Selector            */
            unsigned int DMASEL:2;      /* DMA select (0,1,2,3)              */
            unsigned int DUMMY2:2;      /*                                   */
        } bCTL;
        volatile unsigned short usCTL;
    };

/**********************************************************************/
/* [ T16A timer compare/capture A/B control register ]T16A_CCCTL 0x04 */
/**********************************************************************/
    union T16A_CCCTL_tag {
        volatile struct {
            unsigned int CCAMD:1;       /* Comparison / Capture Mode Select */
            unsigned int TOUTAINV:1;    /* Comparison A Output Inverted     */
            unsigned int DUMMY1:2;      /*                                  */
            unsigned int TOUTAMD:2;     /* Comparison A Output Mode         */
            unsigned int CAPATRG:2;     /* Capture A Trigger Mode Selection */

            unsigned int CCBMD:1;       /* Comparison / Capture Mode Select */
            unsigned int TOUTBINV:1;    /* Comparison B Output Inverted     */
            unsigned int DUMMY2:2;      /*                                  */
            unsigned int TOUTBMD:2;     /* Comparison B Output Mode         */
            unsigned int CAPBTRG:2;     /* Capture B Trigger Mode Selection */
        } bCTL;
        volatile unsigned short usCTL;
    };

/**********************************************************************/
/* [ T16A interrupt enable control register ] T16A_IEN           0x0a */
/**********************************************************************/
    union T16A_IEN_tag {
        volatile struct {
            unsigned int CAIE:1;        /* Comparison A Interrupt Enable        */
            unsigned int CBIE:1;        /* Comparison B Interrupt Enable        */
            unsigned int CAPAIE:1;      /* Capture A Interrupt Enable           */
            unsigned int CAPBIE:1;      /* Capture B Interrupt Enable           */
            unsigned int CAPAOWIE:1;    /* Capture A Overwrite Interrupt Enable */
            unsigned int CAPBOWIE:1;    /* Capture B Overwrite Interrupt Enable */
            unsigned int DUMMY:10;      /*                                      */
        } bCTL;
        volatile unsigned short usCTL;
    };

/**********************************************************************/
/* [ T16A interrupt flag control register ] T16A_IFLG            0x0c */
/**********************************************************************/
    union T16A_IFLG_tag {
        volatile struct {
            unsigned int CAIF:1;        /* Comparison A Interrupt Flag        */
            unsigned int CBIF:1;        /* Comparison B Interrupt Flag        */
            unsigned int CAPAIF:1;      /* Capture A Interrupt Flag           */
            unsigned int CAPBIF:1;      /* Capture B Interrupt Flag           */
            unsigned int CAPAOWIF:1;    /* Capture A Overwrite Interrupt Flag */
            unsigned int CAPBOWIF:1;    /* Capture B Overwrite Interrupt Flag */
            unsigned int DUMMY:10;      /*                                    */
        } bCTL;
        volatile unsigned short usCTL;
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/
#define T16A_BASE          (0x00301160) // 4 channel T16A

#define T16A_CTL(n)        (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).usCTL
#define T16A_PRUN(n)       (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.PRUN
#define T16A_PRESET(n)     (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.PRESET
#define T16A_TMMD(n)       (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.TMMD
#define T16A_CBUFEN(n)     (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.CBUFEN
#define T16A_T16SEL(n)     (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.T16SEL
#define T16A_BUSY(n)       (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.BUSY
#define T16A_CLKS(n)       (*(union T16A_CTL_tag *)(T16A_BASE+0x10*(n))).bCTL.CLKS
#define T16A_DMASEL_23     (*(union T16A_CTL_tag *)(T16A_BASE)).bCTL.DMASEL
#define T16A_DMASEL_45     (*(union T16A_CTL_tag *)(T16A_BASE+0x10)).bCTL.DMASEL

#define T16A_TC(n)         (*(volatile const unsigned short *)(T16A_BASE+0x02+0x10*(n)))        /* Counter Data Reg */

#define T16A_CCCTL(n)      (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).usCTL
#define T16A_CCAMD(n)      (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.CCAMD
#define T16A_TOUTAINV(n)   (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.TOUTAINV
#define T16A_TOUTAMD(n)    (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.TOUTAMD
#define T16A_CAPATRG(n)    (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.CAPATRG
#define T16A_CCBMD(n)      (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.CCBMD
#define T16A_TOUTBINV(n)   (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.TOUTBINV
#define T16A_TOUTBMD(n)    (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.TOUTBMD
#define T16A_CAPBTRG(n)    (*(union T16A_CCCTL_tag *)(T16A_BASE+0x04+0x10*(n))).bCTL.CAPBTRG

#define T16A_CCRA(n)       (*(volatile unsigned short *)(T16A_BASE+0x06+0x10*(n)))      /* Cmp/Cap A Reg */
#define T16A_CCRB(n)       (*(volatile unsigned short *)(T16A_BASE+0x08+0x10*(n)))      /* Cmp/Cap B Reg */

#define T16A_IEN(n)        (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).usCTL
#define T16A_CAIE(n)       (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).bCTL.CAIE
#define T16A_CBIE(n)       (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).bCTL.CBIE
#define T16A_CAPAIE(n)     (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).bCTL.CAPAIE
#define T16A_CAPBIE(n)     (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).bCTL.CAPBIE
#define T16A_CAPAOWIE(n)   (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).bCTL.CAPAOWIE
#define T16A_CAPBOWIE(n)   (*(union T16A_IEN_tag *)(T16A_BASE+0x0a+0x10*(n))).bCTL.CAPBOWIE

#define T16A_IFLG(n)       (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).usCTL
#define T16A_CAIF(n)       (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).bCTL.CAIF
#define T16A_CBIF(n)       (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).bCTL.CBIF
#define T16A_CAPAIF(n)     (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).bCTL.CAPAIF
#define T16A_CAPBIF(n)     (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).bCTL.CAPBIF
#define T16A_CAPAOWIF(n)   (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).bCTL.CAPAOWIF
#define T16A_CAPBOWIF(n)   (*(union T16A_IFLG_tag *)(T16A_BASE+0x0c+0x10*(n))).bCTL.CAPBOWIF


#ifdef __cplusplus
}
#endif
#endif                          /* _T16A_REG_H */
