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
 *	inittask_main.c (sysmain)
 *	Initial Task
 */

#include "sysmain.h"
#include "kernel.h"
#include <sys/rominfo.h>
#include <sys/debug.h>

typedef INT	(*MAIN_FP)(INT, UB **);

#if USE_KERNEL_MESSAGE
LOCAL const char boot_message[] = {	/* Boot message */
	BOOT_MESSAGE
};
#endif

/* ------------------------------------------------------------------------ */

/*
 * Initial task Main
 *	The available stack size is slightly less than 8KB.
 *	The initial task is the system task,
 *	so it should not be deleted.
 */
EXPORT INT init_task_main( void )
{
	INT	fin;
	MAIN_FP	adr;

	/* Start message */
#if USE_KERNEL_MESSAGE
	tm_putstring((UB*)boot_message);
#endif

	fin = 1;
	adr = (MAIN_FP)ROMInfo->userinit;
	if ( adr != NULL ) {
		/* Perform user defined initialization sequence */
		fin = (*adr)(0, NULL);
	}
	if ( fin > 0 ) {
		/* Perform user main */
		fin = usermain();
	}
	if ( adr != NULL ) {
		/* Perform user defined finalization sequence */
		(*adr)(-1, NULL);
	}

	return fin;
}
