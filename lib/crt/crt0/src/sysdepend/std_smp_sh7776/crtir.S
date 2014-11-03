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
	sts.l	pr, @-r15
	mov	#0, r12

	.section	.fini
	.global		Csym(_fini)
Csym(_fini):
	mov.l	r12, @-r15
	sts.l	pr, @-r15
	mov	#0, r12
