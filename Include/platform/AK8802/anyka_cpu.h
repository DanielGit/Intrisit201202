#include "anyka_types.h"
#include "anyka_cpu_780x.h"


/** @file
 * @brief Define the register of ANYKA CPU
 *
 * Define the register address and bit map for system.
 * Copyright (C) 2006 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author 
 * @date 2008-01-05
 * @version 1.0
 * @note CPU AK7802M
 */

#ifndef _ANYKA_CPU_H_
#define _ANYKA_CPU_H_



/** @defgroup ANYKA_CPU  
 *	@ingroup M3PLATFORM
 */
/*@{*/


/** @{@name Base Address Define
 *	The base address of system memory space is define here. 
 *	Include memory assignment and module base address define.
 */
 /**Memory assignment*/
#define ROM_BASE_ADDR					0x00000000	//on chip rom
#define NORFLASH_BASE_ADDR				0x10000000	//NOR FLASH start address
#define CHIP_CONF_BASE_ADDR				0xB8000000	// chip configurations
#define TRAM_BASE_ADDR					0x40000000	// on chip RAM start address
#define RAMLIKE_BASE_ADDR				0x50000000	// RAM-LIKE start address
#define NANDFLASH_BASE_ADDR				0x60000000	// NAND FLASH start address
#define USB_BASE_ADDR					0xB7000000	// USB
#define L2_BUF_MEM_BASE_ADDR	    	0xBC000000  //L2 Buffer start address



/**Module base address of chip config*/
#define LCD_MODULE_BASE_ADDR			0xB0010000	// LCD controller
#define IMAGE_MODULE_BASE_ADDR			0xB0030000	// image sensor
#define UART_MODULE_BASE_ADDR			0xB0026000	// UART
                                    	
#define SPI_MODULE_BASE_ADDR			0xB0020000	// SPI
#define L2_BASE_ADDR					0xB002c000	//L2
#define NF_REG_BASE_ADDR				0xB0020000	// NAND FLASH controller
#define ADC_MODULE_BASE_ADDR    		0xB8000000	// Analog 
#define GPIO_MODULE_BASE_ADDR			0xB8000000	// GPIO registers
#define TIMER_MODULE_BASE_ADDR			0xB8000000	// timer registers
#define FLASH_MODULE_BASE_ADDR			0xB00A0000	// NOR FLASH controller
#define RAM_MODULE_BASE_ADDR			0xB00B0000	// SDRAM/SSRAM/SRAM controller
#define DMA_MODULE_BASE_ADDR			0xB00C0000	// DMA registers
#define CRC_MODULE_BASE_ADDR			0xB8000000	// CRC registers
#define RTC_MODULE_BASE_ADDR			0xB8000000	// RTC
/** @} */


/** @{@name System Control Register Bit map
 *	Define system control register bit map here,
 *	include CPU work mode, CLOCK control, INTERRUPT control
 */
/**CPU work mode */
#define ANYKA_CPU_Mode_USR				0x10
#define ANYKA_CPU_Mode_FIQ				0x11
#define ANYKA_CPU_Mode_IRQ				0x12
#define ANYKA_CPU_Mode_SVC				0x13
#define ANYKA_CPU_Mode_ABT				0x17
#define ANYKA_CPU_Mode_UNDEF			0x1B
#define ANYKA_CPU_Mode_SYS				0x1F		
#define ANYKA_CPU_I_Bit					0x80
#define ANYKA_CPU_F_Bit					0x40

/** CLOCK control register bit map*/
#define	CLOCK_CTRL_IMAGE_H263_MPEG4		(0x1)
#define	CLOCK_CTRL_CAMERA			    (1 << 1)
#define	CLOCK_CTRL_SPI12_MMC_UART2      (1 << 2)
#define	CLOCK_CTRL_LCD					(1 << 3)
#define	CLOCK_CTRL_AUDIO				(1 << 4)
#define	CLOCK_CTRL_USBOTG				(1 << 5)
#define	CLOCK_CTRL_H264				    (1 << 6)
#define	CLOCK_CTRL_USBFS				(1 << 7)
#define	CLOCK_CTRL_SDIO_UART34			(1 << 8)
#define	CLOCK_CTRL_SDRAM_DDR			(1 << 10)
#define	CLOCK_CTRL_MOTION_MODULE		(1 << 11)
#define	CLOCK_CTRL_2D_ACCELERATOR		(1 << 12)
#define	CLOCK_CTRL_NANDFLASH		    (1 << 13)
#define	CLOCK_CTRL_L2_UART1		        (1 << 15)

