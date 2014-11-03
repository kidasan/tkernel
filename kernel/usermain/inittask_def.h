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
 *	inittask_def.h (extension)
 *	Initial task definition
 */

#ifndef _INITTASK_DEF_
#define _INITTASK_DEF_

#include <basic.h>
#include <tk/tkernel.h>

/*
 * Initial task parameter
 */
#define INITTASK_EXINF		(0)
#define INITTASK_TSKATR		(TA_HLNG | TA_RNG0)
#define INITTASK_ITSKPRI	(1)
#define INITTASK_STKSZ		(8 * 1024)
#define INITTASK_SSTKSZ		(0)
#define INITTASK_STKPTR		(NULL)
#define INITTASK_UATB		(NULL)
#define INITTASK_LSID		(0)
#define INITTASK_RESID		(0)
#define INITTASK_DOMID		(0)
#define INITTASK_ASSPRC		(0)
#define INITTASK_ONAME		""

#endif /* _INITTASK_DEF_ */
