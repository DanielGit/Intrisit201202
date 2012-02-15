/** @file
 * @brief Define the register of ANYKA CPU
 *
 * Define the register address and bit map for system.
 * Copyright (C) 2006 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author
 * @date 2006-01-16
 * @version 1.0
 * @note CPU AK3223M
 */

#ifndef _ANYKA_CPU_H_
#define _ANYKA_CPU_H_




/** @defgroup ANYKA_CPU
 *  @ingroup M3PLATFORM
 */
/*@{*/


/** @{@name Base Address Define
 *  The base address of system memory space is define here.
 *  Include memory assignment and module base address define.
 */
 /**Memory assignment*/
#define ROM_BASE_ADDR               0x00000000  //on chip rom
#define NORFLASH_BASE_ADDR          0x10000000  //NOR FLASH start address
#define CHIP_CONF_BASE_ADDR         0xb0000000  // chip configurations
#define RAM_BASE_ADDR               0x30000000  // RAM start address
#define TRAM_BASE_ADDR              0x40000000  // on chip RAM start address
#define RAMLIKE_BASE_ADDR           0x50000000  // RAM-LIKE start address
#define NANDFLASH_BASE_ADDR         0x60000000  // NAND FLASH start address
#define USB_BASE_ADDR               0xC0000000  // USB

/**Module base address of chip config*/
#define LCD_MODULE_BASE_ADDR        0xB0010000  // LCD controller
#define IMAGE_MODULE_BASE_ADDR      0xB0030000  // image sensor
#define VIDEO_MODULE_BASE_ADDR      0xB0040000  // JPEG/H.263/MPEG4/MECodec
#define AUDIO_MODULE_BASE_ADDR      0xB0050000  // audio processor(DSP?)
#define UART_MODULE_BASE_ADDR       0xB0060000  // UART
#define MMC_MODULE_BASE_ADDR        0xB0060000  // MMC
#define SD_MODULE_BASE_ADDR         0xB0060000  // SD
#define SPI_MODULE_BASE_ADDR        0xB0060000  // SPI
#define HOST_MODULE_BASE_ADDR       0xB0080000  // host interface
#define NAND_MODULE_BASE_ADDR       0xB0080000  // NAND FLASH controller
#define ADC_MODULE_BASE_ADDR        0xB0090000  // Analog
#define GPIO_MODULE_BASE_ADDR       0xB0090000  // GPIO registers
#define TIMER_MODULE_BASE_ADDR      0xB0090000  // timer registers
#define FLASH_MODULE_BASE_ADDR      0xB00A0000  // NOR FLASH controller
#define RAM_MODULE_BASE_ADDR        0xB00B0000  // SDRAM/SSRAM/SRAM controller
#define DMA_MODULE_BASE_ADDR        0xB00C0000  // DMA registers
#define CRC_MODULE_BASE_ADDR        0xB00C0000  // CRC registers
#define RTC_MODULE_BASE_ADDR        0xB00D0000  // RTC
/** @} */


/** @{@name System Control Register
 *  Define system control register here, include CLOCK/INT/RESET
 */
#define CLOCK_CTRL_REG              (CHIP_CONF_BASE_ADDR + 0x00000000)  // module clock control(switch)
#define RESET_CTRL_REG              (CHIP_CONF_BASE_ADDR + 0x00000004)  // module software reset control register
#define INT_STATUS_REG              (CHIP_CONF_BASE_ADDR + 0x00000014)      // module interrupt status register
#define PLL_STANDBY_REG             (CHIP_CONF_BASE_ADDR + 0x00000034)  // module standby register

#define IRQINT_MASK_REG             (CHIP_CONF_BASE_ADDR + 0x00000018)  // module IRQ interrupt mask register, 1: mask; 0:unmask(default);
#define FRQINT_MASK_REG             (CHIP_CONF_BASE_ADDR + 0x0000001C)  // module FRQ interrupt mask register, 1: mask; 0:unmask(default);

#define CLOCK_DIV_REG               (CHIP_CONF_BASE_ADDR + 0x00000020)  // clock divider register 1

#define CLOCK2X_CTRL_REG            (CHIP_CONF_BASE_ADDR + 0x00000040)  // clock2x control register,1: 2*ASIC clock; 0: ASIC clock

#define BASE_XTAL_CLK               (12*1000*1000)
#define BASE_SYSTEM_CLK             (120*1000*1000)
/** @} */


/** @{@name System Control Register Bit map
 *  Define system control register bit map here,
 *  include CPU work mode, CLOCK control, INTERRUPT control
 */
/**CPU work mode */
#define ANYKA_CPU_Mode_USR      0x10
#define ANYKA_CPU_Mode_FIQ      0x11
#define ANYKA_CPU_Mode_IRQ      0x12
#define ANYKA_CPU_Mode_SVC      0x13
#define ANYKA_CPU_Mode_ABT      0x17
#define ANYKA_CPU_Mode_UNDEF    0x1B
#define ANYKA_CPU_Mode_SYS      0x1F
#define ANYKA_CPU_I_Bit         0x80
#define ANYKA_CPU_F_Bit         0x40

