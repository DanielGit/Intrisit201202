/*
 * jz4760gpio.h
 * JZ4760 GPIO register definition
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 *
 * Author: whxu@ingenic.cn
 */

#ifndef __JZ4760GPIO_H__
#define __JZ4760GPIO_H__


/*
 * General purpose I/O port module(GPIO) address definition
 */
#define	GPIO_BASE	0xb0010000

#define GPIO_BASE_U_VIRTUAL		( 0xB0010000 )


/* GPIO group offset */
#define GPIO_GOS	0x100

/* Each group address */
#define GPIO_BASEA	(GPIO_BASE + (0) * GPIO_GOS)
#define GPIO_BASEB	(GPIO_BASE + (1) * GPIO_GOS)
#define GPIO_BASEC	(GPIO_BASE + (2) * GPIO_GOS)
#define GPIO_BASED	(GPIO_BASE + (3) * GPIO_GOS)
#define GPIO_BASEE	(GPIO_BASE + (4) * GPIO_GOS)
#define GPIO_BASEF	(GPIO_BASE + (5) * GPIO_GOS)


/*
 * GPIO registers offset address definition
 */
#define GPIO_PXPIN_OFFSET	(0x00)	/*  r, 32, 0x00000000 */
#define GPIO_PXDAT_OFFSET	(0x10)	/*  r, 32, 0x00000000 */
#define GPIO_PXDATS_OFFSET	(0x14)  /*  w, 32, 0x???????? */
#define GPIO_PXDATC_OFFSET	(0x18)  /*  w, 32, 0x???????? */
#define GPIO_PXIM_OFFSET	(0x20)  /*  r, 32, 0xffffffff */
#define GPIO_PXIMS_OFFSET	(0x24)  /*  w, 32, 0x???????? */
#define GPIO_PXIMC_OFFSET	(0x28)  /*  w, 32, 0x???????? */
#define GPIO_PXPE_OFFSET	(0x30)  /*  r, 32, 0x00000000 */
#define GPIO_PXPES_OFFSET	(0x34)  /*  w, 32, 0x???????? */
#define GPIO_PXPEC_OFFSET	(0x38)  /*  w, 32, 0x???????? */
#define GPIO_PXFUN_OFFSET	(0x40)  /*  r, 32, 0x00000000 */
#define GPIO_PXFUNS_OFFSET	(0x44)  /*  w, 32, 0x???????? */
#define GPIO_PXFUNC_OFFSET	(0x48)  /*  w, 32, 0x???????? */
#define GPIO_PXSEL_OFFSET	(0x50)  /*  r, 32, 0x00000000 */
#define GPIO_PXSELS_OFFSET	(0x54)  /*  w, 32, 0x???????? */
#define GPIO_PXSELC_OFFSET	(0x58)  /*  w, 32, 0x???????? */
#define GPIO_PXDIR_OFFSET	(0x60)  /*  r, 32, 0x00000000 */
#define GPIO_PXDIRS_OFFSET	(0x64)  /*  w, 32, 0x???????? */
#define GPIO_PXDIRC_OFFSET	(0x68)  /*  w, 32, 0x???????? */
#define GPIO_PXTRG_OFFSET	(0x70)  /*  r, 32, 0x00000000 */
#define GPIO_PXTRGS_OFFSET	(0x74)  /*  w, 32, 0x???????? */
#define GPIO_PXTRGC_OFFSET	(0x78)  /*  w, 32, 0x???????? */
#define GPIO_PXFLG_OFFSET	(0x80)  /*  r, 32, 0x00000000 */
#define GPIO_PXFLGC_OFFSET	(GPIO_PXDATS_OFFSET)  /*  w, 32, 0x???????? */
#define GPIO_PXDS0_OFFSET	(0xC0)  /*  r, 32, 0x00000000 */
#define GPIO_PXDS0S_OFFSET	(0xC4)  /*  w, 32, 0x00000000 */
#define GPIO_PXDS0C_OFFSET	(0xC8)  /*  w, 32, 0x00000000 */
#define GPIO_PXDS1_OFFSET	(0xD0)  /*  r, 32, 0x00000000 */
#define GPIO_PXDS1S_OFFSET	(0xD4)  /*  w, 32, 0x00000000 */
#define GPIO_PXDS1C_OFFSET	(0xD8)  /*  w, 32, 0x00000000 */
#define GPIO_PXDS2_OFFSET	(0xE0)  /*  r, 32, 0x00000000 */
#define GPIO_PXDS2S_OFFSET	(0xE4)  /*  w, 32, 0x00000000 */
#define GPIO_PXDS2C_OFFSET	(0xE8)  /*  w, 32, 0x00000000 */
#define GPIO_PXSL_OFFSET	(0xF0)  /*  r, 32, 0x00000000 */
#define GPIO_PXSLS_OFFSET	(0xF4)  /*  w, 32, 0x00000000 */
#define GPIO_PXSLC_OFFSET	(0xF8)  /*  w, 32, 0x00000000 */

/*
 * GPIO registers address definition
 */
#define GPIO_PXPIN(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPIN_OFFSET)
#define GPIO_PXDAT(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDAT_OFFSET)
#define GPIO_PXDATS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDATS_OFFSET)
#define GPIO_PXDATC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDATC_OFFSET)
#define GPIO_PXIM(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIM_OFFSET)
#define GPIO_PXIMS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIMS_OFFSET)
#define GPIO_PXIMC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIMC_OFFSET)
#define GPIO_PXPE(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPE_OFFSET)
#define GPIO_PXPES(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPES_OFFSET)
#define GPIO_PXPEC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPEC_OFFSET)
#define GPIO_PXFUN(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUN_OFFSET)
#define GPIO_PXFUNS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUNS_OFFSET)
#define GPIO_PXFUNC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUNC_OFFSET)
#define GPIO_PXSEL(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSEL_OFFSET)
#define GPIO_PXSELS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSELS_OFFSET)
#define GPIO_PXSELC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSELC_OFFSET)
#define GPIO_PXDIR(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDIR_OFFSET)
#define GPIO_PXDIRS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDIRS_OFFSET)
#define GPIO_PXDIRC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDIRC_OFFSET)
#define GPIO_PXTRG(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXTRG_OFFSET)
#define GPIO_PXTRGS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXTRGS_OFFSET)
#define GPIO_PXTRGC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXTRGC_OFFSET)
#define GPIO_PXFLG(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFLG_OFFSET)
#define GPIO_PXFLGC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFLGC_OFFSET)
#define GPIO_PXDS0(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS0_OFFSET)
#define GPIO_PXDS0S(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS0S_OFFSET)
#define GPIO_PXDS0C(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS0C_OFFSET)
#define GPIO_PXDS1(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS1_OFFSET)
#define GPIO_PXDS1S(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS1S_OFFSET)
#define GPIO_PXDS1C(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS1C_OFFSET)
#define GPIO_PXDS2(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS2_OFFSET)
#define GPIO_PXDS2S(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS2S_OFFSET)
#define GPIO_PXDS2C(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDS2C_OFFSET)
#define GPIO_PXSL(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSL_OFFSET)
#define GPIO_PXSLS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSLS_OFFSET)
#define GPIO_PXSLC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSLC_OFFSET)

/*  */
#define GPIO_PORT_NUM   6
#define MAX_GPIO_NUM	192
#define GPIO_WAKEUP     (30)
#define GPIO_PORT_NUM   6
#define MAX_GPIO_NUM	192
#define GPIO_WAKEUP     (30)

#define GPIO_HIGH_LEVEL		1
#define GPIO_LOW_LEVEL		0

