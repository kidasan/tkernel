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
 *	@(#)_spinlock.S (libsplock/NAVIENGINE)
 *
 *	Spin lock.
 */

#define	_in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>

	.text
	.balign	4
/*
 * Initialize spin lock
 * 	ER InitSpinLock( T_SPLOCK *lock )
 */
	.globl	Csym(InitSpinLock)
Csym(InitSpinLock):
	mov	r1, #0
	str	r1, [r0]
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr

/*
 * Acquire spin lock
 *	ER SpinLock( T_SPLOCK *lock )
 */
	.globl	Csym(SpinLock)
Csym(SpinLock):
	mov	r1, #1
  l_spinlock_retry:
	ldrex	r2, [r0]
	cmp	r2, r1
	strexne	r2, r1, [r0]
	cmpne	r2, r1
	beq	l_spinlock_retry
	mcr	p15, 0, r2, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr

/*
 * Try to acquire spin lock
 *	ER SpinTryLock( T_SPLOCK *lock )
 */
	.globl	Csym(SpinTryLock)
Csym(SpinTryLock):
	mov	r1, #1
	ldrex	r2, [r0]
	cmp	r2, r1
	strexne	r2, r1, [r0]
	cmpne	r2, r1
	ldrne	r0, =E_OK
	ldreq	r0, =E_BUSY
	mcrne	p15, 0, r2, c7, c10, 5		// DMB
	bx	lr

/*
 * Release spin lock
 *	ER SpinUnlock( T_SPLOCK *lock )
 */
	.globl	Csym(SpinUnlock)
Csym(SpinUnlock):
	mov	r1, #0
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	str	r1, [r0]
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr
