/** @file
 * @brief Define the register of ANYKA CPU
 *
 * Define the register address and bit map for system.
 * Copyright (C) 2006 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author 
 * @date 2006-01-16
 * @version 1.0
 * @note CPU AK7802M
 */
//#if ((defined CHIP_AK7801) || (defined CHIP_AK7802))

#ifndef _ANYKA_CPU_780X_H_
#define _ANYKA_CPU_780X_H_

/** @defgroup ANYKA_CPU_7802  
 *	@ingroup M3PLATFORM
 */
/*@{*/


/** @{@name System Control Register
 *	Define system control register here, include CLOCK/INT/RESET
 */
#define CLOCK_CTRL_REG				(CHIP_CONF_BASE_ADDR + 0x0000000C)	// module clock control(switch)
#define CLOCK_CTRL2_REG				(CHIP_CONF_BASE_ADDR + 0x00000008)	// module clock control(switch)
#define RESET_CTRL_REG				(CHIP_CONF_BASE_ADDR + 0x0000000C)	// module software reset control register
#define STANDBY_REG					(CHIP_CONF_BASE_ADDR + 0x00000004)	// module standby register

#define IRQINT_MASK_REG				(CHIP_CONF_BASE_ADDR + 0x00000034)	// module IRQ interrupt mask register, 1: mask; 0:unmask(default);
#define FRQINT_MASK_REG				(CHIP_CONF_BASE_ADDR + 0x00000038)	// module FRQ interrupt mask register, 1: mask; 0:unmask(default);
#define INT_STATUS_REG				(CHIP_CONF_BASE_ADDR + 0x000000CC)	// module interrupt status register
#define INT_SYS_MODULE_REG          (CHIP_CONF_BASE_ADDR + 0x0000004C)	// system module interrupt control register
#define CLOCK_DIV_REG				(CHIP_CONF_BASE_ADDR + 0x00000004)	// clock divider register 1
#define CLOCK2X_CTRL_REG			(CHIP_CONF_BASE_ADDR + 0x00000004)	// clock2x control register,1: 2*ASIC clock; 0: ASIC clock
#define CLOCK3X_CTRL_REG			(CHIP_CONF_BASE_ADDR + 0x00000064)	//bit28 => 1: asic = pll1_clock /3, 0: refresh to bit[6..8] of 0x08000004 register 
#define PLL_NPARAM_REG				(CHIP_CONF_BASE_ADDR + 0x000000dc)	// n configuration register,PLL_Clk = 4*M/N

#define PLL_CHANGE_ENA				(1 << 12)
#define PLL_NPARAM					((1 << 12) | (1 << 13) | (1 << 14) | (1 << 15))

/* clock divider register */
#define CLOCK_DIV_ASIC_DIV          ((1 << 8)|(1 << 7)|(1 << 6))
#define CLOCK_DIV_DIV_ENA           (1 << 14)
/** @} */

//L2 bits
#define FRAC_DMA_DIRECT_WRITE	    (1<<8)
#define FRAC_DMA_DIRECT_READ	    (0<<8)
#define FRAC_DMA_START_REQ		    (1<<9)

/** @{@name UART module register and bit map define
 */
#define UART0_BASE_ADDR			    (UART_MODULE_BASE_ADDR + 0x00000000)
#define UART1_BASE_ADDR				(UART_MODULE_BASE_ADDR + 0x00001000)
#define UART2_BASE_ADDR				(UART_MODULE_BASE_ADDR + 0x00002000)
#define UART3_BASE_ADDR				(UART_MODULE_BASE_ADDR + 0x00003000)

#define	UART_CFG_REG1				(0x00)
#define	UART_CFG_REG2				(0x04)
#define	UART_DATA_CFG				(0x08)
#define	UART_RX_THREINT				(0x0c)

/** @} */


/** @{@name GPIO module register and bit map define
 */
//gpio direction
#define GPIO_DIR_REG1               (GPIO_MODULE_BASE_ADDR + 0x7c)
#define GPIO_DIR_REG2               (GPIO_MODULE_BASE_ADDR + 0x84)
#define GPIO_DIR_REG3               (GPIO_MODULE_BASE_ADDR + 0x8c)
#define GPIO_DIR_REG4               (GPIO_MODULE_BASE_ADDR + 0x94)
#define REG_GPIO_DIR1				GPIO_DIR_REG1
#define REG_GPIO_DIR2				GPIO_DIR_REG2
#define REG_GPIO_DIR3				GPIO_DIR_REG3
#define REG_GPIO_DIR4				GPIO_DIR_REG4
	
