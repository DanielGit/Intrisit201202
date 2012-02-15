/*
 * jz4760wdt.h
 * JZ4760 WDT register definition
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 *
 * Author: whxu@ingenic.cn
 */

#ifndef __JZ4760WDT_H__
#define __JZ4760WDT_H__


/*
 * Watchdog timer module(WDT) address definition
 */
#define	WDT_BASE	0xb0002000


/*
 * WDT registers offset address definition
 */
#define WDT_WDR_OFFSET		(0x00)	/* rw, 16, 0x???? */
#define WDT_WCER_OFFSET		(0x04)	/* rw,  8, 0x00 */
#define WDT_WCNT_OFFSET		(0x08)	/* rw, 16, 0x???? */
#define WDT_WCSR_OFFSET		(0x0c)	/* rw, 16, 0x0000 */


/*
 * WDT registers address definition
 */
#define WDT_WDR		(WDT_BASE + WDT_WDR_OFFSET)
#define WDT_WCER	(WDT_BASE + WDT_WCER_OFFSET)
#define WDT_WCNT	(WDT_BASE + WDT_WCNT_OFFSET)
#define WDT_WCSR	(WDT_BASE + WDT_WCSR_OFFSET)


/*
 * WDT registers common define
 */

/* Watchdog counter enable register(WCER) */
#define WCER_TCEN	BIT0

/* Watchdog control register(WCSR) */
#define WCSR_PRESCALE_LSB	3
#define WCSR_PRESCALE_MASK	BITS_H2L(5, WCSR_PRESCALE_LSB)
#define WCSR_PRESCALE1		(0x0 << WCSR_PRESCALE_LSB)
#define WCSR_PRESCALE4		(0x1 << WCSR_PRESCALE_LSB)
#define WCSR_PRESCALE16		(0x2 << WCSR_PRESCALE_LSB)
#define WCSR_PRESCALE64		(0x3 << WCSR_PRESCALE_LSB)
#define WCSR_PRESCALE256	(0x4 << WCSR_PRESCALE_LSB)
#define WCSR_PRESCALE1024	(0x5 << WCSR_PRESCALE_LSB)

#define WCSR_CLKIN_LSB		0
#define WCSR_CLKIN_MASK		BITS_H2L(2, WCSR_CLKIN_LSB)
#define WCSR_CLKIN_PCK		(0x1 << WCSR_CLKIN_LSB)
#define WCSR_CLKIN_RTC		(0x2 << WCSR_CLKIN_LSB)
#define WCSR_CLKIN_EXT		(0x4 << WCSR_CLKIN_LSB)


#ifndef __MIPS_ASSEMBLER

#define REG_WDT_WDR	REG16(WDT_WDR)
#define REG_WDT_WCER	REG8(WDT_WCER)
#define REG_WDT_WCNT	REG16(WDT_WCNT)
#define REG_WDT_WCSR	REG16(WDT_WCSR)

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4760WDT_H__ */
