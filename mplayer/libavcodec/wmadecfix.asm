	.file	1 "wmadecfix.c"
	.section .mdebug.abi32
	.previous
	.abicalls
	.text
	.align	2
	.globl	sqrtint
	.ent	sqrtint
	.type	sqrtint, @function
sqrtint:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	blez	$4,$L17
	sra	$3,$4,31

	move	$2,$4
	sll	$9,$3,16
	srl	$4,$4,16
	or	$9,$4,$9
	sll	$8,$2,16
	li	$2,2147418112			# 0x7fff0000
	ori	$15,$2,0xfffe
	li	$14,1			# 0x1
	move	$10,$9
	addu	$2,$14,$15
$L22:
	srl	$4,$2,31
	addu	$4,$4,$2
	sra	$5,$4,1
	sra	$3,$4,31
	mul	$4,$3,$5
	multu	$5,$5
	sll	$4,$4,1
	mfhi	$7
	addu	$7,$4,$7
	slt	$2,$9,$7
	bne	$2,$0,$L8
	mflo	$6

	beq	$10,$7,$L18
	sltu	$2,$8,$6

$L6:
	move	$14,$5
$L9:
	subu	$2,$15,$14
	slt	$2,$2,2
	beq	$2,$0,$L22
	addu	$2,$14,$15

	sra	$5,$14,31
	sra	$3,$15,31
	mul	$7,$5,$14
	mul	$6,$3,$15
	multu	$14,$14
	sll	$6,$6,1
	mflo	$4
	mfhi	$5
	multu	$15,$15
	sll	$7,$7,1
	mflo	$2
	mfhi	$3
	addu	$5,$7,$5
	addu	$3,$6,$3
	subu	$12,$8,$4
	subu	$10,$2,$8
	sltu	$4,$2,$10
	sltu	$6,$8,$12
	subu	$13,$9,$5
	subu	$11,$3,$9
	subu	$5,$13,$6
	subu	$11,$11,$4
	slt	$2,$5,$11
	beq	$2,$0,$L19
	nop

$L4:
	j	$31
	move	$2,$14

$L18:
	beq	$2,$0,$L6
	nop

$L8:
	b	$L9
	move	$15,$5

$L19:
	beq	$5,$11,$L20
	nop

	move	$14,$15
$L21:
	j	$31
	move	$2,$14

$L20:
	sltu	$2,$12,$10
	bne	$2,$0,$L4
	nop

	b	$L21
	move	$14,$15

$L17:
	move	$14,$0
	j	$31
	move	$2,$14

	.set	macro
	.set	reorder
	.end	sqrtint
	.align	2
	.globl	Calc_cmul_4750
	.ent	Calc_cmul_4750
	.type	Calc_cmul_4750, @function
Calc_cmul_4750:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lw	$2,16($sp)
#APP
	S32MUL xr1,xr2,$5,$7
	S32MSUB xr1,xr2,$6,$2
	S32MUL xr3,xr4,$5,$2
	S32MADD xr3,xr4,$7,$6
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $2
#NO_APP
	sw	$2,0($4)
#APP
	S32M2I xr3, $3
#NO_APP
	.set	noreorder
	.set	nomacro
	j	$31
	sw	$3,4($4)
	.set	macro
	.set	reorder

	.end	Calc_cmul_4750
	.align	2
	.globl	Calc_cmul_4740
	.ent	Calc_cmul_4740
	.type	Calc_cmul_4740, @function
Calc_cmul_4740:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lw	$3,16($sp)
#APP
	mult $5,$7
msub $6,$3
mfhi $2

#NO_APP
	sll	$2,$2,1
	sw	$2,0($4)
#APP
	mult $5,$3
madd $6,$7
mfhi $5

#NO_APP
	sll	$5,$5,1
	.set	noreorder
	.set	nomacro
	j	$31
	sw	$5,4($4)
	.set	macro
	.set	reorder

	.end	Calc_cmul_4740
	.align	2
	.ent	split_radix_permutation
	.type	split_radix_permutation, @function
split_radix_permutation:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	slt	$2,$5,3
	bne	$2,$0,$L41
	move	$3,$5

	move	$17,$0
	b	$L33
	li	$16,1			# 0x1

$L42:
	move	$3,$7
	slt	$2,$3,3
	bne	$2,$0,$L31
	sll	$16,$16,1

$L33:
	sra	$5,$3,2
	sra	$7,$3,1
	and	$2,$5,$4
	sltu	$8,$2,1
	and	$2,$7,$4
	beq	$2,$0,$L42
	move	$3,$5

	bne	$8,$6,$L37
	slt	$2,$3,3

	addu	$17,$17,$16
	beq	$2,$0,$L33
	sll	$16,$16,2

$L31:
	andi	$2,$4,0x1
	mul	$3,$2,$16
	lw	$31,32($sp)
	addu	$2,$3,$17
	lw	$16,24($sp)
	lw	$17,28($sp)
	j	$31
	addiu	$sp,$sp,40

$L37:
	lw	$25,%got(split_radix_permutation)($28)
	addiu	$25,$25,%lo(split_radix_permutation)
	jalr	$25
	nop

	sll	$2,$2,2
	addiu	$2,$2,-1
	mul	$3,$2,$16
	lw	$28,16($sp)
	addu	$2,$3,$17
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,40

$L41:
	move	$17,$0
	b	$L31
	li	$16,1			# 0x1

	.set	macro
	.set	reorder
	.end	split_radix_permutation
	.align	2
	.ent	pass_4740
	.type	pass_4740, @function
pass_4740:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x00030000,-4
	.fmask	0x00000000,0
	sll	$13,$6,3
	sll	$10,$6,1
	subu	$10,$13,$10
	addiu	$sp,$sp,-8
	sll	$12,$6,5
	sll	$10,$10,3
	sw	$17,4($sp)
	sw	$16,0($sp)
	addu	$12,$12,$4
	addu	$10,$4,$10
	sll	$11,$6,4
	move	$24,$4
	addu	$11,$4,$11
	lw	$7,0($12)
	lw	$4,0($10)
	lw	$2,0($24)
	lw	$3,4($11)
	addu	$8,$7,$4
	subu	$4,$4,$7
	subu	$2,$2,$8
	subu	$3,$3,$4
	sw	$2,0($12)
	lw	$7,4($12)
	lw	$9,4($10)
	sw	$3,4($10)
	lw	$2,0($24)
	lw	$3,4($11)
	addu	$2,$2,$8
	addu	$3,$3,$4
	sw	$2,0($24)
	sw	$3,4($11)
	lw	$2,0($11)
	lw	$3,4($24)
	addu	$4,$7,$9
	subu	$7,$7,$9
	subu	$2,$2,$7
	subu	$3,$3,$4
	sw	$2,0($10)
	sw	$3,4($12)
	lw	$2,0($11)
	lw	$3,4($24)
	addu	$2,$2,$7
	addu	$3,$3,$4
	sw	$2,0($11)
	sw	$3,4($24)
	addu	$16,$13,$5
	addiu	$15,$12,8
	move	$25,$5
	addiu	$17,$6,-1
	lw	$7,8($12)
	lw	$2,4($15)
	lw	$4,4($5)
	lw	$3,-4($16)
#APP
	mult $7,$4
madd $2,$3
mfhi $8

#NO_APP
	sll	$8,$8,1
#APP
	mult $2,$4
msub $7,$3
mfhi $7

#NO_APP
	addiu	$14,$10,8
	sll	$7,$7,1
	lw	$6,8($10)
	lw	$2,4($14)
#APP
	mult $6,$4
msub $2,$3
mfhi $5

#NO_APP
	sll	$5,$5,1
#APP
	mult $2,$4
madd $6,$3
mfhi $6

#NO_APP
	addiu	$9,$11,8
	lw	$4,8($24)
	lw	$2,4($9)
	addu	$13,$8,$5
	subu	$5,$5,$8
	subu	$2,$2,$5
	subu	$4,$4,$13
	sw	$2,4($14)
	sw	$4,8($12)
	lw	$3,8($24)
	lw	$2,4($9)
	addu	$3,$3,$13
	addu	$2,$2,$5
	sw	$2,4($9)
	sw	$3,8($24)
	addiu	$5,$24,8
	lw	$4,8($11)
	lw	$2,4($5)
	sll	$6,$6,1
	addu	$8,$7,$6
	subu	$7,$7,$6
	subu	$2,$2,$8
	subu	$4,$4,$7
	sw	$2,4($15)
	sw	$4,8($10)
	lw	$3,8($11)
	lw	$2,4($5)
	addu	$3,$3,$7
	addu	$2,$2,$8
	sw	$2,4($5)
	sw	$3,8($11)
$L44:
	addiu	$25,$25,8
	addiu	$16,$16,-8
	addiu	$24,$24,16
	lw	$8,16($12)
	lw	$2,20($12)
	lw	$4,0($25)
	lw	$3,0($16)
#APP
	mult $8,$4
madd $2,$3
mfhi $9

#NO_APP
	sll	$9,$9,1
#APP
	mult $2,$4
msub $8,$3
mfhi $8

#NO_APP
	sll	$8,$8,1
	lw	$6,16($10)
	lw	$2,20($10)
#APP
	mult $6,$4
msub $2,$3
mfhi $5

#NO_APP
	sll	$5,$5,1
#APP
	mult $2,$4
madd $6,$3
mfhi $6

#NO_APP
	lw	$7,0($24)
	lw	$2,20($11)
	addu	$4,$9,$5
	subu	$5,$5,$9
	subu	$7,$7,$4
	subu	$2,$2,$5
	sw	$2,20($10)
	sw	$7,16($12)
	lw	$2,0($24)
	lw	$3,20($11)
	addu	$2,$2,$4
	addu	$3,$3,$5
	sw	$2,0($24)
	sw	$3,20($11)
	lw	$2,16($11)
	lw	$3,4($24)
	sll	$6,$6,1
	addu	$4,$8,$6
	subu	$8,$8,$6
	subu	$2,$2,$8
	subu	$3,$3,$4
	sw	$2,16($10)
	sw	$3,20($12)
	lw	$2,16($11)
	lw	$3,4($24)
	addu	$2,$2,$8
	addu	$3,$3,$4
	sw	$2,16($11)
	sw	$3,4($24)
	lw	$8,24($12)
	lw	$2,28($12)
	lw	$4,4($25)
	lw	$3,-4($16)
#APP
	mult $8,$4
madd $2,$3
mfhi $9

#NO_APP
	sll	$9,$9,1
#APP
	mult $2,$4
msub $8,$3
mfhi $8

#NO_APP
	sll	$8,$8,1
	lw	$6,24($10)
	lw	$2,28($10)
#APP
	mult $6,$4
msub $2,$3
mfhi $5

#NO_APP
	sll	$5,$5,1
#APP
	mult $2,$4
madd $6,$3
mfhi $6

#NO_APP
	lw	$7,28($11)
	lw	$2,8($24)
	addu	$4,$9,$5
	subu	$5,$5,$9
	subu	$2,$2,$4
	subu	$7,$7,$5
	sw	$2,24($12)
	sw	$7,28($10)
	lw	$2,28($11)
	lw	$3,8($24)
	addu	$2,$2,$5
	addu	$3,$3,$4
	sw	$2,28($11)
	sw	$3,8($24)
	lw	$2,24($11)
	lw	$3,12($24)
	sll	$6,$6,1
	addu	$4,$8,$6
	subu	$8,$8,$6
	subu	$2,$2,$8
	subu	$3,$3,$4
	sw	$2,24($10)
	sw	$3,28($12)
	lw	$2,24($11)
	lw	$3,12($24)
	addu	$2,$2,$8
	addu	$3,$3,$4
	addiu	$17,$17,-1
	sw	$2,24($11)
	sw	$3,12($24)
	addiu	$12,$12,16
	addiu	$10,$10,16
	.set	noreorder
	.set	nomacro
	bne	$17,$0,$L44
	addiu	$11,$11,16
	.set	macro
	.set	reorder

	lw	$17,4($sp)
	lw	$16,0($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,8
	.set	macro
	.set	reorder

	.end	pass_4740
	.align	2
	.ent	fft4_4740
	.type	fft4_4740, @function
fft4_4740:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x00030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$17,4($sp)
	sw	$16,0($sp)
	addiu	$17,$4,8
	addiu	$16,$4,16
	addiu	$15,$4,24
	lw	$14,4($17)
	lw	$7,4($16)
	lw	$12,4($15)
	lw	$2,4($4)
	lw	$8,16($4)
	lw	$5,24($4)
	lw	$10,8($4)
	lw	$3,0($4)
	addu	$11,$5,$8
	addu	$6,$2,$14
	addu	$13,$7,$12
	subu	$2,$2,$14
	subu	$5,$5,$8
	addu	$9,$3,$10
	addu	$8,$5,$2
	subu	$3,$3,$10
	subu	$2,$2,$5
	addu	$10,$6,$13
	subu	$6,$6,$13
	sw	$2,4($15)
	sw	$8,4($17)
	sw	$6,4($16)
	subu	$7,$7,$12
	lw	$17,4($sp)
	lw	$16,0($sp)
	addu	$14,$9,$11
	addu	$12,$3,$7
	subu	$9,$9,$11
	subu	$3,$3,$7
	addiu	$sp,$sp,8
	sw	$10,4($4)
	sw	$9,16($4)
	sw	$14,0($4)
	sw	$3,24($4)
	j	$31
	sw	$12,8($4)

	.set	macro
	.set	reorder
	.end	fft4_4740
	.align	2
	.ent	fft8_4740
	.type	fft8_4740, @function
fft8_4740:
	.frame	$sp,40,$31		# vars= 0, regs= 4/0, args= 16, gp= 8
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft4_4740)($28)
	addiu	$25,$25,%lo(fft4_4740)
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$16,$4
	.set	macro
	.set	reorder

	addiu	$18,$16,40
	addiu	$5,$16,32
	lw	$8,4($5)
	lw	$3,4($18)
	lw	$28,16($sp)
	subu	$2,$8,$3
	sw	$2,4($18)
	addiu	$17,$16,48
	addiu	$15,$16,56
	lw	$4,4($15)
	lw	$12,4($17)
	addu	$8,$8,$3
	subu	$2,$12,$4
	sw	$2,4($15)
	lw	$3,4($16)
	addu	$12,$12,$4
	addu	$6,$8,$12
	subu	$3,$3,$6
	sw	$3,4($5)
	lw	$2,4($16)
	lw	$5,32($16)
	lw	$14,40($16)
	lw	$13,56($16)
	addu	$2,$2,$6
	lw	$6,48($16)
	sw	$2,4($16)
	addiu	$9,$16,16
	lw	$2,4($9)
	addu	$7,$5,$14
	addu	$10,$6,$13
	subu	$11,$10,$7
	subu	$2,$2,$11
	sw	$2,4($17)
	lw	$3,4($9)
	lw	$2,0($16)
	lw	$4,16($16)
	addu	$7,$7,$10
	addu	$3,$3,$11
	sw	$3,4($9)
	subu	$8,$8,$12
	addu	$3,$2,$7
	subu	$2,$2,$7
	addu	$9,$4,$8
	subu	$5,$5,$14
	subu	$6,$6,$13
	subu	$4,$4,$8
	sw	$2,32($16)
	li	$2,1518469120			# 0x5a820000
	sw	$3,0($16)
	sw	$4,48($16)
	sw	$9,16($16)
	sw	$5,40($16)
	sw	$6,56($16)
	ori	$2,$2,0x7999
	lw	$3,4($18)
#APP
	mult $5,$2
madd $3,$2
mfhi $7

#NO_APP
	sll	$7,$7,1
#APP
	mult $3,$2
msub $5,$2
mfhi $5

#NO_APP
	sll	$5,$5,1
	lw	$3,4($15)
#APP
	mult $6,$2
msub $3,$2
mfhi $4

#NO_APP
	sll	$4,$4,1
#APP
	mult $3,$2
madd $6,$2
mfhi $6

#NO_APP
	addiu	$17,$16,8
	addiu	$13,$16,24
	lw	$3,4($17)
	lw	$2,4($13)
	lw	$8,8($16)
	lw	$9,24($16)
	sll	$6,$6,1
	addu	$10,$7,$4
	addu	$11,$5,$6
	subu	$4,$4,$7
	subu	$5,$5,$6
	addu	$14,$8,$10
	addu	$7,$2,$4
	addu	$6,$9,$5
	addu	$12,$3,$11
	subu	$8,$8,$10
	subu	$2,$2,$4
	subu	$9,$9,$5
	subu	$3,$3,$11
	sw	$2,4($15)
	sw	$6,24($16)
	sw	$7,4($13)
	sw	$8,40($16)
	sw	$3,4($18)
	sw	$14,8($16)
	sw	$12,4($17)
	sw	$9,56($16)
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,40
	.set	macro
	.set	reorder

	.end	fft8_4740
	.align	2
	.ent	fft16_4740
	.type	fft16_4740, @function
fft16_4740:
	.frame	$sp,48,$31		# vars= 0, regs= 5/0, args= 16, gp= 8
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-48
	sw	$31,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft8_4740)($28)
	addiu	$25,$25,%lo(fft8_4740)
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$16,$4
	.set	macro
	.set	reorder

	lw	$28,16($sp)
	addiu	$18,$16,64
	lw	$2,%got(fft4_4740)($28)
	addiu	$17,$16,96
	addiu	$19,$2,%lo(fft4_4740)
	move	$25,$19
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$18
	.set	macro
	.set	reorder

	move	$25,$19
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$9,96($16)
	lw	$5,64($16)
	addiu	$13,$16,32
	lw	$8,4($18)
	lw	$12,4($17)
	lw	$2,4($13)
	lw	$3,4($16)
	subu	$4,$9,$5
	lw	$6,0($16)
	addu	$10,$8,$12
	lw	$7,32($16)
	addu	$5,$5,$9
	addu	$11,$2,$4
	subu	$2,$2,$4
	lw	$28,16($sp)
	addu	$9,$3,$10
	sw	$2,4($17)
	subu	$8,$8,$12
	addu	$2,$6,$5
	subu	$3,$3,$10
	sw	$11,4($13)
	subu	$6,$6,$5
	sw	$3,4($18)
	addiu	$19,$16,80
	addu	$3,$7,$8
	sw	$9,4($16)
	subu	$7,$7,$8
	sw	$2,0($16)
	li	$2,1518469120			# 0x5a820000
	sw	$3,32($16)
	sw	$6,64($16)
	sw	$7,96($16)
	ori	$2,$2,0x7999
	lw	$5,80($16)
	lw	$3,4($19)
#APP
	mult $5,$2
madd $3,$2
mfhi $7

#NO_APP
	sll	$7,$7,1
#APP
	mult $3,$2
msub $5,$2
mfhi $5

#NO_APP
	addiu	$18,$16,112
	sll	$5,$5,1
	lw	$4,112($16)
	lw	$3,4($18)
#APP
	mult $4,$2
msub $3,$2
mfhi $6

#NO_APP
	sll	$6,$6,1
#APP
	mult $3,$2
madd $4,$2
mfhi $4

#NO_APP
	addiu	$17,$16,16
	addiu	$13,$16,48
	lw	$3,4($17)
	lw	$2,4($13)
	lw	$8,16($16)
	lw	$9,48($16)
	sll	$4,$4,1
	addu	$10,$7,$6
	addu	$11,$5,$4
	subu	$6,$6,$7
	subu	$5,$5,$4
	addu	$15,$9,$5
	addu	$4,$2,$6
	addu	$14,$8,$10
	addu	$7,$3,$11
	lw	$12,%got(ff_cos_16)($28)
	subu	$2,$2,$6
	subu	$9,$9,$5
	subu	$8,$8,$10
	subu	$3,$3,$11
	sw	$2,4($18)
	sw	$8,80($16)
	sw	$4,4($13)
	sw	$14,16($16)
	sw	$3,4($19)
	sw	$9,112($16)
	sw	$7,4($17)
	sw	$15,48($16)
	addiu	$19,$16,72
	lw	$6,12($12)
	lw	$5,72($16)
	lw	$2,4($19)
	lw	$4,4($12)
#APP
	mult $5,$4
madd $2,$6
mfhi $8

#NO_APP
	sll	$8,$8,1
#APP
	mult $2,$4
msub $5,$6
mfhi $5

#NO_APP
	addiu	$18,$16,104
	sll	$5,$5,1
	lw	$3,104($16)
	lw	$2,4($18)
#APP
	mult $3,$4
msub $2,$6
mfhi $7

#NO_APP
	sll	$7,$7,1
#APP
	mult $2,$4
madd $3,$6
mfhi $3

#NO_APP
	addiu	$17,$16,8
	addiu	$15,$16,40
	lw	$9,4($17)
	lw	$2,4($15)
	sll	$3,$3,1
	addu	$13,$5,$3
	subu	$10,$7,$8
	lw	$11,8($16)
	lw	$12,40($16)
	addu	$8,$8,$7
	addu	$14,$2,$10
	addu	$7,$9,$13
	subu	$5,$5,$3
	subu	$2,$2,$10
	subu	$9,$9,$13
	sw	$2,4($18)
	addu	$3,$12,$5
	sw	$14,4($15)
	addu	$2,$11,$8
	sw	$9,4($19)
	subu	$12,$12,$5
	sw	$7,4($17)
	subu	$11,$11,$8
	addiu	$18,$16,88
	sw	$2,8($16)
	sw	$11,72($16)
	sw	$12,104($16)
	sw	$3,40($16)
	lw	$5,88($16)
	lw	$2,4($18)
#APP
	mult $5,$6
madd $2,$4
mfhi $8

#NO_APP
	sll	$8,$8,1
#APP
	mult $2,$6
msub $5,$4
mfhi $5

#NO_APP
	addiu	$17,$16,120
	sll	$5,$5,1
	lw	$3,120($16)
	lw	$2,4($17)
#APP
	mult $3,$6
msub $2,$4
mfhi $7

#NO_APP
	sll	$7,$7,1
#APP
	mult $2,$6
madd $3,$4
mfhi $3

#NO_APP
	addiu	$15,$16,24
	addiu	$13,$16,56
	lw	$4,4($15)
	lw	$2,4($13)
	lw	$6,24($16)
	lw	$9,56($16)
	sll	$3,$3,1
	addu	$10,$8,$7
	addu	$11,$5,$3
	subu	$7,$7,$8
	subu	$5,$5,$3
	addu	$14,$6,$10
	addu	$8,$2,$7
	addu	$3,$9,$5
	addu	$12,$4,$11
	subu	$6,$6,$10
	subu	$2,$2,$7
	subu	$9,$9,$5
	subu	$4,$4,$11
	sw	$2,4($17)
	sw	$3,56($16)
	sw	$8,4($13)
	sw	$6,88($16)
	sw	$4,4($18)
	sw	$14,24($16)
	sw	$12,4($15)
	sw	$9,120($16)
	lw	$31,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	fft16_4740
	.align	2
	.ent	fft32_4740
	.type	fft32_4740, @function
fft32_4740:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft16_4740)($28)
	addiu	$25,$25,%lo(fft16_4740)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft8_4740)($28)
	addiu	$17,$2,%lo(fft8_4740)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,128

	move	$25,$17
	jalr	$25
	addiu	$4,$16,192

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4740)($28)
	lw	$5,%got(ff_cos_32)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,4			# 0x4
	addiu	$25,$25,%lo(pass_4740)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft32_4740
	.align	2
	.ent	fft64_4740
	.type	fft64_4740, @function
fft64_4740:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft32_4740)($28)
	addiu	$25,$25,%lo(fft32_4740)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft16_4740)($28)
	addiu	$17,$2,%lo(fft16_4740)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,256

	move	$25,$17
	jalr	$25
	addiu	$4,$16,384

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4740)($28)
	lw	$5,%got(ff_cos_64)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,8			# 0x8
	addiu	$25,$25,%lo(pass_4740)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft64_4740
	.align	2
	.ent	fft128_4740
	.type	fft128_4740, @function
fft128_4740:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft64_4740)($28)
	addiu	$25,$25,%lo(fft64_4740)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft32_4740)($28)
	addiu	$17,$2,%lo(fft32_4740)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,512

	move	$25,$17
	jalr	$25
	addiu	$4,$16,768

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4740)($28)
	lw	$5,%got(ff_cos_128)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,16			# 0x10
	addiu	$25,$25,%lo(pass_4740)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft128_4740
	.align	2
	.ent	fft256_4740
	.type	fft256_4740, @function
fft256_4740:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft128_4740)($28)
	addiu	$25,$25,%lo(fft128_4740)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft64_4740)($28)
	addiu	$17,$2,%lo(fft64_4740)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,1024

	move	$25,$17
	jalr	$25
	addiu	$4,$16,1536

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4740)($28)
	lw	$5,%got(ff_cos_256)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,32			# 0x20
	addiu	$25,$25,%lo(pass_4740)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft256_4740
	.align	2
	.ent	fft512_4740
	.type	fft512_4740, @function
fft512_4740:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft256_4740)($28)
	addiu	$25,$25,%lo(fft256_4740)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft128_4740)($28)
	addiu	$17,$2,%lo(fft128_4740)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,2048

	move	$25,$17
	jalr	$25
	addiu	$4,$16,3072

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4740)($28)
	lw	$5,%got(ff_cos_512)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,64			# 0x40
	addiu	$25,$25,%lo(pass_4740)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft512_4740
	.align	2
	.ent	fft1024_4740
	.type	fft1024_4740, @function
