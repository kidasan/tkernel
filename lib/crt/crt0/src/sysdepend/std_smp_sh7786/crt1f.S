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

/*
 *	@(#)crt1f.S (SH)
 *
 *	C startup 
 *
 *	For static links 
 *	shared library loader is not linked.
 */

#include <machine.h>

	.data
	.globl	Csym(__initloading)
	.globl	Csym(__callinit)
	.globl	Csym(__callfini)

			.balign	4
Csym(__initloading):	.int	initloading
Csym(__callinit):	.int	Csym(_init)
Csym(__callfini):	.int	Csym(_fini)

	.text
	.balign	2
initloading:
	/* No action */
	rts
	nop
