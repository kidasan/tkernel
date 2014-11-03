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
#include <sys/svc/tdfncd.h>

	.text
	.balign	2
	.globl	Csym(td_acp_que)
	.type	Csym(td_acp_que), @function
Csym(td_acp_que):
	mov.l	fno, r0
	trapa	#TRAP_DEBUG
	rts
	nop

	.balign	4
fno:	.long	TDFN_ACP_QUE