fft1024_4740:
	.frame	$sp,48,$31		# vars= 0, regs= 5/0, args= 16, gp= 8
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-48
	sw	$31,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft512_4740)($28)
	addiu	$25,$25,%lo(fft512_4740)
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$19,$4
	.set	macro
	.set	reorder

	lw	$28,16($sp)
	addiu	$17,$19,4096
	lw	$2,%got(fft256_4740)($28)
	addiu	$16,$19,6144
	addiu	$18,$2,%lo(fft256_4740)
	move	$25,$18
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$17
	.set	macro
	.set	reorder

	move	$25,$18
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$10,4($16)
	lw	$4,4($17)
	lw	$8,4096($19)
	lw	$2,6144($19)
	addiu	$13,$19,2048
	lw	$28,16($sp)
	lw	$6,4($19)
	lw	$3,4($13)
	lw	$5,0($19)
	lw	$7,2048($19)
	addu	$9,$8,$2
	addu	$11,$4,$10
	subu	$2,$2,$8
	subu	$4,$4,$10
	addu	$8,$3,$2
	addu	$15,$7,$4
	addu	$14,$5,$9
	addu	$10,$6,$11
	lw	$12,%got(ff_cos_1024)($28)
	subu	$3,$3,$2
	subu	$7,$7,$4
	subu	$6,$6,$11
	subu	$5,$5,$9
	sw	$3,4($16)
	sw	$5,4096($19)
	sw	$8,4($13)
	sw	$14,0($19)
	sw	$6,4($17)
	sw	$7,6144($19)
	sw	$10,4($19)
	sw	$15,2048($19)
	addiu	$18,$19,4104
	lw	$6,4104($19)
	lw	$2,4($18)
	lw	$4,4($12)
	lw	$3,1020($12)
#APP
	mult $6,$4
madd $2,$3
mfhi $8

#NO_APP
	sll	$8,$8,1
#APP
	mult $2,$4
msub $6,$3
mfhi $6

#NO_APP
	addiu	$17,$19,6152
	sll	$6,$6,1
	lw	$5,6152($19)
	lw	$2,4($17)
#APP
	mult $5,$4
msub $2,$3
mfhi $7

#NO_APP
	sll	$7,$7,1
#APP
	mult $2,$4
madd $5,$3
mfhi $5

#NO_APP
	addiu	$16,$19,8
	addiu	$13,$19,2056
	lw	$3,4($16)
	lw	$2,4($13)
	sll	$5,$5,1
	lw	$4,8($19)
	lw	$9,2056($19)
	addu	$10,$8,$7
	addu	$11,$6,$5
	subu	$7,$7,$8
	subu	$6,$6,$5
	addu	$14,$4,$10
	addu	$15,$9,$6
	addu	$5,$2,$7
	addu	$8,$3,$11
	subu	$4,$4,$10
	subu	$2,$2,$7
	subu	$9,$9,$6
	subu	$3,$3,$11
	sw	$2,4($17)
	sw	$14,8($19)
	sw	$5,4($13)
	sw	$15,2056($19)
	sw	$3,4($18)
	move	$24,$12
	sw	$8,4($16)
	sw	$4,4104($19)
	sw	$9,6152($19)
	addiu	$14,$12,1024
	addiu	$15,$12,1016
$L66:
	addiu	$19,$19,16
	addiu	$24,$24,8
	addiu	$14,$14,-8
	lw	$7,4096($19)
	lw	$2,4100($19)
	lw	$4,0($24)
	lw	$3,0($14)
#APP
	mult $7,$4
madd $2,$3
mfhi $8

#NO_APP
	sll	$8,$8,1
#APP
	mult $2,$4
msub $7,$3
mfhi $7

#NO_APP
	sll	$7,$7,1
	lw	$5,6144($19)
	lw	$2,6148($19)
#APP
	mult $5,$4
msub $2,$3
mfhi $6

#NO_APP
	sll	$6,$6,1
#APP
	mult $2,$4
madd $5,$3
mfhi $5

#NO_APP
	lw	$3,0($19)
	lw	$9,4($19)
	lw	$4,2048($19)
	lw	$2,2052($19)
	sll	$5,$5,1
	addu	$11,$7,$5
	addu	$10,$8,$6
	subu	$7,$7,$5
	subu	$6,$6,$8
	addu	$12,$4,$7
	addu	$8,$3,$10
	addu	$5,$2,$6
	addu	$13,$9,$11
	subu	$3,$3,$10
	subu	$2,$2,$6
	subu	$4,$4,$7
	subu	$9,$9,$11
	sw	$2,6148($19)
	sw	$3,4096($19)
	sw	$8,0($19)
	sw	$5,2052($19)
	sw	$4,6144($19)
	sw	$12,2048($19)
	sw	$9,4100($19)
	sw	$13,4($19)
	lw	$7,4104($19)
	lw	$2,4108($19)
	lw	$4,4($24)
	lw	$3,-4($14)
#APP
	mult $7,$4
madd $2,$3
mfhi $8

#NO_APP
	sll	$8,$8,1
#APP
	mult $2,$4
msub $7,$3
mfhi $7

#NO_APP
	sll	$7,$7,1
	lw	$5,6152($19)
	lw	$2,6156($19)
#APP
	mult $5,$4
msub $2,$3
mfhi $6

#NO_APP
	sll	$6,$6,1
#APP
	mult $2,$4
madd $5,$3
mfhi $5

#NO_APP
	sll	$5,$5,1
	lw	$3,8($19)
	lw	$9,12($19)
	lw	$4,2056($19)
	lw	$2,2060($19)
	addu	$11,$7,$5
	addu	$10,$8,$6
	subu	$7,$7,$5
	subu	$6,$6,$8
	addu	$5,$2,$6
	addu	$8,$3,$10
	addu	$12,$4,$7
	addu	$13,$9,$11
	subu	$3,$3,$10
	subu	$2,$2,$6
	subu	$4,$4,$7
	subu	$9,$9,$11
	sw	$3,4104($19)
	sw	$8,8($19)
	sw	$2,6156($19)
	sw	$5,2060($19)
	sw	$4,6152($19)
	sw	$12,2056($19)
	sw	$9,4108($19)
	.set	noreorder
	.set	nomacro
	bne	$24,$15,$L66
	sw	$13,12($19)
	.set	macro
	.set	reorder

	lw	$31,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	fft1024_4740
	.align	2
	.globl	ff_fft_calc_fixed_4740
	.ent	ff_fft_calc_fixed_4740
	.type	ff_fft_calc_fixed_4740, @function
ff_fft_calc_fixed_4740:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	lw	$2,0($4)
	lw	$3,%got(fft_dispatch_4740)($28)
	sll	$2,$2,2
	addiu	$3,$3,%lo(fft_dispatch_4740)
	addu	$2,$2,$3
	lw	$25,-8($2)
	jr	$25
	move	$4,$5

	.set	macro
	.set	reorder
	.end	ff_fft_calc_fixed_4740
	.align	2
	.globl	ff_imdct_half_fixed_4740
	.ent	ff_imdct_half_fixed_4740
	.type	ff_imdct_half_fixed_4740, @function
ff_imdct_half_fixed_4740:
	.frame	$sp,112,$31		# vars= 40, regs= 10/0, args= 24, gp= 8
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-112
	sw	$31,108($sp)
	sw	$fp,104($sp)
	sw	$23,100($sp)
	sw	$22,96($sp)
	sw	$21,92($sp)
	sw	$20,88($sp)
	sw	$19,84($sp)
	sw	$18,80($sp)
	sw	$17,76($sp)
	sw	$16,72($sp)
	.cprestore	24
	lw	$3,4($4)
	move	$8,$4
	li	$2,1			# 0x1
	sll	$2,$2,$3
	move	$fp,$5
	lw	$3,24($4)
	lw	$5,12($8)
	lw	$4,8($4)
	sra	$22,$2,2
	sra	$23,$2,3
	sw	$4,52($sp)
	blez	$22,$L74
	sw	$5,48($sp)

	sra	$2,$2,1
	sll	$2,$2,2
	addu	$2,$6,$2
	addiu	$20,$2,-4
	move	$19,$3
	move	$18,$6
	move	$17,$4
	move	$16,$5
	move	$21,$0
$L76:
	lhu	$4,0($19)
	lw	$2,0($16)
	lw	$5,0($20)
	lw	$7,0($17)
	lw	$6,0($18)
	lw	$25,%call16(Calc_cmul_4740)($28)
	sll	$4,$4,3
	sw	$8,64($sp)
	sw	$2,16($sp)
	addu	$4,$fp,$4
	jalr	$25
	addiu	$21,$21,1

	addiu	$18,$18,8
	lw	$28,24($sp)
	addiu	$20,$20,-8
	addiu	$19,$19,2
	addiu	$17,$17,4
	addiu	$16,$16,4
	bne	$22,$21,$L76
	lw	$8,64($sp)

$L74:
	lw	$25,%call16(ff_fft_calc_fixed_4740)($28)
	addiu	$4,$8,16
	jalr	$25
	move	$5,$fp

	blez	$23,$L80
	lw	$28,24($sp)

	lw	$5,52($sp)
	sll	$2,$23,2
	addu	$4,$5,$2
	lw	$5,48($sp)
	sll	$3,$23,3
	addu	$2,$5,$2
	addu	$3,$fp,$3
	move	$21,$2
	addiu	$19,$2,-4
	addiu	$2,$sp,32
	addiu	$17,$3,4
	move	$20,$4
	addiu	$16,$3,-8
	addiu	$18,$4,-4
	move	$22,$0
	sw	$2,56($sp)
	addiu	$fp,$sp,40
$L79:
	lw	$2,0($18)
	lw	$5,4($16)
	lw	$6,0($16)
	lw	$7,0($19)
	lw	$25,%call16(Calc_cmul_4740)($28)
	lw	$4,56($sp)
	jalr	$25
	sw	$2,16($sp)

	lw	$28,24($sp)
	lw	$2,0($20)
	lw	$5,0($17)
	lw	$6,-4($17)
	lw	$7,0($21)
	lw	$25,%call16(Calc_cmul_4740)($28)
	sw	$2,16($sp)
	jalr	$25
	move	$4,$fp

	lw	$2,32($sp)
	lw	$3,44($sp)
	lw	$28,24($sp)
	sw	$2,0($16)
	sw	$3,4($16)
	lw	$2,40($sp)
	lw	$3,36($sp)
	addiu	$22,$22,1
	sw	$2,-4($17)
	sw	$3,0($17)
	addiu	$16,$16,-8
	addiu	$19,$19,-4
	addiu	$18,$18,-4
	addiu	$17,$17,8
	addiu	$21,$21,4
	bne	$23,$22,$L79
	addiu	$20,$20,4

$L80:
	lw	$31,108($sp)
	lw	$fp,104($sp)
	lw	$23,100($sp)
	lw	$22,96($sp)
	lw	$21,92($sp)
	lw	$20,88($sp)
	lw	$19,84($sp)
	lw	$18,80($sp)
	lw	$17,76($sp)
	lw	$16,72($sp)
	j	$31
	addiu	$sp,$sp,112

	.set	macro
	.set	reorder
	.end	ff_imdct_half_fixed_4740
	.align	2
	.ent	pass_4750
	.type	pass_4750, @function
pass_4750:
	.frame	$sp,32,$31		# vars= 0, regs= 8/0, args= 0, gp= 0
	.mask	0x00ff0000,-4
	.fmask	0x00000000,0
	addiu	$sp,$sp,-32
	sw	$21,20($sp)
	sll	$10,$6,1
	sll	$21,$6,3
	subu	$10,$21,$10
	sll	$10,$10,3
	sll	$12,$6,5
	sw	$23,28($sp)
	sw	$22,24($sp)
	sw	$20,16($sp)
	sw	$19,12($sp)
	sw	$18,8($sp)
	sw	$17,4($sp)
	sw	$16,0($sp)
	move	$24,$4
	addu	$12,$12,$4
	addu	$20,$4,$10
	sll	$16,$6,4
	lw	$15,0($12)
	lw	$13,4($20)
	lw	$4,4($12)
	lw	$2,0($20)
	addu	$19,$24,$16
	lw	$7,0($24)
	lw	$9,4($24)
	lw	$8,0($19)
	lw	$3,4($19)
	addu	$14,$4,$13
	addu	$11,$15,$2
	subu	$4,$4,$13
	subu	$2,$2,$15
	addu	$17,$8,$4
	addu	$18,$9,$14
	addu	$15,$7,$11
	addu	$13,$3,$2
	subu	$8,$8,$4
	subu	$9,$9,$14
	subu	$7,$7,$11
	subu	$3,$3,$2
	sw	$7,0($12)
	sw	$3,4($20)
	sw	$15,0($24)
	sw	$13,4($19)
	sw	$8,0($20)
	sw	$9,4($12)
	sw	$17,0($19)
	move	$25,$5
	sw	$18,4($24)
	addu	$22,$21,$5
	addiu	$23,$6,-1
	addiu	$14,$12,8
	lw	$5,8($12)
	lw	$4,4($25)
#APP
	S32MUL xr1,xr2,$5,$4
#NO_APP
	lw	$2,4($14)
	lw	$3,-4($22)
#APP
	S32MADD xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$4
	S32MSUB xr3,xr4,$5,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $13
	S32M2I xr3, $7
#NO_APP
	addu	$10,$24,$10
	lw	$5,8($10)
	addiu	$10,$10,8
#APP
	S32MUL xr1,xr2,$5,$4
#NO_APP
	lw	$2,4($10)
#APP
	S32MSUB xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$4
	S32MADD xr3,xr4,$5,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $3
	S32M2I xr3, $11
#NO_APP
	addu	$16,$24,$16
	addiu	$21,$24,8
	addiu	$15,$16,8
	lw	$4,8($24)
	lw	$5,4($21)
	lw	$6,8($16)
	lw	$2,4($15)
	addu	$8,$13,$3
	addu	$9,$7,$11
	subu	$3,$3,$13
	subu	$7,$7,$11
	addu	$18,$6,$7
	addu	$17,$4,$8
	addu	$11,$2,$3
	addu	$13,$5,$9
	subu	$4,$4,$8
	subu	$2,$2,$3
	subu	$6,$6,$7
	subu	$5,$5,$9
	sw	$4,0($14)
	sw	$2,4($10)
	sw	$17,8($24)
	sw	$11,4($15)
	sw	$6,0($10)
	sw	$5,4($14)
	sw	$18,8($16)
	sw	$13,4($21)
	move	$18,$12
$L84:
	addiu	$25,$25,8
	addiu	$24,$24,16
	addiu	$22,$22,-8
	lw	$4,16($18)
	lw	$5,0($25)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,20($18)
	lw	$3,0($22)
#APP
	S32MADD xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MSUB xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $13
	S32M2I xr3, $7
#NO_APP
	lw	$4,16($20)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,20($20)
#APP
	S32MSUB xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MADD xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $2
	S32M2I xr3, $8
#NO_APP
	lw	$4,0($24)
	lw	$6,4($24)
	lw	$5,16($19)
	lw	$3,20($19)
	addu	$11,$7,$8
	addu	$9,$13,$2
	subu	$7,$7,$8
	subu	$2,$2,$13
	addu	$8,$3,$2
	addu	$13,$4,$9
	addu	$14,$5,$7
	addu	$15,$6,$11
	subu	$4,$4,$9
	subu	$5,$5,$7
	subu	$3,$3,$2
	subu	$6,$6,$11
	sw	$4,16($18)
	sw	$3,20($20)
	sw	$13,0($24)
	sw	$8,20($19)
	sw	$5,16($20)
	sw	$6,20($18)
	sw	$14,16($19)
	sw	$15,4($24)
	lw	$4,24($18)
	lw	$5,4($25)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,28($18)
	lw	$3,-4($22)
#APP
	S32MADD xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MSUB xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $15
	S32M2I xr3, $6
#NO_APP
	lw	$4,24($20)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,28($20)
#APP
	S32MSUB xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MADD xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $2
	S32M2I xr3, $8
#NO_APP
	lw	$5,24($19)
	lw	$3,28($19)
	lw	$4,8($24)
	lw	$7,12($24)
	addu	$11,$6,$8
	addu	$9,$15,$2
	subu	$6,$6,$8
	subu	$2,$2,$15
	addu	$16,$3,$2
	addu	$15,$4,$9
	addu	$8,$5,$6
	addu	$17,$7,$11
	subu	$4,$4,$9
	subu	$3,$3,$2
	subu	$5,$5,$6
	subu	$7,$7,$11
	addiu	$23,$23,-1
	sw	$4,24($18)
	sw	$3,28($20)
	sw	$15,8($24)
	sw	$16,28($19)
	sw	$5,24($20)
	sw	$7,28($18)
	sw	$8,24($19)
	sw	$17,12($24)
	addiu	$18,$18,16
	addiu	$20,$20,16
	.set	noreorder
	.set	nomacro
	bne	$23,$0,$L84
	addiu	$19,$19,16
	.set	macro
	.set	reorder

	lw	$23,28($sp)
	lw	$22,24($sp)
	lw	$21,20($sp)
	lw	$20,16($sp)
	lw	$19,12($sp)
	lw	$18,8($sp)
	lw	$17,4($sp)
	lw	$16,0($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,32
	.set	macro
	.set	reorder

	.end	pass_4750
	.align	2
	.ent	fft4_4750
	.type	fft4_4750, @function
fft4_4750:
	.frame	$sp,8,$31		# vars= 0, regs= 2/0, args= 0, gp= 0
	.mask	0x00030000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-8
	sw	$17,4($sp)
	sw	$16,0($sp)
	addiu	$17,$4,8
	addiu	$16,$4,16
	addiu	$15,$4,24
	lw	$14,4($17)
	lw	$7,4($16)
	lw	$12,4($15)
	lw	$2,4($4)
	lw	$8,16($4)
	lw	$5,24($4)
	lw	$10,8($4)
	lw	$3,0($4)
	addu	$11,$5,$8
	addu	$6,$2,$14
	addu	$13,$7,$12
	subu	$2,$2,$14
	subu	$5,$5,$8
	addu	$9,$3,$10
	addu	$8,$5,$2
	subu	$3,$3,$10
	subu	$2,$2,$5
	addu	$10,$6,$13
	subu	$6,$6,$13
	sw	$2,4($15)
	sw	$8,4($17)
	sw	$6,4($16)
	subu	$7,$7,$12
	lw	$17,4($sp)
	lw	$16,0($sp)
	addu	$14,$9,$11
	addu	$12,$3,$7
	subu	$9,$9,$11
	subu	$3,$3,$7
	addiu	$sp,$sp,8
	sw	$10,4($4)
	sw	$9,16($4)
	sw	$14,0($4)
	sw	$3,24($4)
	j	$31
	sw	$12,8($4)

	.set	macro
	.set	reorder
	.end	fft4_4750
	.align	2
	.ent	fft8_4750
	.type	fft8_4750, @function
fft8_4750:
	.frame	$sp,40,$31		# vars= 0, regs= 4/0, args= 16, gp= 8
	.mask	0x80070000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-40
	sw	$31,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft4_4750)($28)
	addiu	$25,$25,%lo(fft4_4750)
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$16,$4
	.set	macro
	.set	reorder

	addiu	$18,$16,40
	addiu	$5,$16,32
	lw	$7,4($5)
	lw	$3,4($18)
	lw	$28,16($sp)
	subu	$2,$7,$3
	sw	$2,4($18)
	addiu	$17,$16,48
	addiu	$15,$16,56
	lw	$4,4($15)
	lw	$9,4($17)
	addu	$7,$7,$3
	subu	$2,$9,$4
	sw	$2,4($15)
	lw	$3,4($16)
	addu	$9,$9,$4
	addu	$6,$7,$9
	subu	$3,$3,$6
	sw	$3,4($5)
	lw	$2,4($16)
	lw	$5,32($16)
	lw	$14,40($16)
	lw	$8,48($16)
	lw	$13,56($16)
	addu	$2,$2,$6
	sw	$2,4($16)
	addiu	$11,$16,16
	addu	$10,$8,$13
	lw	$2,4($11)
	addu	$3,$5,$14
	subu	$12,$10,$3
	subu	$2,$2,$12
	sw	$2,4($17)
	lw	$2,4($11)
	lw	$4,0($16)
	lw	$6,16($16)
	addu	$3,$3,$10
	subu	$7,$7,$9
	addu	$2,$2,$12
	addu	$9,$4,$3
	addu	$10,$6,$7
	subu	$5,$5,$14
	subu	$8,$8,$13
	subu	$4,$4,$3
	subu	$6,$6,$7
	sw	$2,4($11)
	li	$2,1518469120			# 0x5a820000
	sw	$8,56($16)
	sw	$4,32($16)
	sw	$9,0($16)
	sw	$6,48($16)
	sw	$10,16($16)
	sw	$5,40($16)
	ori	$2,$2,0x7999
#APP
	S32MUL xr1,xr2,$5,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
	lw	$4,4($18)
#APP
	S32MADD xr1,xr2,$4,$3
#NO_APP
	li	$2,1518469120			# 0x5a820000
	ori	$2,$2,0x7999
#APP
	S32MUL xr3,xr4,$4,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
#APP
	S32MSUB xr3,xr4,$5,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $11
	S32M2I xr3, $8
#NO_APP
	li	$2,1518469120			# 0x5a820000
	ori	$2,$2,0x7999
	lw	$5,0($15)
#APP
	S32MUL xr1,xr2,$5,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
	lw	$4,4($15)
#APP
	S32MSUB xr1,xr2,$4,$3
#NO_APP
	li	$2,1518469120			# 0x5a820000
	ori	$2,$2,0x7999
#APP
	S32MUL xr3,xr4,$4,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
#APP
	S32MADD xr3,xr4,$5,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $3
	S32M2I xr3, $9
#NO_APP
	addiu	$17,$16,8
	addiu	$12,$16,24
	lw	$5,4($17)
	lw	$4,4($12)
	lw	$2,8($16)
	lw	$6,24($16)
	addu	$7,$11,$3
	addu	$10,$8,$9
	subu	$3,$3,$11
	subu	$8,$8,$9
	addu	$13,$2,$7
	addu	$9,$4,$3
	addu	$14,$6,$8
	addu	$11,$5,$10
	subu	$2,$2,$7
	subu	$4,$4,$3
	subu	$6,$6,$8
	subu	$5,$5,$10
	sw	$2,0($18)
	sw	$4,4($15)
	sw	$13,8($16)
	sw	$9,4($12)
	sw	$6,0($15)
	sw	$5,4($18)
	sw	$14,24($16)
	sw	$11,4($17)
	lw	$31,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,40
	.set	macro
	.set	reorder

	.end	fft8_4750
	.align	2
	.ent	fft16_4750
	.type	fft16_4750, @function
fft16_4750:
	.frame	$sp,48,$31		# vars= 0, regs= 5/0, args= 16, gp= 8
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-48
	sw	$31,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft8_4750)($28)
	addiu	$25,$25,%lo(fft8_4750)
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$16,$4
	.set	macro
	.set	reorder

	lw	$28,16($sp)
	addiu	$18,$16,64
	lw	$2,%got(fft4_4750)($28)
	addiu	$17,$16,96
	addiu	$19,$2,%lo(fft4_4750)
	move	$25,$19
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$18
	.set	macro
	.set	reorder

	move	$25,$19
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$17
	.set	macro
	.set	reorder

	lw	$11,4($17)
	lw	$4,4($18)
	lw	$9,64($16)
	lw	$2,96($16)
	addiu	$12,$16,32
	lw	$6,4($16)
	lw	$3,4($12)
	lw	$5,0($16)
	lw	$7,32($16)
	addu	$8,$9,$2
	addu	$10,$4,$11
	subu	$2,$2,$9
	subu	$4,$4,$11
	addu	$9,$3,$2
	addu	$13,$5,$8
	addu	$11,$7,$4
	addu	$14,$6,$10
	subu	$5,$5,$8
	subu	$3,$3,$2
	subu	$7,$7,$4
	subu	$6,$6,$10
	li	$2,1518469120			# 0x5a820000
	lw	$28,16($sp)
	ori	$2,$2,0x7999
	sw	$3,4($17)
	sw	$5,64($16)
	sw	$9,4($12)
	sw	$13,0($16)
	sw	$6,4($18)
	sw	$7,96($16)
	sw	$11,32($16)
	sw	$14,4($16)
	addiu	$17,$16,80
	lw	$5,80($16)
#APP
	S32MUL xr1,xr2,$5,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
	lw	$4,4($17)
#APP
	S32MADD xr1,xr2,$4,$3
#NO_APP
	li	$2,1518469120			# 0x5a820000
	ori	$2,$2,0x7999
#APP
	S32MUL xr3,xr4,$4,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
#APP
	S32MSUB xr3,xr4,$5,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $11
	S32M2I xr3, $8
#NO_APP
	li	$2,1518469120			# 0x5a820000
	ori	$2,$2,0x7999
	addiu	$12,$16,112
	lw	$5,112($16)
#APP
	S32MUL xr1,xr2,$5,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
	lw	$4,4($12)
#APP
	S32MSUB xr1,xr2,$4,$3
#NO_APP
	li	$2,1518469120			# 0x5a820000
	ori	$2,$2,0x7999
#APP
	S32MUL xr3,xr4,$4,$2
#NO_APP
	li	$3,1518469120			# 0x5a820000
	ori	$3,$3,0x7999
#APP
	S32MADD xr3,xr4,$5,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $3
	S32M2I xr3, $9
#NO_APP
	lw	$2,16($16)
	addiu	$18,$16,16
	addiu	$13,$16,48
	lw	$5,4($18)
	lw	$4,4($13)
	lw	$6,48($16)
	addu	$7,$11,$3
	addu	$10,$8,$9
	subu	$3,$3,$11
	subu	$8,$8,$9
	addu	$14,$2,$7
	subu	$2,$2,$7
	sw	$2,0($17)
	addu	$15,$6,$8
	addu	$9,$4,$3
	addu	$11,$5,$10
	subu	$4,$4,$3
	subu	$6,$6,$8
	subu	$5,$5,$10
	lw	$2,%got(ff_cos_16)($28)
	sw	$4,4($12)
	sw	$14,16($16)
	sw	$9,4($13)
	sw	$6,0($12)
	sw	$5,4($17)
	sw	$15,48($16)
	sw	$11,4($18)
	lw	$6,4($2)
	addiu	$15,$16,72
	lw	$4,72($16)
#APP
	S32MUL xr1,xr2,$4,$6
#NO_APP
	lw	$3,%got(ff_cos_16)($28)
	lw	$2,4($15)
	lw	$5,12($3)
