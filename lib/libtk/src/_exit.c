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
 *	@(#)_exit.c (libtk)
 */

#include <basic.h>
#include <tk/syscall.h>

#ifdef __GNUC__
extern void tk_ext_tsk(void) __attribute__ ((noreturn));
#else
extern void tk_ext_tsk(void);
#endif

EXPORT void _exit(int status)
{
	tk_ext_tsk();
}
