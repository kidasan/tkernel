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
 *	tkstart.c (T-Kernel/OS)
 *	T-Kernel Startup / Finalization
 */

#include "kernel.h"
#include "smpkernel.h"
#include "timer.h"
#include "task.h"
#include <sys/kernel_util.h>
#include <tm/tmonitor.h>

/*
 * Start initial task
 */
LOCAL void init_task_startup( T_CTSK *ctsk )
{
	ER	ercd;

#if TA_GP
	extern int _gp;
	ercd = _tk_cre_tsk(ctsk, 0,0,0,0,&_gp);
#else
	ercd = _tk_cre_tsk(ctsk);
#endif
	if ( ercd < E_OK ) {
		goto err_exit;
	}

	ercd = _tk_sta_tsk((ID)ercd, 0);
	if ( ercd < E_OK ) {
		goto err_exit;
	}

	return;

err_exit:
#if USE_KERNEL_MESSAGE
	tm_putstring((UB*)"init_task can not started\n");
#endif
	tm_monitor();
}

/*
 * Call module initialization
 */
LOCAL void init_module( ER (*initfunc)( void ), UB *name )
{
	ER	ercd;

	ercd = (*initfunc)();
	if ( ercd < E_OK ) {
#if USE_KERNEL_MESSAGE
		tm_putstring(name);
		tm_putstring((UB*)" : module initialize error\n");
#endif
		tm_monitor();
	}
}

IMPORT void setEvent(void);
IMPORT T_COUNTLOCK	SpinLockObj;			/* Spin lock for critical section */

#define InitModule(name)	init_module( name##_initialize, #name )

/*
 * Initialize kernel and create/start initial task
 */
EXPORT void t_kernel_main( T_CTSK *inittask )
{
IMPORT	void DispProgress( W n );	/* sysinit */

	DISABLE_INTERRUPT;

	/*
	 * Target-dependent initialization
	 */
	DispProgress(0x20);
	InitModule(cpu);
	if ( get_prid() == 0 ) {
		InitModule(tkdev);

		/*
		 * Each module initialization
		 */
		DispProgress(0x21);
		InitModule(task);
#ifdef NUM_SEMID
		InitModule(semaphore);
#endif
#ifdef NUM_FLGID
		InitModule(eventflag);
#endif
#ifdef NUM_MBXID
		InitModule(mailbox);
#endif
#ifdef NUM_MBFID
		InitModule(messagebuffer);
#endif
#ifdef NUM_PORID
		InitModule(rendezvous);
#endif
#ifdef NUM_MTXID
		InitModule(mutex);
#endif
#ifdef NUM_MPLID
		InitModule(memorypool);
#endif
#ifdef NUM_MPFID
		InitModule(fix_memorypool);
#endif
#ifdef NUM_CYCID
		InitModule(cyclichandler);
#endif
#ifdef NUM_ALMID
		InitModule(alarmhandler);
#endif
		InitModule(subsystem);
		InitModule(resource_group);
		InitModule(timer);

		/* For SMP T-Kernel */
		InitModule(mp_domain);

		/*
		 * Create/start initial task
		 */
		DispProgress(0x22);
		init_task_startup(inittask);
	}

	/*
	 * Dispatch to initial task and start kernel operation. 
	 */

	/* mp interrupt setup before start task */
	mp_int_init();

	/* if i'm first processor, set event to other processor
	   other processor waiting at set stack in init_device() */
	if ( get_prid() == 0 ) {
		setEvent();
	}

	/* It enters the dispatcher after the lock is acquired to synchronize dispatch. */
	DispProgress(0x23);
	CountSpinLock(&SpinLockObj);
	force_dispatch();
	/* No return */
}

/*
 * Finalization
 */
EXPORT void t_kernel_shutdown( void )
{
	timer_shutdown();
	cpu_shutdown();
}

/*
 * Execute finalization and stop
 */
EXPORT void t_kernel_exit( void )
{
	t_kernel_shutdown();
	tkdev_exit();
	/* No return */
}
