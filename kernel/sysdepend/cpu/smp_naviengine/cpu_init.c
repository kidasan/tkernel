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
 *	cpu_init.c (NAVIENGINE)
 *	CPU-Dependent Initialization/Finalization
 */

#include "kernel.h"
#include "task.h"
#include "cpu_insn.h"
#include <sys/kernel_util.h>
#include <sys/rominfo.h>

EXPORT MONHDR	SaveMonHdr;	/* For saving monitor exception handler */
EXPORT ATR	available_cop;	/* Available coprocessor */

EXPORT T_COUNTLOCK SpinLockObj;	/* Spin lock for critical section */

/*
 * CPU-dependent initialization
 */
EXPORT ER cpu_initialize( void )
{
	UINT	prid = get_prid();

IMPORT void FlushCache( VP laddr, INT len );	/* Cache flush */
IMPORT void dispatch_entry( void );		/* Dispatcher call */
IMPORT void call_entry( void );			/* System call */
IMPORT void _tk_ret_int( void );		/* 'tk_ret_int()' only call */
IMPORT void call_dbgspt( void );		/* Debugger support call */

	if ( prid == 0 ) {
		_tk_get_cfn(SCTAG_TKERPRNUM, (INT*)&num_proc, 1);

		/* Save monitor exception handler */
		SaveMonHdr.default_hdr = SCArea->intvec[VECNO_DEFAULT];
		SaveMonHdr.break_hdr   = SCArea->intvec[VECNO_BREAK];
		SaveMonHdr.monitor_hdr = SCArea->intvec[VECNO_MONITOR];

		/* Disable task space */
		FlushCache(NULL, 0);	/* Cache flush */
		PurgeTLB();		/* TLB disable */

		/* No available coprocessor */
		available_cop = TA_NULL;

		/* Register exception handler used on OS */
		define_inthdr(SWI_SVC,      call_entry);
		define_inthdr(SWI_RETINT,   _tk_ret_int);
		define_inthdr(SWI_DISPATCH, dispatch_entry);
#if USE_DBGSPT
		define_inthdr(SWI_DEBUG,    call_dbgspt);
#endif
	}

	return E_OK;
}

/*
 * CPU-dependent finalization
 */
EXPORT void cpu_shutdown( void )
{
	/* Restore saved monitor exception handler */
	SCArea->intvec[VECNO_DEFAULT] = SaveMonHdr.default_hdr;
	SCArea->intvec[VECNO_BREAK]   = SaveMonHdr.break_hdr;
	SCArea->intvec[VECNO_MONITOR] = SaveMonHdr.monitor_hdr;
}

/* ------------------------------------------------------------------------- */

/*
 * Task exception handler startup reservation
 */
EXPORT void request_tex( TCB *tcb )
{
	/* Cannot set to the task operating at protected level 0 */
	if ( tcb->isysmode == 0 ) {
		tcb->reqdct = 1;
	}
}

/*
 * Task exception handler startup setting
 *
 *	First stack state
 *		System stack		User stack
 *		+---------------+		+---------------+
 *	ssp ->	| SPSR_svc	|	usp ->	| (xxxxxxxxxxx)	|
 *		| R12     = ip	|		|		|
 *		| R14_svc = lr	|
 *		+---------------+
 *
 *	Stack state after change (Change *)
 *		+---------------+		+---------------+
 *	ssp ->	| SPSR_svc	|*	usp* ->	| CPSR		|*
 *		| R12		|		| texcd		|*
 *		| texhdr	|*		| retadr	|*
 *		+---------------+		+---------------+
 *						| (xxxxxxxxxxx)	|
 */
EXPORT void setup_texhdr( UW *ssp )
{
	FP	texhdr;
	INT	texcd;
	UINT	m;
	UW	*usp;
	UINT	prid = get_prid();

	/* Called in interrupt disable state */

	ENTER_CRITICAL_SECTION

	ctxtsk[prid]->reqdct = 0;	/* Clear DCT */

	/* Get exception code */
	m = 0x00000001;
	for ( texcd = 0; texcd <= 31; texcd++ ) {
		if ( (ctxtsk[prid]->exectex & m) != 0 ) {
			break;
		}
		m <<= 1;
	}
	/* Exception does not occur /Exception is cleared */
	if ( texcd > 31 ) {
		LEAVE_CRITICAL_SECTION
		return;
	}

	ctxtsk[prid]->exectex = 0;
	ctxtsk[prid]->pendtex &= ~m;
	ctxtsk[prid]->texflg |= ( texcd == 0 )? TEX0_RUNNING: TEX1_RUNNING;
	texhdr = ctxtsk[prid]->texhdr;

	/* Get user stack pointer */
	Asm("stmia %0, {sp}^ ; nop":: "r"(&usp));

	/* If the exception code is 0, return the user stack
	   to the initial value */
	if ( texcd == 0 ) {
		usp = ctxtsk[prid]->istack;
	}

	usp -= 3;

	/* Set user stack pointer */
	Asm("ldmia %0, {sp}^ ; nop":: "r"(&usp));

	LEAVE_CRITICAL_SECTION

	ENABLE_INTERRUPT;

	/* Stack adjustment
	 *	A page fault may occur 
	 *	due to access to the user stack.
	 */
	*(usp + 0) = *(ssp + 0);	/* CPSR */
	*(usp + 1) = texcd;
	*(usp + 2) = *(ssp + 2);	/* retadr */
	*(ssp + 2) = (UW)texhdr & ~0x00000001U;
	if ( ((UW)texhdr & 1) == 0 ) {
		*ssp &= ~PSR_T;
	} else {
		*ssp |= PSR_T;
	}
}

