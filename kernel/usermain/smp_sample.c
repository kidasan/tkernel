/*
 *----------------------------------------------------------------------------
 *    SMP Sample
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
 *	smp_sample.c
 *
 *	SMP sample program
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <tm/tmonitor.h>

/*
 * Various definitions
 */
#define INIT_TASK_PRI	(1)
#define DELAY_TIME	(1000)
#define TKERPRNUM	((UB*)"TKerPrNum")

/* Task creation information*/
#define TSK_EXINF	((VP)0)
#define TSK_TSKATR	(TA_HLNG | TA_RNG1 | TA_PUBLIC)
#define TSK_ITSKPRI_D	(100)
#define TSK_ITSKPRI_S	(101)
#define TSK_STKSZ	(4096)
#define TSK_STACD	(0)

/* Message */
LOCAL UB message[] = "SMP T-Kernel\n";

/*
 * Prototype definitions
 */
LOCAL void out_msg( UB *msg );
LOCAL void dummy_task( INT stacd, VP exinf );
LOCAL void sample_task( INT stacd, VP exinf );
LOCAL void cre_tsk( FP task, PRI pri );
EXPORT void smp_sample( void );

/*
 * Output message
 */
LOCAL void out_msg( UB *msg )
{
	tm_putstring(msg);
}

/*
 * Dummy task
 */
LOCAL void dummy_task( INT stacd, VP exinf )
{
	for ( ;; ) {
		/* nothing to do */
	}
}

/*
 * Sample task
 */
LOCAL void sample_task( INT stacd, VP exinf )
{
	for ( ;; ) {
		/* Wait */
		tk_dly_tsk(DELAY_TIME);
		/* Output message */
		out_msg(message);
	}
}

/*
 * Create & Start Task
 */
LOCAL void cre_tsk( FP task, PRI pri )
{
	T_CTSK	ctsk;
	ID	tskid;

	ctsk.exinf   = TSK_EXINF;
	ctsk.tskatr  = TSK_TSKATR;
	ctsk.task    = task;
	ctsk.itskpri = pri;
	ctsk.stksz   = TSK_STKSZ;
	tskid = tk_cre_tsk(&ctsk);
	tk_sta_tsk(tskid, 0);
}

/*
 * Main
 */
EXPORT void smp_sample( void )
{
	INT	i, prnum;

	tk_chg_pri(TSK_SELF, INIT_TASK_PRI);
	/* Create task */
	tk_get_cfn(TKERPRNUM, &prnum, 1);
	for ( i = 0; i < (prnum - 1); i++ ) {
		cre_tsk(dummy_task, TSK_ITSKPRI_D);
	}
	cre_tsk(sample_task, TSK_ITSKPRI_S);
}
