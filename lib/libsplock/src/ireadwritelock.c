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
 *	@(#)ireadlock.c (libsplock)
 *
 *	Read Lock with interrupt prohibition.
 */


#include <basic.h>
#include <tk/syslib.h>
#include <tk/smp_typedef.h>
#include <tk/smp_util.h>

IMPORT ER ReadLock( T_RWSPLOCK *lock );
IMPORT ER ReadTryLock( T_RWSPLOCK *lock );
IMPORT ER ReadUnlock( T_RWSPLOCK *lock );
IMPORT ER WriteLock( T_RWSPLOCK *lock );
IMPORT ER WriteTryLock( T_RWSPLOCK *lock );
IMPORT ER WriteUnlock( T_RWSPLOCK *lock );


EXPORT ER IReadLock( T_RWSPLOCK *lock, UINT *intsts )
{
UW	imask;

	DI( imask );
	*intsts = imask;

	return ReadLock( lock );
}

EXPORT ER IReadTryLock( T_RWSPLOCK *lock, UINT *intsts )
{
ER	rtn;
UW	imask;

	DI( imask );

	rtn = ReadTryLock( lock );
	if(rtn == E_OK){
		*intsts = imask;
	}
	else{
		EI( imask );
	}

	return rtn;
}

EXPORT ER IReadUnlock( T_RWSPLOCK *lock, UINT intsts )
{
ER	rtn;

	rtn = ReadUnlock ( lock );
	EI( intsts );

	return rtn;
}

EXPORT ER IWriteLock( T_RWSPLOCK *lock, UINT *intsts )
{
UW	imask;

	DI( imask );
	*intsts = imask;

	return WriteLock( lock );
}

EXPORT ER IWriteTryLock( T_RWSPLOCK *lock, UINT *intsts )
{
ER	rtn;
UW	imask;

	DI( imask );

	rtn = WriteTryLock( lock );
	if(rtn == E_OK){
		*intsts = imask;
	}
	else{
		EI( imask );
	}

	return rtn;
}

EXPORT ER IWriteUnlock( T_RWSPLOCK *lock, UINT intsts )
{
ER	rtn;

	rtn = WriteUnlock ( lock );
	EI( intsts );

	return rtn;
}

