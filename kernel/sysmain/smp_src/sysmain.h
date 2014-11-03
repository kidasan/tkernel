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
 *	sysmain.h (sysmain)
 *	Kernel Main
 */

#ifndef _SYSMAIN_
#define _SYSMAIN_

#include <basic.h>
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

/* Boot message */
#define BOOT_MESSAGE \
	"\n" \
	"SMP T-Kernel Version 1.00.01\n" \
	"\n\0"

/*
 Display the progress of start processing (sysinit)
 *	Display the value specified by 'n' on display or LED.
 *	Some platform may not support this function.
 */
IMPORT void DispProgress( W n );

/*
 * User main
 */
IMPORT INT usermain( void );

#endif /* _SYSMAIN_ */
