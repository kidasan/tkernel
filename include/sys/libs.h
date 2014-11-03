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
 *	@(#)libs.h (sys)
 *
 *	Internal library functions
 */

#ifndef __SYS_LIBS_H__
#define __SYS_LIBS_H__

#include <basic.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Internal library error during process startup sequence 
 */
IMPORT ER _StartupError;

/*
 * TRUE for multitasking library 
 */
IMPORT BOOL _isUseMT( void );

/*
 * Common exclusion control lock in library 
 */
IMPORT ER  _lib_lock( W lockno, BOOL ignore_mintr );
IMPORT ER  _lib_locktmo( W lockno, W tmo, BOOL ignore_mintr );
IMPORT void _lib_unlock( W lockno );

/* lockno (0 - 15) */
#define _LL_MEMALLOC	0	/* Allocate memory */
#define _LL_DLL		1	/* Dynamic loader */
#define _LL_GCC		2	/* gcc run-time support */
#define _LL_TF		3	/* TRON Code Framework (libtf) */
#define _LL_LOOKUP	4	/* character search library (liblookup) */

#ifdef __cplusplus
}
#endif
#endif /* __SYS_LIBS_H__ */
