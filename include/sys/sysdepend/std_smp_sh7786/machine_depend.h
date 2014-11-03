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
 *	@(#)machine_depend.h (sys/SH7786)
 *
 *	Machine type definition (system dependent)
 */

#ifndef __SYS_MACHINE_DEPEND_H__
#define __SYS_MACHINE_DEPEND_H__

/*
 * CPU_xxxx		CPU type
 * ALLOW_MISALIGN	1 if access to misalignment data is allowed 
 * BIGENDIAN		1 if big endian 
 * VIRTUAL_ADDRESS	1 if virtual memory 
 * ALLOCA_NOSPT		1 if alloca() is not supported 
 */

/* ----- Standard T-Engine/SH7786(SH4a) definition ----- */
#undef _STD_SMP_SH7786_

#define _STD_SMP_SH7786_	1
#define CPU_SH4			1
#define CPU_SH7786		1
#define STD_SMP_SH7786		1
#define ALLOW_MISALIGN		0
#define BIGENDIAN		0
#define VIRTUAL_ADDRESS		1
#define ALLOCA_NOSPT		0

#ifndef _Csym
#define _Csym			1
#endif

#endif /* __SYS_MACHINE_DEPEND_H__ */
