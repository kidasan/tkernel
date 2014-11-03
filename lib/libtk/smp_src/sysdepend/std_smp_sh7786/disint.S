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
 *	@(#)disint.S (libtk/SH7786)
 *
 *	Enable/disable external interrupt.
 */

#include <machine.h>
#include <tk/sysdef.h>

/*
 * Disable external interrupt 
 * UINT disint( void )
 *	SR.I = 15; the return value is SR prior to the change.
 */
	.text
	.balign	2
	.globl	Csym(disint)
	.type	Csym(disint), @function
Csym(disint):
	mov.l	r1, @-SP
	stc	sr, r0
	stc	sr, r1
	or	#SR_I(15), r0
	ldc	r0, sr
	mov	r1, r0
	rts
	mov.l	@SP+, r1

/*
 * Enable external interrupt (restore to original state)
 * UINT enaint( UINT intsts )
 *	SR.I = msk.I. Only  SR.I changes; all else remains unchanged.
 *	The return value is SR prior to the change.
 */
	.text
	.balign	2
	.globl	Csym(enaint)
	.type	Csym(enaint), @function
Csym(enaint):
	mov.l	r1, @-SP
	stc	sr, r0
	stc	sr, r1
	xor	r4, r0
	and	#SR_I(15), r0
	xor	r1, r0
#if CPU_SH4
	mov.l	L_NOT_SR_FD, r4		// To prevent accidental FD = 1 setting
	and	r4, r0			// Set to FD = 0 at all times
#endif
	ldc	r0, sr
	mov	r1, r0
	rts
	mov.l	@SP+, r1

#if CPU_SH4
		.balign	4
  L_NOT_SR_FD:	.long	~SR_FD
#endif
