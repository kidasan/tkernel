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
 *	@(#)call.h (libtk)
 *
 *	Kernel function call
 */

#include <basic.h>
#include <tk/syscall.h>
#include <sys/commarea.h>

#if TA_GP

IMPORT INT _CallKernelFunc( INT p1, INT p2, FP func, VP gp );

#define CallKernelFunc(func, p1, p2)	\
	_CallKernelFunc((INT)(p1), (INT)(p2), (FP)(func), __CommArea->gp)

#else /* TA_GP */

#define CallKernelFunc(func, p1, p2)	(*func)(p1, p2)

#endif /* TA_GP */
