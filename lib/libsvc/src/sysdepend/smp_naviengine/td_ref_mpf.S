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
 *	T-Kernel SVC interface library (smp_naviengine)
 */

#include <machine.h>
#include <tk/sysdef.h>
#include <sys/svc/tdfncd.h>

	.text
	.balign	4
	.globl	Csym(td_ref_mpf)
	.type	Csym(td_ref_mpf), %function
Csym(td_ref_mpf):
	stmfd	sp!, {lr}
	ldr	ip, =TDFN_REF_MPF
	swi	SWI_DEBUG
	ldmfd	sp!, {lr}
	bx	lr

