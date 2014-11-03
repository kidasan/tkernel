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
 *	cpu_status.h (SH7776)
 *	CPU-Dependent Definition
 */

#ifndef _CPU_STATUS_
#define _CPU_STATUS_

#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <tk/smp_util.h>
#include "cpu_insn.h"

/*
 * Start/End critical section
 */
IMPORT T_COUNTLOCK SpinLockObj;			/* Spin lock for critical section */
IMPORT volatile INT restore_context_disable;
#define RDS_DISABLE	(0)
#define RDS_ENABLE	(1)

#define dispatch_request_other() {							\
					INT	i;					\
					UW	reg = INTICI0;				\
					INT	prid = get_prid();			\
					for ( i = 0; i < MAX_PROC; i++ ) {		\
						if ( i == prid ) continue;		\
						if ( (ctxtsk[i] != NULL) && (ctxtsk[i] != schedtsk[i]) ){ \
					 		out_w(reg + (i * 4), prid + 1);	\
						}					\
					}						\
				}

#define tex_request_other()	{									\
					INT	i;							\
					UW	reg = INTICI0;						\
					INT	prid = get_prid();					\
					for ( i = 0; i < MAX_PROC; i++ ) {				\
						if ( i == prid ) continue;				\
						if ( (ctxtsk[i] != NULL) && (ctxtsk[i]->reqdct == 1) ){	\
					 		out_w(reg + (i * 4), prid + 1);			\
						}							\
					}								\
				}

#define ENTER_CRITICAL_SECTION CountSpinLock(&SpinLockObj);
#define LEAVE_CRITICAL_SECTION CountSpinUnlock(&SpinLockObj);\

#define BEGIN_CRITICAL_SECTION	{ volatile UINT _sr_ = disint();		\
				  CountSpinLock(&SpinLockObj);			\
				  restore_context_disable = RDS_DISABLE;	\
				  { INT prid = get_prid();			\
				    /* care for tk_ter_tsk(), tk_del_tsk() while I'm waiting for SpinLock */ \
				    if ( ctxtsk[prid] && ((ctxtsk[prid]->state == TS_DORMANT) || (ctxtsk[prid]->state == TS_NONEXIST)) && !isTaskIndependent() ) {	\
				      force_dispatch();				\
				    }						\
				  }

#define END_CRITICAL_SECTION	  update_schedtsk();				\
				  dispatch_request_other();			\
				  { INT prid =get_prid();			\
				    if ( !isDI(_sr_) && (ctxtsk[prid] != schedtsk[prid]) && !isTaskIndependent() && !dispatch_disabled[prid] ) {	\
				      dispatch();				\
				    } else {					\
				      CountSpinUnlock(&SpinLockObj);		\
				    }						\
				  }						\
				  enaint(_sr_);					\
				}

#define END_CRITICAL_TEX	  tex_request_other();				\
				  CountSpinUnlock(&SpinLockObj);		\
				  enaint(_sr_);					\
				}

#define END_CRITICAL_FORCE	  update_schedtsk();				\
				  dispatch_request_other();			\
				  force_dispatch();				\
				  /* never return */				\
				  enaint(_sr_);					\
				}

#define BEGIN_CRITICAL_NO_DISPATCH { volatile UINT _sr_ = disint();		\
				     CountSpinLock(&SpinLockObj);

#define END_CRITICAL_NO_DISPATCH     CountSpinUnlock(&SpinLockObj);		\
				     enaint(_sr_); }

/*
 * Start/End interrupt disable section
 */
#define BEGIN_DISABLE_INTERRUPT	{ UINT _sr_ = disint();
#define END_DISABLE_INTERRUPT	enaint(_sr_); }

/*
 * Interrupt enable/disable
 */
#define ENABLE_INTERRUPT	{ enaint(0); }
#define DISABLE_INTERRUPT	{ disint(); }

/*
 * Enable interrupt nesting
 *	Enable the interrupt that has a higher priority than 'level.'
 */
