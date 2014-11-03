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
 *	@(#)_readwritelock.S (libsplock/SH7786)
 *
 *	Read Write Spin Lock.
 */

#define _in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>


/*
 * Read Lock
 * ER ReadLock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(ReadLock)
Csym(ReadLock):
	mov.l	WL, r1
  l_retry_r:
	movli.l	@r4, r0
	cmp/eq	r1, r0				/* Is the write locked ? */
	bt	l_loop_r			/* It spin if already locked */
	add	#1, r0
	movco.l	r0, @r4
	bf	l_retry_r			/* It retries when failing in the lock */
	mov.l	e_ok_rw, r0
	rts
	nop

  l_loop_r:
	mov.l	@r4, r0
	cmp/eq	r1, r0				/* Is the write locked ? */
	bt	l_loop_r			/* It spin if already locked */
	bf	l_retry_r

/*
 * Read Try Lock
 * ER ReadTryLock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(ReadTryLock)
Csym(ReadTryLock):
	mov.l	WL, r1
	movli.l	@r4, r0
	cmp/eq	r1, r0				/* Is the write locked ? */
	bt	l_busy_r			/* It retries if already locked */
	add	#1, r0
	movco.l	r0, @r4
	bf	l_exit_r			/* It retries when failing in the lock */

  l_ok_r:
	mov.l	e_ok_rw, r0
	rts
	nop

  l_busy_r:
	movco.l	r0, @r4
  l_exit_r:
	mov.l	e_busy_rw, r0
	rts
	nop

/*
 * Read Unlock
 * ER ReadUnlock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(ReadUnlock)
Csym(ReadUnlock):
	movli.l	@r4, r0
	add	#-1, r0
	movco.l	r0, @r4
	bf	Csym(ReadUnlock)		/* It retries when failing in the lock */
	mov.l	e_ok_rw, r0
	rts
	nop

/*
 * Write Lock
 * ER WriteLock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(WriteLock)
Csym(WriteLock):
  l_retry_w:
	movli.l	@r4, r0
	tst	r0, r0
	bf	l_loop_w			/* It spin if already locked */
	mov.l	WL, r0
	movco.l	r0, @r4
	bf	l_retry_w			/* It retries when failing in the lock */
	mov.l	e_ok_rw, r0
	rts
	nop

  l_loop_w:
	mov.l	@r4, r0
	tst	r0, r0
	bf	l_loop_w			/* It spin if already locked */
	bt	l_retry_w

/*
 * Write Try Lock
 * ER WriteTryLock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(WriteTryLock)
Csym(WriteTryLock):
	movli.l	@r4, r0
	tst	r0, r0
	bf	l_busy_w			/* It retries if already locked */
	mov.l	WL, r0
	movco.l	r0, @r4
	bf	l_exit_w			/* It retries when failing in the lock */

  l_ok_w:
	mov.l	e_ok_rw, r0
	rts
	nop

  l_busy_w:
	movco.l	r0, @r4
  l_exit_w:
	mov.l	e_busy_rw, r0
	rts
	nop

/*
 * Write Unlock
 * ER WriteUnlock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(WriteUnlock)
Csym(WriteUnlock):
	mov	#0, r0
	mov.l	r0, @r4
	mov.l	e_ok_rw, r0
	rts
	nop

/*
 * Initialize Read/Write Spin Lock
 * ER InitRWSpinLock( T_RWSPLOCK *lock )
 */
	.text
	.balign	4
	.global	Csym(InitRWSpinLock)
Csym(InitRWSpinLock):
	mov	#0, r0
	mov.l	r0, @r4
	rts
	nop

		.balign	4
e_ok_rw:	.long	E_OK
e_busy_rw:	.long	E_BUSY
WL:		.long	0xffffffff