#define UNUSED_GPIO_PIN		0xFFFFFFFF
#define GPIO_GROUP_NUM		0x6
#define	GPIO_GROUP_A	0x00
#define	GPIO_GROUP_B	0x20
#define	GPIO_GROUP_C	0x40
#define	GPIO_GROUP_D	0x60
#define	GPIO_GROUP_E	0x80
#define	GPIO_GROUP_F	0xA0
#define POWER_OFF_PIN	(GPIO_GROUP_A + 30)



#ifndef __MIPS_ASSEMBLER

//n = 0,1,2,3,4,5 (PORTA, PORTB, PORTC, PORTD, PORTE, PORTF)
#define REG_GPIO_PXPIN(n)	REG32(GPIO_PXPIN(n))
#define REG_GPIO_PXDAT(n)	REG32(GPIO_PXDAT(n))
#define REG_GPIO_PXDATS(n)	REG32(GPIO_PXDATS(n))
#define REG_GPIO_PXDATC(n)	REG32(GPIO_PXDATC(n))
#define REG_GPIO_PXIM(n)	REG32(GPIO_PXIM(n))
#define REG_GPIO_PXIMS(n)	REG32(GPIO_PXIMS(n))
#define REG_GPIO_PXIMC(n)	REG32(GPIO_PXIMC(n))
#define REG_GPIO_PXPE(n)	REG32(GPIO_PXPE(n))
#define REG_GPIO_PXPES(n)	REG32(GPIO_PXPES(n))
#define REG_GPIO_PXPEC(n)	REG32(GPIO_PXPEC(n))
#define REG_GPIO_PXFUN(n)	REG32(GPIO_PXFUN(n))
#define REG_GPIO_PXFUNS(n)	REG32(GPIO_PXFUNS(n))
#define REG_GPIO_PXFUNC(n)	REG32(GPIO_PXFUNC(n))
#define REG_GPIO_PXSEL(n)	REG32(GPIO_PXSEL(n))
#define REG_GPIO_PXSELS(n)	REG32(GPIO_PXSELS(n))
#define REG_GPIO_PXSELC(n)	REG32(GPIO_PXSELC(n))
#define REG_GPIO_PXDIR(n)	REG32(GPIO_PXDIR(n))
#define REG_GPIO_PXDIRS(n)	REG32(GPIO_PXDIRS(n))
#define REG_GPIO_PXDIRC(n)	REG32(GPIO_PXDIRC(n))
#define REG_GPIO_PXTRG(n)	REG32(GPIO_PXTRG(n))
#define REG_GPIO_PXTRGS(n)	REG32(GPIO_PXTRGS(n))
#define REG_GPIO_PXTRGC(n)	REG32(GPIO_PXTRGC(n))
#define REG_GPIO_PXFLG(n)	REG32(GPIO_PXFLG(n))
#define REG_GPIO_PXFLGC(n)	REG32(GPIO_PXFLGC(n))
#define REG_GPIO_PXDS0(n) 	REG32(GPIO_PXDS0(n))
#define REG_GPIO_PXDS0S(n) 	REG32(GPIO_PXDS0S(n))
#define REG_GPIO_PXDS0C(n) 	REG32(GPIO_PXDS0C(n))
#define REG_GPIO_PXDS1(n) 	REG32(GPIO_PXDS1(n))
#define REG_GPIO_PXDS1S(n) 	REG32(GPIO_PXDS1S(n))
#define REG_GPIO_PXDS1C(n) 	REG32(GPIO_PXDS1C(n))
#define REG_GPIO_PXDS2(n) 	REG32(GPIO_PXDS2(n))
#define REG_GPIO_PXDS2S(n) 	REG32(GPIO_PXDS2S(n))
#define REG_GPIO_PXDS2C(n) 	REG32(GPIO_PXDS2C(n))
#define REG_GPIO_PXSL(n) 	REG32(GPIO_PXSL(n))
#define REG_GPIO_PXSLS(n) 	REG32(GPIO_PXSLS(n))
#define REG_GPIO_PXSLC(n) 	REG32(GPIO_PXSLC(n))


typedef enum
{
	GITT_L_LEVEL = 0x00,
	GITT_H_LEVEL = 0x01,
	GITT_F_EDGE = 0x02,
	GITT_R_EDGE = 0x03,
	GITT_MAX = 0x04,		// Just a value of the boundary
}GPIO_INTR_TRIGGER_TYPE;


/*----------------------------------------------------------------
 * p is the port number (0,1,2,3,4,5)
 * o is the pin offset (0-31) inside the port
 * n is the absolute number of a pin (0-127), regardless of the port
 */

//----------------------------------------------------------------
// Function Pins Mode

#define __gpio_as_func0(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFUNS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXSELC(p) = (1 << o);		\
} while (0)

#define __gpio_as_func1(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFUNS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
} while (0)

#define __gpio_as_func2(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFUNS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXSELC(p) = (1 << o);		\
} while (0)

#define __gpio_as_func3(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFUNS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
} while (0)

/*
 * UART0_TxD, UART0_RxD
 */
#define __gpio_as_uart0()			\
do {						\
	unsigned int bits = BIT3 | BIT0;	\
	REG_GPIO_PXFUNS(5) = bits;		\
	REG_GPIO_PXTRGC(5) = bits;		\
	REG_GPIO_PXSELC(5) = bits;		\
	REG_GPIO_PXPES(5)  = bits;		\
} while (0)

/*
 * UART0_TxD, UART0_RxD, UART0_CTS, UART0_RTS
 */
#define __gpio_as_uart0_ctsrts()		\
do {						\
	unsigned int bits = BITS_H2L(3, 0);	\
	REG_GPIO_PXFUNS(5) = bits;		\
	REG_GPIO_PXTRGC(5) = bits;		\
	REG_GPIO_PXSELC(5) = bits;		\
	REG_GPIO_PXPES(5)  = bits;		\
} while (0)

/*
 * UART1_TxD, UART1_RxD
 */
#define __gpio_as_uart1()			\
do {						\
	unsigned int bits = BIT28 | BIT26;	\
	REG_GPIO_PXFUNS(3) = bits;		\
	REG_GPIO_PXTRGC(3) = bits;		\
	REG_GPIO_PXSELC(3) = bits;		\
	REG_GPIO_PXPES(3)  = bits;		\
} while (0)

/*
 * UART1_TxD, UART1_RxD, UART1_CTS, UART1_RTS
 */
#define __gpio_as_uart1_ctsrts()		\
do {						\
	unsigned int bits = BITS_H2L(29, 26);	\
	REG_GPIO_PXFUNS(3) = bits;		\
	REG_GPIO_PXTRGC(3) = bits;		\
	REG_GPIO_PXSELC(3) = bits;		\
	REG_GPIO_PXPES(3)  = bits;		\
} while (0)


/*
 * UART2_TxD, UART2_RxD
 */
#define __gpio_as_uart2()			\
do {						\
	unsigned int bits = BIT30 | BIT28;	\
	REG_GPIO_PXFUNS(2) = bits;		\
	REG_GPIO_PXTRGC(2) = bits;		\
	REG_GPIO_PXSELC(2) = bits;		\
	REG_GPIO_PXPES(2)  = bits;		\
} while (0)

/*
 * UART2_TxD, UART2_RxD, UART2_CTS, UART2_RTS
 */
#define __gpio_as_uart2_ctsrts()		\
do {						\
	unsigned int bits = BITS_H2L(31, 28);	\
	REG_GPIO_PXFUNS(2) = bits;		\
	REG_GPIO_PXTRGC(2) = bits;		\
	REG_GPIO_PXSELC(2) = bits;		\
	REG_GPIO_PXPES(2)  = bits;		\
} while (0)

