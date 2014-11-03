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
 *	@(#)machine_depend.h (sys/NAVIENGINE)
 *
 *	Machine type definition
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

/* ----- T-Engine/NAVIENGINE(ARM11MPCORE) definition ----- */
#undef _STD_SMP_NAVIENGINE_

#define _STD_SMP_NAVIENGINE_	1
#define CPU_ARM6K		1
#define CPU_ARM11MPCORE		1
#define CPU_NAVIENGINE		1
#define STD_SMP_NAVIENGINE	1
#define ALLOW_MISALIGN		0
#define BIGENDIAN		0
#define VIRTUAL_ADDRESS		1
#define ALLOCA_NOSPT		0

#ifndef _Csym
#define _Csym			0
#endif

#endif /* __SYS_MACHINE_DEPEND_H__ */
