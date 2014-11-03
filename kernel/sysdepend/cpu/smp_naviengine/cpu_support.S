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
 *	cpu_support.S (NAVIENGINE)
 *	Device-Dependent CPU Operation
 */

#define	_in_asm_source_

#include <machine.h>
#include <tk/errno.h>
#include <tk/sysdef.h>
#include <tk/asm.h>
#include <sys/sysinfo.h>
#include <sys/smp_def.h>

#include "config.h"
#include "cpu_conf.h"
#include "isysconf.h"
#include "tkdev_conf.h"
#include "offset.h"

/* ------------------------------------------------------------------------ */
/*
 * Dispatcher
 *	dispatch_to_schedtsk:
 *		Throw away the current contexts and forcibly dispatch to
 *		'schedtsk.'
 *		Called directly by jump (bx) but do not return.
 *		Called on the undefined stack state (undefined 'ssp').
 *		Called on the interrupt disable state.
 *	dispatch_entry:
 *		Normal dispatch processing. Called by 'swi 8.'
 *	_ret_int_dispatch:
 *		Called when dispatch is required by 'tk_ret_int().'
 *
 *	Contexts to save
 *	Save registers except for ssp(R13_svc) to a stack. Save 'ssp' to TCB.
 *
 *		+---------------+
 *	ssp ->	| R0 - R11	|
 *		| taskmode	|
 *		| R13_usr = usp	| Available only for RNG 1-3
 *		| R14_usr	|
 *		+---------------+
 *		| R14_svc	| R14_svc before interrupt
 *		|		| (Available only for tk_ret_int)
 *		+---------------+			-
 *		| SPSR_svc	|			| Save by interrupt 
 *		| R12     = ip	|			| entry routine
 *		| R14_svc = lr	| Return address (pc)	|
 *		+---------------+			-
 */

	/* Temporal stack used when 'dispatch_to_schedtsk' is called */

#define	TMP_STACK_SZ	(4*1024)
#define TMP_STACK_SFT	(12)
#define	TMP_STACK_TOP	(tmp_stack + TMP_STACK_SZ)
	.lcomm	tmp_stack, TMP_STACK_SZ*MAX_PROC

	.text
	.balign	4
	.globl	Csym(dispatch_to_schedtsk)
	.globl	Csym(dispatch_entry)

Csym(dispatch_to_schedtsk):
	/* During SVC mode/interrupt disable CPSR.I=1 F=1 */

	mrc	p15, 0, lr, c0, c0, 5		// cpuid
	and	lr, lr, #3			//

	ldr	sp, =TMP_STACK_TOP		//
	add	sp, sp, lr, lsl #TMP_STACK_SFT	// sp = TMP_STACK[cpuid]

	ldr	r4, =Csym(ctxtsk)		//
	add	r4, r4, lr, lsl #2		//
#if USE_DBGSPT
	ldr	r8, [r4]			// r8 = ctxtsk (this register must be r8, use dbgspt)
#endif

	mov	r0, lr				//
	bl	Csym(into_dsp)			// into_dsp(pid);

	msr	cpsr_c, #PSR_SVC		// Interrupt enable
	b	l_dispatch0

Csym(dispatch_entry):
	/* During SVC mode/interrupt disable CPSR.I=1 F=1 */
	ldr	ip, [sp]
	bic	ip, ip, #PSR_DI
	str	ip, [sp]			// SPSR_svc compensation

	stmfd	sp!, {lr}			// Context save (R14_svc)
						// The contents are invalid due to number adjustment
