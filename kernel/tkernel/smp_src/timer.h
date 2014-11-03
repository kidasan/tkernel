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
 *	timer.h (T-Kernel/OS)
 *	Timer Module Definition
 */

#ifndef _TIMER_
#define _TIMER_

#include <longlong.h>

/*
 * SYSTIM internal expression and conversion
 */
typedef	longlong	LSYSTIM;	/* SYSTIM int. expression */

Inline LSYSTIM toLSYSTIM( SYSTIM *time )
{
	LSYSTIM		ltime;

	hilo_ll(ltime, time->hi, time->lo);

	return ltime;
}

Inline SYSTIM toSYSTIM( LSYSTIM ltime )
{
	SYSTIM		time;

	ll_hilo(time.hi, time.lo, ltime);

	return time;
}

/*
 * Definition of timer event block 
 */
typedef void	(*CBACK)(VP);	/* Type of callback function */

typedef struct timer_event_block {
	QUEUE	queue;		/* Timer event queue */
	LSYSTIM	time;		/* Event time */
	CBACK	callback;	/* Callback function */
	VP	arg;		/* Argument to be sent to callback function */
} TMEB;

/*
 * Timer interrupt interval (millisecond)
 */
IMPORT UINT	TIMER_PERIOD;

/*
 * Current time (Software clock)
 */
IMPORT LSYSTIM	current_time;	/* System operation time */
IMPORT LSYSTIM	real_time_ofs;	/* Difference from actual time */

/* Actual time */
#define real_time()	( ll_add(current_time, real_time_ofs) )

/*
 * Timer initialization and stop
 */
IMPORT ER   timer_initialize( void );
IMPORT void timer_shutdown( void );

/*
 * Register timer event onto timer queue
 */
IMPORT void timer_insert( TMEB *evt, TMO tmout, CBACK cback, VP arg );
IMPORT void timer_insert_reltim( TMEB *event, RELTIM tmout, CBACK callback, VP arg );
IMPORT void timer_insert_abs( TMEB *evt, LSYSTIM time, CBACK cback, VP arg );

/*
 * Delete from timer queue
 */
Inline void timer_delete( TMEB *event )
{
	QueRemove(&event->queue);
}

#endif /* _TIMER_ */
