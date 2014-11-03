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
 *	@(#)sysinfo_depend.h (sys/SH7786)
 *
 *	System common information 
 *
 *   0x88000000	+-----------------------+
 *		|Vector table of	| (EXPEVT / 0x20 * 4) + 0x88000000
 *		|exception codes and	| (INTEVT / 0x20 * 4) + 0x88000000
 *		|interrupt factors	|
 *   0x88000200	+-----------------------+
 *		|Default handler	|
 *   0x88000204	+-----------------------+
 *		|TLB miss exception	| VBR+0x400 exception handler
 *		|handler		|
 *   0x88000208	+-----------------------+
 *		|TRAPA vector table	| TRA + 0x88000000
 *		|(TRAPA 0x70 - 0x7f)	|
 *   0x88000280	+-----------------------+
 *		|System common info.	|
 *   0x88000300	+-----------------------+
 *		|EIT stack (Processor 0)|
 *   0x88000600	+-----------------------+
 *		|EIT stack (Processor 1)|
 *   0x88000900	+-----------------------+
 */

#ifndef __SYS_SYSINFO_DEPEND_H__
#define __SYS_SYSINFO_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _in_asm_source_

#include <sys/smp_def.h>

/*
 * Boot mode/Operation mode
 */
typedef union {
	struct {
		UW	debug:1;	/* When in debug mode 1 */
		UW	fsrcv:1;	/* When in disk repair mode 1 */
		UW	rsv:12;		/* Reserved (always 0) */
		UW	basic:1;	/* When in basic operation mode 1 */
		UW	rsv2:17;	/* Reserved (always 0) */
	} c;
	UW	w;
} BootMode;

#define BM_DEBUG	0x00000001U	/* Debug mode */
#define BM_FSRCV	0x00000002U	/* Disk repair mode */
#define BM_BASIC	0x00004000U	/* Basic operation mode */

/*
 * System common information 
 */
typedef struct {
	VP	ramtop;		/* RAM free space top */
	VP	ramend;		/* RAM free space end */
	UB	*sysconf;	/* SYSCONF top */
	UB	*devconf;	/* DEVCONF top */
	FP	chkmem;		/* Monitor memory check function */
	UH	Ick;		/* SH7786 CPU clock (MHz) */
	UH	Pck;		/* SH7786 peripheral clock (1/100 MHz) */
	UH	Bck;		/* Bus clock (1/100 MHz) */
	UH	dck;		/* DCK clock (1/100 MHz) */
	UW	loop64us;	/* Loop count per 64 micro sec */
	BootMode bm;		/* Boot mode */
	UB	bootdev[8];	/* Boot device name */
	VW	rsv[21];	/* Reserved (always 0) */
} SysCommonInfo;

/*
 * System common area 
 */
#define N_INTVEC	128
#define N_TRAVEC	30
#define N_INTSTACK	192 * MAX_PROC		/* 2 processors */
typedef struct {
	FP		intvec[N_INTVEC];	/* EIT vector */
	FP		defaulthdr;		/* Default handler */
	FP		tlbmisshdr;		/* TLB miss exception handler */
	FP		travec[N_TRAVEC];	/* TRAPA vector */
	SysCommonInfo	scinfo;			/* System common information */
	UW		intstack[N_INTSTACK];	/* Interrupt stack area */
} SysCommonArea;

#define SCArea	( (SysCommonArea*)0x88000000 )
#define SCInfo	( SCArea->scinfo )

#endif /* _in_asm_source_ */

/*
 * Vector table addresses 
 */
#define VECTBL		0x88000000	/* Vector table top */
#define DEFAULTHDR	0x88000200	/* Default handler */
#define TLBMISSHDR	0x88000204	/* TLB miss exception handler */

#ifdef __cplusplus
}
#endif
#endif /* __SYS_SYSINFO_DEPEND_H__ */
