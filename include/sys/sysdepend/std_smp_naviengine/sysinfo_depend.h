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
 *	@(#)sysinfo_depend.h (sys/NAVIENGINE)
 *
 *	System common information
 *
 * 0x80004000 +------------------------+
 *            | Exception vector table |
 * 0x80004400 +------------------------+
 *            | System common info     |
 * 0x80004460 +------------------------+
 *
 */

#ifndef __SYS_SYSINFO_DEPEND_H__
#define __SYS_SYSINFO_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _in_asm_source_

/*
 * Boot mode/Operation mode
 */
typedef union {
	struct {
		UW	debug:1;		/* When in debug mode 1 */
		UW	fsrcv:1;		/* When in disk repair mode 1 */
		UW	rsv:12;			/* Reserved (always 0) */
		UW	basic:1;		/* When in basic operation mode 1 */
		UW	rsv2:17;		/* Reserved (always 0) */
	} c;
	UW	w;
} BootMode;

#define BM_DEBUG	0x00000001U		/* Debug mode */
#define BM_FSRCV	0x00000002U		/* Disk repair mode */
#define BM_BASIC	0x00004000U		/* Basic operation mode */

/*
 * System shared information
 */
#include <sys/smp_def.h>
typedef struct {
	VP		ramtop;			/* RAM free space top */
	VP		ramend;			/* RAM free space end */
	UB		*sysconf;		/* SYSCONF top */
	UB		*devconf;		/* DEVCONF top */
	W		taskindp[MAX_PROC];	/* Task independent context flag */
	UW		taskmode[MAX_PROC];	/* Task mode flag */
	UH		Cclk;			/* CPU clock (1/100 MHz) */
	UH		Pclk1;			/* Peripheral clock1 (1/100 MHz) */
	UH		Pclk2;			/* PeriPheral clock2 (1/100 MHz) */
	UH		Pclk3;			/* PeriPheral clock3 (1/100 MHz) */
	UW		loop64us;		/* Loop count per 64 micro sec */
	BootMode	bm;			/* Boot mode */
	UB		bootdev[8];		/* Boot device name */
} SysCommonInfo;

/*
 * System common area 
 */
#define N_INTVEC	256
typedef struct {
	FP		intvec[N_INTVEC];	/* EIT vector */
	SysCommonInfo	scinfo;			/* System common information */
} SysCommonArea;

#define SCArea		( (SysCommonArea*)0x80004000 )
#define SCInfo		( SCArea->scinfo )

#endif /* _in_asm_source_ */

/* Definitions used by assembler */
#define SCINFO		0x80004400
#define RAM_TOP		( SCINFO + 0*4 )	/* RAM free space top */
#define RAM_END		( SCINFO + 1*4 )	/* RAM free space end */
#ifdef _in_asm_source_
#define	SYSCONF		( SCINFO + 2*4 )	/* SYSCONF top */
#endif /* _in_asm_source_ */
#define TASKINDP	( SCINFO + 4*4 )	/* Task independent context flag */
#define TASKMODE	( TASKINDP + 4*4 )	/* Task mode flag */

/*
 * Vector table address
 */
#define EIT_VECTBL	0x80004000		/* Vector table top */

#define EITVEC(n)	( EIT_VECTBL + (n) * 4 )

#define EIT_DEFAULT	0			/* Default handler */
#define EIT_UNDEF	1			/* Undefined instruction */
#define EIT_IABORT	2			/* Pre-fetch abort */
#define EIT_DABORT	3			/* Data abort */
#define EIT_FIQ		31			/* High-speed interrupt (FIQ) */

#define EIT_IRQ(n)	( 32 + (n) )		/* INT 0-127 */
#define EIT_MPSWI(n)	( EIT_IRQ(n) )		/* MP SWINT 0-15 */
#define EIT_GPIO(n)	( 160 + (n) )		/* GPIO 0-31 */
#define EIT_END		( N_INTVEC - 1 )

#ifdef __cplusplus
}
#endif
#endif /* __SYS_SYSINFO_DEPEND_H__ */
