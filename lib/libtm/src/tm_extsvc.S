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
 *	@(#)tm_extsvc.S (libtm)
 *
 *	Extended service call 
 */
#define	_in_asm_source_

#include "tmon.h"

/*
 * INT tm_extsvc(INT fno, INT par1, INT par2, INT par3 )
 */
TMCALL(tm_extsvc, TM_EXTSVC)
