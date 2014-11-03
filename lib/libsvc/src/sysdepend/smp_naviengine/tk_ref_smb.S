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
 *	T-Kernel extended SVC interface library (smp_naviengine)
 */

#include <machine.h>
#include <tk/sysdef.h>
#include <sys/svc/fnsysmgr.h>

	.text
	.balign	4
	.globl	Csym(tk_ref_smb)
	.type	Csym(tk_ref_smb), %function
Csym(tk_ref_smb):
	stmfd	sp!, {r0-r3}
	mov	r0, sp
	stmfd	sp!, {lr}
	ldr	ip, =SYSTEM_TK_REF_SMB_FN
	swi	SWI_SVC
	ldmfd	sp!, {lr}
	add	sp, sp, #4*4
	bx	lr

