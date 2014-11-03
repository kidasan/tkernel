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
 *	cpu_support_c.c (SH7776)
 *	Device-Dependent CPU Operation (C language part)
 */

#include "kernel.h"
#include "task.h"
#include "check.h"
#include "cpu_task.h"

#include <sys/sysinfo.h>
#include <tk/sysdef.h>
#include <tk/smp_util.h>

#define RET_NO_DISPATCH	(0)
#define RET_DISPATCH	(1)
volatile INT restore_context_disable = RDS_DISABLE;

/*
 * Check dispatch - chk_dsp()
 * The chk_dsp() function is built into _tk_ret_int() routine.
 */

/*
 * Into dispatch
 * Called from dispatcher, Durring DI && SpinLocked
 */
IMPORT void _del_tsk( TCB *tcb );
EXPORT void into_dsp( INT prid )
{
	/* Enter task independt part */
	EnterTaskIndependent();
	/* If delayed _del_tsk flag, call _del_tsk() */
	if ( ctxtsk[prid] && (ctxtsk[prid]->stkfree) ) {
		_del_tsk(ctxtsk[prid]);
		ctxtsk[prid]->stkfree = FALSE;
	}
	/* Save context complete */
	ctxtsk[prid] = NULL;
	/* Unlock spin */
	CountSpinUnlock(&SpinLockObj);
}
/*
 * Check contest restore
 * Called form dispatcher, Durring DI
 */
EXPORT INT chk_restore( INT prid )
{
	INT i;
RETRY:
	if ( CountSpinTryLock(&SpinLockObj) == E_OK ) {
		/* Am I dispatch */
		if ( schedtsk[prid] ) {

			/* check all core ready to load context */
			for ( i = 0; i < num_proc; i++ ) {
				if ( (ctxtsk[i] != schedtsk[i]) && (ctxtsk[i]) ) {
					CountSpinUnlock(&SpinLockObj);
					return RET_NO_DISPATCH;
				}
			}
			restore_context_disable = RDS_ENABLE;
			return RET_DISPATCH;
		}
		CountSpinUnlock(&SpinLockObj);
		return RET_NO_DISPATCH;
	} else {
		/* already enable restore context */
		if ( (restore_context_disable == RDS_ENABLE) && (schedtsk[prid]) ){
			return RET_DISPATCH;
		} else {
			goto RETRY;
		}
	}
}
/*
 * Exit dispatch
 * Called from dispacher, Durring DI
 */
EXPORT void exit_dsp( INT prid )
{
	INT i;

	/* Unlock spin */
	if ( SpinLockObj.SpinLockCnt[prid] ) {
		/* check all core complete load context */
RETRY:
		for ( i = 0; i < num_proc; i++ ) {
			if ( (ctxtsk[i] != schedtsk[i]) && (ctxtsk[i]) ) {
				goto RETRY;
			}
		}
		restore_context_disable = RDS_DISABLE;
		CountSpinUnlock(&SpinLockObj);
	} else {
		while (1) {
			if (restore_context_disable == RDS_DISABLE) {
				break;
			}
		}
	}
}