/** CLOCK control register bit map*/
#define CLOCK_CTRL_JPEG_MPEG            0x00000001
#define CLOCK_CTRL_IMG_CAPTURE          0x00000002
#define CLOCK_CTRL_SPI_CTL              0x00000004
#define CLOCK_CTRL_LCD                  0x00000008
#define CLOCK_CTRL_AUDIO                0x00000010
#define CLOCK_CTRL_USB                  0x00000020
#define CLOCK_CTRL_SD_MMC               0x00000040
#define CLOCK_CTRL_HOST                 0x00000080
#define CLOCK_CTRL_GPIO_TIMER           0x00000100
#define CLOCK_CTRL_RAM_ROM              0x00000200
#define CLOCK_CTRL_JANUS                0x00000800
#define CLOCK_CTRL_MOTION_ESTIMATION    0x00001000
#define CLOCK_CTRL_CRC                  0x00002000
#define CLOCK_CTRL_RTC                  0x00004000
#define CLOCK_CTRL_UART0                0x00008000
#define CLOCK_CTRL_UART1                0x00010000
#define CLOCK_CTRL_UART2                0x00020000
#define CLOCK_CTRL_USB_PLL              0x00040000
#define CLOCK_CTRL_EN_SET               0x00080000

/** interrupt status register bit map*/
#define INT_STATUS_LCD_BIT              (1 << 1)
#define INT_STATUS_GUI_BIT              (1 << 2)
#define INT_STATUS_CAMERA_BIT           (1 << 3)
#define INT_STATUS_VIDEO_BIT            (1 << 4)
#define INT_STATUS_AUDIO_BIT            (1 << 5)
#define INT_STATUS_UART_MMC_BIT         (1 << 6)
#define INT_STATUS_USBC_BIT             (1 << 7)
#define INT_STATUS_NANDFLASH_BIT        (1 << 8)
#define INT_STATUS_GPIO_TIMER_BIT       (1 << 9)
#define INT_STATUS_FLASH_BIT            (1 << 10)
#define INT_STATUS_RAM_BIT              (1 << 11)
#define INT_STATUS_MMC_BIT              (1 << 12)
#define INT_STATUS_UART1_BIT            (1 << 13)
#define INT_STATUS_UART0_BIT            (1 << 14)
#define INT_STATUS_SPI_BIT              (1 << 15)
#define INT_STATUS_TIMER2_BIT           (1 << 16)
#define INT_STATUS_TIMER1_BIT           (1 << 17)
#define INT_STATUS_GPIO1SET_BIT         (1 << 18)
#define INT_STATUS_CHARGER_BIT          (1 << 19)
#define INT_STATUS_JPEG_MPEG_BIT        (1 << 20)
#define INT_STATUS_TPERATUREDETECT_BIT  (1 << 21)
#define INT_STATUS_UART2_BIT            (1 << 22)
#define INT_STATUS_CRC_DMA_BIT          (1 << 23)
#define INT_STATUS_TIMER3_BIT           (1 << 24)
#define INT_STATUS_TIMER4_BIT           (1 << 25)
#define INT_STATUS_ADC1_BIT             (1 << 26)
#define INT_STATUS_RTC_BIT              (1 << 27)
#define INT_STATUS_USB_BIT              (1 << 28)
#define INT_STATUS_USBDMA_BIT           (1 << 29)

/** IRQ interrupt mask register bit map*/
#define IRQ_MASK_LCD_BIT                0x00000002
#define IRQ_MASK_CAMERA_BIT             0x00000008
#define IRQ_MASK_JPEG_MPEG_MOTIONESTIMATION_BIT     0x00000010
#define IRQ_MASK_AUDIO_BIT              0x00000020
#define IRQ_MASK_UART_MMC_BIT           0x00000040
#define IRQ_MASK_USB_BIT                0x00000080
#define IRQ_MASK_HOST_BIT               0x00000100
#define IRQ_MASK_GPIO_TIMER_BIT         0x00000200
#define IRQ_MASK_FLASH_BIT              0x00000400
#define IRQ_MASK_RAM_BIT                0x00000800
#define IRQ_MASK_CRC_DMA_BIT            0x00001000
#define IRQ_MASK_RTC_BIT                0x00002000
/** @} */


/** @{@name LCD module register and bit map define
 */
#define LCD_TOP_CONFIGURE_REG           (LCD_MODULE_BASE_ADDR | 0x0000)
#define LCD_MPU_1_REG                   (LCD_MODULE_BASE_ADDR | 0x0004)
#define LCD_MPU_DISPLAY_SIZE_REG        (LCD_MODULE_BASE_ADDR | 0x0008)
#define LCD_MPU_READ_REG                (LCD_MODULE_BASE_ADDR | 0x000C)
#define LCD_GRB_GENERAL_INFO_REG        (LCD_MODULE_BASE_ADDR | 0x0010)
#define LCD_GRB_CONTROL_REG             (LCD_MODULE_BASE_ADDR | 0x0014)
#define LCD_GRB_BACKGROUND_REG          (LCD_MODULE_BASE_ADDR | 0x0018)
#define LCD_GRB_INTERFACE1_REG          (LCD_MODULE_BASE_ADDR | 0x001C)
#define LCD_GRB_INTERFACE2_REG          (LCD_MODULE_BASE_ADDR | 0x0020)
#define LCD_GRB_INTERFACE3_REG          (LCD_MODULE_BASE_ADDR | 0x0024)
#define LCD_GRB_INTERFACE4_REG          (LCD_MODULE_BASE_ADDR | 0x0028)
#define LCD_GRB_INTERFACE5_REG          (LCD_MODULE_BASE_ADDR | 0x002C)
#define LCD_Y_ADDR_REG                  (LCD_MODULE_BASE_ADDR | 0x0030)
#define LCD_U_ADDR_REG                  (LCD_MODULE_BASE_ADDR | 0x0034)
#define LCD_V_ADDR_REG                  (LCD_MODULE_BASE_ADDR | 0x0038)
#define LCD_YUV_H_INFO_REG              (LCD_MODULE_BASE_ADDR | 0x003C)
#define LCD_YUV_V_INFO_REG              (LCD_MODULE_BASE_ADDR | 0x0040)
#define LCD_YUV_SCALER_INFO_REG         (LCD_MODULE_BASE_ADDR | 0x0044)
#define LCD_YUV_DISPLAY_INFO_REG        (LCD_MODULE_BASE_ADDR | 0x0048)
#define LCD_PANEL_SIZE_REG              (LCD_MODULE_BASE_ADDR | 0x004C)
#define LCD_REG_CONFIG_REG              (LCD_MODULE_BASE_ADDR | 0x0050)
#define LCD_LCD_GO_REG                  (LCD_MODULE_BASE_ADDR | 0x0054)
#define LCD_STATUS_REG                  (LCD_MODULE_BASE_ADDR | 0x0058)
#define LCD_LCD_INT_MASK                (LCD_MODULE_BASE_ADDR | 0x005C)
#define LCD_SW_CTRL_REG                 (LCD_MODULE_BASE_ADDR | 0x0060)

