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
 *	@(#)smp_typedef_depend.h (tk/SH7776)
 *
 *	Definition about SH7776
 */

#ifndef __TK_SMP_TYPEDEF_DEPEND_H__
#define __TK_SMP_TYPEDEF_DEPEND_H__

#include <basic.h>
#include <sys/smp_def.h>

/*
 * Variable type of spin lock
 */
typedef UINT	T_SPLOCK;			/* Lock variable */
typedef UINT	T_RWSPLOCK;			/* Read Write Spinlock variable */

#endif /* __TK_SMP_TYPEDEF_DEPEND_H__ */
