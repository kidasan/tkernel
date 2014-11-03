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
 *	klock.c (T-Kernel/OS)
 *	Kernel Lock
 */

#include "kernel.h"
#include "task.h"
#include "ready_queue.h"

/*
 * Object lock
 *	Do not call from critical section
 */
EXPORT void LockOBJ( OBJLOCK *loc )
{
	BOOL	klocked;
	UINT	prid;

  retry:
	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	klocked = ctxtsk[prid]->klocked;
	if ( !klocked ) {
		if ( loc->wtskq.next == NULL ) {
			/* Lock */
			QueInit(&loc->wtskq);

			ctxtsk[prid]->klocked = klocked = TRUE;
			ready_queue.klocktsk = ctxtsk[prid];
		} else {
			/* Ready for lock */
			ready_queue_delete(&ready_queue, ctxtsk[prid]);
			ctxtsk[prid]->klockwait = TRUE;
			QueInsert(&ctxtsk[prid]->tskque, &loc->wtskq);

			ready_queue_top(&ready_queue, schedtsk);
			dispatch_request();
		}
	}
	END_CRITICAL_SECTION;
	/* Since wait could be freed without getting lock, 
	   need to re-try if lock is not got */
	if ( !klocked ) {
		goto retry;
	}
}

/*
 * Object unlock
 *	It may be called from a critical section.
 */
EXPORT void UnlockOBJ( OBJLOCK *loc )
{
	TCB	*tcb;
	UINT	prid;

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	ctxtsk[prid]->klocked = FALSE;
	ready_queue.klocktsk = NULL;

	tcb = (TCB*)QueRemoveNext(&loc->wtskq);
	if ( tcb == NULL ) {
		/* Free lock */
		loc->wtskq.next = NULL;
	} else {
		/* Wake lock wait task */
		tcb->klockwait = FALSE;
		tcb->klocked = TRUE;
		ready_queue_insert_top(&ready_queue, tcb);
	}

	ready_queue_top(&ready_queue, schedtsk);
	if ( is_change_schedule(ctxtsk, schedtsk) ) {
		dispatch_request();
	}
	END_CRITICAL_SECTION;
}

/* ------------------------------------------------------------------------ */

/*
 * Extended SVC lock 
 *	Do not call from critical section
 */
EXPORT void LockSVC( SVCLOCK *loc )
{
	BOOL	lock;
	UINT	prid;

  retry:
	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	lock = ( ctxtsk[prid]->svclocked == loc );
	if ( !lock ) {
		if ( loc->wtskq.next == NULL ) {
			/* Lock */
			QueInit(&loc->wtskq);

			loc->locklist = ctxtsk[prid]->svclocked;
			ctxtsk[prid]->svclocked = loc;
			lock = TRUE;
		} else {
			/* Ready for lock */
			ready_queue_delete(&ready_queue, ctxtsk[prid]);
			ctxtsk[prid]->klockwait = TRUE;
			QueInsert(&ctxtsk[prid]->tskque, &loc->wtskq);

			ready_queue_top(&ready_queue, schedtsk);
			dispatch_request();
		}
	}
	END_CRITICAL_SECTION;
	/* Since wait could be freed without getting lock, 
	   need to re-try if lock is not got */
	if ( !lock ) {
		goto retry;
	}
}

/*
 * Extended SVC unlock 
 */
EXPORT void UnlockSVC( void )
{
	SVCLOCK	*loc;
	TCB	*tcb;
	UINT	prid;

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	loc = ctxtsk[prid]->svclocked;
	ctxtsk[prid]->svclocked = loc->locklist;

	tcb = (TCB*)QueRemoveNext(&loc->wtskq);
	if ( tcb == NULL ) {
		/* Free lock */
		loc->wtskq.next = NULL;
	} else {
		/* Wake lock wait task */
		tcb->klockwait = FALSE;
		loc->locklist = tcb->svclocked;
		tcb->svclocked = loc;
		ready_queue_insert(&ready_queue, tcb);

		ready_queue_top(&ready_queue, schedtsk);
		if ( is_change_schedule(ctxtsk, schedtsk) ) {
			dispatch_request();
		}
	}
	END_CRITICAL_SECTION;
}

/*
 * Unlock all extended SVCs in which 'tcb' tasks are locked
 */
EXPORT void AllUnlockSVC( TCB *tcb )
{
	SVCLOCK	*loc;
	UINT	prid;

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	while ( (loc = ctxtsk[prid]->svclocked) != NULL ) {
		ctxtsk[prid]->svclocked = loc->locklist;

		tcb = (TCB*)QueRemoveNext(&loc->wtskq);
		if ( tcb == NULL ) {
			/* Free lock */
			loc->wtskq.next = NULL;
		} else {
			/* Wake lock wait task */
			tcb->klockwait = FALSE;
			loc->locklist = tcb->svclocked;
			tcb->svclocked = loc;
			ready_queue_insert(&ready_queue, tcb);
		}
	}
	ready_queue_top(&ready_queue, schedtsk);
	if ( is_change_schedule(ctxtsk, schedtsk) ) {
		dispatch_request();
	}
	END_CRITICAL_SECTION;
}
