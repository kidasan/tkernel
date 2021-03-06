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
 *	wait.c (T-Kernel/OS)
 *	Common Routine for Synchronization
 */

#include "kernel.h"
#include "task.h"
#include "wait.h"

/*
 * Update the task state to release wait. When it becomes ready state,
 * connect to the ready queue.
 * Call when the task is in the wait state (including double wait).
 */
Inline void make_non_wait( TCB *tcb )
{
	if ( tcb->state == TS_WAIT ) {
		make_ready(tcb);
	} else {
		tcb->state = TS_SUSPEND;
	}
}

/*
 * Release wait state of the task.
 */
Inline void wait_release( TCB *tcb )
{
	timer_delete(&tcb->wtmeb);
	QueRemove(&tcb->tskque);
	make_non_wait(tcb);
}

EXPORT void wait_release_ok( TCB *tcb )
{
	*tcb->wercd = E_OK;
	wait_release(tcb);
}

EXPORT void wait_release_ok_ercd( TCB *tcb, ER ercd )
{
	*tcb->wercd = ercd;
	wait_release(tcb);
}

EXPORT void wait_release_ng( TCB *tcb, ER ercd )
{
	*tcb->wercd = ercd;
	wait_release(tcb);
	if ( tcb->wspec->rel_wai_hook != NULL ) {
		(*tcb->wspec->rel_wai_hook)(tcb);
	}
}

EXPORT void wait_release_tmout( TCB *tcb )
{
	QueRemove(&tcb->tskque);
	make_non_wait(tcb);
	if ( tcb->wspec->rel_wai_hook != NULL ) {
		(*tcb->wspec->rel_wai_hook)(tcb);
	}
}

/*
 * Change the active task state to wait state and connect to the
 * timer event queue.
 *	Normally, 'ctxtsk' is in the RUN state, but when an interrupt
 *	occurs during executing system call, 'ctxtsk' may become the
 *	other state by system call called in the interrupt handler.
 *	However, it does not be in WAIT state.
 *
 *	"include/tk/typedef.h"
 *	typedef	INT		TMO;
 *	typedef UINT		RELTIM;
 *	#define TMO_FEVR	(-1)
 */
EXPORT void make_wait( TMO tmout, ATR atr )
{
	UINT	prid = get_prid();

	switch ( ctxtsk[prid]->state ) {
	  case TS_READY:
		make_non_ready(ctxtsk[prid]);
		ctxtsk[prid]->state = TS_WAIT;
		break;
	  case TS_SUSPEND:
		ctxtsk[prid]->state = TS_WAITSUS;
		break;
	}
	ctxtsk[prid]->nodiswai = ( (atr & TA_NODISWAI) != 0 )? TRUE: FALSE;
	timer_insert(&ctxtsk[prid]->wtmeb, tmout, (CBACK)wait_release_tmout, ctxtsk[prid]);
}

EXPORT void make_wait_reltim( RELTIM tmout, ATR atr )
{
	UINT	prid = get_prid();

	switch ( ctxtsk[prid]->state ) {
	  case TS_READY:
		make_non_ready(ctxtsk[prid]);
		ctxtsk[prid]->state = TS_WAIT;
		break;
	  case TS_SUSPEND:
		ctxtsk[prid]->state = TS_WAITSUS;
		break;
	}
	ctxtsk[prid]->nodiswai = ( (atr & TA_NODISWAI) != 0 )? TRUE: FALSE;
	timer_insert_reltim(&ctxtsk[prid]->wtmeb, tmout, (CBACK)wait_release_tmout, ctxtsk[prid]);
}

/*
 * Release all tasks connected to the wait queue, and define it
 * as E_DLT error.
 */
EXPORT void wait_delete( QUEUE *wait_queue )
{
	TCB	*tcb;

	while ( !isQueEmpty(wait_queue) ) {
		tcb = (TCB*)wait_queue->next;
		*tcb->wercd = E_DLT;
		wait_release(tcb);
	}
}

/*
 * Get ID of the head task in the wait queue.
 */
EXPORT ID wait_tskid( QUEUE *wait_queue )
{
	if ( isQueEmpty(wait_queue) ) {
		return 0;
	}

	return ((TCB*)wait_queue->next)->tskid;
}

/*
 * Change the active task state to wait state and connect to the timer wait 
 * queue and the object wait queue. Also set 'wid' in 'ctxtsk'.
 */
EXPORT void gcb_make_wait( GCB *gcb, TMO tmout )
{
	UINT	prid = get_prid();

	ctxtsk[prid]->wid = gcb->objid;
	make_wait(tmout, gcb->objatr);
	if ( (gcb->objatr & TA_TPRI) != 0 ) {
		queue_insert_tpri(ctxtsk[prid], &gcb->wait_queue);
	} else {
		QueInsert(&ctxtsk[prid]->tskque, &gcb->wait_queue);
	}
}

/*
 * 'gcb_make_wait' with wait disable check function
 */
EXPORT void gcb_make_wait_with_diswai( GCB *gcb, TMO tmout )
{
	UINT	prid = get_prid();

	if ( is_diswai(gcb, ctxtsk[prid], ctxtsk[prid]->wspec->tskwait) ) {
		*ctxtsk[prid]->wercd = E_DISWAI;
	} else {
		*ctxtsk[prid]->wercd = E_TMOUT;
		if ( tmout != TMO_POL ) {
			gcb_make_wait(gcb, tmout);
		}
	}
}

/*
 * When the task priority changes, adjust the task position at the wait queue.
 * It is called only if the object attribute TA_TPRI is specified.
 * 
 */
EXPORT void gcb_change_priority( GCB *gcb, TCB *tcb )
{
	QueRemove(&tcb->tskque);
	queue_insert_tpri(tcb, &gcb->wait_queue);
}

/*
 * Search the first task of wait queue include "tcb" with target.
 * (Not insert "tcb" into wait queue.)
 *
 */
EXPORT TCB* gcb_top_of_wait_queue( GCB *gcb, TCB *tcb )
{
	TCB	*q;

	if ( isQueEmpty(&gcb->wait_queue) ) {
		return tcb;
	}

	q = (TCB*)gcb->wait_queue.next;
	if ( (gcb->objatr & TA_TPRI) == 0 ) {
		return q;
	}

	return ( tcb->priority < q->priority )? tcb: q;
}
