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
 *	@(#)_readwritelock.S (NAVIENGINE)
 *
 *	Read write spin lock
 */

#define	_in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>

	.text
	.balign	4
/*
 * Initialize read write spin lock
 *	ER InitRWSpinLock( T_RWSPLOCK *lock )
 */
	.globl	Csym(InitRWSpinLock)
Csym(InitRWSpinLock):
	mov	r1, #0
	str	r1, [r0]
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr

/*
 * Acquire read lock
 *	ER ReadLock( T_RWSPLOCK *lock )
 */
	.globl	Csym(ReadLock)
Csym(ReadLock):
  l_readlock_retry:
	ldrex	r1, [r0]
	subs	r1, r1, #1
	strexmi	r2, r1, [r0]
	cmpmi	r2, #1
	bpl	l_readlock_retry
	mcr	p15, 0, r2, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr

/*
 * Try to acquire read lock
 *	ER ReadTryLock( T_RWSPLOCK *lock )
 */
	.globl	Csym(ReadTryLock)
Csym(ReadTryLock):
	ldrex	r1, [r0]
	subs	r1, r1, #1
	strexmi	r2, r1, [r0]
	cmpmi	r2, #1
	mcrmi	p15, 0, r2, c7, c10, 5		// DMB
	ldrmi	r0, =E_OK
	ldrpl	r0, =E_BUSY
	bx	lr

/*
 * Release read lock
 *	ER ReadUnlock( T_RWSPLOCK *lock )
 */
	.globl	Csym(ReadUnlock)
Csym(ReadUnlock):
	mov	r1, #0
  l_readunlock_retry:
	ldrex	r1, [r0]
	add	r1, r1, #1
	strex	r2, r1, [r0]
	cmp	r2, #0
	bne	l_readunlock_retry
	mcr	p15, 0, r2, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr

/*
 * Acquire write lock
 *	ER WriteLock( T_RWSPLOCK *lock )
 */
	.globl	Csym(WriteLock)
Csym(WriteLock):
	mov	r1, #1
  l_writelock_retry:
	ldrex	r2, [r0]
	cmp	r2, #0
	strexeq	r2, r1, [r0]
	cmpeq	r2, #0
	bne	l_writelock_retry
	mcr	p15, 0, r2, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr

/*
 * Try to acquire write lock
 *	ER WriteTryLock( T_RWSPLOCK *lock )
 */
	.globl	Csym(WriteTryLock)
Csym(WriteTryLock):
	ldrex	r1, [r0]
	mov	r2, #1
	cmp	r1, #0
	strexeq	r3, r2, [r0]
	cmpeq	r3, #0
	mcreq	p15, 0, r3, c7, c10, 5		// DMB
	ldreq	r0, =E_OK
	ldrne	r0, =E_BUSY
	bx	lr

/*
 * Release write lock
 *	ER WriteUnlock( T_RWSPLOCK *lock )
 */
	.globl	Csym(WriteUnlock)
Csym(WriteUnlock):
	mov	r1, #0
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	str	r1, [r0]
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	ldr	r0, =E_OK
	bx	lr