//gpio output control
#define GPIO_OUT_REG1               (GPIO_MODULE_BASE_ADDR + 0x80)
#define GPIO_OUT_REG2               (GPIO_MODULE_BASE_ADDR + 0x88)
#define GPIO_OUT_REG3               (GPIO_MODULE_BASE_ADDR + 0x90)
#define GPIO_OUT_REG4               (GPIO_MODULE_BASE_ADDR + 0x98)
#define REG_GPIO_OUT1				GPIO_OUT_REG1
#define REG_GPIO_OUT2				GPIO_OUT_REG2
#define REG_GPIO_OUT3				GPIO_OUT_REG3
#define REG_GPIO_OUT4				GPIO_OUT_REG4
	
//gpio input control
#define GPIO_IN_REG1                (GPIO_MODULE_BASE_ADDR + 0xbc)
#define GPIO_IN_REG2                (GPIO_MODULE_BASE_ADDR + 0xc0)
#define GPIO_IN_REG3                (GPIO_MODULE_BASE_ADDR + 0xc4)
#define GPIO_IN_REG4                (GPIO_MODULE_BASE_ADDR + 0xc8)
#define REG_GPIO_IN1				GPIO_IN_REG1
#define REG_GPIO_IN2				GPIO_IN_REG2
#define REG_GPIO_IN3				GPIO_IN_REG3
#define REG_GPIO_IN4				GPIO_IN_REG4
	
//gpio interrupt enable/disable
#define GPIO_INT_EN1                (GPIO_MODULE_BASE_ADDR + 0xe0)
#define GPIO_INT_EN2                (GPIO_MODULE_BASE_ADDR + 0xe4)
#define GPIO_INT_EN3                (GPIO_MODULE_BASE_ADDR + 0xe8)
#define GPIO_INT_EN4                (GPIO_MODULE_BASE_ADDR + 0xec)
#define REG_GPIO_INT_CTL1			GPIO_INT_EN1
#define REG_GPIO_INT_CTL2			GPIO_INT_EN2
#define REG_GPIO_INT_CTL3			GPIO_INT_EN3
#define REG_GPIO_INT_CTL4			GPIO_INT_EN4
	
//gpio interrupt sensitivity level
#define GPIO_INT_LEVEL_REG1         (GPIO_MODULE_BASE_ADDR + 0xf0)
#define GPIO_INT_LEVEL_REG2	        (GPIO_MODULE_BASE_ADDR + 0xf4)
#define GPIO_INT_LEVEL_REG3	        (GPIO_MODULE_BASE_ADDR + 0xf8)
#define GPIO_INT_LEVEL_REG4	        (GPIO_MODULE_BASE_ADDR + 0xfc)
#define REG_GPIO_INT_POL1			GPIO_INT_LEVEL_REG1
#define REG_GPIO_INT_POL2			GPIO_INT_LEVEL_REG2
#define REG_GPIO_INT_POL3			GPIO_INT_LEVEL_REG3
#define REG_GPIO_INT_POL4			GPIO_INT_LEVEL_REG4
	
//gpio pull/pulldown reg
#define GPIO_PULLUPDOWN_REG1        (GPIO_MODULE_BASE_ADDR + 0x9c)
#define GPIO_PULLUPDOWN_REG2        (GPIO_MODULE_BASE_ADDR + 0xa0)
#define GPIO_PULLUPDOWN_REG3        (GPIO_MODULE_BASE_ADDR + 0xa4)
#define GPIO_PULLUPDOWN_REG4        (GPIO_MODULE_BASE_ADDR + 0xa8)
#define REG_GPIO_UPDOWN1			GPIO_PULLUPDOWN_REG1
#define REG_GPIO_UPDOWN2			GPIO_PULLUPDOWN_REG2
#define REG_GPIO_UPDOWN3			GPIO_PULLUPDOWN_REG3
#define REG_GPIO_UPDOWN4			GPIO_PULLUPDOWN_REG4
	
//io control reg
#define GPIO_IO_CONTROL_REG1        (GPIO_MODULE_BASE_ADDR + 0xD4)
#define GPIO_IO_CONTROL_REG2        (GPIO_MODULE_BASE_ADDR + 0xD8)

//share pin control reg
#define GPIO_SHAREPIN_CONTROL2	    (GPIO_MODULE_BASE_ADDR + 0x74)
#define GPIO_SHAREPIN_CONTROL1	    (GPIO_MODULE_BASE_ADDR + 0x78)
#define REG_GPIO_SEL1				GPIO_SHAREPIN_CONTROL1
#define REG_GPIO_SEL2				GPIO_SHAREPIN_CONTROL2


/** @} */

/** @{@name GPIO module register and bit map define
 */
//timer read back register

#define TIMER1_READ_BACK_REG        (0xB8000100)
#define TIMER2_READ_BACK_REG        (0xB8000104)
#define TIMER3_READ_BACK_REG        (0xB8000108)
#define TIMER4_READ_BACK_REG        (0xB800010c)
#define TIMER5_READ_BACK_REG        (0xB8000110)


/** @} */




/** @{@name ANALOG module register and bit map define
 */
#define ADC_CONTROL1			    (ADC_MODULE_BASE_ADDR + 0x60)
#define ADC_CLK_DIV			        (ADC_MODULE_BASE_ADDR + 0x08)
#define ANLSOFT_CONTROL		        (ADC_MODULE_BASE_ADDR + 0x34)
#define USB_CONTROL			        (ADC_MODULE_BASE_ADDR + 0x44)
#define X_COORDINATE_REG            (ADC_MODULE_BASE_ADDR + 0x68)
#define Y_COORDINATE_REG            (ADC_MODULE_BASE_ADDR + 0x6c)
#define TS_CONTROL_REG2         	(ADC_MODULE_BASE_ADDR + 0x64)
#define ADC1_STAT_REG		        (ADC_MODULE_BASE_ADDR + 0x70)
#define TS_CONTROL_REG1		        (ADC_MODULE_BASE_ADDR + 0x5c)


//following macro is 7802 specific
#define PWM_CTRL_REG1	    	    (ADC_MODULE_BASE_ADDR + 0x2c)
#define PWM_CTRL_REG2	    	    (ADC_MODULE_BASE_ADDR + 0x30)


//following macro is 322L specific
#define TS_CONTROL_REG		        TS_CONTROL_REG1
//#define PWM_CTRL_REG1	    	    (ADC_MODULE_BASE_ADDR + 0x70)
//#define PWM_CTRL_REG2	    	    (ADC_MODULE_BASE_ADDR + 0x74)
#define ANALOG_CONTROL1			    (ADC_MODULE_BASE_ADDR + 0x20)
#define ANALOG_CONTROL2			    (ADC_MODULE_BASE_ADDR + 0x30)
#define DAC_CONTROL				    (ADC_MODULE_BASE_ADDR + 0x30)

/** @} */

//*******************************************************************
/** @{@name USB register and bit map define
 *  Define register to control USB port and the bit map of the register
 */
 /** System register*/
#define USB_CONTROL_REG             (0xB8000058)

#define USB_SUS_EN                  (1 << 2)
#define USB_REG_EN                  (1 << 1)

/** USB controller register*/
#define USB_FIFO_EP0                (USB_BASE_ADDR + 0x0020)

#define USB_REG_FADDR               (USB_BASE_ADDR + 0x0000)
#define USB_REG_POWER               (USB_BASE_ADDR + 0x0001)
#define USB_REG_INTRTX1             (USB_BASE_ADDR + 0x0002)
#define USB_REG_INTRTX2             (USB_BASE_ADDR + 0x0003)
#define USB_REG_INTRRX1             (USB_BASE_ADDR + 0x0004)
#define USB_REG_INTRRX2           	(USB_BASE_ADDR + 0x0005)
#define USB_REG_INTRTX1E          	(USB_BASE_ADDR + 0x0006)
#define USB_REG_INTRTX2E          	(USB_BASE_ADDR + 0x0007)
#define USB_REG_INTRRX1E          	(USB_BASE_ADDR + 0x0008)
#define USB_REG_INTRRX2E          	(USB_BASE_ADDR + 0x0009)
#define USB_REG_INTRUSB           	(USB_BASE_ADDR + 0x000A)
#define USB_REG_INTRUSBE          	(USB_BASE_ADDR + 0x000B)
#define USB_REG_FRAME1            	(USB_BASE_ADDR + 0x000C)
#define USB_REG_FRAME2            	(USB_BASE_ADDR + 0x000D)
#define USB_REG_INDEX             	(USB_BASE_ADDR + 0x000E)
#define USB_REG_TESEMODE          	(USB_BASE_ADDR + 0x000F)
#define USB_REG_DEVCTL            	(USB_BASE_ADDR + 0x0060)
#define USB_REG_TXMAXP0           	(USB_BASE_ADDR + 0x0010)
#define USB_REG_TXMAXP1           	(USB_BASE_ADDR + 0x0010)
#define USB_REG_CSR0              	(USB_BASE_ADDR + 0x0012)
#define USB_REG_TXCSR1            	(USB_BASE_ADDR + 0x0012)
#define USB_REG_CSR02             	(USB_BASE_ADDR + 0x0013)
#define USB_REG_TXCSR2            	(USB_BASE_ADDR + 0x0013)
#define USB_REG_RXMAXP1           	(USB_BASE_ADDR + 0x0014)
#define USB_REG_RXMAXP2           	(USB_BASE_ADDR + 0x0015)
#define USB_REG_RXCSR1            	(USB_BASE_ADDR + 0x0016)
#define USB_REG_RXCSR2            	(USB_BASE_ADDR + 0x0017)
#define USB_REG_COUNT0            	(USB_BASE_ADDR + 0x0018)
#define USB_REG_RXCOUNT1          	(USB_BASE_ADDR + 0x0018)
#define USB_REG_RXCOUNT2          	(USB_BASE_ADDR + 0x0019)
#define USB_REG_TXTYPE            	(USB_BASE_ADDR + 0x001A)
#define USB_REG_RXTYPE            	(USB_BASE_ADDR + 0x001C)
#define USB_REG_RXINTERVAL        	(USB_BASE_ADDR + 0x001D)
#define USB_REG_NAKLIMIT0         	(USB_BASE_ADDR + 0x001B)
                                  	
#define	USB_EP0_TX_COUNT		  	(USB_BASE_ADDR + 0x0330)
#define	USB_EP2_TX_COUNT		  	(USB_BASE_ADDR + 0x0334)
                                  	
#define	USB_FORBID_WRITE_REG	  	(USB_BASE_ADDR + 0x0338)
                                  	
#define	USB_START_PRE_READ_REG	  	(USB_BASE_ADDR + 0x033C)
#define	USB_FS_SPEED_REG		  	(USB_BASE_ADDR + 0x0344)


#define USB_FS_HOST_BASE_ADDR       0xB7000800

#define USB_FS_HOST_REG_FADDR               (USB_FS_HOST_BASE_ADDR + 0x0000)
#define USB_FS_HOST_REG_POWER               (USB_FS_HOST_BASE_ADDR + 0x0001)
#define USB_FS_HOST_REG_INTRTX1             (USB_FS_HOST_BASE_ADDR + 0x0002)
#define USB_FS_HOST_REG_INTRTX2             (USB_FS_HOST_BASE_ADDR + 0x0003)
#define USB_FS_HOST_REG_INTRRX1             (USB_FS_HOST_BASE_ADDR + 0x0004)
#define USB_FS_HOST_REG_INTRRX2           	(USB_FS_HOST_BASE_ADDR + 0x0005)
#define USB_FS_HOST_REG_INTRTX1E          	(USB_FS_HOST_BASE_ADDR + 0x0006)
#define USB_FS_HOST_REG_INTRTX2E          	(USB_FS_HOST_BASE_ADDR + 0x0007)
#define USB_FS_HOST_REG_INTRRX1E          	(USB_FS_HOST_BASE_ADDR + 0x0008)
#define USB_FS_HOST_REG_INTRRX2E          	(USB_FS_HOST_BASE_ADDR + 0x0009)
#define USB_FS_HOST_REG_INTRUSB           	(USB_FS_HOST_BASE_ADDR + 0x000A)
#define USB_FS_HOST_REG_INTRUSBE          	(USB_FS_HOST_BASE_ADDR + 0x000B)
#define USB_FS_HOST_REG_FRAME1            	(USB_FS_HOST_BASE_ADDR + 0x000C)
#define USB_FS_HOST_REG_FRAME2            	(USB_FS_HOST_BASE_ADDR + 0x000D)
#define USB_FS_HOST_REG_INDEX             	(USB_FS_HOST_BASE_ADDR + 0x000E)
#define USB_FS_HOST_REG_TESEMODE          	(USB_FS_HOST_BASE_ADDR + 0x000F)
#define USB_FS_HOST_REG_DEVCTL            	(USB_FS_HOST_BASE_ADDR + 0x0060)
#define USB_FS_HOST_REG_TXMAXP0           	(USB_FS_HOST_BASE_ADDR + 0x0010)
#define USB_FS_HOST_REG_TXMAXP1           	(USB_FS_HOST_BASE_ADDR + 0x0010)
#define USB_FS_HOST_REG_CSR0              	(USB_FS_HOST_BASE_ADDR + 0x0012)
#define USB_FS_HOST_REG_TXCSR1            	(USB_FS_HOST_BASE_ADDR + 0x0012)
#define USB_FS_HOST_REG_CSR02             	(USB_FS_HOST_BASE_ADDR + 0x0013)
#define USB_FS_HOST_REG_TXCSR2            	(USB_FS_HOST_BASE_ADDR + 0x0013)
#define USB_FS_HOST_REG_RXMAXP1           	(USB_FS_HOST_BASE_ADDR + 0x0014)
#define USB_FS_HOST_REG_RXMAXP2           	(USB_FS_HOST_BASE_ADDR + 0x0015)
#define USB_FS_HOST_REG_RXCSR1            	(USB_FS_HOST_BASE_ADDR + 0x0016)
#define USB_FS_HOST_REG_RXCSR2            	(USB_FS_HOST_BASE_ADDR + 0x0017)
#define USB_FS_HOST_REG_COUNT0            	(USB_FS_HOST_BASE_ADDR + 0x0018)
#define USB_FS_HOST_REG_RXCOUNT1          	(USB_FS_HOST_BASE_ADDR + 0x0018)
#define USB_FS_HOST_REG_RXCOUNT2          	(USB_FS_HOST_BASE_ADDR + 0x0019)
#define USB_FS_HOST_REG_TXTYPE            	(USB_FS_HOST_BASE_ADDR + 0x001A)
#define USB_FS_HOST_REG_RXTYPE            	(USB_FS_HOST_BASE_ADDR + 0x001C)
#define USB_FS_HOST_REG_RXINTERVAL        	(USB_FS_HOST_BASE_ADDR + 0x001D)
#define USB_FS_HOST_REG_NAKLIMIT0         	(USB_FS_HOST_BASE_ADDR + 0x001B)
#define USB_FS_HOST_REG_FIFOSIZE          	(USB_FS_HOST_BASE_ADDR + 0x001F)