//LCD_TOP_CONFIGURE's  Value
#define PCLK_POL_N                      0
#define PCLK_POL_P                      1
#define RGB_CHANNEL_ENABLE              (1<<7)
#define RGB_CHANNEL_DISABLE             (0<<7)
#define RGB_565_ENABLE                  (1<<8)
#define MPU_INTERFACE_MODE              (1<<9)
#define RGB_INTERFACE_MODE              (2<<9)
#define RGB_OR_BGR                      (1<<27)

//LCD_MPU_1's  Value

#define MPU_4K_COLOR                    (0<<13)
#define MPU_256_64K_COLOR               (1<<13)
#define MPU_BUS_WIDTH_8BIT              (0<<14)
#define MPU_BUS_WIDTH_16BIT             (1<<14)
#define MPU_BUS_WIDTH_18BIT             (2<<14)
#define MPU_BUS_WIDTH_9BIT              (3<<14)
#define MPU_WR_CYCLE_BIT                (6)
#define MPU_WR_LOW_BIT                  (1)
#define MPU_A0_POLARITY_N               (0<<0)
#define MPU_A0_POLARITY_P               (1<<0)

#define MPU_READ_ENABLE                 (0<<21)
#define MPU_READ_DISABLE                (1<<21)
#define MPU_RD_SIG_ENABLE               (0<<20)
#define MPU_RD_SIG_DISABLE              (1<<20)
#define MPU_CS_SIG_ENABLE               (0<<19)
#define MPU_CS_SIG_DISABLE              (1<<19)
#define MPU_RS_SIG_ENABLE               (0<<18)
#define MPU_RS_SIG_DISABLE              (1<<18)


#define YUV_DISPLAY_ENABLE              (1<<22)
#define YUV_DISPLAY_DISABLE             (0<<22)
#define YUV_H_SCALER_ENABLE             (1<<21)
#define YUV_H_SCALER_DISABLE            (0<<21)
#define YUV_V_SCALER_ENABLE             (1<<20)
#define YUV_V_SCALER_DISABLE            (0<<20)


#define MPU_REFLASH_START               (1<<2)
#define MPU_REFLASH_DISABLE             (0<<2)
#define RGB_REFLASH_START               (1<<1)
#define RGB_REFLASH_DISABLE             (0<<1)
#define RGB_SYSTEIM_DISABLE             (0)
#define RGB_SYSTEIM_ENABLE              (1)


#define FIFO_ALARM_STAT                 (1<<8)
#define ALERT_VALID_STAT                (1<<7)
#define RGB_REFLASH_START_STAT          (1<<6)
#define RGB_REFLASH_OK_STAT             (1<<5)
#define RGB_REFLASH_HALF_OK_STAT        (1<<4)
#define MPU_DISPLAY_OK_STAT             (1<<3)
#define MPU_DISPLAY_START_STAT          (1<<2)
#define MPU_DISPLAYING_STAT             (1<<1)
#define RGB_SYS_ERROR_STAT              (1)

#define MPU_RS_SIG_CMD                  (0<<18)
#define MPU_RS_SIG_DATA                 (1<<18)

/** @} */

/** @{@name IMAGE module register and bit map define
 */
#define IMG_CMD_ADDR        (IMAGE_MODULE_BASE_ADDR | 0x0000)
#define IMG_HINFO1_ADDR     (IMAGE_MODULE_BASE_ADDR | 0x0004)
#define IMG_HINFO2_ADDR     (IMAGE_MODULE_BASE_ADDR | 0x0008)
#define IMG_VINFO1_ADDR     (IMAGE_MODULE_BASE_ADDR | 0x000C)
#define IMG_VINFO2_ADDR     (IMAGE_MODULE_BASE_ADDR | 0x0010)
#define IMG_SINFO_ADDR      (IMAGE_MODULE_BASE_ADDR | 0x0014)
#define IMG_YADDR_ADDR      (IMAGE_MODULE_BASE_ADDR | 0x0018)
#define IMG_UADDR_ADDR      (IMAGE_MODULE_BASE_ADDR | 0x001C)
#define IMG_VADDR_ADDR      (IMAGE_MODULE_BASE_ADDR | 0x0020)
#define IMG_RGBADDR_ADDR    (IMAGE_MODULE_BASE_ADDR | 0x0024)
#define IMG_STATUS_ADDR     (IMAGE_MODULE_BASE_ADDR | 0x0028)
#define IMG_HISTA_ADDR      (IMAGE_MODULE_BASE_ADDR | 0x0060)
#define IMG_HISTD_ADDR      (IMAGE_MODULE_BASE_ADDR | 0x0064)
/** @} */

/** @{@name UART module register and bit map define
 */
