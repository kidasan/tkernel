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
 *	@(#)_spinlock.S (libsplock/SH7786)
 *
 *	Spin Lock.
 */

#define _in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>
#include <sys/smp_def.h>


/*
 * Lock
 * ER SpinLock( T_SPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(SpinLock)
Csym(SpinLock):
  l_retry:
	movli.l	@r4, r0
	tst	r0, r0
	bf	l_loop				/* It spin if already locked */
	mov	#0x80, r0
	movco.l	r0, @r4
	bf	l_retry				/* It retries when failing in the lock */
	mov.l	e_ok, r0
	rts
	nop

  l_loop:
	mov.l	@r4, r0
	tst	r0, r0
	bf	l_loop				/* It spin if already locked */
	bt	l_retry

/*
 * Try lock
 * ER SpinTryLock( T_SPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(SpinTryLock)
Csym(SpinTryLock):
	movli.l	@r4, r0
	tst	r0, r0
	bf	l_busy
	mov	#0x80, r0
	movco.l	r0, @r4
	bf	l_exit

  l_ok:
	mov.l	e_ok, r0
	rts
	nop

  l_busy:
	movco.l	r0, @r4
  l_exit:
	mov.l	e_busy, r0
	rts
	nop

/*
 * Unlock
 * ER SpinUnlock( T_SPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(SpinUnlock)
Csym(SpinUnlock):
	mov	#0, r0
	mov.l	r0, @r4
	rts
	nop

/*
 * Initialize
 * ER InitSpinLock( T_SPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(InitSpinLock)
Csym(InitSpinLock):
	mov	#0, r0
	mov.l	r0, @r4
	rts
	nop

		.balign	4
e_ok:		.long	E_OK
e_busy:		.long	E_BUSY