/* WARNING: the folloing macro do NOT check */
/*
 * UART3_TxD, UART3_RxD
 */
 
 #ifndef JZ4760B
 
#define __gpio_as_uart3()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00000028;		\
	REG_GPIO_PXTRGC(4) = 0x00000028;		\
	REG_GPIO_PXSELS(4) = 0x00000028;		\
	REG_GPIO_PXPES(4)  = 0x00000028;	\
} while (0)
/*
 * UART3_TxD, UART3_RxD, UART3_CTS, UART3_RTS
 */
#define __gpio_as_uart3_ctsrts()		\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00000028;		\
	REG_GPIO_PXTRGC(4) = 0x00000028;		\
	REG_GPIO_PXSELS(4) = 0x00000028;		\
	REG_GPIO_PXFUNS(4) = 0x00000300;		\
	REG_GPIO_PXTRGC(4) = 0x00000300;		\
	REG_GPIO_PXSELC(4) = 0x00000300;		\
	REG_GPIO_PXPES(4)  = 0x00000328;	\
}

#else
#define __gpio_as_uart3()			\
do {						\
	unsigned int bits = BIT12;	\
	REG_GPIO_PXFUNS(3) = bits;		\
	REG_GPIO_PXTRGC(3) = bits;		\
	REG_GPIO_PXSELS(3) = bits;		\
	REG_GPIO_PXPES(3)  = bits;	\
	bits = BIT5;	\
	REG_GPIO_PXFUNS(4) = bits;		\
	REG_GPIO_PXTRGC(4) = bits;		\
	REG_GPIO_PXSELS(4) = bits;		\
	REG_GPIO_PXPES(4)  = bits;	\
} while (0)
/*
 * UART3_TxD, UART3_RxD, UART3_CTS, UART3_RTS
 */
#define __gpio_as_uart3_ctsrts()		\
do {						\
	REG_GPIO_PXFUNS(3) = (1 << 12);		\
	REG_GPIO_PXTRGC(3) = (1 << 12);		\
	REG_GPIO_PXSELC(3) = (1 << 12);		\
	REG_GPIO_PXPES(3)  = (1 << 12);		\
	REG_GPIO_PXFUNS(4) = 0x00000320;	\
	REG_GPIO_PXTRGC(4) = 0x00000320;	\
	REG_GPIO_PXSELS(4) = 0x00000020;	\
	REG_GPIO_PXSELC(4) = 0x00000300;	\
	REG_GPIO_PXPES(4)  = 0x00000320;	\
} while (0)
#endif
/*
 * SD0 ~ SD7, CS1#, CLE, ALE, FRE#, FWE#, FRB#
 * @n: chip select number(1 ~ 6)
 */
#define __gpio_as_nand_8bit(n)						\
do {		              						\
									\
	REG_GPIO_PXFUNS(0) = 0x000c00ff; /* SD0 ~ SD7, CS1#, FRE#, FWE# */ \
	REG_GPIO_PXSELC(0) = 0x000c00ff;				\
	REG_GPIO_PXTRGC(0) = 0x000c00ff;				\
	REG_GPIO_PXPES(0) = 0x000c00ff;					\
	REG_GPIO_PXFUNS(1) = 0x00000003; /* CLE(SA2), ALE(SA3) */	\
	REG_GPIO_PXSELC(1) = 0x00000003;				\
	REG_GPIO_PXTRGC(1) = 0x00000003;				\
	REG_GPIO_PXPES(1) = 0x00000003;					\
									\
	REG_GPIO_PXFUNS(0) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXSELC(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPES(0) = 0x00200000 << ((n)-1);			\
									\
 	REG_GPIO_PXFUNC(0) = 0x00100000; /* FRB#(input) */		\
	REG_GPIO_PXSELC(0) = 0x00100000;				\
	REG_GPIO_PXDIRC(0) = 0x00100000;				\
	REG_GPIO_PXPES(0) = 0x00100000;					\
} while (0)
#ifndef JZ4760B

#define __gpio_as_nand_16bit(n)						\
do {		              						\
									\
	REG_GPIO_PXFUNS(0) = 0x000cffff; /* SD0 ~ SD15, CS1#, FRE#, FWE# */ \
	REG_GPIO_PXSELC(0) = 0x000cffff;				\
	REG_GPIO_PXTRGC(0) = 0x000cffff;				\
	REG_GPIO_PXPES(0) = 0x000cffff;					\
	REG_GPIO_PXFUNS(1) = 0x00000003; /* CLE(SA2), ALE(SA3) */	\
	REG_GPIO_PXSELC(1) = 0x00000003;				\
	REG_GPIO_PXTRGC(1) = 0x00000003;				\
	REG_GPIO_PXPES(1) = 0x00000003;					\
									\
	REG_GPIO_PXFUNS(0) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXSELC(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPES(0) = 0x00200000 << ((n)-1);			\
									\
 	REG_GPIO_PXFUNC(0) = 0x00100000; /* FRB#(input) */		\
	REG_GPIO_PXSELC(0) = 0x00100000;				\
	REG_GPIO_PXDIRC(0) = 0x00100000;				\
	REG_GPIO_PXPES(0) = 0x00100000;					\
} while (0)

#else
#define __gpio_as_nand_16bit(n)						\
do {		              						\
									\
	REG_GPIO_PXFUNS(0) = 0x000cffff; /* SD0 ~ SD15, CS1#, FRE#, FWE# */ \
	REG_GPIO_PXSELC(0) = 0x000cffff;				\
	REG_GPIO_PXTRGC(0) = 0x000cffff;				\
	REG_GPIO_PXPES(0) = 0x000cffff;					\
	REG_GPIO_PXFUNS(1) = 0x00000003; /* CLE(SA2), ALE(SA3) */	\
	REG_GPIO_PXSELC(1) = 0x00000003;				\
	REG_GPIO_PXTRGC(1) = 0x00000003;				\
	REG_GPIO_PXPES(1) = 0x00000003;					\
									\
	REG_GPIO_PXFUNS(0) = 0x00200000 << ((n)-1); /* CSn */		\
	REG_GPIO_PXSELC(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXTRGC(0) = 0x00200000 << ((n)-1);			\
	REG_GPIO_PXPES(0) = 0x00200000 << ((n)-1);			\
									\
 	REG_GPIO_PXFUNC(0) = 0x00100000; /* FRB#(input) */		\
	REG_GPIO_PXSELC(0) = 0x00100000;				\
	REG_GPIO_PXDIRC(0) = 0x00100000;				\
	REG_GPIO_PXPES(0) = 0x00100000;					\
} while (0)

/*
 *  SLCD
 */
 
#define __gpio_as_slcd_16bit() \
do {						\
	REG_GPIO_PXFUNS(2) = 0x03cff0fc;	\
	REG_GPIO_PXTRGC(2) = 0x03cff0fc;	\
	REG_GPIO_PXSELC(2) = 0x03cff0fc;    \
	REG_GPIO_PXPES(2) = 0x03cff0fc;    \
} while (0)
#endif 
/*
 * LCD_R3~LCD_R7, LCD_G2~LCD_G7, LCD_B3~LCD_B7,
 * LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_16bit()			\
do {						\
	REG_GPIO_PXFUNS(2) = 0x0f8ff3f8;	\
	REG_GPIO_PXTRGC(2) = 0x0f8ff3f8;	\
	REG_GPIO_PXSELC(2) = 0x0f8ff3f8;	\
	REG_GPIO_PXPES(2) = 0x0f8ff3f8;		\
} while (0)

/*
 * LCD_R2~LCD_R7, LCD_G2~LCD_G7, LCD_B2~LCD_B7,
 * LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_18bit()			\
do {						\
	REG_GPIO_PXFUNS(2) = 0x0fcff3fc;	\
	REG_GPIO_PXTRGC(2) = 0x0fcff3fc;	\
	REG_GPIO_PXSELC(2) = 0x0fcff3fc;	\
	REG_GPIO_PXPES(2) = 0x0fcff3fc;		\
} while (0)

/*
 * LCD_R0~LCD_R7, LCD_G0~LCD_G7, LCD_B0~LCD_B7,
 * LCD_PCLK, LCD_HSYNC, LCD_VSYNC, LCD_DE
 */
#define __gpio_as_lcd_24bit()			\
do {						\
	REG_GPIO_PXFUNS(2) = 0x0fffffff;	\
	REG_GPIO_PXTRGC(2) = 0x0fffffff;	\
	REG_GPIO_PXSELC(2) = 0x0fffffff;	\
	REG_GPIO_PXPES(2) = 0x0fffffff;		\
} while (0)


#ifdef JZ4760B
/* Set data pin driver strength v: 0~7 */
#define __gpio_set_lcd_data_driving_strength(v) \
do {						\
	unsigned int d;			\
	d = v & 0x1;				\
	if(d) REG_GPIO_PXDS0S(2) = 0x0ff3fcff;	\
	else REG_GPIO_PXDS0C(2) = 0x0ff3fcff;	\
	d = v & 0x2;				\
	if(d) REG_GPIO_PXDS1S(2) = 0x0ff3fcff;	\
	else REG_GPIO_PXDS1C(2) = 0x0ff3fcff;	\
	d = v & 0x4;				\
	if(d) REG_GPIO_PXDS2S(2) = 0x0ff3fcff;	\
	else REG_GPIO_PXDS2C(2) = 0x0ff3fcff;	\
} while(0)
/* Set HSYNC VSYNC DE driver strength v: 0~7 */
#define __gpio_set_lcd_sync_driving_strength(v) \
do {						\
	unsigned int d;				\
	d = v & 0x1;				\
	if(d) REG_GPIO_PXDS0S(2) = 0x000c0200;	\
	else REG_GPIO_PXDS0C(2) = 0x000c0200;	\
	d = v & 0x2;				\
	if(d) REG_GPIO_PXDS1S(2) = 0x000c0200;	\
	else REG_GPIO_PXDS1C(2) = 0x000c0200;	\
	d = v & 0x4;				\
	if(d) REG_GPIO_PXDS2S(2) = 0x000c0200;	\
	else REG_GPIO_PXDS2C(2) = 0x000c0200;	\
} while(0)
/* Set PCLK driver strength v: 0~7 */
#define __gpio_set_lcd_clk_driving_strength(v)	\
do {						\
	unsigned int d;				\
	d = v & 0x1;				\
	if(d) REG_GPIO_PXDS0S(2) = (1 << 8);	\
	else REG_GPIO_PXDS0C(2) = (1 << 8);	\
	d = v & 0x2;				\
	if(d) REG_GPIO_PXDS1S(2) = (1 << 8);	\
	else REG_GPIO_PXDS1C(2) = (1 << 8);	\
	d = v & 0x4;				\
	if(d) REG_GPIO_PXDS2S(2) = (1 << 8);	\
	else REG_GPIO_PXDS2C(2) = (1 << 8);	\
} while(0)
/* Set fast slew rate */
#define __gpio_set_lcd_data_fslew(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLS(p) = 0x0ff3fcff;		\
} while(0)
/* Set slow slew rate */
#define __gpio_set_lcd_data_sslew(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLC(p) = 0x0ff3fcff;		\
} while(0)

/* Set fast slew rate */
#define __gpio_set_lcd_sync_fslew(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLS(p) = 0x000c0200;		\
} while(0)