#define UART0_BASE_ADDR                 (UART_MODULE_BASE_ADDR + 0x00000000)
#define UART1_BASE_ADDR                 (UART_MODULE_BASE_ADDR + 0x00001000)
#define UART2_BASE_ADDR                 (UART_MODULE_BASE_ADDR + 0x00002000)

#define UART_RX_REG_OFFSET              0x00
#define UART_TX_REG_OFFSET              0x00
#define BAUDRATE_LOWDIV_REG_OFFSET      0x04
#define BAUDRATE_HIDIV_REG_OFFSET       0x08
#define UART_STATUS_REG_OFFSET          0x0C
#define UART_INTMODE_REG_OFFSET         0x10
#define UART_INTENA_REG_OFFSET          0x14

#define UART_TRANS_FIFO_FULL            (1 << 2)    // 1: transmit FIFO is full
#define UART_RECV_FIFO_EMPTY            (1 << 8)    // 1: receive FIFO is empty
/** @} */

/** @{@name HOST module register and bit map define
 */
#define HOST_INTERFACE_CPU          HOST_MODULE_BASE_ADDR
#define HOST_INTERFACE_TIMER        (HOST_MODULE_BASE_ADDR + 0x0200)
#define LCD_BYPASS_CONTROL          (HOST_MODULE_BASE_ADDR + 0x1018)

/** @} */

/** @{@name GPIO module register and bit map define
 */
#define GPIO_DIR1           (GPIO_MODULE_BASE_ADDR)
#define GPIO_INTE1          (GPIO_MODULE_BASE_ADDR + 0x04)
#define GPIO_INTP1          (GPIO_MODULE_BASE_ADDR + 0x08)
#define GPIO_EDGE1          (GPIO_MODULE_BASE_ADDR + 0x1008)
#define GPIO_IN1            (GPIO_MODULE_BASE_ADDR + 0xC)
#define GPIO_OUT1           (GPIO_MODULE_BASE_ADDR + 0x10)

#define GPIO_DIR2           (GPIO_MODULE_BASE_ADDR + 0x100)
#define GPIO_INTE2          (GPIO_MODULE_BASE_ADDR + 0x104)
#define GPIO_INTP2          (GPIO_MODULE_BASE_ADDR + 0x108)
#define GPIO_EDGE2          (GPIO_MODULE_BASE_ADDR + 0x1108)
#define GPIO_IN2            (GPIO_MODULE_BASE_ADDR + 0x10C)
#define GPIO_OUT2           (GPIO_MODULE_BASE_ADDR + 0x110)


#define GPIO_DIRECTION_REG1             (GPIO_MODULE_BASE_ADDR + 0x000)
#define GPIO_INT_CTRL_REG1              (GPIO_MODULE_BASE_ADDR + 0x004)
#define GPIO_INT_POLARITY1              (GPIO_MODULE_BASE_ADDR + 0x008)
#define GPIO_INPUT1                     (GPIO_MODULE_BASE_ADDR + 0x00C)
#define GPIO_OUTPUT1                    (GPIO_MODULE_BASE_ADDR + 0x010)
#define GPIO_PULLUP_REG1                (GPIO_MODULE_BASE_ADDR + 0x60)
#define GPIO_PULLUP_REG2                (GPIO_MODULE_BASE_ADDR + 0x64)
#define GPIO_PULLDOWN_REG               (GPIO_MODULE_BASE_ADDR + 0x68)


#define GPIO_DIRECTION_REG2             (GPIO_MODULE_BASE_ADDR + 0x100)
#define GPIO_INT_CTRL_REG2              (GPIO_MODULE_BASE_ADDR + 0x104)
#define GPIO_INT_POLARITY2              (GPIO_MODULE_BASE_ADDR + 0x108)
#define GPIO_INPUT2                     (GPIO_MODULE_BASE_ADDR + 0x10C)
#define GPIO_OUTPUT2                    (GPIO_MODULE_BASE_ADDR + 0x110)

#define REG_GPIO_SEL		0xb0090020
#define REG_GPIO_DIR		0xb0090000
#define REG_GPIO_INT_CTL	0xb0090004
#define REG_GPIO_INT_POL	0xb0090008
#define REG_GPIO_IN			0xb009000C
#define REG_GPIO_OUT		0xb0090010
#define REG_GPIO_PUP		0xb0090060
#define REG_GPIO_PDOWN		0xb0090068

/** @} */

/** @{@name TIMER module register and bit map define
 */
#define TIMER1_CTRL_REG         (TIMER_MODULE_BASE_ADDR + 0x14)
#define TIMER2_CTRL_REG         (TIMER_MODULE_BASE_ADDR + 0x18)
#define TIMER3_CTRL_REG         (TIMER_MODULE_BASE_ADDR + 0x114)
#define TIMER4_CTRL_REG         (TIMER_MODULE_BASE_ADDR + 0x118)    // watchdog function

#define TIMER_CLEAR_STATUS_BIT  (1 << 30)
/** @} */



/** @{@name Nandflash Controller Define
 *  Define the register and bit map of nandflash controller
 *
 */
// DMA
#define FLASH_DMA_REG0      (NAND_MODULE_BASE_ADDR + 0x8000)
#define FLASH_DMA_REG1      (NAND_MODULE_BASE_ADDR + 0x8004)
#define FLASH_DMA_REG2      (NAND_MODULE_BASE_ADDR + 0x8008)
#define FLASH_DMA_REG3      (NAND_MODULE_BASE_ADDR + 0x800C)
#define FLASH_DMA_REG4      (NAND_MODULE_BASE_ADDR + 0x8010)
#define FLASH_DMA_REG5      (NAND_MODULE_BASE_ADDR + 0x8014)

