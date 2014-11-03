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
 *	smpcall_startup.S (naviengine)
 *
 */

#define	_in_asm_source_

#include <machine.h>
#include <tk/asm.h>
#include <sys/sysinfo.h>

/*
 * High level programming language routine for smp-call handler
 *	Called by interrupt entry routine on the state saved in the interrupt 
 *	stack as shown below.
 *		+---------------+
 *	ssp ->	| R3		|
 *		| SPSR		|
 *		| R12 = ip	|
 *		| R14 = lr	|
 *		+---------------+
 *
 *	The vector table address is set in 'ip.'
 *	(ip - EIT_VECTBL) / 4 = Vector number
 *
 *	The CPU source ID is set in 'r3.'
 *
 */
	.text
	.balign	4
	.globl	Csym(smpc_handler_startup)
Csym(smpc_handler_startup):
	/* IRQ mode/During interrupt disable CPSR.I=1 F=? */
	msr	cpsr_c, #PSR_SVC|PSR_DI	// Move to SVC mode/Interrupt disable

	stmfd	sp!, {r0-r2, r4-r5, lr}	// Save register

	ldr	r4, =TASKINDP		// Enter task independent part
	mrc	p15, 0, r5, c0, c0, 5	// cpuid(*)
	and 	r5, r5, #3		// mask (*)
	add	r4, r4, r5, lsl #2	// task_independent[cpuid]
	ldr	r5, [r4]
	add	r0, r5, #1
	str	r0, [r4]

					// Argument of handler
	mov	r0, r3			// r0 = CPU source ID
	bl	Csym(smpc_handler)	// call smpc_handler()

	str	r5, [r4]		// Leave task independent part

	ldmfd	sp!, {r0-r2, r4-r5, lr}	// Restore register

	TK_RET_INT_FIQ PSR_IRQ		// tk_ret_int()