#define ENABLE_INTERRUPT_UPTO(level)	{ enaint(SR_I(level)); }

/*
 * Move to/Restore task independent part
 */
#define ENTER_TASK_INDEPENDENT	{ EnterTaskIndependent(); }
#define LEAVE_TASK_INDEPENDENT	{ LeaveTaskIndependent(); }

/* ----------------------------------------------------------------------- */
/*
 *	Check system state
 */

IMPORT TCB	*ctxtsk[];
IMPORT INT	dispatch_disabled[MAX_PROC];

/*
 * When a system call is called from the task independent part, TRUE
 */
Inline BOOL in_indp()
{
	BOOL res;

	BEGIN_DISABLE_INTERRUPT;
	res = ( isTaskIndependent() || ctxtsk[get_prid()] == NULL );
	END_DISABLE_INTERRUPT;

	return res;
}

/*
 * When a system call is called during dispatch disable, TRUE
 * Also include the task independent part as during dispatch disable.
 */
Inline BOOL in_ddsp()
{
	BOOL res;
	
	BEGIN_DISABLE_INTERRUPT;
	res = dispatch_disabled[get_prid()];
	END_DISABLE_INTERRUPT;
	res =	res || in_indp() || isDI(getSR());

	return res;
}

/*
 * When a system call is called during CPU lock (interrupt disable), TRUE
 * Also include the task independent part as during CPU lock.
 */
#define in_loc()	( isDI(getSR())		\
			|| in_indp() )

/*
 * When a system call is called during executing the quasi task part, TRUE
 * Valid only when in_indp() == FALSE because it is not discriminated from
 * the task independent part.
 */
Inline TCB* ctx()
{
	TCB	*tcb;

	BEGIN_DISABLE_INTERRUPT
	tcb = ctxtsk[get_prid()];
	END_DISABLE_INTERRUPT

	return tcb;
}
#define in_qtsk()	( ctx()->sysmode > ctx()->isysmode )

/* ----------------------------------------------------------------------- */
/*
 *	Task dispatcher startup routine
 */

/*
 * Request for task dispatcher startup
 *	Do nothing at this point because there is no delayed interrupt
 *	function in SH.
 *	Perform dispatcher startup with END_CRITICAL_SECTION.
 */
#define dispatch_request()	;

/*
 * Throw away the current task context
 * and forcibly dispatch to the task that should be performed next.
 *	Use at system startup and 'tk_ext_tsk, tk_exd_tsk.'
 */
Inline void force_dispatch( void )
{
IMPORT	void	dispatch_to_schedtsk();

	Asm("	jmp @%0		\n"
	"	nop		"
		:: "r"(&dispatch_to_schedtsk));
}

/*
 * Start task dispatcher
 */
Inline void dispatch( void )
{
	Asm("trapa %0":: "i"(TRAP_DISPATCH));
}

/* ----------------------------------------------------------------------- */
/*
 *	Task exception
 */

/*
 * Task exception handler startup reservation
 */
IMPORT void request_tex( TCB *tcb );

/* ----------------------------------------------------------------------- */

/*
 * Task context block
 */
typedef struct {
	VP	ssp;		/* System stack pointer */
	VP	uatb;		/* Task space page table */
	INT	lsid;		/* Task space ID */
} CTXB;

/*
 * CPU information
 */
IMPORT ATR	available_cop;	/* Enabled coprocessor (TA_COPn) */

/* ----------------------------------------------------------------------- */
/*
 *	Cyclic handler / Alarm handler
 */

/*
 * Notify the other processor of handler request
 */
Inline BOOL handler_request( UW assprc )
{
	UW	list, i;
	INT	prid = get_prid();
	UW	reg = INTICI0;

	i = 0;
	list = assprc & ~(1 << prid);
	while ( list != 0 ) {
		if( list & 0x00000001 ) {
			out_w(reg + (i * 4), 1 << 8);
		}
		list >>= 1;
		i++;
	}

	return ((assprc & (1 << prid)) != 0);
}

#endif /* _CPU_STATUS_ */