// ECC
#define FLASH_ECC_REG0      (NAND_MODULE_BASE_ADDR + 0x8080)    // encode mudule start address
#define FLASH_ECC_REG4      (NAND_MODULE_BASE_ADDR + 0x8090)    // decode module start address
#define FLASH_ECC_REG5      (NAND_MODULE_BASE_ADDR + 0x8094)    // decode module start address
#define FLASH_ECC_REG6      (NAND_MODULE_BASE_ADDR + 0x8098)    // decode module start address
#define FLASH_ECC_REG7      (NAND_MODULE_BASE_ADDR + 0x809C)    // decode module start address

// NF controller
#define FLASH_CTRL_REG0     (NAND_MODULE_BASE_ADDR + 0x8100)
#define FLASH_CTRL_REG20    (NAND_MODULE_BASE_ADDR + 0x8150)
#define FLASH_CTRL_REG21    (NAND_MODULE_BASE_ADDR + 0x8154)
#define FLASH_CTRL_REG22    (NAND_MODULE_BASE_ADDR + 0x8158)
#define FLASH_CTRL_REG23    (NAND_MODULE_BASE_ADDR + 0x815C)
#define FLASH_CTRL_REG24    (NAND_MODULE_BASE_ADDR + 0x8160)


//basic save information
#define PHY_ERR_FLAG    0       //bad block flag for large page(2kb)
#define PHY_ERR_FLAG1   5       //bad block flag for small page(512b)
#define NFC_FIFO_SIZE       512
#define NFC_LOG_PAGE_SIZE   512
#define NFC_LOG_SPARE_SIZE  16
#define NFC_FS_INFO_OFFSET  520//must be a value %4 = 0
#define NFC_FS_INFO_SIZE    6
#define NFC_SUPPORT_CHIPNUM 4


// ecc check define
#define CHECK_OVER              0x0540
#define CHECK_NO_ERROR          0x3800
#define CANT_CORRECT            0x02A0
#define ECC_RS_LEN              251
#define FS_INFO_LEN             6
#define ECC_BCH_LEN             (518-ECC_RS_LEN*2-FS_INFO_LEN)


#define COMMAND_CYCLES_CONF     0x64
#define ADDRESS_CYCLES_CONF     0x62
#define READ_DATA_CONF          0x118
#define WRITE_DATA_CONF         0x128
#define READ_CMD_CONF           0x58
#define READ_INFO_CONF          0x58

#define DELAY_CNT_CONF          0x400



#define WAIT_JUMP_CONF          0x200

#define LAST_CMD_FLAG           (1 << 0)

#define DEFAULT_GO              ((1 << 30)|(1<<9))

#define SAVE_MODE_FLAG          1

#define ECC_ENCODE_CONFIG       0x0018
#define ECC_ENCODE_DISCFG       0x10000018

#define ECC_DECODE_CONFIG       0x0018
#define ECC_DECODE_DISCFG       0x20000018
/** @} */


/** @{@name RTC module register and bit map define
 */
#define RTC_LOAD_REG            (RTC_MODULE_BASE_ADDR + 0x00)   // power on/off
#define RTC_ALARM_MASK_REG      (RTC_MODULE_BASE_ADDR + 0x04)   // alarm mask
#define RTC_WGPIO_MASK_REG      (RTC_MODULE_BASE_ADDR + 0x08)   // wakeup GPIO mask
#define RTC_WAKEUP_STATUS_REG   (RTC_MODULE_BASE_ADDR + 0x0C)   // hold each wakeup GPIO or alarm status
#define RTC_CONFIG_REG          (RTC_MODULE_BASE_ADDR + 0x10)   // config the RTC module
#define RTC_COUNT_REG           (RTC_MODULE_BASE_ADDR + 0x14)   // real time clock ?
#define RTC_PD_LAST_VALUE_REG   (RTC_MODULE_BASE_ADDR + 0x18)   // wakeup signal last cycle
#define RTC_WGPIO_POL_REG       (RTC_MODULE_BASE_ADDR + 0x20)   // wakeup GPIO polarity control

/** @} */

/** @{@name CRC module register and bit map define
 */
#define DMACRC_CMD_REG              (DMA_MODULE_BASE_ADDR + 0x0000)
#define DMACRC_STATUS_REG           (DMA_MODULE_BASE_ADDR + 0x0004)
#define DMACRC_CNT_REG              (DMA_MODULE_BASE_ADDR + 0x0008)
#define DMACRC_SRC_ADDR_REG         (DMA_MODULE_BASE_ADDR + 0x000C)
#define DMACRC_SRC_ADDR_REG         (DMA_MODULE_BASE_ADDR + 0x000C)
#define DMACRC_DEST_ADDR_REG        (DMA_MODULE_BASE_ADDR + 0x0010)
#define DMACRC_MEM_TYPE_REG         (DMA_MODULE_BASE_ADDR + 0x0014)
#define DMACRC_VALUE_REG            (DMA_MODULE_BASE_ADDR + 0x0018)
#define DMACRC_CMD_START_BIT        0x01
#define DMACRC_CMD_ENABLE_BIT       0x02
#define DMACRC_STATUS_FINISH_BIT    0x01
/** @} */

/** @{@name ANALOG module register and bit map define
 */
#define SHARE_PIN_CONTROL2      (ADC_MODULE_BASE_ADDR + 0x1C)
#define SHARE_PIN_CONTROL1      (ADC_MODULE_BASE_ADDR + 0x20)
#define ADC2_CONTROL_REG        (ADC_MODULE_BASE_ADDR + 0x24)
#define DAC_CONTROL             (ADC_MODULE_BASE_ADDR + 0x28)
#define TS_THRESHOLD_REG        (ADC_MODULE_BASE_ADDR + 0x2C)
#define ANALOG_CONTROL2         (ADC_MODULE_BASE_ADDR + 0x30)
#define ANLSOFT_CONTROL         (ADC_MODULE_BASE_ADDR + 0x34)
#define TS_INT_STATUS_REG       (ADC_MODULE_BASE_ADDR + 0x38)
#define TS_CONTROL_REG          (ADC_MODULE_BASE_ADDR + 0x3C)
#define ADC1_CONTROL_REG        (ADC_MODULE_BASE_ADDR + 0x40)
#define USB_CONTROL             (ADC_MODULE_BASE_ADDR + 0x44)
#define X_COORDINATE_REG        (ADC_MODULE_BASE_ADDR + 0x50)
#define Y_COORDINATE_REG        (ADC_MODULE_BASE_ADDR + 0x54)
#define VOICE_DATA_REG          (ADC_MODULE_BASE_ADDR + 0x58)
#define BATTERY_DATA_REG        (ADC_MODULE_BASE_ADDR + 0x78)

#define PWM0_CONTROL            (ADC_MODULE_BASE_ADDR + 0x70)
#define PWM1_CONTROL            (ADC_MODULE_BASE_ADDR + 0x74)

#define PMU_OT_CONTROL          (ADC_MODULE_BASE_ADDR + 0x80)
/** @} */
/*@}*/


#ifdef USB_HOST

//*******************************************************************
/** @{@name USB register and bit map define
 *  Define register to control USB port and the bit map of the register
 */
 /** System register*/
#define USB_CONTROL_REG          (TIMER_MODULE_BASE_ADDR + 0x0044)

#define USB_SUS_EN                  (1 << 2)
#define USB_REG_EN                  (1 << 1)

/** USB controller register*/
#define USB_FIFO_EP0              (USB_BASE_ADDR + 0x0020)

#define USB_REG_FADDR             (USB_BASE_ADDR + 0x0000)
#define USB_REG_POWER             (USB_BASE_ADDR + 0x0001)
#define USB_REG_INTRTX1           (USB_BASE_ADDR + 0x0002)
#define USB_REG_INTRTX2           (USB_BASE_ADDR + 0x0003)
#define USB_REG_INTRRX1           (USB_BASE_ADDR + 0x0004)
#define USB_REG_INTRRX2           (USB_BASE_ADDR + 0x0005)
#define USB_REG_INTRTX1E          (USB_BASE_ADDR + 0x0006)
#define USB_REG_INTRTX2E          (USB_BASE_ADDR + 0x0007)
#define USB_REG_INTRRX1E          (USB_BASE_ADDR + 0x0008)
#define USB_REG_INTRRX2E          (USB_BASE_ADDR + 0x0009)
#define USB_REG_INTRUSB           (USB_BASE_ADDR + 0x000A)
#define USB_REG_INTRUSBE          (USB_BASE_ADDR + 0x000B)
#define USB_REG_FRAME1            (USB_BASE_ADDR + 0x000C)
#define USB_REG_FRAME2            (USB_BASE_ADDR + 0x000D)
#define USB_REG_INDEX             (USB_BASE_ADDR + 0x000E)
#define USB_REG_TESEMODE          (USB_BASE_ADDR + 0x000F)
#define USB_REG_DEVCTL            (USB_BASE_ADDR + 0x0060)
#define USB_REG_TXMAXP0           (USB_BASE_ADDR + 0x0010)
#define USB_REG_TXMAXP1           (USB_BASE_ADDR + 0x0010)
#define USB_REG_CSR0              (USB_BASE_ADDR + 0x0012)
#define USB_REG_TXCSR1            (USB_BASE_ADDR + 0x0012)
#define USB_REG_CSR02             (USB_BASE_ADDR + 0x0013)
#define USB_REG_TXCSR2            (USB_BASE_ADDR + 0x0013)
#define USB_REG_RXMAXP1           (USB_BASE_ADDR + 0x0014)
#define USB_REG_RXMAXP2           (USB_BASE_ADDR + 0x0015)
#define USB_REG_RXCSR1            (USB_BASE_ADDR + 0x0016)
#define USB_REG_RXCSR2            (USB_BASE_ADDR + 0x0017)
#define USB_REG_COUNT0            (USB_BASE_ADDR + 0x0018)
#define USB_REG_RXCOUNT1          (USB_BASE_ADDR + 0x0018)
#define USB_REG_RXCOUNT2          (USB_BASE_ADDR + 0x0019)
#define USB_REG_TXTYPE            (USB_BASE_ADDR + 0x001A)
#define USB_REG_RXTYPE            (USB_BASE_ADDR + 0x001C)
#define USB_REG_RXINTERVAL        (USB_BASE_ADDR + 0x001D)
#define USB_REG_NAKLIMIT0         (USB_BASE_ADDR + 0x001B)


/**Dynamic FIFO sizing   JUST FOR HOST  */
#define USB_REG_TXFIFO1           (USB_BASE_ADDR + 0x001C)
#define USB_REG_TXFIFO2           (USB_BASE_ADDR + 0x001D)
#define USB_REG_RXFIFO1           (USB_BASE_ADDR + 0x001E)
#define USB_REG_RXFIFO2           (USB_BASE_ADDR + 0x001F)

#define USB_REG_FIFOSIZE          (USB_BASE_ADDR + 0x001F)

/**  USB DMA */
#define USB_DMA_INTR              (USB_BASE_ADDR + 0x0200)
#define USB_DMA_CNTL_1            (USB_BASE_ADDR + 0x0204)
#define USB_DMA_ADDR_1            (USB_BASE_ADDR + 0x0208)
#define USB_DMA_COUNT_1           (USB_BASE_ADDR + 0x020c)
#define USB_DMA_CNTL_2            (USB_BASE_ADDR + 0x0214)
#define USB_DMA_ADDR_2            (USB_BASE_ADDR + 0x0218)
#define USB_DMA_COUNT_2           (USB_BASE_ADDR + 0x021c)


/** CSR0 bit masks mode-agnostic */
#define USB_CSR0_RXPKTRDY         (1 << 0)
#define USB_CSR0_TXPKTRDY         (1 << 1)

/** CSR0 bit masks IN SLAVE MODE */
#define USB_CSR0_P_SENTSTALL      (1 << 2)
#define USB_CSR0_P_DATAEND        (1 << 3)
#define USB_CSR0_P_SETUPEND       (1 << 4)
#define USB_CSR0_P_SENDSTALL      (1 << 5)
#define USB_CSR0_P_SVDRXPKTRDY    (1 << 6)
#define USB_CSR0_P_SVDSETUPEND    (1 << 7)

/**  CSR0 bit masks in MASTER mode   */
/*#define USB_CSR0_H_RXSTALL        (1 << 2)
#define USB_CSR0_H_SETUPPKT       (1 << 3)
#define USB_CSR0_H_ERROR          (1 << 4)
#define USB_CSR0_H_REQPKT         (1 << 5)
#define USB_CSR0_H_STATUSPKT      (1 << 6)

#define USB_CSR0_FLUSHFIFO        0x1*/


/** TXCSR1 bit masks mode-agnostic */
#define USB_TXCSR1_TXPKTRDY       (1 << 0)
#define USB_TXCSR1_FIFONOTEMPTY   (1 << 1)
#define USB_TXCSR1_FLUSHFIFO      (1 << 3)
#define USB_TXCSR1_CLRDATATOG     (1 << 6)

/** TXCSR1 bit masks slave mode */
#define USB_TXCSR1_P_UNDERRUN     (1 << 2)
#define USB_TXCSR1_P_SENDSTALL    (1 << 4)
#define USB_TXCSR1_P_SENTSTALL    (1 << 5)

/** TXCSR1 bit masks MASTER MODE */
#define USB_TXCSR1_H_ERROR        0x04
#define USB_TXCSR1_H_RXSTALL      0x20

/** RXCSR1 bit masks mode-agnostic */
#define USB_RXCSR1_RXPKTRDY       (1 << 0)
/*#define USB_RXCSR1_FIFOFULL       (1 << 1)
#define USB_RXCSR1_DATAERR        (1 << 3)
#define USB_RXCSR1_FLUSHFIFO      (1 << 4)
#define USB_RXCSR1_CLRDATATOG     (1 << 7)*/

/** RXCSR1 bit masks slave mode */
#define USB_RXCSR1_P_OVERRUN      (1 << 2) //0x04
#define USB_RXCSR1_P_SENDSTALL    (1 << 5) //0x20
#define USB_RXCSR1_P_SENTSTALL    (1 << 6) //0x40

/** RXCSR1 bit masks MASTER MODE */
#define USB_RXCSR1_H_ERROR        (1 << 2) //0x04
#define USB_RXCSR1_H_REQPKT       (1 << 5) //0x20
#define USB_RXCSR1_H_RXSTALL      (1 << 6) //0x40

/** TXCSR2 bit masks** THERE IS NO DIFFERENCE FOR MASTER AND SLAVE MODE */
#define USB_TXCSR2_FRCDT          (1 << 3)
#define USB_TXCSR2_DMAMODE        (1 << 2)
#define USB_TXCSR2_DMAENAB        (1 << 4)
#define USB_TXCSR2_MODE           (1 << 5)
#define USB_TXCSR2_ISO            (1 << 6)
#define USB_TXCSR2_AUTOSET        (1 << 7)

/** RXCSR2 bit masks */
#define USB_RXCSR2_AUTOCLEAR      (1 << 7)
#define USB_RXCSR2_DMAENAB        (1 << 5)
#define USB_RXCSR2_DISNYET        (1 << 4)
#define USB_RXCSR2_DMAMODE        (1 << 3)
#define USB_RXCSR2_INCOMPRX       (1 << 0)


/** POWER REGISTER  */
#define USB_POWER_ENSUSPEND       (1 << 0)
#define USB_POWER_SUSPENDM        (1 << 1)
#define USB_POWER_RESUME          (1 << 2)
#define USB_POWER_RESET           (1 << 3)
#define USB_POWER_VBUSLO          (1 << 4)
#define USB_POWER_VBUSSESS        (1 << 5)
#define USB_POWER_VBUSVAL         (1 << 6)
#define USB_POWER_ISOUP           (1 << 7)


/** EPn ENABLE */
#define USB_EP0_TX_ENABLE         (1 << 0)
#define USB_EP1_TX_ENABLE         (1 << 1)
#define USB_EP2_TX_ENABLE         (1 << 2)
#define USB_EP3_TX_ENABLE         (1 << 3)
#define USB_EP4_TX_ENABLE         (1 << 4)
#define USB_EP5_TX_ENABLE         (1 << 5)

#define USB_EP0_RX_ENABLE         (1 << 0)
#define USB_EP1_RX_ENABLE         (1 << 1)
#define USB_EP2_RX_ENABLE         (1 << 2)
#define USB_EP3_RX_ENABLE         (1 << 3)
#define USB_EP4_RX_ENABLE         (1 << 4)
#define USB_EP5_RX_ENABLE         (1 << 5)


