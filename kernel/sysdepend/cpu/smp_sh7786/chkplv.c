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
 *	chkplv.c (SH7786)
 *	Call Protected Level Test
 */

#include "sysmgr.h"

IMPORT INT	svc_call_limit;	/* SVC protected level (T-Kernel/OS) */

#define TMF_PPLOFF	(16)	/* offset of Previous Protection Level in taskmode */

Inline INT PPL( void )
{
	UW	mdr;
	Asm("stc r2_bank, %0": "=r"(mdr));
	return (mdr >> TMF_PPLOFF) & TPL_BIT;
}

/*
 * Check protected level of extended SVC caller
 */
EXPORT ER ChkCallPLevel( void )
{
	return ( PPL() > svc_call_limit )? E_OACV: E_OK;
}
