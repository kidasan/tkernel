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
 *	smpkernel.h (T-Kernel/OS)
 *	SMP T-Kernel Common Definition
 */

#ifndef _SMPKERNEL_
#define _SMPKERNEL_

/*
 * Maximum ID number of each object
 *	1 - max_??? is valid ID.
 */
IMPORT ID	max_domid;

/*
 * System initialization (each module)
 */
IMPORT ER mp_domain_initialize( void );

/*
 * Inter-processor Interrupt
 */
IMPORT void mp_int_init( void );

#endif /* _SMPKERNEL_ */
