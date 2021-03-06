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
 *	T-Kernel SVC interface library (smp_sh7776)
 */

#include <machine.h>
#include <tk/sysdef.h>
#include <sys/svc/tkfncd.h>

	.text
	.balign	2
	.globl	Csym(tk_sta_tsk)
	.type	Csym(tk_sta_tsk), @function
Csym(tk_sta_tsk):
	mov.l	fno, r0
	trapa	#TRAP_SVC
	rts
	nop

	.balign	4
fno:	.long	TFN_STA_TSK