#APP
	S32MADD xr1,xr2,$2,$5
	S32MUL xr3,xr4,$2,$6
	S32MSUB xr3,xr4,$4,$5
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $18
	S32M2I xr3, $11
#NO_APP
	addiu	$14,$16,104
	lw	$3,104($16)
#APP
	S32MUL xr1,xr2,$3,$6
#NO_APP
	lw	$2,4($14)
#APP
	S32MSUB xr1,xr2,$2,$5
	S32MUL xr3,xr4,$2,$6
	S32MADD xr3,xr4,$3,$5
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $4
	S32M2I xr3, $8
#NO_APP
	lw	$2,8($16)
	lw	$10,40($16)
	addiu	$19,$16,8
	addiu	$17,$16,40
	lw	$7,4($19)
	lw	$3,4($17)
	addu	$9,$18,$4
	subu	$13,$11,$8
	addu	$12,$2,$9
	addu	$11,$11,$8
	subu	$4,$4,$18
	subu	$8,$10,$13
	subu	$2,$2,$9
	sw	$2,0($15)
	addu	$9,$3,$4
	addu	$10,$10,$13
	addu	$2,$7,$11
	subu	$3,$3,$4
	sw	$12,8($16)
	subu	$7,$7,$11
	sw	$8,0($14)
	sw	$3,4($14)
	sw	$10,40($16)
	sw	$9,4($17)
	addiu	$13,$16,88
	sw	$7,4($15)
	lw	$3,88($16)
	sw	$2,4($19)
#APP
	S32MUL xr1,xr2,$3,$5
#NO_APP
	lw	$2,4($13)
#APP
	S32MADD xr1,xr2,$2,$6
	S32MUL xr3,xr4,$2,$5
	S32MSUB xr3,xr4,$3,$6
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $12
	S32M2I xr3, $7
#NO_APP
	addiu	$11,$16,120
	lw	$3,120($16)
#APP
	S32MUL xr1,xr2,$3,$5
#NO_APP
	lw	$2,4($11)
#APP
	S32MSUB xr1,xr2,$2,$6
	S32MUL xr3,xr4,$2,$5
	S32MADD xr3,xr4,$3,$6
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $3
	S32M2I xr3, $9
#NO_APP
	addiu	$18,$16,24
	addiu	$14,$16,56
	lw	$5,4($18)
	lw	$4,4($14)
	lw	$2,24($16)
	lw	$6,56($16)
	addu	$8,$12,$3
	addu	$10,$7,$9
	subu	$3,$3,$12
	subu	$7,$7,$9
	addu	$17,$6,$7
	addu	$15,$2,$8
	addu	$9,$4,$3
	addu	$12,$5,$10
	subu	$2,$2,$8
	subu	$4,$4,$3
	subu	$6,$6,$7
	subu	$5,$5,$10
	sw	$2,0($13)
	sw	$4,4($11)
	sw	$15,24($16)
	sw	$9,4($14)
	sw	$6,0($11)
	sw	$5,4($13)
	sw	$17,56($16)
	sw	$12,4($18)
	lw	$31,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	fft16_4750
	.align	2
	.ent	fft32_4750
	.type	fft32_4750, @function
fft32_4750:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft16_4750)($28)
	addiu	$25,$25,%lo(fft16_4750)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft8_4750)($28)
	addiu	$17,$2,%lo(fft8_4750)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,128

	move	$25,$17
	jalr	$25
	addiu	$4,$16,192

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4750)($28)
	lw	$5,%got(ff_cos_32)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,4			# 0x4
	addiu	$25,$25,%lo(pass_4750)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft32_4750
	.align	2
	.ent	fft64_4750
	.type	fft64_4750, @function
fft64_4750:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft32_4750)($28)
	addiu	$25,$25,%lo(fft32_4750)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft16_4750)($28)
	addiu	$17,$2,%lo(fft16_4750)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,256

	move	$25,$17
	jalr	$25
	addiu	$4,$16,384

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4750)($28)
	lw	$5,%got(ff_cos_64)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,8			# 0x8
	addiu	$25,$25,%lo(pass_4750)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft64_4750
	.align	2
	.ent	fft128_4750
	.type	fft128_4750, @function
fft128_4750:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft64_4750)($28)
	addiu	$25,$25,%lo(fft64_4750)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft32_4750)($28)
	addiu	$17,$2,%lo(fft32_4750)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,512

	move	$25,$17
	jalr	$25
	addiu	$4,$16,768

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4750)($28)
	lw	$5,%got(ff_cos_128)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,16			# 0x10
	addiu	$25,$25,%lo(pass_4750)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft128_4750
	.align	2
	.ent	fft256_4750
	.type	fft256_4750, @function
fft256_4750:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft128_4750)($28)
	addiu	$25,$25,%lo(fft128_4750)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft64_4750)($28)
	addiu	$17,$2,%lo(fft64_4750)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,1024

	move	$25,$17
	jalr	$25
	addiu	$4,$16,1536

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4750)($28)
	lw	$5,%got(ff_cos_256)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,32			# 0x20
	addiu	$25,$25,%lo(pass_4750)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft256_4750
	.align	2
	.ent	fft512_4750
	.type	fft512_4750, @function
fft512_4750:
	.frame	$sp,40,$31		# vars= 0, regs= 3/0, args= 16, gp= 8
	.mask	0x80030000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$31,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft256_4750)($28)
	addiu	$25,$25,%lo(fft256_4750)
	jalr	$25
	move	$16,$4

	lw	$28,16($sp)
	lw	$2,%got(fft128_4750)($28)
	addiu	$17,$2,%lo(fft128_4750)
	move	$25,$17
	jalr	$25
	addiu	$4,$16,2048

	move	$25,$17
	jalr	$25
	addiu	$4,$16,3072

	lw	$28,16($sp)
	move	$4,$16
	lw	$25,%got(pass_4750)($28)
	lw	$5,%got(ff_cos_512)($28)
	lw	$31,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	li	$6,64			# 0x40
	addiu	$25,$25,%lo(pass_4750)
	jr	$25
	addiu	$sp,$sp,40

	.set	macro
	.set	reorder
	.end	fft512_4750
	.align	2
	.ent	fft1024_4750
	.type	fft1024_4750, @function
fft1024_4750:
	.frame	$sp,48,$31		# vars= 0, regs= 5/0, args= 16, gp= 8
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	addiu	$sp,$sp,-48
	sw	$31,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%got(fft512_4750)($28)
	addiu	$25,$25,%lo(fft512_4750)
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$19,$4
	.set	macro
	.set	reorder

	lw	$28,16($sp)
	addiu	$17,$19,4096
	lw	$2,%got(fft256_4750)($28)
	addiu	$16,$19,6144
	addiu	$18,$2,%lo(fft256_4750)
	move	$25,$18
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$17
	.set	macro
	.set	reorder

	move	$25,$18
	.set	noreorder
	.set	nomacro
	jalr	$25
	move	$4,$16
	.set	macro
	.set	reorder

	lw	$11,4($16)
	lw	$5,4($17)
	lw	$9,4096($19)
	lw	$2,6144($19)
	addiu	$13,$19,2048
	lw	$28,16($sp)
	lw	$7,4($19)
	lw	$3,4($13)
	lw	$4,0($19)
	lw	$6,2048($19)
	addu	$8,$9,$2
	addu	$10,$5,$11
	subu	$2,$2,$9
	subu	$5,$5,$11
	addu	$12,$4,$8
	addu	$9,$3,$2
	addu	$11,$6,$5
	addu	$14,$7,$10
	subu	$3,$3,$2
	subu	$6,$6,$5
	subu	$4,$4,$8
	subu	$7,$7,$10
	lw	$2,%got(ff_cos_1024)($28)
	sw	$3,4($16)
	sw	$12,0($19)
	sw	$9,4($13)
	sw	$6,6144($19)
	sw	$7,4($17)
	sw	$4,4096($19)
	sw	$11,2048($19)
	sw	$14,4($19)
	lw	$5,4($2)
	addiu	$12,$19,4104
	lw	$6,4104($19)
#APP
	S32MUL xr1,xr2,$6,$5
#NO_APP
	lw	$4,%got(ff_cos_1024)($28)
	lw	$2,4($12)
	lw	$3,1020($4)
#APP
	S32MADD xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MSUB xr3,xr4,$6,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $11
	S32M2I xr3, $7
#NO_APP
	lw	$4,8($16)
	addiu	$16,$16,8
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,4($16)
#APP
	S32MSUB xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MADD xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $2
	S32M2I xr3, $9
#NO_APP
	addiu	$18,$19,8
	addiu	$14,$13,8
	lw	$4,8($19)
	lw	$5,4($18)
	lw	$6,8($13)
	lw	$3,4($14)
	addu	$8,$11,$2
	addu	$10,$7,$9
	subu	$2,$2,$11
	subu	$7,$7,$9
	lw	$24,%got(ff_cos_1024)($28)
	addu	$15,$4,$8
	addu	$9,$3,$2
	addu	$17,$6,$7
	addu	$11,$5,$10
	subu	$4,$4,$8
	subu	$3,$3,$2
	subu	$6,$6,$7
	subu	$5,$5,$10
	sw	$3,4($16)
	sw	$4,0($12)
	sw	$9,4($14)
	sw	$15,8($19)
	sw	$5,4($12)
	sw	$6,0($16)
	sw	$11,4($18)
	sw	$17,8($13)
	addiu	$15,$24,1024
	addiu	$16,$24,1016
$L106:
	addiu	$19,$19,16
	addiu	$24,$24,8
	addiu	$15,$15,-8
	lw	$4,4096($19)
	lw	$5,0($24)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,4100($19)
	lw	$3,0($15)
#APP
	S32MADD xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MSUB xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $12
	S32M2I xr3, $7
#NO_APP
	lw	$4,6144($19)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,6148($19)
#APP
	S32MSUB xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MADD xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $2
	S32M2I xr3, $9
#NO_APP
	lw	$4,0($19)
	lw	$8,4($19)
	lw	$5,2048($19)
	lw	$3,2052($19)
	addu	$11,$7,$9
	addu	$10,$12,$2
	subu	$7,$7,$9
	subu	$2,$2,$12
	addu	$9,$3,$2
	addu	$12,$4,$10
	addu	$13,$5,$7
	addu	$14,$8,$11
	subu	$4,$4,$10
	subu	$5,$5,$7
	subu	$3,$3,$2
	subu	$8,$8,$11
	sw	$4,4096($19)
	sw	$12,0($19)
	sw	$3,6148($19)
	sw	$9,2052($19)
	sw	$5,6144($19)
	sw	$13,2048($19)
	sw	$8,4100($19)
	sw	$14,4($19)
	lw	$4,4104($19)
	lw	$5,4($24)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,4108($19)
	lw	$3,-4($15)
#APP
	S32MADD xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MSUB xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $12
	S32M2I xr3, $7
#NO_APP
	lw	$4,6152($19)
#APP
	S32MUL xr1,xr2,$4,$5
#NO_APP
	lw	$2,6156($19)
#APP
	S32MSUB xr1,xr2,$2,$3
	S32MUL xr3,xr4,$2,$5
	S32MADD xr3,xr4,$4,$3
	D32SLL xr1,xr1,xr3,xr3,1
	S32M2I xr1, $2
	S32M2I xr3, $9
#NO_APP
	lw	$4,8($19)
	lw	$8,12($19)
	lw	$5,2056($19)
	lw	$3,2060($19)
	addu	$10,$12,$2
	addu	$11,$7,$9
	subu	$2,$2,$12
	subu	$7,$7,$9
	addu	$12,$4,$10
	addu	$13,$3,$2
	addu	$14,$5,$7
	addu	$9,$8,$11
	subu	$4,$4,$10
	subu	$3,$3,$2
	subu	$5,$5,$7
	subu	$8,$8,$11
	sw	$9,12($19)
	sw	$4,4104($19)
	sw	$12,8($19)
	sw	$3,6156($19)
	sw	$13,2060($19)
	sw	$5,6152($19)
	sw	$14,2056($19)
	.set	noreorder
	.set	nomacro
	bne	$24,$16,$L106
	sw	$8,4108($19)
	.set	macro
	.set	reorder

	lw	$31,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	.set	noreorder
	.set	nomacro
	j	$31
	addiu	$sp,$sp,48
	.set	macro
	.set	reorder

	.end	fft1024_4750
	.align	2
	.globl	ff_fft_calc_fixed_4750
	.ent	ff_fft_calc_fixed_4750
	.type	ff_fft_calc_fixed_4750, @function
ff_fft_calc_fixed_4750:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	lw	$2,0($4)
	lw	$3,%got(fft_dispatch_4750)($28)
	sll	$2,$2,2
	addiu	$3,$3,%lo(fft_dispatch_4750)
	addu	$2,$2,$3
	lw	$25,-8($2)
	jr	$25
	move	$4,$5

	.set	macro
	.set	reorder
	.end	ff_fft_calc_fixed_4750
	.align	2
	.globl	ff_imdct_half_fixed_4750
	.ent	ff_imdct_half_fixed_4750
	.type	ff_imdct_half_fixed_4750, @function
ff_imdct_half_fixed_4750:
	.frame	$sp,112,$31		# vars= 40, regs= 10/0, args= 24, gp= 8
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-112
	sw	$31,108($sp)
	sw	$fp,104($sp)
	sw	$23,100($sp)
	sw	$22,96($sp)
	sw	$21,92($sp)
	sw	$20,88($sp)
	sw	$19,84($sp)
	sw	$18,80($sp)
	sw	$17,76($sp)
	sw	$16,72($sp)
	.cprestore	24
	lw	$3,4($4)
	move	$8,$4
	li	$2,1			# 0x1
	sll	$2,$2,$3
	move	$fp,$5
	lw	$3,24($4)
	lw	$5,12($8)
	lw	$4,8($4)
	sra	$22,$2,2
	sra	$23,$2,3
	sw	$4,52($sp)
	blez	$22,$L114
	sw	$5,48($sp)

	sra	$2,$2,1
	sll	$2,$2,2
	addu	$2,$6,$2
	addiu	$20,$2,-4
	move	$19,$3
	move	$18,$6
	move	$17,$4
	move	$16,$5
	move	$21,$0
$L116:
	lhu	$4,0($19)
	lw	$2,0($16)
	lw	$5,0($20)
	lw	$7,0($17)
	lw	$6,0($18)
	lw	$25,%call16(Calc_cmul_4750)($28)
	sll	$4,$4,3
	sw	$8,64($sp)
	sw	$2,16($sp)
	addu	$4,$fp,$4
	jalr	$25
	addiu	$21,$21,1

	addiu	$18,$18,8
	lw	$28,24($sp)
	addiu	$20,$20,-8
	addiu	$19,$19,2
	addiu	$17,$17,4
	addiu	$16,$16,4
	bne	$22,$21,$L116
	lw	$8,64($sp)

$L114:
	lw	$25,%call16(ff_fft_calc_fixed_4750)($28)
	addiu	$4,$8,16
	jalr	$25
	move	$5,$fp

	blez	$23,$L120
	lw	$28,24($sp)

	lw	$5,52($sp)
	sll	$2,$23,2
	addu	$4,$5,$2
	lw	$5,48($sp)
	sll	$3,$23,3
	addu	$2,$5,$2
	addu	$3,$fp,$3
	move	$21,$2
	addiu	$19,$2,-4
	addiu	$2,$sp,32
	addiu	$17,$3,4
	move	$20,$4
	addiu	$16,$3,-8
	addiu	$18,$4,-4
	move	$22,$0
	sw	$2,56($sp)
	addiu	$fp,$sp,40
$L119:
	lw	$2,0($18)
	lw	$5,4($16)
	lw	$6,0($16)
	lw	$7,0($19)
	lw	$25,%call16(Calc_cmul_4750)($28)
	lw	$4,56($sp)
	jalr	$25
	sw	$2,16($sp)

	lw	$28,24($sp)
	lw	$2,0($20)
	lw	$5,0($17)
	lw	$6,-4($17)
	lw	$7,0($21)
	lw	$25,%call16(Calc_cmul_4750)($28)
	sw	$2,16($sp)
	jalr	$25
	move	$4,$fp

	lw	$2,32($sp)
	lw	$3,44($sp)
	lw	$28,24($sp)
	sw	$2,0($16)
	sw	$3,4($16)
	lw	$2,40($sp)
	lw	$3,36($sp)
	addiu	$22,$22,1
	sw	$2,-4($17)
	sw	$3,0($17)
	addiu	$16,$16,-8
	addiu	$19,$19,-4
	addiu	$18,$18,-4
	addiu	$17,$17,8
	addiu	$21,$21,4
	bne	$23,$22,$L119
	addiu	$20,$20,4

$L120:
	lw	$31,108($sp)
	lw	$fp,104($sp)
	lw	$23,100($sp)
	lw	$22,96($sp)
	lw	$21,92($sp)
	lw	$20,88($sp)
	lw	$19,84($sp)
	lw	$18,80($sp)
	lw	$17,76($sp)
	lw	$16,72($sp)
	j	$31
	addiu	$sp,$sp,112

	.set	macro
	.set	reorder
	.end	ff_imdct_half_fixed_4750
	.align	2
	.globl	ff_imdct_calc_fixed
	.ent	ff_imdct_calc_fixed
	.type	ff_imdct_calc_fixed, @function
ff_imdct_calc_fixed:
	.frame	$sp,48,$31		# vars= 0, regs= 5/0, args= 16, gp= 8
	.mask	0x800f0000,-8
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-48
	sw	$31,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$3,4($4)
	li	$2,1			# 0x1
	sll	$17,$2,$3
	lw	$2,%got(ff_imdct_half_fixed)($28)
	sra	$16,$17,2
	move	$19,$5
	lw	$25,0($2)
	sll	$5,$16,2
	jalr	$25
	addu	$5,$5,$19

	sra	$18,$17,1
	blez	$16,$L127
	lw	$28,16($sp)

	sll	$3,$18,2
	sll	$2,$17,2
	addu	$3,$19,$3
	addu	$2,$19,$2
	move	$7,$19
	move	$6,$3
	addiu	$4,$2,-4
	addiu	$5,$3,-4
	move	$8,$0
$L126:
	lw	$2,0($5)
	addiu	$8,$8,1
	subu	$2,$0,$2
	sw	$2,0($7)
	lw	$3,0($6)
	addiu	$5,$5,-4
	sw	$3,0($4)
	addiu	$7,$7,4
	addiu	$6,$6,4
	bne	$16,$8,$L126
	addiu	$4,$4,-4

$L127:
	lw	$31,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,48

	.set	macro
	.set	reorder
	.end	ff_imdct_calc_fixed
	.align	2
	.ent	wma_window
	.type	wma_window, @function
wma_window:
	.frame	$sp,56,$31		# vars= 0, regs= 8/0, args= 16, gp= 8
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	move	$16,$4
	lw	$6,1112($16)
	lw	$4,1104($4)
	li	$2,42128			# 0xa490
	slt	$3,$6,$4
	move	$18,$5
	bne	$3,$0,$L130
	addu	$17,$16,$2

	lw	$8,1092($16)
	lw	$10,1116($16)
	blez	$10,$L132
	subu	$2,$8,$4

	sll	$2,$2,2
	addu	$2,$2,$16
	li	$3,65536			# 0x10000
	addu	$2,$2,$3
	lw	$8,-6844($2)
	move	$7,$17
	move	$6,$5
	move	$9,$0
$L134:
	lw	$2,0($7)
	lw	$4,0($8)
	lw	$5,0($6)
	mult	$2,$4
	addiu	$9,$9,1
	mflo	$2
	mfhi	$3
	srl	$2,$2,16
	sll	$4,$3,16
	or	$2,$4,$2
	addu	$5,$5,$2
	sw	$5,0($6)
	addiu	$8,$8,4
	addiu	$7,$7,4
	bne	$10,$9,$L134
	addiu	$6,$6,4

	lw	$4,1104($16)
	lw	$10,1116($16)
	lw	$8,1092($16)
$L132:
	lw	$7,1108($16)
	sll	$5,$10,2
	slt	$2,$7,$4
	addu	$20,$5,$18
	bne	$2,$0,$L138
	addu	$18,$17,$5

	blez	$10,$L146
	subu	$2,$8,$4

	sll	$2,$2,2
	addu	$2,$2,$16
	li	$3,65536			# 0x10000
	addu	$2,$2,$3
	lw	$11,-6844($2)
	move	$8,$18
	move	$7,$5
	move	$6,$20
	move	$9,$0
$L142:
	addu	$4,$11,$7
	lw	$2,0($8)
	lw	$5,-4($4)
	addiu	$9,$9,1
	mult	$2,$5
	addiu	$8,$8,4
	mflo	$2
	mfhi	$3
	srl	$2,$2,16
	sll	$4,$3,16
	or	$2,$4,$2
	sw	$2,0($6)
	addiu	$7,$7,-4
	bne	$10,$9,$L142
	addiu	$6,$6,4

$L146:
	lw	$31,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,56

$L130:
	li	$2,1			# 0x1
	lw	$3,1116($16)
	sll	$11,$2,$6
	subu	$3,$3,$11
	srl	$2,$3,31
	lw	$4,1092($16)
	addu	$2,$2,$3
	subu	$4,$4,$6
	blez	$11,$L147
	sra	$12,$2,1

	sll	$2,$4,2
	addu	$2,$2,$16
	li	$3,65536			# 0x10000
	addu	$2,$2,$3
	sll	$6,$12,2
	lw	$9,-6844($2)
	addu	$8,$17,$6
	addu	$7,$5,$6
	move	$10,$0
$L137:
	lw	$2,0($8)
	lw	$4,0($9)
	lw	$5,0($7)
	mult	$2,$4
	addiu	$10,$10,1
	mflo	$2
	mfhi	$3
	srl	$2,$2,16
	sll	$4,$3,16
	or	$2,$4,$2
	addu	$5,$5,$2
	sw	$5,0($7)
	addiu	$9,$9,4
	addiu	$8,$8,4
	bne	$11,$10,$L137
	addiu	$7,$7,4

$L135:
	addu	$4,$12,$11
	sll	$4,$4,2
	lw	$25,%call16(memcpy)($28)
	addu	$5,$17,$4
	jalr	$25
	addu	$4,$18,$4

	lw	$28,16($sp)
	lw	$4,1104($16)
	lw	$10,1116($16)
	b	$L132
	lw	$8,1092($16)

$L138:
	li	$2,1			# 0x1
	sll	$17,$2,$7
	subu	$3,$10,$17
	srl	$2,$3,31
	addu	$2,$2,$3
	sra	$22,$2,1
	lw	$25,%call16(memcpy)($28)
	sll	$21,$22,2
	move	$4,$20
	move	$5,$18
	move	$6,$21
	jalr	$25
	subu	$19,$8,$7

	blez	$17,$L143
	lw	$28,16($sp)

	sll	$2,$19,2
	addu	$2,$2,$16
	li	$3,65536			# 0x10000
	addu	$2,$2,$3
	lw	$10,-6844($2)
	addu	$8,$18,$21
	sll	$7,$17,2
	addu	$6,$20,$21
	move	$9,$0
$L145:
	addu	$4,$10,$7
	lw	$2,0($8)
	lw	$5,-4($4)
	addiu	$9,$9,1
	mult	$2,$5
	addiu	$8,$8,4
	mflo	$2
	mfhi	$3
	srl	$2,$2,16
	sll	$4,$3,16
	or	$2,$4,$2
	sw	$2,0($6)
	addiu	$7,$7,-4
	bne	$17,$9,$L145
	addiu	$6,$6,4

$L143:
	addu	$4,$22,$17
	sll	$4,$4,2
	lw	$25,%call16(memset)($28)
	addu	$4,$4,$20
	move	$6,$21
	lw	$31,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	move	$5,$0
	jr	$25
	addiu	$sp,$sp,56

$L147:
	b	$L135
	sll	$6,$12,2

	.set	macro
	.set	reorder
	.end	wma_window
	.align	2
	.globl	ff_fft_end_fixed
	.ent	ff_fft_end_fixed
	.type	ff_fft_end_fixed, @function
ff_fft_end_fixed:
	.frame	$sp,32,$31		# vars= 0, regs= 2/0, args= 16, gp= 8
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%call16(av_freep)($28)
	move	$16,$4
	jalr	$25
	addiu	$4,$4,8

	lw	$28,16($sp)
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$16,12

	lw	$28,16($sp)
	addiu	$4,$16,16
	lw	$25,%call16(av_freep)($28)
	lw	$31,28($sp)
	lw	$16,24($sp)
	jr	$25
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	ff_fft_end_fixed
	.align	2
	.globl	ff_mdct_end_fixed
	.ent	ff_mdct_end_fixed
	.type	ff_mdct_end_fixed, @function
ff_mdct_end_fixed:
	.frame	$sp,32,$31		# vars= 0, regs= 2/0, args= 16, gp= 8
	.mask	0x80010000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$31,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$25,%call16(av_freep)($28)
	move	$16,$4
	jalr	$25
	addiu	$4,$4,8

	lw	$28,16($sp)
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$16,12

	lw	$28,16($sp)
	addiu	$4,$16,16
	lw	$25,%call16(ff_fft_end_fixed)($28)
	lw	$31,28($sp)
	lw	$16,24($sp)
	jr	$25
	addiu	$sp,$sp,32

	.set	macro
	.set	reorder
	.end	ff_mdct_end_fixed
	.section	.rodata.cst8,"aM",@progbits,8
	.align	3
$LC0:
	.word	1413754136
	.word	1075388923
	.section	.rodata.cst4,"aM",@progbits,4
	.align	2
$LC1:
	.word	1325400064
	.text
	.align	2
	.globl	ff_fft_init_fixed
	.ent	ff_fft_init_fixed
	.type	ff_fft_init_fixed, @function
