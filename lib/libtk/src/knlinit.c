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
 *	@(#)knlinit.c (libtk)
 *
 *	KnlInit() is always linked as it is called from the
 *	manager startup part.
 *	Note that adding too many processing can make the
 *	program quite large.
 */

#include <basic.h>
#include <sys/commarea.h>
#include <tk/sysmgr.h>

/*
 * Kernel utility initialization 
 */
EXPORT void KnlInit( void )
{
	if ( __CommArea == NULL ) {
		/* Get kernel common data area */
		_GetKernelCommonArea(&__CommArea);
	}
}
