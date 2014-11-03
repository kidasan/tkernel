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
 *	subsystem.c (extension)
 *	Subsystem start/stop 
 */

#include "sysinit.h"

/*
 * Subsystem initialization 
 *	The initialization sequence is called before kernel startup.
 *	Normally there is no need for any processing.
 */
EXPORT ER init_subsystems( void )
{
	ER	ercd = E_OK;

	/* Describe subsystem initialization sequence */

	return ercd;
}

/*
 * Subsystem start 
 *	Called from initial task context after kernel startup.
 *	Drivers and subsystems used by the extensions are
 *	started at this point. */
EXPORT ER start_subsystems( void )
{
	ER	ercd = E_OK;

	/* Describe subsystem startup sequence */

	return ercd;
}

/*
 * End subsystems 
 */
EXPORT ER finish_subsystems( void )
{
	ER	ercd = E_OK;

	/* Describe subsystem end sequence */

	return ercd;
}
