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
 *	mp_util.c (T-Kernel/OS)
 *	CPU-Dependent Peculiar routine to multi-processor system
 */

#include "kernel.h"
#include <tk/smp_util.h>
#include <sys/rominfo.h>

LOCAL _UW sync;

/*
 * Set Event
 */
EXPORT void setEvent(void)
{
	INT	n;

	Asm("sev");
	_tk_get_cfn(SCTAG_TKERPRNUM, &n, 1);
	while ( sync != (n-1) );
	sync = 0;
	mp_memory_barrier();
}

/*
 * Wait for event
 */
EXPORT void waitForEvent( void )
{
	Asm("wfe");
	atomic_inc((UW*)&sync);
	while ( sync != 0 );
}

