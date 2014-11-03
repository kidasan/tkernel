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
 *	task_sync.c (T-Kernel/OS)
 *	Task with Synchronize Function
 */

#include <limits.h>
#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"

/* For SMP T-Kernel */
#include "smpkernel.h"
#include "mp_common.h"
#include "mp_domain.h"

/*
 * Suspend task
 */
SYSCALL ER _tk_sus_tsk( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
	if ( state == TS_DORMANT ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	if ( is_ctxtsk(tcb) && dispatch_disabled[get_prid_from_ctx(tcb)] >= DDS_DISABLE ) {
		ercd = E_CTX;
		goto error_exit;
	}
	if ( tcb->suscnt == INT_MAX ) {
		ercd = E_QOVR;
		goto error_exit;
	}

	/* Update suspend request count */
	++tcb->suscnt;

	/* Move to forced wait state */
	if ( state == TS_READY ) {
		make_non_ready(tcb);
		tcb->state = TS_SUSPEND;

	} else if ( state == TS_WAIT ) {
		tcb->state = TS_WAITSUS;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Resume task
 */
SYSCALL ER _tk_rsm_tsk( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	switch ( tcb->state ) {
	  case TS_DORMANT:
	  case TS_READY:
	  case TS_WAIT:
		ercd = E_OBJ;
		break;

	  case TS_SUSPEND:
		if ( --tcb->suscnt == 0 ) {
			make_ready(tcb);
		}
		break;
	  case TS_WAITSUS:
		if ( --tcb->suscnt == 0 ) {
			tcb->state = TS_WAIT;
		}
		break;

	  default:
		ercd = E_SYS;
		break;
	}
    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Force resume task
 */
SYSCALL ER _tk_frsm_tsk( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	switch ( tcb->state ) {
	  case TS_DORMANT:
	  case TS_READY:
	  case TS_WAIT:
		ercd = E_OBJ;
		break;

	  case TS_SUSPEND:
		tcb->suscnt = 0;
		make_ready(tcb);
		break;
	  case TS_WAITSUS:
		tcb->suscnt = 0;
		tcb->state = TS_WAIT;
		break;

	  default:
		ercd = E_SYS;
		break;
	}
    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Definition of task wait specification
 */
LOCAL WSPEC wspec_slp = { TTW_SLP, NULL, NULL };

/*
 * Move its own task state to wait state
 */
SYSCALL ER _tk_slp_tsk( TMO tmout )
{
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	/* Check wait disable */
	if ( (ctxtsk[prid]->waitmask & TTW_SLP) != 0 ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	if ( ctxtsk[prid]->wupcnt > 0 ) {
		ctxtsk[prid]->wupcnt--;
	} else {
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) {
			ctxtsk[prid]->wspec = &wspec_slp;
			ctxtsk[prid]->wid = 0;
			ctxtsk[prid]->wercd = &ercd;
			make_wait(tmout, TA_NULL);
			QueInit(&ctxtsk[prid]->tskque);
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Wakeup task
 */
SYSCALL ER _tk_wup_tsk( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;

	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
	if ( state == TS_DORMANT ) {
		ercd = E_OBJ;
	} else if ( ((state & TS_WAIT) != 0) && (tcb->wspec == &wspec_slp) ) {
		wait_release_ok(tcb);

	} else if ( tcb->wupcnt == INT_MAX ) {
		ercd = E_QOVR;
	} else {
		++tcb->wupcnt;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Cancel wakeup request
 */
SYSCALL INT _tk_can_wup( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;

	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
	if ( tcb->state == TS_DORMANT ) {
		ercd = E_OBJ;
	} else {
		ercd = tcb->wupcnt;
		tcb->wupcnt = 0;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/* ------------------------------------------------------------------------ */

#define toTTW(evtmask)		( (UINT)(evtmask) << 16 )
#define toEVT(tskwait)		( (UB)((tskwait) >> 16) )

/*
 * Wait for task event
 */
SYSCALL INT _tk_wai_tev( INT waiptn, TMO tmout )
{
	WSPEC	wspec;
	ER	ercd;
	UINT	prid;

	CHECK_TMOUT(tmout);
	CHECK_PAR((((UINT)waiptn & ~0x000000ffU) == 0)&&(((UINT)waiptn & 0x000000FFU) != 0));
	CHECK_DISPATCH();

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	/* Check wait disable */
	if ( (ctxtsk[prid]->waitmask & toTTW(waiptn)) != 0 ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	if ( (ctxtsk[prid]->tskevt & waiptn) != 0 ) {
		ercd = ctxtsk[prid]->tskevt;
		ctxtsk[prid]->tskevt &= ~waiptn;
	} else {
		ercd = E_TMOUT;
		if ( tmout != TMO_POL ) {
			wspec.tskwait = toTTW(waiptn);
			wspec.chg_pri_hook = NULL;
			wspec.rel_wai_hook = NULL;
			ctxtsk[prid]->wspec = &wspec;
			ctxtsk[prid]->wid = 0;
			ctxtsk[prid]->wercd = &ercd;
			make_wait(tmout, TA_NULL);
			QueInit(&ctxtsk[prid]->tskque);
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Send task event
 */
SYSCALL ER _tk_sig_tev( ID tskid, INT tskevt )
{
	UINT	evtmsk;
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);
	CHECK_PAR((tskevt >= 1) && (tskevt <= 8));

	evtmsk = (UINT)(1 << (tskevt - 1));
	BEGIN_CRITICAL_SECTION;

	tcb = get_tcb_self(tskid);
	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	if ( tskid != TSK_SELF ) {
		ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
	}
	if ( state == TS_DORMANT ) {
		ercd = E_OBJ;
		goto error_exit;
	}

	if ( ((state & TS_WAIT) != 0) && ((tcb->wspec->tskwait & toTTW(evtmsk)) != 0) ) {
		wait_release_ok_ercd(tcb, (ER)(tcb->tskevt | evtmsk));
	} else {
		tcb->tskevt |= evtmsk;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}