/* Set slow slew rate */
#define __gpio_set_lcd_sync_sslew(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLC(p) = 0x000c0200;		\
} while(0)

/* Set fast slew rate */
#define __gpio_set_lcd_pclk_fslew(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLS(p) = (1 << 8);		\
} while(0)

/* Set slow slew rate */
#define __gpio_set_lcd_pclk_sslew(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLC(p) = (1 << 8);		\
} while(0)

#endif /*
 *  LCD_CLS, LCD_SPL, LCD_PS, LCD_REV
 */
#define __gpio_as_lcd_special()			\
do {						\
	REG_GPIO_PXFUNS(2) = 0x0fffffff;	\
	REG_GPIO_PXTRGC(2) = 0x0fffffff;	\
	REG_GPIO_PXSELC(2) = 0x0feffbfc;	\
	REG_GPIO_PXSELS(2) = 0x00100403;	\
	REG_GPIO_PXPES(2) = 0x0fffffff;		\
} while (0)

#ifdef JZ4760B
#define __gpio_as_epd()				\
do {						\
	REG_GPIO_PXFUNS(1) = 0x00011e00;	\
	REG_GPIO_PXTRGS(1) = 0x00011e00;	\
	REG_GPIO_PXSELS(1) = 0x00011e00;	\
	REG_GPIO_PXPES(1)  = 0x00011e00;	\
} while (0)
#endif 
/*
 * CIM_D0~CIM_D7, CIM_MCLK, CIM_PCLK, CIM_VSYNC, CIM_HSYNC
 */
#define __gpio_as_cim()				\
do {						\
	REG_GPIO_PXFUNS(1) = 0x0003ffc0;	\
	REG_GPIO_PXTRGC(1) = 0x0003ffc0;	\
	REG_GPIO_PXSELC(1) = 0x0003ffc0;	\
	REG_GPIO_PXPES(1)  = 0x0003ffc0;	\
} while (0)

/*
 * SDATO, SDATI, BCLK, SYNC, SCLK_RSTN(gpio sepc) or
 * SDATA_OUT, SDATA_IN, BIT_CLK, SYNC, SCLK_RESET(aic spec)
 */
 
 #ifndef JZ4760B
#define __gpio_as_aic()				\
do {						\
	REG_GPIO_PXFUNS(4) = 0x16c00000;	\
	REG_GPIO_PXTRGC(4) = 0x02c00000;	\
	REG_GPIO_PXTRGS(4) = 0x14000000;	\
	REG_GPIO_PXSELC(4) = 0x14c00000;	\
	REG_GPIO_PXSELS(4) = 0x02000000;	\
	REG_GPIO_PXPES(4)  = 0x16c00000;	\
} while (0)

#else
#define __gpio_as_aic()		\
do {					\
	REG_GPIO_PXFUNS(3) = 0x00003000;	\
	REG_GPIO_PXTRGC(3) = 0x00003000;	\
	REG_GPIO_PXSELS(3) = 0x00001000;	\
	REG_GPIO_PXSELC(3) = 0x00002000;	\
	REG_GPIO_PXPES(3)  = 0x00003000;	\
	REG_GPIO_PXFUNS(4) = 0x000000e0;	\
	REG_GPIO_PXTRGS(4) = 0x00000020;	\
	REG_GPIO_PXTRGC(4) = 0x000000c0;	\
	REG_GPIO_PXSELC(4) = 0x000000e0;	\
	REG_GPIO_PXPES(4)  = 0x000000e0;	\
} while (0)

#define __gpio_as_spdif()		\
do {					\
	REG_GPIO_PXFUNS(3) = 0x00003000;	\
	REG_GPIO_PXTRGC(3) = 0x00003000;	\
	REG_GPIO_PXSELS(3) = 0x00001000;	\
	REG_GPIO_PXSELC(3) = 0x00002000;	\
	REG_GPIO_PXPES(3)  = 0x00003000;	\
	REG_GPIO_PXFUNS(4) = 0x000038e0;	\
	REG_GPIO_PXTRGC(4) = 0x000038c0;	\
	REG_GPIO_PXTRGS(4) = 0x00000020;	\
	REG_GPIO_PXSELC(4) = 0x000038e0;	\
	REG_GPIO_PXPES(4)  = 0x000038e0;	\
} while (0)
#endif 