#define USB_L2_CONTROL_FIFO       	(0xBC001500)

#define USB_FS_FIFO_EP0              0xB7000820
#define USB_FS_FIFO_EP1              0xB7000824
#define USB_FS_FIFO_EP2              0xB7000828
#define USB_FS_FIFO_EP3              0xB700082c 


/**  USB DMA */                   	
#define USB_FS_HOST_DMA_INTR              	(USB_FS_HOST_BASE_ADDR + 0x0200)
#define USB_FS_HOST_DMA_CNTL_1            	(USB_FS_HOST_BASE_ADDR + 0x0204)
#define USB_FS_HOST_DMA_ADDR_1            	(USB_FS_HOST_BASE_ADDR + 0x0208)
#define USB_FS_HOST_DMA_COUNT_1           	(USB_FS_HOST_BASE_ADDR + 0x020c)
#define USB_FS_HOST_DMA_CNTL_2            	(USB_FS_HOST_BASE_ADDR + 0x0214)
#define USB_FS_HOST_DMA_ADDR_2            	(USB_FS_HOST_BASE_ADDR + 0x0218)
#define USB_FS_HOST_DMA_COUNT_2           	(USB_FS_HOST_BASE_ADDR + 0x021c)

/**Dynamic FIFO sizing   JUST FOR HOST  */
#define USB_REG_TXFIFO1           	(USB_BASE_ADDR + 0x001C)
#define USB_REG_TXFIFO2           	(USB_BASE_ADDR + 0x001D)
#define USB_REG_RXFIFO1           	(USB_BASE_ADDR + 0x001E)
#define USB_REG_RXFIFO2           	(USB_BASE_ADDR + 0x001F)
                                  	
#define USB_REG_FIFOSIZE          	(USB_BASE_ADDR + 0x001F)
                                  	
/**  USB DMA */                   	
#define USB_DMA_INTR              	(USB_BASE_ADDR + 0x0200)
#define USB_DMA_CNTL_1            	(USB_BASE_ADDR + 0x0204)
#define USB_DMA_ADDR_1            	(USB_BASE_ADDR + 0x0208)
#define USB_DMA_COUNT_1           	(USB_BASE_ADDR + 0x020c)
#define USB_DMA_CNTL_2            	(USB_BASE_ADDR + 0x0214)
#define USB_DMA_ADDR_2            	(USB_BASE_ADDR + 0x0218)
#define USB_DMA_COUNT_2           	(USB_BASE_ADDR + 0x021c)
                                  	
#define	USB_FS_SPEED_REG		  	(USB_BASE_ADDR + 0x0344)


#define	USB_EP0_FORBID_WRITE	  	(0x1)
#define	USB_EP2_FORBID_WRITE  	  	(0x2)
#define	USB_EP4_FORBID_WRITE	  	(0x4)
                                  	
#define	USB_EP0_START_PRE_READ	  	(0x1)
#define	USB_EP2_START_PRE_READ	  	(0x2)
#define	USB_EP4_START_PRE_READ	  	(0x4)



/** CSR0 bit masks mode-agnostic */
#define USB_CSR0_RXPKTRDY         	(1 << 0)
#define USB_CSR0_TXPKTRDY         	(1 << 1)

/** CSR0 bit masks IN SLAVE MODE */
#define USB_CSR0_P_SENTSTALL      	(1 << 2)
#define USB_CSR0_P_DATAEND        	(1 << 3)
#define USB_CSR0_P_SETUPEND       	(1 << 4)
#define USB_CSR0_P_SENDSTALL      	(1 << 5)
#define USB_CSR0_P_SVDRXPKTRDY    	(1 << 6)
#define USB_CSR0_P_SVDSETUPEND    	(1 << 7)

/**  CSR0 bit masks in MASTER mode   */
#define USB_CSR0_H_RXSTALL        	(1 << 2)
#define USB_CSR0_H_SETUPPKT       	(1 << 3)
#define USB_CSR0_H_ERROR          	(1 << 4)
#define USB_CSR0_H_REQPKT         	(1 << 5)
#define USB_CSR0_H_STATUSPKT      	(1 << 6)
#define USB_CSR0_H_NAKTIMEOUT     	(1 << 7)


#define USB_CSR0_FLUSHFIFO        	(0x1)


