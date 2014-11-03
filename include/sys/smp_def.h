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
 *	@(#)smp_def.h (sys)
 *
 *	SymmetricMultiProcessor dependent definition
 */

#ifndef __SYS_SMP_DEF_H__
#define __SYS_SMP_DEF_H__

/* ===== Common definitions ============================================= */

/* Mask PROCESSORID(bit24-27) */
#define MP_PRID_MASK		(0x0f000000U)
/* Shift Number of ProcessorID to ObjParam */
#define MP_PRID_SHIFT		(24)
/* Bit pattern that indicate processor which a kernel controls */
#define ALL_CORE_BIT		(0xFFFFFFFFU >> (32 - num_proc))

/* ===== System dependencies definitions ================================ */

#include <sys/sysdepend/smp_def_common.h>

#endif /* __SYS_SMP_DEF_H__ */
