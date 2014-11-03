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
 *	@(#)tmsvc.h (libtm/NAVIENGINE)
 *
 *	NAVIENGINE definitions 
 *
 *	* Used by assembler  
 */

#include <tk/sysdef.h>

/*
 * T-Monitor service call 
 */
.macro _TMCALL func, fno
	.text
	.balign	4
	.globl	Csym(\func)
	.type	Csym(\func), %function
Csym(\func):
	stmfd	sp!, {lr}
	ldr	ip, =\fno
	swi	SWI_MONITOR
	ldmfd	sp!, {lr}
	bx	lr
.endm

#define TMCALL(func, fno)	_TMCALL func, fno
