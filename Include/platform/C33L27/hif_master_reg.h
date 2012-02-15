/****************************************************************************
 *  Copyright (C) SEIKO EPSON CORP. 2010
 *
 *  File Name   : hif_master_reg.h
 *  Description : S1C33L27 HIF Master register definition file
 *
 *  Revision History:
 *  2010/02/01   Jackie Zhu   New creation
 ****************************************************************************/

#ifndef _HIF_MASTER_REG_H
#define _HIF_MASTER_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************/
/* [ HIF Indirect interface Control register ] HIF_IDIF_CTRL    0x4   */
/**********************************************************************/
    union HIF_IDIF_CTRL_8_tag {
        volatile struct {
            unsigned int RW_SEL:1;      // Read/Write cycle select
            unsigned int ACCESS_MD:2;   // Access mode slect
            unsigned int BUSY:1;        // BUSY
            unsigned int WR_BF:1;       // write bufferfull flag
        } bCTL;
        volatile unsigned char ucCTL;
    };

    union HIF_IDIF_CTRL_16_tag {
        volatile struct {
            unsigned int RW_SEL:1;      // Read/Write cycle select
            unsigned int ACCESS_MD:2;   // Access mode slect
            unsigned int BUSY:1;        // BUSY
            unsigned int WR_BF:1;       // write bufferfull flag
        } bCTL;
        volatile unsigned short usCTL;
    };


/**********************************************************************/
/* [ HIF Control register ]                         HIF_CTRL    0x6   */
/**********************************************************************/
    union HIF_CTRL_8_tag {
        volatile struct {
            unsigned int AHB_EN:1;      // AHBinterface enable
            unsigned int ENDIAN:1;      // Endian
            unsigned int ADDR_MD:2;     // Address control mode
            unsigned int INT_SEL:1;     // interrupt source select
            unsigned int DUMMY:2;
            unsigned int HIFSEL:1;      // HIF data width select
        } bCTL;
        volatile unsigned char ucCTL;
    };

    union HIF_CTRL_16_tag {
        volatile struct {
            unsigned int AHB_EN:1;      // AHBinterface enable
            unsigned int ENDIAN:1;      // Endian
            unsigned int ADDR_MD:2;     // Address control mode
            unsigned int INT_SEL:1;     // interrupt source select
            unsigned int DUMMY:2;
            unsigned int HIFSEL:1;      // HIF data width select
        } bCTL;
        volatile unsigned short usCTL;
    };



/**********************************************************************/
/* [ HIF Special Register index register ]          HIF_SP_IDX  0xc   */
/**********************************************************************/
    union HIF_SP_IDX_8_tag {
        volatile struct {
            unsigned int INDEX:4;       // special register index
        } bCTL;
        volatile unsigned char ucCTL;
    };

    union HIF_SP_IDX_16_tag {
        volatile struct {
            unsigned int INDEX:4;       // special register index
        } bCTL;
        volatile unsigned short usCTL;
    };


/**********************************************************************/
/* [ HIF Extend Control register1 ]            HIF_EXTCTRL1 0xd[0x1]  */
/**********************************************************************/
    union HIF_EXTCTRL1_tag {
        volatile struct {
            unsigned int DUMMY:1;
            unsigned int HINT_MD:1;     // Host interrupt mode select
        } bCTL;
        volatile unsigned char ucCTL;
    };


/**********************************************************************/
/* [ HIF Semsphore registers ]                 HIF_SMPH  0xd[0x8-0xf] */
/**********************************************************************/
    union HIF_SMPH_tag {
        volatile struct {
            unsigned int SMPH:1;        // semaphore
        } bCTL;
        volatile unsigned char ucCTL;
    };

