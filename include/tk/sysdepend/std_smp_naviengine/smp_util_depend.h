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
 *	@(#)smp_util_depend.h (SMP T-Kernel)
 *
 *	General Utilities for NAVIENGINE
 */

#ifndef	__TK_SMP_UTIL_DEPENDH__
#define __TK_SMP_UTIL_DEPENDH__

#include <basic.h>
#include <tk/sysdef.h>
#include <tk/errno.h>
#include <tk/smp_typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Spin lock (libsplock)
 */
/* Initialize */
IMPORT ER	InitSpinLock( T_SPLOCK *lock );
/* Lock / TryLock / Unlock */
IMPORT ER	SpinLock( T_SPLOCK *lock );
IMPORT ER	SpinTryLock( T_SPLOCK *lock );
IMPORT ER	SpinUnlock( T_SPLOCK *lock );
/* With interrupt prohibition */
IMPORT ER	ISpinLock( T_SPLOCK *lock, UINT *intsts );
IMPORT ER	ISpinTryLock( T_SPLOCK *lock, UINT *intsts );
IMPORT ER	ISpinUnlock( T_SPLOCK *lock, UINT intsts );

/*
 * Read write spin lock (libsplock)
 */
/* Read lock */
/* Initialize */
IMPORT ER	InitRWSpinLock( T_RWSPLOCK *lock );
/* Lock / TryLock / Unlock */
IMPORT ER	ReadLock( T_RWSPLOCK *lock );
IMPORT ER	ReadTryLock( T_RWSPLOCK *lock );
IMPORT ER	ReadUnlock( T_RWSPLOCK *lock );
/* With interrupt prohibition */
IMPORT ER	IReadLock( T_RWSPLOCK *lock, UINT *intsts );
IMPORT ER	IReadTryLock( T_RWSPLOCK *lock, UINT *intsts );
IMPORT ER	IReadUnlock( T_RWSPLOCK *lock, UINT intsts );

/* Write lock */
/* Lock / TryLock / Unlock */
IMPORT ER	WriteLock( T_RWSPLOCK *lock );
IMPORT ER	WriteTryLock( T_RWSPLOCK *lock );
IMPORT ER	WriteUnlock( T_RWSPLOCK *lock );
/* With interrupt prohibition */
IMPORT ER	IWriteLock( T_RWSPLOCK *lock, UINT *intsts );
IMPORT ER	IWriteTryLock( T_RWSPLOCK *lock, UINT *intsts );
IMPORT ER	IWriteUnlock( T_RWSPLOCK *lock, UINT intsts );

/*
 * Mp memory barrier
 */
/* Data memory barrier */
#define DMB()	Asm("mcr p15, 0, %0, c7, c10, 5" : : "r"(0));
#define mp_memory_barrier()	DMB()

/*
 * Atomic operation (libatomic)
 */
IMPORT UW atomic_inc( UW *addr );
IMPORT UW atomic_dec( UW *addr );
IMPORT UW atomic_add( UW *addr, UW val );
IMPORT UW atomic_sub( UW *addr, UW val );
IMPORT UW atomic_xchg( UW *addr, UW val );
IMPORT UW atomic_cmpxchg( UW *addr, UW val, UW cmp );
IMPORT UW atomic_bitset( UW *addr, UW setptn );
IMPORT UW atomic_bitclr( UW *addr, UW clrptn );

#ifdef __cplusplus
}
#endif
#endif /* __TK_SMP_UTIL_DEPENDH__ */