/** interrupt status register bit map*/
#define	INT_STATUS_LCD_BIT				(1 << 1)
#define	INT_STATUS_CAMERA_BIT			(1 << 2)
#define	INT_STATUS_AUDIO_BIT			(1 << 5)
#define INT_STATUS_L2_BIT				(1 << 10)
#define	INT_STATUS_UART4_BIT			(1 << 13)
#define	INT_STATUS_UART3_BIT			(1 << 14)
#define	INT_STATUS_UART2_BIT			(1 << 15)
#define	INT_STATUS_UART1_BIT			(1 << 16)
#define	INT_STATUS_SPI2_BIT				(1 << 17)
#define	INT_STATUS_SPI1_BIT				(1 << 18)
#define INT_STATUS_SDIO_BIT				(1 << 21)
#define INT_STATUS_MMCSD_BIT			(1 << 22)
#define INT_STATUS_USB_BIT				(1 << 25)
#define	INT_STATUS_SYS_MODULE_BIT		(1 << 27)
                                        
//level 2 interrupt status bit map      
#define	INT_STATUS_TS_BIT				(1 << 16)
#define	INT_STATUS_TIMER5_BIT			(1 << 17)
#define	INT_STATUS_TIMER4_BIT			(1 << 18)
#define	INT_STATUS_TIMER3_BIT			(1 << 19)
#define	INT_STATUS_TIMER2_BIT			(1 << 20)
#define	INT_STATUS_TIMER1_BIT			(1 << 21)
#define	INT_STATUS_ASICCLK_BIT			(1 << 22)
#define	INT_STATUS_WGPIO_BIT			(1 << 23)
#define	INT_STATUS_RTC_READY_BIT		(1 << 24)
#define	INT_STATUS_RTC_ALARM_BIT		(1 << 25)
#define	INT_STATUS_GPIO_BIT				(1 << 26)
//#define	INT_STATUS_GPS_BIT				0x08000000


/* define the level1 interrupt valid bits */
#define INT_STATUS_NBITS				27

/** IRQ interrupt mask register bit map*/
#define IRQ_MASK_LCD_BIT				(1 << 1)
#define IRQ_MASK_CAMERA_BIT				(1 << 2)
#define IRQ_MASK_AUDIO_BIT				(1 << 5)
#define IRQ_MASK_L2_BIT					(1 << 10)
#define	IRQ_MASK_UART4_BIT				(1 << 13)
#define	IRQ_MASK_UART3_BIT				(1 << 14)
#define	IRQ_MASK_UART2_BIT				(1 << 15)
#define	IRQ_MASK_UART1_BIT				(1 << 16)
#define	IRQ_MASK_SPI2_BIT				(1 << 17)
#define	IRQ_MASK_SPI1_BIT				(1 << 18)
#define IRQ_MASK_SDIO_BIT				(1 << 21)
#define IRQ_MASK_MMCSD_BIT				(1 << 22)
#define IRQ_MASK_USB_BIT				(1 << 25)
#define	IRQ_MASK_SYS_MODULE_BIT		    (1 << 27)
#define IRQ_MASK_USB_FS_BIT				(1 << 23)

//level 2 interrupt mask bit map
#define	IRQ_MASK_TS_BIT					1
#define	IRQ_MASK_TIMER5_BIT			    (1 << 1)
#define	IRQ_MASK_TIMER4_BIT			    (1 << 2)
#define	IRQ_MASK_TIMER3_BIT			    (1 << 3)
#define	IRQ_MASK_TIMER2_BIT			    (1 << 4)
#define	IRQ_MASK_TIMER1_BIT			    (1 << 5)
#define	IRQ_MASK_ASICCLK_BIT			(1 << 6)
#define	IRQ_MASK_WGPIO_BIT				(1 << 7)
#define	IRQ_MASK_RTC_READY_BIT			(1 << 8)
#define	IRQ_MASK_RTC_ALARM_BIT			(1 << 9)
#define	IRQ_MASK_GPIO_BIT				(1 << 10)

