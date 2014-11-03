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
 *	@(#)waitnsec.c (libtk/SH7786)
 *
 *	Busy loop wait time in nanoseconds 
 */

#include <basic.h>
#include <sys/sysinfo.h>

EXPORT void WaitNsec( UINT nsec )
{
	UW	count = nsec * SCInfo.loop64us / 64000U;

	Asm("	loop%=:	tst	%0, %0	\n"
	"		bf/s	loop%=	\n"
	"		add	#-1, %0	"
		: "=r"(count)
		: "0"(count)
	);
}
