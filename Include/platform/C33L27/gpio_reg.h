/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : gpio_reg.h
 * 	Description : S1C33L27 GPIO register definition file
 *
 * 	Revision History:
 *  2010.01.26  Joan Lin       New creation
 *  2010.01.29  Jackie Zhu     Modify according to  L27 manual
 ****************************************************************************/
#ifndef _GPIO_REG_H
#define _GPIO_REG_H


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures & unions & enums (#typedef)
 ****************************************************************************/

/* SCTP */
//enum {
//    SCTP_NCHAT = 0x0,
//    SCTP_PCLK_DIV_1 = 0x1,
//    SCTP_PCLK_DIV_2 = 0x2,
//    SCTP_PCLK_DIV_4 = 0x3,
//    SCTP_PCLK_DIV_8 = 0x4,
//    SCTP_PCLK_DIV_16 = 0x5,
//    SCTP_PCLK_DIV_32 = 0x6,
//    SCTP_PCLK_DIV_64 = 0x7
//};
//
///* SPTRG */
//enum {
//    SPTRG_0 = 0x0,
//    SPTRG_1 = 0x1,
//    SPTRG_2 = 0x2,
//    SPTRG_3 = 0x3,
//    SPTRG_4 = 0x4,
//    SPTRG_5 = 0x5,
//    SPTRG_6 = 0x6,
//    SPTRG_7 = 0x7,
//    SPTRG_8 = 0x8,
//    SPTRG_9 = 0x9,
//    SPTRG_A = 0xa,
//    SPTRG_B = 0xb,
//    SPTRG_C = 0xc,
//    SPTRG_D = 0xd,
//    SPTRG_E = 0xe,
//    SPTRG_F = 0xf
//};

/********************************************************************/
/* [ Port Data ] Px_PD                              0x00030_0300    */
/********************************************************************/
union Px_PD_tag {
    volatile struct {
        unsigned int Px0D:1;    // Px0 data
        unsigned int Px1D:1;    // Px1 data
        unsigned int Px2D:1;    // Px2 data
        unsigned int Px3D:1;    // Px3 data
        unsigned int Px4D:1;    // Px4 data
        unsigned int Px5D:1;    // Px5 data
        unsigned int Px6D:1;    // Px6 data
        unsigned int Px7D:1;    // Px7 data
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ Port I/O Control ] Px_IOC                       0x0030_0301    */
/********************************************************************/
union Px_IOC_tag {
    volatile struct {
        unsigned int IOCx0:1;   // Px0 IO control
        unsigned int IOCx1:1;   // Px1 IO control
        unsigned int IOCx2:1;   // Px2 IO control
        unsigned int IOCx3:1;   // Px3 IO control
        unsigned int IOCx4:1;   // Px4 IO control
        unsigned int IOCx5:1;   // Px5 IO control
        unsigned int IOCx6:1;   // Px6 IO control
        unsigned int IOCx7:1;   // Px7 IO control
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ Bus signal low driver control ] BUS_LDRV           0x0030_0320 */
/********************************************************************/
union BUS_LDRV_tag {
    volatile struct {
        unsigned int LDRVAD:1;  // A0~20 low drive
        unsigned int LDRVDB:1;  // D0~ 7 low drive
        unsigned int DUMMY:6;
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ Px pull up control ] Px_PUP                        0x0030_0321 */
/********************************************************************/
union Px_PUP_tag {
    volatile struct {
        unsigned int PUPx0:1;   // Px pull up control
        unsigned int PUPx1:1;   // Px pull up control
        unsigned int PUPx2:1;   // Px pull up control
        unsigned int PUPx3:1;   // Px pull up control
        unsigned int PUPx4:1;   // Px pull up control
        unsigned int PUPx5:1;   // Px pull up control
        unsigned int PUPx6:1;   // Px pull up control
        unsigned int PUPx7:1;   // Px pull up control
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPT Interrupt Input Port Select ] PINTSEL_FPT      0x0030_0330 */
/********************************************************************/
union PINTSEL_FPT_tag {
    volatile struct {
        unsigned int SPT0:3;    // interrupt input port selection
        unsigned int DUMMY:1;   //
        unsigned int SPT1:3;    // interrupt input port selection

        //unsigned int SPT2:2;        // ~
        //unsigned int SPT3:2;        // ~
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPT Interrupt Polarity Select ] PINTPRI_FPT        0x0030_0334 */
/********************************************************************/
union PINTPRI_FPT_tag {
    volatile struct {
        unsigned int SPPT0:1;   // interrupt input polarity selection
        unsigned int SPPT1:1;   // ~
        unsigned int SPPT2:1;   // ~
        unsigned int SPPT3:1;   // ~
        unsigned int DUMMY:4;   //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPT Interrupt Edge/Level Select ] PINTEVL_FPT      0x0030_0338 */
/********************************************************************/
union PINTEVL_FPT_tag {
    volatile struct {
        unsigned int SEPT0:1;   // interrupt input edge/level selection
        unsigned int SEPT1:1;   // ~
        unsigned int SEPT2:1;   // ~
        unsigned int SEPT3:1;   // ~
        unsigned int DUMMY:4;   //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPT Interrupt Mask Register ] PINTMSK_FPT          0x0030_033c */
/********************************************************************/
union PINTMSK_FPT_tag {
    volatile struct {
        unsigned int SIET0:1;   // interrupt input mask selection
        unsigned int SIET1:1;   // ~
        unsigned int SIET2:1;   // ~
        unsigned int SIET3:1;   // ~
        unsigned int DUMMY:4;   //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPT Interrupt Flag Register ] PINTFLG_FPT          0x0030_0340 */
/********************************************************************/
union PINTFLG_FPT_tag {
    volatile struct {
        unsigned int SFGP0:1;   // interrupt input flag selection
        unsigned int SFGP1:1;   // ~
        unsigned int SFGP2:1;   // ~
        unsigned int SFGP3:1;   // ~
        unsigned int DUMMY:4;   //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPT Chattering Control Register ] PSCTP            0x0030_0344 */
/********************************************************************/
union PSCTP_tag {
    volatile struct {
        unsigned int SCTP0:3;   // FPT chattering filter time selection
        unsigned int DUMMY1:1;  //
        unsigned int SCTP1:3;   // FPT chattering filter time selection
        unsigned int DUMMY2:4;  //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPK Interrupt Select Register ] PSPK                0x0030_0360 */
/********************************************************************/
union PSPK_tag {
    volatile struct {
        unsigned int SPK000:1;   // FPK interrupt select
        unsigned int SPK001:1;   // FPK interrupt select
        unsigned int SPK002:1;   // FPK interrupt select
        unsigned int SPK003:1;   // FPK interrupt select
        unsigned int SPK004:1;   // FPK interrupt select
        unsigned int SPK005:1;   // FPK interrupt select
        unsigned int SPK006:1;   // FPK interrupt select
        unsigned int SPK007:1;   // FPK interrupt select
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPK Input Polarity Control Register ] PFPK_POL     0x0030_0370 */
/********************************************************************/
union PFPK_POL_tag {
    volatile struct {
        unsigned int SPKT0:1;   // FPK input polarity selection
        unsigned int SPKT1:1;   // FPK input polarity selection
        unsigned int DUMMY1:6;  //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPK Interrupt Mode Select Register ] PSEKT         0x0030_0371 */
/********************************************************************/
union PSEKT_tag {
    volatile struct {
        unsigned int SEKT0:1;   // FPK interrupt mode select
        unsigned int SEKT1:1;   // FPK interrupt mode select
        unsigned int DUMMY1:6;  //
    } bCTL;
    volatile unsigned char ucCTL;
};



/********************************************************************/
/* [ FPK Interrupt Enable Register ] PSIEKT             0x0030_0372 */
/********************************************************************/
union PSIEKT_tag {
    volatile struct {
        unsigned int SIEKT0:1;   // FPK interrupt enable
        unsigned int SIEKT1:1;   // FPK interrupt enable
        unsigned int DUMMY1:6;   //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPK Interrupt Flag Register ] PSFGK             0x0030_0373 */
/********************************************************************/
union PSFGK_tag {
    volatile struct {
        unsigned int SFGK0:1;   // FPK interrupt flag
        unsigned int SFGK1:1;   // FPK interrupt flag
        unsigned int DUMMY1:6;  //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ FPK Interrupt Chattering Filter Register ] PSCTK   0x0030_0373 */
/********************************************************************/
union PSCTK_tag {
    volatile struct {
        unsigned int SCTK0:3;   // FPK interrupt Chattering Time
        unsigned int DUMMY1:1;  //
        unsigned int SCTK1:3;   // FPK interrupt Chattering Time
        unsigned int DUMMY2:1;  //
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/* [ Port Function Select ] Px_CFP                   0x0030_0800    */
/********************************************************************/
union Px_CFP_tag {
    volatile struct {
        unsigned int CFPx0:2;   // Px0 function select
        unsigned int CFPx1:2;   // Px1 function select
        unsigned int CFPx2:2;   // Px2 function select
        unsigned int CFPx3:2;   // Px3 function select
        unsigned int CFPx4:2;   // Px4 function select
        unsigned int CFPx5:2;   // Px5 function select
        unsigned int CFPx6:2;   // Px6 function select
        unsigned int CFPx7:2;   // Px7 function select
    } bCTL;
    volatile unsigned short usCTL;
};

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/

#define GPIO_BASE          (0x00300300)

#define GPIO_Px_PD(n)           (*(union Px_PD_tag *)(GPIO_BASE+2*(n))).ucCTL
#define GPIO_PD(n, m)           (*(union Px_PD_tag *)(GPIO_BASE+2*(n))).bCTL.Px##m##D

#define GPIO_Px_IOC(n)          (*(union Px_IOC_tag *)(GPIO_BASE+1+2*(n))).ucCTL
#define GPIO_IOC(n, m)          (*(union Px_IOC_tag *)(GPIO_BASE+1+2*(n))).bCTL.IOCx##m

//Port Interrupt Select
#define GPIO_SPT_0              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x40)).bCTL.SPT0
#define GPIO_SPT_1              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x40)).bCTL.SPT1
#define GPIO_SPT_2              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x41)).bCTL.SPT0
#define GPIO_SPT_3              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x41)).bCTL.SPT1
#define GPIO_SPT_4              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x42)).bCTL.SPT0
#define GPIO_SPT_5              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x42)).bCTL.SPT1
#define GPIO_SPT_6              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x43)).bCTL.SPT0
#define GPIO_SPT_7              (*(union PINTSEL_FPT_tag *)(GPIO_BASE+0x43)).bCTL.SPT1

//Port Input Polarity
#define GPIO_SPPT_0             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x44)).bCTL.SPPT0
#define GPIO_SPPT_1             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x44)).bCTL.SPPT1
#define GPIO_SPPT_2             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x44)).bCTL.SPPT2
#define GPIO_SPPT_3             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x44)).bCTL.SPPT3
#define GPIO_SPPT_4             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x45)).bCTL.SPPT0
#define GPIO_SPPT_5             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x45)).bCTL.SPPT1
#define GPIO_SPPT_6             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x45)).bCTL.SPPT2
#define GPIO_SPPT_7             (*(union PINTPRI_FPT_tag *)(GPIO_BASE+0x45)).bCTL.SPPT3

#define GPIO_SPPT(n)            GPIO_SPPT_##n

//Port Interrupt Edge or Level
#define GPIO_SEPT_0             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x46)).bCTL.SEPT0
#define GPIO_SEPT_1             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x46)).bCTL.SEPT1
#define GPIO_SEPT_2             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x46)).bCTL.SEPT2
#define GPIO_SEPT_3             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x46)).bCTL.SEPT3
#define GPIO_SEPT_4             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x47)).bCTL.SEPT0
#define GPIO_SEPT_5             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x47)).bCTL.SEPT1
#define GPIO_SEPT_6             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x47)).bCTL.SEPT2
#define GPIO_SEPT_7             (*(union PINTEVL_FPT_tag *)(GPIO_BASE+0x47)).bCTL.SEPT3

#define GPIO_SEPT(n)            GPIO_SEPT_##n

//Port Interrupt Enable
#define GPIO_SIET_0             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x48)).bCTL.SIET0
#define GPIO_SIET_1             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x48)).bCTL.SIET1
#define GPIO_SIET_2             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x48)).bCTL.SIET2
#define GPIO_SIET_3             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x48)).bCTL.SIET3
#define GPIO_SIET_4             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x49)).bCTL.SIET0
#define GPIO_SIET_5             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x49)).bCTL.SIET1
#define GPIO_SIET_6             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x49)).bCTL.SIET2
#define GPIO_SIET_7             (*(union PINTMSK_FPT_tag *)(GPIO_BASE+0x49)).bCTL.SIET3

#define GPIO_SIET(n)            GPIO_SIET_##n
#define GPIO_SIET_0123          (*(volatile unsigned char *)(GPIO_BASE+0x48))
#define GPIO_SIET_4567          (*(volatile unsigned char *)(GPIO_BASE+0x49))

//Port Interrupt Flag
#define GPIO_SFGP_0             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4a)).bCTL.SFGP0
#define GPIO_SFGP_1             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4a)).bCTL.SFGP1
#define GPIO_SFGP_2             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4a)).bCTL.SFGP2
#define GPIO_SFGP_3             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4a)).bCTL.SFGP3
#define GPIO_SFGP_4             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4b)).bCTL.SFGP0
#define GPIO_SFGP_5             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4b)).bCTL.SFGP1
#define GPIO_SFGP_6             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4b)).bCTL.SFGP2
#define GPIO_SFGP_7             (*(union PINTFLG_FPT_tag *)(GPIO_BASE+0x4b)).bCTL.SFGP3

#define GPIO_SFGP(n)            GPIO_SFGP_##n
#define GPIO_SFGP_0123          (*(volatile unsigned char *)(GPIO_BASE+0x4a))
#define GPIO_SFGP_4567          (*(volatile unsigned char *)(GPIO_BASE+0x4b))

//Port Chattering
#define GPIO_SCTP_0             (*(union PSCTP_tag *)(GPIO_BASE+0x4c)).bCTL.SCTP0
#define GPIO_SCTP_1             (*(union PSCTP_tag *)(GPIO_BASE+0x4c)).bCTL.SCTP1
#define GPIO_SCTP_2             (*(union PSCTP_tag *)(GPIO_BASE+0x4d)).bCTL.SCTP0
#define GPIO_SCTP_3             (*(union PSCTP_tag *)(GPIO_BASE+0x4d)).bCTL.SCTP1
#define GPIO_SCTP_4             (*(union PSCTP_tag *)(GPIO_BASE+0x4e)).bCTL.SCTP0
#define GPIO_SCTP_5             (*(union PSCTP_tag *)(GPIO_BASE+0x4e)).bCTL.SCTP1
#define GPIO_SCTP_6             (*(union PSCTP_tag *)(GPIO_BASE+0x4f)).bCTL.SCTP0
#define GPIO_SCTP_7             (*(union PSCTP_tag *)(GPIO_BASE+0x4f)).bCTL.SCTP1

#define GPIO_SCTP(n)            GPIO_SCTP_##n

#define GPIO_SDMAT              (*(volatile unsigned char *)(GPIO_BASE+0x50))        /* DMA trigger source selection */

#define GPIO_SPK000             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK000
#define GPIO_SPK001             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK001
#define GPIO_SPK002             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK002
#define GPIO_SPK003             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK003
#define GPIO_SPK004             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK004
#define GPIO_SPK005             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK005
#define GPIO_SPK006             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK006
#define GPIO_SPK007             (*(union PSPK_tag *)(GPIO_BASE+0x60)).bCTL.SPK007

#define GPIO_SPK008             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK000
#define GPIO_SPK009             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK001
#define GPIO_SPK010             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK002
#define GPIO_SPK011             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK003
#define GPIO_SPK012             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK004
#define GPIO_SPK013             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK005
#define GPIO_SPK014             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK006
#define GPIO_SPK015             (*(union PSPK_tag *)(GPIO_BASE+0x61)).bCTL.SPK007

#define GPIO_SPK100             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK000
#define GPIO_SPK101             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK001
#define GPIO_SPK102             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK002
#define GPIO_SPK103             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK003
#define GPIO_SPK104             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK004
#define GPIO_SPK105             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK005
#define GPIO_SPK106             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK006
#define GPIO_SPK107             (*(union PSPK_tag *)(GPIO_BASE+0x62)).bCTL.SPK007

#define GPIO_SPK108             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK000
#define GPIO_SPK109             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK001
#define GPIO_SPK110             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK002
#define GPIO_SPK111             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK003
#define GPIO_SPK112             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK004
#define GPIO_SPK113             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK005
#define GPIO_SPK114             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK006
#define GPIO_SPK115             (*(union PSPK_tag *)(GPIO_BASE+0x63)).bCTL.SPK007

#define GPIO_SPKT0              (*(union PFPK_POL_tag *)(GPIO_BASE+0x70)).bCTL.SPKT0
#define GPIO_SPKT1              (*(union PFPK_POL_tag *)(GPIO_BASE+0x70)).bCTL.SPKT1

#define GPIO_SEKT0              (*(union PSEKT_tag *)(GPIO_BASE+0x71)).bCTL.SEKT0
#define GPIO_SEKT1              (*(union PSEKT_tag *)(GPIO_BASE+0x71)).bCTL.SEKT1

#define GPIO_SIEKT0             (*(union PSIEKT_tag *)(GPIO_BASE+0x72)).bCTL.SIEKT0
#define GPIO_SIEKT1             (*(union PSIEKT_tag *)(GPIO_BASE+0x72)).bCTL.SIEKT1

#define GPIO_SFGK0              (*(union PSFGK_tag *)(GPIO_BASE+0x73)).bCTL.SFGK0
#define GPIO_SFGK1              (*(union PSFGK_tag *)(GPIO_BASE+0x73)).bCTL.SFGK1
#define GPIO_PSFGK              (*(volatile unsigned char *)(GPIO_BASE+0x73))

#define GPIO_SCTK0              (*(union PSCTK_tag *)(GPIO_BASE+0x74)).bCTL.SCTK0
#define GPIO_SCTK1              (*(union PSCTK_tag *)(GPIO_BASE+0x74)).bCTL.SCTK1

#define PMUX_BASE               (0x00300800)

#define PMUX_Px_CFP(n)          (*(union Px_CFP_tag *)(PMUX_BASE+2*(n))).usCTL
#define PMUX_CFP(n, m)          (*(union Px_CFP_tag *)(PMUX_BASE+2*(n))).bCTL.CFPx##m

#define GPIO_Px_PUP(n)          (*(union Px_PUP_tag *)(PMUX_BASE+0x20+n)).ucCTL
#define GPIO_PUP(n, m)          (*(union Px_PUP_tag *)(PMUX_BASE+0x20+n)).bCTL.PUPx##m

#define GPIO_BUS_DRV            (*(union BUS_LDRV_tag *)(PMUX_BASE+0x30)).usCTL
#define GPIO_LDRVAD             (*(union BUS_LDRV_tag *)(PMUX_BASE+0x30)).bCTL.LDRVAD
#define GPIO_LDRVDB             (*(union BUS_LDRV_tag *)(PMUX_BASE+0x30)).bCTL.LDRVDB

#define GPIO_ANFEN              (*(volatile unsigned char *)(PMUX_BASE+0x3e))        /* Port Noise Filter Control Reg.   */
#define GPIO_PROTECT            (*(volatile unsigned char *)(PMUX_BASE+0x3f))        /* CFP, BUS_LDRV Write Protect Reg. */

#ifdef __cplusplus
}
#endif
#endif                          // _GPIO_REG_H
