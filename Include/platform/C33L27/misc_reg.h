/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : misc_reg.h
 * 	Description : S1C33L27 misc register definition file
 *
 * 	Revision History:
 *  2010/01/12  Joan Lin       New creation
 ****************************************************************************/


#ifndef _MISC_REG_H
#define _MISC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/

/********************************************************************/
/* [ MISC RTC WAIT reg  ] MISC_RTC_WAIT                REG[0x10]    */
/********************************************************************/
    union MISC_RTC_WAIT_tag {
        volatile struct {
            unsigned int RTCWT:3;       //RTC wait cycle
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ MISC USB Control reg  ] MISC_USB_CONTRL           REG[0x12]    */
/********************************************************************/
    union MISC_USB_CONTRL_tag {
        volatile struct {
            unsigned int USBWT:3;       //USB wait cycle
            unsigned int DUMMY:2;       //
            unsigned int USBSNZ:1;      //USB snooze enable
            unsigned int USBIEN:1;      //USB interrupt enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ MISC HDRAM/COROM WAIT reg  ] MISC_HDRAM_COROM_WAIT  REG[0x14]  */
/********************************************************************/
    union MISC_HDRAM_COROM_WAIT_tag {
        volatile struct {
            unsigned int A0WT:1;        //A0 area wait cycle
            unsigned int A3WT:1;        //A3 area wait cycle
            unsigned int DUMMY:1;       //
            unsigned int CALRWT:1;      //COROM wait cycle
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ MISC BOOT CONTROL reg  ] MISC_BOOT_CONTRL           REG[0x16]  */
/********************************************************************/
    union MISC_BOOT_CONTRL_tag {
        volatile struct {
            unsigned int DUMMY0:1;      //
            unsigned int BOOTEN:1;      //Boot enable
            unsigned int DUMMY1:2;      //
            unsigned int BOOT:3;        //Boot mode select
        } bCTL;
        volatile unsigned char ucCTL;
    };


/********************************************************************/
/* [ MISC TRACE ENABLE reg  ]    MISC_TRACE_EN           REG[0x18]  */
/********************************************************************/
    union MISC_TRACE_EN_tag {
        volatile struct {
            unsigned int TRACE:1;       //TRACE enable
        } bCTL;
        volatile unsigned char ucCTL;
    };

/********************************************************************/
/* [ MISC WRITE PROTECT0 reg  ] MISC_PROT0               REG[0x20]  */
/********************************************************************/
    union MISC_PROT0_tag {
        volatile struct {
            unsigned int PROT0:8;       //Protect register 0
        } bCTL;
    };

/********************************************************************/
/* [ MISC WRITE PROTECT1 reg  ] MISC_PROT1               REG[0x22]  */
/********************************************************************/
    union MISC_PROT1_tag {
        volatile struct {
            unsigned int PROT1:8;       //Protect register 0
        } bCTL;
    };

/********************************************************************/
/* [ MISC RAM CONFIG reg  ] MISC_RAM_CONFIG              REG[0x24]  */
/********************************************************************/
    union MISC_RAM_CONFIG_tag {
        volatile struct {
            unsigned int RAMCFG:3;      //RAM switch configure
        } bCTL;
        volatile unsigned char ucCTL;
    };

//------------------------------
#define MISC_BASE    0x300010

#define MISC_RTC_WAIT            (*(union MISC_RTC_WAIT_tag *)(MISC_BASE)).ucCTL
#define MISC_RTCWT               (*(union MISC_RTC_WAIT_tag *)(MISC_BASE)).bCTL.RTCWT

#define MISC_USB                 (*(union MISC_USB_CONTRL_tag *)(MISC_BASE+0x02)).ucCTL
#define MISC_USBWT               (*(union MISC_USB_CONTRL_tag *)(MISC_BASE+0x02)).bCTL.USBWT
#define MISC_USBSNZ              (*(union MISC_USB_CONTRL_tag *)(MISC_BASE+0x02)).bCTL.USBSNZ
#define MISC_USBIEN              (*(union MISC_USB_CONTRL_tag *)(MISC_BASE+0x02)).bCTL.USBIEN

#define MISC_RAMWT               (*(union MISC_HDRAM_COROM_WAIT_tag *)(MISC_BASE+0x04)).ucCTL
#define MISC_A0WT                (*(union MISC_HDRAM_COROM_WAIT_tag *)(MISC_BASE+0x04)).bCTL.A0WT
#define MISC_A3WT                (*(union MISC_HDRAM_COROM_WAIT_tag *)(MISC_BASE+0x04)).bCTL.A3WT
#define MISC_CALRWT              (*(union MISC_HDRAM_COROM_WAIT_tag *)(MISC_BASE+0x04)).bCTL.CALRWT

#define MISC_BOOT_CONTRL         (*(union MISC_BOOT_CONTRL_tag *)(MISC_BASE+0x06)).ucCTL
#define MISC_BOOTEN              (*(union MISC_BOOT_CONTRL_tag *)(MISC_BASE+0x06)).bCTL.BOOTEN
#define MISC_BOOT                (*(union MISC_BOOT_CONTRL_tag *)(MISC_BASE+0x06)).bCTL.BOOT

#define MISC_TRACE_EN            (*(union MISC_TRACE_EN_tag *)(MISC_BASE+0x08)).ucCTL
#define MISC_TRACE               (*(union MISC_TRACE_EN_tag *)(MISC_BASE+0x08)).bCTL.TRACE

#define MISC_PROT0               *(volatile unsigned char *)(MISC_BASE+0x10)
#define MISC_PROT1               *(volatile unsigned char *)(MISC_BASE+0x12)

#define MISC_RAM_CONFIG          (*(union MISC_RAM_CONFIG_tag *)(MISC_BASE+0x14)).ucCTL
#define MISC_RAMCFG              (*(union MISC_RAM_CONFIG_tag *)(MISC_BASE+0x14)).bCTL.RAMCFG

#ifdef __cplusplus
}
#endif
#endif                          /* _MISC_REG_H */
