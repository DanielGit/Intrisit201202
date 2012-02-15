/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : rtc_reg.h
 * 	Description : S1C33L27 RTC register definition file
 *
 * 	Revision History:
 *  2010/01/29   Joan Lin   New creation
 ****************************************************************************/
#ifndef _RTC_REG_H
#define _RTC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Structures (typedef, struct, union)
 ****************************************************************************/
/********************************************************************/
/*   [ RTC Interrupt Status reg ] RTC_INTSTAT              0x00     */
/********************************************************************/
union RTC_INTSTAT_tag {
    volatile struct {
        unsigned int RTCIRQ:1;  // interrupt status
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Interrupt Mode reg ] RTC_INTMODE                 0x01    */
/********************************************************************/
union RTC_INTMODE_tag {
    volatile struct {
        unsigned int RTCIEN:1;  // RTC interrupt enable
        unsigned int RTCIMD:1;  // RTC interrupt mode select
        unsigned int RTCT:2;    // RTC interrupt cycle setup
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Control 0 reg ] RTC_CNTL0                        0x02    */
/********************************************************************/
union RTC_CNTL0_tag {
    volatile struct {
        unsigned int RTCRST:1;  // software reset
        unsigned int RTCSTP:1;  // Counter run/stop control
        unsigned int RTCADJ:1;  // 30-second adjustment
        unsigned int DUMMY:1;
        unsigned int RTC24H:1;  // 24H/12H mode select

    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Control 1 reg ] RTC_CNTL1                       0x03     */
/********************************************************************/
union RTC_CNTL1_tag {
    volatile struct {
        unsigned int RTCHLD:1;   // Counter hold control
        unsigned int RTCBSY:1;   // Counter busy flag
        unsigned int RTCRDHLD:1; // Counter read busy flag
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Second reg ] RTC_SEC                            0x04     */
/********************************************************************/
union RTC_SEC_tag {
    volatile struct {
        unsigned int RTCSL:4;   // RTC 1-second counter
        unsigned int RTCSH:3;   // RTC 10-second counter
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Minute reg ] RTC_MIN                            0x05     */
/********************************************************************/
union RTC_MIN_tag {
    volatile struct {
        unsigned int RTCMIL:4;  // RTC 1-minute counter
        unsigned int RTCMIH:3;  // RTC 10-minute counter
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Hour reg ] RTC_HOUR                             0x06     */
/********************************************************************/
union RTC_HOUR_tag {
    volatile struct {
        unsigned int RTCHL:4;    // RTC hour counter
        unsigned int RTCHH:2;    // RTC hour counter
        unsigned int RTCAP:1;    // AM/PM indicator
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Day reg ] RTC_DAY                               0x07     */
/********************************************************************/
union RTC_DAY_tag {
    volatile struct {
        unsigned int RTCDL:4;  // RTC 1-day counter
        unsigned int RTCDH:2;  // RTC 10-day counter
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Month reg ] RTC_MONTH                           0x08     */
/********************************************************************/
union RTC_MONTH_tag {
    volatile struct {
        unsigned int RTCMOL:4;  // RTC 1-month counter
        unsigned int RTCMOH:1;  // RTC 10-month counter
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Year reg ] RTC_YEAR                             0x09     */
/********************************************************************/
union RTC_YEAR_tag {
    volatile struct {
        unsigned int RTCYL:4;   // RTC 1-year counter
        unsigned int RTCYH:4;   // RTC 10-year counter
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Days of Week reg ] RTC_WEEK                     0x0a     */
/********************************************************************/
union RTC_WEEK_tag {
    volatile struct {
        unsigned int RTCWK:3;   // RTC days of week counter
    } bCTL;
    volatile unsigned char ucCTL;
};

/********************************************************************/
/*   [ RTC Wakeup Configuration reg ] RTC_WAKEUP           0x0f     */
/********************************************************************/
union RTC_WAKEUP_tag {
    volatile struct {
        unsigned int WUP_POL:1; // WAKEUP polarity selection
        unsigned int WUP_CTL:1; // WAKEUP control
    } bCTL;
    volatile unsigned char ucCTL;
};

//------------------------------
// Macro Define
#define RTC_BASE    0x00300a00

#define RTC_INTSTAT                   (*(union RTC_INTSTAT_tag *)(RTC_BASE+0x0)).ucCTL
#define RTC_RTCIRQ                    (*(union RTC_INTSTAT_tag *)(RTC_BASE+0x0)).bCTL.RTCIRQ

#define RTC_INTMODE                   (*(union RTC_INTMODE_tag *)(RTC_BASE+0x1)).ucCTL
#define RTC_RTCIEN                    (*(union RTC_INTMODE_tag *)(RTC_BASE+0x1)).bCTL.RTCIEN
#define RTC_RTCIMD                    (*(union RTC_INTMODE_tag *)(RTC_BASE+0x1)).bCTL.RTCIMD
#define RTC_RTCT                      (*(union RTC_INTMODE_tag *)(RTC_BASE+0x1)).bCTL.RTCT

#define RTC_CNTL0                     (*(union RTC_CNTL0_tag *)(RTC_BASE+0x2)).ucCTL
#define RTC_RTCRST                    (*(union RTC_CNTL0_tag *)(RTC_BASE+0x2)).bCTL.RTCRST
#define RTC_RTCSTP                    (*(union RTC_CNTL0_tag *)(RTC_BASE+0x2)).bCTL.RTCSTP
#define RTC_RTCADJ                    (*(union RTC_CNTL0_tag *)(RTC_BASE+0x2)).bCTL.RTCADJ
#define RTC_RTC24H                    (*(union RTC_CNTL0_tag *)(RTC_BASE+0x2)).bCTL.RTC24H

#define RTC_CNTL1                     (*(union RTC_CNTL1_tag *)(RTC_BASE+0x3)).ucCTL
#define RTC_RTCHLD                    (*(union RTC_CNTL1_tag *)(RTC_BASE+0x3)).bCTL.RTCHLD
#define RTC_RTCBSY                    (*(union RTC_CNTL1_tag *)(RTC_BASE+0x3)).bCTL.RTCBSY
#define RTC_RTCRDHLD                  (*(union RTC_CNTL1_tag *)(RTC_BASE+0x3)).bCTL.RTCRDHLD

#define RTC_SEC                       (*(union RTC_SEC_tag *)(RTC_BASE+0x4)).ucCTL
#define RTC_RTCSL                     (*(union RTC_SEC_tag *)(RTC_BASE+0x4)).bCTL.RTCSL
#define RTC_RTCSH                     (*(union RTC_SEC_tag *)(RTC_BASE+0x4)).bCTL.RTCSH

#define RTC_MIN                       (*(union RTC_MIN_tag *)(RTC_BASE+0x5)).ucCTL
#define RTC_RTCMIL                    (*(union RTC_MIN_tag *)(RTC_BASE+0x5)).bCTL.RTCSL
#define RTC_RTCMIH                    (*(union RTC_MIN_tag *)(RTC_BASE+0x5)).bCTL.RTCSH

#define RTC_HOUR                      (*(union RTC_HOUR_tag *)(RTC_BASE+0x6)).ucCTL
#define RTC_RTCHL                     (*(union RTC_HOUR_tag *)(RTC_BASE+0x6)).bCTL.RTCHL
#define RTC_RTCHH                     (*(union RTC_HOUR_tag *)(RTC_BASE+0x6)).bCTL.RTCHH
#define RTC_RTCAP                     (*(union RTC_HOUR_tag *)(RTC_BASE+0x6)).bCTL.RTCAP

#define RTC_DAY                       (*(union RTC_DAY_tag *)(RTC_BASE+0x7)).ucCTL
#define RTC_RTCDL                     (*(union RTC_DAY_tag *)(RTC_BASE+0x7)).bCTL.RTCDL
#define RTC_RTCDH                     (*(union RTC_DAY_tag *)(RTC_BASE+0x7)).bCTL.RTCDH

#define RTC_MONTH                     (*(union RTC_MONTH_tag *)(RTC_BASE+0x8)).ucCTL
#define RTC_RTCMOL                    (*(union RTC_MONTH_tag *)(RTC_BASE+0x8)).bCTL.RTCMOL
#define RTC_RTCMOH                    (*(union RTC_MONTH_tag *)(RTC_BASE+0x8)).bCTL.RTCMOH

#define RTC_YEAR                      (*(union RTC_YEAR_tag *)(RTC_BASE+0x9)).ucCTL
#define RTC_RTCYL                     (*(union RTC_YEAR_tag *)(RTC_BASE+0x9)).bCTL.RTCYL
#define RTC_RTCYH                     (*(union RTC_YEAR_tag *)(RTC_BASE+0x9)).bCTL.RTCYH

#define RTC_WEEK                      (*(union RTC_WEEK_tag *)(RTC_BASE+0xa)).ucCTL
#define RTC_RTCWK                     (*(union RTC_WEEK_tag *)(RTC_BASE+0xa)).bCTL.RTCWK

#define RTC_WAKEUP                    (*(union RTC_WAKEUP_tag *)(RTC_BASE+0xf)).ucCTL
#define RTC_WUP_POL                   (*(union RTC_WAKEUP_tag *)(RTC_BASE+0xf)).bCTL.WUP_POL
#define RTC_WUP_CTL                   (*(union RTC_WAKEUP_tag *)(RTC_BASE+0xf)).bCTL.WUP_CTL

#ifdef __cplusplus
}
#endif

#endif                          //_RTC_REG_H
