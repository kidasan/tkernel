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
 *	@(#)tmsvc.h (libtm/SH7776)
 *
 *	Standard T-Engine/SH7776 definitions
 *
 *	* Used by assembler 
 */

#include <tk/sysdef.h>

/*
 * T-Monitor service call 
 */
.macro _TMCALL func, fno
	.text
	.balign	2
	.globl	Csym(\func)
	.type	Csym(\func), @function
Csym(\func):
  .if \fno < 128
	mov	#\fno, r0
	trapa	#TRAP_MONITOR
	rts
	nop
  .else
	mov.l	fno_\func, r0
	trapa	#TRAP_MONITOR
	rts
	nop
		.balign	4
    fno_\func:	.long	\fno
  .endif
.endm

#define TMCALL(func, fno)	_TMCALL func, fno
