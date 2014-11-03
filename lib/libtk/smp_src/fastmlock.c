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
 *	@(#)fastmlock.c (libtk)
 *
 *	High-speed exclusive control multi-lock 
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <tk/util.h>
#include <libstr.h>
#include <tk/smp_util.h>

/* ------------------------------------------------------------------------ */
/*
 *	void INC( FastMLock *lock )		increment 
 *	void DEC( FastMLock *lock )		decrement 
 *	BOOL BTS( FastMLock *lock, INT no )	bit test and set 
 *	void BR( FastMLock *lock, INT no )	bit reset 
 *
 *	The above must be operated exclusively.
 */

Inline void INC( FastMLock *lock )
{
	atomic_inc((UW*)&lock->wai);
}

Inline void DEC( FastMLock *lock )
{
	atomic_dec((UW*)&lock->wai);
}

Inline BOOL BTS( FastMLock *lock, INT no )
{
	UINT	b;
	UINT	bm = (UINT)(1 << no);

	b = (atomic_bitset((UW*)&lock->flg, bm) & bm);
	return (BOOL)b;
}

Inline void BR( FastMLock *lock, INT no )
{
	atomic_bitclr((UW*)&lock->flg, ~(UINT)(1 << no));
}

/* ------------------------------------------------------------------------ */

/*
 * Lock with wait time designation 
 *	no	lock number 0 - 31 
 */
EXPORT ER MLockTmo( FastMLock *lock, INT no, TMO tmo )
{
	UINT	ptn = (UINT)(1 << no);
	UINT	flg;
	ER	ercd;

	INC(lock);
	for ( ;; ) {
		if ( !BTS(lock, no) ) {
			ercd = E_OK;
			break;
		}

		ercd = tk_wai_flg(lock->id, ptn, TWF_ORW|TWF_BITCLR, &flg, tmo);
		if ( ercd < E_OK ) {
			break;
		}
	}
	DEC(lock);

	return ercd;
}

/*
 * Lock 
 *	no	Lock number 0 - 31 
 */
EXPORT ER MLock( FastMLock *lock, INT no )
{
	return MLockTmo(lock, no, TMO_FEVR);
}

/*
 * Lock release 
 *	no	Lock number 0 - 31 
 */
EXPORT ER MUnlock( FastMLock *lock, INT no )
{
	UINT	ptn = (UINT)(1 << no);
	ER	ercd;

	BR(lock, no);
	ercd = ( lock->wai == 0 )? E_OK: tk_set_flg(lock->id, ptn);

	return ercd;
}

/*
 * Create multi-lock 
 */
EXPORT ER CreateMLock( FastMLock *lock, UB *name )
{
	T_CFLG	cflg;
	ER	ercd;

	if ( name == NULL ) {
		cflg.exinf = NULL;
	} else {
		strncpy((char*)&cflg.exinf, (char*)name, sizeof(cflg.exinf));
	}
	cflg.flgatr  = TA_TPRI | TA_WMUL | TA_NODISWAI;
	cflg.iflgptn = 0;

	lock->id = ercd = tk_cre_flg(&cflg);
	if ( ercd < E_OK ) {
		return ercd;
	}

	lock->wai = 0;
	lock->flg = 0;

	return E_OK;
}

/*
 * Delete multi-lock 
 */
EXPORT ER DeleteMLock( FastMLock *lock )
{
	ER	ercd;

	if ( lock->id <= 0 ) {
		return E_PAR;
	}

	ercd = tk_del_flg(lock->id);
	if ( ercd < E_OK ) {
		return ercd;
	}

	lock->id = 0;

	return E_OK;
}
