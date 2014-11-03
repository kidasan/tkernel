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
 *	@(#)tm_getchar.S (libtm)
 *
 *	Input single character from console 
 */
#define	_in_asm_source_

#include "tmon.h"

/*
 * INT tm_getchar( INT wait )
 */
TMCALL(tm_getchar, TM_GETCHAR)
