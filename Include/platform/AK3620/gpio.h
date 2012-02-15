/**
 * @file gpio.h
 * @brief gpio function header file
 * This file provides gpio APIs: initialization, set gpio, get gpio,
 * gpio interrupt handler.
 * Copyright (C) 2004 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author Miaobaoli
 * @date 2005-07-24
 * @version 1.0
 * @ref AK3220 technical manual.
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "anyka_types.h"

#define GPIO_NUMBER             32
#define LEVEL_HIGH              1
#define LEVEL_LOW               0
#define GPIO_DIR_INPUT          0
#define GPIO_DIR_OUTPUT         1

#if (BOARD_MODE == 0)

#define GPIO_USB_BOOT           4
#define GPIO_I2C_SCLK           0
#define GPIO_I2C_SDA            1
#ifdef USBDC_DET_USE_GPIO
#define GPIO_HP_DET             32
#define GPIO_USB_IN             6
#define GPIO_SD_DET             30
#else
#define GPIO_HP_DET             30
#define GPIO_USB_IN             32
#define GPIO_SD_DET             6
#endif
#define GPIO_DC_CHG             32
#define GPIO_POWER_KEY          2
#define GPIO_POWER_OFF          3
#define GPIO_LCD_LIGHT          4
#define GPIO_STATUS_LED         32
#define GPIO_FLASH_CTL0         32
#define GPIO_FLASH_CTL1         32
#define GPIO_SD_WP              32
#define GPIO_DC_IN              32

#define GPIO_HP_MUTE            32
#define GPIO_FM_RESET           32

#elif ((BOARD_MODE == 1) || (BOARD_MODE == 2))

#define GPIO_USB_BOOT           4
#define GPIO_I2C_SCLK           0
#define GPIO_I2C_SDA            1
#define GPIO_HP_DET             6
#define GPIO_USB_IN             32
#define GPIO_SD_DET             3
#define GPIO_DC_CHG             32
#define GPIO_POWER_KEY          1
#define GPIO_POWER_OFF          5
#define GPIO_LCD_LIGHT          4
#define GPIO_STATUS_LED         0
#define GPIO_FLASH_CTL0         32
#define GPIO_FLASH_CTL1         32
#define GPIO_SD_WP              32
#define GPIO_DC_IN              32
#define GPIO_KEYPAD_INTR        2

#define GPIO_HP_MUTE            32
#define GPIO_FM_RESET           9

#elif (BOARD_MODE == 3)

#define GPIO_USB_BOOT           4
#define GPIO_I2C_SCLK           0
#define GPIO_I2C_SDA            1
#define GPIO_HP_DET             6
#ifdef USBDC_DET_USE_GPIO
#define GPIO_USB_IN             30
#else
#define GPIO_USB_IN             32
#endif
#define GPIO_SD_DET             9
#define GPIO_DC_CHG             32
#define GPIO_POWER_KEY          32
#define GPIO_POWER_OFF          2
#define GPIO_CAM_RST            32
#define GPIO_CAM_nPWRDN         32
#define GPIO_LCD_LIGHT          4
#define GPIO_STATUS_LED         0
#define GPIO_FLASH_CTL0         32
#define GPIO_FLASH_CTL1         32
#define GPIO_SD_WP              32
#define GPIO_DC_IN              32
#define GPIO_KEYPAD_INTR        10

#define GPIO_HP_MUTE            32
#define GPIO_FM_RESET           32

#elif (BOARD_MODE == 7)

#define GPIO_USB_BOOT           4
#define GPIO_I2C_SCLK           0
#define GPIO_I2C_SDA            1
#define GPIO_HP_DET             6
#ifdef USBDC_DET_USE_GPIO
#define GPIO_USB_IN             10
#else
#define GPIO_USB_IN             32
#endif
#define GPIO_SD_DET             15
#define GPIO_DC_CHG             32
#define GPIO_POWER_KEY          32
#define GPIO_POWER_OFF          2
#define GPIO_CAM_RST            32
#define GPIO_CAM_nPWRDN         9
#define GPIO_LCD_LIGHT          4
#define GPIO_STATUS_LED         32
#define GPIO_FLASH_CTL0         32
#define GPIO_FLASH_CTL1         32
#define GPIO_SD_WP              32
#define GPIO_DC_IN              32
#define GPIO_KEYPAD_INTR        32

#define GPIO_HP_MUTE            32
#define GPIO_FM_RESET           32

#elif (BOARD_MODE == 8)

#define GPIO_USB_BOOT           4
#define GPIO_I2C_SCLK           0
#define GPIO_I2C_SDA            1
#define GPIO_HP_DET             6
#ifdef USBDC_DET_USE_GPIO
#define GPIO_USB_IN             2
#else
#define GPIO_USB_IN             32
#endif
#define GPIO_SD_DET             3
#define GPIO_DC_CHG             32
#define GPIO_POWER_KEY          32
#define GPIO_POWER_OFF          5
#define GPIO_CAM_RST            32
#define GPIO_CAM_nPWRDN         32
#define GPIO_LCD_LIGHT          4
#define GPIO_STATUS_LED         32
#define GPIO_FLASH_CTL0         32
#define GPIO_FLASH_CTL1         32
#define GPIO_SD_WP              32
#define GPIO_DC_IN              32
#define GPIO_KEYPAD_INTR        32

#define GPIO_HP_MUTE            32
#define GPIO_FM_RESET           32

#endif

typedef enum
{
    ALL_IS_GPIO = 0,
    LCD_MPU_DATA_BUS,
    LCD_RGB_DATA_BUS,
    NANDFLASH_DATA_BUS,
    MMC_DATA_BUS,
    CAMERA_DATA_BUS,
    SPI_DATA_BUS,
    I2S_DATA_BUS,
    JTAG_DATA_BUS,
    UTD_URD,
    GPIO_9_TO_10,
    NANDFLASH_CE1,
    GPIO_30,
    GPIO_31,
    GPIO_25_TO_29,
    PWM_ENABLE,
    GPIO_4,
    WATCH_DOG,
    GPIO_5,
    GPIO_6,
    GPIO_11_TO_13,
    PCLK_OUT,
    GPIO_14,
    DRVVBUS,
    GPIO_15,
    GPIO_16_TO_24,
    GPIO_7_TO_8,
    GPIO_3,
    MPU_TO_GPIO,
    CAMERA_TO_GPIO
}E_GPIO_PIN_CONFIG;

typedef T_VOID (*T_fGPIO_CALLBACK)( T_U32 pin, T_U8 polarity );

/**
 * @brief: set gpio pin group
 *
 * for gpio share-pin 's different setting
 * @author YiRuoxiang
 * @date 2006-09-20
 * @param [in] PinCfg different setting of share-pin
 * @return T_VOID
 * @retval
 */
