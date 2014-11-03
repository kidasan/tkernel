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
 *	@(#)asmstartup.S (crttk)
 *
 *	Driver/manager startup 
 */

#include <machine.h>
#include <tk/asm.h>
#include <tk/syscall.h>

	.text
	.balign	4
	.globl	Csym(_C_startup)
	.type	Csym(_C_startup), @function
Csym(_C_startup):
	cmp/pz	r4
	bf	l_noinit	// Initialize is performed only when ac >= 0

	mov.l	DATA_ORG, r1	// When data area does not need to be moved,
	mov.l	DATA_START, r2	// OS loads
	cmp/eq	r1, r2
	bt	l_bssinit

        mov.l   DATA_END, r3
        bra     l1		// initialize data area
        nop
  copydata:
	mov.l	@r1+, r0
	mov.l	r0, @r2
	add	#4, r2
  l1:
	cmp/hi	r2, r3
	bt	copydata

  l_bssinit:
	mov.l	BSS_START, r2	// clear bss area
	mov.l	BSS_END, r3
	bra	l2
	mov	#0, r0
  clrbss:
	mov.l	r0, @-r3
  l2:
	cmp/hi	r2, r3
	bt	clrbss

  l_noinit:
#if TA_FPU
	mov.l	r4, @-r15
	sts.l	pr, @-r15

	/* fpscr_values[] initialize */
	mov.l	SET_FPSCR, r1
	mov.l	INIT_FPSCR, r4
	jsr	@r1
	nop
	lds	r3, fpscr		// fpscr_values[1] -> fpscr

	lds.l	@r15+, pr
	mov.l	@r15+, r4
#endif /* TA_FPU */

	mov.l	L_Startup, r0
	jmp	@r0
	nop

		.balign	4
DATA_ORG:	.long	Csym(__data_org)
DATA_START:	.long	Csym(__data_start)
DATA_END:	.long	Csym(_edata)
BSS_START:	.long	Csym(__bss_start)
BSS_END:	.long	Csym(_end)
L_Startup:	.long	Csym(_P_startup)
#if TA_FPU
SET_FPSCR:	.long	Csym(__set_fpscr)
INIT_FPSCR:	.long	0x000c0000		/* FPU register initial value for task exception handler */
#endif /* TA_FPU */
