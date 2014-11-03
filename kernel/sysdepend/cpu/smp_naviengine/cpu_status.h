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
 *	cpu_status.h (NAVIENGINE)
 *	NAVIENGINE Dependent Definition
 */

#ifndef _CPU_STATUS_
#define _CPU_STATUS_

#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <sys/kernel_util.h>
#include "cpu_insn.h"

/*
 * Start/End critical section
 */
IMPORT T_COUNTLOCK SpinLockObj;			/* Spin lock for critical section */

#define dispatch_request_other()	{ INT	i;				\
					  UW	list = 0;			\
					  INT	prid = get_prid();		\
					  for ( i = 0; i < MAX_PROC; i++ ) {	\
					    if ( i == prid ) continue;		\
					    if ( (ctxtsk[i] != NULL)		\
					      && (ctxtsk[i] != schedtsk[i]) ) {	\
					      list |= 1 << (16+i);		\
					    }					\
					  }					\
					  if ( list != 0 ) {			\
					    out_w(DIC_SWI(0), list);		\
					  }					\
					}

#define tex_request_other()	{ INT	i;				\
				  UW	list = 0;			\
				  INT	prid = get_prid();		\
				  for ( i = 0; i < MAX_PROC; i++ ) {	\
				    if ( i == prid ) continue;		\
				    if ( (ctxtsk[i] != NULL)		\
				      && (ctxtsk[i]->reqdct == 1) ) {	\
				      list |= 1 << (16+i);		\
				    }					\
				  }					\
				  if ( list != 0 ) {			\
				    out_w(DIC_SWI(0), list);		\
				  }					\
				}

#define DMB()	Asm("mcr p15, 0, %0, c7, c10, 5" : : "r"(0));

#define ENTER_CRITICAL_SECTION	CountSpinLock(&SpinLockObj);
#define LEAVE_CRITICAL_SECTION	CountSpinUnlock(&SpinLockObj);

#define BEGIN_CRITICAL_SECTION	{ UINT volatile _cpsr_ = disint();		\
				  CountSpinLock(&SpinLockObj);			\
				  { INT prid = get_prid();			\
				    /* care for tk_ter_tsk(), tk_del_tsk() */	\
				    /* while I'm waiting for SpinLock      */	\
				    if ( ctxtsk[prid]				\
				      && ((ctxtsk[prid]->state == TS_DORMANT)	\
				       || (ctxtsk[prid]->state == TS_NONEXIST))	\
				      && !isTaskIndependent() ) {		\
				      force_dispatch();				\
				    }						\
				  }

#define END_CRITICAL_SECTION	  update_schedtsk();				\
				  DMB();					\
				  dispatch_request_other();			\
				  { INT prid =get_prid();			\
				    if ( !isDI(_cpsr_)				\
				      && (ctxtsk[prid] != schedtsk[prid])	\
				      && !isTaskIndependent()			\
				      && !dispatch_disabled[prid] ) {		\
				      dispatch();				\
				    } else {					\
				      CountSpinUnlock(&SpinLockObj);		\
				    }						\
				  }						\
				  enaint(_cpsr_);				\
				}

#define END_CRITICAL_TEX	  DMB();					\
				  tex_request_other();				\
				  CountSpinUnlock(&SpinLockObj);		\
				  enaint(_cpsr_);				\
				}

#define END_CRITICAL_FORCE	  update_schedtsk();				\
				  DMB();					\
				  dispatch_request_other();			\
				  force_dispatch();				\
				  /* never return */				\
				  enaint(_cpsr_);				\
				}

#define BEGIN_CRITICAL_NO_DISPATCH	{ UINT volatile _cpsr_ = disint();	\
					  CountSpinLock(&SpinLockObj);

#define END_CRITICAL_NO_DISPATCH	  CountSpinUnlock(&SpinLockObj);	\
					  enaint(_cpsr_); }

/*
 * Start/End interrupt disable section
 */
#define BEGIN_DISABLE_INTERRUPT	{ UINT _cpsr_ = disint();
#define END_DISABLE_INTERRUPT	enaint(_cpsr_); }

/*
 * Interrupt enable/disable
 */
#define ENABLE_INTERRUPT	{ enaint(0); }
#define DISABLE_INTERRUPT	{ disint(); }

/*
 * Enable interrupt nesting
 *	Enable the interrupt that has a higher priority than 'level.'
 */
#define ENABLE_INTERRUPT_UPTO(level)	{ enaint(0); }

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
IMPORT INT	dispatch_disabled[];

/*
 * When a system call is called from the task independent part, TRUE
 */
Inline BOOL in_indp()
{
	BOOL	res;

	BEGIN_DISABLE_INTERRUPT
	res = ( isTaskIndependent() || ctxtsk[get_prid()] == NULL );
	END_DISABLE_INTERRUPT

	return res;
}
/*
 * When a system call is called during dispatch disable, TRUE
 * Also include the task independent part as during dispatch disable.
 */
Inline BOOL in_ddsp()
{
	BOOL	res;

	BEGIN_DISABLE_INTERRUPT
	res = dispatch_disabled[get_prid()];
	END_DISABLE_INTERRUPT
	res = res || in_indp() || isDI(getCPSR());

	return res;
}

/*
 * When a system call is called during CPU lock (interrupt disable), TRUE
 * Also include the task independent part as during CPU lock.
 */
#define in_loc()	( isDI(getCPSR())		\
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
 *	Do nothing at this point because there is no delayed
 *	interrupt function in ARM.
 *	Perform dispatcher startup with END_CRITICAL_SECTION.
 */
#define dispatch_request()	;

/*
 * Throw away the current task context. 
 * and forcibly dispatch to the task that should be performed next.
 *	Use at system startup and 'tk_ext_tsk, tk_exd_tsk.'
 */
Inline void force_dispatch( void )
{
IMPORT	void	dispatch_to_schedtsk();

	Asm("bx %0":: "r"(&dispatch_to_schedtsk));
}

/*
 * Start task dispatcher
 */
Inline void dispatch( void )
{
	Asm("swi %0":: "i"(SWI_DISPATCH): "lr");
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
	UW	list;
	INT	prid = get_prid();

	list = assprc & ~(1 << prid);
	if ( list != 0 ) {
		list = (list << 16) | IPI_HDR_ID;
		out_w(DIC_SWI(0), list);
	}

	return ((assprc & (1 << prid)) != 0);
}

#endif /* _CPU_STATUS_ */
