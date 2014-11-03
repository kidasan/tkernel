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
 *	@(#)bchkspc2.c (libtk)
 *
 *	Address check 
 */

#include <basic.h>
#include <sys/util.h>
#include <tk/syslib.h>

/*
 * Address space check (TC string)
 *	Checks to see that the memory area from str through to
 *	either TNULL or the max character is valid.
 *	If max = 0, the number of characters (max) is ignored. 
 * 	If the memory area is valid, the value returned is either
 *	the number of characters or the max value (through to the
 *	max character with max !=0 character and no TNULL).
 *	If the memory area is not valid, the error code is returned.
*/
EXPORT ER CheckStrSpaceR( TC *str, W max )
{
	return ChkSpaceTstrR(str, max);
}
EXPORT ER CheckStrSpaceRW( TC *str, W max )
{
	return ChkSpaceTstrRW(str, max);
}
