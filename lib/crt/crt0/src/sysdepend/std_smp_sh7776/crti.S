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

#include <machine.h>

	.section	.init
	.global		Csym(_init)
Csym(_init):
	mov.l	r12, @-r15
	mova	I_GOT, r0
	mov.l	I_GOT, r12
	sts.l	pr, @-r15
	bra	I_TOP
	add	r0, r12
I_GOT:	.long	_GLOBAL_OFFSET_TABLE_
I_TOP:

	.section	.fini
	.global		Csym(_fini)
Csym(_fini):
	mov.l	r12, @-r15
	mova	F_GOT, r0
	mov.l	F_GOT, r12
	sts.l	pr, @-r15
	bra	F_TOP
	add	r0, r12
F_GOT:	.long	_GLOBAL_OFFSET_TABLE_
F_TOP:
