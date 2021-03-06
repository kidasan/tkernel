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
 *	@(#)_putchar.c (libtm)
 */

#include <tm/tmonitor.h>
#include <sys/consio.h>

EXPORT int _PutChar( int c )
{
	return ( tm_putchar(c) < 0 )? -1: 1;
}
