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

IMPORT T_KLOCK waitlock;		/* Spin lock for system synchronization */
volatile UW	corestat = 0;


/*
 * setEvent(), waitForEvent()
 * It is a function for the same period waiting when the kernel starts. 
 */

/*
 * Set Event
 * When the boot processor completes the initialization of the kernel, it calls. 
 */
EXPORT void setEvent(void)
{
INT	n;
UW	waitptn;

	_tk_get_cfn(SCTAG_TKERPRNUM, &n, 1);
	waitptn = ((0xffffffff >> (32 - n)) & 0xfffffffe);

	while(corestat != waitptn);	/* When all other cores become waiting. */
	corestat = 0;			/* Waiting is released. */
}

/*
 * Wait for Event
 * When the initialization of a core peculiar part is finished, it calls. 
 * It becomes the setEvent call waiting of the boot processor. 
 */
EXPORT void waitForEvent(void)
{
	KernelLock(&waitlock);
	corestat |= (1 << get_prid());	/* The flag of the waiting state is set. */
	KernelUnlock(&waitlock);

	while(corestat);		/* SetEvent call waiting of boot processor. */
}

