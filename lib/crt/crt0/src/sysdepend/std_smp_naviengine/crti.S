/*
 *----------------------------------------------------------------------
 *    SMP T-Kernel
 *
 *    Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
 *    SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
 *----------------------------------------------------------------------
 *
 *    Version:   1.00.01
 *    Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
 *
 *----------------------------------------------------------------------
 */

.macro FUNC_START
	.arm
	mov	ip, sp
	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}
	sub	fp, ip, #4
.endm

	.section	.init
	.balign		4
	.global		_init
_init:
	FUNC_START
	ldr	sl, I_GOT
.L4:
	add	sl, pc, sl
	b	I_TOP
I_GOT:	.word	_GLOBAL_OFFSET_TABLE_-(.L4+8)
I_TOP:

	.section	.fini
	.balign		4
	.global		_fini
_fini:
	FUNC_START
	ldr	sl, F_GOT
.L5:
	add	sl, pc ,sl
	b	F_TOP
F_GOT:	.word	_GLOBAL_OFFSET_TABLE_-(.L5+8)
F_TOP:
