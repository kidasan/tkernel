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
 *	patch.h (SH7786)
 *	System-dependent initialize process
 */

#ifndef _PATCH_
#define _PATCH_

/* Define '1' when using patch processes */
#define USE_SYSDEPEND_PATCH1	0
#define USE_SYSDEPEND_PATCH2	0

EXPORT void sysdepend_patch1( void );
EXPORT void sysdepend_patch2( void );

#endif /* _PATCH_ */
