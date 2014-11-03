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
 *	tkdev_timer.h (NAVIENGINE)
 *	Hardware-Dependent Timer Processing
 */

#ifndef _TKDEV_TIMER_
#define _TKDEV_TIMER_

#include <tk/syslib.h>
#include <sys/sysinfo.h>
#include "tkdev_conf.h"

/*
 * Settable interval range (millisecond)
 */
#define MIN_TIMER_PERIOD	1
#define MAX_TIMER_PERIOD	50

#define	TMR_CLK_KHZ		(CPU_CLK_KHZ / TIMER_PRESCALER / 2)

/*
 * Set timer
 */
Inline void init_hw_timer( void )
{
	UW	imask;

	DI(imask);

	/* Stop timer count */
	out_w(PTIMER_CONTROL, 0);

	/* Set counter */
	out_w(PTIMER_LOAD, TIMER_COUNTER);

	/* Start timer count */
	out_w(PTIMER_CONTROL, ((TIMER_PRESCALER-1) << PTCR_PRESCALER)
				| PTCR_ENABLE | PTCR_RELOAD | PTCR_INTEN);

	EI(imask);
}

/*
 * Timer start processing
 *	Initialize the timer and start the periodical timer interrupt.
 */
Inline void start_hw_timer( void )
{
IMPORT	void	timer_handler_startup( void );

	/* Set timer */
	init_hw_timer();

	/* Set interrupt Mode */
	SetIntMode(VECNO_PTM, IM_EDGE | IM_1N);

	/* Interrupt handler definition */
	define_inthdr(VECNO_PTM, timer_handler_startup);

	/* Set cpu target */
	SetIntAssprc(VECNO_PTM, 1 << get_prid());

	/* Timer interrupt enable */
	EnableInt(VECNO_PTM, TIMER_INTLEVEL);
}

/*
 * Clear timer interrupt
 *	Clear the timer interrupt request. Depending on the type of
 *	hardware, there are two timings for clearing: at the beginning
 *	and the end of the interrupt handler.
 *	'clear_hw_timer_interrupt()' is called at the beginning of the
 *	timer interrupt handler.
 *	'end_of_hw_timer_interrupt()' is called at the end of the timer
 *	interrupt handler.
 *	Use either or both according to hardware.
 */
Inline void clear_hw_timer_interrupt( void )
{
	/* Clear timer interrupt */
	out_w(PTIMER_STATUS, 1);
}
Inline void end_of_hw_timer_interrupt( void )
{
	EndOfInt(VECNO_PTM);
}

/*
 * Timer stop processing
 *	Stop the timer operation.
 *	Called when system stops.
 */
Inline void terminate_hw_timer( void )
{
	/* Interrupt disable */
	DisableInt(VECNO_PTM);
}

/*
 * Get processing time from the previous timer interrupt to the
 * current (nanosecond)
 *	Consider the possibility that the timer interrupt occurred
 *	during the interrupt disable and calculate the processing time
 *	within the following
 *	range: 0 <= Processing time < TIMER_PERIOD * 2
 */
Inline UINT get_hw_timer_nsec( void )
{
	UW	ofs, max, unf, imask;

	DI(imask);

	max = TIMER_COUNTER;
	do {
		unf = in_w(PTIMER_STATUS) & 1;
		ofs = max - in_w(PTIMER_COUNTER);
	} while ( unf != (in_w(PTIMER_STATUS) & 1) );
	if ( unf != 0 ) {
		ofs += max;
	}

	EI(imask);

	/* ofs * 1000 / TMR_CLK_KHZ : to avoid overflow */
	return ofs * 1000 / TMR_CLK_KHZ;
}

#endif /* _TKDEV_TIMER_ */
