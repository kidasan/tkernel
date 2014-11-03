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
 *	@(#)misc.h (sys)
 *
 *	Various functions useful for such as debug
 */

#ifndef __SYS_MISC_H__
#define __SYS_MISC_H__

#include <basic.h>
#include <tk/tkernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Task register display (libtk)
 *	Display the contents of 'gr,' 'er,' and 'cr' using 'prfn.'
 *	'prfn' needs to be a printf compatible function.
 *	Return the number of rows displayed in the return value.
 */
IMPORT W PrintTaskRegister( int (*prfn)( const char *format, ... ),
				T_REGS *gr, T_EIT *er, T_CREGS *cr );

#ifdef __cplusplus
}
#endif
#endif /* __SYS_MISC_H__ */
