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
 *	T-Kernel extended SVC interface library (smp_sh7786)
 */

#include <machine.h>
#include <tk/sysdef.h>
#include <sys/svc/fnsysmgr.h>

	.text
	.balign	2
	.globl	Csym(tk_ref_smb)
	.type	Csym(tk_ref_smb), @function
Csym(tk_ref_smb):
	mov.l	r7, @-r15
	mov.l	r6, @-r15
	mov.l	r5, @-r15
	mov.l	r4, @-r15
	mov.l	fno, r0
	mov	r15, r4
	trapa	#TRAP_SVC
	rts
	add	#4*4, r15

	.balign	4
fno:	.long	SYSTEM_TK_REF_SMB_FN

