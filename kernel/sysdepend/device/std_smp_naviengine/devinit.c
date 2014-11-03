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
 *	devinit.c (NAVIENGINE)
 *	Device-Dependent Initialization
 */

#include "sysinit.h"
#include "kernel.h"
#include <tk/sysdef.h>
#include <sys/sysinfo.h>
#include <sys/rominfo.h>
#include <sys/util.h>
#include <device/devconf.h>
#include <libstr.h>
#include "smpcall.h"

EXPORT	UB	*BootDeviceName;	/* Boot device name */
LOCAL	UB	bootdevnm[L_DEVNM + 1];

/* Saved data of system common information */
EXPORT	SysCommonInfo	SaveSCInfo;

/*
 * Display the progress of start processing
 *	0x10 - 0x1F : Initialization (before kernel starts)
 *	0x20 - 0x2F : Kernel start processing
 *	0x30 - 0x3F : Start processing (after kernel starts)
 */
EXPORT void DispProgress( W n )
{
}

/* ------------------------------------------------------------------------ */

/*
 * Initialization at ROM startup
 *	This function is called before 'main()'.
 *	(same as 'SBOOT' for disk boot)
 */
EXPORT ER ROM_startup( void )
{
	W	val[L_DEVCONF_VAL];
	W	n;

	/* Set SYSCONF/DEVCONF */
	SCInfo.sysconf = ROMInfo->sysconf;
	SCInfo.devconf = ROMInfo->devconf;

	/* Initialize Boot mode/Operation mode */
	SCInfo.bm.w = 0;

	/* Set debug mode */
	n = GetDevConf(DCTAG_DEBUGMODE, val);
	SCInfo.bm.c.debug = ( (n >= 1) && (val[0] > 0) )? 1: 0;

	/* Set boot device (no boot device) */
	SCInfo.bootdev[0] = '\0';

	return E_OK;
}

IMPORT void waitForEvent(void);
IMPORT T_COUNTLOCK	SpinLockObj;			/* Spin lock for critical section */

/*
 * Set stack by exception mode
 */
LOCAL void setExcStack( UW mode, UW stack )
{
	Asm("	msr	cpsr_c, %1	\n"
	"	mov	sp, %0		\n"
	"	msr	cpsr_c, %2	"
	:: "l"(stack), "r"(mode|PSR_I|PSR_F), "i"(PSR_SVC|PSR_I|PSR_F));
}

/*
 * Initialization before T-Kernel starts
 */
#define SYSCONF_VAL_MAX (16)
UW abt_stack[MAX_PROC-1];
UW und_stack[MAX_PROC-1];
UW irq_stack[MAX_PROC-1];
UW fiq_stack[MAX_PROC-1];

EXPORT ER init_device( void )
{
IMPORT	void	CountWaitUsec( void );	/* cntwus.c */

/* Low level memory manager information */
IMPORT	UW	lowmem_top;

	UINT	i;
	UINT	prid = get_prid();
	W	n, v[SYSCONF_VAL_MAX];

	if ( prid == 0 ) {
		/* Initialization of kernel use spin lock */
		InitCountSpinLock(&SpinLockObj);

		/* Compute loop count of micro second wait */
		CountWaitUsec();

		/* Save system shared information */
		SaveSCInfo = SCInfo;

		/* Set boot device */
		if ( SCInfo.bootdev[0] != '\0' ) {
			strncpy((char*)bootdevnm, (char*)SCInfo.bootdev, L_DEVNM+1);
			BootDeviceName = bootdevnm;
		} else {
			BootDeviceName = NULL; /* No boot device */
		}

		/* Set stack for each exception mode */
		lowmem_top = (lowmem_top + 3) & ~0x00000003U; /* Alignment match */
		n = GetSysConf(SCTAG_ABTSTKSZ, v);	/* Abort (MMU) */
		n = ( n < 1 )? 64: v[0];
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_ABT, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			abt_stack[i] = lowmem_top;
		}

		n = GetSysConf(SCTAG_UNDSTKSZ, v);	/* Undefined order exception */
		n = ( n < 1 )? 64: v[0];
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_UND, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			und_stack[i] = lowmem_top;
		}

		n = GetSysConf(SCTAG_IRQSTKSZ, v);	/* Interrupt (IRQ) */
		n = ( n < 1 )? 512: v[0];
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_IRQ, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			irq_stack[i] = lowmem_top;
		}

		n = GetSysConf(SCTAG_FIQSTKSZ, v);	/* Fast interrupt (FIQ) */
		n = ( n < 1 )? 128: v[0];
		lowmem_top += (n + 3) & ~0x00000003U;
		setExcStack(PSR_FIQ, lowmem_top);

		for ( i = 0; i < MAX_PROC - 1; i++ ) {
			lowmem_top += (n + 3) & ~0x00000003U;
			fiq_stack[i] = lowmem_top;
		}

		/* Initialize smp-call driver */
		smpc_init();
		/* Processor is registered in smp-call driver */
		smpc_regist(prid);

	} else {
		/* wait for first processor complete initialize */
		waitForEvent();

		setExcStack(PSR_ABT, abt_stack[prid-1]);

		setExcStack(PSR_UND, und_stack[prid-1]);

		setExcStack(PSR_IRQ, irq_stack[prid-1]);

		setExcStack(PSR_FIQ, fiq_stack[prid-1]);

		/* Registration smp-call driver */
		smpc_regist(prid);
	}

	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 * Start processing after T-Kernel starts
 *	Called from the initial task contexts.
 */
EXPORT ER start_device( void )
{
	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 * System finalization
 *	Called just before system shutdown.
 *	Execute finalization that must be done before system shutdown.
 */
EXPORT ER finish_device( void )
{
	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 *	Re-starting processing
 */

/*
 * Re-starting processing
 *	mode = -1		Reset and re-start	(cold boot)
 *	mode = -2		Re-start		(warm boot)
 *	mode = -3		Reboot			(normal boot)
 *	mode = 0xFFhhmmss	Re-start at hh:mm:ss
 *				0 <= hh < 24, 0 <= mm,ss < 60
 */
EXPORT ER restart_device( W mode )
{
	if ( mode == -1 ) {
		/* Reset and re-start (cold boot) */
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"\n<< SYSTEM RESTART >>\n");
#endif
		tm_exit(-1);  /* no return */
		return E_OBJ;
	}

	if ( mode == -3 ) {
		/* Reboot (normal boot) */
		static UB bdcmd[4 + L_DEVNM] = "bd ";
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"\n<< SYSTEM REBOOT >>\n");
#endif
		strncat((char*)bdcmd, (char*)SCInfo.bootdev, L_DEVNM);
		tm_command(bdcmd);	/* Normally no return */
		return E_OBJ;		/* When the BD command is an error */
	}

	if ( mode == -2 ) {
		return E_NOSPT; /* Unsupported */
	}

	if ( ((UW)mode & 0xff000000U) == 0xff000000U ) {
		/* Re-start at specified time */
		return E_NOSPT;	/* Unsupported */
	}

	return E_PAR;
}
