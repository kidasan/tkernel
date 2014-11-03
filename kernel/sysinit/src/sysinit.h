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
 *	sysinit.h (sysinit)
 *	Initialize System
 */

#ifndef _SYSINIT_
#define _SYSINIT_

#include <basic.h>
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

/*
 * Get system configuration information (SYSCONF)
 */
IMPORT W GetSysConf( UB *name, W *val );
IMPORT W GetSysConfStr( UB *name, UB *str );

/*
 * Get device configuration information (DEVCONF)
 */
IMPORT W GetDevConf( UB *name, W *val );
IMPORT W GetDevConfStr( UB *name, UB *str );

/*
 * Platform dependent sequence
 */
IMPORT ER init_device( void );
IMPORT ER start_device( void );
IMPORT ER finish_device( void );
IMPORT ER restart_device( W mode );
IMPORT void DispProgress( W n );

/* ------------------------------------------------------------------------ */

#define IMPORT_DEFINE   1
#if IMPORT_DEFINE
IMPORT void init_system( void );
IMPORT void start_system( void );
IMPORT void shutdown_system( INT fin );
#endif

#endif /* _SYSINIT_ */