/**********************************************************************/
/* [ HIF Flag Register0 ]   HIF_FLAG0                             0xe */
/**********************************************************************/
    union HIF_FLAG0_8_tag {
        volatile struct {
            unsigned int SINT:1;        // slave interrupt flag
            unsigned int SFLAG0:1;      // slave flag0
            unsigned int SFLAG1:1;      // slave flag1
            unsigned int SFLAG2:1;      // slave flag2
            unsigned int SFLAG3:1;      // slave flag3
            unsigned int DUMMY:2;
            unsigned int PROTECT:1;     // HIF_FLAG write protect (only Host mode)
        } bCTL;
        volatile unsigned char ucCTL;
    };

    union HIF_FLAG0_16_tag {
        volatile struct {
            unsigned int SINT:1;        // slave interrupt flag
            unsigned int SFLAG0:1;      // slave flag0
            unsigned int SFLAG1:1;      // slave flag1
            unsigned int SFLAG2:1;      // slave flag2
            unsigned int SFLAG3:1;      // slave flag3
            unsigned int DUMMY:2;
            unsigned int PROTECT:1;     // HIF_FLAG write protect (only Host mode)
        } bCTL;
        volatile unsigned short usCTL;
    };

/**********************************************************************/
/* [ HIF Flag Register1 ]   HIF_FLAG1                             0xf */
/**********************************************************************/
    union HIF_FLAG1_8_tag {
        volatile struct {
            unsigned int HINT:1;        // host interrupt flag
            unsigned int HFLAG0:1;      // host flag0
            unsigned int HFLAG1:1;      // host flag1
            unsigned int HFLAG2:1;      // host flag2
            unsigned int HFLAG3:1;      // host flag3
        } bCTL;
        volatile unsigned char ucCTL;
    };

    union HIF_FLAG1_16_tag {
        volatile struct {
            unsigned int HINT:1;        // host interrupt flag
            unsigned int HFLAG0:1;      // host flag0
            unsigned int HFLAG1:1;      // host flag1
            unsigned int HFLAG2:1;      // host flag2
            unsigned int HFLAG3:1;      // host flag3
        } bCTL;
        volatile unsigned short usCTL;
    };

/****************************************************************************
 *  Macros (#define)
 ****************************************************************************/
#define HIF_MASTER_BASE_CE4        (0x3000000)
#define HIF_MASTER_BASE_CE5        (0x4000000)
#define HIF_MASTER_BASE_CE7        (0x10000000)
#define HIF_MASTER_BASE_CE10       (0x20000000)
#define HIF_MASTER_BASE_CE8        (0x40000000)
#define HIF_MASTER_BASE_CE9        (0x80000000)

#define HIF_MASTER_BASE             HIF_MASTER_BASE_CE9
#define HIF_BUS_8
//#define HIF_BUS_16

