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
 *	@(#)waitusec.c (libtk/NAVIENGINE)
 *
 *	Busy loop wait time in micro-sec 
 */

#include <basic.h>
#include <sys/sysinfo.h>

EXPORT void WaitUsec( UINT usec )
{
	UW	count = usec * SCInfo.loop64us / 64U;

	Asm("	loop:	subs	%0, %0, #1	\n"
	"		bhi	loop		"
		: "=r"(count)
		: "0"(count + 1)
	);
}
