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
 *	smpcall_drv.c (NAVIENGINE)
 *
 *	Inter-processor communication driver source file
 */

#include "sysinit.h"
#include "kernel.h"
#include <sys/sysinfo.h>
#include <tk/sysdef.h>
#include <tk/smp_util.h>
#include "smpcall_drv.h"

/*
 * Local Data
 */
LOCAL SMPC_DRVCB smpc_dcb;

/*
 * Set IPI
 */
Inline void set_ipi( SMPC_DRVCB *dcb, UW to )
{
	/* Set DIC_SWI */
	out_w(DIC_SWI(0), (to << IPI_SWI_CPUBIT) | dcb->ipi_irq);
}

/*
 * Clear IPI
 */
Inline void clr_ipi( SMPC_DRVCB *dcb, ID from )
{
	/* IPI is already cleared */
}

/*
 * Initialization of processor information
 */
LOCAL void init_pinfo( SMPC_PINFO *pi, ID proc_id )
{
	pi->proc_mask = (~(1 << proc_id)) & ((1 << MAX_PROC_NUM) - 1);
	pi->status = 0;
	pi->func = NULL;
	pi->param = NULL;
}

/*
 * The execution request is issued to other processor
 */
Inline UW notify( SMPC_DRVCB *dcb, SMPC_PINFO *pi, FP func, VP param )
{
	UW	bitmap;

	bitmap = dcb->available_proc & pi->proc_mask;
	if ( bitmap != 0 ) {
		pi->status = bitmap | (bitmap << EXE_SHIFT);
		pi->func = func;
		pi->param = param;
		mp_memory_barrier();
		set_ipi(dcb, bitmap);
	}

	return bitmap;
}

/*
 * Clear acceptance bit
 */
Inline void clr_acp_bit( SMPC_PINFO *pi, ID proc_id )
{
	(void)atomic_bitclr(&pi->status, ~ACP_BIT(proc_id));
}

/*
 * Clear execute bit
 */
Inline void clr_exe_bit( SMPC_PINFO *pi, ID proc_id )
{
	(void)atomic_bitclr(&pi->status, ~EXE_BIT(proc_id));
}

/*
 * Wait for acceptance
 */
Inline void wait_for_acceptance( SMPC_PINFO *pi )
{
	while ( (*(_UW*)&pi->status & ACP_MASK) != 0 );
}

/*
 * Wait for execution
 */
Inline void wait_for_completion( SMPC_PINFO *pi )
{
	while ( (*(_UW*)&pi->status & EXE_MASK) != 0 );
}

/*
 * Do request
 * 	Called when accept request from other processor
 */
LOCAL void do_request( ID from )
{
	ID		myprid;
	SMPC_DRVCB	*dcb;
	SMPC_PINFO	*pi;

	/* Information acquisition */
	myprid = get_prid();
	dcb = get_smpc_dcb();
	pi = get_smpc_pinfo(dcb, from);

	/* Clear IPI */
	clr_ipi(dcb, from);

	/* Acceptance completion notification */
	clr_acp_bit(pi, myprid);

	/* Execution */
	(*pi->func)(pi->param);

	/* Execution completion notification */
	clr_exe_bit(pi, myprid);
}

/*
 * Interrupt handler
 * (*) called when inter processor interrupt occuerred
 */
EXPORT void smpc_handler( UW proc_id )
{
	do_request(proc_id);
}

/*
 * Lock smp-call
 */
EXPORT void smpc_lock( void )
{
	SMPC_DRVCB	*dcb;
	UINT		intsts;

	/* Information acquisition */
	dcb = get_smpc_dcb();

	while ( ISpinTryLock(&dcb->lock, &intsts) != E_OK );
	dcb->intsts = intsts;
}

/*
 * Unlock smp-call
 */
EXPORT void smpc_unlock( void )
{
	SMPC_DRVCB	*dcb;
	UINT		intsts;

	/* Information acquisition */
	dcb = get_smpc_dcb();

	intsts = dcb->intsts;
	ISpinUnlock(&dcb->lock, intsts);
}

/*
 * Smp-call request
 */
