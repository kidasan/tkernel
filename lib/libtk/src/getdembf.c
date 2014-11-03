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
 *	@(#)getdembf.c (libtk)
 */

#include <basic.h>
#include <sys/util.h>
#include <tk/tkernel.h>

/*
	Get message buffer for device event notice  
*/
EXPORT	ID	GetDevEvtMbf( void )
{
	T_IDEV	idev;
	tk_ref_idv(&idev);
	return idev.evtmbfid;
}
