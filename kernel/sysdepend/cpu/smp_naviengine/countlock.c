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
 *	@(#)countlock.c (T-Kernel/OS)
 *
 *	Counting Spin Lock.
 */


#include <basic.h>
#include <tk/smp_typedef.h>
#include <tk/smp_util.h>
#include <sys/smp_def.h>
#include <sys/kernel_util.h>

/*
 * ER CountSpinLock( T_COUNTLOCK *lock )
 *
 * The lock is acquired. 
 * It busy loops until the lock can been acquired by other processors 
 *  and it learns and , be acquired.
 * The counter is done in the increment if it has locked with 
 *  the same processor, and E_OK is returned. 
 *
 */
EXPORT ER CountSpinLock( T_COUNTLOCK *lock )
{
	ER	ercd;
	UINT	prid = get_prid();

	if ( lock->SpinLockCnt[prid] != 0 ) {
		lock->SpinLockCnt[prid]++;
		return E_OK;
	}

	ercd = SpinLock(&lock->lock);
	lock->SpinLockCnt[prid]++;

	return E_OK;
}

/*
 * ER CountSpinLock( T_COUNTLOCK *lock )
 *
 * The lock is acquired. 
 * E_BUSY is returned if it has locked with other processors. 
 * The counter is done in the increment if it has locked with 
 *  the same processor, and E_OK is returned. 
 *
 */
EXPORT ER CountSpinTryLock( T_COUNTLOCK *lock )
{
	ER	ercd;
	UINT	prid = get_prid();

	if ( lock->SpinLockCnt[prid] != 0 ) {
		lock->SpinLockCnt[prid]++;
		return E_OK;
	}

	ercd = SpinTryLock(&lock->lock);
	if ( ercd == E_OK ) {
		lock->SpinLockCnt[prid]++;
	}

	return ercd;
}

/*
 * ER CountSpinUnlock( T_COUNTLOCK *lock )
 *
 * The lock is returned. 
 * The counter is decreased if two or more locks have been acquired by 
 *  the same processor, and E_OK is returned. 
 *
 */
EXPORT ER CountSpinUnlock( T_COUNTLOCK *lock )
{
	UINT	prid = get_prid();

	if ( lock->SpinLockCnt[prid] > 1 ) {
		lock->SpinLockCnt[prid]--;
		return E_OK;
	}

	lock->SpinLockCnt[prid]--;
	SpinUnlock(&lock->lock);

	return E_OK;
}

EXPORT ER InitCountSpinLock( T_COUNTLOCK *lock )
{
	INT	i;

	InitSpinLock(&lock->lock);
	for ( i = 0; i < MAX_PROC; i++ ) {
		lock->SpinLockCnt[i] = 0;
	}
	mp_memory_barrier();

	return E_OK;
}