#ifndef JZ4760B

/*
 * MSC0_CMD, MSC0_CLK, MSC0_D0 ~ MSC0_D7
 */
#define __gpio_as_msc0_8bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x3ff00000;	\
	REG_GPIO_PXTRGC(4) = 0x3ff00000;	\
	REG_GPIO_PXSELC(4) = 0x3ff00000;	\
	REG_GPIO_PXPES(4)  = 0x3ff00000;	\
} while (0)

/*
 * MSC0_CMD, MSC0_CLK, MSC0_D0 ~ MSC0_D3
 */
#define __gpio_as_msc0_4bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x30f00000;	\
	REG_GPIO_PXTRGC(4) = 0x30f00000;	\
	REG_GPIO_PXSELC(4) = 0x30f00000;	\
	REG_GPIO_PXPES(4)  = 0x30f00000;	\
} while (0)

/*
 * MSC1_CMD, MSC1_CLK, MSC1_D0 ~ MSC1_D3
 */
#define __gpio_as_msc1_4bit()			\
do {						\
	REG_GPIO_PXFUNS(3) = 0x3f00000;	\
	REG_GPIO_PXTRGC(3) = 0x3f00000;	\
	REG_GPIO_PXSELC(3) = 0x3f00000;	\
	REG_GPIO_PXPES(3)  = 0x3f00000;	\
} while (0)

#if 0
/*
 * MSC0_CMD, MSC0_CLK, MSC0_D0 ~ MSC0_D3
 */
#define __gpio_as_msc0_4bit()			\
do {						\
	REG_GPIO_PXFUNS(2) = 0x38400300;	\
	REG_GPIO_PXTRGC(2) = 0x38400300;	\
	REG_GPIO_PXSELS(2) = 0x30400300;	\
	REG_GPIO_PXSELC(2) = 0x08000000;	\
	REG_GPIO_PXPES(2)  = 0x38400300;	\
} while (0)

/*
 * MSC1_CMD, MSC1_CLK, MSC1_D0 ~ MSC1_D3
 */
#define __gpio_as_msc1_4bit()			\
do {						\
	REG_GPIO_PXFUNS(1) = 0xfc000000;	\
	REG_GPIO_PXTRGC(1) = 0xfc000000;	\
	REG_GPIO_PXSELC(1) = 0xfc000000;	\
	REG_GPIO_PXPES(1)  = 0xfc000000;	\
} while (0)
#endif

/* Port B
 * MSC2_CMD, MSC2_CLK, MSC2_D0 ~ MSC2_D3
 */
#define __gpio_as_msc2_4bit_1()			\
do {						\
	REG_GPIO_PXFUNS(1) = 0xf0300000;	\
	REG_GPIO_PXTRGC(1) = 0xf0300000;	\
	REG_GPIO_PXSELC(1) = 0xf0300000;	\
	REG_GPIO_PXPES(1)  = 0xf0300000;	\
} while (0)

#define __gpio_as_msc 	__gpio_as_msc0_4bit /* default as msc0 4bit */
#define __gpio_as_msc0 	__gpio_as_msc0_4bit /* msc0 default as 4bit */
#define __gpio_as_msc1 	__gpio_as_msc1_4bit /* msc1 only support 4bit */

/*
 * TSCLK, TSSTR, TSFRM, TSFAIL, TSDI0~7
 */
#define __gpio_as_tssi_1()			\
do {						\
	REG_GPIO_PXFUNS(1) = 0x0003ffc0;	\
	REG_GPIO_PXTRGC(1) = 0x0003ffc0;	\
	REG_GPIO_PXSELS(1) = 0x0003ffc0;	\
	REG_GPIO_PXPES(1)  = 0x0003ffc0;	\
} while (0)

/*
 * TSCLK, TSSTR, TSFRM, TSFAIL, TSDI0~7
 */
#define __gpio_as_tssi_2()			\
do {						\
	REG_GPIO_PXFUNS(1) = 0xfff00000;	\
	REG_GPIO_PXTRGC(1) = 0x0fc00000;	\
	REG_GPIO_PXTRGS(1) = 0xf0300000;	\
	REG_GPIO_PXSELC(1) = 0xfff00000;	\
	REG_GPIO_PXPES(1)  = 0xfff00000;	\
} while (0)

/*
 * SSI_CE0, SSI_CE1, SSI_GPC, SSI_CLK, SSI_DT, SSI_DR
 */
#define __gpio_as_ssi()				\
do {						\
	REG_GPIO_PXFUNS(0) = 0x002c0000; /* SSI0_CE0, SSI0_CLK, SSI0_DT	*/ \
	REG_GPIO_PXTRGS(0) = 0x002c0000;	\
	REG_GPIO_PXSELC(0) = 0x002c0000;	\
	REG_GPIO_PXPES(0)  = 0x002c0000;	\
						\
	REG_GPIO_PXFUNS(0) = 0x00100000; /* SSI0_DR */	\
	REG_GPIO_PXTRGC(0) = 0x00100000;	\
	REG_GPIO_PXSELS(0) = 0x00100000;	\
	REG_GPIO_PXPES(0)  = 0x00100000;	\
} while (0)

/*
 * SSI_CE0, SSI_CE2, SSI_GPC, SSI_CLK, SSI_DT, SSI1_DR
 */
#define __gpio_as_ssi_1()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0x0000fc00;	\
	REG_GPIO_PXTRGC(5) = 0x0000fc00;	\
	REG_GPIO_PXSELC(5) = 0x0000fc00;	\
	REG_GPIO_PXPES(5)  = 0x0000fc00;	\
} while (0)

/* Port B
 * SSI2_CE0, SSI2_CE2, SSI2_GPC, SSI2_CLK, SSI2_DT, SSI12_DR
 */
#define __gpio_as_ssi2_1()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0xf0300000;	\
	REG_GPIO_PXTRGC(5) = 0xf0300000;	\
	REG_GPIO_PXSELS(5) = 0xf0300000;	\
	REG_GPIO_PXPES(5)  = 0xf0300000;	\
} while (0)

/*
 * I2C_SCK, I2C_SDA
 */
#define __gpio_as_i2c(n)		       \
do {						\
	REG_GPIO_PXFUNS(3+(n)) = 0xc0000000;	\
	REG_GPIO_PXTRGC(3+(n)) = 0xc0000000;	\
	REG_GPIO_PXSELC(3+(n)) = 0xc0000000;	\
	REG_GPIO_PXPES(3+(n))  = 0xc0000000;	\
} while (0)

/*
 * PWM0
 */
#define __gpio_as_pwm0()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00100000;	\
	REG_GPIO_PXSELC(4) = 0x00100000;	\
	REG_GPIO_PXPES(4) = 0x00100000;		\
} while (0)

/*
 * PWM1
 */
#define __gpio_as_pwm1()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x2;	        \
	REG_GPIO_PXTRGC(4) = 0x2;		\
	REG_GPIO_PXSELC(4) = 0x2;		\
	REG_GPIO_PXPEC(4) = 0x2;		\
} while (0)

/*
 * PWM2
 */
#define __gpio_as_pwm2()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00400000;	\
	REG_GPIO_PXSELC(4) = 0x00400000;	\
	REG_GPIO_PXPES(4) = 0x00400000;		\
} while (0)

/*
 * PWM3
 */
#define __gpio_as_pwm3()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x00800000;	\
	REG_GPIO_PXSELC(4) = 0x00800000;	\
	REG_GPIO_PXPES(4) = 0x00800000;		\
} while (0)