/** TXCSR1 bit masks mode-agnostic */
#define USB_TXCSR1_TXPKTRDY       	(1 << 0)
#define USB_TXCSR1_FIFONOTEMPTY   	(1 << 1)
#define USB_TXCSR1_FLUSHFIFO      	(1 << 3)
#define USB_TXCSR1_CLRDATATOG     	(1 << 6)

/** TXCSR1 bit masks slave mode */
#define USB_TXCSR1_P_UNDERRUN     	(1 << 2)
#define USB_TXCSR1_P_SENDSTALL    	(1 << 4)
#define USB_TXCSR1_P_SENTSTALL    	(1 << 5)

/** TXCSR1 bit masks MASTER MODE */
#define USB_TXCSR1_H_ERROR        	(0x04)
#define USB_TXCSR1_H_RXSTALL      	(0x20)

/** RXCSR1 bit masks mode-agnostic */
#define USB_RXCSR1_RXPKTRDY       	(1 << 0)
#define USB_RXCSR1_FIFOFULL       	(1 << 1)
#define USB_RXCSR1_NAKTIMEOUT     	(1 << 3)
#define USB_RXCSR1_FLUSHFIFO      	(1 << 4)
#define USB_RXCSR1_CLRDATATOG     	(1 << 7)

/** RXCSR1 bit masks slave mode */
#define USB_RXCSR1_P_OVERRUN      	(1 << 2) //0x04
#define USB_RXCSR1_P_SENDSTALL    	(1 << 5) //0x20
#define USB_RXCSR1_P_SENTSTALL    	(1 << 6) //0x40

/** RXCSR1 bit masks MASTER MODE */
#define USB_RXCSR1_H_ERROR        	(1 << 2) //0x04
#define USB_RXCSR1_H_REQPKT       	(1 << 5) //0x20
#define USB_RXCSR1_H_RXSTALL      	(1 << 6) //0x40

/** TXCSR2 bit masks** THERE IS NO DIFFERENCE FOR MASTER AND SLAVE MODE */
#define USB_TXCSR2_FRCDT          	(1 << 3)
#define USB_TXCSR2_DMAMODE        	(1 << 2)
#define USB_TXCSR2_DMAENAB        	(1 << 4)
#define USB_TXCSR2_MODE           	(1 << 5)
#define USB_TXCSR2_ISO            	(1 << 6)
#define USB_TXCSR2_AUTOSET        	(1 << 7)

/** RXCSR2 bit masks */
#define USB_RXCSR2_AUTOCLEAR      	(1 << 7)
#define USB_RXCSR2_DMAENAB        	(1 << 5)
#define USB_RXCSR2_DISNYET        	(1 << 4)
#define USB_RXCSR2_DMAMODE        	(1 << 3)
#define USB_RXCSR2_INCOMPRX       	(1 << 0)


/** POWER REGISTER  */
#define USB_POWER_ENSUSPEND       	(1 << 0)
#define USB_POWER_SUSPENDM        	(1 << 1)
#define USB_POWER_RESUME          	(1 << 2)
#define USB_POWER_RESET           	(1 << 3)
#define USB_POWER_HSMODE          	(1 << 4)
#define USB_POWER_HSENABLE        	(1 << 5)
#define USB_POWER_SOFTCONN        	(1 << 6)
#define USB_POWER_ISOUP           	(1 << 7)


/** EPn ENABLE */
#define USB_EP0_ENABLE            	(1 << 0)
#define USB_EP2_TX_ENABLE         	(1 << 2)
#define USB_EP4_TX_ENABLE         	(1 << 4)
                                  	
#define USB_EP1_RX_ENABLE         	(1 << 1)
#define USB_EP3_RX_ENABLE         	(1 << 3)
#define USB_EP4_RX_ENABLE         	(1 << 4)


#define USB_EP0_INDEX             	(0)
#define USB_EP1_INDEX             	(1 << 0)
#define USB_EP2_INDEX             	(1 << 1)
#define USB_EP3_INDEX             	((1 << 1)|(1 << 0))
#define USB_EP4_INDEX             	(1 << 2)
#define USB_EP5_INDEX             	((1 << 2)|(1 << 0))
#define USB_EP6_INDEX             	((1 << 2)|(1 << 1))
#define USB_EP7_INDEX             	((1 << 2)|(1 << 1)|(1 << 0))


/** EP INTERRUPT in INTR register */
#define USB_INTR_EP0              	(1 << 0)
#define USB_INTR_EP1              	(1 << 1)
#define USB_INTR_EP2              	(1 << 2)
#define USB_INTR_EP3              	(1 << 3)
#define USB_INTR_EP4              	(1 << 4)
#define USB_INTR_EP5              	(1 << 5)
#define USB_INTR_EP6              	(1 << 6)
#define USB_INTR_EP7              	(1 << 7)


