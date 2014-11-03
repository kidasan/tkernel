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

#if _Csym == 0
#define	START	_start
#else
#define	START	start
#endif

	.text
	.globl	START
	.type	START, @function
	.balign	2
START:
	mov.l	L_Startup, r0
	jmp	@r0
	nop

		.balign	4
L_Startup:	.long	Csym(_C_startup)