#define USB_EP0_INDEX             0
#define USB_EP1_INDEX             (1 << 0)
#define USB_EP2_INDEX             (1 << 1)
#define USB_EP3_INDEX             ((1 << 1)|(1 << 0))
#define USB_EP4_INDEX             (1 << 2)
#define USB_EP5_INDEX             ((1 << 2)|(1 << 0))
#define USB_EP6_INDEX             ((1 << 2)|(1 << 1))
#define USB_EP7_INDEX             ((1 << 2)|(1 << 1)|(1 << 0))


/** EP INTERRUPT in INTR register */
#define USB_INTR_EP0              (1 << 0)
#define USB_INTR_EP1              (1 << 1)
#define USB_INTR_EP2              (1 << 2)
#define USB_INTR_EP3              (1 << 3)
#define USB_INTR_EP4              (1 << 4)
#define USB_INTR_EP5              (1 << 5)
#define USB_INTR_EP6              (1 << 6)
#define USB_INTR_EP7              (1 << 7)


/** dma control register */
#define USB_CTL_DMA_ENA           (1 << 0)
#define USB_CTL_DMA_DIRE_IN       (1 << 1)
#define USB_CTL_DMA_MODE          (1 << 2)
#define USB_CTL_DMA_INT_ENA       (1 << 3)
#define USB_CTL_DMA_BUS_ERR       (1 << 7)

/* usb control and status register */
#define USB_REG_RXCSR1_RXSTALL    (1 << 6)
#define USB_REG_RXCSR1_REQPKT     (1 << 5)

#define USB_TXCSR_AUTOSET               0x80
#define USB_TXCSR_ISO                   0x40
#define USB_TXCSR_MODE1                 0x20
#define USB_TXCSR_DMAREQENABLE          0x10
#define USB_TXCSR_FRCDATATOG            0x8
#define USB_TXCSR_DMAREQMODE1           0x4
#define USB_TXCSR_DMAREQMODE0           0x0


#define USB_RXCSR_AUTOSET               0x80
#define USB_RXCSR_ISO                   0x40
#define USB_RXCSR_DMAREQENAB            0x20
#define USB_RXCSR_DISNYET               0x10
#define USB_RXCSR_DMAREQMODE1           0x8
#define USB_RXCSR_DMAREQMODE0           0x0
#define USB_RXCSR_INCOMPRX              0x1

#define USB_ENABLE_DMA                  1
#define USB_DIRECTION_RX                (0<<1)
#define USB_DIRECTION_TX                (1<<1)
#define USB_DMA_MODE1                   (1<<2)
#define USB_DMA_MODE0                   (0<<2)
#define USB_DMA_INT_ENABLE              (1<<3)
#define USB_DMA_INT_DISABLE             (0<<3)
#define USB_DMA_BUS_ERROR               (1<<8)
#define USB_DMA_BUS_MODE0               (0<<9)
#define USB_DMA_BUS_MODE1               (1<<9)
#define USB_DMA_BUS_MODE2               (2<<9)
#define USB_DMA_BUS_MODE3               (3<<9)

#define DMA_CHANNEL1_INT                1
#define DMA_CHANNEL2_INT                2
#define DMA_CHANNEL3_INT                4
#define DMA_CHANNEL4_INT                8
#define DMA_CHANNEL5_INT                0x10

/**Interrupt register bit masks */
#define USB_INTR_SUSPEND          (1 << 0)
#define USB_INTR_RESUME           (1 << 1)
#define USB_INTR_RESET            (1 << 2)
#define USB_INTR_SOF              (1 << 3)
#define USB_INTR_CONNECT          (1 << 4)
#define USB_INTR_DISCONNECT       (1 << 5)
#define USB_INTR_SESSREQ          (1 << 6)
#define USB_INTR_VBUSERROR        (1 << 7)   /* #FOR SESSION END  */

/** Interrupt register bit enable */
#define USB_INTR_SUSPEND_ENA      (1 << 0)
#define USB_INTR_RESUME_ENA       (1 << 1)
#define USB_INTR_RESET_ENA        (1 << 2)
#define USB_INTR_SOF_ENA          (1 << 3)
#define USB_INTR_CONNECT_ENA      (1 << 4)
#define USB_INTR_DISCONNECT_ENA   (1 << 5)
#define USB_INTR_SESSREQ_ENA      (1 << 6)
#define USB_INTR_VBUSERROR_ENA    (1 << 7)   /* #FOR SESSION END  */

/**Reset mode */
#define RESET_CTRL_USB          (1 << 5)

#define SHARE_PIN_GPIO_DRVVBUS          (1<<17)
/** @} */
#endif

/** @{@name Register Operation Define
 *  Define the macro for read/write register and memory
 */
    /* ------ Macro definition for reading/writing data from/to register ------ */
#define HAL_READ_UINT8( _register_, _value_ )        ((_value_) = *((volatile BYTE *)(_register_)))
#define HAL_WRITE_UINT8( _register_, _value_ )       (*((volatile BYTE *)(_register_)) = (_value_))
#define HAL_READ_UINT16( _register_, _value_ )      ((_value_) = *((volatile WORD *)(_register_)))
#define HAL_WRITE_UINT16( _register_, _value_ )     (*((volatile WORD *)(_register_)) = (_value_))
#define HAL_READ_UINT32( _register_, _value_ )      ((_value_) = *((volatile DWORD *)(_register_)))
#define HAL_WRITE_UINT32( _register_, _value_ )     (*((volatile DWORD *)(_register_)) = (_value_))

/** @} */
/*@}*/

#endif  // _ANYKA_CPU_H_
