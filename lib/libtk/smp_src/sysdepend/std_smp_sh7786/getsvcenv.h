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
 *	@(#)getsvcenv.h (libtk/SH7786)
 *
 *	Get extended SVC call environment 
 */

Inline UW getsvcenv( void )
{
	UW	mdr;
	Asm("stc r2_bank, %0": "=r"(mdr));
	return mdr;
}
