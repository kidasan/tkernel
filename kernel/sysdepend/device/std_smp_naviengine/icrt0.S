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
 *	icrt0.S (NAVIENGINE)
 *	System Startup
 */

#define	_in_asm_source_

#include <machine.h>
#include <tk/asm.h>
#include <sys/sysinfo.h>

#if _Csym == 0
#define	START	_start
#else
#define	START	start
#endif

	/* Low level memory manager information */
	.comm	Csym(lowmem_top), 4	// Head of area (Low address)
	.comm	Csym(lowmem_limit), 4	// End of area (High address)

	.lcomm	monitor_stacktop, 4	// Monitor stack pointer for re-startup

	.text
	.balign	4
	.globl	START
	.type	START, %function
START:
	/* Use the stack set by the monitor */
	msr	cpsr_c, #PSR_SVC|PSR_I|PSR_F	// SVC mode/Interrupt disable

	/* MMU is already ON. The initial setting is done.
	   Only cache should be ON */
	mrc	p15, 0, r4, cr1, c0
	orr	r4, r4, #CR1_C
	orr	r4, r4, #CR1_I
	mcr	p15, 0, r4, cr1, c0
	ldr	r4, =0
	mcr	p15, 0, r4, cr7, c7, 0	// Cache flush

	// if i'm not first processor jump to main
	mrc	p15, 0, r5, c0, c0, 5
	and	r5, r5, #3
	cmp	r5, #0
	bne	skip


	ldr	r5, =__data_org		// Initialization of 'data' area (ROM startup)
	ldr	r6, =__data_start
	subs	r10, r5, r6		// If r10 != 0, start Rom
	beq	nocopy_data
	ldr	r7, =_edata
	cmp	r6, r7
	bhs	nocopy_data
  copy_data:
	ldr	r4, [r5], #4
	str	r4, [r6], #4
	cmp	r6, r7
	blo	copy_data
  nocopy_data:

	ldr	r4, =0			// Clear 'bss' area
	ldr	r5, =__bss_start
	ldr	r6, =_end
	cmp	r5, r6
	bhs	nobss
  clrbss:
	str	r4, [r5], #4
	cmp	r5, r6
	blo	clrbss
  nobss:

	ldr	ip, =monitor_stacktop	// Monitor stack pointer for re-startup
	str	sp, [ip]

	ldr	ip, =RAM_TOP		// Low level memory manager initial setting
	ldr	r5, [ip]
	cmp	r6, r5			// _end or RAM_TOP
	movhi	r5, r6			// Either of High addresses
	ldr	ip, =lowmem_top
	str	r5, [ip]		// lowmem_top = _end or RAM_TOP
	ldr	ip, =RAM_END
	ldr	r5, [ip]
	ldr	ip, =lowmem_limit
	str	r5, [ip]		// lowmem_limit = RAM_END

	cmp	r10, #0
//	blne	Csym(ROM_startup)	// Initialization at ROM startup
	bl	Csym(ROM_startup)	// Initialization at ROM startup
  skip:
	bl	Csym(main)		// System startup

  l_end:				// Not suppose to return from 'main',
	b	l_end			// but, just in case, prepare for out of control.