ff_fft_init_fixed:
	.frame	$sp,104,$31		# vars= 8, regs= 10/4, args= 16, gp= 8
	.mask	0xc0ff0000,-36
	.fmask	0x0ff00000,-8
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-104
	sw	$31,68($sp)
	sw	$fp,64($sp)
	sw	$23,60($sp)
	sw	$22,56($sp)
	sw	$21,52($sp)
	sw	$20,48($sp)
	sw	$19,44($sp)
	sw	$18,40($sp)
	sw	$17,36($sp)
	sw	$16,32($sp)
	sdc1	$f26,96($sp)
	sdc1	$f24,88($sp)
	sdc1	$f22,80($sp)
	sdc1	$f20,72($sp)
	.cprestore	16
	li	$2,1			# 0x1
	sll	$22,$2,$5
	srl	$3,$22,31
	addu	$3,$3,$22
	sra	$3,$3,1
	lw	$25,%call16(av_malloc)($28)
	sw	$4,104($sp)
	sw	$5,0($4)
	sll	$4,$3,3
	move	$16,$5
	jalr	$25
	move	$17,$6

	lw	$3,104($sp)
	lw	$28,16($sp)
	beq	$2,$0,$L159
	sw	$2,12($3)

	lw	$25,%call16(av_malloc)($28)
	jalr	$25
	sll	$4,$22,1

	lw	$25,104($sp)
	lw	$28,16($sp)
	beq	$2,$0,$L159
	sw	$2,8($25)

	slt	$2,$16,4
	sw	$17,4($25)
	bne	$2,$0,$L162
	sw	$0,16($25)

	lw	$2,%got($LC0)($28)
	lw	$3,%got($LC1)($28)
	ldc1	$f24,%lo($LC0)($2)
	lwc1	$f26,%lo($LC1)($3)
	addiu	$16,$16,1
	lw	$fp,%got(ff_cos_tabs)($28)
	sw	$16,24($sp)
	li	$23,4			# 0x4
$L164:
	li	$2,1			# 0x1
	sll	$20,$2,$23
	mtc1	$20,$f2
	lw	$21,0($fp)
	cvt.d.w	$f0,$f2
	div.d	$f20,$f24,$f0
	bltz	$20,$L181
	move	$2,$20

$L165:
	sra	$19,$2,2
	bltz	$19,$L182
	slt	$2,$19,2

	mov.s	$f22,$f26
	move	$16,$21
	move	$17,$0
	addiu	$18,$19,1
$L168:
	mtc1	$17,$f0
	lw	$25,%call16(cos)($28)
	cvt.d.w	$f12,$f0
	mul.d	$f12,$f20,$f12
	jalr	$25
	addiu	$17,$17,1

	cvt.s.d	$f0,$f0
	mul.s	$f0,$f0,$f22
	lw	$28,16($sp)
	trunc.w.s $f2,$f0
	swc1	$f2,0($16)
	bne	$17,$18,$L168
	addiu	$16,$16,4

	slt	$2,$19,2
$L182:
	bne	$2,$0,$L169
	srl	$2,$20,31

	addu	$2,$2,$20
	sra	$2,$2,1
	sll	$2,$2,2
	addu	$2,$21,$2
	move	$4,$21
	addiu	$3,$2,-4
	li	$5,1			# 0x1
$L171:
	lw	$2,4($4)
	addiu	$5,$5,1
	sw	$2,0($3)
	addiu	$4,$4,4
	bne	$5,$19,$L171
	addiu	$3,$3,-4

$L169:
	lw	$2,24($sp)
	addiu	$23,$23,1
	bne	$23,$2,$L164
	addiu	$fp,$fp,4

$L162:
	blez	$22,$L172
	lw	$3,104($sp)

	lw	$2,%got(split_radix_permutation)($28)
	lw	$19,4($3)
	lw	$18,8($3)
	addiu	$20,$2,%lo(split_radix_permutation)
	addiu	$17,$22,-1
	move	$16,$0
	move	$4,$16
$L183:
	move	$5,$22
	move	$25,$20
	jalr	$25
	move	$6,$19

	subu	$2,$0,$2
	and	$2,$17,$2
	sll	$2,$2,1
	addu	$2,$2,$18
	lw	$28,16($sp)
	sh	$16,0($2)
	addiu	$16,$16,1
	bne	$22,$16,$L183
	move	$4,$16

$L172:
	move	$2,$0
$L175:
	ldc1	$f26,96($sp)
	ldc1	$f24,88($sp)
	ldc1	$f22,80($sp)
	ldc1	$f20,72($sp)
	lw	$31,68($sp)
	lw	$fp,64($sp)
	lw	$23,60($sp)
	lw	$22,56($sp)
	lw	$21,52($sp)
	lw	$20,48($sp)
	lw	$19,44($sp)
	lw	$18,40($sp)
	lw	$17,36($sp)
	lw	$16,32($sp)
	j	$31
	addiu	$sp,$sp,104

$L181:
	b	$L165
	addiu	$2,$20,3

$L159:
	lw	$2,104($sp)
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$2,8

	lw	$28,16($sp)
	lw	$3,104($sp)
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$3,12

	lw	$28,16($sp)
	lw	$2,104($sp)
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$2,16

	lw	$28,16($sp)
	b	$L175
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	ff_fft_init_fixed
	.section	.rodata.cst8
	.align	3
$LC2:
	.word	0
	.word	1072693248
	.align	3
$LC3:
	.word	0
	.word	1069547520
	.align	3
$LC4:
	.word	1413754136
	.word	1075388923
	.section	.rodata.cst4
	.align	2
$LC5:
	.word	1325400064
	.text
	.align	2
	.globl	ff_mdct_init_fixed
	.ent	ff_mdct_init_fixed
	.type	ff_mdct_init_fixed, @function
