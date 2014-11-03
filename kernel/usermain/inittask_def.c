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
 *	inittask_def.c (extension)
 *	Initial task definition
 */

#include "inittask_def.h"

IMPORT void init_task(void);

/*
 * Initial task creation parameter
 */
EXPORT const T_CTSK c_init_task = {
	(VP)INITTASK_EXINF,	/* exinf */
	INITTASK_TSKATR,	/* tskatr */
	(FP)&init_task,		/* task */
	INITTASK_ITSKPRI,	/* itskpri */
	INITTASK_STKSZ,		/* stksz */
	INITTASK_SSTKSZ,	/* sstksz */
	(VP)INITTASK_STKPTR,	/* stkptr */
	(VP)INITTASK_UATB,	/* uatb */
	INITTASK_LSID,		/* lsid */
	INITTASK_RESID,		/* resid */
	INITTASK_DOMID,		/* domid */
	INITTASK_ASSPRC,	/* assprc */
	INITTASK_ONAME		/* oname */
};
