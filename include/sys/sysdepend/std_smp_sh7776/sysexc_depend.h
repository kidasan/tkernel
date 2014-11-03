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
 *	@(#)sysexc_depend.h (sys/SH7776)
 *
 *	System exception processing (SH7776)
 */

#ifndef __SYS_SYSEXC_DEPEND_H__
#define __SYS_SYSEXC_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/* System exception message */
typedef struct {
	W	type;		/* Message type (MS_SYS1) */
	W	size;		/* Message size */
	ID	taskid;		/* Exception generated task ID */
	ID	procid;		/* Exception generated process ID */
	UW	exccode;	/* Exception code (EXPEVT, INTEVT2) */
	UW	excinfo;	/* Exception information (TRA, INTEVT) */
	UW	excaddr;	/* Exception address (TEA) */
} EXCMESG;

#ifdef __cplusplus
}
#endif

#endif /* __SYS_SYSEXC_DEPEND_H__ */