_ret_int_dispatch:
	/* During SVC mode/interrupt disable CPSR.I=1 F=1 */
	/* ip, lr usable */
	mrc	p15, 0, lr, c0, c0, 5		// cpuid
	and	lr, lr, #3			//
	ldr	ip, =TASKMODE			//
	add	ip, ip, lr, lsl #2		//
	ldr	ip, [ip]			// ip = taskmode[cpuid]
	sub	sp, sp, #15*4			//
	stmia	sp, {r0-r11, ip, sp, lr}^	// Context save

	ldr	r4, =Csym(ctxtsk)		//
	add	r4, r4, lr, lsl #2		//
	ldr	r8, [r4]			// r8 = ctxtsk (this register must be r8, use dbgspt)
	str	sp, [r8, #TCB_tskctxb + CTXB_ssp] // Save 'ssp' to TCB

	ldr	sp, =TMP_STACK_TOP		//
	add	sp, sp, lr, lsl #TMP_STACK_SFT	// sp = TMP_STACK[cpuid]

	mov	r0, lr				//
	bl	Csym(into_dsp)			// into_dsp(pid);

	msr	cpsr_c, #PSR_SVC		// Interrupt enable

  l_dispatch0:
	/* During interrupt enable CPSR.I=0 F=0 */
#if USE_DBGSPT
	ldr	ip, =hook_stop_jmp		// Hook processing
	ldr	pc, [ip]
  ret_hook_stop:
#endif

	mrc	p15, 0, r9, c0, c0, 5		// cpuid
	and	r9, r9, #3			//
	ldr	r5, =Csym(schedtsk)		// R5 = &schedtsk
	add	r5, r5, r9, lsl #2		//
	ldr	r6, =Csym(lowpow_discnt)	// R6 = &lowpow_discnt

  l_dispatch1:
	msr	cpsr_c, #PSR_SVC|PSR_DI		// Interrupt disable

	ldr	r8, [r5]			// R8 = schedtsk (this register must be r8, use dbgspt)
	cmp	r8, #0				// Is there 'schedtsk'?
	beq	l_lowpow

	mov	r0, r9				//
	bl 	Csym(chk_restore)		// chk_restore(pid);
	cmp	r0, #0				// Am I dispatch ?
	bne	l_dispatch2			//   yes

	/* Because there is no task that should be executed, move to the power-saving mode */
  l_lowpow:
	ldr	ip, [r6]			// Is 'low_pow' disabled?
	cmp	ip, #0
	bleq	Csym(low_pow)			// call low_pow()

	msr	cpsr_c, #PSR_SVC		// Interrupt enable
	b	l_dispatch1

  l_dispatch2:					// Switch to 'schedtsk'
	/* During interrupt disable CPSR.I=1 F=1 */
	mov	r8, r0				// R8 = schedtsk (this register must be r8, use dbgspt)
	ldr	sp, [r8, #TCB_tskctxb + CTXB_ssp] // Restore 'ssp' from TCB

	/* Switch task eigenspace */
	mov	r0, #0
	ldr	r1, [r8, #TCB_tskctxb + CTXB_uatb]	// uatb
	cmp	r1, r0
	beq	l_nochg_uatb			// uatb == 0, no change

	mcr	p15, 0, r0, cr7, c10, 4		// Drain Write Buffer
	mcr	p15, 0, r1, cr2, c0, 0		// uatb --> PageTableBase 0
	ldr	r1, [r8, #TCB_tskctxb + CTXB_lsid]
	mcr	p15, 0, r1, cr13, c0, 1		// ASID = lsid

  l_nochg_uatb:

#if USE_DBGSPT
	ldr	ip, =hook_exec_jmp		// Hook processing
	ldr	pc, [ip]
  ret_hook_exec:
#endif

	ldr	ip, [r8, #TCB_reqdct]		// DCT request
	cmp	ip, #1

	ldmia	sp, {r0-r11, ip, sp, lr}^	// Context restore 
	nop
	add	sp, sp, #15*4

	stmfd	sp!, {r0}			//
	mrc	p15, 0, r0, c0, c0, 5		// cpuid
	and	r0, r0, #3			//
	ldr	lr, =TASKMODE			//
	add	lr, lr, r0, lsl #2		// taskmode[cpuid]
	ldmfd	sp!, {r0}			//

	str	ip, [lr]
	movne	ip, #0				// If there is no DCT request ip = 0

	ldmfd	sp!, {lr}			// R14_svc restore

	ands	ip, ip, #TMF_CPL(3)		// If the protected level is 0, DCT disable
	bne	Csym(dct_startup)		// To DCT processing

	EXC_RETURN


#if USE_DBGSPT
/*
 * Task dispatcher hook routine call
 *	void stop( ID tskid, INT lsid, UINT tskstat )
 *	void exec( ID tskid, INT lsid )
 */
	.text
	.balign	4
hook_stop:
	cmp	r8, #0			// r8 = ctxtsk
	beq	l_notask

	ldrb	r2, [r8, #TCB_state]			// tskstat
	mov	r2, r2, lsl #1
	ldr	r1, [r8, #TCB_tskctxb + CTXB_lsid]	// lsid
	ldr	r0, [r8, #TCB_tskid]			// tskid

	ldr	ip, =Csym(hook_stopfn)
	ldr	ip, [ip]
	mov	lr, pc
	bx	ip			// call stop(tskid, lsid, tskstat)

  l_notask:
	b	ret_hook_stop

hook_exec:
					// r8 = ctxtsk
	ldr	r1, [r8, #TCB_tskctxb + CTXB_lsid]	// lsid
	ldr	r0, [r8, #TCB_tskid]			// tskid

	ldr	ip, =Csym(hook_execfn)
	ldr	ip, [ip]
	mov	lr, pc
	bx	ip			// call exec(tskid, lsid)

	b	ret_hook_exec

/*
 * Set/Free task dispatcher hook routine
 */
	.text
	.balign	4
	.globl	Csym(hook_dsp)
Csym(hook_dsp):
	ldr	r0, =hook_exec_jmp
	ldr	r1, =hook_stop_jmp
	ldr	r2, =hook_exec
	ldr	r3, =hook_stop
	str	r2, [r0]
	str	r3, [r1]
	bx	lr

	.globl	Csym(unhook_dsp)
Csym(unhook_dsp):
	ldr	r0, =hook_exec_jmp
	ldr	r1, =hook_stop_jmp
	ldr	r2, =ret_hook_exec
	ldr	r3, =ret_hook_stop
	str	r2, [r0]
	str	r3, [r1]
	bx	lr

			.data
			.balign	4
  hook_exec_jmp:	.long	ret_hook_exec
  hook_stop_jmp:	.long	ret_hook_stop

#endif /* USE_DBGSPT */

/* ------------------------------------------------------------------------ */
/*
 * High level programming language routine for interrupt handler
 *	Called by interrupt entry routine on the state saved in the interrupt 
 *	stack as shown below.
 *		+---------------+
 *	ssp ->	| R3		| Only FIQ, IRQ
 *		+---------------+
 *	ssp ->	| SPSR		|
 *		| R12 = ip	|
 *		| R14 = lr	|
 *		+---------------+
 *
 *	The vector table address is set in 'ip.'
 *	(ip - EIT_VECTBL) / 4 = Vector number
 */
	.text
	.balign	4
	.globl	Csym(defaulthdr_startup)
	.globl	Csym(inthdr_startup)
	.globl	Csym(exchdr_startup)
Csym(defaulthdr_startup):
	/* Unknown mode/During interrupt disable CPSR.I=1 F=? */
	mrs	lr, cpsr
	and	lr, lr, #PSR_M(31)
	cmp	lr, #PSR_FIQ
	cmpne	lr, #PSR_IRQ
	stmnefd	sp!, {r3}		// Register save
	stmfd	sp!, {r0-r2}

	ldr	r3, =EIT_VECTBL
	sub	r3, ip, r3
					// Argument of handler
	mov	r0, r3, lsr #2		// r0 = dintno
	add	r1, sp, #4*4		// r1 = sp

	ldr	r3, =EIT_DEFAULT * 4	// r3 = Vector table offset
	b	l_inthdr2

Csym(exchdr_startup):
	/* Unknown mode/During interrupt disable CPSR.I=1 F=? */
	stmfd	sp!, {r3}		// Register save
	b	l_inthdr1

Csym(inthdr_startup):
	/* Unknown mode/During interrupt disable CPSR.I=1 F=? */
	mrs	lr, cpsr
	and	lr, lr, #PSR_M(31)
	cmp	lr, #PSR_SVC
	stmeqfd	sp!, {r3}		// If it is SWI, also save 'r3'
  l_inthdr1:
	stmfd	sp!, {r0-r2}		// Register save

	ldr	r3, =EIT_VECTBL
	sub	r3, ip, r3		// r3 = Vector table offset

					// Argument of handler
	mov	r0, r3, lsr #2		// r0 = dintno
	add	r1, sp, #4*4		// r1 = sp

  l_inthdr2:
	mrs	r2, cpsr		// r2 = CPSR save
	msr	cpsr_c, #PSR_SVC|PSR_DI	// Move to SVC mode/Interrupt disable

	stmfd	sp!, {r2, lr}		// SVC mode register save
	stmfd	sp!, {ip}		// In normal situation, save only for FIQ

	mrc	p15, 0, lr, c0, c0, 5	// cpuid(*)
	and	lr, lr, #3		// mask (*)
	ldr	ip, =TASKINDP		// Task independent part
	add	ip, ip, lr, lsl #2	// taskindependent[cpuid]
	ldr	lr, [ip]
	add	lr, lr, #1
	str	lr, [ip]

#if USE_DBGSPT
	ldr	ip, =hook_ienter_jmp
	ldr	pc, [ip]
  ret_hook_ienter:
#endif

	ldr	ip, =Csym(hll_inthdr)
	ldr	ip, [ip, r3]
	mov	lr, pc
	bx	ip			// call hll_inthdr[n](dintno, sp)

#if USE_DBGSPT
	ldr	ip, =hook_ileave_jmp
	ldr	pc, [ip]
  ret_hook_ileave:
#endif

	ldmfd	sp!, {ip}		// SVC mode register restore
	ldmfd	sp!, {r2, r3}		// r2 = Original mode
					// r3 = R14_svc
	orr	r2, r2, #PSR_DI
	msr	cpsr_c, r2		// Return to original mode/Interrupt disable

	mrc	p15, 0, lr, c0, c0, 5	// cpuid(*)
	and 	lr, lr, #3		// mask (*)
	ldr	ip, =TASKINDP
	add	ip, ip, lr, lsl #2	// taskindependent[cpuid] (*)
	ldr	lr, [ip]
	sub	lr, lr, #1
	str	lr, [ip]

	ldmfd	sp!, {r0-r2}		// Register restore
	swp	r3, r3, [sp]		// r3 restore, R14_svc save
	swi	SWI_RETINT		// tk_ret_int()


#if USE_DBGSPT
/*
 * Interrupt handler hook routine call
 */
	.text
	.balign	4
hook_ienter:
	stmfd	sp!, {r0-r2}		// Register save
	stmfd	sp!, {r3}

	ldr	ip, =Csym(hook_ienterfn)
	ldr	ip, [ip]
	mov	lr, pc
	bx	ip			// call enter(dintno, sp)

	ldmfd	sp!, {r3}		// Register restore
	ldmfd	sp, {r0-r2}		// Leave 'dintno,' 'sp' on stack
	b	ret_hook_ienter

hook_ileave:
	ldmfd	sp!, {r0-r2}		// Restore 'dintno,' 'sp' 

	ldr	ip, =Csym(hook_ileavefn)
	ldr	ip, [ip]
	mov	lr, pc
	bx	ip			// call leave(dintno, info)

	b	ret_hook_ileave

/*
 * Set/Free interrupt handler hook routine
 */
	.text
	.balign	4
	.globl	Csym(hook_int)
Csym(hook_int):
	ldr	r0, =hook_ienter_jmp
	ldr	r1, =hook_ileave_jmp
	ldr	r2, =hook_ienter
	ldr	r3, =hook_ileave
	str	r2, [r0]
	str	r3, [r1]
	bx	lr

	.globl	Csym(unhook_int)
Csym(unhook_int):
	ldr	r0, =hook_ienter_jmp
	ldr	r1, =hook_ileave_jmp
	ldr	r2, =ret_hook_ienter
	ldr	r3, =ret_hook_ileave
	str	r2, [r0]
	str	r3, [r1]
	bx	lr

			.data
			.balign	4
  hook_ienter_jmp:	.long	ret_hook_ienter
  hook_ileave_jmp:	.long	ret_hook_ileave

#endif /* USE_DBGSPT */

/*
 * tk_ret_int() processing
 *	When called, the interrupt stack is configured as shown below.
 *		+---------------+
 *	ssp ->	| SPSR_svc	| Save by 'swi SWI_RETINT'
 *		| R12_usr	|
 *		| R14_svc	|
 *		+---------------+
 *
 *		+---------------+
 *	isp ->	| R14_svc	| Save when calling 'tk_ret_int'
 *		+---------------+
 *		| SPSR_xxx	| Save when the interrupt occurs
 *		| R12_xxx	|
 *		| R14_xxx	| <- Return address
 *		+---------------+
 */
	.text
	.balign	4
	.globl	Csym(_tk_ret_int)
Csym(_tk_ret_int):
	ldr	ip, [sp]		// ip = SPSR

	and	lr, ip, #PSR_M(31)
	cmp	lr, #PSR_SVC
	beq	l_retint_svc		// Is it 'tk_ret_int' from SWI?

	stmfd	sp!, {r2, r3}		// Save 'r2' for work (Save 'r3' for acquiring location)
	add	r2, sp, #4

	orr	ip, ip, #PSR_DI
	bic	ip, ip, #PSR_T
	cmp	lr, #PSR_FIQ
	msr	cpsr_c, ip		// Move to interrupted mode/Interrupt disable

	ldmfd	sp!, {ip, lr}		// Copy from 'isp' to 'ssp'/Free 'isp'
	str	ip, [r2, #0*4]		// R14_svc
	str	lr, [r2, #1*4]		// SPSR_xxx
	ldmfd	sp!, {ip, lr}
	strne	ip, [r2, #2*4]		// R12_xxx (except for FIQ)
	str	lr, [r2, #3*4]		// R14_xxx (Return address)

	msr	cpsr_c, #PSR_SVC|PSR_DI	// Move to SVC mode/Interrupt disable

	ldmfd	sp!, {r2}		// r2 restore
	b	l_retint1

  l_retint_svc:
	add	sp, sp, #3*4		// Trash parts saved by 'swi SWI_RETINT'

	msr	cpsr_c, #PSR_SVC|PSR_DI	// Interrupt disable

  l_retint1:
  	/* lr, ip usable */
  	stmfd	sp!, {r0-r3}		// register save for C laungage
	ldr	r0, [sp, #4+4*4]	// SPSR
	bl	Csym(chk_dsp)		// check dispatch
	cmp	r0, #2			// Is there DCT request ?
	beq	_ret_int_dct_startup    //   yes, To DCT processing
	cmp	r0, #1			// Am I dispatch ?
  	ldmfd	sp!, {r0-r3}		// register store for C laungage
	beq	_ret_int_dispatch	//   yes, To dispatch processing

  l_nodispatch:
	ldmfd	sp!, {lr}		// lr restore 
	EXC_RETURN


/* ------------------------------------------------------------------------ */

/*
 * Unsupported system call
 */
	.text
	.balign	4
	.globl	Csym(no_support)
Csym(no_support):
	ldr	r0, =E_RSFN
	bx	lr

/*
 * System call entry table
 */
	.text
	.balign	4
_svctbl:
	.int	Csym(no_support)
#define	_tk_ret_int	no_support
#include <sys/svc/tksvctbl.h>
#undef	_tk_ret_int

/*
 * System call entry
 *	Do not need to save the temporary register.
 *	The compiler saves the permanent register.
 *		+---------------+
 *	ssp ->	| SPSR		|
 *		| ip		| Function code
 *		| lr		| Return address
 *		+---------------+
 */
	.text
	.balign	4
	.globl	Csym(call_entry)
Csym(call_entry):
	/* SVC mode/During interrupt disable CPSR.I=1 F=? */
	stmfd	sp!, {r9-r10, fp}	// Save register for work
	add	fp, sp, #3*4

	mrc	p15, 0, r9, c0, c0, 5	// cpuid(*)
	and 	r9, r9, #3		// mask (*)
	ldr	ip, =TASKMODE		// Task mode flag update
	add	ip, ip, r9, lsl #2	// taskmode[cpuid]
	ldr	r10, [ip]
	stmfd	sp!, {r10}		// taskmode save 
	mov	lr, r10, lsl #16
	str	lr, [ip]

#if USE_DBGSPT
	ldr	ip, =hook_enter_jmp	// Hook processing
	ldr	pc, [ip]
  ret_hook_enter:
#endif

	ldr	lr, [fp, #4]		// lr = Function code
	cmp	lr, #0			//	< 0: System call
	bge	l_esvc_function		//	>= 0: Extended SVC

	/* T-Kernel system call */
	ldr	ip, =TASKINDP		// If it is a call from the task independent part,
	add	ip, ip, r9, lsl #2	// taskindependent[cpuid]
	ldr	ip, [ip]		// protected level check is not necessary.
	cmp	ip, #0
	bhi	l_nochklevel

	ldr	ip, =Csym(svc_call_limit)
	ldr	ip, [ip]		// Limit of protected level
	and	r10, r10, #TMF_CPL(3)	// r10 = taskmode when calling
	cmp	r10, ip
	bhi	l_oacv_err
  l_nochklevel:

	mov	r10, lr, asr #16	// r10 = Function number
	ldr	ip, =N_TFN + 0xffff8000
	cmp	r10, ip
	bgt	l_illegal_svc

	mov	lr, lr, lsr #8
	and	lr, lr, #0xff		// lr = Number of arguments
	cmp	lr, #5
	bne	l_nocopy
	ldr	ip, [r4]		// Copy fifth argument
	stmfd	sp!, {ip}
  l_nocopy:

	ldr	ip, [fp]
	and	ip, ip, #PSR_DI
	orr	ip, ip, #PSR_SVC
	msr	cpsr_c, ip		// Return interrupt disable state to caller's state

	ldr	ip, =_svctbl - (0xffff8000 << 2)
	mov	lr, pc
	ldr	pc, [ip, r10, lsl #2]	// T-Kernel system call

  l_retsvc:
#if USE_DBGSPT
	ldr	ip, =hook_leave_jmp	// Hook processing
	ldr	pc, [ip]
  ret_hook_leave:
#endif

	sub	sp, fp, #4*4
	ldmfd	sp!, {r1, r9-r10}	// Restore register for work "except fp"

	msr	cpsr_c, #PSR_SVC|PSR_DI	// Interrupt disable

	mrc	p15, 0, fp, c0, c0, 5	// cpuid(*)
	and	fp, fp, #3		// mask (*)
	ldr	ip, =TASKMODE		// Task mode restore
	add	ip, ip, fp, lsl #2	// taskmode[cpuid] (*)
	str	r1, [ip]

	ands	r1, r1, #TMF_CPL(3)	// If it is not a call from protected level 0
	beq	l_nodct			// Do not process DCT

	ldr	ip, =TASKINDP		// If it is a call from the task independent part
	add	ip, ip, fp, lsl #2	// taskindependent[cpuid]

	ldr	ip, [ip]		// Do not process DCT
	cmp	ip, #0
	bne	l_nodct

	ldr	ip, =Csym(ctxtsk)	// Is there a DCT request?
	add	ip, ip, fp, lsl #2	// ctxtsk[cpuid](*)
	ldr	ip, [ip]
	ldr	ip, [ip, #TCB_reqdct]
	cmp	ip, #1
	bne	l_nodct

	ldr	ip, =Csym(dispatch_disabled)
	add	ip, ip, fp, lsl #2	// (*)
	ldr	ip, [ip]		// During dispatch disable
	cmp	ip, #0			// Do not process DCT
	bne	l_nodct

	ldmfd	sp!, {fp}		// Restore register for work "fp"
	b	Csym(dct_startup)	// To DCT processing
	EXC_RETURN

  l_nodct:
	ldmfd	sp!, {fp}		// Restore register for work "fp"
	EXC_RETURN


  l_esvc_function:
	/* Extended SVC */
	ldr	ip, [fp]
	and	ip, ip, #PSR_DI
	orr	ip, ip, #PSR_SVC
	msr	cpsr_c, ip		// Return interrupt disable state to caller's state

	mov	r1, lr			// r1 = Function code
	bl	Csym(svc_ientry)	// svc_ientry(pk_para, fncd)

	b	l_retsvc


  l_illegal_svc:
	ldr	r0, =E_RSFN
	b	l_retsvc

  l_oacv_err:
	ldr	r0, =E_OACV
	b	l_retsvc


#if USE_DBGSPT

/*
 * System call/Extended SVC hook routine call
 *	VP enter( FN fncd, TD_CALINF *calinf, ... )
 *	void leave( FN fncd, INT ret, VP exinf )
 *
 *	typedef struct td_calinf {
 *		VP	ssp;	System stack pointer
 *		VP	r11;	Flame pointer when calling
 *	} TD_CALINF;
 */
	.text
	.balign	4
hook_enter:
	stmfd	sp!, {r0-r3, r8-r9}	// Save argument and register for work
	mov	r8, sp			// r8 = Keep stack position

	ldr	ip, [fp, #-4]		// Flame pointer when calling
	stmfd	sp!, {fp, ip}		// Create TD_CALINF
	mov	r9, sp			// r9 = &TD_CALINF

	ldr	lr, [fp, #4]		// lr = Function code
	cmp	lr, #0			//	< 0: System call
	bge	l_hooksvc		//	>= 0: Extended SVC

	mov	lr, lr, lsr #8
	and	lr, lr, #0xff		// Number of arguments

	cmp	lr, #5
	ldrge	ip, [r4]
	stmgefd	sp!, {ip}		// Fifth argument
	cmp	lr, #4
	stmgefd	sp!, {r3}		// Fourth argument
	cmp	lr, #3
	stmgefd	sp!, {r2}		// Third argument
	mov	r3, r1			// Second argument
  l_hooksvc:
	mov	r2, r0			// First argument
	mov	r1, r9			// calinf
	ldr	r0, [fp, #4]		// fncd
	ldr	ip, =Csym(hook_enterfn)
	ldr	ip, [ip]
	mov	lr, pc
	bx	ip			// exinf = enter(fncd, ...)
	mov	r9, r0			// Temporarily save 'exinf'

	mov	sp, r8			// Return stack position
	ldmfd	sp!, {r0-r3, r8}	// Restore argument and register for work
	swp	r9, r9, [sp]		// Restore 'r9' and save 'exinf' in it
	b	ret_hook_enter

hook_leave:
	mov	r1, r0			// r1 = ret

	mov	lr, #0
	ldr	r0, [fp, #4]		// r0 = Function code
	cmp	r0, #0			//	< 0: System call
	bge	l_hooksvc2		//	>= 0: Extended SVC

	mov	lr, r0, lsr #8
	and	lr, lr, #0xff		// Number of arguments
	subs	lr, lr, #4
	movlt	lr, #0
  l_hooksvc2:

	add	lr, lr, #3		// Whether 'hook_enter' is executed
	sub	ip, sp, fp		// Check by stack usage
	sub	sp, fp, #5*4		// Location in which 'sp = exinf' is saved
	cmp	lr, ip, lsr #2		// If 'hook_enter' is executed,
	ldrne	r2, [sp]		// Get 'exinf' from stack
	ldreq	r2, =0			// If 'exinf' is not saved, 0

	str	r9, [sp]		// r9 save
	mov	r9, r1			// Save 'ret' in 'r9'

	ldr	ip, =Csym(hook_leavefn)
	ldr	ip, [ip]
	mov	lr, pc
	bx	ip			// call leave(fncd, ret, exinf)

	mov	r0, r9			// r0 = ret restore
	ldmfd	sp!, {r9}		// r9 restore
	b	ret_hook_leave

/*
 * Set/Free system call/extended SVC hook routine 
 */
	.text
	.balign	4
	.globl	Csym(hook_svc)
Csym(hook_svc):
	ldr	r0, =hook_enter_jmp
	ldr	r1, =hook_leave_jmp
	ldr	r2, =hook_enter
	ldr	r3, =hook_leave
	str	r2, [r0]
	str	r3, [r1]
	bx	lr

	.globl	Csym(unhook_svc)
Csym(unhook_svc):
	ldr	r0, =hook_enter_jmp
	ldr	r1, =hook_leave_jmp
	ldr	r2, =ret_hook_enter
	ldr	r3, =ret_hook_leave
	str	r2, [r0]
	str	r3, [r1]
	bx	lr

			.data
			.balign	4
  hook_enter_jmp:	.long	ret_hook_enter
  hook_leave_jmp:	.long	ret_hook_leave

#endif /* USE_DBGSPT */

/* ------------------------------------------------------------------------ */

#if USE_DBGSPT
/*
 * Debugger support function service call entry table
 */
	.text
	.balign	4
_tdsvctbl:
	.int	Csym(no_support)
#include <sys/svc/tdsvctbl.h>

/*
 * Debugger support function service call entry
 */
	.text
	.balign	4
	.globl	Csym(call_dbgspt)
Csym(call_dbgspt):
	/* SVC mode/During interrupt disable CPSR.I=1 F=? */
	ldr	ip, [sp]
	and	ip, ip, #PSR_I|PSR_F
	orr	ip, ip, #PSR_SVC
	msr	cpsr_c, ip		// Return interrupt disable state to caller's state

	stmfd	sp!, {r10, fp}		// Save register for work
	add	fp, sp, #2*4

	mrc	p15, 0, r10, c0, c0, 5	// cpuid(*)
	and	r10, r10, #3		// mask (*)
	ldr	ip, =TASKMODE
	add	ip, ip, r10, lsl #2	// taskmode[cpuid] (*)
	ldr	lr, =Csym(svc_call_limit)
	ldr	ip, [ip]
	ldr	lr, [lr]		// Call protected level limit
	and	ip, ip, #TMF_CPL(3)	// Protected level when calling
	cmp	ip, lr
	bhi	b_oacv_err

	ldr	lr, [fp, #4]		// lr = Function code
	mov	r10, lr, asr #16
	ldr	ip, =N_TDFN + 0xffff8000
	cmp	r10, ip
	bgt	b_illegal_svc

	ldr	ip, =_tdsvctbl - (0xffff8000 << 2)
	mov	lr, pc
	ldr	pc, [ip, r10, lsl #2]	// T-Kernel/DS service call

  b_retsvc:
	msr	cpsr_c, #PSR_SVC|PSR_DI	// disable interrupt
	ldmfd	sp!, {r10, fp}		// Restore register for work
	EXC_RETURN


  b_illegal_svc:
	ldr	r0, =E_RSFN
	b	b_retsvc

  b_oacv_err:
	ldr	r0, =E_OACV
	b	b_retsvc

#endif /* USE_DBGSPT */

/* ------------------------------------------------------------------------ */
/*
 * High level programming language routine for timer handler
 */
	.text
	.balign	4
	.globl	Csym(timer_handler_startup)
Csym(timer_handler_startup):
	/* IRQ mode/During interrupt disable CPSR.I=1 F=? */
	msr	cpsr_c, #PSR_SVC|PSR_DI	// Move to SVC mode/Interrupt disable

	stmfd	sp!, {r0-r2, r4-r5, lr}	// Save register

	mrc	p15, 0, r5, c0, c0, 5	// cpuid(*)
	and 	r5, r5, #3		// mask (*)
	ldr	r4, =TASKINDP		// Enter task independent part
	add	r4, r4, r5, lsl #2	// task_independent[cpuid]
	ldr	r5, [r4]
	add	r0, r5, #1
	str	r0, [r4]

	bl	Csym(timer_handler)	// call timer_handler()
	/* Return by interrupt disable CPSR.I=1 F=1 */

	str	r5, [r4]		// Leave task independent part

	ldmfd	sp!, {r0-r2, r4-r5, lr}	// Restore register

	TK_RET_INT_FIQ PSR_IRQ		// tk_ret_int()

/* ------------------------------------------------------------------------ */
/*
 * Delayed context trap (DCT)
 *	Task exception handler startup
 *	When called, the interrupt stack is configured as shown below.
 *		+---------------+
 *	ssp ->	| SPSR_svc	|
 *		| R12     = ip	|
 *		| R14_svc = lr	| Return address (pc)
 *		+---------------+
 */
	.text
	.balign	4
	.globl	Csym(dct_startup)
Csym(dct_startup):
	/* SVC mode/During interrupt disable CPSR.I=1 F=1 */
	stmfd	sp!, {r0-r3, lr}	// Register save

_ret_int_dct_startup:
	ldr	ip, [sp, #5*4]		// spsr
	and	ip, ip, #PSR_M(31)	// return processor mode
	cmp	ip, #PSR_USR		// when neither user mode or
	cmpne	ip, #PSR_SYS		// system mode,
	bne	l_nodct_startup		// don't execute DCT processing

	add	r0, sp, #5*4
	bl	Csym(setup_texhdr)	// call setup_texhdr(ssp)
	/* Return in interrupt enable state */
	msr	cpsr_c, #PSR_SVC|PSR_DI	// disable interrupt

  l_nodct_startup:
	ldmfd	sp!, {r0-r3, lr}	// Register restore
	EXC_RETURN

/* ------------------------------------------------------------------------ */
