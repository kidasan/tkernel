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
 *	task.c (T-Kernel/OS)
 *	Task Control
 */

#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "ready_queue.h"
#include "cpu_task.h"
#include "tkdev_timer.h"
#include "check.h"
#include <sys/rominfo.h>

/* For SMP T-Kernel */
#include "mp_common.h"

/*
 * Task dispatch disable state
 */
EXPORT INT	dispatch_disabled[MAX_PROC];	/* DDS_XXX see task.h */

/*
 * Task execution control 
 */
EXPORT TCB	*ctxtsk[MAX_PROC];	/* Task in execution */
EXPORT TCB	*schedtsk[MAX_PROC];	/* Task which should be executed */
EXPORT INT	num_proc;		/* Number of processors used of specification by configuration */
EXPORT RDYQUE	ready_queue;	/* Ready queue */

/*
 * Task control information
 */
EXPORT TCB	*tcb_table;	/* Task control block */
EXPORT QUEUE	free_tcb;	/* FreeQue */
EXPORT ID	max_tskid;	/* Maximum task ID */

EXPORT INT	default_sstksz;	/* Default system stack size */
EXPORT INT	svc_call_limit;	/* Protect level of system call */

#define MIN_RNG_LVL	(INT)(TA_RNG0>>8)
#define MAX_RNG_LVL	(INT)(TA_RNG3>>8)

/*
 * TCB Initialization
 */