/** dma control register */
#define USB_CTL_DMA_ENA           	(1 << 0)
#define USB_CTL_DMA_DIRE_IN       	(1 << 1)
#define USB_CTL_DMA_MODE          	(1 << 2)
#define USB_CTL_DMA_INT_ENA       	(1 << 3)
#define USB_CTL_DMA_BUS_ERR       	(1 << 7)

/* usb control and status register */
#define USB_REG_RXCSR1_RXSTALL    	(1 << 6)
#define USB_REG_RXCSR1_REQPKT     	(1 << 5)

#define USB_TXCSR_AUTOSET          	(0x80)
#define USB_TXCSR_ISO              	(0x40)
#define USB_TXCSR_MODE1            	(0x20)
#define USB_TXCSR_DMAREQENABLE     	(0x10)
#define USB_TXCSR_FRCDATATOG       	(0x8)
#define USB_TXCSR_DMAREQMODE1      	(0x4)
#define USB_TXCSR_DMAREQMODE0      	(0x0)


#define USB_RXCSR_AUTOSET          	(0x80)
#define USB_RXCSR_ISO              	(0x40)
#define USB_RXCSR_DMAREQENAB       	(0x20)
#define USB_RXCSR_DISNYET          	(0x10)
#define USB_RXCSR_DMAREQMODE1      	(0x8)
#define USB_RXCSR_DMAREQMODE0      	(0x0)
#define USB_RXCSR_INCOMPRX         	(0x1)

#define USB_ENABLE_DMA              (1)
#define USB_DIRECTION_RX            (0<<1)
#define USB_DIRECTION_TX            (1<<1)
#define USB_DMA_MODE1               (1<<2)
#define USB_DMA_MODE0               (0<<2)
#define USB_DMA_INT_ENABLE          (1<<3)
#define USB_DMA_INT_DISABLE         (0<<3)
#define USB_DMA_BUS_ERROR           (1<<8)
#define USB_DMA_BUS_MODE0           (0<<9)
#define USB_DMA_BUS_MODE1           (1<<9)
#define USB_DMA_BUS_MODE2           (2<<9)
#define USB_DMA_BUS_MODE3           (3<<9)

#define DMA_CHANNEL1_INT          	(1)
#define DMA_CHANNEL2_INT          	(2)
#define DMA_CHANNEL3_INT          	(4)
#define DMA_CHANNEL4_INT          	(8)
#define DMA_CHANNEL5_INT          	(0x10)

/**Interrupt register bit masks */
#define USB_INTR_SUSPEND          	(1 << 0)
#define USB_INTR_RESUME           	(1 << 1)
#define USB_INTR_RESET            	(1 << 2)
#define USB_INTR_SOF              	(1 << 3)
#define USB_INTR_CONNECT          	(1 << 4)
#define USB_INTR_DISCONNECT       	(1 << 5)
#define USB_INTR_SESSREQ          	(1 << 6)
#define USB_INTR_VBUSERROR        	(1 << 7)   /* #FOR SESSION END  */

/** Interrupt register bit enable */
#define USB_INTR_SUSPEND_ENA      	(1 << 0)
#define USB_INTR_RESUME_ENA       	(1 << 1)
#define USB_INTR_RESET_ENA        	(1 << 2)
#define USB_INTR_SOF_ENA          	(1 << 3)
#define USB_INTR_CONNECT_ENA      	(1 << 4)
#define USB_INTR_DISCONNECT_ENA   	(1 << 5)
#define USB_INTR_SESSREQ_ENA      	(1 << 6)
#define USB_INTR_VBUSERROR_ENA    	(1 << 7)   /* #FOR SESSION END  */

/**Reset mode */
#define RESET_CTRL_USB            	(1 << 21)
#define RESET_CTRL_USB_FS           (1 << 23)

/**usb drvvbus */
#define SHARE_PIN_GPIO_DRVVBUS      (1 << 3)


#define SDRAM_SELF_REFRESH_BIT    	(1 << 28)
/** @} */
/*@}*/


/***************************************************************************/

#define	ASPEN_L2_BUF_BASE			(0xBC000000)
#define	UART0_TXBUF_ADDR			(ASPEN_L2_BUF_BASE+0x1000)
#define	UART0_RXBUF_ADDR			(ASPEN_L2_BUF_BASE+0x1080)
#define	SET_FLAG_OFFSET0			(0x3c)
#define	SET_FLAG_OFFSET1			(0x7c)

/** @{@name SPI module register and bit map define
 */
#define SPI0_BASE_ADDR				(SPI_MODULE_BASE_ADDR + 0x00004000)
#define SPI1_BASE_ADDR				(SPI_MODULE_BASE_ADDR + 0x00005000)
 
