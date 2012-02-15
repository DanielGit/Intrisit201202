/*
 * linux/include/asm-mips/mach-jz4760/jz4760nemc.h
 *
 * JZ4760 NEMC register definition.
 *
 * Copyright (C) 2010 Ingenic Semiconductor Co., Ltd.
 */

#ifndef __JZ4760NEMC_H__
#define __JZ4760NEMC_H__


#define NEMC_BASE	0xB3410000

/*************************************************************************
 * NEMC (External Memory Controller for NAND)
 *************************************************************************/

#define NEMC_NFCSR	(NEMC_BASE + 0x050) /* NAND Flash Control/Status Register */
#define NEMC_SMCR	(NEMC_BASE + 0x14)  /* Static Memory Control Register 1 */


#define REG_NEMC_NFCSR	REG32(NEMC_NFCSR)
#define REG_NEMC_SMCR1	REG32(NEMC_SMCR)

// PN(bit 0):0-disable, 1-enable
// PN(bit 1):0-no reset, 1-reset
// (bit 2):Reserved
// BITCNT(bit 3):0-disable, 1-enable
// BITCNT(bit 4):0-calculate, 1's number, 1-calculate 0's number
// BITCNT(bit 5):0-no reset, 1-reset bitcnt
#define NEMC_PNCR 	(NEMC_BASE+0x100)
#define NEMC_PNDR 	(NEMC_BASE+0x104)
#define NEMC_BITCNT	(NEMC_BASE+0x108)

#define REG_NEMC_PNCR REG32(NEMC_PNCR)
#define REG_NEMC_PNDR REG32(NEMC_PNDR)
#define REG_NEMC_BITCNT REG32(NEMC_BITCNT)

#define REG_NEMC_SMCR	REG32(NEMC_SMCR)

/* NAND Flash Control/Status Register */
#define NEMC_NFCSR_NFCE4	(1 << 7) /* NAND Flash Enable */
#define NEMC_NFCSR_NFE4		(1 << 6) /* NAND Flash FCE# Assertion Enable */
#define NEMC_NFCSR_NFCE3	(1 << 5)
#define NEMC_NFCSR_NFE3		(1 << 4)
#define NEMC_NFCSR_NFCE2	(1 << 3)
#define NEMC_NFCSR_NFE2		(1 << 2)
#define NEMC_NFCSR_NFCE1	(1 << 1)
#define NEMC_NFCSR_NFE1		(1 << 0)


#ifndef __MIPS_ASSEMBLER

#endif /* __MIPS_ASSEMBLER */

#endif /* __JZ4760NEMC_H__ */
