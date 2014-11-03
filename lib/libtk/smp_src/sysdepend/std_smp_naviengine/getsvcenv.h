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
 *	@(#)getsvcenv.h (libtk/NAVIENGINE)
 *
 *	Get extended SVC call environment 
 */

#include <sys/sysinfo.h>
#include <sys/kernel_util.h>

Inline UW getsvcenv( void )
{
	UINT	intsts;
	UW	res;

	DI(intsts);
	res = SCInfo.taskmode[get_prid()];
	EI(intsts);

	return res;
}

