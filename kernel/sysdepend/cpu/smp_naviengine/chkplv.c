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
 *	chkplv.c (NAVIENGINE)
 *	Check Call Protected Level
 */

#include "sysmgr.h"
#include <sys/sysinfo.h>
#include <tk/syslib.h>
#include <sys/kernel_util.h>

IMPORT INT	svc_call_limit;	/* SVC protected level (T-Kernel/OS) */

#define TMF_PPLOFF	(16)	/* offset of Previous Protection Level in taskmode */

Inline INT PPL( void )
{
	UINT	intsts;
	INT	res;

	DI(intsts);
	res = (INT)((SCInfo.taskmode[get_prid()] >> TMF_PPLOFF) & TPL_BIT);
	EI(intsts);

	return res;
}

/*
 * Check protected level of extended SVC caller
 */
EXPORT ER ChkCallPLevel( void )
{
	return ( PPL() > svc_call_limit )? E_OACV: E_OK;
}
