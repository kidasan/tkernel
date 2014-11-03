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
 *	cpu_support_c.c (NAVIENGINE)
 *	Device-Dependent CPU Operation (C language part)
 */

#include "kernel.h"
#include "task.h"
#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <sys/sysinfo.h>
#include <sys/kernel_util.h>
#include "cpu_insn.h"

#define RET_NO_DISPATCH	(0)
#define RET_DISPATCH	(1)
#define RET_DCT		(2)

/*
 * Check dispatch
 * Called from tk_ret_int, During DI
 */
EXPORT INT chk_dsp( UINT _cpsr_ )
{
	INT	prid = get_prid();

	if ( !isDI(_cpsr_) && !isTaskIndependent() ) {
		/* Lock spin */
		CountSpinLock(&SpinLockObj);
		if ( !dispatch_disabled[prid] ) {
			/* Am I dispatch ? */
			if ( ctxtsk[prid] != schedtsk[prid] ) {
				return RET_DISPATCH;
			}
			/* I'm not dispatch, but there is DCT */
			if ( (ctxtsk[prid] != NULL) && (ctxtsk[prid]->reqdct == 1) && (SCInfo.taskmode[prid] & TMF_CPL(3)) ) {
				/* Unlock spin */
				CountSpinUnlock(&SpinLockObj);
				return RET_DCT;
			}
		}
		/* Unlock spin */
		CountSpinUnlock(&SpinLockObj);
	}
	return RET_NO_DISPATCH;
}
/*
 * Into dispatch
 * Called from dispatcher, During DI && SpinLocked
 */
IMPORT void _del_tsk( TCB *tcb );
EXPORT void into_dsp( INT prid )
{
	/* Enter task independent part */
	SCInfo.taskindp[prid]++;
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
 * Check context restore
 * Called from dispatcher, During DI
 */
EXPORT TCB* chk_restore( INT prid )
{
	TCB	*tcb = NULL;

	if ( CountSpinTryLock(&SpinLockObj) == E_OK ) {
		tcb = schedtsk[prid];
		if ( tcb != NULL ) {
			if ( is_ctxtsk(tcb) ) {
				CountSpinUnlock(&SpinLockObj);
				return NULL;
			}
			ctxtsk[prid] = tcb;
			/* Leave task independent part */
			SCInfo.taskindp[prid]--;
		}
		/* Unlock spin */
		CountSpinUnlock(&SpinLockObj);
	}
	return tcb;
}

