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
 *	@(#)asm_depend.h (tk/NAVIENGINE)
 *
 *	Assembler Macro for NAVIENGINE
 */

#ifndef __TK_ASM_DEPEND_H__
#define __TK_ASM_DEPEND_H__

#define base(n)		( (n) & 0xfffff000 )
#define offs(n)		( (n) & 0x00000fff )

/* ------------------------------------------------------------------------ */
/*
 *	Exception restore
 */

/*
 * FIQ support by OS
 */
#define _FIQ_SupportedByOS_	0	/* unsupport */

/*
 * Interrupt (IRQ)/fast interrupt (FIQ) return
 */
 .macro INT_RETURN
	.arm
	ldmfd	sp!, {r3, ip}
	msr	spsr_fsxc, ip
	ldmfd	sp!, {ip, pc}^
 .endm

/*
 * Exception return
 */
 .macro EXC_RETURN
	.arm
	ldmfd	sp!, {ip}
	msr	spsr_fsxc, ip
	ldmfd	sp!, {ip, pc}^
 .endm

/*
 * Exception/Interrupt return
 */
 .macro EIT_RETURN
	.arm
	mrs	ip, cpsr
	and	ip, ip, #PSR_M(31)
	cmp	ip, #PSR_IRQ
#if _FIQ_SupportedByOS_
	cmpne	ip, #PSR_FIQ
#endif
	ldmeqfd	sp!, {r3, ip}		// FIQ, IRQ
	ldmnefd	sp!, {ip}		// Others
	msr	spsr_fsxc, ip
	ldmfd	sp!, {ip, pc}^
 .endm

/* ------------------------------------------------------------------------ */
/*
 *	tk_ret_int() system call
 */

/*
 * Transit to SVC mode
 */
 .macro ENTER_SVC_MODE
	.arm
	mrs	ip, cpsr
	bic	ip, ip, #PSR_M(31)
	orr	ip, ip, #PSR_SVC
	msr	cpsr_c, ip		// Transit to SVC mode
 .endm

/*
 * Return from handler by tk_ret_int() 
 *	mode	Handler exception mode
 *		(Available on other modes except for FIQ)
 *	Use from SVC mode.
 *
 *	State of handler exception mode stack at Macro call
 *		+---------------+
 *	sp  ->	|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		|
 *		+---------------+
 */
 .macro TK_RET_INT mode
	.arm
	mov	ip, lr			// ip = lr_svc
	msr	cpsr_c, #PSR_DI|\mode	// Return to previous exception mode
	stmfd	sp!, {ip}		// Save lr_svc
	swi	SWI_RETINT
 .endm

/*
 * Return from handler by tk_ret_int()
 *	mode	Handler exception mode (Available on all modes including FIQ)
 *
 *	Use from SVC mode.
 *
 *	State of handler exception mode stack at Macro call
 *		+---------------+
 *	sp  ->	|R3		|
 *		|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		|
 *		+---------------+
 */
 .macro TK_RET_INT_FIQ mode
	.arm
	mov	r3, lr				// r3 = lr_svc
	msr	cpsr_c, #PSR_I|PSR_F|\mode	// Return to previous exception mode
	swp	r3, r3, [sp]			// Save lr_svc and restore r3
	swi	SWI_RETINT
 .endm

/* ------------------------------------------------------------------------ */
/*
 *	Task exception handler entry
 *
 *		+---------------+
 *	sp  ->	|CPSR		| Restore when returning from handler  CPSR
 *		|texcd		| Exception code
 *		|PC		| Return address from handler
 *		+---------------+
 */

 .macro TEXHDR_ENTRY texhdr
	.arm
	stmfd	sp!, {lr}		// lr save 
	add	lr, sp, #2*4
	swp	r0, r0, [lr]		// r0 save, r0 = texcd

	stmfd	sp!, {r1-r3, ip}	// Other register saves

	ldr	ip, =\texhdr
	mov	lr, pc
	bx	ip			// call texhdr(texcd)

	ldmfd	sp!, {r1-r3, ip, lr}	// Register restore

	ldmfd	sp!, {r0}		// r0 = CPSR
	tst	r0, #PSR_T
	bne	thumb_ret_\texhdr

	msr	cpsr_f, r0
	ldmfd	sp!, {r0, pc}		// Return to ARM instruction

  thumb_ret_\texhdr:
	msr	cpsr_f, r0
	add	r0, pc, #1
	bx	r0
	.thumb
	pop	{r0, pc}		// Return to Thumb instruction
	.arm
 .endm

/* ------------------------------------------------------------------------ */
#endif /* __TK_ASM_DEPEND_H__ */
