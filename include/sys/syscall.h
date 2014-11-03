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
 *	@(#)syscall.h (sys)
 *
 *	System call interface common definition 
 */

#ifndef __SYS_SYSCALL_H__
#define __SYS_SYSCALL_H__

#include <basic.h>
#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function number
 */
typedef struct {
#if BIGENDIAN
	UW	funcno:16;	/* Function number in SVC */
	UW	parsize:8;	/* Parameter W size */
	UW	svcno:8;	/* SVC number */
#else
	UW	svcno:8;	/* SVC number */
	UW	parsize:8;	/* Parameter W size */
	UW	funcno:16;	/* Function number in SVC */
#endif
} EFN;

typedef union {
	EFN	efn;
	UW	w;
} FunctionNumber;

/*
 * Command packet format (header portion)
 * 	Optional data is added after this header if necessary.
 */
typedef struct {
	QUEUE		q;		/* For queue connection */
	FunctionNumber	fno;		/* Function number */
	ID		tid;		/* Call task ID */
	W		ret;		/* Return value */
	VP		para;		/* Pointer to argument list */
} SyscallCmdPacket;

/*
 * Rendezvous call classification  
 */
typedef enum {
	Syscall_APL	= 0x00000001,	/* Call from application */
	Syscall_OS	= 0x00000002,	/* Call from within OS */
	Syscall_OWN	= 0x00000004	/* Self-originated redirect,
					   re-call, or other */
} SyscallPattern;

#ifdef __cplusplus
}
#endif
#endif /* __SYS_SYSCALL_H__ */
