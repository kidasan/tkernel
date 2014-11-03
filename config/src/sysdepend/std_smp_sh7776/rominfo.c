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
 *	@(#)rominfo.c (SH7776)
 *
 *	ROM information
 */

#include <basic.h>
#include <sys/rominfo.h>
#include "rominfo_conf.h"

RomInfo rominfo = {
	RI_KERNEL_START,	/* Kernel startup address */
	RI_SYSCONF,		/* SYSCONF top */
	RI_DEVCONF,		/* DEVCONF top */
	RI_USERAREA_TOP,	/* RAM user area top */
	RI_USERINIT,		/* User initialization program address */
	RI_RESETINIT,		/* Reset initialization program address */
	RI_IOCONFIG,		/* I/O configuration address */
	{0},			/* Reserved (always 0) */

	RI_RDSK_TYPE,		/* ROM disk type */
	RI_RDSK_BLOCK,		/* ROM disk block size */
	RI_RDSK_START,		/* ROM disk start address */
	RI_RDSK_END,		/* ROM disk end address */
	{0}			/* Reserved (always 0) */
};