/*
 * PWM4
 */
#define __gpio_as_pwm4()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x01000000;	\
	REG_GPIO_PXSELC(4) = 0x01000000;	\
	REG_GPIO_PXPES(4) = 0x01000000;		\
} while (0)

/*
 * PWM5
 */
#define __gpio_as_pwm5()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x02000000;	\
	REG_GPIO_PXSELC(4) = 0x02000000;	\
	REG_GPIO_PXPES(4) = 0x02000000;		\
} while (0)

/*
 * n = 0 ~ 5
 */
#define __gpio_as_pwm(n)	__gpio_as_pwm##n()

/*
 * OWI - PA29 function 1
 */
#define __gpio_as_owi()				\
do {						\
	REG_GPIO_PXFUNS(0) = 0x20000000;	\
	REG_GPIO_PXTRGC(0) = 0x20000000;	\
	REG_GPIO_PXSELS(0) = 0x20000000;	\
} while (0)

/*
 * SCC - PD08 function 0
 *       PD09 function 0
 */
#define __gpio_as_scc()				\
do {						\
	REG_GPIO_PXFUNS(3) = 0xc0000300;	\
	REG_GPIO_PXTRGC(3) = 0xc0000300;	\
	REG_GPIO_PXSELC(3) = 0xc0000300;	\
} while (0)

#define __gpio_as_otg_drvvbus()	\
do {	\
	REG_GPIO_PXDATC(4) = (1 << 10);		\
	REG_GPIO_PXPEC(4) = (1 << 10);		\
	REG_GPIO_PXSELC(4) = (1 << 10);		\
	REG_GPIO_PXTRGC(4) = (1 << 10);		\
	REG_GPIO_PXFUNS(4) = (1 << 10);		\
} while (0)

//-------------------------------------------
// GPIO or Interrupt Mode

#define __gpio_get_port(p)	(REG_GPIO_PXPIN(p))

#define __gpio_port_as_output(p, o)		\
do {						\
    REG_GPIO_PXFUNC(p) = (1 << (o));		\
    REG_GPIO_PXSELC(p) = (1 << (o));		\
    REG_GPIO_PXDIRS(p) = (1 << (o));		\
    REG_GPIO_PXPES(p) = (1 << (o));		\
} while (0)

#define __gpio_port_as_input(p, o)		\
do {						\
    REG_GPIO_PXFUNC(p) = (1 << (o));		\
    REG_GPIO_PXSELC(p) = (1 << (o));		\
    REG_GPIO_PXDIRC(p) = (1 << (o));		\
} while (0)

#define __gpio_as_output(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output(p, o);		\
} while (0)

#define __gpio_as_input(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_input(p, o);		\
} while (0)

#define __gpio_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXDATS(p) = (1 << o);		\
} while (0)

#define __gpio_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXDATC(p) = (1 << o);		\
} while (0)

#define __gpio_get_pin(n)			\
({						\
	unsigned int p, o, v;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (__gpio_get_port(p) & (1 << o))	\
		v = 1;				\
	else					\
		v = 0;				\
	v;					\
})

#define __gpio_as_irq_high_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRS(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_low_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRC(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_rise_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRS(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_fall_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRC(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_ack_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
} while (0)

#define __gpio_get_irq()			\
({						\
	unsigned int p, i, tmp, v = 0;		\
	for (p = 3; p >= 0; p--) {		\
		tmp = REG_GPIO_PXFLG(p);	\
		for (i = 0; i < 32; i++)	\
			if (tmp & (1 << i))	\
				v = (32*p + i);	\
	}					\
	v;					\
})

#define __gpio_group_irq(n)			\
({						\
	register int tmp, i;			\
	tmp = REG_GPIO_PXFLG(n) & (~REG_GPIO_PXIM(n));	\
	for (i=31;i>=0;i--)			\
		if (tmp & (1 << i))		\
			break;			\
	i;					\
})

#define __gpio_enable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPEC(p) = (1 << o);		\
} while (0)

#define __gpio_disable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPES(p) = (1 << o);		\
} while (0)

#else
/*
 * MSC0_CMD, MSC0_CLK, MSC0_D0 ~ MSC0_D3
 */
#define __gpio_as_msc0_pa_4bit()		\
do {						\
	REG_GPIO_PXFUNS(0) = 0x00fc0000;	\
	REG_GPIO_PXTRGC(0) = 0x00fc0000;	\
	REG_GPIO_PXSELS(0) = 0x00ec0000;	\
	REG_GPIO_PXSELC(0) = 0x00100000;	\
	REG_GPIO_PXPES(0)  = 0x00fc0000;	\
} while (0)

/*
 * MSC0_CMD, MSC0_CLK, MSC0_D0 ~ MSC0_D7
 */
#define __gpio_as_msc0_pe_8bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x3ff00000;	\
	REG_GPIO_PXTRGC(4) = 0x3ff00000;	\
	REG_GPIO_PXSELC(4) = 0x3ff00000;	\
	REG_GPIO_PXPES(4)  = 0x3ff00000;	\
} while (0)
/*
 * MSC0_CMD, MSC0_CLK, MSC0_D0 ~ MSC0_D3
 */
#define __gpio_as_msc0_pe_4bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x30f00000;	\
	REG_GPIO_PXTRGC(4) = 0x30f00000;	\
	REG_GPIO_PXSELC(4) = 0x30f00000;	\
	REG_GPIO_PXPES(4)  = 0x30f00000;	\
} while (0)

#define __gpio_as_msc0_boot()			\
do {        					\
	REG_GPIO_PXFUNS(0) = 0x00ec0000;	\
	REG_GPIO_PXTRGC(0) = 0x00ec0000;	\
	REG_GPIO_PXSELS(0) = 0x00ec0000;	\
	REG_GPIO_PXPES(0)  = 0x00ec0000;	\
	REG_GPIO_PXFUNS(0) = 0x00100000;	\
	REG_GPIO_PXTRGC(0) = 0x00100000;	\
	REG_GPIO_PXSELC(0) = 0x00100000;	\
	REG_GPIO_PXPES(0)  = 0x00100000;	\
						\
} while (0)

/*
 * MSC1_CMD, MSC1_CLK, MSC1_D0 ~ MSC1_D7
 */
#define __gpio_as_msc1_pe_8bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x3ff00000;	\
	REG_GPIO_PXTRGC(4) = 0x3ff00000;	\
	REG_GPIO_PXSELS(4) = 0x3ff00000;	\
	REG_GPIO_PXPES(4)  = 0x3ff00000;	\
} while (0)
/*
 * MSC1_CMD, MSC1_CLK, MSC1_D0 ~ MSC1_D3
 */
#define __gpio_as_msc1_pe_4bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x30f00000;	\
	REG_GPIO_PXTRGC(4) = 0x30f00000;	\
	REG_GPIO_PXSELS(4) = 0x30f00000;	\
	REG_GPIO_PXPES(4)  = 0x30f00000;	\
} while (0)



/*
 * MSC1_CMD, MSC1_CLK, MSC1_D0 ~ MSC1_D3
 */
#define __gpio_as_msc1_pd_4bit()			\
do {						\
	REG_GPIO_PXFUNS(3) = 0x03f00000;	\
	REG_GPIO_PXTRGC(3) = 0x03f00000;	\
	REG_GPIO_PXSELC(3) = 0x03f00000;	\
	REG_GPIO_PXPES(3)  = 0x03f00000;	\
} while (0)


/* Port B
 * MSC2_CMD, MSC2_CLK, MSC2_D0 ~ MSC2_D3
 */
