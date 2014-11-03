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
 *	@(#)util.h (T-Kernel)
 *
 *	General Utilities 
 */

#ifndef	__TK_UTIL_H__
#define __TK_UTIL_H__

#include <basic.h>
#include <tk/typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Fast Lock
 */
typedef struct {
	W		cnt;
	ID		id;
} FastLock;

IMPORT ER CreateLock( FastLock *lock, UB *name );
IMPORT void DeleteLock( FastLock *lock );
IMPORT void Lock( FastLock *lock );
IMPORT void Unlock( FastLock *lock );

/*
 * Multi Lock
 *	Can use the maximum of 32 independent locks with a single FastMLock.
 *	Divided by the lock number (no). Can specify 0-31 for 'no.'
 *	(Slightly less efficient than FastLock)
 */
typedef struct {
	UINT		flg;
	INT		wai;
	ID		id;
} FastMLock;

IMPORT ER CreateMLock( FastMLock *lock, UB *name );
IMPORT ER DeleteMLock( FastMLock *lock );
IMPORT ER MLockTmo( FastMLock *lock, INT no, TMO tmout );
IMPORT ER MLock( FastMLock *lock, INT no );
IMPORT ER MUnlock( FastMLock *lock, INT no );

/*
 * 4-character object name
 *	(Example)
 *	T_CTSK	ctsk;
 *	SetOBJNAME(ctsk.exinf, "TEST");
 */
union objname {
	char	s[4];
	VP	i;
};

#define SetOBJNAME(exinf, name)					\
	{							\
		const static union objname _nm = { name };	\
		exinf = _nm.i;					\
	}

#ifdef __cplusplus
}
#endif
#endif /* __TK_UTIL_H__ */
