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
 *	@(#)cpuattr.h (tk/SH7776)
 *
 *	Type Specific Function
 */

#ifndef __TK_CPUATTR_H__
#define __TK_CPUATTR_H__

/*
 * Global pointer support
 *   0: No global pointer support
 */
#define TA_GP		0		/* No global pointer support */

/*
 * Using FPU (depend on CPU)
 *   0: not using FPU
 *   TA_COPn(n = 0-3): using FPU
 */
#define TA_FPU		TA_COP0

#endif /* __TK_CPUATTR_H__ */