#ifdef  HIF_BUS_8
    #define HIF_MASTER_DRIF_B       ((unsigned char *)(HIF_MASTER_BASE+0x0))
    #define HIF_MASTER_DRIF_HW      ((unsigned char *)(HIF_MASTER_BASE+0x1))
    #define HIF_MASTER_DRIF_W       ((unsigned char *)(HIF_MASTER_BASE+0x2))
    #define HIF_MASTER_IDIF_DAT     ((unsigned char *)(HIF_MASTER_BASE+0x3))

    #define HIF_MASTER_IDIF_CTRL    (*(union HIF_IDIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x4)).ucCTL
    #define HIF_MASTER_RW_SEL       (*(union HIF_IDIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x4)).bCTL.RW_SEL
    #define HIF_MASTER_ACCESS_MD    (*(union HIF_IDIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x4)).bCTL.ACCESS_MD
    #define HIF_MASTER_BUSY         (*(union HIF_IDIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x4)).bCTL.BUSY
    #define HIF_MASTER_WR_BF        (*(union HIF_IDIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x4)).bCTL.WR_BF

    #define HIF_MASTER_CTRL         (*(union HIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x6)).ucCTL
    #define HIF_MASTER_AHB_EN       (*(union HIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x6)).bCTL.AHB_EN
    #define HIF_MASTER_ENDIAN       (*(union HIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x6)).bCTL.ENDIAN
    #define HIF_MASTER_ADDR_MD      (*(union HIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x6)).bCTL.ADDR_MD
    #define HIF_MASTER_INT_SEL      (*(union HIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x6)).bCTL.INT_SEL
    #define HIF_MASTER_HIFSEL       (*(union HIF_CTRL_8_tag *)(HIF_MASTER_BASE+0x6)).bCTL.HIFSEL

    #define HIF_MASTER_ADDR0        (*(unsigned char *)(HIF_MASTER_BASE+0x8))
    #define HIF_MASTER_ADDR1        (*(unsigned char *)(HIF_MASTER_BASE+0x9))
    #define HIF_MASTER_ADDR2        (*(unsigned char *)(HIF_MASTER_BASE+0xa))
    #define HIF_MASTER_ADDR3        (*(unsigned char *)(HIF_MASTER_BASE+0xb))

    #define HIF_MASTER_SP_IDX       (*(union HIF_SP_IDX_8_tag *)(HIF_MASTER_BASE+0xc)).bCTL.INDEX
    #define HIF_MASTER_SP_DAT       (*(unsigned char *)(HIF_MASTER_BASE+0xd))

    #define HIF_MASTER_HINT_MD      (*(union HIF_EXTCTRL1_tag *)(HIF_MASTER_BASE+0xd)).bCTL.HINT_MD
    #define HIF_MASTER_SMPH         (*(union HIF_SMPH_tag *)(HIF_MASTER_BASE+0xd)).bcTL.SMPH
    #define HIF_MASTER_SCRATCH      (*(unsigned char *)(HIF_MASTER_BASE+0xd))

    #define HIF_MASTER_FLAG0        (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).ucCTL
    #define HIF_MASTER_SINT         (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).bCTL.SINT
    #define HIF_MASTER_SFLAG0       (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).bCTL.SFLAG0
    #define HIF_MASTER_SFLAG1       (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).bCTL.SFLAG1
    #define HIF_MASTER_SFLAG2       (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).bCTL.SFLAG2
    #define HIF_MASTER_SFLAG3       (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).bCTL.SFLAG3
    #define HIF_MASTER_PROTECT      (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).bCTL.PROTECT
    #define HIF_MASTER_FLAG_PROT    (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).ucCTL

    #define HIF_MASTER_FLAG1        (*(union HIF_FLAG0_8_tag *)(HIF_MASTER_BASE+0xe)).ucCTL
    #define HIF_MASTER_HINT         (*(union HIF_FLAG1_8_tag *)(HIF_MASTER_BASE+0xf)).bCTL.HINT
    #define HIF_MASTER_HFLAG0       (*(union HIF_FLAG1_8_tag *)(HIF_MASTER_BASE+0xf)).bCTL.HFLAG0
    #define HIF_MASTER_HFLAG1       (*(union HIF_FLAG1_8_tag *)(HIF_MASTER_BASE+0xf)).bCTL.HFLAG1
    #define HIF_MASTER_HFLAG2       (*(union HIF_FLAG1_8_tag *)(HIF_MASTER_BASE+0xf)).bCTL.HFLAG2
    #define HIF_MASTER_HFLAG3       (*(union HIF_FLAG1_8_tag *)(HIF_MASTER_BASE+0xf)).bCTL.HFLAG3
#endif

