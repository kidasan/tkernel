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
 *	@(#)bchkspc.c (libtk)
 *
 *	Address check 
 */

#include <basic.h>
#include <sys/util.h>
#include <tk/syslib.h>

/*
 * Check address space 
 *	Checks to see that the memory area from address to the
 *	len byte is valid.
 */
EXPORT ER CheckSpaceR( VP address, W len )
{
	return ChkSpaceR(address, len);
}
EXPORT ER CheckSpaceRW( VP address, W len )
{
	return ChkSpaceRW(address, len);
}
EXPORT ER CheckSpaceRE( VP address, W len )
{
	return ChkSpaceRE(address, len);
}