#define	ASPEN_SPI_CTRL				(SPI0_BASE_ADDR+0x00)
#define	ASPEN_SPI_STA				(SPI0_BASE_ADDR+0x04)
#define	ASPEN_SPI_INTENA			(SPI0_BASE_ADDR+0x08)
#define	ASPEN_SPI_NBR				(SPI0_BASE_ADDR+0x0c)
#define	ASPEN_SPI_TX_EXBUF			(SPI0_BASE_ADDR+0x10)
#define	ASPEN_SPI_RX_EXBUF			(SPI0_BASE_ADDR+0x14)
#define	ASPEN_SPI_TX_INBUF			(SPI0_BASE_ADDR+0x18)
#define	ASPEN_SPI_RX_INBUF			(SPI0_BASE_ADDR+0x1c)
#define	ASPEN_SPI_RTIM				(SPI0_BASE_ADDR+0x20)
/** @} */

#define DMA_PRIORITY_CTRL_BASE_ADDR (0xB002d000)
#define DMA_PRIORITY_CTRL_REG1      (DMA_PRIORITY_CTRL_BASE_ADDR + 0x0000000c)
#define DMA_PRIORITY_CTRL_REG2      (DMA_PRIORITY_CTRL_BASE_ADDR + 0x00000010)
#define AHB_PRIORITY_CTRL_REG       (DMA_PRIORITY_CTRL_BASE_ADDR + 0x00000014)


#define CLK_RESET_ADDR          0xB800000C              // Clock Control & Soft Reset Control Register

#define GUI_SET_REG(reg_addr, value) (*((volatile T_U32 *)(reg_addr))) = (value)
#define GUI_GET_REG(reg_addr)        (*((volatile T_U32 *)(reg_addr)))

#define GUI_BASE_ADDR           0xB0022000              // gui register base address
#define GUI_SCALSRCADDR1_ADDR   (GUI_BASE_ADDR+0x108)   // Input image start address 1
#define GUI_SCALSRCADDR2_ADDR   (GUI_BASE_ADDR+0x10c) // Input image start address 2
#define GUI_SCALSRCADDR3_ADDR   (GUI_BASE_ADDR+0x110) // Input image start address 3
#define GUI_SCALSRCSTRD_ADDR    (GUI_BASE_ADDR+0x118)   // Input image line stride
#define GUI_SCALDSTADDR_ADDR    (GUI_BASE_ADDR+0x11c)   // Output image start address
#define GUI_SCALSRCRECT_ADDR    (GUI_BASE_ADDR+0x114)   // Input image rectangle dimensions
#define GUI_SCALDSTRECT_ADDR    (GUI_BASE_ADDR+0x120)   // Output image rectangle dimensions
#define GUI_SCALDSTSTRD_ADDR    (GUI_BASE_ADDR+0x124)   // Output image line stride
#define GUI_SCALRATIO_ADDR      (GUI_BASE_ADDR+0x104)   // Scaling parameters, scale=8192/ILX[8:0]
#define GUI_POINT1_ADDR         (GUI_BASE_ADDR+0x0c)    // Destination Offset
#define GUI_CMD_ADDR            (GUI_BASE_ADDR+0x04)    // Command
#define GUI_SCALCTRL_ADDR       (GUI_BASE_ADDR+0x100)   // Color Space conversion and scaling control
#define GUI_SCALOFFSET_ADDR     (GUI_BASE_ADDR+0x128)   // Output image line stride

#define SDRAM_CFG_REG1          (0xB002d000 + 0x0)
#define SDRAM_CFG_REG2          (0xB002d000 + 0x4)
#define SDRAM_CFG_REG3          (0xB002d000 + 0x8)

#define L2_DMA_BLOCK_ADDR_CONF	0xb002c000
#define L2_DMA_BLOCK_COUNT_CONF	0xb002c040

#define L2_DMA_CRTL				(L2_BASE_ADDR+0x80)
#define L2_FRACTIOIN_DMA_CONF	(L2_BASE_ADDR+0x84)
#define L2_BLOCK_DMA_CONF		(L2_BASE_ADDR+0x88)
#define L2_FLAG_CONF			(L2_BASE_ADDR+0x8C)
#define L2_BUF_ID_CONF			(L2_BASE_ADDR+0x90)
#define L2_BUF2_ID_CONF			(L2_BASE_ADDR+0x94)
#define L2_LDMA_CONF			(L2_BASE_ADDR+0x98)
#define L2_INTR_EN				(L2_BASE_ADDR+0x9C)
#define L2_STATUS1	 			(L2_BASE_ADDR+0xA0)
#define L2_STATUS2	 			(L2_BASE_ADDR+0xA8)

#define FRAC_DMA_DIRECT_WRITE	(1<<8)
#define FRAC_DMA_DIRECT_READ	(0<<8)
#define FRAC_DMA_START_REQ		(1<<9)
#define	L2_DMA_ENABLE			(1)
#endif  // #ifdef _ANYKA_CPU_780X_H_
//#endif // #if ((defined CHIP_AK7801) || (defined CHIP_AK7802))
