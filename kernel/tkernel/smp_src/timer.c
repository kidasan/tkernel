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
 *	timer.c (T-Kernel/OS)
 *	Timer Control
 */

#include "kernel.h"
#include "timer.h"
#include "task.h"
#include "ready_queue.h"
#include "tkdev_timer.h"
#include <sys/rominfo.h>

/*
 * Timer interrupt interval (millisecond)
 */
EXPORT UINT	TIMER_PERIOD;

/*
 * Current time (Software clock)
 *	'current_time' shows the total operation time since
 *	operating system Starts. 'real_time_ofs' shows difference
 *	between the current time and the operating system clock
 *	(current_time). Do not change 'current_time' when setting
 *	time by 'set_tim()'. Set 'real_time_ofs' with the time  	 
 *   	difference between 'current_time' and setup time.
 *	Therefore 'current_time' does not affect with time change
 *	and it increases simply.
 */
EXPORT LSYSTIM	current_time;	/* System operation time */
EXPORT LSYSTIM	real_time_ofs;	/* Actual time - System operation time */

/* 
 * Timer event queue
 */
LOCAL QUEUE	timer_queue;

/*
 * Handler
 */
IMPORT void handler_initialize( void );
IMPORT UW check_handler_queue( void );
IMPORT void handler_update( UW assprc );

/*
 * Initialization of timer module
 */
EXPORT ER timer_initialize( void )
{
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TTIMPERIOD, (INT*)&TIMER_PERIOD, 1);
	if ( (n < 1)
	  || (TIMER_PERIOD < MIN_TIMER_PERIOD)
	  || (TIMER_PERIOD > MAX_TIMER_PERIOD) ) {
		return E_SYS;
	}

	current_time = real_time_ofs = ltoll(0);
	QueInit(&timer_queue);

	/* Initialize handler */
	handler_initialize();

	/* Start timer interrupt */
	start_hw_timer();

	return E_OK;
}

/*
 * Stop timer
 */
EXPORT void timer_shutdown( void )
{
	terminate_hw_timer();
}


/*
 * Insert timer event to timer event queue
 */
LOCAL void enqueue_tmeb( TMEB *event )
{
	QUEUE	*q;

	for ( q = timer_queue.next; q != &timer_queue; q = q->next ) {
		if ( ll_cmp(event->time, ((TMEB*)q)->time) < 0) {
			break;
		}
	}
	QueInsert(&event->queue, q);
}

/*
 * Set timeout event
 *	Register the timer event 'event' onto the timer queue to
 *	start after the timeout 'tmout'. At timeout, start with the
 *	argument 'arg' on the callback function 'callback'.
 *	When 'tmout' is TMO_FEVR, do not register onto the timer
 *	queue, but initialize queue area in case 'timer_delete' 
 *	is called later.
 *
 *	"include/tk/typedef.h"
 *	typedef	INT		TMO;
 *	typedef UINT		RELTIM;
 *	#define TMO_FEVR	(-1)
 */
EXPORT void timer_insert( TMEB *event, TMO tmout, CBACK callback, VP arg )
{
	event->callback = callback;
	event->arg = arg;

	if ( tmout == TMO_FEVR ) {
		QueInit(&event->queue);
	} else {
		/* To guarantee longer wait time specified by 'tmout',
		   add TIMER_PERIOD on wait time */
		event->time = ll_add( ll_add(current_time, ltoll(tmout)),
					uitoll(TIMER_PERIOD) );
		enqueue_tmeb(event);
	}
}

EXPORT void timer_insert_reltim( TMEB *event, RELTIM tmout, CBACK callback, VP arg )
{
	event->callback = callback;
	event->arg = arg;

	/* To guarantee longer wait time specified by 'tmout',
	   add TIMER_PERIOD on wait time */
	event->time = ll_add( ll_add(current_time, ultoll(tmout)),
				uitoll(TIMER_PERIOD) );
	enqueue_tmeb(event);
}

/*
 * Set time specified event
 *	Register the timer event 'evt' onto the timer queue to start at the 
 *	(absolute) time 'sotime'.
 *	'sotime' is not an actual time. It is system operation time.
 */
EXPORT void timer_insert_abs( TMEB *evt, LSYSTIM sotime, CBACK cback, VP arg )
{
	evt->callback = cback;
	evt->arg = arg;
	evt->time = sotime;
	enqueue_tmeb(evt);
}

/* ------------------------------------------------------------------------ */

/*
 * Timer interrupt handler
 *	Timer interrupt handler starts every TIMER_PERIOD millisecond 
 *	interval by hardware timer. Update the software clock and start the 
 *	timer event upon arriving at start time.
 */
EXPORT void timer_handler( void )
{
	TMEB	*event;
	INT	i;
	BOOL	sched_change = FALSE;
	TCB 	*pTcb;
	UW	assprc;

	clear_hw_timer_interrupt();		/* Clear timer interrupt */

	BEGIN_CRITICAL_SECTION;
	current_time = ll_add(current_time, uitoll(TIMER_PERIOD));

	for ( i = 0; i < MAX_PROC; i++ ) {
		pTcb = ctxtsk[i];
		if ( pTcb != NULL ) {
			/* Task at execution */
			if ( pTcb->sysmode > 0 ) {
				pTcb->stime += TIMER_PERIOD;
			} else {
				pTcb->utime += TIMER_PERIOD;
			}

			if ( is_schedtsk(pTcb) ) {
				if ( time_slice_schedule(pTcb) ) {
					sched_change = TRUE;
				}
			}
		}
	}
	if ( sched_change ) {
		ready_queue_top(&ready_queue, schedtsk);
		if ( is_change_schedule(schedtsk, ctxtsk) ) {
			dispatch_request();
		}
	}

	/* Execute event that passed occurring time. */
	while ( !isQueEmpty(&timer_queue) ) {
		event = (TMEB*)timer_queue.next;
		if ( ll_cmp(event->time, current_time) > 0 ) {
			break;
		}

		QueRemove(&event->queue);
		if ( event->callback != NULL ) {
			(*event->callback)(event->arg);
		}
	}

	/* Handler queue update processing */
	assprc = check_handler_queue();
	handler_update(assprc);

	END_CRITICAL_SECTION;

	end_of_hw_timer_interrupt();		/* Clear timer interrupt */
}