#define __gpio_as_msc2_pb_4bit()		\
do {						\
	REG_GPIO_PXFUNS(1) = 0xf0300000;	\
	REG_GPIO_PXTRGC(1) = 0xf0300000;	\
	REG_GPIO_PXSELC(1) = 0xf0300000;	\
	REG_GPIO_PXPES(1)  = 0xf0300000;	\
} while (0)


/*
 * MSC2_CMD, MSC2_CLK, MSC2_D0 ~ MSC2_D7
 */
#define __gpio_as_msc2_pe_8bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x3ff00000;	\
	REG_GPIO_PXTRGS(4) = 0x3ff00000;	\
	REG_GPIO_PXSELC(4) = 0x3ff00000;	\
	REG_GPIO_PXPES(4)  = 0x3ff00000;	\
} while (0)
/*
 * MSC2_CMD, MSC2_CLK, MSC2_D0 ~ MSC2_D3
 */
#define __gpio_as_msc2_pe_4bit()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x30f00000;	\
	REG_GPIO_PXTRGS(4) = 0x30f00000;	\
	REG_GPIO_PXSELC(4) = 0x30f00000;	\
	REG_GPIO_PXPES(4)  = 0x30f00000;	\
} while (0)
//#define __gpio_as_msc0_4bit	__gpio_as_msc0_pe_4bit /* default as msc0 4bit */
#define __gpio_as_msc0_4bit	__gpio_as_msc0_pa_4bit /* default as msc0 4bit */
#define __gpio_as_msc1_4bit __gpio_as_msc1_pd_4bit /* msc1 only support 4bit */
#define __gpio_as_msc 	__gpio_as_msc0_4bit /* default as msc0 4bit */
#define __gpio_as_msc0 	__gpio_as_msc0_4bit /* msc0 default as 4bit */
#define __gpio_as_msc1 	__gpio_as_msc1_4bit /* msc1 only support 4bit */

/*
 * TSCLK, TSSTR, TSFRM, TSFAIL, TSDI0~7
 */
#define __gpio_as_tssi_1()			\
do {						\
	REG_GPIO_PXFUNS(1) = 0x0003ffc0;	\
	REG_GPIO_PXTRGC(1) = 0x0003ffc0;	\
	REG_GPIO_PXSELS(1) = 0x0003ffc0;	\
	REG_GPIO_PXPES(1)  = 0x0003ffc0;	\
} while (0)

/*
 * TSCLK, TSSTR, TSFRM, TSFAIL, TSDI0~7
 */
#define __gpio_as_tssi_2()			\
do {						\
	REG_GPIO_PXFUNS(1) = 0xfff00000;	\
	REG_GPIO_PXTRGC(1) = 0x0fc00000;	\
	REG_GPIO_PXTRGS(1) = 0xf0300000;	\
	REG_GPIO_PXSELC(1) = 0xfff00000;	\
	REG_GPIO_PXPES(1)  = 0xfff00000;	\
} while (0)

/*
 * SSI_CE0, SSI_CE1, SSI_GPC, SSI_CLK, SSI_DT, SSI_DR
 */
#define __gpio_as_ssi()				\
do {						\
	REG_GPIO_PXFUNS(0) = 0x002c0000; /* SSI0_CE0, SSI0_CLK, SSI0_DT	*/ \
	REG_GPIO_PXTRGS(0) = 0x002c0000;	\
	REG_GPIO_PXSELC(0) = 0x002c0000;	\
	REG_GPIO_PXPES(0)  = 0x002c0000;	\
						\
	REG_GPIO_PXFUNS(0) = 0x00100000; /* SSI0_DR */	\
	REG_GPIO_PXTRGC(0) = 0x00100000;	\
	REG_GPIO_PXSELS(0) = 0x00100000;	\
	REG_GPIO_PXPES(0)  = 0x00100000;	\
} while (0)

/*
 * SSI_CE0, SSI_CE2, SSI_GPC, SSI_CLK, SSI_DT, SSI1_DR
 */
#define __gpio_as_ssi_1()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0x0000fc00;	\
	REG_GPIO_PXTRGC(5) = 0x0000fc00;	\
	REG_GPIO_PXSELC(5) = 0x0000fc00;	\
	REG_GPIO_PXPES(5)  = 0x0000fc00;	\
} while (0)

/* Port B
 * SSI2_CE0, SSI2_CE2, SSI2_GPC, SSI2_CLK, SSI2_DT, SSI12_DR
 */
#define __gpio_as_ssi2_1()			\
do {						\
	REG_GPIO_PXFUNS(5) = 0xf0300000;	\
	REG_GPIO_PXTRGC(5) = 0xf0300000;	\
	REG_GPIO_PXSELS(5) = 0xf0300000;	\
	REG_GPIO_PXPES(5)  = 0xf0300000;	\
} while (0)

#define __gpio_as_pcm0() \
do {						\
	REG_GPIO_PXFUNS(3) = 0xf;	\
	REG_GPIO_PXTRGC(3) = 0xf;	\
	REG_GPIO_PXSELC(3) = 0xf;	\
	REG_GPIO_PXPES(3)  = 0xf;	\
} while (0)

#define __gpio_as_pcm1() \
do {						\
	REG_GPIO_PXFUNS(3) = 0x1000;	\
	REG_GPIO_PXTRGS(3) = 0x1000;	\
	REG_GPIO_PXSELC(3) = 0x1000;	\
	REG_GPIO_PXPES(3)  = 0x1000;	\
	REG_GPIO_PXFUNS(4) = 0x1000;	\
	REG_GPIO_PXTRGC(4) = 0x300;	\
	REG_GPIO_PXTRGS(4) = 0x20;	\
	REG_GPIO_PXSELS(4) = 0x320;	\
	REG_GPIO_PXPES(4)  = 0x320;	\
} while (0)
/*
 * I2C_SCK, I2C_SDA
 */
#define __gpio_as_i2c(n)		       \
do {						\
	REG_GPIO_PXFUNS(3+(n)) = 0xc0000000;	\
	REG_GPIO_PXTRGC(3+(n)) = 0xc0000000;	\
	REG_GPIO_PXSELC(3+(n)) = 0xc0000000;	\
	REG_GPIO_PXPES(3+(n))  = 0xc0000000;	\
} while (0)

/*
 * PWM0
 */
#define __gpio_as_pwm0()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x1;	\
	REG_GPIO_PXTRGC(4) = 0x1;	\
	REG_GPIO_PXSELC(4) = 0x1;	\
	REG_GPIO_PXPES(4) = 0x1;		\
} while (0)

/*
 * PWM1
 */
#define __gpio_as_pwm1()			\
do {						\
	REG_GPIO_PXFUNS(4) = 0x2;	        \
	REG_GPIO_PXTRGC(4) = 0x2;		\
	REG_GPIO_PXSELC(4) = 0x2;		\
	REG_GPIO_PXPEC(4) = 0x2;		\
} while (0)

/*
 * PWM2
 */
#define __gpio_as_pwm2()		\
do {					\
	REG_GPIO_PXFUNS(4) = 0x4;	\
	REG_GPIO_PXTRGC(4) = 0x4;	\
	REG_GPIO_PXSELC(4) = 0x4;	\
	REG_GPIO_PXPES(4) = 0x4;	\
} while (0)

/*
 * PWM3
 */
#define __gpio_as_pwm3()		\
do {					\
	REG_GPIO_PXFUNS(4) = 0x8;	\
	REG_GPIO_PXTRGC(4) = 0x8;	\
	REG_GPIO_PXSELC(4) = 0x8;	\
	REG_GPIO_PXPES(4) = 0x8;	\
} while (0)

/*
 * PWM4
 */
