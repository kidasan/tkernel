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
 *	@(#)kernel_util_depend.h (sys/NAVIENGINE)
 *
 *	Kernel Utilities for NAVIENGINE
 */

#ifndef	__SYS_KERNEL_UTIL_DEPEND_H__
#define __SYS_KERNEL_UTIL_DEPEND_H__

#include <basic.h>
#include <tk/smp_typedef.h>
#include <sys/smp_def.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Get processor id
 */
Inline UINT get_prid( void )
{
	UINT	prid;

	Asm("mrc p15, 0, %0, c0, c0, 5":"=r"(prid));
	return (prid & 0x0F);
}

/*
 * Counting Spin Lock
 */
typedef struct t_countlock{
	T_SPLOCK	lock;			/* Lock variable */
	UW 		SpinLockCnt[MAX_PROC];	/* Lock counter */
} T_COUNTLOCK;

IMPORT ER	CountSpinLock( T_COUNTLOCK *lock );
IMPORT ER	CountSpinTryLock( T_COUNTLOCK *lock );
IMPORT ER	CountSpinUnlock( T_COUNTLOCK *lock );
IMPORT ER	InitCountSpinLock( T_COUNTLOCK *lock );

#ifdef __cplusplus
}
#endif
#endif /* __SYS_KERNEL_UTIL_DEPEND_H__ */
