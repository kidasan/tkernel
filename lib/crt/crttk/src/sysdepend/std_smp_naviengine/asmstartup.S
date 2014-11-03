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
 *	@(#)asmstartup.S (crttk/NAVIENGINE)
 *
 *	Driver/manager startup 
 */

#include <machine.h>
#include <tk/asm.h>

	.text
	.balign	4
	.globl	Csym(_C_startup)
	.type	Csym(_C_startup), %function
Csym(_C_startup):
	cmp	r0, #0
	blt	l_noinit		// Initialization sequence is performed only when ac >= 0

	ldr	r4, =__data_org		// When data area does not need to be moved,
	ldr	r5, =__data_start	// OS loads
	cmp	r4, r5
	beq	nocopy_data

	ldr	r6, =_edata		// initialize data area
	cmp	r5, r6
	bhs	nocopy_data
  copy_data:
	ldr	ip, [r4], #4
	str	ip, [r5], #4
	cmp	r5, r6
	blo	copy_data
  nocopy_data:

	ldr	ip, =0			// clear bss area
	ldr	r4, =__bss_start
	ldr	r5, =_end
	cmp	r4, r5
	bhs	nobss
  clrbss:
	str	ip, [r4], #4
	cmp	r4, r5
	blo	clrbss
  nobss:

  l_noinit:
	b	Csym(_P_startup)
