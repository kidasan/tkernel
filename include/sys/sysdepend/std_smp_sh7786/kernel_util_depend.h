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
 *	@(#)kernel_util_depend.h (sys/SH7786)
 *
 *	Kernel Utilities for SH7786
 */

#ifndef	__SYS_KERNEL_UTIL_DEPEND_H__
#define __SYS_KERNEL_UTIL_DEPEND_H__

#include <basic.h>
#include <tk/sysdef.h>
#include <tk/errno.h>
#include <tk/smp_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Get processor id
 */
Inline UINT get_prid( void )
{
	UINT	prid;

	prid = *((UW*)CPIDR);

	return prid;
}

/*
 * Counting Spin Lock
 */
typedef struct t_klock{
	UINT	lock __attribute__ ((aligned (32)));	/* Lock variable */
	B	dumy[32 - sizeof(UINT)];
} T_KLOCK;

/*
 * Structure type of spin lock
 */
typedef struct t_countklock{
	T_KLOCK		lock;			/* Lock variable */
	UW 		SpinLockCnt[MAX_PROC];	/* Lock counter */
} T_COUNTLOCK;


IMPORT ER KernelLock( T_KLOCK *lock );
IMPORT ER KernelTryLock( T_KLOCK *lock );
IMPORT ER KernelUnlock( T_KLOCK *lock );
IMPORT ER InitKernelLock( T_KLOCK *lock );

IMPORT ER CountSpinLock( T_COUNTLOCK *lock );
IMPORT ER CountSpinTryLock( T_COUNTLOCK *lock );
IMPORT ER CountSpinUnlock( T_COUNTLOCK *lock );
IMPORT ER InitCountSpinLock( T_COUNTLOCK *lock );

#ifdef __cplusplus
}
#endif
#endif /* __SYS_KERNEL_UTIL_DEPEND_H__ */
