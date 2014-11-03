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
 *	ready_queue.h (T-Kernel/OS)
 *	Ready Queue Operation Routine
 */

#ifndef _READY_QUEUE_
#define _READY_QUEUE_

#include <libstr.h>
#include <sys/smp_def.h>
#include "bitop.h"
#include "config.h"

/*
 * Definition of ready queue structure 
 *	In the ready queue, the task queue 'tskque' is provided per priority.
 *	The task TCB is registered onto queue with the applicable priority.
 *	For effective ready queue search, the bitmap area 'bitmap' is provided
 *	to indicate whether there are tasks in task queue per priority.
 *	
 *	Also, to search a task at the highest priority in the ready queue  
 *    	effectively, put the highest task priority in the 'top_priority' field.
 *	If the ready queue is empty, set the value in this field to NUM_PRI. 
 *	In this case, to return '0' with refering 'tskque[top_priority]',
 *      there is 'null' field which is always '0'.
 *
 *	Multiple READY tasks with kernel lock do not exist at the same time.
 */

#define BITMAPSZ	( sizeof(UINT) * 8 )
#define NUM_BITMAP	( (NUM_PRI + BITMAPSZ - 1) / BITMAPSZ )

typedef	struct ready_queue {
	QUEUE	tskque[NUM_PRI];	/* Task queue per priority */
	TCB	*null;			/* When the ready queue is empty, */
	UINT	bitmap[NUM_BITMAP];	/* Bitmap area per priority */
	TCB	*klocktsk;		/* READY task with kernel lock */
	TCB	*top[MAX_PROC];		/* top4 priority in ready queue */
	BOOL	chg;
} RDYQUE;

IMPORT RDYQUE	ready_queue;

/*
 * Ready queue initialization
 */
Inline void ready_queue_initialize( RDYQUE *rq )
{
	INT	i;

	for ( i = 0; i < MAX_PROC; i++ ) {
		rq->top[i] = NULL;
	}
	for ( i = 0; i < NUM_PRI; i++ ) {
		QueInit(&rq->tskque[i]);
	}
	rq->null = NULL;
	rq->klocktsk = NULL;
	memset(rq->bitmap, 0, sizeof(rq->bitmap));
	rq->chg = FALSE;
}

/*
 * Return the highest priority task in ready queue
 * It is in ready_queue.c.
 */
IMPORT void ready_queue_top( RDYQUE *rq, TCB *tcb[] );

/*
 * Return the priority of the highest priority task in the ready queue
 */
Inline INT ready_queue_top_priority( const RDYQUE *rq )
{
	return rq->top[0]->priority;
}

Inline BOOL is_change_ready_queue( const RDYQUE *rq )
{
	return rq->chg;
}

Inline void update_top_when_insert( TCB *top[], TCB *tcb )
{
	INT	n, i;

	/* decide insert point */
	for ( n = 0; n < num_proc; n++ ) {
		if ( (top[n] == NULL) || (top[n]->priority > tcb->priority) ) {
			/* shift */
			for ( i = num_proc - 1; n < i; i-- ) {
				top[i] = top[i-1];
			}
			/* insert */
			top[n] = tcb;

			return;
		}
	}
}

/*
 * Insert task in ready queue
 *	Insert it at the end of the same priority tasks with task priority 
 *	indicated with 'tcb'. Set the applicable bit in the bitmap area and 
 *	update 'top_priority' if necessary. When updating 'top_priority,' 
 *	return TRUE, otherwise FALSE.
 */
Inline void ready_queue_insert( RDYQUE *rq, TCB *tcb )
{
	INT	priority = tcb->priority;

	tcb->slicecnt = 0;

	QueInsert(&tcb->tskque, &rq->tskque[priority]);
	tstdlib_bitset(rq->bitmap, priority);

	if ( tcb->klocked ) {
		rq->klocktsk = tcb;
	}

	/* update top tcb */
	update_top_when_insert(rq->top, tcb);
	rq->chg = TRUE;
}

/*
 * Update top[MAX_PROC] when insert top
 */
Inline void update_top_when_insert_top( TCB *top[], TCB *tcb )
{
	INT	n, i;

	/* decide insert point */
	for ( n = 0; n < num_proc; n++ ) {
		if ( (top[n] == NULL) || (top[n]->priority >= tcb->priority) ) {
			/* shift */
			for ( i = num_proc - 1; n < i; i-- ) {
				top[i] = top[i-1];
			}
			/* insert */
			top[n] = tcb;

			return;
		}
	}
}


