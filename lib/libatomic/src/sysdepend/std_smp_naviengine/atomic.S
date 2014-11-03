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
 *	atomic.S (libatomic)
 *
 *	Atomic memory operation function (NAVIENGINE)
 */

#define	_in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>

	.text
	.balign	4
/*
 * UW	atomic_inc(UW *addr)
 *
 * The increment does *addr.
 * The value after the increment is returned.
 */
	.globl	Csym(atomic_inc)
Csym(atomic_inc):
	mov	r1, #0
	mcr	p15, 0, r1, c7, c10, 5		// DMB
  l_atomic_inc_retry:
	ldrex	r1, [r0]
	add	r1, r1, #1
	strex	r2, r1, [r0]
	cmp	r2, #0
	bne	l_atomic_inc_retry
	mcr	p15, 0, r2, c7, c10, 5		// DMB
	mov	r0, r1
	bx	lr

/*
 * UW	atomic_dec(UW *addr)
 *
 * The decrement does *addr.
 * The value after the decrement is returned.
 */
	.globl	Csym(atomic_dec)
Csym(atomic_dec):
	mov	r1, #0
	mcr	p15, 0, r1, c7, c10, 5		// DMB
  l_atomic_dec_retry:
	ldrex	r1, [r0]
	sub	r1, r1, #1
	strex	r2, r1, [r0]
	cmp	r2, #0
	bne	l_atomic_dec_retry
	mcr	p15, 0, r2, c7, c10, 5		// DMB
	mov	r0, r1
	bx	lr

/*
 * UW	atomic_add(UW *addr, UW val)
 *
 * Val is added to *addr.
 * The added value is returned. 
 */
	.globl	Csym(atomic_add)
Csym(atomic_add):
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 5		// DMB
  l_atomic_add_retry:
	ldrex	r2, [r0]
	add	r2, r2, r1
	strex	r3, r2, [r0]
	cmp	r3, #0
	bne	l_atomic_add_retry
	mcr	p15, 0, r3, c7, c10, 5		// DMB
	mov	r0, r2
	bx	lr

/*
 * UW	atomic_sub(UW *addr, UW val)
 *
 * Val is subtracted to *addr.
 * The subtracted value is returned. 
 */
	.globl	Csym(atomic_sub)
Csym(atomic_sub):
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 5		// DMB
  l_atomic_sub_retry:
	ldrex	r2, [r0]
	sub	r2, r2, r1
	strex	r3, r2, [r0]
	cmp	r3, #0
	bne	l_atomic_sub_retry
	mcr	p15, 0, r3, c7, c10, 5		// DMB
	mov	r0, r2
	bx	lr

/*
 * UW	atomic_xchg(UW *addr, UW val)
 *
 * Val is substituted for *addr. 
 * The value before it substitutes it is returned. 
 */
	.globl	Csym(atomic_xchg)
Csym(atomic_xchg):
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 5		// DMB
  l_atomic_xchg_retry:
	ldrex	r2, [r0]
	strex	r3, r1, [r0]
	cmp	r3, #0
	bne	l_atomic_xchg_retry
	mcr	p15, 0, r3, c7, c10, 5		// DMB
	mov	r0, r2
	bx	lr

/*
 * UW	atomic_cmpxchg(UW *addr, UW val, UW cmp)
 *
 * If *addr and cmp are the same, val is substituted. 
 * The value before it substitutes it is returned. 
 */
	.globl	Csym(atomic_cmpxchg)
Csym(atomic_cmpxchg):
	mov	r3, #0
	mcr	p15, 0, r3, c7, c10, 5		// DMB
  l_atomic_cmpxchg_retry:
	ldrex	r3, [r0]
	cmp	r3, r2
	bne	l_atomic_cmpxchg_exit
	strex	ip, r1, [r0]
	cmp	ip, #0
	bne	l_atomic_cmpxchg_retry
	mcr	p15, 0, ip, c7, c10, 5		// DMB
  l_atomic_cmpxchg_exit:
	mov	r0, r3
	bx	lr

/*
 * UW	atomic_bitset(UW *addr, UW setptn)
 *
 * The logical disjunction of *addr and setptn is substituted for *addr. 
 * The value before it substitutes it is returned. 
 */
	.globl	Csym(atomic_bitset)
Csym(atomic_bitset):
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 5		// DMB
  l_atomic_bitset_retry:
	ldrex	r2, [r0]
	orr	r3, r2, r1
	strex	ip, r3, [r0]
	cmp	ip, #0
	bne	l_atomic_bitset_retry
	mcr	p15, 0, ip, c7, c10, 5		// DMB
	mov	r0, r2
	bx	lr

/*
 * UW	atomic_bitclr(UW *addr, UW clrptn)
 *
 * The logical conjunction of *addr and clrptn is substituted for *addr. 
 * The value before it substitutes it is returned. 
 */
	.globl	Csym(atomic_bitclr)
Csym(atomic_bitclr):
	mov	r2, #0
	mcr	p15, 0, r2, c7, c10, 5		// DMB
  l_atomic_bitclr_retry:
	ldrex	r2, [r0]
	and	r3, r2, r1
	strex	ip, r3, [r0]
	cmp	ip, #0
	bne	l_atomic_bitclr_retry
	mcr	p15, 0, ip, c7, c10, 5		// DMB
	mov	r0, r2
	bx	lr