/** @} */


/** @{@name L2 cache register and bit map define
 */ 

#define	L2_DMA_ADDR				        (L2_BASE_ADDR+0x00)
#define	L2_DMA_CNT				        (L2_BASE_ADDR+0x40)
#define	L2_DMA_REQ				        (L2_BASE_ADDR+0x80)
#define	L2_FRAC_ADDR					(L2_BASE_ADDR+0x84)
#define	L2_COMBUF_CFG					(L2_BASE_ADDR+0x88)
#define	L2_UARTBUF_CFG			        (L2_BASE_ADDR+0x8c)
#define	L2_ASSIGN_REG1			        (L2_BASE_ADDR+0x90)
#define	L2_ASSIGN_REG2			        (L2_BASE_ADDR+0x94)
#define	L2_LDMA_CFG				        (L2_BASE_ADDR+0x98)
#define	L2_INT_ENA						(L2_BASE_ADDR+0x9c)
#define	L2_STAT_REG1					(L2_BASE_ADDR+0xa0)
#define	L2_STAT_REG2					(L2_BASE_ADDR+0xa8)


/** @{@name LCD module register and bit map define
 */ 
#define LCD_TOP_CONFIGURE_REG		    (LCD_MODULE_BASE_ADDR | 0x0000)
#define LCD_MPU_CTL_REG		            (LCD_MODULE_BASE_ADDR | 0x0004)
#define LCD_RST_PIN_REG			    (LCD_MODULE_BASE_ADDR | 0x0008)
#define LCD_MPU_READ_REG		    (LCD_MODULE_BASE_ADDR | 0x000C)
#define LCD_RGB_CTL_REG1		    (LCD_MODULE_BASE_ADDR | 0x0010)
#define LCD_RGB_CTL_REG2		    (LCD_MODULE_BASE_ADDR | 0x0014)
#define LCD_RGB_VIRTUAL_SIZE_REG	    (LCD_MODULE_BASE_ADDR | 0x0018)
#define LCD_RGB_VIRTUAL_OFFSET_REG	    (LCD_MODULE_BASE_ADDR | 0x001C)

#define LCD_OSD_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x20)
#define LCD_OSD_OFFSET_REG			    (LCD_MODULE_BASE_ADDR | 0x24)
#define LCD_OSD_F_COLOR1_REG			(LCD_MODULE_BASE_ADDR | 0x28)
#define LCD_OSD_F_COLOR2_REG			(LCD_MODULE_BASE_ADDR | 0x2C)
#define LCD_OSD_F_COLOR3_REG			(LCD_MODULE_BASE_ADDR | 0x30)
#define LCD_OSD_F_COLOR4_REG			(LCD_MODULE_BASE_ADDR | 0x34)
#define LCD_OSD_F_COLOR5_REG			(LCD_MODULE_BASE_ADDR | 0xd0)
#define LCD_OSD_F_COLOR6_REG			(LCD_MODULE_BASE_ADDR | 0xd4)
#define LCD_OSD_F_COLOR7_REG			(LCD_MODULE_BASE_ADDR | 0xd8)
#define LCD_OSD_F_COLOR8_REG			(LCD_MODULE_BASE_ADDR | 0xdc)
#define LCD_OSD_SIZE_ALPHA_REG			(LCD_MODULE_BASE_ADDR | 0x38)