EXPORT ER task_initialize( void )
{
	INT	i;
	TCB	*tcb;
	ID	tskid;

	/* Get system information */
	i = _tk_get_cfn(SCTAG_TMAXTSKID, &max_tskid, 1);
	if ( (i < 1) || (NUM_TSKID < 1) ) {
		return E_SYS;
	}
	i = _tk_get_cfn(SCTAG_TSYSSTKSZ, &default_sstksz, 1);
	if ( (i < 1) || (default_sstksz < MIN_SYS_STACK_SIZE) ) {
		return E_SYS;
	}
	i = _tk_get_cfn(SCTAG_TSVCLIMIT, &svc_call_limit, 1);
	if ( (i < 1) || (svc_call_limit < MIN_RNG_LVL) || (svc_call_limit > MAX_RNG_LVL) ) {
		return E_SYS;
	}

	/* Allocate TCB area */
	tcb_table = Imalloc((UINT)NUM_TSKID * sizeof(TCB));
	if ( tcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Initialize task execution control information */
	for ( i = 0; i < MAX_PROC; i++ ) {
		ctxtsk[i] = schedtsk[i] = NULL;
		dispatch_disabled[i] = DDS_ENABLE;
	}
	ready_queue_initialize(&ready_queue);

	/* Register all TCBs onto FreeQue */
	QueInit(&free_tcb);
	for ( tcb = tcb_table, i = 0; i < NUM_TSKID; tcb++, i++ ) {
		tskid = ID_TSK(i);
		tcb->tskid = tskid;
		tcb->state = TS_NONEXIST;
#ifdef NUM_PORID
		tcb->wrdvno = tskid;
#endif
		InitSVCLOCK(&tcb->svclock);
		tcb->svclocked = NULL;

		QueInsert(&tcb->tskque, &free_tcb);
	}

	return E_OK;
}

/*
 * Prepare task execution.
 */
EXPORT void make_dormant( TCB *tcb )
{
	/* Initialize variables which should be reset at DORMANT state */
	tcb->state	= TS_DORMANT;
	tcb->priority	= tcb->bpriority = tcb->ipriority;
	tcb->sysmode	= (H)tcb->isysmode;
	tcb->wupcnt	= 0;
	tcb->suscnt	= 0;
	tcb->waitmask	= 0;

	tcb->stkfree	= FALSE;
	tcb->nodiswai	= FALSE;
	tcb->klockwait	= FALSE;
	tcb->klocked	= FALSE;

	tcb->slicetime	= 0;
	tcb->stime	= 0;
	tcb->utime	= 0;

#ifdef NUM_MTXID
	tcb->mtxlist	= NULL;
#endif
	tcb->tskevt	= 0;

	tcb->reqdct	= 0;	/* clear DCT request */

	tcb->texhdr	= NULL; /* Undefined task exception handler */
	tcb->texmask	= 0;
	tcb->pendtex	= 0;
	tcb->exectex	= 0;
	tcb->texflg	= 0;
	tcb->execssid	= 0;
}

/* ------------------------------------------------------------------------ */

/*
 * Scheduling by time slice
 *	Add TIMER_PERIOD to the 'tcb' task's continuous execution
 *	time counter, and then if the counter exceeds the maximum
 *	continuation time, put the task at the end of the ready queue.
 */
EXPORT BOOL time_slice_schedule( TCB *tcb )
{
	BOOL	chg = FALSE;

	if ( (tcb != NULL) && (tcb->slicetime > 0) ) {
		tcb->slicecnt += TIMER_PERIOD;
		if ( tcb->slicecnt > tcb->slicetime ) {
			chg = ready_queue_move_last(&ready_queue, tcb);
		}
	}

	return chg;
}

/*
 * Check schedule change
 */
EXPORT BOOL is_change_schedule( TCB *cur[], TCB *pre[] )
{
	INT	i;

	for ( i = 0; i < num_proc; i++ ) {
		if ( cur[i] != pre[i] ) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Check tcb is scheduled
 */
EXPORT BOOL is_schedtsk( TCB *tcb )
{
	INT	i;

	for ( i = 0; i < num_proc; i++ ) {
		if ( schedtsk[i] == tcb ) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Check tcb is context
 */
EXPORT BOOL is_ctxtsk( TCB *tcb )
{
	INT	i;

	for ( i = 0; i < num_proc; i++ ) {
		if ( ctxtsk[i] == tcb ) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Get processor ID from context
 */
EXPORT UINT get_prid_from_ctx( TCB *tcb )
{
	INT	i;

	for ( i = 0; i < num_proc; i++ ) {
		if ( ctxtsk[i] == tcb ) {
			return i;
		}
	}

	return 0;
}

/*
 * Check whether tcb is an array element
 */
LOCAL BOOL is_element( TCB *tcb, TCB *array[] )
{
	INT	i;

	for ( i = 0; i < num_proc; i++ ) {
		if ( array[i] == tcb ) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Reselect task to execute
 *	Set 'schedtsk' to the head task at the ready queue.
 */
Inline void reschedule( void )
{
	TCB	*toptsk[MAX_PROC];
	INT	i;

	ready_queue_top(&ready_queue, toptsk);
	if ( is_change_schedule(schedtsk, toptsk) ) {
		/*
		 * When the state becomes RUN to READY,
		 * execute the time slice scheduling.
		 */
		for ( i = 0; i < num_proc; i++ ) {
			if ( (schedtsk[i] == ctxtsk[i]) && !is_element(ctxtsk[i], toptsk) ) {
				time_slice_schedule(schedtsk[i]);
			}
			schedtsk[i] = toptsk[i];
		}

		dispatch_request();
	}
}

/*
 * Update schedtsk
 */
EXPORT void update_schedtsk( void )
{
	if ( is_change_ready_queue(&ready_queue) ) {
		reschedule();
	}
}

/*
 * Set task to READY state.
 *	Update the task state and insert in the ready queue. If necessary, 
 *	update 'schedtsk' and request to start task dispatcher. 
 */
EXPORT void make_ready( TCB *tcb )
{
	tcb->state = TS_READY;
	ready_queue_insert(&ready_queue, tcb);
}

/*
 * Set task to non-executable state.
 *	Delete the task from the ready queue.
 *	If the deleted task is 'schedtsk', set 'schedtsk' to the
 *	highest priority task in the ready queue. 
 *	'tcb' task must be READY.
 */
EXPORT void make_non_ready( TCB *tcb )
{
	ready_queue_delete(&ready_queue, tcb);
	if ( is_schedtsk(tcb) ) {
		ready_queue_top(&ready_queue, schedtsk);
		dispatch_request();
	}
}

/*
 * Change task priority.
 */
EXPORT void change_task_priority( TCB *tcb, INT priority )
{
	INT	oldpri;

	if ( tcb->state == TS_READY ) {
		/*
		 * When deleting a task from the ready queue, 
		 * a value in the 'priority' field in TCB is needed. 
		 * Therefore you need to delete the task from the
		 * ready queue before changing 'tcb->priority.'
		 */
		ready_queue_delete(&ready_queue, tcb);
		tcb->priority = (UB)priority;
		ready_queue_insert(&ready_queue, tcb);
	} else {
		oldpri = (INT)tcb->priority;
		tcb->priority = (UB)priority;

		/* If the hook routine at the task priority change is defined,
		   execute it */
		if ( ((tcb->state & TS_WAIT) != 0) && (tcb->wspec->chg_pri_hook != NULL)) {
			(*tcb->wspec->chg_pri_hook)(tcb, oldpri);
		}
	}
}

/*
 * Rotate ready queue.
 */
EXPORT void rotate_ready_queue( INT priority )
{
	ready_queue_rotate(&ready_queue, priority);
}

/*
 * Rotate the ready queue including the highest priority task.
 */
EXPORT void rotate_ready_queue_run( void )
{
	ready_queue_rotate(&ready_queue,
			ready_queue_top_priority(&ready_queue));
}

/*
 * Rotate the ready queue including the task the own processor is executing
 */
EXPORT void rotate_ready_queue_ctx( void )
{
	if ( ready_queue_move_last(&ready_queue, ctxtsk[get_prid()]) ) {
		reschedule();
	}
}

/* ------------------------------------------------------------------------ */
/*
 *	Debug support function
 */
#if USE_DBGSPT

/*
 * Refer ready queue
 */
SYSCALL INT _td_rdy_que( PRI pri, ID list[], INT nent )
{
	QUEUE	*q, *tskque;
	INT	n = 0;

	CHECK_PRI(pri);

	BEGIN_CRITICAL_SECTION;
	tskque = &ready_queue.tskque[int_priority(pri)];
	for ( q = tskque->next; q != tskque; q = q->next ) {
		if ( n++ < nent ) {
			*list++ = ((TCB*)q)->tskid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

#endif /* USE_DBGSPT */
