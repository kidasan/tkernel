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
 *	@(#)dbgmode.c (libtk)
 *
 *	Kernel utilities 
 *	Get debug mode 
 */

#include <basic.h>
#include <sys/util.h>
#include <sys/sysinfo.h>

EXPORT BOOL _isDebugMode( void )
{
	return SCInfo.bm.c.debug;
}