#define LCD_GRB_BACKGROUND_REG		(LCD_MODULE_BASE_ADDR | 0x003c)
#define LCD_RGB_CTL_REG3		    (LCD_MODULE_BASE_ADDR | 0x0040)
#define LCD_RGB_CTL_REG4		    (LCD_MODULE_BASE_ADDR | 0x0044)
#define LCD_RGB_CTL_REG5		    (LCD_MODULE_BASE_ADDR | 0x0048)
#define LCD_RGB_CTL_REG6		    (LCD_MODULE_BASE_ADDR | 0x004C)
#define LCD_RGB_CTL_REG7		    (LCD_MODULE_BASE_ADDR | 0x0050)
#define LCD_RGB_CTL_REG8		    (LCD_MODULE_BASE_ADDR | 0x0054)
#define LCD_RGB_CTL_REG9		    (LCD_MODULE_BASE_ADDR | 0x0058)

#define LCD_Y1_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x005c)
#define LCD_U1_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x0060) 
#define LCD_V1_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x0064) 
#define LCD_YUV1_H_INFO_REG		    (LCD_MODULE_BASE_ADDR | 0x0068) 
#define LCD_YUV1_V_INFO_REG		    (LCD_MODULE_BASE_ADDR | 0x006c) 
#define LCD_YUV1_VIR_SIZE_REG		    (LCD_MODULE_BASE_ADDR | 0x0078) 
#define LCD_YUV1_VIR_OFFSET_REG		    (LCD_MODULE_BASE_ADDR | 0x007c) 
#define LCD_YUV1_SCALER_INFO_REG	    (LCD_MODULE_BASE_ADDR | 0x0070) 
#define LCD_YUV1_DISPLAY_INFO_REG	    (LCD_MODULE_BASE_ADDR | 0x0074)


#define LCD_Y2_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x0080)
#define LCD_U2_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x0084) 
#define LCD_V2_ADDR_REG			    (LCD_MODULE_BASE_ADDR | 0x0088) 
#define LCD_YUV2_H_INFO_REG		    (LCD_MODULE_BASE_ADDR | 0x008c) 
#define LCD_YUV2_V_INFO_REG		    (LCD_MODULE_BASE_ADDR | 0x0090) 
#define LCD_YUV2_SCALER_INFO_REG	    (LCD_MODULE_BASE_ADDR | 0x0094) 
#define LCD_YUV2_DISPLAY_INFO_REG	    (LCD_MODULE_BASE_ADDR | 0x0098)

#define LCD_RGB_OFFSET_REG		    (LCD_MODULE_BASE_ADDR | 0x00A8)
#define LCD_RGB_SIZE_REG		    (LCD_MODULE_BASE_ADDR | 0x00AC)
#define LCD_PANEL_SIZE_REG		    (LCD_MODULE_BASE_ADDR | 0x00B0)
#define LCD_REG_CONFIG_REG		    (LCD_MODULE_BASE_ADDR | 0x00B4)
#define LCD_LCD_GO_REG			    (LCD_MODULE_BASE_ADDR | 0x00B8)
#define LCD_LCD_STATUS			    (LCD_MODULE_BASE_ADDR | 0x00BC)
#define LCD_SOFTWARE_CTL_REG		    (LCD_MODULE_BASE_ADDR | 0x00C8)
#define LCD_TVOUT_CTL_REG		    (LCD_MODULE_BASE_ADDR | 0x00CC)
#define LCD_CLK_CTL_REG			    (LCD_MODULE_BASE_ADDR | 0x00E8)

/** @{@name LCD config define
 *	Define LCD controller config value and bit map
 *	
 */
#define MAIN_LCD_MPU_CMD				0x00000000	//master LCD command
#define MAIN_LCD_MPU_DATA	    		0x00080000	//master LCD data
#define SUB_LCD_MPU_CMD		    		0x00040000	//slaver LCD command
#define SUB_LCD_MPU_DATA				0x000C0000	//slaver LCD data
/** @} */

/** @{@name IMAGE module register and bit map define
 */
#define IMG_CMD_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0000)
#define IMG_HINFO1_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0004)
#define IMG_HINFO2_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0008)
#define IMG_VINFO1_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x000C)
#define IMG_VINFO2_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0010)
                        				
