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
 *	@(#)ssid.h (sys)
 *
 *	Subsystem ID and subsystem priority 
 *
 *	(NB) Included also from the assembler source,
 *	     so only macros can be described.
 */

#ifndef __SYS_SSID_H__
#define __SYS_SSID_H__

/*
 * T-Kernel (1 - 9)
 *	1 - 6 and 9 were previously used for other purposes.
 */
#define CONSIO_SVC	1		/* Console I/O */
#define CONSIO_PRI		1
#define DEVICE_SVC	8		/* T-Kernel/SM device management */
#define DEVICE_PRI		4
#define SYSTEM_SVC	9		/* T-Kernel/SM system management */
#define SYSTEM_PRI		1

/*
 * Subsystem (10 - )
 */
#define MM_SVC		11		/* Memory management		*/
#define MM_PRI			2
#define SM_SVC		16		/* System management		*/
#define SM_PRI			2
#define SEG_SVC		17		/* Segment management		*/
#define SEG_PRI			2

#endif /* __SYS_SSID_H__ */
