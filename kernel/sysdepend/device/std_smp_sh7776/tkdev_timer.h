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
 *	tkdev_timer.h (SH7776)
 *	Hardware-Dependent Timer Processing
 */

#ifndef _TKDEV_TIMER_
#define _TKDEV_TIMER_

#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <sys/sysinfo.h>
#include "tkdev_conf.h"

/*
 * Settable interval range (millisecond)
 */
#define MIN_TIMER_PERIOD	1
#define MAX_TIMER_PERIOD	50

/*
 * Timer control register setting value
 *	Interrupt enable, 16 dividing
 */
#define TCR0_INIT	( TCR_UNIE | TCR_TPSC_P16 )

/*
 * Set timer
 */
Inline void init_hw_timer( void )
{
	UB	tstr;
	UW	cnt;

	/* Clock module operates */
	out_w(MSTPCR0, in_w(MSTPCR0) & ~MSTPCR0_MSTP16);

	/* Stop timer */
	tstr = in_b(TSTR0) & ~TSTR_STR0;
	out_b(TSTR0, tstr);

	/* Set timer mode */
	out_h(TCR0, TCR0_INIT);

	/* Set counter */
	cnt = TIMER_PERIOD * (UW)SCInfo.Pck * 10 / 16 - 1;
	out_w(TCOR0, cnt);
	out_w(TCNT0, cnt);

	/* Start timer count */
	out_b(TSTR0, tstr | TSTR_STR0);
}

/*
 * Timer start processing
 *	Initialize the timer and start the periodical timer interrupt.
 */
Inline void start_hw_timer( void )
{
IMPORT	void	timer_handler_startup( void );
	UW	ipri;

	/* Set timer */
	init_hw_timer();

	/* Interrupt handler definition */
	define_inthdr(VECNO_TMU0, timer_handler_startup);

	/* Timer interrupt enable */
	ipri = in_w(C0INT2PRI1);
	ipri = (ipri & ~(INTLEVEL_CLR << TMU0_SFT)) | (TMU0_PRI << TMU0_SFT);
	out_w(C0INT2PRI1, ipri);

	out_w(C0INT2MSKCLR1, INT2MSKCLR1_TMU0);
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
 *	Use either or both depending on hardware.
 */
Inline void clear_hw_timer_interrupt( void )
{
	/* Clear underflow flag */
	out_h(TCR0, TCR0_INIT);
}
Inline void end_of_hw_timer_interrupt( void )
{
	/* Nothing required to do at this point */
}

/*
 * Timer stop processing
 *	Stop the timer operation.
 *	Called when system stops.
 */
Inline void terminate_hw_timer( void )
{
	UW	ipri;

	/* Timer interrupt disable */
	ipri = in_w(C0INT2PRI1);
	ipri = (ipri & ~(INTLEVEL_CLR << TMU0_SFT));
	out_w(C0INT2PRI1, ipri);
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
	UW	ofs, max;
	UH	unf;

	max = in_w(TCOR0) + 1;
	do {
		unf = in_h(TCR0) & TCR_UNF;
		ofs = max - in_w(TCNT0);
	} while ( unf != (in_h(TCR0) & TCR_UNF) );
	if ( unf != 0 ) {
		ofs += max;
	}

	return ofs * (100 * 16) / SCInfo.Pck * 1000;
}

#endif /* _TKDEV_TIMER_ */