#define IMG_YADDR_ODD					(IMAGE_MODULE_BASE_ADDR | 0x0018)
#define IMG_UADDR_ODD					(IMAGE_MODULE_BASE_ADDR | 0x001c)
#define IMG_VADDR_ODD					(IMAGE_MODULE_BASE_ADDR | 0x0020)
#define IMG_RGBADDR_ODD					(IMAGE_MODULE_BASE_ADDR | 0x0024)
#define IMG_YADDR_EVE					(IMAGE_MODULE_BASE_ADDR | 0x0028)
#define IMG_UADDR_EVE					(IMAGE_MODULE_BASE_ADDR | 0x002c)
#define IMG_VADDR_EVE					(IMAGE_MODULE_BASE_ADDR | 0x0030)
#define IMG_RGBADDR_EVE					(IMAGE_MODULE_BASE_ADDR | 0x0034)
#define IMG_CONFIG_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0040)
#define IMG_STATUS_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0060)
#define IMG_NUM_ADDR					(IMAGE_MODULE_BASE_ADDR | 0x0080)

#define MUL_FUN_CTL_REG					(CHIP_CONF_BASE_ADDR | 0x0058)

/** @{@name TIMER module register and bit map define
 */
#define TIMER1_CTRL_REG					(TIMER_MODULE_BASE_ADDR + 0x18)
#define TIMER2_CTRL_REG					(TIMER_MODULE_BASE_ADDR + 0x1c)
#define TIMER3_CTRL_REG					(TIMER_MODULE_BASE_ADDR + 0x20)
#define TIMER4_CTRL_REG					(TIMER_MODULE_BASE_ADDR + 0x24)
#define TIMER5_CTRL_REG					(TIMER_MODULE_BASE_ADDR + 0x28)	
/** @} */

/** @{@name Nandflash Controller Define 
 *	Define the register and bit map of nandflash controller
 *	
 */

// ECC
#define FLASH_ECC_REG0					0xB002b000	// encode mudule start address
#define FLASH_ECC_REPAIR_REG0			(FLASH_ECC_REG0+0x4)
#define FLASH_ECC_REPAIR_REG1			(FLASH_ECC_REG0+0x8)
#define FLASH_ECC_REPAIR_REG2			(FLASH_ECC_REG0+0xc)
#define FLASH_ECC_REPAIR_REG3			(FLASH_ECC_REG0+0x10)
#define FLASH_ECC_ENCODE_REG1			(FLASH_ECC_REG0+0x14)
#define FLASH_ECC_ENCODE_REG2			(FLASH_ECC_REG0+0x18)
#define FLASH_ECC_ENCODE_REG3			(FLASH_ECC_REG0+0x1C)
#define FLASH_FS_REG0					(FLASH_ECC_REG0+0x20)
#define FLASH_FS_REG1					(FLASH_ECC_REG0+0x24)

#define ECC_ENCODE_READY				(0x1<<23)
#define ECC_DECODE_READY				(0x1<<24)
#define ECC_CHECK_NO_ERROR				(0x1<<26)
#define ECC_ERROR_CAN_NOT_REPAIR		(0x1<<25)
#define ECC_ERROR_REPAIR_CAN_TRUST		(0x0<<27)
#define ECC_ERROR_REPAIR_CAN_NOT_TRUST	(0x1<<27)

#define ECC_POSITION_0_VALID			(0x1<<28)
#define ECC_POSITION_1_VALID			(0x1<<29)
#define ECC_POSITION_2_VALID			(0x1<<30)
#define ECC_POSITION_3_VALID			(0x1UL<<31)

#define DATA_ECC_CHECK_OK							1
#define DATA_ECC_CHECK_ERROR						0
#define DATA_ECC_ERROR_REPAIR_CAN_TRUST				2
#define DATA_ECC_ERROR_REPAIR_CAN_NOT_TRUST			3

// NF controller
#define FLASH_CTRL_REG0					(0xB002a100)
#define FLASH_CTRL_REG20				(FLASH_CTRL_REG0+0x50)
#define FLASH_CTRL_REG21				(FLASH_CTRL_REG0+0x54)
#define FLASH_CTRL_REG22				(FLASH_CTRL_REG0+0x58)
#define FLASH_CTRL_REG23				(FLASH_CTRL_REG0+0x5c)
#define FLASH_CTRL_REG24				(FLASH_CTRL_REG0+0x60)