#define __gpio_as_pwm4()		\
do {					\
	REG_GPIO_PXFUNS(4) = 0x10;	\
	REG_GPIO_PXTRGC(4) = 0x10;	\
	REG_GPIO_PXSELC(4) = 0x10;	\
	REG_GPIO_PXPES(4) = 0x10;	\
} while (0)

/*
 * PWM5
 */
#define __gpio_as_pwm5()		\
do {					\
	REG_GPIO_PXFUNS(4) = 0x20;	\
	REG_GPIO_PXTRGC(4) = 0x20;	\
	REG_GPIO_PXSELC(4) = 0x20;	\
	REG_GPIO_PXPES(4) = 0x20;	\
} while (0)

/*
 * n = 0 ~ 5
 */
#define __gpio_as_pwm(n)	__gpio_as_pwm##n()

/*
 * OWI - PA29 function 1
 */
#define __gpio_as_owi()				\
do {						\
	REG_GPIO_PXFUNS(0) = 0x20000000;	\
	REG_GPIO_PXTRGC(0) = 0x20000000;	\
	REG_GPIO_PXSELS(0) = 0x20000000;	\
} while (0)

/*
 * SCC - PD08 function 0
 *       PD09 function 0
 */
#define __gpio_as_scc()				\
do {						\
	REG_GPIO_PXFUNS(3) = 0xc0000300;	\
	REG_GPIO_PXTRGC(3) = 0xc0000300;	\
	REG_GPIO_PXSELC(3) = 0xc0000300;	\
} while (0)

#define __gpio_as_otg_drvvbus()	\
do {	\
	REG_GPIO_PXDATC(4) = (1 << 10);		\
	REG_GPIO_PXPEC(4) = (1 << 10);		\
	REG_GPIO_PXSELC(4) = (1 << 10);		\
	REG_GPIO_PXTRGC(4) = (1 << 10);		\
	REG_GPIO_PXFUNS(4) = (1 << 10);		\
} while (0)

//-------------------------------------------
// GPIO or Interrupt Mode

#define __gpio_get_port(p)	(REG_GPIO_PXPIN(p))

#define __gpio_port_as_output(p, o)		\
do {						\
    REG_GPIO_PXFUNC(p) = (1 << (o));		\
    REG_GPIO_PXSELC(p) = (1 << (o));		\
    REG_GPIO_PXDIRS(p) = (1 << (o));		\
    REG_GPIO_PXPES(p) = (1 << (o));		\
} while (0)

#define __gpio_port_as_input(p, o)		\
do {						\
    REG_GPIO_PXFUNC(p) = (1 << (o));		\
    REG_GPIO_PXSELC(p) = (1 << (o));		\
    REG_GPIO_PXDIRC(p) = (1 << (o));		\
} while (0)

#define __gpio_as_output(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_output(p, o);		\
} while (0)

#define __gpio_as_input(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	__gpio_port_as_input(p, o);		\
} while (0)

#define __gpio_set_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXDATS(p) = (1 << o);		\
} while (0)

#define __gpio_clear_pin(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXDATC(p) = (1 << o);		\
} while (0)

#define __gpio_get_pin(n)			\
({						\
	unsigned int p, o, v;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	if (__gpio_get_port(p) & (1 << o))	\
		v = 1;				\
	else					\
		v = 0;				\
	v;					\
})

#define __gpio_as_irq_high_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRS(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_low_level(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGC(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRC(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_rise_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRS(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_as_irq_fall_edge(n)		\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
	REG_GPIO_PXTRGS(p) = (1 << o);		\
	REG_GPIO_PXFUNC(p) = (1 << o);		\
	REG_GPIO_PXSELS(p) = (1 << o);		\
	REG_GPIO_PXDIRC(p) = (1 << o);		\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_mask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMS(p) = (1 << o);		\
} while (0)

#define __gpio_unmask_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXIMC(p) = (1 << o);		\
} while (0)

#define __gpio_ack_irq(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXFLGC(p) = (1 << o);		\
} while (0)

#define __gpio_get_irq()			\
({						\
	unsigned int p, i, tmp, v = 0;		\
	for (p = 5; p >= 0; p--) {		\
		tmp = REG_GPIO_PXFLG(p);	\
		for (i = 0; i < 32; i++)	\
			if (tmp & (1 << i))	\
				v = (32*p + i);	\
	}					\
	v;					\
})

#define __gpio_group_irq(n)			\
({						\
	register int tmp, i;			\
	tmp = REG_GPIO_PXFLG(n) & (~REG_GPIO_PXIM(n));	\
	for (i=31;i>=0;i--)			\
		if (tmp & (1 << i))		\
			break;			\
	i;					\
})

#define __gpio_enable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPEC(p) = (1 << o);		\
} while (0)

#define __gpio_disable_pull(n)			\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXPES(p) = (1 << o);		\
} while (0)

/* Set pin driver strength v: 0~7 */
#define __gpio_set_driving_strength(n, v)	\
do {						\
	unsigned int p, o, d;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	d = v & 0x1;				\
	if(d) REG_GPIO_PXDS0S(p) = (1 << o);	\
	else REG_GPIO_PXDS0C(p) = (1 << o);	\
	d = v & 0x2;				\
	if(d) REG_GPIO_PXDS1S(p) = (1 << o);	\
	else REG_GPIO_PXDS1C(p) = (1 << o);	\
	d = v & 0x4;				\
	if(d) REG_GPIO_PXDS2S(p) = (1 << o);	\
	else REG_GPIO_PXDS2C(p) = (1 << o);	\
} while(0)

/* Set fast slew rate */
#define __gpio_set_fslew(n)	\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLS(p) = (1 << o);	\
} while(0)

/* Set slow slew rate */
#define __gpio_set_sslew(n)	\
do {						\
	unsigned int p, o;			\
	p = (n) / 32;				\
	o = (n) % 32;				\
	REG_GPIO_PXSLC(p) = (1 << o);	\
} while(0)
#endif 

#endif /* __MIPS_ASSEMBLER */



#define A_GPIO_PXPIN(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPIN_OFFSET)
#define A_GPIO_PXDAT(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDAT_OFFSET)
#define A_GPIO_PXDATS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDATS_OFFSET)
#define A_GPIO_PXDATC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDATC_OFFSET)
#define A_GPIO_PXIM(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIM_OFFSET)
#define A_GPIO_PXIMS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIMS_OFFSET)
#define A_GPIO_PXIMC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXIMC_OFFSET)
#define A_GPIO_PXPE(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPE_OFFSET)
#define A_GPIO_PXPES(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPES_OFFSET)
#define A_GPIO_PXPEC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXPEC_OFFSET)
#define A_GPIO_PXFUN(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUN_OFFSET)
#define A_GPIO_PXFUNS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUNS_OFFSET)
#define A_GPIO_PXFUNC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFUNC_OFFSET)
#define A_GPIO_PXSEL(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSEL_OFFSET)
#define A_GPIO_PXSELS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSELS_OFFSET)
#define A_GPIO_PXSELC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXSELC_OFFSET)
#define A_GPIO_PXDIR(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDIR_OFFSET)
#define A_GPIO_PXDIRS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDIRS_OFFSET)
#define A_GPIO_PXDIRC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXDIRC_OFFSET)
#define A_GPIO_PXTRG(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXTRG_OFFSET)
#define A_GPIO_PXTRGS(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXTRGS_OFFSET)
#define A_GPIO_PXTRGC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXTRGC_OFFSET)
#define A_GPIO_PXFLG(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFLG_OFFSET)
#define A_GPIO_PXFLGC(n)	(GPIO_BASE + (n)*GPIO_GOS + GPIO_PXFLGC_OFFSET)


#endif /* __JZ4760GPIO_H__ */
