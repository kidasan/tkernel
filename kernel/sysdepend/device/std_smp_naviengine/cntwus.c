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
 *	cntwus.c (NAVIENGINE)
 *	Loop Count Computation of micro Second Wait
 */

#include <basic.h>
#include <sys/sysinfo.h>
#include <tk/syslib.h>
#include "tkdev_conf.h"

/*
 * Wait loop
 */
LOCAL void WaitLoop( UW count )
{
	Asm("	_loop%=:	subs	%0, %0, #1	\n"
	"			bhi	_loop%=		"
		: "=r"(count)
		: "0"(count + 1)
	);
}

/*
 * WaitUsec()  Compute loop count of micro second wait
 *	Suppose that it is executed by interrupt disable.
 */
EXPORT void CountWaitUsec( void )
{
	UW	cnt, sc, ec;

	/* Stop timer count */
	out_w(PTIMER_CONTROL, 0);

	/* Clear timer Interrupt */
	out_w(PTIMER_STATUS, 1);

	/* Initialize counter */
	out_w(PTIMER_LOAD, 0xffffffff);

	/* Start timer count : interupr disable */
	out_w(PTIMER_CONTROL, ((TIMER_PRESCALER-1) << PTCR_PRESCALER) | PTCR_ENABLE);

	/* Measure time for 110000 loops */
	sc = in_w(PTIMER_COUNTER);
	WaitLoop(110000);
	ec = in_w(PTIMER_COUNTER);
	cnt = sc - ec;

	/* Measure time for 10000 loops */
	sc = in_w(PTIMER_COUNTER);
	WaitLoop(10000);
	ec = in_w(PTIMER_COUNTER);

	/* Delete time for operation except for loops and
	   compute the net time for 100000 loops */
	cnt -= sc - ec;

	/* Stop timer */
	out_w(PTIMER_CONTROL, 0);

/* Compute number of loops for 64 micro second
 *			100000 loops
 *	loop64us = -------------------------------------- * 64 (usec)
 *		    cnt * PRESCALER * 2 / CPU_CLK (MHz)
 *	PRESCALER = 200
 *	CPU_CLK = 400 (MHz)
 */
	SCInfo.loop64us = 100000 * 64 * (CPU_CLK_KHZ / 1000 / TIMER_PRESCALER / 2) / cnt;
}
