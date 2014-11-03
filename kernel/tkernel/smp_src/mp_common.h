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
 *	mp_common.h (T-Kernel/OS)
 *	Definition of SMP System
 */

#ifndef _MP_COMMON_
#define _MP_COMMON_

#include <sys/svc/tkfncd.h>
#include <sys/smp_def.h>


#define MP_SFT_PRID		((1) << MP_PRID_SHIFT)

/* Get protection level */
#define mp_get_plevel(tcb)	(((tcb)->tskatr & TA_RNG3) >> 8)

/*
 * Get task ID of caller information
 * Note : This function is called in a critical section
 */
Inline ID mp_get_calinf_tskid()
{
	ID	prid = get_prid();
	return (ctxtsk[prid]->tskid);
}

/* 
 * Get protection level of caller information
 * Note : This function is called in a critical section
 */
Inline ID mp_get_calinf_plevel()
{
	ID	prid = get_prid();
	return (mp_get_plevel(ctxtsk[prid]));
}

#endif /* _MP_COMMON_ */
