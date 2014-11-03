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
 *	cpu_insn.h (NAVIENGINE)
 *	NAVIENGINE Dependent Operation
 */

#ifndef _CPU_INSN_
#define _CPU_INSN_

#include <sys/sysinfo.h>
#include <sys/kernel_util.h>

/* ------------------------------------------------------------------------ */
/*
 *	Control register operation
 */

/*
 * Get CPSR
 */
Inline UINT getCPSR( void )
{
	UINT	cpsr;
	Asm("mrs %0, cpsr": "=r"(cpsr));
	return cpsr;
}

/*
 * TLB disable
 */
Inline void PurgeTLB( void )
{
	Asm("mcr p15, 0, %0, cr8, c7, 0":: "r"(0));
}

/* ------------------------------------------------------------------------ */
/*
 *	EIT-related
 */

/*
 * Monitor use vector number
 */
#define VECNO_DEFAULT	EIT_DEFAULT	/* Default handler */
#define VECNO_BREAK	EIT_UNDEF	/* Break point (Invalid instruction) */
#define VECNO_MONITOR	( 4 )		/* Monitor service call */

/*
 * For saving monitor exception handler
 */
typedef struct monhdr {
	FP	default_hdr;		/* Default handler */
	FP	break_hdr;		/* Break point (Invalid instruction) */
	FP	monitor_hdr;		/* Monitor service call */
} MONHDR;

/* For saving monitor exception handler */
IMPORT MONHDR	SaveMonHdr;

/*
 * Set interrupt handler
 */
Inline void define_inthdr( INT vecno, FP inthdr )
{
	SCArea->intvec[vecno] = inthdr;
}

/*
 * If it is the task-independent part, TRUE
 */
Inline BOOL isTaskIndependent( void )
{
	return ( SCInfo.taskindp[get_prid()] > 0 )? TRUE: FALSE;
}
/*
 * Move to/Restore task independent part
 */
Inline void EnterTaskIndependent( void )
{
	SCInfo.taskindp[get_prid()]++;
}
Inline void LeaveTaskIndependent( void )
{
	SCInfo.taskindp[get_prid()]--;
}

/* ------------------------------------------------------------------------ */

#endif /* _CPU_INSN_ */