ff_mdct_init_fixed:
	.frame	$sp,104,$31		# vars= 0, regs= 9/5, args= 16, gp= 8
	.mask	0x80ff0000,-48
	.fmask	0x3ff00000,-8
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-104
	sw	$31,56($sp)
	sw	$23,52($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	sdc1	$f28,96($sp)
	sdc1	$f26,88($sp)
	sdc1	$f24,80($sp)
	sdc1	$f22,72($sp)
	sdc1	$f20,64($sp)
	.cprestore	16
	li	$2,1			# 0x1
	sll	$16,$2,$5
	sra	$22,$16,2
	lw	$25,%call16(av_malloc)($28)
	sll	$17,$22,2
	move	$21,$4
	sw	$5,4($4)
	sw	$0,8($4)
	sw	$0,12($4)
	sw	$0,16($4)
	sw	$0,20($4)
	sw	$0,24($4)
	sw	$0,28($4)
	sw	$0,32($4)
	sw	$16,0($4)
	move	$4,$17
	jalr	$25
	move	$23,$6

	lw	$28,16($sp)
	beq	$2,$0,$L185
	sw	$2,8($21)

	lw	$25,%call16(av_malloc)($28)
	jalr	$25
	move	$4,$17

	move	$20,$2
	lw	$28,16($sp)
	beq	$2,$0,$L185
	sw	$2,12($21)

	blez	$22,$L188
	mtc1	$16,$f0

	lw	$2,%got($LC2)($28)
	cvt.d.w	$f2,$f0
	ldc1	$f0,%lo($LC2)($2)
	lw	$2,%got($LC3)($28)
	lw	$3,%got($LC4)($28)
	div.d	$f22,$f0,$f2
	ldc1	$f28,%lo($LC3)($2)
	lw	$2,%got($LC5)($28)
	ldc1	$f24,%lo($LC4)($3)
	lwc1	$f26,%lo($LC5)($2)
	lw	$19,8($21)
	move	$18,$0
$L190:
	mtc1	$18,$f0
	lw	$25,%call16(cosf)($28)
	cvt.d.w	$f20,$f0
	add.d	$f20,$f20,$f28
	sll	$16,$18,2
	addu	$17,$19,$16
	addiu	$18,$18,1
	mul.d	$f20,$f20,$f24
	addu	$16,$20,$16
	mul.d	$f20,$f22,$f20
	cvt.s.d	$f20,$f20
	jalr	$25
	mov.s	$f12,$f20

	neg.s	$f0,$f0
	lw	$28,16($sp)
	mul.s	$f0,$f0,$f26
	lw	$25,%call16(sinf)($28)
	mov.s	$f12,$f20
	trunc.w.s $f2,$f0
	jalr	$25
	swc1	$f2,0($17)

	neg.s	$f0,$f0
	lw	$28,16($sp)
	mul.s	$f0,$f0,$f26
	trunc.w.s $f2,$f0
	bne	$22,$18,$L190
	swc1	$f2,0($16)

$L188:
	lw	$5,4($21)
	lw	$25,%call16(ff_fft_init_fixed)($28)
	addiu	$5,$5,-2
	move	$6,$23
	jalr	$25
	addiu	$4,$21,16

	bltz	$2,$L185
	lw	$28,16($sp)

	move	$2,$0
$L192:
	ldc1	$f28,96($sp)
	ldc1	$f26,88($sp)
	ldc1	$f24,80($sp)
	ldc1	$f22,72($sp)
	ldc1	$f20,64($sp)
	lw	$31,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	j	$31
	addiu	$sp,$sp,104

$L185:
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$21,8

	lw	$28,16($sp)
	lw	$25,%call16(av_freep)($28)
	jalr	$25
	addiu	$4,$21,12

	lw	$28,16($sp)
	b	$L192
	li	$2,-1			# 0xffffffffffffffff

	.set	macro
	.set	reorder
	.end	ff_mdct_init_fixed
	.section	.rodata.cst8
	.align	3
$LC6:
	.word	1413754136
	.word	1074340347
	.section	.rodata.cst4
	.align	2
$LC7:
	.word	1199570944
	.section	.rodata.cst8
	.align	3
$LC8:
	.word	0
	.word	1073741824
	.align	3
$LC9:
	.word	0
	.word	-1076887552
	.section	.rodata.cst4
	.align	2
$LC10:
	.word	1065353216
	.align	2
$LC11:
	.word	998244352
	.text
	.align	2
	.ent	wma_decode_init
	.type	wma_decode_init, @function
wma_decode_init:
	.frame	$sp,104,$31		# vars= 0, regs= 6/2, args= 56, gp= 8
	.mask	0x801f0000,-20
	.fmask	0x00f00000,-8
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-104
	sw	$31,84($sp)
	sw	$20,80($sp)
	sw	$19,76($sp)
	sw	$18,72($sp)
	sw	$17,68($sp)
	sw	$16,64($sp)
	sdc1	$f22,96($sp)
	sdc1	$f20,88($sp)
	.cprestore	56
	lw	$2,132($4)
	lw	$20,136($4)
	lw	$3,8($2)
	li	$5,65536			# 0x10000
	ori	$2,$5,0x5008
	sw	$4,0($20)
	beq	$3,$2,$L231
	lw	$6,24($4)

	ori	$2,$5,0x5009
	beq	$3,$2,$L232
	nop

$L198:
	move	$6,$0
	move	$2,$0
	move	$3,$0
	lw	$25,%call16(ff_wma_init)($28)
	move	$5,$0
	sw	$6,68($20)
	sw	$2,60($20)
	jalr	$25
	sw	$3,64($20)

	bltz	$2,$L203
	lw	$28,56($sp)

$L235:
	lw	$3,%got(jz_cpu_type_id)($28)
	lw	$2,0($3)
	slt	$2,$2,4750
	bne	$2,$0,$L205
	lw	$3,%got(ff_imdct_half_fixed_4740)($28)

	lw	$3,%got(ff_imdct_half_fixed_4750)($28)
	lw	$2,%got(ff_imdct_half_fixed)($28)
	sw	$3,0($2)
$L207:
	lw	$2,1096($20)
	blez	$2,$L208
	li	$2,58512			# 0xe490

	addu	$16,$20,$2
	move	$17,$0
$L210:
	lw	$5,1092($20)
	lw	$25,%call16(ff_mdct_init_fixed)($28)
	subu	$5,$5,$17
	move	$4,$16
	addiu	$5,$5,1
	jalr	$25
	li	$6,1			# 0x1

	lw	$3,1096($20)
	addiu	$17,$17,1
	slt	$3,$17,$3
	lw	$28,56($sp)
	bne	$3,$0,$L210
	addiu	$16,$16,36

$L208:
	lw	$2,72($20)
	bne	$2,$0,$L233
	lw	$2,%got(ff_wma_hgain_huffcodes)($28)

$L211:
	lw	$2,68($20)
	bne	$2,$0,$L234
	lw	$2,%got($LC6)($28)

	lw	$18,1088($20)
	mtc1	$18,$f0
	cvt.d.w	$f2,$f0
	ldc1	$f0,%lo($LC6)($2)
	div.d	$f0,$f0,$f2
	blez	$18,$L216
	cvt.s.d	$f20,$f0

	lw	$3,%got($LC7)($28)
	li	$2,131072			# 0x20000
	lwc1	$f22,%lo($LC7)($3)
	ori	$2,$2,0x4574
	addu	$16,$20,$2
	move	$17,$0
$L218:
	mtc1	$17,$f0
	lw	$25,%call16(cos)($28)
	cvt.s.w	$f12,$f0
	mul.s	$f12,$f20,$f12
	addiu	$17,$17,1
	jalr	$25
	cvt.d.s	$f12,$f12

	add.d	$f0,$f0,$f0
	lw	$28,56($sp)
	cvt.s.d	$f0,$f0
	mul.s	$f0,$f0,$f22
	trunc.w.s $f2,$f0
	swc1	$f2,0($16)
	bne	$18,$17,$L218
	addiu	$16,$16,4

$L216:
	lw	$2,%got($LC8)($28)
	lw	$19,%got($LC9)($28)
	ldc1	$f22,%lo($LC8)($2)
	ldc1	$f20,%lo($LC9)($19)
	li	$2,131072			# 0x20000
	ori	$2,$2,0x8d74
	addu	$16,$20,$2
	li	$17,-126			# 0xffffffffffffff82
	li	$18,130			# 0x82
$L219:
	mtc1	$17,$f0
	lw	$25,%call16(pow)($28)
	cvt.d.w	$f14,$f0
	mul.d	$f14,$f14,$f20
	mov.d	$f12,$f22
	jalr	$25
	addiu	$17,$17,1

	cvt.s.d	$f0,$f0
	lw	$28,56($sp)
	swc1	$f0,0($16)
	bne	$17,$18,$L219
	addiu	$16,$16,4

	lw	$2,%got($LC10)($28)
	lw	$3,%got($LC11)($28)
	lwc1	$f20,%lo($LC10)($2)
	lwc1	$f22,%lo($LC11)($3)
	li	$2,196608			# 0x30000
	ori	$2,$2,0x1fc
	addu	$16,$20,$2
	li	$17,127			# 0x7f
	li	$18,-1			# 0xffffffffffffffff
$L221:
	addiu	$2,$17,128
	mtc1	$2,$f0
	ldc1	$f14,%lo($LC9)($19)
	cvt.s.w	$f12,$f0
	mul.s	$f12,$f12,$f22
	lw	$25,%call16(pow)($28)
	addiu	$17,$17,-1
	jalr	$25
	cvt.d.s	$f12,$f12

	cvt.s.d	$f0,$f0
	add.s	$f2,$f0,$f0
	sub.s	$f4,$f20,$f0
	lw	$28,56($sp)
	sub.s	$f2,$f2,$f20
	swc1	$f4,-27788($16)
	mov.s	$f20,$f0
	swc1	$f2,-28300($16)
	bne	$17,$18,$L221
	addiu	$16,$16,-4

	ldc1	$f22,96($sp)
	ldc1	$f20,88($sp)
	lw	$31,84($sp)
	lw	$20,80($sp)
	lw	$19,76($sp)
	lw	$18,72($sp)
	lw	$17,68($sp)
	lw	$16,64($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,104

$L205:
	lw	$2,%got(ff_imdct_half_fixed)($28)
	b	$L207
	sw	$3,0($2)

$L231:
	lw	$2,28($4)
	slt	$2,$2,4
	bne	$2,$0,$L198
	nop

	lbu	$2,3($6)
	lbu	$3,2($6)
$L230:
	sll	$2,$2,8
	or	$5,$2,$3
	andi	$6,$5,0x1
	andi	$2,$5,0x2
	andi	$3,$5,0x4
	lw	$25,%call16(ff_wma_init)($28)
	sw	$6,68($20)
	sw	$2,60($20)
	jalr	$25
	sw	$3,64($20)

	bgez	$2,$L235
	lw	$28,56($sp)

$L203:
	ldc1	$f22,96($sp)
	ldc1	$f20,88($sp)
	lw	$31,84($sp)
	lw	$20,80($sp)
	lw	$19,76($sp)
	lw	$18,72($sp)
	lw	$17,68($sp)
	lw	$16,64($sp)
	li	$2,-1			# 0xffffffffffffffff
	j	$31
	addiu	$sp,$sp,104

$L234:
	lw	$2,%got(ff_wma_scale_huffcodes)($28)
	li	$3,1			# 0x1
	li	$4,4			# 0x4
	lw	$25,%call16(init_vlc_sparse)($28)
	lw	$7,%got(ff_wma_scale_huffbits)($28)
	sw	$4,32($sp)
	sw	$4,28($sp)
	sw	$3,20($sp)
	sw	$2,24($sp)
	sw	$3,16($sp)
	sw	$0,36($sp)
	sw	$0,40($sp)
	sw	$0,44($sp)
	sw	$0,48($sp)
	addiu	$4,$20,80
	li	$5,8			# 0x8
	jalr	$25
	li	$6,121			# 0x79

	ldc1	$f22,96($sp)
	ldc1	$f20,88($sp)
	lw	$28,56($sp)
	lw	$31,84($sp)
	lw	$20,80($sp)
	lw	$19,76($sp)
	lw	$18,72($sp)
	lw	$17,68($sp)
	lw	$16,64($sp)
	move	$2,$0
	j	$31
	addiu	$sp,$sp,104

$L233:
	li	$3,1			# 0x1
	li	$4,2			# 0x2
	lw	$25,%call16(init_vlc_sparse)($28)
	lw	$7,%got(ff_wma_hgain_huffbits)($28)
	sw	$4,32($sp)
	sw	$4,28($sp)
	sw	$3,20($sp)
	sw	$2,24($sp)
	sw	$3,16($sp)
	sw	$0,36($sp)
	sw	$0,40($sp)
	sw	$0,44($sp)
	sw	$0,48($sp)
	addiu	$4,$20,752
	li	$5,9			# 0x9
	jalr	$25
	li	$6,37			# 0x25

	b	$L211
	lw	$28,56($sp)

$L232:
	lw	$2,28($4)
	slt	$2,$2,6
	bne	$2,$0,$L198
	nop

	lbu	$2,5($6)
	b	$L230
	lbu	$3,4($6)

	.set	macro
	.set	reorder
	.end	wma_decode_init
	.section	.rodata.str1.4,"aMS",@progbits,1
	.align	2
$LC12:
	.ascii	" ++++ found error last_exp(1) = %d +++\012\000"
	.align	2
$LC13:
	.ascii	" ++++ found error last_exp(2) = %d +++\012\000"
	.globl	__divdi3
	.align	2
$LC14:
	.ascii	"overflow in spectral RLE, ignoring\012\000"
	.section	.rodata.cst4
	.align	2
$LC15:
	.word	1065353216
	.align	2
$LC16:
	.word	1199570944
	.section	.data.rel.ro,"aw",@progbits
	.align	2
$LC17:
	.word	ff_wma_lspfix_codebook
	.align	2
$LC18:
	.word	wma_window
	.text
	.align	2
	.ent	wma_decode_frame
	.type	wma_decode_frame, @function
wma_decode_frame:
	.frame	$sp,288,$31		# vars= 224, regs= 10/0, args= 16, gp= 8
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-288
	sw	$31,284($sp)
	sw	$fp,280($sp)
	sw	$23,276($sp)
	sw	$22,272($sp)
	sw	$21,268($sp)
	sw	$20,264($sp)
	sw	$19,260($sp)
	sw	$18,256($sp)
	sw	$17,252($sp)
	sw	$16,248($sp)
	.cprestore	16
	li	$2,65536			# 0x10000
	move	$20,$4
	ori	$2,$2,0x560
	li	$6,131072			# 0x20000
	addu	$3,$4,$2
	sw	$5,292($sp)
	sw	$0,1120($4)
	sw	$0,1124($4)
	addiu	$5,$20,1152
	addiu	$4,$sp,32
	addu	$6,$20,$6
	sw	$3,232($sp)
	sw	$4,224($sp)
	sw	$5,228($sp)
	sw	$6,204($sp)
	sw	$3,208($sp)
$L237:
	lw	$2,64($20)
	beq	$2,$0,$L238
	li	$2,-65536			# 0xffffffffffff0000

	lw	$3,1096($20)
	addiu	$3,$3,-1
	and	$2,$3,$2
	bne	$2,$0,$L240
	nop

	move	$6,$0
$L242:
	andi	$2,$3,0xff00
	beq	$2,$0,$L562
	lw	$2,%got(ff_log2_tab)($28)

	srl	$3,$3,8
	addiu	$6,$6,8
$L562:
	lw	$5,1100($20)
	addu	$2,$3,$2
	lbu	$4,0($2)
	addiu	$3,$6,1
	beq	$5,$0,$L245
	addu	$10,$4,$3

	sw	$0,1100($20)
	addiu	$18,$20,4
	lw	$6,8($18)
	lw	$8,4($20)
	sra	$3,$6,3
	addu	$3,$3,$8
	lbu	$2,0($3)
	lbu	$5,1($3)
	lbu	$7,3($3)
	lbu	$4,2($3)
	sll	$2,$2,24
	sll	$5,$5,16
	or	$2,$2,$5
	addu	$3,$10,$6
	or	$2,$2,$7
	sll	$4,$4,8
	or	$4,$4,$2
	sw	$3,8($18)
	andi	$6,$6,0x7
	li	$3,32			# 0x20
	sll	$4,$4,$6
	subu	$9,$3,$10
	lw	$2,1096($20)
	srl	$4,$4,$9
	slt	$2,$4,$2
	beq	$2,$0,$L472
	li	$2,-1			# 0xffffffffffffffff

	lw	$2,1092($20)
	subu	$2,$2,$4
	sw	$2,1112($20)
	lw	$6,8($18)
	sra	$4,$6,3
	addu	$4,$4,$8
	lbu	$2,0($4)
	lbu	$5,1($4)
	lbu	$7,3($4)
	lbu	$3,2($4)
	sll	$2,$2,24
	sll	$5,$5,16
	or	$2,$2,$5
	or	$2,$2,$7
	addu	$4,$10,$6
	sll	$3,$3,8
	or	$3,$3,$2
	sw	$4,8($18)
	andi	$6,$6,0x7
	sll	$3,$3,$6
	lw	$2,1096($20)
	srl	$3,$3,$9
	slt	$2,$3,$2
	beq	$2,$0,$L472
	li	$2,-1			# 0xffffffffffffffff

	lw	$2,1092($20)
	subu	$2,$2,$3
	sw	$2,1104($20)
$L250:
	lw	$6,8($18)
	sra	$4,$6,3
	addu	$4,$4,$8
	lbu	$2,0($4)
	lbu	$5,1($4)
	lbu	$7,3($4)
	lbu	$3,2($4)
	sll	$2,$2,24
	sll	$5,$5,16
	or	$2,$2,$5
	or	$2,$2,$7
	addu	$4,$10,$6
	sll	$3,$3,8
	or	$3,$3,$2
	sw	$4,8($18)
	andi	$6,$6,0x7
	sll	$3,$3,$6
	lw	$2,1096($20)
	srl	$3,$3,$9
	slt	$2,$3,$2
	beq	$2,$0,$L472
	li	$2,-1			# 0xffffffffffffffff

	lw	$2,1092($20)
	subu	$2,$2,$3
	sw	$2,1108($20)
$L252:
	lw	$2,1104($20)
	lw	$3,1124($20)
	li	$7,1			# 0x1
	sll	$9,$7,$2
	lw	$4,1088($20)
	addu	$3,$9,$3
	slt	$3,$4,$3
	bne	$3,$0,$L247
	sw	$9,1116($20)

	lw	$8,44($20)
	li	$2,2			# 0x2
	beq	$8,$2,$L532
	addiu	$18,$20,4

	blez	$8,$L476
	nop

$L538:
	move	$7,$0
	move	$6,$0
	addiu	$18,$20,4
$L258:
	lw	$5,8($18)
	lw	$2,4($20)
	sra	$4,$5,3
	addu	$2,$2,$4
	lbu	$3,0($2)
	addiu	$12,$5,1
	sw	$12,8($18)
	andi	$5,$5,0x7
	sll	$3,$3,$5
	lw	$8,44($20)
	addu	$4,$6,$20
	andi	$3,$3,0x00ff
	addiu	$6,$6,1
	srl	$3,$3,7
	slt	$2,$6,$8
	sb	$3,1129($4)
	bne	$2,$0,$L258
	or	$7,$7,$3

	beq	$7,$0,$L476
	li	$7,127			# 0x7f

	lw	$3,1092($20)
	lw	$2,1104($20)
	li	$8,1			# 0x1
	lw	$12,8($18)
	lw	$13,0($18)
	subu	$fp,$3,$2
	sw	$8,172($sp)
$L261:
	sra	$4,$12,3
	addu	$4,$4,$13
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	andi	$4,$12,0x7
	or	$2,$2,$3
	lw	$9,172($sp)
	sll	$2,$2,$4
	srl	$2,$2,25
	addiu	$12,$12,7
	addu	$9,$9,$2
	sw	$12,8($18)
	beq	$2,$7,$L261
	sw	$9,172($sp)

	lw	$25,%call16(ff_wma_total_gain_to_bits)($28)
	sll	$11,$fp,2
	move	$4,$9
	jalr	$25
	sw	$11,212($sp)

	lw	$14,212($sp)
	sw	$2,156($sp)
	addu	$2,$14,$20
	lw	$4,392($2)
	lw	$3,388($20)
	lw	$8,44($20)
	lw	$28,16($sp)
	blez	$8,$L263
	subu	$4,$4,$3

	move	$3,$0
	addiu	$2,$sp,24
$L265:
	addiu	$3,$3,1
	sw	$4,0($2)
	bne	$8,$3,$L265
	addiu	$2,$2,4

$L263:
	lw	$2,72($20)
	beq	$2,$0,$L266
	nop

	blez	$8,$L266
	lw	$25,212($sp)

	sll	$2,$fp,6
	addiu	$16,$2,432
	move	$11,$0
	addiu	$10,$sp,24
	addu	$15,$25,$20
	li	$14,768			# 0x300
$L269:
	addu	$2,$11,$20
	lbu	$3,1129($2)
	beq	$3,$0,$L270
	nop

	lw	$9,412($15)
	blez	$9,$L270
	nop

	lw	$13,0($18)
	addu	$6,$20,$14
	addu	$7,$20,$16
	move	$8,$0
$L273:
	lw	$5,8($18)
	addiu	$8,$8,1
	sra	$2,$5,3
	addu	$2,$13,$2
	lbu	$3,0($2)
	andi	$4,$5,0x7
	sll	$3,$3,$4
	andi	$3,$3,0x00ff
	srl	$3,$3,7
	addiu	$12,$5,1
	sw	$12,8($18)
	sw	$3,0($6)
	beq	$3,$0,$L274
	addiu	$6,$6,4

	lw	$2,0($10)
	lw	$3,0($7)
	subu	$2,$2,$3
	sw	$2,0($10)
$L274:
	bne	$9,$8,$L273
	addiu	$7,$7,4

	lw	$8,44($20)
$L270:
	addiu	$11,$11,1
	slt	$2,$11,$8
	addiu	$14,$14,64
	bne	$2,$0,$L269
	addiu	$10,$10,4

	blez	$8,$L266
	lw	$2,212($sp)

	move	$17,$0
	addu	$22,$2,$20
	li	$21,768			# 0x300
$L278:
	addu	$2,$17,$20
	lbu	$3,1129($2)
	beq	$3,$0,$L279
	nop

	lw	$16,412($22)
	blez	$16,$L279
	nop

	addu	$10,$20,$21
	move	$14,$0
	li	$15,-2147483648			# 0xffffffff80000000
	li	$19,-2147483648			# 0xffffffff80000000
$L282:
	lw	$2,0($10)
	beq	$2,$0,$L283
	nop

	beq	$15,$19,$L533
	nop

	lw	$11,8($18)
	lw	$13,0($18)
	sra	$4,$11,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$11,0x7
	sll	$2,$2,$4
	lw	$12,756($20)
	srl	$2,$2,23
	sll	$2,$2,2
	addu	$2,$12,$2
	lh	$8,2($2)
	bltz	$8,$L534
	lh	$9,0($2)

	addu	$12,$11,$8
	addu	$2,$9,$15
	bltz	$9,$L247
	sw	$12,8($18)

$L539:
	addiu	$15,$2,-18
$L287:
	sw	$15,128($10)
$L283:
	addiu	$14,$14,1
	bne	$16,$14,$L282
	addiu	$10,$10,4

	lw	$8,44($20)
$L279:
	addiu	$17,$17,1
	slt	$2,$17,$8
	bne	$2,$0,$L278
	addiu	$21,$21,64

$L266:
	lw	$3,1104($20)
	lw	$2,1092($20)
	beq	$3,$2,$L291
	nop

	lw	$12,8($18)
	lw	$13,0($18)
	sra	$2,$12,3
	addu	$2,$13,$2
	lbu	$3,0($2)
	andi	$4,$12,0x7
	sll	$3,$3,$4
	andi	$3,$3,0x00ff
	addiu	$12,$12,1
	srl	$3,$3,7
	bne	$3,$0,$L535
	sw	$12,8($18)

	lw	$8,44($20)
$L293:
	blez	$8,$L294
	li	$11,17544			# 0x4488

	move	$21,$0
	addiu	$23,$sp,24
	li	$22,1			# 0x1
	sw	$11,220($sp)
$L365:
	addu	$2,$21,$20
	lbu	$3,1129($2)
	beq	$3,$0,$L366
	nop

	beq	$21,$22,$L368
	nop

	move	$2,$0
	move	$17,$0
$L370:
	sll	$2,$2,2
	lw	$14,220($sp)
	addu	$2,$2,$20
	lw	$7,1064($2)
	lw	$3,0($23)
	lw	$6,1116($20)
	lw	$2,1056($2)
	addu	$16,$20,$14
	lw	$25,%call16(memset)($28)
	sll	$3,$3,1
	sll	$6,$6,1
	move	$4,$16
	move	$5,$0
	sw	$7,180($sp)
	sw	$2,184($sp)
	jalr	$25
	addu	$19,$16,$3

	sll	$3,$17,4
	addu	$3,$3,$20
	lw	$28,16($sp)
	lw	$14,1028($3)
	lw	$12,8($18)
	lw	$13,0($18)
	b	$L371
	li	$15,32			# 0x20

$L537:
	sra	$2,$12,3
	andi	$11,$12,0x7
	addu	$12,$8,$12
	addu	$2,$2,$13
	sra	$5,$12,3
	addu	$5,$5,$13
	lbu	$10,3($2)
	lbu	$7,2($2)
	lbu	$4,0($2)
	lbu	$8,1($2)
	sw	$12,8($18)
	lbu	$2,0($5)
	lbu	$6,1($5)
	lbu	$9,3($5)
	lbu	$3,2($5)
	sll	$4,$4,24
	sll	$8,$8,16
	sll	$2,$2,24
	sll	$6,$6,16
	or	$4,$4,$8
	or	$2,$2,$6
	or	$2,$2,$9
	lw	$5,1092($20)
	or	$4,$4,$10
	sll	$7,$7,8
	sll	$3,$3,8
	lw	$9,156($sp)
	or	$7,$7,$4
	or	$3,$3,$2
	andi	$4,$12,0x7
	sll	$3,$3,$4
	subu	$2,$15,$9
	sll	$7,$7,$11
	subu	$4,$15,$5
	addu	$12,$12,$5
	srl	$7,$7,$2
	sw	$12,8($18)
	srl	$2,$3,$4
$L379:
	lw	$12,8($18)
	sll	$5,$2,1
	sra	$2,$12,3
	addu	$2,$13,$2
	lbu	$3,0($2)
	andi	$4,$12,0x7
	sll	$3,$3,$4
	addu	$5,$16,$5
	andi	$3,$3,0x00ff
	addiu	$12,$12,1
	srl	$3,$3,7
	subu	$2,$0,$7
	sltu	$4,$5,$19
	movz	$7,$2,$3
	beq	$4,$0,$L484
	sw	$12,8($18)

	addiu	$16,$5,2
	sltu	$2,$16,$19
	beq	$2,$0,$L478
	sh	$7,0($5)

$L371:
	sra	$4,$12,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$12,0x7
	sll	$2,$2,$4
	srl	$2,$2,23
	sll	$2,$2,2
	addu	$2,$14,$2
	lh	$8,2($2)
	move	$9,$12
	bltz	$8,$L536
	lh	$7,0($2)

$L372:
	addu	$12,$9,$8
	bltz	$7,$L247
	sw	$12,8($18)

	beq	$7,$22,$L478
	nop

	beq	$7,$0,$L537
	lw	$8,156($sp)

	lw	$11,180($sp)
	lw	$25,184($sp)
	sll	$2,$7,1
	addu	$3,$11,$2
	addu	$2,$25,$2
	lhu	$2,0($2)
	b	$L379
	lhu	$7,0($3)

$L532:
	lw	$5,8($18)
	lw	$8,4($20)
	sra	$2,$5,3
	addu	$2,$8,$2
	lbu	$3,0($2)
	andi	$4,$5,0x7
	addiu	$5,$5,1
	sw	$5,8($18)
	sll	$3,$3,$4
	andi	$3,$3,0x00ff
	lw	$8,44($20)
	srl	$3,$3,7
	bgtz	$8,$L538
	sb	$3,1128($20)

$L476:
	lw	$9,1116($20)
$L256:
	lw	$2,1124($20)
	lw	$3,1120($20)
	lw	$6,1088($20)
	addu	$2,$2,$9
	addiu	$3,$3,1
	slt	$4,$2,$6
	sw	$3,1120($20)
	sw	$2,1124($20)
	bne	$4,$0,$L237
	move	$16,$6

	blez	$8,$L471
	lw	$19,292($sp)

	sll	$17,$8,1
	move	$18,$0
$L463:
	li	$3,65536			# 0x10000
	sll	$2,$18,14
	ori	$3,$3,0x560
	addu	$2,$2,$3
	blez	$16,$L464
	addu	$4,$20,$2

	move	$8,$19
	move	$7,$4
	move	$9,$0
	li	$11,32768			# 0x8000
	li	$10,-65536			# 0xffffffffffff0000
$L466:
	lw	$2,0($7)
	addiu	$7,$7,4
	sra	$3,$2,31
	sra	$5,$2,14
	xori	$3,$3,0x7fff
	addu	$2,$5,$11
	sll	$3,$3,16
	and	$2,$2,$10
	bne	$2,$0,$L469
	sra	$3,$3,16

	sll	$3,$5,16
	sra	$3,$3,16
$L469:
	addiu	$9,$9,1
	sh	$3,0($8)
	bne	$9,$16,$L466
	addu	$8,$8,$17

$L464:
	sll	$5,$18,12
	addu	$5,$6,$5
	lw	$7,208($sp)
	lw	$25,%call16(memmove)($28)
	sll	$5,$5,2
	addu	$5,$7,$5
	jalr	$25
	sll	$6,$6,2

	lw	$3,44($20)
	addiu	$18,$18,1
	slt	$3,$18,$3
	lw	$28,16($sp)
	beq	$3,$0,$L471
	addiu	$19,$19,2

	b	$L463
	lw	$6,1088($20)

$L533:
	lw	$6,8($18)
	lw	$13,0($18)
	sra	$4,$6,3
	addu	$4,$4,$13
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$7,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$7
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$6,0x7
	sll	$2,$2,$4
	srl	$2,$2,25
	addiu	$12,$6,7
	addiu	$15,$2,-19
	b	$L287
	sw	$12,8($18)

$L534:
	addiu	$11,$11,9
	sra	$4,$11,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	andi	$7,$11,0x7
	or	$2,$2,$3
	sll	$2,$2,$7
	srl	$2,$2,$8
	addu	$2,$2,$9
	sll	$2,$2,2
	addu	$2,$12,$2
	lh	$8,2($2)
	lh	$9,0($2)
	addu	$12,$11,$8
	addu	$2,$9,$15
	bgez	$9,$L539
	sw	$12,8($18)

$L247:
	li	$2,-1			# 0xffffffffffffffff
$L472:
	lw	$31,284($sp)
	lw	$fp,280($sp)
	lw	$23,276($sp)
	lw	$22,272($sp)
	lw	$21,268($sp)
	lw	$20,264($sp)
	lw	$19,260($sp)
	lw	$18,256($sp)
	lw	$17,252($sp)
	lw	$16,248($sp)
	j	$31
	addiu	$sp,$sp,288

$L238:
	lw	$2,1092($20)
	sw	$2,1104($20)
	sw	$2,1108($20)
	b	$L252
	sw	$2,1112($20)

$L245:
	lw	$2,1104($20)
	lw	$3,1108($20)
	sw	$2,1112($20)
	lw	$8,4($20)
	li	$2,32			# 0x20
	subu	$9,$2,$10
	addiu	$18,$20,4
	b	$L250
	sw	$3,1104($20)

$L240:
	srl	$3,$3,16
	b	$L242
	li	$6,16			# 0x10

$L535:
	lw	$8,44($20)
$L291:
	blez	$8,$L294
	addiu	$3,$20,17536

	sw	$3,140($sp)
	sw	$0,164($sp)
$L296:
	lw	$4,164($sp)
	addu	$2,$20,$4
	lbu	$3,1129($2)
	beq	$3,$0,$L563
	lw	$5,140($sp)

	lw	$2,68($20)
	beq	$2,$0,$L299
	lw	$9,224($sp)

	lw	$3,1104($20)
	lw	$2,1092($20)
	sll	$5,$4,13
	subu	$2,$2,$3
	sll	$4,$2,3
	sll	$2,$2,1
	addu	$2,$2,$4
	sll	$3,$2,2
	lw	$4,1116($20)
	addu	$2,$2,$3
	addiu	$5,$5,1152
	lw	$3,52($20)
	addu	$17,$20,$5
	addu	$2,$2,$20
	sll	$4,$4,2
	li	$5,1			# 0x1
	addiu	$22,$2,116
	beq	$3,$5,$L540
	addu	$23,$17,$4

	li	$19,36			# 0x24
	move	$21,$0
$L310:
	sltu	$2,$17,$23
	beq	$2,$0,$L564
	slt	$2,$0,$21

$L520:
	lw	$11,8($18)
	lw	$13,0($18)
	sra	$4,$11,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$11,0x7
	sll	$2,$2,$4
	srl	$2,$2,24
	lw	$9,84($20)
	sll	$2,$2,2
	addu	$2,$9,$2
	lh	$7,2($2)
	bltz	$7,$L541
	lh	$8,0($2)

$L312:
	addu	$12,$11,$7
	bltz	$8,$L247
	sw	$12,8($18)

	addu	$2,$8,$19
	addiu	$19,$2,-60
	bltz	$19,$L542
	slt	$2,$19,78

	beq	$2,$0,$L319
	lw	$3,%got(tablefix16_10exp1div16)($28)

	sll	$2,$19,2
	addu	$2,$2,$3
	lw	$16,0($2)
$L318:
	slt	$2,$21,$16
	lhu	$3,0($22)
	movn	$21,$16,$2
$L321:
	addiu	$3,$3,-1
	sw	$16,0($17)
	bne	$3,$0,$L321
	addiu	$17,$17,4

	sltu	$2,$17,$23
	beq	$2,$0,$L323
	slt	$2,$0,$21

	b	$L520
	addiu	$22,$22,2

$L478:
	lw	$8,44($20)
$L366:
	lw	$2,52($20)
	beq	$2,$22,$L543
	slt	$2,$8,2

$L384:
	lw	$3,220($sp)
$L567:
	addiu	$21,$21,1
	addiu	$3,$3,4096
	slt	$2,$21,$8
	sw	$3,220($sp)
	bne	$2,$0,$L365
	addiu	$23,$23,4

$L294:
	lw	$9,1116($20)
	lw	$3,52($20)
	srl	$2,$9,31
	addu	$2,$2,$9
	sra	$2,$2,1
	mtc1	$2,$f6
	lw	$2,%got($LC15)($28)
	cvt.s.w	$f2,$f6
	lwc1	$f0,%lo($LC15)($2)
	li	$4,1			# 0x1
	beq	$3,$4,$L544
	div.s	$f4,$f0,$f2

$L388:
	lw	$2,%got($LC16)($28)
	lwc1	$f0,%lo($LC16)($2)
	mul.s	$f0,$f4,$f0
	trunc.w.s $f0,$f0
	blez	$8,$L390
	swc1	$f0,176($sp)

	addiu	$5,$sp,24
	addiu	$6,$20,17536
	sw	$5,144($sp)
	sw	$6,148($sp)
	sw	$0,136($sp)
	sw	$5,216($sp)
$L392:
	lw	$7,136($sp)
	addu	$2,$20,$7
	lbu	$3,1129($2)
	beq	$3,$0,$L565
	lw	$4,136($sp)

	sll	$2,$7,12
	sll	$17,$7,13
	addiu	$2,$2,17544
	lw	$8,148($sp)
	lw	$4,68($20)
	addiu	$3,$17,1152
	addu	$2,$20,$2
	sw	$2,188($sp)
	addu	$16,$20,$3
	beq	$4,$0,$L395
	lw	$22,-16404($8)

	lw	$9,172($sp)
	lw	$11,%got(pow_table)($28)
	sll	$2,$9,3
	addu	$2,$2,$11
	lw	$6,0($2)
	lw	$3,4($2)
	lw	$8,0($8)
	srl	$2,$6,17
	lw	$25,%call16(__divdi3)($28)
	sll	$5,$3,15
	sll	$4,$6,15
	or	$5,$2,$5
	move	$6,$8
	jalr	$25
	sra	$7,$8,31

	lw	$4,176($sp)
	lw	$28,16($sp)
	mult	$4,$2
	mfhi	$3
	mflo	$2
	sll	$4,$3,16
	srl	$2,$2,16
	or	$2,$4,$2
	lw	$3,72($20)
	sw	$2,192($sp)
	addiu	$2,$17,25744
	beq	$3,$0,$L398
	addu	$18,$20,$2

$L557:
	lw	$12,388($20)
	blez	$12,$L401
	lw	$8,192($sp)

	li	$13,131072			# 0x20000
	sra	$9,$8,31
	move	$11,$0
	addu	$10,$20,$13
$L404:
	sll	$2,$11,$fp
	sra	$2,$2,$22
	sll	$2,$2,2
	addu	$2,$2,$16
	lw	$3,0($2)
	lw	$6,17772($10)
	sra	$5,$3,31
	mul	$12,$5,$8
	multu	$8,$3
	move	$4,$3
	mflo	$2
	mfhi	$3
	mul	$7,$9,$4
	sll	$6,$6,2
	addu	$4,$7,$12
	addu	$6,$6,$20
	addu	$3,$4,$3
	addu	$6,$6,$13
	sll	$4,$3,17
	lw	$7,-14996($6)
	srl	$2,$2,15
	or	$2,$4,$2
	mult	$2,$7
	addiu	$11,$11,1
	mflo	$2
	mfhi	$3
	srl	$2,$2,18
	sll	$4,$3,14
	or	$2,$4,$2
	sw	$2,0($18)
	lw	$4,17772($10)
	lw	$12,388($20)
	addiu	$4,$4,1
	andi	$4,$4,0x1fff
	slt	$2,$11,$12
	sw	$4,17772($10)
	bne	$2,$0,$L404
	addiu	$18,$18,4

$L401:
	lw	$3,212($sp)
	lw	$6,136($sp)
	addu	$2,$3,$20
	lw	$4,412($2)
	li	$5,1			# 0x1
	sll	$6,$6,11
	lw	$3,368($2)
	sw	$4,196($sp)
	sw	$5,72($sp)
	blez	$4,$L405
	sw	$6,168($sp)

	lw	$4,1104($20)
	lw	$2,1092($20)
	lw	$7,136($sp)
	subu	$2,$2,$4
	sll	$3,$3,$fp
	addu	$3,$3,$6
	sll	$2,$2,6
	sll	$4,$7,6
	lw	$8,228($sp)
	sll	$3,$3,2
	addiu	$2,$2,432
	addiu	$4,$4,768
	addiu	$9,$sp,72
	addu	$17,$8,$3
	addu	$21,$20,$2
	addu	$19,$20,$4
	move	$23,$0
	sw	$0,200($sp)
	sw	$9,152($sp)
$L407:
	lw	$2,0($19)
	beq	$2,$0,$L408
	lw	$16,0($21)

	blez	$16,$L545
	move	$10,$0

	move	$8,$0
	move	$9,$0
$L413:
	sll	$4,$10,$fp
	sra	$4,$4,$22
	sll	$4,$4,2
	addu	$4,$4,$17
	lw	$5,0($4)
	addiu	$10,$10,1
	sra	$3,$5,31
	mul	$4,$3,$5
	multu	$5,$5
	sll	$4,$4,1
	mfhi	$3
	mflo	$2
	addu	$3,$4,$3
	sll	$5,$3,18
	srl	$2,$2,14
	or	$2,$5,$2
	addu	$6,$8,$2
	sra	$3,$3,14
	sltu	$4,$6,$8
	addu	$7,$9,$3
	addu	$7,$4,$7
	move	$8,$6
	bne	$16,$10,$L413
	move	$9,$7

	srl	$2,$6,16
	sll	$5,$7,16
	or	$5,$2,$5
	sll	$4,$6,16
$L412:
	lw	$25,%call16(__divdi3)($28)
	sw	$12,240($sp)
	move	$6,$16
	jalr	$25
	sra	$7,$16,31

	sll	$4,$3,16
	srl	$2,$2,16
	or	$2,$4,$2
	lw	$4,200($sp)
	lw	$5,152($sp)
	movn	$4,$23,$2
	lw	$12,240($sp)
	lw	$28,16($sp)
	sw	$4,200($sp)
	sw	$2,0($5)
$L408:
	lw	$6,152($sp)
	lw	$7,196($sp)
	addiu	$6,$6,4
	addiu	$23,$23,1
	addiu	$21,$21,4
	addiu	$19,$19,4
	beq	$7,$23,$L416
	sw	$6,152($sp)

	sll	$2,$16,$fp
	sll	$2,$2,2
	b	$L407
	addu	$17,$17,$2

$L550:
	lw	$9,164($sp)
	lw	$11,1116($20)
	sll	$2,$9,13
	addiu	$2,$2,1152
	sw	$11,160($sp)
	blez	$11,$L546
	addu	$2,$20,$2

	li	$14,131072			# 0x20000
	ori	$14,$14,0x4574
	move	$17,$2
	addu	$19,$20,$14
	move	$22,$0
	move	$21,$0
	addiu	$23,$sp,72
$L335:
	lw	$9,0($19)
	lw	$8,224($sp)
	li	$5,32768			# 0x8000
	li	$6,32768			# 0x8000
$L336:
	lw	$2,0($8)
	lw	$4,4($8)
	subu	$2,$9,$2
	mult	$6,$2
	subu	$4,$9,$4
	mflo	$2
	mfhi	$3
	mult	$5,$4
	sll	$6,$3,16
	mfhi	$5
	mflo	$4
	sll	$7,$5,16
	srl	$14,$2,16
	srl	$2,$4,16
	or	$14,$6,$14
	or	$2,$7,$2
	addiu	$8,$8,8
	move	$6,$14
	bne	$23,$8,$L336
	move	$5,$2

	li	$4,131072			# 0x20000
	subu	$6,$4,$9
	addu	$8,$9,$4
	sra	$9,$8,31
	sra	$7,$6,31
	mul	$16,$7,$2
	mul	$15,$9,$14
	multu	$2,$6
	move	$12,$2
	sra	$13,$2,31
	mfhi	$3
	mflo	$2
	multu	$14,$8
	sra	$11,$14,31
	mflo	$4
	mfhi	$5
	move	$10,$14
	mul	$7,$11,$8
	mul	$14,$13,$6
	addu	$8,$7,$15
	addu	$6,$14,$16
	addu	$5,$8,$5
	addu	$3,$6,$3
	sll	$6,$3,16
	sll	$7,$5,16
	srl	$2,$2,16
	srl	$4,$4,16
	or	$2,$6,$2
	or	$4,$7,$4
	sra	$9,$2,31
	sra	$7,$4,31
	mul	$16,$9,$12
	mul	$15,$7,$10
	multu	$12,$2
	move	$6,$4
	mfhi	$5
	mflo	$4
	multu	$10,$6
	move	$8,$2
	mfhi	$3
	mflo	$2
	mul	$14,$13,$8
	mul	$9,$11,$6
	addu	$8,$14,$16
	addu	$6,$9,$15
	addu	$5,$8,$5
	addu	$3,$6,$3
	sll	$8,$5,16
	sll	$6,$3,16
	srl	$4,$4,16
	srl	$2,$2,16
	or	$4,$8,$4
	or	$2,$6,$2
	addu	$16,$4,$2
	blez	$16,$L547
	li	$2,65535			# 0xffff

	slt	$2,$2,$16
	bne	$2,$0,$L341
	sra	$3,$16,31

	li	$10,1			# 0x1
	sll	$7,$16,14
	move	$6,$0
	li	$9,65536			# 0x10000
	li	$11,1			# 0x1
	addu	$2,$9,$10
$L566:
	srl	$8,$2,1
	mul	$4,$8,$8
	move	$5,$0
	srl	$4,$4,1
	mul	$3,$5,$4
	multu	$4,$4
	sll	$3,$3,1
	mfhi	$5
	addu	$3,$3,$5
	slt	$2,$3,$7
	bne	$2,$0,$L344
	mflo	$4

	beq	$3,$7,$L548
	sltu	$2,$4,$6

$L346:
	move	$9,$8
$L347:
	subu	$2,$9,$10
	beq	$2,$11,$L348
	sltu	$3,$10,$9

	bne	$3,$0,$L566
	addu	$2,$9,$10

$L348:
	lw	$25,%call16(__divdi3)($28)
	move	$4,$0
	li	$5,1073741824			# 0x40000000
	move	$6,$10
	jalr	$25
	move	$7,$0

	sll	$4,$3,2
	srl	$2,$2,30
	or	$2,$4,$2
	slt	$5,$16,2048
	lw	$28,16($sp)
	beq	$5,$0,$L359
	move	$4,$2

	lw	$2,%got(pow_m1_4diff)($28)
	addu	$2,$16,$2
	lb	$3,0($2)
	addu	$3,$3,$4
	sra	$4,$3,1
$L340:
	slt	$2,$21,$3
	movn	$21,$3,$2
	lw	$2,160($sp)
	addiu	$22,$22,1
	sw	$4,0($17)
	addiu	$19,$19,4
	bne	$2,$22,$L335
	addiu	$17,$17,4

	sra	$2,$21,1
$L334:
	lw	$3,140($sp)
	sw	$2,0($3)
$L325:
	lw	$4,140($sp)
	lw	$8,44($20)
	sw	$fp,-16404($4)
	lw	$5,140($sp)
$L563:
	lw	$2,0($5)
	blez	$2,$L549
	li	$6,1			# 0x1

$L362:
	lw	$7,164($sp)
	lw	$9,140($sp)
	addiu	$7,$7,1
	addiu	$9,$9,4
	slt	$2,$7,$8
	sw	$7,164($sp)
	bne	$2,$0,$L296
	sw	$9,140($sp)

	b	$L293
	nop

$L536:
	addiu	$9,$12,9
	sra	$4,$9,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$9,0x7
	sll	$2,$2,$4
	srl	$2,$2,$8
	addu	$2,$2,$7
	sll	$2,$2,2
	addu	$2,$14,$2
	subu	$3,$0,$8
	lh	$8,2($2)
	bgez	$8,$L372
	lh	$7,0($2)

	addu	$9,$9,$3
	sra	$4,$9,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$9,0x7
	sll	$2,$2,$4
	srl	$2,$2,$8
	addu	$2,$2,$7
	sll	$2,$2,2
	addu	$2,$14,$2
	lh	$8,2($2)
	b	$L372
	lh	$7,0($2)

$L319:
	lw	$4,%got($LC13)($28)
	lw	$2,%got(tablefix16_10exp1div16)($28)
	lw	$25,%call16(printf)($28)
	addiu	$4,$4,%lo($LC13)
	move	$5,$19
	jalr	$25
	lw	$16,308($2)

	b	$L318
	lw	$28,16($sp)

$L541:
	addiu	$11,$11,8
	sra	$4,$11,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$11,0x7
	sll	$2,$2,$4
	srl	$2,$2,$7
	addu	$2,$2,$8
	sll	$2,$2,2
	addu	$2,$9,$2
	subu	$3,$0,$7
	lh	$7,2($2)
	bgez	$7,$L312
	lh	$8,0($2)

	addu	$11,$11,$3
	sra	$4,$11,3
	addu	$4,$13,$4
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$6,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$6
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$11,0x7
	sll	$2,$2,$4
	srl	$2,$2,$7
	addu	$2,$2,$8
	sll	$2,$2,2
	addu	$2,$9,$2
	lh	$7,2($2)
	b	$L312
	lh	$8,0($2)

$L542:
	b	$L318
	move	$16,$0

$L543:
	bne	$2,$0,$L567
	lw	$3,220($sp)

	lw	$3,8($18)
	subu	$2,$0,$3
	andi	$2,$2,0x7
	beq	$2,$0,$L384
	addu	$2,$3,$2

	sw	$2,8($18)
	b	$L384
	lw	$8,44($20)

$L299:
	lw	$8,%got($LC17)($28)
	lw	$12,8($18)
	addiu	$8,$8,%lo($LC17)
	lw	$13,0($18)
	lw	$11,0($8)
	move	$7,$0
	b	$L326
	li	$14,10			# 0xa

$L551:
	beq	$3,$0,$L327
	nop

	lbu	$3,0($6)
	lbu	$4,1($6)
	lbu	$5,3($6)
	lbu	$2,2($6)
	sll	$3,$3,24
	sll	$4,$4,16
	or	$3,$3,$4
	or	$3,$3,$5
	sll	$2,$2,8
	or	$2,$2,$3
	sll	$2,$2,$8
	addiu	$12,$12,4
	sw	$12,8($18)
	srl	$3,$2,28
$L330:
	sll	$2,$7,4
	addu	$2,$2,$3
	sll	$2,$2,2
	addu	$2,$2,$11
	lw	$3,0($2)
	addiu	$7,$7,1
	sw	$3,0($9)
	beq	$7,$14,$L550
	addiu	$9,$9,4

$L326:
	sra	$2,$12,3
	addu	$6,$2,$13
	andi	$8,$12,0x7
	bne	$7,$0,$L551
	slt	$3,$7,8

$L327:
	lbu	$3,0($6)
	lbu	$4,1($6)
	lbu	$5,3($6)
	lbu	$2,2($6)
	sll	$3,$3,24
	sll	$4,$4,16
	or	$3,$3,$4
	or	$3,$3,$5
	sll	$2,$2,8
	or	$2,$2,$3
	sll	$2,$2,$8
	addiu	$12,$12,3
	srl	$3,$2,29
	b	$L330
	sw	$12,8($18)

$L548:
	beq	$2,$0,$L346
	nop

$L344:
	b	$L347
	move	$10,$8

$L356:
	lw	$25,%call16(__divdi3)($28)
$L568:
	move	$4,$0
	move	$6,$12
	move	$7,$0
	jalr	$25
	li	$5,1073741824			# 0x40000000

	sll	$4,$3,2
	srl	$2,$2,30
	or	$2,$4,$2
	lw	$28,16($sp)
	move	$4,$2
$L359:
	addiu	$3,$4,1
	b	$L340
	sra	$4,$3,1

$L341:
	srl	$4,$16,16
	sll	$9,$3,16
	li	$12,1			# 0x1
	or	$9,$4,$9
	sll	$8,$16,16
	li	$13,1048576			# 0x100000
	b	$L350
	li	$14,1			# 0x1

$L553:
	beq	$3,$9,$L552
	sltu	$2,$4,$8

$L351:
	subu	$2,$10,$12
$L569:
	move	$13,$10
	beq	$2,$14,$L356
	sltu	$4,$12,$10

	beq	$4,$0,$L568
	lw	$25,%call16(__divdi3)($28)

$L350:
	addu	$2,$12,$13
	srl	$10,$2,1
	move	$5,$0
	mul	$3,$5,$10
	multu	$10,$10
	sll	$3,$3,1
	mfhi	$5
	mflo	$4
	addu	$3,$3,$5
	slt	$2,$3,$9
	beq	$2,$0,$L553
	sll	$11,$3,16

	srl	$6,$4,16
	or	$6,$11,$6
$L571:
	multu	$6,$6
	mfhi	$3
	slt	$4,$3,$9
	bne	$4,$0,$L355
	mflo	$2

	bne	$3,$9,$L351
	sltu	$2,$2,$8

	beq	$2,$0,$L569
	subu	$2,$10,$12

$L355:
	move	$12,$10
	b	$L351
	move	$10,$13

$L552:
	beq	$2,$0,$L351
	srl	$6,$4,16

	b	$L571
	or	$6,$11,$6

$L547:
	move	$3,$0
	b	$L340
	move	$4,$0

$L549:
	b	$L362
	sw	$6,0($5)

$L368:
	lbu	$2,1128($20)
	sltu	$17,$0,$2
	b	$L370
	move	$2,$17

$L323:
$L564:
	li	$3,1			# 0x1
	lw	$6,140($sp)
	movn	$3,$21,$2
	b	$L325
	sw	$3,0($6)

$L484:
	lw	$6,%got($LC14)($28)
	lw	$25,%call16(av_log)($28)
	addiu	$6,$6,%lo($LC14)
	move	$4,$0
	jalr	$25
	move	$5,$0

	lw	$28,16($sp)
	b	$L366
	lw	$8,44($20)

$L405:
	lw	$8,168($sp)
	sll	$2,$12,$fp
	addu	$2,$2,$8
	lw	$9,228($sp)
	lw	$11,196($sp)
	sll	$2,$2,2
	bne	$11,$0,$L418
	addu	$17,$9,$2

	sw	$0,200($sp)
	li	$19,-1			# 0xffffffffffffffff
$L421:
	bltz	$19,$L554
	lw	$25,136($sp)

	lw	$4,1104($20)
	lw	$2,1092($20)
	sll	$21,$25,4
	subu	$2,$2,$4
	addu	$3,$21,$19
	sll	$2,$2,4
	sll	$3,$3,2
	addu	$2,$2,$19
	addu	$3,$3,$20
	sll	$2,$2,2
	lw	$4,768($3)
	addu	$2,$2,$20
	beq	$4,$0,$L424
	lw	$16,432($2)

	lw	$3,200($sp)
	lw	$4,216($sp)
	sll	$2,$3,2
	addu	$2,$2,$4
	lw	$8,48($2)
	bne	$8,$0,$L428
	li	$2,-2147483648			# 0xffffffff80000000

$L430:
	move	$6,$2
	sra	$7,$2,31
$L432:
	addu	$2,$21,$19
	sll	$2,$2,2
	addu	$2,$2,$20
	lw	$3,896($2)
	lw	$11,%got(pow_table)($28)
	lw	$8,148($sp)
	sll	$3,$3,3
	lw	$14,204($sp)
	addu	$3,$3,$11
	lw	$9,0($8)
	lw	$4,0($3)
	lw	$5,4($3)
	blez	$16,$L427
	lw	$8,17776($14)

	mul	$10,$5,$6
	multu	$6,$4
	lw	$25,%call16(__divdi3)($28)
	mflo	$2
	mfhi	$3
	mul	$11,$7,$4
	mult	$9,$8
	addu	$5,$11,$10
	addu	$3,$5,$3
	mfhi	$9
	mflo	$8
	sll	$4,$3,14
	srl	$2,$2,18
	or	$2,$4,$2
	sll	$3,$9,17
	sra	$5,$2,31
	srl	$8,$8,15
	or	$8,$3,$8
	move	$4,$2
	sll	$5,$5,16
	srl	$2,$2,16
	sll	$4,$4,16
	or	$5,$2,$5
	move	$6,$8
	jalr	$25
	sra	$7,$8,31

	lw	$4,176($sp)
	li	$14,131072			# 0x20000
	mult	$4,$2
	lw	$28,16($sp)
	mflo	$2
	mfhi	$3
	srl	$2,$2,16
	sll	$4,$3,16
	or	$2,$4,$2
	move	$9,$18
	move	$12,$0
	move	$10,$2
	sra	$11,$2,31
	addu	$13,$20,$14
$L435:
	lw	$8,17772($13)
	sll	$2,$12,$fp
	addiu	$3,$8,1
	sra	$2,$2,$22
	andi	$3,$3,0x1fff
	sll	$2,$2,2
	sw	$3,17772($13)
	addu	$2,$2,$17
	lw	$3,0($2)
	sll	$8,$8,2
	sra	$7,$3,31
	mul	$5,$7,$10
	multu	$10,$3
	move	$6,$3
	mflo	$2
	mfhi	$3
	mul	$4,$11,$6
	addu	$8,$8,$20
	addu	$6,$4,$5
	addu	$8,$8,$14
	addu	$3,$6,$3
	lw	$4,-14996($8)
	srl	$2,$2,15
	sll	$8,$3,17
	or	$2,$8,$2
	mult	$4,$2
	addiu	$12,$12,1
	mflo	$4
	mfhi	$5
	srl	$4,$4,16
	sll	$2,$5,16
	or	$4,$2,$4
	sw	$4,0($9)
	bne	$12,$16,$L435
	addiu	$9,$9,4

	sll	$2,$16,2
	addu	$18,$18,$2
$L427:
	sll	$2,$16,$fp
	lw	$3,196($sp)
	sll	$2,$2,2
	addiu	$19,$19,1
	addu	$17,$17,$2
	slt	$2,$19,$3
	bne	$2,$0,$L421
	nop

$L418:
	lw	$4,212($sp)
	li	$2,-1			# 0xffffffffffffffff
	sll	$2,$2,$fp
	addu	$3,$4,$20
	sra	$2,$2,$22
	lw	$4,392($3)
	lw	$9,1116($20)
	sll	$2,$2,2
	addu	$2,$2,$17
	subu	$11,$9,$4
	blez	$11,$L480
	lw	$2,0($2)

	lw	$4,192($sp)
	li	$12,131072			# 0x20000
	mult	$4,$2
	move	$10,$0
	mflo	$2
	mfhi	$3
	srl	$2,$2,15
	sll	$4,$3,17
	or	$2,$4,$2
	addu	$7,$20,$12
	move	$8,$2
	sra	$9,$2,31
$L441:
	lw	$2,17772($7)
	addiu	$10,$10,1
	sll	$2,$2,2
	addu	$2,$2,$20
	addu	$2,$2,$12
	lw	$3,-14996($2)
	sra	$5,$3,31
	mul	$13,$5,$8
	multu	$8,$3
	move	$4,$3
	mflo	$2
	mfhi	$3
	mul	$6,$9,$4
	srl	$2,$2,18
	addu	$4,$6,$13
	addu	$3,$4,$3
	sll	$6,$3,14
	or	$2,$6,$2
	sw	$2,0($18)
	lw	$4,17772($7)
	addiu	$18,$18,4
	addiu	$4,$4,1
	andi	$4,$4,0x1fff
	bne	$11,$10,$L441
	sw	$4,17772($7)

$L480:
	lw	$8,44($20)
$L393:
	lw	$4,136($sp)
$L565:
	lw	$5,148($sp)
	lw	$6,144($sp)
	addiu	$4,$4,1
	addiu	$5,$5,4
	addiu	$6,$6,4
	slt	$2,$4,$8
	sw	$4,136($sp)
	sw	$5,148($sp)
	bne	$2,$0,$L392
	sw	$6,144($sp)

	lw	$9,1116($20)
$L390:
	lbu	$2,1128($20)
	beq	$2,$0,$L449
	nop

	lbu	$2,1130($20)
	beq	$2,$0,$L449
	nop

	lbu	$2,1129($20)
	beq	$2,$0,$L555
	lw	$25,%call16(memset)($28)

	blez	$9,$L449
	nop

$L559:
	addiu	$6,$20,25744
	move	$7,$0
$L455:
	lw	$2,0($6)
	lw	$3,8192($6)
	addiu	$7,$7,1
	subu	$4,$2,$3
	slt	$5,$7,$9
	addu	$2,$2,$3
	sw	$2,0($6)
	sw	$4,8192($6)
	bne	$5,$0,$L455
	addiu	$6,$6,4

$L449:
	blez	$8,$L256
	lw	$11,%got($LC18)($28)

	lw	$7,212($sp)
	sll	$2,$fp,5
	addiu	$11,$11,%lo($LC18)
	addu	$2,$7,$2
	li	$3,58512			# 0xe490
	lw	$23,0($11)
	addu	$19,$2,$3
	move	$18,$0
	addiu	$16,$20,25744
	li	$22,42128			# 0xa490
	b	$L457
	li	$21,58720			# 0xe560

$L458:
	addiu	$18,$18,1
$L560:
	slt	$2,$18,$8
	beq	$2,$0,$L256
	addiu	$16,$16,8192

$L457:
	srl	$2,$9,31
	addu	$2,$2,$9
	addu	$3,$18,$20
	sra	$17,$2,1
	lbu	$2,1129($3)
	lw	$25,%call16(ff_imdct_calc_fixed)($28)
	move	$6,$16
	addu	$7,$20,$21
	addu	$4,$20,$19
	beq	$2,$0,$L458
	addu	$5,$20,$22

	jalr	$25
	nop

	lw	$3,1088($20)
	lw	$4,1124($20)
	srl	$2,$3,31
	addu	$2,$2,$3
	sra	$2,$2,1
	addu	$2,$2,$4
	subu	$17,$2,$17
	sll	$5,$18,12
	lw	$14,232($sp)
	addu	$5,$5,$17
	sll	$5,$5,2
	addu	$5,$14,$5
	move	$25,$23
	jalr	$25
	move	$4,$20

	lbu	$2,1128($20)
	beq	$2,$0,$L482
	lw	$28,16($sp)

	lw	$2,232($sp)
	sll	$5,$17,2
	addu	$5,$2,$5
	lbu	$2,1130($20)
	addiu	$5,$5,16384
	beq	$2,$0,$L556
	move	$4,$20

$L482:
	lw	$8,44($20)
	b	$L458
	lw	$9,1116($20)

$L556:
	move	$25,$23
	jalr	$25
	addiu	$18,$18,1

	lw	$28,16($sp)
	lw	$8,44($20)
	b	$L560
	lw	$9,1116($20)

$L395:
	lw	$5,172($sp)
	lw	$6,%got(pow_table)($28)
	sll	$2,$5,3
	addu	$2,$2,$6
	lw	$7,148($sp)
	lw	$6,0($2)
	lw	$3,4($2)
	lw	$8,0($7)
	srl	$2,$6,17
	lw	$25,%call16(__divdi3)($28)
	sll	$5,$3,15
	sll	$4,$6,15
	sra	$7,$8,31
	move	$6,$8
	jalr	$25
	or	$5,$2,$5

	lw	$6,176($sp)
	lw	$28,16($sp)
	sra	$7,$6,31
	mul	$9,$7,$2
	multu	$2,$6
	mflo	$4
	mfhi	$5
	mul	$8,$3,$6
	srl	$4,$4,16
	addu	$6,$8,$9
	addu	$5,$6,$5
	sll	$2,$5,16
	lw	$3,72($20)
	or	$4,$2,$4
	addiu	$2,$17,25744
	sw	$4,192($sp)
	bne	$3,$0,$L557
	addu	$18,$20,$2

$L398:
	lw	$2,388($20)
	blez	$2,$L403
	move	$3,$0

$L442:
	sw	$0,0($18)
	lw	$2,388($20)
	addiu	$3,$3,1
	slt	$2,$3,$2
	bne	$2,$0,$L442
	addiu	$18,$18,4

$L403:
	lw	$7,144($sp)
	lw	$13,0($7)
	blez	$13,$L570
	lw	$3,212($sp)

	lw	$8,192($sp)
	lw	$11,188($sp)
	sra	$9,$8,31
	move	$10,$18
	move	$12,$0
$L445:
	sll	$2,$12,$fp
	sra	$2,$2,$22
	sll	$2,$2,2
	addu	$2,$2,$16
	lw	$3,0($2)
	lh	$6,0($11)
	sra	$5,$3,31
	mul	$14,$5,$8
	multu	$8,$3
	move	$4,$3
	mflo	$2
	mfhi	$3
	mul	$7,$9,$4
	srl	$2,$2,15
	addu	$4,$7,$14
	addu	$3,$4,$3
	sll	$4,$3,17
	sll	$6,$6,16
	or	$2,$4,$2
	mult	$6,$2
	addiu	$12,$12,1
	mflo	$6
	mfhi	$7
	srl	$6,$6,18
	sll	$2,$7,14
	or	$6,$2,$6
	sw	$6,0($10)
	addiu	$11,$11,2
	bne	$13,$12,$L445
	addiu	$10,$10,4

	sll	$2,$13,2
	addu	$18,$18,$2
	lw	$3,212($sp)
$L570:
	lw	$9,1116($20)
	addu	$2,$3,$20
	lw	$3,392($2)
	subu	$2,$9,$3
	blez	$2,$L480
	move	$4,$0

$L448:
	addiu	$4,$4,1
	sw	$0,0($18)
	bne	$2,$4,$L448
	addiu	$18,$18,4

	b	$L393
	lw	$8,44($20)

$L416:
	lw	$8,168($sp)
	sll	$2,$12,$fp
	addu	$2,$2,$8
	lw	$9,228($sp)
	sll	$2,$2,2
	addu	$17,$9,$2
	b	$L421
	li	$19,-1			# 0xffffffffffffffff

$L554:
	lw	$14,212($sp)
	lw	$3,388($20)
	addu	$2,$14,$20
	lw	$4,368($2)
	subu	$16,$4,$3
$L424:
	blez	$16,$L427
	li	$21,131072			# 0x20000

	lw	$10,192($sp)
	lw	$13,188($sp)
	sra	$11,$10,31
	move	$12,$18
	move	$14,$0
	addu	$15,$20,$21
$L438:
	lw	$9,17772($15)
	sll	$2,$14,$fp
	addiu	$3,$9,1
	sra	$2,$2,$22
	andi	$3,$3,0x1fff
	sll	$2,$2,2
	sw	$3,17772($15)
	addu	$2,$2,$17
	lw	$3,0($2)
	sll	$9,$9,2
	sra	$7,$3,31
	mul	$5,$7,$10
	multu	$10,$3
	move	$6,$3
	mflo	$2
	mfhi	$3
	mul	$4,$11,$6
	addu	$9,$9,$20
	addu	$6,$4,$5
	lh	$8,0($13)
	addu	$9,$9,$21
	lw	$4,-14996($9)
	addu	$3,$6,$3
	sll	$8,$8,16
	sll	$6,$3,17
	srl	$2,$2,15
	addu	$4,$4,$8
	or	$2,$6,$2
	mult	$4,$2
	addiu	$14,$14,1
	mflo	$4
	mfhi	$5
	srl	$4,$4,18
	sll	$2,$5,14
	or	$4,$2,$4
	sw	$4,0($12)
	addiu	$13,$13,2
	bne	$14,$16,$L438
	addiu	$12,$12,4

	sll	$2,$16,2
	addu	$18,$18,$2
	lw	$2,188($sp)
	sll	$3,$16,1
	addu	$2,$2,$3
	b	$L427
	sw	$2,188($sp)

$L428:
	lw	$5,216($sp)
	sll	$2,$19,2
	addu	$2,$2,$5
	lw	$3,48($2)
	lw	$25,%call16(__divdi3)($28)
	sra	$5,$3,31
	srl	$2,$3,16
	sll	$5,$5,16
	move	$6,$8
	sra	$7,$8,31
	or	$5,$2,$5
	jalr	$25
	sll	$4,$3,16

	lw	$28,16($sp)
	lw	$25,%call16(sqrtint)($28)
	jalr	$25
	move	$4,$2

	bne	$2,$0,$L430
	lw	$28,16($sp)

	li	$6,2147418112			# 0x7fff0000
	ori	$6,$6,0xfff8
	b	$L432
	move	$7,$0

$L540:
	lw	$6,8($18)
	lw	$13,0($18)
	sra	$4,$6,3
	addu	$4,$4,$13
	lbu	$3,0($4)
	lbu	$5,1($4)
	lbu	$7,3($4)
	lbu	$2,2($4)
	sll	$3,$3,24
	sll	$5,$5,16
	or	$3,$3,$5
	or	$3,$3,$7
	sll	$2,$2,8
	or	$2,$2,$3
	andi	$4,$6,0x7
	sll	$2,$2,$4
	srl	$2,$2,27
	addiu	$12,$6,5
	addiu	$19,$2,10
	bltz	$19,$L558
	sw	$12,8($18)

	slt	$2,$19,78
	beq	$2,$0,$L306
	lw	$3,%got(tablefix16_10exp1div16)($28)

	sll	$2,$19,2
	addu	$2,$2,$3
	lw	$21,0($2)
$L305:
	lhu	$2,0($22)
$L308:
	addiu	$2,$2,-1
	sw	$21,0($17)
	bne	$2,$0,$L308
	addiu	$17,$17,4

	b	$L310
	addiu	$22,$22,2

$L545:
	move	$4,$0
	b	$L412
	move	$5,$0

$L546:
	b	$L334
	move	$2,$0

$L306:
	lw	$4,%got($LC12)($28)
	lw	$2,%got(tablefix16_10exp1div16)($28)
	lw	$25,%call16(printf)($28)
	addiu	$4,$4,%lo($LC12)
	move	$5,$19
	jalr	$25
	lw	$21,308($2)

	b	$L305
	lw	$28,16($sp)

$L555:
	sll	$6,$9,2
	addiu	$4,$20,25744
	jalr	$25
	move	$5,$0

	lw	$9,1116($20)
	li	$2,1
	lw	$28,16($sp)
	lw	$8,44($20)
	bgtz	$9,$L559
	sb	$2,1129($20)

	b	$L449
	nop

$L558:
	b	$L305
	move	$21,$0

$L544:
	cvt.d.w	$f2,$f6
	sqrt.d	$f2,$f2
	cvt.d.s	$f0,$f4
	mul.d	$f0,$f0,$f2
	b	$L388
	cvt.s.d	$f4,$f0

$L471:
	b	$L472
	move	$2,$0

	.set	macro
	.set	reorder
	.end	wma_decode_frame
	.align	2
	.ent	wma_decode_superframe
	.type	wma_decode_superframe, @function
wma_decode_superframe:
	.frame	$sp,64,$31		# vars= 0, regs= 10/0, args= 16, gp= 8
	.mask	0xc0ff0000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$fp,56($sp)
	sw	$23,52($sp)
	sw	$22,48($sp)
	sw	$21,44($sp)
	sw	$20,40($sp)
	sw	$19,36($sp)
	sw	$18,32($sp)
	sw	$17,28($sp)
	sw	$16,24($sp)
	.cprestore	16
	lw	$23,80($sp)
	move	$fp,$5
	sw	$6,72($sp)
	sw	$7,76($sp)
	bne	$23,$0,$L573
	lw	$18,136($4)

	li	$2,131072			# 0x20000
	addu	$2,$18,$2
	sw	$0,-15000($2)
	move	$4,$0
	lw	$31,60($sp)
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	move	$2,$4
	j	$31
	addiu	$sp,$sp,64

$L573:
	sll	$2,$23,3
	sra	$3,$2,3
	move	$20,$5
	bltz	$3,$L576
	addiu	$17,$18,4

	bltz	$2,$L576
	lw	$5,76($sp)

	sw	$2,12($17)
	sw	$0,8($17)
	lw	$2,60($18)
	addu	$4,$5,$3
	move	$3,$5
	sw	$4,4($17)
	beq	$2,$0,$L580
	sw	$3,0($17)

$L613:
	li	$2,4			# 0x4
	sw	$2,8($17)
	li	$2,8			# 0x8
	addiu	$6,$3,1
	lbu	$16,0($3)
	sw	$2,8($17)
	lbu	$3,1($3)
	lbu	$2,1($6)
	lw	$8,76($18)
	sll	$2,$2,16
	sll	$3,$3,24
	lbu	$7,3($6)
	addiu	$5,$8,11
	lbu	$4,2($6)
	or	$3,$3,$2
	li	$2,131072			# 0x20000
	addu	$2,$18,$2
	sw	$5,8($17)
	or	$3,$3,$7
	sll	$4,$4,8
	lw	$5,-15000($2)
	li	$2,29			# 0x1d
	or	$4,$4,$3
	subu	$2,$2,$8
	blez	$5,$L582
	srl	$22,$4,$2

	addiu	$2,$22,7
	sra	$2,$2,3
	addu	$2,$5,$2
	slt	$2,$2,16385
	beq	$2,$0,$L614
	li	$2,131072			# 0x20000

	li	$2,65536			# 0x10000
	ori	$2,$2,0x8560
	addu	$10,$18,$2
	slt	$3,$22,8
	bne	$3,$0,$L611
	addu	$9,$10,$5

	move	$8,$22
$L589:
	lw	$6,8($17)
	lw	$3,0($17)
	sra	$4,$6,3
	addu	$4,$4,$3
	lbu	$2,0($4)
	lbu	$5,1($4)
	lbu	$7,3($4)
	lbu	$3,2($4)
	sll	$2,$2,24
	sll	$5,$5,16
	or	$2,$2,$5
	or	$2,$2,$7
	sll	$3,$3,8
	or	$3,$3,$2
	andi	$4,$6,0x7
	sll	$3,$3,$4
	addiu	$8,$8,-8
	srl	$3,$3,24
	addiu	$6,$6,8
	slt	$2,$8,8
	sw	$6,8($17)
	sb	$3,0($9)
	beq	$2,$0,$L589
	addiu	$9,$9,1

$L588:
	blez	$8,$L615
	li	$2,131072			# 0x20000

	lw	$6,8($17)
	lw	$3,0($17)
	sra	$4,$6,3
	addu	$4,$4,$3
	lbu	$2,0($4)
	lbu	$5,1($4)
	lbu	$7,3($4)
	lbu	$3,2($4)
	sll	$5,$5,16
	sll	$2,$2,24
	or	$2,$2,$5
	or	$2,$2,$7
	sll	$3,$3,8
	or	$3,$3,$2
	andi	$4,$6,0x7
	sll	$3,$3,$4
	subu	$5,$0,$8
	li	$2,8			# 0x8
	srl	$3,$3,$5
	subu	$2,$2,$8
	sll	$3,$3,$2
	addu	$6,$8,$6
	sw	$6,8($17)
	sb	$3,0($9)
	li	$2,131072			# 0x20000
$L615:
	sw	$2,12($17)
	sw	$0,8($17)
	addu	$2,$18,$2
	lw	$2,-15004($2)
	addiu	$3,$10,16384
	sw	$3,4($17)
	blez	$2,$L592
	sw	$10,0($17)

	sw	$2,8($17)
$L592:
	lw	$2,%got(wma_decode_frame)($28)
	move	$4,$18
	addiu	$25,$2,%lo(wma_decode_frame)
	jalr	$25
	move	$5,$fp

	bltz	$2,$L584
	lw	$28,16($sp)

	lw	$2,44($18)
	lw	$3,1088($18)
	mul	$2,$2,$3
	sll	$2,$2,1
	addu	$20,$fp,$2
$L582:
	lw	$2,76($18)
	addiu	$2,$2,11
	addu	$6,$22,$2
	sra	$3,$6,3
	li	$2,16384			# 0x4000
	subu	$2,$2,$3
	sll	$2,$2,3
	sra	$4,$2,3
	bltz	$4,$L595
	nop

	bltz	$2,$L595
	nop

	lw	$5,76($sp)
	addu	$3,$5,$3
	addu	$5,$4,$3
$L598:
	andi	$4,$6,0x7
	sw	$3,0($17)
	sw	$2,12($17)
	sw	$5,4($17)
	blez	$4,$L599
	sw	$0,8($17)

	sw	$4,8($17)
$L599:
	andi	$19,$16,0xf
	li	$2,1			# 0x1
	addiu	$3,$19,-1
	blez	$3,$L601
	sw	$2,1100($18)

	lw	$2,%got(wma_decode_frame)($28)
	li	$16,1			# 0x1
	b	$L603
	addiu	$21,$2,%lo(wma_decode_frame)

$L612:
	lw	$2,44($18)
	lw	$3,1088($18)
	mul	$2,$2,$3
	sll	$2,$2,1
	beq	$16,$19,$L601
	addu	$20,$20,$2

$L603:
	move	$5,$20
	move	$25,$21
	jalr	$25
	move	$4,$18

	addiu	$16,$16,1
	bgez	$2,$L612
	lw	$28,16($sp)

$L584:
	li	$2,131072			# 0x20000
$L614:
	addu	$2,$18,$2
	sw	$0,-15000($2)
	li	$4,-1			# 0xffffffffffffffff
	lw	$31,60($sp)
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	move	$2,$4
	j	$31
	addiu	$sp,$sp,64

$L576:
	move	$2,$0
	sw	$2,12($17)
	sw	$0,8($17)
	lw	$2,60($18)
	move	$3,$0
	move	$4,$0
	sw	$4,4($17)
	bne	$2,$0,$L613
	sw	$3,0($17)

$L580:
	lw	$25,%got(wma_decode_frame)($28)
	move	$4,$18
	addiu	$25,$25,%lo(wma_decode_frame)
	jalr	$25
	move	$5,$fp

	bltz	$2,$L584
	lw	$28,16($sp)

	lw	$2,44($18)
	lw	$3,1088($18)
	mul	$2,$2,$3
	sll	$2,$2,1
	addu	$20,$fp,$2
$L606:
	lw	$3,72($sp)
	subu	$2,$20,$fp
	sw	$2,0($3)
	lw	$4,56($18)
	lw	$31,60($sp)
	lw	$fp,56($sp)
	lw	$23,52($sp)
	lw	$22,48($sp)
	lw	$21,44($sp)
	lw	$20,40($sp)
	lw	$19,36($sp)
	lw	$18,32($sp)
	lw	$17,28($sp)
	lw	$16,24($sp)
	move	$2,$4
	j	$31
	addiu	$sp,$sp,64

$L601:
	lw	$2,76($18)
	lw	$4,8($17)
	addiu	$2,$2,11
	li	$3,-8			# 0xfffffffffffffff8
	addu	$2,$22,$2
	and	$2,$2,$3
	addu	$2,$2,$4
	sra	$5,$2,3
	li	$3,131072			# 0x20000
	subu	$6,$23,$5
	addu	$4,$18,$3
	andi	$2,$2,0x7
	sltu	$3,$6,16385
	beq	$3,$0,$L584
	sw	$2,-15004($4)

	sw	$6,-15000($4)
	lw	$2,76($sp)
	li	$4,65536			# 0x10000
	lw	$25,%call16(memcpy)($28)
	ori	$4,$4,0x8560
	addu	$5,$2,$5
	jalr	$25
	addu	$4,$18,$4

	b	$L606
	lw	$28,16($sp)

$L595:
	move	$3,$0
	move	$2,$0
	b	$L598
	move	$5,$0

$L611:
	b	$L588
	move	$8,$22

	.set	macro
	.set	reorder
	.end	wma_decode_superframe
	.globl	tablefix16_10exp1div16
	.data
	.align	2
	.type	tablefix16_10exp1div16, @object
	.size	tablefix16_10exp1div16, 312
tablefix16_10exp1div16:
	.word	32768
	.word	37839
	.word	43696
	.word	50460
	.word	58270
	.word	67289
	.word	77705
	.word	89732
	.word	103621
	.word	119660
	.word	138181
	.word	159569
	.word	184268
	.word	212789
	.word	245725
	.word	283759
	.word	327680
	.word	378398
	.word	436968
	.word	504603
	.word	582706
	.word	672899
	.word	777051
	.word	897325
	.word	1036215
	.word	1196602
	.word	1381815
	.word	1595695
	.word	1842680
	.word	2127893
	.word	2457253
	.word	2837591
	.word	3276800
	.word	3783989
	.word	4369683
	.word	5046031
	.word	5827065
	.word	6728990
	.word	7770517
	.word	8973253
	.word	10362151
	.word	11966025
	.word	13818151
	.word	15956951
	.word	18426800
	.word	21278937
	.word	24572533
	.word	28375918
	.word	32768000
	.word	37839896
	.word	43696830
	.word	50460312
	.word	58270659
	.word	67289908
	.word	77705173
	.word	89732534
	.word	103621514
	.word	119660258
	.word	138181510
	.word	159569518
	.word	184268005
	.word	212789373
	.word	245725334
	.word	283759189
	.word	327680000
	.word	378398960
	.word	436968302
	.word	504603124
	.word	582706597
	.word	672899080
	.word	777051735
	.word	897325345
	.word	1036215143
	.word	1196602580
	.word	1381815102
	.word	1595695186
	.word	1842680054
	.word	2127893730
	.globl	pow_table
	.align	3
	.type	pow_table, @object
	.size	pow_table, 1600
pow_table:
	.word	65536
	.word	0
	.word	73533
	.word	0
	.word	82505
	.word	0
	.word	92572
	.word	0
	.word	103868
	.word	0
	.word	116541
	.word	0
	.word	130761
	.word	0
	.word	146717
	.word	0
	.word	164619
	.word	0
	.word	184706
	.word	0
	.word	207243
	.word	0
	.word	232531
	.word	0
	.word	260904
	.word	0
	.word	292739
	.word	0
	.word	328458
	.word	0
	.word	368536
	.word	0
	.word	413504
	.word	0
	.word	463959
	.word	0
	.word	520571
	.word	0
	.word	584090
	.word	0
	.word	655360
	.word	0
	.word	735326
	.word	0
	.word	825049
	.word	0
	.word	925721
	.word	0
	.word	1038676
	.word	0
	.word	1165413
	.word	0
	.word	1307615
	.word	0
	.word	1467168
	.word	0
	.word	1646190
	.word	0
	.word	1847056
	.word	0
	.word	2072430
	.word	0
	.word	2325305
	.word	0
	.word	2609035
	.word	0
	.word	2927386
	.word	0
	.word	3284581
	.word	0
	.word	3685360
	.word	0
	.word	4135042
	.word	0
	.word	4639594
	.word	0
	.word	5205710
	.word	0
	.word	5840902
	.word	0
	.word	6553600
	.word	0
	.word	7353260
	.word	0
	.word	8250494
	.word	0
	.word	9257206
	.word	0
	.word	10386756
	.word	0
	.word	11654132
	.word	0
	.word	13076151
	.word	0
	.word	14671683
	.word	0
	.word	16461899
	.word	0
	.word	18470554
	.word	0
	.word	20724302
	.word	0
	.word	23253050
	.word	0
	.word	26090352
	.word	0
	.word	29273856
	.word	0
	.word	32845806
	.word	0
	.word	36853600
	.word	0
	.word	41350420
	.word	0
	.word	46395936
	.word	0
	.word	52057096
	.word	0
	.word	58409020
	.word	0
	.word	65536000
	.word	0
	.word	73532600
	.word	0
	.word	82504936
	.word	0
	.word	92572064
	.word	0
	.word	103867560
	.word	0
	.word	116541320
	.word	0
	.word	130761512
	.word	0
	.word	146716832
	.word	0
	.word	164618992
	.word	0
	.word	184705536
	.word	0
	.word	207243024
	.word	0
	.word	232530496
	.word	0
	.word	260903520
	.word	0
	.word	292738560
	.word	0
	.word	328458080
	.word	0
	.word	368536000
	.word	0
	.word	413504192
	.word	0
	.word	463959360
	.word	0
	.word	520570944
	.word	0
	.word	584090240
	.word	0
	.word	655360000
	.word	0
	.word	735326016
	.word	0
	.word	825049344
	.word	0
	.word	925720576
	.word	0
	.word	1038675584
	.word	0
	.word	1165413248
	.word	0
	.word	1307615104
	.word	0
	.word	1467168256
	.word	0
	.word	1646189952
	.word	0
	.word	1847055488
	.word	0
	.word	2072430336
	.word	0
	.word	-1969662208
	.word	0
	.word	-1685932032
	.word	0
	.word	-1367581696
	.word	0
	.word	-1010386688
	.word	0
	.word	-609607168
	.word	0
	.word	-159925248
	.word	0
	.word	344626176
	.word	1
	.word	910742016
	.word	1
	.word	1545934848
	.word	1
	.word	-2036334592
	.word	1
	.word	-1236674560
	.word	1
	.word	-339441152
	.word	1
	.word	667271168
	.word	2
	.word	1796820992
	.word	2
	.word	-1230770176
	.word	2
	.word	191249408
	.word	3
	.word	1786780672
	.word	3
	.word	-717970432
	.word	3
	.word	1290684416
	.word	4
	.word	-750532608
	.word	4
	.word	1778212864
	.word	5
	.word	320546816
	.word	6
	.word	-790915072
	.word	6
	.word	-1513932800
	.word	7
	.word	-1801105408
	.word	8
	.word	-1599250432
	.word	9
	.word	-848703488
	.word	10
	.word	517488640
	.word	12
	.word	-1720520704
	.word	13
	.word	1111490560
	.word	15
	.word	518160384
	.word	17
	.word	900554752
	.word	19
	.word	-1917222912
	.word	21
	.word	788348928
	.word	24
	.word	577200128
	.word	27
	.word	1912496128
	.word	30
	.word	687947776
	.word	34
	.word	1410236416
	.word	38
	.word	21954560
	.word	43
	.word	1084604416
	.word	48
	.word	602275840
	.word	54
	.word	-1089486848
	.word	60
	.word	680787968
	.word	68
	.word	2040561664
	.word	76
	.word	-831160320
	.word	85
	.word	1187348480
	.word	96
	.word	102891520
	.word	108
	.word	879919104
	.word	121
	.word	-25362432
	.word	135
	.word	-1769996288
	.word	152
	.word	886636544
	.word	171
	.word	415629312
	.word	192
	.word	-1992359936
	.word	215
	.word	-706478080
	.word	241
	.word	1477050368
	.word	271
	.word	1945108480
	.word	304
	.word	-1710489600
	.word	341
	.word	1217396736
	.word	383
	.word	219545600
	.word	430
	.word	-2038956032
	.word	482
	.word	1727791104
	.word	541
	.word	1989935104
	.word	607
	.word	-1782054912
	.word	681
	.word	-1069285376
	.word	764
	.word	278134784
	.word	858
	.word	-1011351552
	.word	962
	.word	1028653056
	.word	1080
	.word	209190912
	.word	1212
	.word	-253231104
	.word	1359
	.word	-520093696
	.word	1525
	.word	276299776
	.word	1712
	.word	-138412032
	.word	1920
	.word	1551892480
	.word	2155
	.word	1524629504
	.word	2418
	.word	1885339648
	.word	2713
	.word	-2024800256
	.word	3044
	.word	74448896
	.word	3416
	.word	-710934528
	.word	3832
	.word	-2099249152
	.word	4300
	.word	1086324736
	.word	4825
	.word	96468992
	.word	5414
	.word	-1574961152
	.word	6074
	.word	-641728512
	.word	6815
	.word	-2103443456
	.word	7647
	.word	-1514143744
	.word	8580
	.word	-1522532352
	.word	9627
	.word	1698693120
	.word	10802
	.word	2092957696
	.word	12120
	.word	1761607680
	.word	13599
	.word	-905969664
	.word	15258
	.word	-1535115264
	.word	17120
	.word	-1384120320
	.word	19209
	.word	-1660944384
	.word	21553
	.word	-1929379840
	.word	24183
	.word	1677721600
	.word	27134
	.word	1233125376
	.word	30445
	.word	738197504
	.word	34160
	.word	1476395008
	.word	38328
	.word	469762048
	.word	43005
	.word	-2030043136
	.word	48252
	.word	973078528
	.word	54140
	.word	1426063360
	.word	60746
	.word	-2113929216
	.word	68158
	.word	436207616
	.word	76475
	.word	2046820352
	.word	85806
	.word	1946157056
	.word	96276
	.word	-201326592
	.word	108023
	.word	-570425344
	.word	121204
	.word	402653184
	.word	135994
	.globl	pow_m1_4diff
	.align	2
	.type	pow_m1_4diff, @object
	.size	pow_m1_4diff, 2048
pow_m1_4diff:
	.byte	0
	.byte	-128
	.byte	-88
	.byte	-21
	.byte	-15
	.byte	-51
	.byte	-9
	.byte	4
	.byte	-11
	.byte	3
	.byte	-26
	.byte	3
	.byte	-1
	.byte	-6
	.byte	32
	.byte	-25
	.byte	-32
	.byte	25
	.byte	-18
	.byte	-7
	.byte	-27
	.byte	13
	.byte	-19
	.byte	-26
	.byte	-22
	.byte	-22
	.byte	17
	.byte	-18
	.byte	18
	.byte	16
	.byte	21
	.byte	-21
	.byte	-21
	.byte	13
	.byte	-11
	.byte	-8
	.byte	17
	.byte	18
	.byte	-6
	.byte	-16
	.byte	-15
	.byte	-7
	.byte	8
	.byte	-13
	.byte	8
	.byte	-8
	.byte	13
	.byte	-6
	.byte	9
	.byte	-18
	.byte	-13
	.byte	-15
	.byte	11
	.byte	-7
	.byte	1
	.byte	0
	.byte	-13
	.byte	-2
	.byte	-3
	.byte	17
	.byte	-10
	.byte	16
	.byte	-4
	.byte	-7
	.byte	9
	.byte	9
	.byte	-6
	.byte	-5
	.byte	11
	.byte	11
	.byte	-5
	.byte	-9
	.byte	2
	.byte	-5
	.byte	2
	.byte	-10
	.byte	-8
	.byte	4
	.byte	-2
	.byte	3
	.byte	-10
	.byte	-14
	.byte	-6
	.byte	10
	.byte	8
	.byte	-12
	.byte	3
	.byte	1
	.byte	6
	.byte	-7
	.byte	-12
	.byte	-9
	.byte	2
	.byte	-7
	.byte	-9
	.byte	-4
	.byte	9
	.byte	2
	.byte	2
	.byte	9
	.byte	-4
	.byte	-11
	.byte	-12
	.byte	-6
	.byte	5
	.byte	-3
	.byte	-5
	.byte	-3
	.byte	5
	.byte	-7
	.byte	12
	.byte	11
	.byte	-10
	.byte	-1
	.byte	12
	.byte	6
	.byte	5
	.byte	8
	.byte	-9
	.byte	3
	.byte	-5
	.byte	-8
	.byte	-8
	.byte	-4
	.byte	5
	.byte	-7
	.byte	9
	.byte	6
	.byte	7
	.byte	10
	.byte	-5
	.byte	5
	.byte	-2
	.byte	-8
	.byte	-9
	.byte	-8
	.byte	-3
	.byte	5
	.byte	-6
	.byte	8
	.byte	3
	.byte	1
	.byte	2
	.byte	6
	.byte	-9
	.byte	0
	.byte	-9
	.byte	4
	.byte	1
	.byte	-1
	.byte	0
	.byte	4
	.byte	10
	.byte	-2
	.byte	9
	.byte	0
	.byte	-4
	.byte	-8
	.byte	-9
	.byte	-7
	.byte	-3
	.byte	3
	.byte	-9
	.byte	1
	.byte	-7
	.byte	7
	.byte	4
	.byte	2
	.byte	1
	.byte	4
	.byte	8
	.byte	-6
	.byte	1
	.byte	-8
	.byte	3
	.byte	-4
	.byte	-8
	.byte	8
	.byte	8
	.byte	8
	.byte	-9
	.byte	-5
	.byte	0
	.byte	7
	.byte	-3
	.byte	8
	.byte	1
	.byte	-5
	.byte	-8
	.byte	7
	.byte	6
	.byte	7
	.byte	9
	.byte	-6
	.byte	-1
	.byte	5
	.byte	-5
	.byte	3
	.byte	-5
	.byte	8
	.byte	2
	.byte	-3
	.byte	-5
	.byte	-6
	.byte	-6
	.byte	-5
	.byte	-3
	.byte	1
	.byte	5
	.byte	-6
	.byte	1
	.byte	-8
	.byte	2
	.byte	-6
	.byte	6
	.byte	1
	.byte	-3
	.byte	-5
	.byte	-6
	.byte	-6
	.byte	-5
	.byte	-4
	.byte	-1
	.byte	3
	.byte	8
	.byte	-4
	.byte	3
	.byte	-6
	.byte	3
	.byte	-4
	.byte	7
	.byte	2
	.byte	-3
	.byte	-6
	.byte	8
	.byte	7
	.byte	6
	.byte	6
	.byte	8
	.byte	-7
	.byte	-4
	.byte	0
	.byte	5
	.byte	-6
	.byte	0
	.byte	7
	.byte	-2
	.byte	8
	.byte	1
	.byte	-5
	.byte	6
	.byte	2
	.byte	-2
	.byte	-4
	.byte	-6
	.byte	-7
	.byte	-8
	.byte	-7
	.byte	-6
	.byte	-4
	.byte	-1
	.byte	1
	.byte	6
	.byte	-5
	.byte	0
	.byte	6
	.byte	-3
	.byte	4
	.byte	-3
	.byte	6
	.byte	0
	.byte	-5
	.byte	5
	.byte	1
	.byte	-3
	.byte	-5
	.byte	8
	.byte	6
	.byte	5
	.byte	5
	.byte	5
	.byte	6
	.byte	8
	.byte	-6
	.byte	-3
	.byte	1
	.byte	4
	.byte	-6
	.byte	-1
	.byte	4
	.byte	-4
	.byte	2
	.byte	-6
	.byte	3
	.byte	-4
	.byte	5
	.byte	-1
	.byte	-6
	.byte	5
	.byte	0
	.byte	-2
	.byte	-5
	.byte	7
	.byte	5
	.byte	4
	.byte	4
	.byte	4
	.byte	3
	.byte	5
	.byte	6
	.byte	7
	.byte	-5
	.byte	-2
	.byte	1
	.byte	5
	.byte	-5
	.byte	-1
	.byte	4
	.byte	-5
	.byte	2
	.byte	-7
	.byte	0
	.byte	-6
	.byte	2
	.byte	-4
	.byte	4
	.byte	-1
	.byte	-6
	.byte	4
	.byte	0
	.byte	-3
	.byte	-6
	.byte	5
	.byte	4
	.byte	2
	.byte	0
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	0
	.byte	1
	.byte	3
	.byte	5
	.byte	7
	.byte	-4
	.byte	-1
	.byte	2
	.byte	6
	.byte	-3
	.byte	1
	.byte	6
	.byte	-2
	.byte	3
	.byte	-5
	.byte	2
	.byte	-5
	.byte	2
	.byte	-4
	.byte	3
	.byte	-2
	.byte	7
	.byte	2
	.byte	-2
	.byte	7
	.byte	3
	.byte	0
	.byte	-3
	.byte	-5
	.byte	5
	.byte	3
	.byte	1
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	1
	.byte	3
	.byte	4
	.byte	6
	.byte	-4
	.byte	-2
	.byte	1
	.byte	4
	.byte	-5
	.byte	-1
	.byte	3
	.byte	-6
	.byte	-1
	.byte	5
	.byte	-4
	.byte	2
	.byte	-4
	.byte	1
	.byte	-5
	.byte	2
	.byte	-4
	.byte	3
	.byte	-2
	.byte	5
	.byte	0
	.byte	-4
	.byte	5
	.byte	1
	.byte	-3
	.byte	6
	.byte	3
	.byte	0
	.byte	-3
	.byte	-5
	.byte	6
	.byte	4
	.byte	2
	.byte	1
	.byte	0
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	0
	.byte	1
	.byte	2
	.byte	4
	.byte	6
	.byte	-4
	.byte	-2
	.byte	1
	.byte	3
	.byte	6
	.byte	-3
	.byte	0
	.byte	4
	.byte	-5
	.byte	0
	.byte	4
	.byte	-3
	.byte	0
	.byte	6
	.byte	-1
	.byte	4
	.byte	-3
	.byte	3
	.byte	-3
	.byte	3
	.byte	-2
	.byte	4
	.byte	-2
	.byte	6
	.byte	1
	.byte	-3
	.byte	4
	.byte	0
	.byte	-4
	.byte	4
	.byte	0
	.byte	-3
	.byte	6
	.byte	2
	.byte	0
	.byte	-3
	.byte	-6
	.byte	5
	.byte	3
	.byte	1
	.byte	-1
	.byte	-2
	.byte	-3
	.byte	-5
	.byte	7
	.byte	6
	.byte	5
	.byte	4
	.byte	5
	.byte	4
	.byte	5
	.byte	6
	.byte	6
	.byte	-5
	.byte	-4
	.byte	-2
	.byte	-1
	.byte	1
	.byte	2
	.byte	4
	.byte	6
	.byte	-3
	.byte	-1
	.byte	1
	.byte	4
	.byte	-4
	.byte	-1
	.byte	2
	.byte	5
	.byte	-3
	.byte	1
	.byte	5
	.byte	-3
	.byte	2
	.byte	6
	.byte	-1
	.byte	3
	.byte	-3
	.byte	2
	.byte	-4
	.byte	1
	.byte	-5
	.byte	0
	.byte	-5
	.byte	1
	.byte	-4
	.byte	2
	.byte	-3
	.byte	4
	.byte	-1
	.byte	6
	.byte	2
	.byte	-2
	.byte	5
	.byte	0
	.byte	-3
	.byte	4
	.byte	2
	.byte	-2
	.byte	6
	.byte	3
	.byte	0
	.byte	-2
	.byte	-4
	.byte	4
	.byte	2
	.byte	-1
	.byte	-2
	.byte	-4
	.byte	5
	.byte	4
	.byte	2
	.byte	1
	.byte	0
	.byte	0
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-3
	.byte	-3
	.byte	-4
	.byte	-4
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	1
	.byte	1
	.byte	3
	.byte	4
	.byte	6
	.byte	-4
	.byte	-2
	.byte	0
	.byte	2
	.byte	4
	.byte	-5
	.byte	-2
	.byte	0
	.byte	3
	.byte	5
	.byte	-2
	.byte	1
	.byte	4
	.byte	-4
	.byte	-1
	.byte	3
	.byte	-4
	.byte	-1
	.byte	3
	.byte	-4
	.byte	0
	.byte	5
	.byte	-2
	.byte	2
	.byte	-4
	.byte	1
	.byte	6
	.byte	-1
	.byte	5
	.byte	-1
	.byte	3
	.byte	-2
	.byte	4
	.byte	-1
	.byte	4
	.byte	0
	.byte	5
	.byte	0
	.byte	-4
	.byte	2
	.byte	-2
	.byte	4
	.byte	0
	.byte	-4
	.byte	3
	.byte	-1
	.byte	-4
	.byte	2
	.byte	-1
	.byte	-5
	.byte	2
	.byte	0
	.byte	-3
	.byte	4
	.byte	2
	.byte	-1
	.byte	-4
	.byte	4
	.byte	2
	.byte	-1
	.byte	-3
	.byte	-5
	.byte	3
	.byte	2
	.byte	0
	.byte	-1
	.byte	-3
	.byte	-4
	.byte	4
	.byte	3
	.byte	2
	.byte	2
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-4
	.byte	-4
	.byte	-3
	.byte	-4
	.byte	-3
	.byte	-3
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	0
	.byte	1
	.byte	1
	.byte	3
	.byte	4
	.byte	5
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	1
	.byte	3
	.byte	5
	.byte	-4
	.byte	-2
	.byte	1
	.byte	3
	.byte	5
	.byte	-3
	.byte	-1
	.byte	1
	.byte	4
	.byte	-3
	.byte	0
	.byte	3
	.byte	5
	.byte	-2
	.byte	2
	.byte	4
	.byte	-2
	.byte	1
	.byte	5
	.byte	-2
	.byte	1
	.byte	6
	.byte	-1
	.byte	3
	.byte	-3
	.byte	0
	.byte	5
	.byte	-1
	.byte	3
	.byte	-2
	.byte	2
	.byte	-4
	.byte	1
	.byte	5
	.byte	0
	.byte	5
	.byte	0
	.byte	5
	.byte	0
	.byte	4
	.byte	0
	.byte	5
	.byte	1
	.byte	-4
	.byte	1
	.byte	-2
	.byte	2
	.byte	-2
	.byte	4
	.byte	0
	.byte	-4
	.byte	3
	.byte	-1
	.byte	5
	.byte	1
	.byte	-2
	.byte	4
	.byte	0
	.byte	-3
	.byte	3
	.byte	1
	.byte	-3
	.byte	4
	.byte	2
	.byte	-2
	.byte	5
	.byte	2
	.byte	0
	.byte	-3
	.byte	4
	.byte	2
	.byte	0
	.byte	-3
	.byte	4
	.byte	2
	.byte	1
	.byte	-1
	.byte	-3
	.byte	5
	.byte	2
	.byte	1
	.byte	0
	.byte	-2
	.byte	-3
	.byte	4
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.byte	-2
	.byte	-3
	.byte	-4
	.byte	5
	.byte	4
	.byte	3
	.byte	3
	.byte	2
	.byte	2
	.byte	2
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	0
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	3
	.byte	3
	.byte	4
	.byte	4
	.byte	-4
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	1
	.byte	2
	.byte	4
	.byte	-3
	.byte	-2
	.byte	0
	.byte	2
	.byte	4
	.byte	-4
	.byte	-1
	.byte	1
	.byte	3
	.byte	-4
	.byte	-2
	.byte	1
	.byte	3
	.byte	-4
	.byte	-1
	.byte	2
	.byte	3
	.byte	-3
	.byte	0
	.byte	3
	.byte	-4
	.byte	0
	.byte	2
	.byte	-4
	.byte	-1
	.byte	2
	.byte	-4
	.byte	0
	.byte	3
	.byte	-3
	.byte	1
	.byte	4
	.byte	-2
	.byte	1
	.byte	-3
	.byte	0
	.byte	4
	.byte	-2
	.byte	2
	.byte	-3
	.byte	1
	.byte	5
	.byte	0
	.byte	3
	.byte	-2
	.byte	2
	.byte	-2
	.byte	2
	.byte	-3
	.byte	2
	.byte	-3
	.byte	1
	.byte	-3
	.byte	1
	.byte	-3
	.byte	1
	.byte	-2
	.byte	2
	.byte	-3
	.byte	3
	.byte	-1
	.byte	3
	.byte	0
	.byte	5
	.byte	1
	.byte	-3
	.byte	2
	.byte	-2
	.byte	3
	.byte	-1
	.byte	-4
	.byte	1
	.byte	-2
	.byte	4
	.byte	0
	.byte	-3
	.byte	3
	.byte	-1
	.byte	-4
	.byte	2
	.byte	-1
	.byte	5
	.byte	2
	.byte	-1
	.byte	5
	.byte	2
	.byte	-1
	.byte	-3
	.byte	3
	.byte	0
	.byte	-3
	.byte	3
	.byte	2
	.byte	-1
	.byte	-3
	.byte	2
	.byte	1
	.byte	-2
	.byte	-4
	.byte	2
	.byte	1
	.byte	-1
	.byte	-3
	.byte	4
	.byte	2
	.byte	0
	.byte	-2
	.byte	4
	.byte	2
	.byte	1
	.byte	-1
	.byte	-2
	.byte	-4
	.byte	4
	.byte	2
	.byte	1
	.byte	-1
	.byte	-2
	.byte	-3
	.byte	4
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.byte	-1
	.byte	-3
	.byte	-3
	.byte	5
	.byte	3
	.byte	3
	.byte	2
	.byte	2
	.byte	1
	.byte	0
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-4
	.byte	4
	.byte	4
	.byte	4
	.byte	4
	.byte	4
	.byte	4
	.byte	4
	.byte	5
	.byte	5
	.byte	-4
	.byte	-3
	.byte	-3
	.byte	-3
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	2
	.byte	2
	.byte	4
	.byte	4
	.byte	-4
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	1
	.byte	3
	.byte	4
	.byte	-4
	.byte	-2
	.byte	-1
	.byte	1
	.byte	2
	.byte	4
	.byte	-3
	.byte	-1
	.byte	0
	.byte	2
	.byte	4
	.byte	-2
	.byte	-1
	.byte	1
	.byte	3
	.byte	-4
	.byte	-1
	.byte	0
	.byte	3
	.byte	4
	.byte	-1
	.byte	0
	.byte	3
	.byte	-4
	.byte	-1
	.byte	1
	.byte	3
	.byte	-2
	.byte	0
	.byte	2
	.byte	-3
	.byte	-1
	.byte	1
	.byte	4
	.byte	-2
	.byte	0
	.byte	4
	.byte	-2
	.byte	1
	.byte	3
	.byte	-2
	.byte	1
	.byte	4
	.byte	-2
	.byte	1
	.byte	4
	.byte	-1
	.byte	1
	.byte	-4
	.byte	0
	.byte	3
	.byte	-2
	.byte	1
	.byte	4
	.byte	-1
	.byte	3
	.byte	-3
	.byte	1
	.byte	4
	.byte	-1
	.byte	3
	.byte	-1
	.byte	1
	.byte	-3
	.byte	0
	.byte	4
	.byte	-1
	.byte	4
	.byte	-1
	.byte	2
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.byte	-3
	.byte	1
	.byte	-3
	.byte	1
	.byte	4
	.byte	1
	.byte	5
	.byte	1
	.byte	4
	.byte	0
	.byte	-4
	.byte	0
	.byte	-3
	.byte	2
	.byte	-2
	.byte	2
	.byte	-2
	.byte	3
	.byte	-1
	.byte	3
	.byte	-1
	.byte	4
	.byte	0
	.byte	-3
	.byte	1
	.byte	-2
	.byte	2
	.byte	-1
	.byte	4
	.byte	1
	.byte	-2
	.byte	2
	.byte	-1
	.byte	4
	.byte	0
	.byte	-3
	.byte	2
	.byte	-1
	.byte	4
	.byte	1
	.byte	-2
	.byte	2
	.byte	0
	.byte	-2
	.byte	2
	.byte	-1
	.byte	-3
	.byte	1
	.byte	-1
	.byte	4
	.byte	1
	.byte	-1
	.byte	4
	.byte	1
	.byte	-1
	.byte	-4
	.byte	2
	.byte	0
	.byte	-3
	.byte	2
	.byte	0
	.byte	-2
	.byte	4
	.byte	1
	.byte	-1
	.byte	-4
	.byte	2
	.byte	0
	.byte	-2
	.byte	3
	.byte	1
	.byte	0
	.byte	-2
	.byte	3
	.byte	1
	.byte	-1
	.byte	-3
	.byte	3
	.byte	1
	.byte	0
	.byte	-2
	.byte	4
	.byte	3
	.byte	0
	.byte	-2
	.byte	-3
	.byte	3
	.byte	2
	.byte	0
	.byte	-2
	.byte	-3
	.byte	3
	.byte	2
	.byte	0
	.byte	-1
	.byte	-3
	.byte	4
	.byte	2
	.byte	1
	.byte	0
	.byte	-2
	.byte	-2
	.byte	4
	.byte	3
	.byte	1
	.byte	1
	.byte	-1
	.byte	-1
	.byte	-3
	.byte	3
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.byte	0
	.byte	-2
	.byte	-3
	.byte	4
	.byte	3
	.byte	3
	.byte	2
	.byte	1
	.byte	1
	.byte	0
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-3
	.byte	4
	.byte	4
	.byte	3
	.byte	2
	.byte	2
	.byte	1
	.byte	1
	.byte	0
	.byte	1
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	0
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	1
	.byte	2
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	4
	.byte	-3
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	2
	.byte	3
	.byte	3
	.byte	4
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	0
	.byte	1
	.byte	2
	.byte	2
	.byte	3
	.byte	-3
	.byte	-1
	.byte	-1
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	2
	.byte	3
	.byte	4
	.byte	-3
	.byte	-1
	.byte	0
	.byte	1
	.byte	3
	.byte	-3
	.byte	-2
	.byte	0
	.byte	1
	.byte	3
	.byte	4
	.byte	-2
	.byte	-1
	.byte	1
	.byte	3
	.byte	4
	.byte	-2
	.byte	0
	.byte	2
	.byte	3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	2
	.byte	-3
	.byte	-2
	.byte	1
	.byte	3
	.byte	4
	.byte	-1
	.byte	0
	.byte	2
	.byte	-3
	.byte	-1
	.byte	0
	.byte	3
	.byte	-3
	.byte	0
	.byte	1
	.byte	3
	.byte	-2
	.byte	1
	.byte	2
	.byte	-3
	.byte	-1
	.byte	1
	.byte	3
	.byte	-1
	.byte	0
	.byte	2
	.byte	-2
	.byte	0
	.byte	2
	.byte	-3
	.byte	0
	.byte	2
	.byte	-3
	.byte	-1
	.byte	2
	.byte	4
	.byte	-1
	.byte	1
	.byte	4
	.byte	-1
	.byte	2
	.byte	-2
	.byte	0
	.byte	2
	.byte	-2
	.byte	0
	.byte	3
	.byte	-2
	.byte	1
	.byte	3
	.byte	-1
	.byte	1
	.byte	-3
	.byte	0
	.byte	2
	.byte	-2
	.byte	1
	.byte	4
	.byte	0
	.byte	2
	.byte	-3
	.byte	0
	.byte	3
	.byte	-1
	.byte	2
	.byte	-2
	.byte	0
	.byte	3
	.byte	0
	.byte	3
	.byte	-1
	.byte	1
	.byte	-3
	.byte	1
	.byte	3
	.byte	-1
	.byte	3
	.byte	-1
	.byte	1
	.byte	-2
	.byte	1
	.byte	-3
	.byte	0
	.byte	3
	.byte	0
	.byte	3
	.byte	-1
	.byte	2
	.byte	-2
	.byte	2
	.byte	-2
	.byte	1
	.byte	-3
	.byte	1
	.byte	-2
	.byte	1
	.byte	-3
	.byte	1
	.byte	-3
	.byte	1
	.byte	-3
	.byte	1
	.byte	-3
	.byte	0
	.byte	-3
	.byte	0
	.byte	-2
	.byte	1
	.byte	-3
	.byte	1
	.byte	-2
	.byte	1
	.byte	-1
	.byte	2
	.byte	-2
	.byte	2
	.byte	-1
	.byte	3
	.byte	-1
	.byte	3
	.byte	0
	.byte	4
	.byte	1
	.byte	-3
	.byte	1
	.byte	-2
	.byte	2
	.byte	-1
	.byte	3
	.byte	0
	.byte	4
	.byte	1
	.byte	-2
	.byte	2
	.byte	-1
	.byte	3
	.byte	0
	.byte	-2
	.byte	2
	.byte	-1
	.byte	3
	.byte	0
	.byte	3
	.byte	2
	.byte	-1
	.byte	2
	.byte	-1
	.byte	4
	.byte	1
	.byte	-2
	.byte	3
	.byte	0
	.byte	-3
	.byte	2
	.byte	-1
	.byte	3
	.byte	0
	.byte	-2
	.byte	3
	.byte	0
	.byte	-2
	.byte	2
	.byte	-1
	.byte	-3
	.byte	2
	.byte	-1
	.byte	3
	.byte	1
	.byte	-1
	.byte	4
	.byte	1
	.byte	-1
	.byte	4
	.byte	1
	.byte	-2
	.byte	4
	.byte	1
	.byte	-1
	.byte	4
	.byte	1
	.byte	-1
	.byte	4
	.byte	1
	.byte	0
	.byte	-2
	.byte	2
	.byte	0
	.byte	-2
	.byte	3
	.byte	0
	.byte	-1
	.byte	3
	.byte	2
	.byte	0
	.byte	-3
	.byte	2
	.byte	0
	.byte	-1
	.byte	4
	.byte	1
	.byte	0
	.byte	-3
	.byte	3
	.byte	1
	.byte	0
	.byte	-3
	.byte	2
	.byte	1
	.byte	-1
	.byte	4
	.byte	2
	.byte	0
	.byte	-1
	.byte	4
	.byte	2
	.byte	1
	.byte	-1
	.byte	-3
	.byte	2
	.byte	1
	.byte	-1
	.byte	-3
	.byte	3
	.byte	2
	.byte	0
	.byte	-2
	.byte	3
	.byte	3
	.byte	1
	.byte	-1
	.byte	-3
	.byte	3
	.byte	2
	.byte	1
	.byte	-1
	.byte	-3
	.byte	3
	.byte	1
	.byte	1
	.byte	-1
	.byte	-2
	.byte	3
	.byte	2
	.byte	1
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	2
	.byte	1
	.byte	1
	.byte	0
	.byte	-2
	.byte	3
	.byte	2
	.byte	2
	.byte	1
	.byte	0
	.byte	-1
	.byte	-2
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.byte	-1
	.byte	-2
	.byte	4
	.byte	3
	.byte	2
	.byte	1
	.byte	0
	.byte	0
	.byte	-1
	.byte	-2
	.byte	3
	.byte	3
	.byte	2
	.byte	1
	.byte	1
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-3
	.byte	3
	.byte	2
	.byte	2
	.byte	1
	.byte	1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	4
	.byte	3
	.byte	2
	.byte	2
	.byte	2
	.byte	1
	.byte	0
	.byte	0
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-3
	.byte	-3
	.byte	3
	.byte	3
	.byte	2
	.byte	2
	.byte	2
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	0
	.byte	0
	.byte	-1
	.byte	0
	.byte	-1
	.byte	-1
	.byte	-2
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-3
	.byte	-3
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-2
	.byte	-3
	.byte	-2
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	-2
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	1
	.byte	2
	.byte	2
	.byte	2
	.byte	3
	.byte	3
	.byte	3
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	0
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	2
	.byte	2
	.byte	3
	.byte	4
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	1
	.byte	2
	.byte	3
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	2
	.byte	-3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	1
	.byte	2
	.byte	2
	.byte	3
	.byte	-2
	.byte	-1
	.byte	0
	.byte	0
	.byte	1
	.byte	2
	.byte	3
	.byte	4
	.byte	-2
	.byte	-1
	.byte	0
	.byte	0
	.byte	2
	.byte	3
	.byte	-3
	.byte	-2
	.byte	0
	.byte	1
	.byte	1
	.byte	2
	.byte	4
	.byte	-2
	.byte	-1
	.byte	1
	.byte	1
	.byte	3
	.byte	-3
	.byte	-1
	.byte	0
	.byte	0
	.byte	2
	.byte	3
	.byte	-2
	.byte	0
	.byte	0
	.byte	2
	.byte	2
	.byte	-2
	.byte	-1
	.byte	0
	.byte	1
	.byte	3
	.byte	-3
	.byte	-1
	.byte	1
	.byte	1
	.byte	3
	.byte	-2
	.byte	-1
	.byte	1
	.byte	1
	.byte	3
	.byte	-2
	.byte	0
	.byte	1
	.byte	3
	.byte	-2
	.byte	-1
	.byte	1
	.byte	2
	.byte	4
	.byte	-1
	.byte	0
	.byte	2
	.byte	3
	.byte	-2
	.byte	0
	.byte	1
	.byte	2
	.byte	-2
	.byte	0
	.byte	1
	.byte	3
	.byte	-2
	.byte	-1
	.byte	2
	.byte	3
	.byte	-2
	.byte	-1
	.byte	2
	.byte	3
	.byte	-2
	.byte	0
	.byte	2
	.byte	3
	.byte	-1
	.byte	0
	.byte	3
	.byte	-2
	.byte	0
	.byte	1
	.byte	3
	.byte	-2
	.byte	1
	.byte	2
	.byte	-2
	.byte	-1
	.byte	1
	.byte	2
	.byte	-1
	.byte	1
	.byte	2
	.byte	-2
	.byte	0
	.byte	1
	.byte	3
	.byte	-1
	.byte	1
	.byte	3
	.byte	-1
	.byte	1
	.byte	3
	.byte	-2
	.byte	0
	.byte	2
	.byte	-2
	.byte	0
	.byte	2
	.byte	-2
	.byte	0
	.byte	2
	.byte	4
	.byte	-1
	.byte	1
	.byte	3
	.byte	-1
	.byte	2
	.byte	4
	.byte	0
	.byte	2
	.byte	4
	.byte	0
	.byte	2
	.byte	-2
	.byte	-1
	.byte	1
	.byte	-2
	.byte	0
	.byte	2
	.byte	-2
	.byte	1
	.byte	3
	.byte	-1
	.byte	0
	.byte	3
	.byte	-1
	.byte	1
	.byte	4
	.byte	0
	.byte	2
	.byte	-2
	.byte	0
	.byte	3
	.byte	-1
	.byte	1
	.byte	3
	.byte	-1
	.byte	2
	.byte	-2
	.byte	1
	.byte	2
	.byte	-1
	.byte	2
	.byte	-2
	.byte	0
	.byte	2
	.byte	-1
	.byte	2
	.byte	3
	.byte	0
	.byte	3
	.byte	-2
	.byte	1
	.byte	4
	.byte	0
	.byte	2
	.byte	-1
	.byte	2
	.byte	-3
	.byte	0
	.byte	3
	.byte	-1
	.byte	2
	.byte	-1
	.byte	0
	.byte	3
	.byte	-1
	.byte	2
	.byte	-1
	.byte	1
	.byte	-2
	.byte	0
	.byte	3
	.byte	0
	.byte	2
	.byte	-1
	.byte	1
	.byte	-2
	.byte	1
	.byte	3
	.byte	0
	.byte	2
	.byte	0
	.byte	2
	.byte	-1
	.byte	1
	.byte	-2
	.byte	0
	.byte	4
	.byte	0
	.byte	3
	.byte	-1
	.byte	3
	.byte	-1
	.byte	2
	.byte	-2
	.byte	2
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.byte	3
	.byte	0
	.byte	3
	.byte	0
	.byte	3
	.byte	0
	.byte	2
	.byte	0
	.byte	2
	.byte	0
	.byte	2
	.byte	0
	.byte	2
	.byte	-1
	.byte	2
	.byte	-1
	.byte	1
	.byte	-1
	.byte	2
	.byte	-2
	.byte	2
	.byte	-1
	.byte	1
	.byte	-1
	.byte	2
	.byte	-2
	.byte	2
	.byte	-1
	.byte	2
	.byte	-1
	.byte	2
	.byte	-1
	.byte	2
	.byte	-1
	.byte	2
	.byte	-1
	.byte	2
	.byte	0
	.byte	3
	.byte	0
	.byte	3
	.byte	0
	.byte	3
	.byte	0
	.byte	-3
	.byte	1
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.byte	-1
	.byte	2
	.byte	-1
	.byte	2
	.byte	0
	.byte	3
	.byte	1
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.byte	-1
	.byte	3
	.byte	0
	.byte	3
	.byte	0
	.byte	-2
	.byte	1
	.byte	-2
	.byte	1
	.byte	0
	.byte	3
	.byte	0
	.byte	4
	.byte	1
	.byte	-2
	.byte	2
	.byte	-1
	.byte	2
	.byte	0
	.byte	3
	.byte	1
	.byte	-2
	.byte	1
	.byte	-1
	.byte	2
	.byte	0
	.byte	-2
	.byte	1
	.byte	-1
	.byte	2
	.byte	0
	.byte	3
	.byte	1
	.byte	-1
	.byte	2
	.byte	0
	.byte	3
	.byte	0
	.byte	-1
	.byte	2
	.byte	-1
	.byte	3
	.byte	1
	.byte	-2
	.byte	2
	.byte	0
	.byte	-2
	.byte	1
	.byte	-1
	.byte	3
	.byte	1
	.byte	-2
	.byte	1
	.byte	-1
	.byte	3
	.byte	1
	.byte	-1
	.byte	2
	.byte	0
	.byte	-2
	.byte	2
	.byte	0
	.byte	3
	.byte	1
	.byte	-1
	.byte	3
	.globl	ff_cos_tabs
	.section	.data.rel,"aw",@progbits
	.align	2
	.type	ff_cos_tabs, @object
	.size	ff_cos_tabs, 28
ff_cos_tabs:
	.word	ff_cos_16
	.word	ff_cos_32
	.word	ff_cos_64
	.word	ff_cos_128
	.word	ff_cos_256
	.word	ff_cos_512
	.word	ff_cos_1024
	.globl	wmav1_decoder
	.section	.rodata.str1.4
	.align	2
$LC19:
	.ascii	"wmav1\000"
	.section	.data.rel
	.align	2
	.type	wmav1_decoder, @object
	.size	wmav1_decoder, 52
wmav1_decoder:
	.word	$LC19
	.word	1
	.word	86024
	.word	172448
	.word	wma_decode_init
	.word	0
	.word	ff_wma_end
	.word	wma_decode_superframe
	.space	20
	.globl	wmav2_decoder
	.section	.rodata.str1.4
	.align	2
$LC20:
	.ascii	"wmav2\000"
	.section	.data.rel
	.align	2
	.type	wmav2_decoder, @object
	.size	wmav2_decoder, 52
wmav2_decoder:
	.word	$LC20
	.word	1
	.word	86025
	.word	172448
	.word	wma_decode_init
	.word	0
	.word	ff_wma_end
	.word	wma_decode_superframe
	.space	20
	.rdata
	.align	2
	.type	ff_wma_lspfix_codebook, @object
	.size	ff_wma_lspfix_codebook, 640
ff_wma_lspfix_codebook:
	.word	130242
	.word	129725
	.word	129223
	.word	128621
	.word	127820
	.word	126705
	.word	124990
	.word	122022
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	129276
	.word	128505
	.word	127784
	.word	127013
	.word	126168
	.word	125180
	.word	124015
	.word	122651
	.word	120956
	.word	118855
	.word	116405
	.word	113551
	.word	110040
	.word	105486
	.word	98848
	.word	87242
	.word	124590
	.word	122213
	.word	120206
	.word	118075
	.word	115769
	.word	113257
	.word	110430
	.word	107308
	.word	103876
	.word	99480
	.word	94162
	.word	87786
	.word	81380
	.word	73571
	.word	63406
	.word	48528
	.word	115962
	.word	109984
	.word	104822
	.word	100637
	.word	96646
	.word	92543
	.word	88152
	.word	83453
	.word	78187
	.word	71936
	.word	64762
	.word	57173
	.word	48739
	.word	39170
	.word	28291
	.word	11781
	.word	93997
	.word	86533
	.word	79347
	.word	72468
	.word	65904
	.word	59203
	.word	52716
	.word	46340
	.word	39602
	.word	32646
	.word	25238
	.word	17765
	.word	9442
	.word	144
	.word	-10961
	.word	-24206
	.word	65468
	.word	55174
	.word	46369
	.word	37950
	.word	30838
	.word	24227
	.word	17581
	.word	11248
	.word	4724
	.word	-2007
	.word	-91775
	.word	-16469
	.word	-24389
	.word	-33473
	.word	-42525
	.word	-52631
	.word	17377
	.word	4138
	.word	-5814
	.word	-13830
	.word	-20362
	.word	-26005
	.word	-30948
	.word	-35499
	.word	-40268
	.word	-45183
	.word	-50188
	.word	-55817
	.word	-62449
	.word	-69805
	.word	-78978
	.word	-90189
	.word	-35350
	.word	-48347
	.word	-57950
	.word	-66269
	.word	-74311
	.word	-83119
	.word	-93089
	.word	-106191
	.word	-72194
	.word	-76358
	.word	-80830
	.word	-85468
	.word	-90219
	.word	-96242
	.word	-103302
	.word	-109375
	.word	-25298
	.word	-36706
	.word	-43895
	.word	-49826
	.word	-54949
	.word	-59551
	.word	-63838
	.word	-67958
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	-102331
	.word	-108753
	.word	-113174
	.word	-116561
	.word	-119409
	.word	-122041
	.word	-124705
	.word	-127678
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.section	.data.rel.ro.local,"aw",@progbits
	.align	2
	.type	fft_dispatch_4750, @object
	.size	fft_dispatch_4750, 36
fft_dispatch_4750:
	.word	fft4_4750
	.word	fft8_4750
	.word	fft16_4750
	.word	fft32_4750
	.word	fft64_4750
	.word	fft128_4750
	.word	fft256_4750
	.word	fft512_4750
	.word	fft1024_4750
	.align	2
	.type	fft_dispatch_4740, @object
	.size	fft_dispatch_4740, 36
fft_dispatch_4740:
	.word	fft4_4740
	.word	fft8_4740
	.word	fft16_4740
	.word	fft32_4740
	.word	fft64_4740
	.word	fft128_4740
	.word	fft256_4740
	.word	fft512_4740
	.word	fft1024_4740

	.comm	ff_imdct_half_fixed,4,4

	.comm	ff_cos_16,32,4

	.comm	ff_cos_32,64,4

	.comm	ff_cos_64,128,4

	.comm	ff_cos_128,256,4

	.comm	ff_cos_256,512,4

	.comm	ff_cos_512,1024,4

	.comm	ff_cos_1024,2048,4
	.ident	"GCC: (GNU) 4.1.2"
