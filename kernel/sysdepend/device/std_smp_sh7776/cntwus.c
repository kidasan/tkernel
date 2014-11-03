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
 *	cntwus.c (SH7776)
 *	Loop Count Computation of micro Second Wait 
 */

#include <basic.h>
#include <sys/sysinfo.h>
#include <tk/syslib.h>
#include <tk/sysdef.h>
#include "tkdev_conf.h"

/*
 * Wait loop
 */
LOCAL void WaitLoop( UW count )
{
	Asm("	_loop%=:tst	%0, %0	\n"
	"		bf/s	_loop%=	\n"
	"		add	#-1, %0	"
		: "=r"(count)
		: "0"(count)
	);
}

/*
 * WaitUsec()  Compute loop count of micro second wait
 *	Suppose that it is executed by interrupt disable.
 */
EXPORT void CountWaitUsec( void )
{
	UW	cnt, sc, ec;
	UB	tstr;

	/* Clock module operates */
	out_w(MSTPCR0, in_w(MSTPCR0) & ~MSTPCR0_MSTP16);

	/* Stop TMU0 */
	tstr = in_b(TSTR0) & ~TSTR_STR0;
	out_b(TSTR0, tstr);

	/* Set TMU0 */
	out_h(TCR0, TCR_TPSC_P64);	/* Select clock/Disable interrupt */
	out_w(TCOR0, 0xffffffff);	/* Counter initial value */
	out_w(TCNT0, 0xffffffff);	/* Counter initial value */

	/* Start TMU0 count */
	out_b(TSTR0, tstr | TSTR_STR0);

	/* Measure time for 110000 loops */
	sc = in_w(TCNT0);
	WaitLoop(110000);
	ec = in_w(TCNT0);
	cnt = sc - ec;

	/* Measure time for 10000 loops */
	sc = in_w(TCNT0);
	WaitLoop(10000);
	ec = in_w(TCNT0);

	/* Delete time for operation except for loops and 
	   compute the net time for 100000 loops */
	cnt -= sc - ec;

	/* Stop TMU0 */
	out_b(TSTR0, tstr);

	/* Compute number of loops for 64 micro seconds
	 *			100000 loops
	 *	loop64us = ----------------------- * 64usec
	 *		    cnt * (64 dividing / Pck)
	 *	Pck = Peripheral clock (1/100MHz)
	 */
	SCInfo.loop64us = SCInfo.Pck * 1000 / cnt;
}
