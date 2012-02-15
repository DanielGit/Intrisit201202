/****************************************************************************
 * 	Copyright (C) SEIKO EPSON CORP. 2010
 *
 * 	File Name   : itc_reg.h
 * 	Description : S1C33L27 ITC register definition file
 *
 * 	Revision History:
 *  2010/01/26   Joan Lin   New creation
 *
 ****************************************************************************/

#ifndef _ITC_REG_H
#define _ITC_REG_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * 	Constants (#define, enum, const)
 ****************************************************************************/

/* ITC_LV */
    enum {
        ITC_LV_0 = 0,
        ITC_LV_1 = 1,
        ITC_LV_2 = 2,
        ITC_LV_3 = 3,
        ITC_LV_4 = 4,
        ITC_LV_5 = 5,
        ITC_LV_6 = 6,
        ITC_LV_7 = 7
    };

/****************************************************************************
 * 	Macros (#define)
 ****************************************************************************/

#define ITC_BASE          (0x00300200)

#define ITC_LV(n)         (*(volatile unsigned char *)(ITC_BASE + n ))

#define ITC_PINT0_LV      ITC_LV(0x10)        // Interrupt Level Reg: Port input group 0
#define ITC_PINT1_LV      ITC_LV(0x11)        // Interrupt Level Reg: Port input group 1
#define ITC_KINT0_LV      ITC_LV(0x12)        // Interrupt Level Reg: Key input group 0
#define ITC_KINT1_LV      ITC_LV(0x13)        // Interrupt Level Reg: Key input group 1
#define ITC_DMA02_LV      ITC_LV(0x14)        // Interrupt Level Reg: Simple DMA Ch.0,2
#define ITC_DMA13_LV      ITC_LV(0x15)        // Interrupt Level Reg: Simple DMA Ch.1,3
#define ITC_DMA46_LV      ITC_LV(0x16)        // Interrupt Level Reg: Simple DMA Ch.4,6
#define ITC_DMA57_LV      ITC_LV(0x17)        // Interrupt Level Reg: Simple DMA Ch.5,7
#define ITC_T16P0_LV      ITC_LV(0x18)        // Interrupt Level Reg: T16P Ch.0
#define ITC_T16P1_LV      ITC_LV(0x19)        // Interrupt Level Reg: T16P Ch.1
#define ITC_T16A_LV(m)    ITC_LV(0x1a + m)    // Interrupt Level Reg: T16 Ch.0~3
#define ITC_T8F024_LV     ITC_LV(0x1e)        // Interrupt Level Reg: T8F Ch.024
#define ITC_T8F135_LV     ITC_LV(0x1f)        // Interrupt Level Reg: T8F Ch.135
#define ITC_ADC10_LV      ITC_LV(0x20)        // Interrupt Level Reg: ADC
#define ITC_RTC_LV        ITC_LV(0x21)        // Interrupt Level Reg: RTC
#define ITC_USI0_LV       ITC_LV(0x22)        // Interrupt Level Reg: USI Ch.0/UART
#define ITC_USI1_LV       ITC_LV(0x23)        // Interrupt Level Reg: USI Ch.1
#define ITC_USI2_LV       ITC_LV(0x24)        // Interrupt Level Reg: USI Ch.2
#define ITC_USIL_LV       ITC_LV(0x25)        // Interrupt Level Reg: USIL
#define ITC_LCDC_LV       ITC_LV(0x26)        // Interrupt Level Reg: LCDC
#define ITC_SDMMC_LV      ITC_LV(0x27)        // Interrupt Level Reg: SDMMC
#define ITC_REMC_LV       ITC_LV(0x28)        // Interrupt Level Reg: REMC
#define ITC_I2SOUT_LV     ITC_LV(0x29)        // Interrupt Level Reg: I2S Output
#define ITC_I2SIN_LV      ITC_LV(0x2a)        // Interrupt Level Reg: I2S Input
#define ITC_HIF_LV        ITC_LV(0x2b)        // Interrupt Level Reg: HIF
#define ITC_USB_LV        ITC_LV(0x2c)        // Interrupt Level Reg: USB

#ifdef __cplusplus
}
#endif
#endif    /* _ITC_REG_H */