/*
 * Insert task at head in ready queue 
 */
Inline void ready_queue_insert_top( RDYQUE *rq, TCB *tcb )
{
	INT	priority = tcb->priority;

	QueInsert(&tcb->tskque, rq->tskque[priority].next);
	tstdlib_bitset(rq->bitmap, priority);
	if ( tcb->klocked ) {
		rq->klocktsk = tcb;
	}

	/* update top tcb */
	update_top_when_insert_top(rq->top, tcb);
	rq->chg = TRUE;
}

/*
 * Get next TCB
 */
Inline TCB *get_next_tcb( RDYQUE *rq, TCB *tcb )
{
	INT 	priority = tcb->priority;
	INT	i;

	/* this tcb isn't bottom of ready queue */
	if ( tcb->tskque.next != &rq->tskque[priority] ) {
		return (TCB*)tcb->tskque.next;
	}

	/* bitmap search from next priority */
	priority++;
	i = tstdlib_bitsearch1(rq->bitmap, priority, NUM_PRI - priority);
	if ( i >= 0 ) {
		return (TCB*)rq->tskque[(priority + i)].next;
	} else {
		return NULL;
	}
}

/*
 * Update top[MAX_PROC] when remove
 */
Inline void update_top_when_remove( RDYQUE *rq, TCB *top[], TCB *tcb )
{
	INT	n, i;

	for ( n = 0; n < num_proc; n++ ) {
		if ( tcb == top[n] ) {
			if ( num_proc == 1 ) {
				top[n] = get_next_tcb(rq, tcb);
			} else {
				for ( i = n; i < num_proc-1; i++ ) {
					top[i] = top[i+1];
				}
				if ( top[num_proc-2] != NULL ) {
					top[num_proc-1] = get_next_tcb(rq, top[num_proc-2]);
				}
			}
			return;
		}
	}
}

/*
 * Delete task from ready queue
 *	Take out TCB from the applicable priority task queue, and if the task 
 *	queue becomes empty, clear the applicable bit from the bitmap area.
 *	In addition, update 'top_priority' if the deleted task had the highest 
 *	priority. In such case, use the bitmap area to search the second
 *	highest priority task.
 */
Inline void ready_queue_delete( RDYQUE *rq, TCB *tcb )
{
	INT	priority = tcb->priority;

	if ( rq->klocktsk == tcb ) {
		rq->klocktsk = NULL;
	}

	QueRemove(&tcb->tskque);
	if ( isQueEmpty(&rq->tskque[priority]) ) {
		tstdlib_bitclr(rq->bitmap, priority);
	}

	if ( tcb->klockwait ) {
		/* Delete from kernel lock wait queue */
		tcb->klockwait = FALSE;
		return;
	}

	/* update top tcb */
	update_top_when_remove(rq, rq->top, tcb);
	rq->chg = TRUE;
}

/*
 * Move the task, whose ready queue priority is 'priority', at head of
 * queue to the end of queue. Do nothing, if the queue is empty.
 */
Inline void ready_queue_rotate( RDYQUE *rq, INT priority )
{
	QUEUE	*tskque = &rq->tskque[priority];
	TCB	*tcb;

	if ( isQueEmpty(tskque) ) {
		return;
	}
	if ( tskque->next->next != tskque ) {
		tcb = (TCB*)QueRemoveNext(tskque);
		update_top_when_remove(rq, rq->top, tcb);
		QueInsert((QUEUE*)tcb, tskque);
		update_top_when_insert(rq->top, tcb);
		rq->chg = TRUE;
	} else {
		tcb = (TCB*)tskque->next;
	}

	tcb->slicecnt = 0;
}

/*
 * Put 'tcb' to the end of ready queue. 
 */
Inline BOOL ready_queue_move_last( RDYQUE *rq, TCB *tcb )
{
	QUEUE	*tskque = &rq->tskque[tcb->priority];
	BOOL	chg = FALSE;

	if ( tcb->tskque.next != tskque ) {
		QueRemove(&tcb->tskque);
		update_top_when_remove(rq, rq->top, tcb);
		QueInsert(&tcb->tskque, tskque);
		update_top_when_insert(rq->top, tcb);
		chg = TRUE;
	}

	tcb->slicecnt = 0;

	return chg;
}

#endif /* _READY_QUEUE_ */