T_VOID gpio_pin_group_cfg(E_GPIO_PIN_CONFIG PinCfg);

/**
 * @brief: Init gpio.
 * @author Miaobaoli
 * @date 2004-09-22
 * @return T_VOID
 * @retval
 */
T_VOID gpio_init(void);

/**
 * @brief: Set gpio output level
 * @author: Miaobaoli
 * @date 2004-09-22
 * @param: pin: gpio pin ID.
 * @param: level: 0 or 1.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_set_pin_level( T_U32 pin, T_U8 level );

/**
 * @brief: Get gpio input level
 * @author: Miaobaoli
 * @param: pin: gpio pin ID.
 * @date 2004-09-22
 * @return T_U8
 * @retval: pin level; 1: high; 0: low;
 */
T_U8 gpio_get_pin_level( T_U32 pin );

/**
 * @brief: Set gpio direction
 * @author: Miaobaoli
 * @date 2004-09-22
 * @param: pin: gpio pin ID.
 * @param: dir: 0 means input; 1 means output;
 * @return T_VOID
 * @retval
 */
T_VOID gpio_set_pin_dir( T_U32 pin, T_U8 dir );

/**
 * @brief: gpio interrupt control
 * @author: Miaobaoli
 * @date 2004-09-22
 * @param: pin: gpio pin ID.
 * @param: enable: 1 means enable interrupt. 0 means disable interrupt.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_int_control( T_U32 pin, T_U8 enable );

/**
 * @brief: set gpio interrupt polarity.
 * @author: Miaobaoli
 * @date 2004-09-22
 * @param: pin: gpio pin ID.
 * @param: polarity: 1 means active high interrupt. 0 means active low interrupt.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_set_int_p( T_U32 pin, T_U8 polarity );



T_U8 gpio_get_intr_p(T_U32 pin);



/**
 * @brief: set gpio pull up (connect to pull up resistance).
 * @author: lizhuobin
 * @date 2006-02-10
 * @param: pin: gpio pin ID.
 * @param: mode: 0 means do not connect the pull up resistance.
                 1 means connect the pull up resistance.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_set_pull_up_r( T_U32 pin, T_U8 mode );

/**
 * @brief: set gpio pull down resistance(connect to pull down resistance).
 * @author: Ri'an Zeng
 * @date 2007-03-06
 * @param: pin: gpio pin ID.
 * @param: mode: 0 means do not connect the pull down resistance.
                 1 means connect the pull down resistance.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_set_pull_down_r( T_U32 pin, T_U8 mode );

/**
 * @brief: wakeup gpio initialized
 * @author: YiRuoxiang
 * @date 2006-03-02
 * @param: T_VOID
 * @return T_VOID
 * @retval
 */
