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
 *	@(#)chkspc.c (libtk)
 *
 *	Address check 
 */

#include <basic.h>
#include <tk/syslib.h>
#include <sys/segment.h>

#if VIRTUAL_ADDRESS

#include "getsvcenv.h"

/*
 * Check address space 
 *	Checks to see that the memory space from address to the
 *	len byte is valid. 
 */
EXPORT ER ChkSpaceR( VP addr, INT len )
{
	return ChkSpace(addr, len, MA_READ, getsvcenv());
}
EXPORT ER ChkSpaceRW( VP addr, INT len )
{
	return ChkSpace(addr, len, MA_READ|MA_WRITE, getsvcenv());
}
EXPORT ER ChkSpaceRE( VP addr, INT len )
{
	return ChkSpace(addr, len, MA_READ|MA_EXECUTE, getsvcenv());
}

#else /* VIRTUAL_ADDRESS */

EXPORT ER ChkSpaceR( VP addr, INT len )
{
	return E_OK;
}
EXPORT ER ChkSpaceRW( VP addr, INT len )
{
	return E_OK;
}
EXPORT ER ChkSpaceRE( VP addr, INT len )
{
	return E_OK;
}

#endif /* VIRTUAL_ADDRESS */
