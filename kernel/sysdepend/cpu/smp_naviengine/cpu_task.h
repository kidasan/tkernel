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
 *	cpu_task.h (NAVIENGINE)
 *	CPU-Dependent Task Start Processing
 */

#ifndef _CPU_TASK_
#define _CPU_TASK_

#include "cpu_insn.h"

/*
 * System stack configuration at task startup
 */
typedef struct {
	VW	r[12];		/* R0-R11 */
	UW	taskmode;
	VP	usp;		/* R13_usr */
	VP	lr_usr;		/* R14_usr */
	VP	lr_svc;		/* R14_svc */
	VW	spsr_svc;
	VW	ip;		/* R12 */
	VP	pc;		/* R15 */
} SStackFrame;

/*
 * User stack configuration at task startup (only RNG 1-3)
 */
typedef struct {
	/* Empty */
} UStackFrame;

/*
 * Size of system stack area destroyed by 'make_dormant()'
 * In other words, the size of area required to write by 'setup_context().'
 */
#define DORMANT_STACK_SIZE	( sizeof(VW) * 7 )	/* To 'taskmode' */

/*
 * Size of area kept for special use from system stack
 */
#define RESERVE_SSTACK(tskatr)	0

/*
 * Initial value for task startup
 */
#if USE_MMU
#define INIT_PSR(rng)	( ( (rng) == 0 )? PSR_SVC: \
			  ( (rng) == 3 )? PSR_USR: PSR_SYS )
#else
#define INIT_PSR(rng)	( ( (rng) == 0 )? PSR_SVC: PSR_SYS )
#endif

#define INIT_TMF(rng)	( TMF_PPL(rng) | TMF_CPL(rng) )

/*
 * Switch task space
 */
Inline void change_space( VP uatb, INT lsid )
{
	if ( uatb ) {
		Asm("mcr  p15, 0, %0, cr7, c10, 4" :: "r"(0));
		Asm("mcr  p15, 0, %0, cr2,  c0, 0" :: "r"(uatb));
		Asm("mcr  p15, 0, %0, cr13, c0, 1" :: "r"(lsid));
	}
}

/*
 * Create stack frame for task startup
 *	Call from 'make_dormant()'
 */
Inline void setup_context( TCB *tcb )
{
	SStackFrame	*ssp;
	W		rng;
	UW		pc, spsr;

	rng = (tcb->tskatr & TA_RNG3) >> 8;
	ssp = tcb->isstack;
	ssp--;

	spsr = INIT_PSR(rng);
	pc = (UW)tcb->task;
	if ( (pc & 1) != 0 ) {
		spsr |= PSR_T;		/* Thumb mode */
	}

	/* CPU context initialization */
	ssp->taskmode = INIT_TMF(rng);	/* Initial taskmode */
	ssp->spsr_svc = spsr;		/* Initial SR */
	ssp->pc = (VP)(pc & ~0x00000001U);	/* Task startup address */
	tcb->tskctxb.ssp = ssp;		/* System stack */

	if ( rng > 0 ) {
		ssp->usp = tcb->istack;	/* User stack */
	}
}

/*
 * Set task startup code
 *	Called by 'tk_sta_tsk()' processing.
 */
Inline void setup_stacd( TCB *tcb, INT stacd )
{
	SStackFrame	*ssp = tcb->tskctxb.ssp;

	ssp->r[0] = stacd;
	ssp->r[1] = (VW)tcb->exinf;
}

/*
 * Delete task contexts
 */
Inline void cleanup_context( TCB *tcb )
{
	/* Do nothing */
}

#endif /* _CPU_TASK_ */