T_VOID gpio_wgpio_init(void);

/**
 * @brief: free wakeup gpio
 * @author: Junhua Zhao
 * @date 2006-03-25
 * @param: T_VOID
 * @return T_VOID
 * @retval
 */
T_VOID gpio_wgpio_free(void);

/**
 * @brief: wakeup gpio mask set, set which wgpio should be masked
 * @author: YiRuoxiang
 * @date 2006-03-02
 * @param: pin: gpio pin ID. the contrast list is shown below as (gpio,wgpio) format
 * @(56,15)(55,14)(54,13)(53,12)(52,11)(51,10)(50,9)(12,8)(11,7)(10,6)(9,5)(8,4)(7,3)(6,2)(5,1)(4,0)
 * @param: mask: 1 means unmask. 0 means mask.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_wgpio_mask( T_U32 pin, T_U8 mask );

T_VOID gpio_wgpio_pol(T_U32 pin, T_U8 pol);

/**
 * @brief: set gpio interrupt mode (edge or level).
 * @author: Miaobaoli
 * @date 2004-09-22
 * @param: pin: gpio pin ID.
 * @param: mode: 0 means level interrupt. 1 means edge interrupt.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_set_int_mode( T_U32 pin, T_U8 mode );

/**
 * @brief: Register one gpio interrupt callback function.
 * @author: Miaobaoli
 * @date 2004-09-22
 * @param: pin: gpio pin ID.
 * @param: polarity: 1 means active high interrupt. 0 means active low interrupt.
 * @param: callback: gpio interrupt callback function.
 * @return T_VOID
 * @retval
 */
T_VOID gpio_register_int_callback( T_U32 pin, T_U8 polarity, T_fGPIO_CALLBACK callback );

/**
 * @brief: Get chip type, AK36XX or AK36XXL.
 * @author: GuoChunlai
 * @date 2008-11-18
 * @param: T_VOID
 * @return AK_TRUE: is AK36XX, AK_FALSE: is AK36XXL
 * @retval
 */
T_BOOL get_chip_type(void);

#endif //#ifndef __GPIO_H__
