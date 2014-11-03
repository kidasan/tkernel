/*
 *----------------------------------------------------------------------------
 *    Boot Sample
 *
 *    Version: 1.00.00
 *----------------------------------------------------------------------------
 *    Description of license
 *    Original Source:   Copyright (C) 2009 T-Engine Forum. 
 *    This software is distributed under the T-Engine Public License.
 *    Refer to T-Engine Public License at http://www.t-engine.org for use
 *    conditions and redistribution conditions. 
 *    Modification and redistribution are allowed to this software, in
 *    accordance with T-Engine Public License. 
 *    Therefore, the modification may be added to the program distributed by
 *    T-Engine forum.
 *    The original source is available for download from the download page at
 *    http://www.t-engine.org.
 *----------------------------------------------------------------------------
 */

/*
 *	sysdepend.h	Boot
 *
 *	System-dependent definition (for NAVIENGINE)
 */
#include <machine.h>
#include "navienginedef.h"

/* System memory map */
#define ROM_START	0x00000000
#define ROM_END		0x04000000
#define RAM_START	0x80000000
#define RAM_END		0x90000000

#define BOOT_START	(ROM_START)
#define BOOT_END	(BOOT_START + 0x00020000)

/* RAM free space end */
#define MIN_RAM_END	(RAM_START + 0x80000)	/* Minimum RAM_END (512KB) */
#define DEF_RAM_END	(RAM_END)

/*
 * System information
 */
#define N_EITVEC	256
typedef struct {
	VW	pagetbl[0x1000];	/* 0000: First-level page table        */
	VP	eitvec[N_EITVEC];	/* 4000: EIT vector                    */
					/* 4400: System shared information     */
	VP	ramtop;			/* 4400: RAM free space top            */
	VP	ramend;			/* 4404: RAM free space end            */
	UB	*sysconf;		/* 4408: SYSCONF top                   */
	UB	*devconf;		/* 440C: DEVCONF top                   */
	W	taskindp[4];		/* 4410: Task independent context flag */
	UW	taskmode[4];		/* 4420: Task mode flag                */
	UH	Cclk;			/* 4430: CPU clock (1/100 MHz)         */
	UH	Pclk1;			/* 4432: Peripheral clock1 (1/100 MHz) */
	UH	Pclk2;			/* 4434: Peripheral clock2 (1/100 MHz) */
	UH	Pclk3;			/* 4436: Peripheral clock3 (1/100 MHz) */
	VW	osrsv[4];		/* 4438: Reserved                      */
	VW	rsv[6];			/* 4448: Reserved                      */
	UB	work[0x6000 - 0x4460];	/* 4460-5FFF: Work area                */
} SystemInfo;

#define sysInfo		((SystemInfo*)RAM_START)
#define RAM_TOP		(RAM_START + sizeof(SystemInfo))

/*
 * ROM information
 */
typedef struct {
	FP	kernel;			/* Kernel startup address               */
	UB	*sysconf;		/* SYSCONF top                          */
	UB	*devconf;		/* DEVCONF top                          */
	VP	userarea;		/* RAM user area top                    */
	FP	userinit;		/* User initialization program address  */
	FP	resetinit;		/* Reset initialization program address */
	VW	rsv[10];		/* Reserved (always 0)                  */
	UW	rd_type;		/* ROM disk type                        */
	UW	rd_blksz;		/* ROM disk block size                  */
	UW	rd_saddr;		/* ROM disk start address               */
	UW	rd_eaddr;		/* ROM disk end address                 */
	VW	rsv2[12];		/* Reserved (always 0)                  */
} RomInfo;

#define romInfo		((RomInfo*)(BOOT_END))

/* Clock */
#define CPU_CLK_KHZ		(400*1000)	/* CPU Clock 400 MHz */
#define TIMER_PRESCALER		(200)

/* Serial */
#define DEBUG_PORT_NUM		(0)
#define DEBUG_PORT_BPS		(38400)

/*
 * I/O port access
 */
Inline	void	out_w(INT port, UW data)
{
	*(_UW*)port = data;
}
Inline	void	out_h(INT port, UH data)
{
	*(_UH*)port = data;
}
Inline	void	out_b(INT port, UB data)
{
	*(_UB*)port = data;
}
Inline	UW	in_w(INT port)
{
	return *(_UW*)port;
}
Inline	UH	in_h(INT port)
{
	return *(_UH*)port;
}
Inline	UB	in_b(INT port)
{
	return *(_UB*)port;
}

/*
 * Get processor id
 */
Inline UW get_prid( void )
{
	UW	prid;

	Asm("mrc p15, 0, %0, c0, c0, 5":"=r"(prid));
	return (prid & 0x0F);
}

/*
 * External function
 */
/* eitent_smp_naviengine.S */
IMPORT	void	_resetHdr(void);
IMPORT	void	_irqGPIOHdr(void);
IMPORT	void	_defaultHdr(void);
IMPORT	void	_svcHdr(void);

/* reset.c */
IMPORT	void	procReset(void);
