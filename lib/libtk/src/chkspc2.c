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
 *	@(#)chkspc2.c (libtk)
 *
 *	Address check 
 */

#include <basic.h>
#include <tk/syslib.h>
#include <sys/segment.h>

#if VIRTUAL_ADDRESS

#include "getsvcenv.h"

/*
 * Check address space (TC string)
 *	Checks to see that the memory area from str through to
 *	either TNULL or the max character is valid.
 *	If max = 0, the number of characters (max) is ignored. 
 * 	If the memory area is valid, the value returned is either
 *	the number of characters or the max value (through to the
 *	max character with max !=0 character and no TNULL).
 *	If the memory area is not valid, the error code is returned.
 */
EXPORT INT ChkSpaceTstrR( TC *str, INT max )
{
	return ChkSpaceTstr(str, max, MA_READ, getsvcenv());
}
EXPORT INT ChkSpaceTstrRW( TC *str, INT max )
{
	return ChkSpaceTstr(str, max, MA_READ|MA_WRITE, getsvcenv());
}

#else /* VIRTUAL_ADDRESS */

LOCAL INT chklen( TC *p, INT max )
{
	INT	len = 0;

	while ( *p++ != TNULL ) {
		len++;
		if ( --max == 0 ) {
			break;
		}
	}
	return len;
}

EXPORT INT ChkSpaceTstrR( TC *str, INT max )
{
	return chklen(str, max);
}
EXPORT INT ChkSpaceTstrRW( TC *str, INT max )
{
	return chklen(str, max);
}

#endif /* VIRTUAL_ADDRESS */
