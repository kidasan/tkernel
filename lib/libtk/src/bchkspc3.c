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
 *	@(#)bchkspc3.c (libtk)
 *
 *	Address check 
 */

#include <basic.h>
#include <sys/util.h>
#include <tk/syslib.h>

/*
 * Check address space (B string)
 *	Checks to see that the memory area from str through to
 *	either '\0' or the max byte is valid.
 *	If max = 0, the number of bytes (max) is ignored.
 *	If the memory area is valid, the value returned is either
 *	the number of bytes or the max value (through to the max
 *	byte with max !=0 and no '\0').
 *	If the memory area is not valid, the error code is returned. 
 */
EXPORT ER CheckBStrSpaceR( UB *str, W max )
{
	return ChkSpaceBstrR(str, max);
}
EXPORT ER CheckBStrSpaceRW( UB *str, W max )
{
	return ChkSpaceBstrRW(str, max);
}
