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
 *	@(#)rominfo_conf.h (SH7786)
 *
 *	RomInfo configuration
 */

#ifndef _ROMINFO_CONF_
#define _ROMINFO_CONF_

IMPORT UB SYSCONF[], DEVCONF[];

/* Kernel address */
#define RI_KERNEL_START	(FP)0x80020000	/* Kernel start address */
#define RI_SYSCONF	(UB*)SYSCONF	/* SYSCONF top */
#define RI_DEVCONF	(UB*)DEVCONF	/* DEVCONF top */

/* User definition */
#define RI_USERAREA_TOP	(VP)0x90000000	/* RAM user area top */
#define RI_USERINIT	(FP)NULL	/* User initialization program */
#define RI_RESETINIT	(FP)NULL	/* Reset initialization program */

/* Rom disk */
#define RI_RDSK_TYPE	(UW)0		/* ROM disk type */
#define RI_RDSK_BLOCK	(UW)0		/* ROM disk block size */
#define RI_RDSK_START	(UW)0		/* ROM disk start address */
#define RI_RDSK_END	(UW)0		/* ROM disk end address */

/* System dependence */
#define RI_IOCONFIG	(UW*)NULL	/* I/O configuration address */

#endif /* _ROMINFO_CONF_ */
