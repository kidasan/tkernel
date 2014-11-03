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
 *	@(#)ispinlock.c (libsplock)
 *
 *	Spin Lock with interrupt prohibition.
 */


#include <basic.h>
#include <tk/syslib.h>
#include <tk/smp_typedef.h>
#include <tk/smp_util.h>

IMPORT ER SpinLock( T_SPLOCK *lock );
IMPORT ER SpinTryLock( T_SPLOCK *lock );
IMPORT ER SpinUnlock( T_SPLOCK *lock );


EXPORT ER ISpinLock( T_SPLOCK *lock, UINT *intsts )
{
UW	imask;

	DI( imask );
	*intsts = imask;

	return SpinLock( lock );
}

EXPORT ER ISpinTryLock( T_SPLOCK *lock, UINT *intsts )
{
ER	rtn;
UW	imask;

	DI( imask );

	rtn = SpinTryLock( lock );
	if(rtn == E_OK){
		*intsts = imask;
	}
	else{
		EI( imask );
	}

	return rtn;
}

EXPORT ER ISpinUnlock( T_SPLOCK *lock, UINT intsts )
{
ER	rtn;

	rtn = SpinUnlock ( lock );
	EI( intsts );

	return rtn;
}

