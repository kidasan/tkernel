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
 *	@(#)kernellock.S (T-Kernel/OS)
 *
 *	Spin lock.
 *	Spin lock routine of kernel internal usage.
 */

#define _in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>
#include <sys/smp_def.h>


/*
 * Lock
 * ER KernelLock( T_KLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(KernelLock)
Csym(KernelLock):
	mov.l	L_NONCACHE, r0
	or	r0, r4				/* non cache area access */
  l_loop:
	tas.b	@r4
	bf	l_loop
	mov.l	e_ok, r0
	rts
	nop

/*
 * Try lock
 * ER KernelTryLock( T_KLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(KernelTryLock)
Csym(KernelTryLock):
	mov.l	L_NONCACHE, r0
	or	r0, r4				/* non cache area access */
	tas.b	@r4
	bf	l_busy

  l_ok:
	mov.l	e_ok, r0
	rts
	nop

  l_busy:
	mov.l	e_busy, r0
	rts
	nop

/*
 * Unlock
 * ER KernelUnlock( T_KLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(KernelUnlock)
Csym(KernelUnlock):
	mov.l	L_NONCACHE, r0
	or	r0, r4				/* non cache area access */
	mov	#0, r1
	mov.b	r1, @r4
	mov.l	e_ok, r0
	rts
	nop

/*
 * Initialize
 * ER InitKernelLock( T_KLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(InitKernelLock)
Csym(InitKernelLock):
	mov.l	L_NONCACHE, r0
	or	r0, r4				/* non cache area access */
	mov	#0, r1
	mov.b	r1, @r4
	rts
	nop

		.balign	4
L_NONCACHE:	.long	0xa0000000
e_ok:		.long	E_OK
e_busy:		.long	E_BUSY