EXPORT void smpc_req( FP func, VP param, BOOL sync, BOOL self )
{
	ID		myprid;
	SMPC_DRVCB	*dcb;
	SMPC_PINFO	*pi;
	UW		res;

	/* Information acquisition */
	myprid = get_prid();
	dcb = get_smpc_dcb();
	pi = get_smpc_pinfo(dcb, myprid);

	/* Wait until the last execution is completed */
	wait_for_completion(pi);

	/* Notify the other processor */
	res = notify(dcb, pi, func, param);
	if ( self ) {
		/* Own processor also */
		(*func)(param);
	}
	if ( res != 0 ) {
		/* Wait */
		if ( sync ) {
			wait_for_completion(pi);
		} else {
			wait_for_acceptance(pi);
		}
	}
}

/*
 * Completion waiting of smp-call
 */
EXPORT void smpc_wai( void )
{
	ID		myprid;
	SMPC_DRVCB	*dcb;
	SMPC_PINFO	*pi;

	/* Information acquisition */
	myprid = get_prid();
	dcb = get_smpc_dcb();
	pi = get_smpc_pinfo(dcb, myprid);

	/* Wait */
	wait_for_completion(pi);
}

/*
 * Acquire the bitmap of a registered processor
 */
EXPORT UW smpc_get_procbit( void )
{
	ID		myprid;
	SMPC_DRVCB	*dcb;
	SMPC_PINFO	*pi;

	/* Information acquisition */
	myprid = get_prid();
	dcb = get_smpc_dcb();
	pi = get_smpc_pinfo(dcb, myprid);

	return dcb->available_proc & pi->proc_mask;
}

/*
 * Registration of processor
 */
EXPORT ER smpc_regist( ID proc_id )
{
	SMPC_DRVCB	*dcb;

	/* Acquisition of driver control block */
	dcb = get_smpc_dcb();
	if ( !is_dcb_valid(dcb) ) {
		/* Initialization is not completed */
		return E_SYS;
	}

	/* Enable interrupt */
	EnableInt(EIT_MPSWI(dcb->ipi_irq), dcb->ipi_level);

	/* The processor is registered in DCB */
	atomic_bitset(&dcb->available_proc, (1 << proc_id));

	return E_OK;
}

/*
 * Initialize smp-call driver
 */
#define SYSCONF_VAL_MAX (16)

EXPORT ER smpc_init( void )
{
IMPORT	void	smpc_handler_startup( UW );
	SMPC_DRVCB	*dcb;
	SMPC_PINFO	*pi;
	W		i, n, v[SYSCONF_VAL_MAX];

	/* Acquisition of driver control block */
	dcb = get_smpc_dcb();
	if ( is_dcb_valid(dcb) ) {
		/* The initialization is already completed */
		return E_SYS;
	}

	/* Get system information */
	/* The IRQ number acquisition for smp-call */
	n = GetSysConf(SCTAG_SMPCALL_IPI_IRQ, v);
	if ( n != 1 ) {
		return E_SYS;
	}
	dcb->ipi_irq = (UW)v[0];
	/* The interrupt level acquisition for smp-call */
	n = GetSysConf(SCTAG_SMPCALL_IPI_LEVEL, v);
	if ( n != 1 ) {
		return E_SYS;
	}
	dcb->ipi_level = (UW)v[0];

	/* Initialize a lock variable */
	InitSpinLock(&dcb->lock);

	/* Initialize the information of the available processor */
	dcb->available_proc = 0;

	/* Initialization of processor information */
	for ( i = 0; i < MAX_PROC_NUM; i++ ) {
		pi = get_smpc_pinfo(dcb, i);
		init_pinfo(pi, i);
	}

	/* Registration of smp-call handler */
	define_inthdr(EIT_MPSWI(dcb->ipi_irq), smpc_handler_startup);
	/* Set interrupt mode */
	SetIntMode(EIT_MPSWI(dcb->ipi_irq), IM_EDGE | IM_NN);

	/* Initialization completion */
	atomic_xchg(&dcb->valid, DCB_VALID);

	return E_OK;
}