#define NAND_DATA_RECE_SEND_FINISH 		(0x1<<6)

/** @} */


/** @{@name RTC module register and bit map define
 */
#define RTC_CONFIG_REG					(RTC_MODULE_BASE_ADDR + 0x50)	// rtc confgiuration
#define RTC_BACK_DAT_REG				(RTC_MODULE_BASE_ADDR + 0x54)	// rtc read back data
#define RTC_CLOCK_DIV_REG				(RTC_MODULE_BASE_ADDR + 0x04)	// enable/disable wakeup function
#define RTC_WAKEUP_GPIO_P_REG			(RTC_MODULE_BASE_ADDR + 0x3C)	// wakeup GPIO polarity
#define RTC_WAKEUP_GPIO_C_REG			(RTC_MODULE_BASE_ADDR + 0x40)	// clear wakeup GPIO status
#define RTC_WAKEUP_GPIO_E_REG			(RTC_MODULE_BASE_ADDR + 0x44)	// enable wake-up GPIO wakeup function
#define RTC_WAKEUP_GPIO_S_REG			(RTC_MODULE_BASE_ADDR + 0x48)	// wakeup GPIO status

/** @} */

/** @{@name CRC module register and bit map define
 */
#define CRC_CONFIG_REG					(0xB002c064)	//special register?
#define CRC_POLY_LEN_REG				(CRC_MODULE_BASE_ADDR + 0xAC) //polynomial length
#define CRC_COEFF_CON_REG				(CRC_MODULE_BASE_ADDR + 0xB0) //conefficient configuration
#define CRC_RESULT_REG					(CRC_MODULE_BASE_ADDR + 0xD0) //CRC result
/** @} */


/*@}*/

/** @{@name Register Operation Define
 *	Define the macro for read/write register and memory
 */
#define HAL_READ_UINT8( _register_, _value_ )        ((_value_) = *((volatile T_U8 *)(_register_)))
#define HAL_WRITE_UINT8( _register_, _value_ )       (*((volatile T_U8 *)(_register_)) = (_value_))
#define HAL_READ_UINT16( _register_, _value_ )      ((_value_) = *((volatile T_U16 *)(_register_)))
#define HAL_WRITE_UINT16( _register_, _value_ )     (*((volatile T_U16 *)(_register_)) = (_value_))
#define HAL_READ_UINT32( _register_, _value_ )      ((_value_) = *((volatile T_U32 *)(_register_)))
#define HAL_WRITE_UINT32( _register_, _value_ )     (*((volatile T_U32 *)(_register_)) = (_value_))

//	#define REG32(_register_)   (*(volatile T_U32 *)(_register_))
//	#define REG16(_register_)   (*(volatile T_U16 *)(_register_))
//	#define REG8(_register_)   (*(volatile T_U8 *)(_register_))

//read and write register
#define outb(v,p)			(*(volatile unsigned char *)(p) = (v))
#define outw(v,p)			(*(volatile unsigned short *)(p) = (v))
#define outl(v,p)			(*(volatile unsigned long  *)(p) = (v))

#define inb(p)			(*(volatile unsigned char *)(p))
#define inw(p)			(*(volatile unsigned short *)(p))
#define inl(p)				(*(volatile unsigned long *)(p))
#define WriteBuf(v,p)		(*(volatile unsigned long  *)(p) = (v))
#define ReadBuf(p)		(*(volatile unsigned long *)(p))

#define WriteRamb(v,p)	(*(volatile unsigned char *)(p) = (v))
#define WriteRamw(v,p)	(*(volatile unsigned short *)(p) = (v))
#define WriteRaml(v,p)	(*(volatile unsigned long  *)(p) = (v))

#define ReadRamb(p)		(*(volatile unsigned char *)(p))
#define ReadRamw(p)		(*(volatile unsigned short *)(p))
#define ReadRaml(p)		(*(volatile unsigned long *)(p))


//error type define
#define	UNDEF_ERROR		1
#define	ABT_ERROR		2
#define	PREF_ERROR		3

/** @} */
/*@}*/

#endif	// _ANYKA_CPU_H_
