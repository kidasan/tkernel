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
 *	Atomic memory operation function (SH7786)
 */

#define	_in_asm_source_

#include <machine.h>

/*
 * UW	atomic_inc(UW *addr)
 *
 * The increment does *addr.
 * The value after the increment is returned.
 */
	.text
	.balign	2
	.globl	Csym(atomic_inc)
	.type	Csym(atomic_inc), @function
Csym(atomic_inc):
	movli.l	@r4, r0
	add	#1, r0
	movco.l	r0, @r4
	bf	Csym(atomic_inc)
	rts
	nop

/*
 * UW	atomic_dec(UW *addr)
 *
 * The decrement does *addr.
 * The value after the decrement is returned.
 */
	.text
	.balign	2
	.globl	Csym(atomic_dec)
	.type	Csym(atomic_dec), @function
Csym(atomic_dec):
	movli.l	@r4, r0
	add	#-1, r0
	movco.l	r0, @r4
	bf	Csym(atomic_dec)
	rts
	nop

/*
 * UW	atomic_add(UW *addr, UW val)
 *
 * Val is added to *addr.
 * The added value is returned. 
 */
	.text
	.balign	2
	.globl	Csym(atomic_add)
	.type	Csym(atomic_add), @function
Csym(atomic_add):
	movli.l	@r4, r0
	add	r5, r0
	movco.l	r0, @r4
	bf	Csym(atomic_add)
	rts
	nop

/*
 * UW	atomic_sub(UW *addr, UW val)
 *
 * Val is subtracted to *addr.
 * The subtracted value is returned. 
 */
	.text
	.balign	2
	.globl	Csym(atomic_sub)
	.type	Csym(atomic_sub), @function
Csym(atomic_sub):
	movli.l	@r4, r0
	sub	r5, r0
	movco.l	r0, @r4
	bf	Csym(atomic_sub)
	rts
	nop

/*
 * UW	atomic_xchg(UW *addr, UW val)
 *
 * Val is substituted for *addr. 
 * The value before it substitutes it is returned. 
 */
	.text
	.balign	2
	.globl	Csym(atomic_xchg)
	.type	Csym(atomic_xchg), @function
Csym(atomic_xchg):
	movli.l	@r4, r0
	mov	r0, r7
	mov	r5, r0
	movco.l	r0, @r4
	bf	Csym(atomic_xchg)
	rts
	mov	r7, r0

/*
 * UW	atomic_cmpxchg(UW *addr, UW val, UW cmp)
 *
 * If *addr and cmp are the same, val is substituted. 
 * The value before it substitutes it is returned. 
 */
	.text
	.balign	2
	.globl	Csym(atomic_cmpxchg)
	.type	Csym(atomic_cmpxchg), @function
Csym(atomic_cmpxchg):
	movli.l	@r4, r0
	mov	r0, r7
	cmp/eq	r6, r0
	bf	l_atomcmp
	mov	r5, r0
	movco.l	r0, @r4
	bf	Csym(atomic_cmpxchg)
	rts
	mov	r7, r0

  l_atomcmp:
	movco.l	r0, @r4
	rts
	mov	r7, r0

/*
 * UW	atomic_bitset(UW *addr, UW setptn)
 *
 * The logical disjunction of *addr and setptn is substituted for *addr. 
 * The value before it substitutes it is returned. 
 */
	.text
	.balign	2
	.globl	Csym(atomic_bitset)
	.type	Csym(atomic_bitset), @function
Csym(atomic_bitset):
	movli.l	@r4, r0
	mov	r0, r1
	or	r5, r0
	movco.l	r0, @r4
	bf	Csym(atomic_bitset)
	rts
	mov	r1, r0

/*
 * UW	atomic_bitclr(UW *addr, UW clrptn)
 *
 * The logical conjunction of *addr and clrptn is substituted for *addr. 
 * The value before it substitutes it is returned. 
 */
	.text
	.balign	2
	.globl	Csym(atomic_bitclr)
	.type	Csym(atomic_bitclr), @function
Csym(atomic_bitclr):
	movli.l	@r4, r0
	mov	r0, r1
	and	r5, r0
	movco.l	r0, @r4
	bf	Csym(atomic_bitclr)
	rts
	mov	r1, r0