#ifdef  HIF_BUS_16
    #define HIF_MASTER_DRIF_B      ((unsigned short *)(HIF_MASTER_BASE+0x0*2))
    #define HIF_MASTER_DRIF_HW     ((unsigned short *)(HIF_MASTER_BASE+0x1*2))
    #define HIF_MASTER_DRIF_W      ((unsigned short *)(HIF_MASTER_BASE+0x2*2))
    #define HIF_MASTER_IDIF_DAT    ((unsigned short *)(HIF_MASTER_BASE+0x3*2))

    #define HIF_MASTER_IDIF_CTRL   (*(union HIF_IDIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x4*2)).usCTL
    #define HIF_MASTER_RW_SEL      (*(union HIF_IDIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x4*2)).bCTL.RW_SEL
    #define HIF_MASTER_ACCESS_MD   (*(union HIF_IDIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x4*2)).bCTL.ACCESS_MD
    #define HIF_MASTER_BUSY        (*(union HIF_IDIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x4*2)).bCTL.BUSY
    #define HIF_MASTER_WR_BF       (*(union HIF_IDIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x4*2)).bCTL.WR_BF

    #define HIF_MASTER_CTRL        (*(union HIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x6*2)).usCTL
    #define HIF_MASTER_AHB_EN      (*(union HIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x6*2)).bCTL.AHB_EN
    #define HIF_MASTER_ENDIAN      (*(union HIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x6*2)).bCTL.ENDIAN
    #define HIF_MASTER_ADDR_MD     (*(union HIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x6*2)).bCTL.ADDR_MD
    #define HIF_MASTER_INT_SEL     (*(union HIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x6*2)).bCTL.INT_SEL
    #define HIF_MASTER_HIFSEL      (*(union HIF_CTRL_16_tag *)(HIF_MASTER_BASE+0x6*2)).bCTL.HIFSEL

    #define HIF_MASTER_ADDR0       (*(unsigned short *)(HIF_MASTER_BASE+0x8*2))
    #define HIF_MASTER_ADDR1       (*(unsigned short *)(HIF_MASTER_BASE+0x9*2))

    #define HIF_MASTER_SP_IDX      (*(union HIF_SP_IDX_16_tag *)(HIF_MASTER_BASE+0xc*2)).bCTL.INDEX
    #define HIF_MASTER_SP_DAT      (*(unsigned char *)(HIF_MASTER_BASE+0xd*2))

    #define HIF_MASTER_HINT_MD     (*(union HIF_EXTCTRL1_tag *)(HIF_MASTER_BASE+0xd*2)).bCTL.HINT_MD
    #define HIF_MASTER_SMPH        (*(union HIF_SMPH_tag *)(HIF_MASTER_BASE+0xd*2)).bcTL.SMPH
    #define HIF_MASTER_SCRATCH     (*(unsigned char *)(HIF_MASTER_BASE+0xd*2))

    #define HIF_MASTER_FLAG0       (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).usCTL
    #define HIF_MASTER_SINT        (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).bCTL.SINT
    #define HIF_MASTER_SFLAG0      (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).bCTL.SFLAG0
    #define HIF_MASTER_SFLAG1      (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).bCTL.SFLAG1
    #define HIF_MASTER_SFLAG2      (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).bCTL.SFLAG2
    #define HIF_MASTER_SFLAG3      (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).bCTL.SFLAG3
    #define HIF_MASTER_PROTECT     (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).bCTL.PROTECT
    #define HIF_MASTER_FLAG_PROT   (*(union HIF_FLAG0_16_tag *)(HIF_MASTER_BASE+0xe*2)).ucCTL

    #define HIF_MASTER_FLAG1       (*(union HIF_FLAG1_16_tag *)(HIF_MASTER_BASE+0xf*2)).usCTL
    #define HIF_MASTER_HINT        (*(union HIF_FLAG1_16_tag *)(HIF_MASTER_BASE+0xf*2)).bCTL.HINT
    #define HIF_MASTER_HFLAG0      (*(union HIF_FLAG1_16_tag *)(HIF_MASTER_BASE+0xf*2)).bCTL.HFLAG0
    #define HIF_MASTER_HFLAG1      (*(union HIF_FLAG1_16_tag *)(HIF_MASTER_BASE+0xf*2)).bCTL.HFLAG1
    #define HIF_MASTER_HFLAG2      (*(union HIF_FLAG1_16_tag *)(HIF_MASTER_BASE+0xf*2)).bCTL.HFLAG2
    #define HIF_MASTER_HFLAG3      (*(union HIF_FLAG1_16_tag *)(HIF_MASTER_BASE+0xf*2)).bCTL.HFLAG3
#endif

#ifdef __cplusplus
}
#endif
#endif                          /* _HIF_MASTER_REG_H */

