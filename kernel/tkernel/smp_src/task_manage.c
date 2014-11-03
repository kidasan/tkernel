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
 *	task_manage.c (T-Kernel/OS)
 *	Task Management Function
 */

#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"

#include "cpu_task.h"
#include <tm/tmonitor.h>

/* For SMP T-Kernel */
#include "smpkernel.h"
#include "mp_common.h"
#include "mp_domain.h"

/*
 * Create task
 */
SYSCALL ID _tk_cre_tsk P1( T_CTSK *pk_ctsk )
{
#if CHK_RSATR
	const ATR VALID_TSKATR = {	/* Valid value of task attribute */
		 TA_HLNG
		|TA_SSTKSZ
		|TA_USERSTACK
		|TA_TASKSPACE
		|TA_RESID
		|TA_RNG3
		|TA_FPU
		|TA_COP0
		|TA_COP1
		|TA_COP2
		|TA_COP3
		|TA_GP
		|TA_ASSPRC
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	TCB	*tcb;
	INT	stksz, sstksz, sysmode, resid;
	VP	stack = NULL, sstack;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_ctsk->tskatr, VALID_TSKATR);
	CHECK_PRI(pk_ctsk->itskpri);
	CHECK_NOCOP(pk_ctsk->tskatr);
#if USE_SINGLE_STACK
	CHECK_NOSPT((pk_ctsk->tskatr & TA_USERSTACK) == 0);
#endif
#if CHK_PAR
	if ( (pk_ctsk->tskatr & TA_USERSTACK) != 0 ) {
		CHECK_PAR((pk_ctsk->tskatr & TA_RNG3) != TA_RNG0);
		CHECK_PAR(pk_ctsk->stksz == 0);
	} else {
		CHECK_PAR(pk_ctsk->stksz >= 0);
	}
	if ( (pk_ctsk->tskatr & TA_TASKSPACE) != 0 ) {
		CHECK_PAR((pk_ctsk->lsid >= 0) && (pk_ctsk->lsid <= MAX_LSID));
	}
	if ( (pk_ctsk->tskatr & TA_ASSPRC) != 0 ) {
		CHECK_PAR(pk_ctsk->assprc != 0);
		CHECK_PAR((pk_ctsk->assprc & ~ALL_CORE_BIT) == 0);
	}
#endif
	CHECK_PATR(pk_ctsk->tskatr);
#if CHK_ID
	if ( (pk_ctsk->tskatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_ctsk->domid);
	}
#endif

	if ( (pk_ctsk->tskatr & TA_RESID) != 0 ) {
		CHECK_RESID(pk_ctsk->resid);
		resid = pk_ctsk->resid;
	} else {
		resid = SYS_RESID; /* System resource group */
	}

	if ( (pk_ctsk->tskatr & TA_SSTKSZ) != 0 ) {
		CHECK_PAR(pk_ctsk->sstksz >= MIN_SYS_STACK_SIZE);
		sstksz = pk_ctsk->sstksz;
	} else {
		sstksz = default_sstksz;
	}
	if ( (pk_ctsk->tskatr & TA_RNG3) == TA_RNG0 ) {
		sysmode = 1;
		sstksz += pk_ctsk->stksz;
		stksz = 0;
	} else {
		sysmode = 0;
#if USE_SINGLE_STACK
		sstksz += pk_ctsk->stksz;
		stksz = 0;
#else
		stksz = pk_ctsk->stksz;
#endif
	}

	/* Adjust stack size by 8 bytes */
	sstksz = (sstksz + 7) / 8 * 8;
	stksz  = (stksz  + 7) / 8 * 8;

	/* Allocate system stack area */
	sstack = IAmalloc((UINT)sstksz, TA_RNG0);
	if ( sstack == NULL ) {
		return E_NOMEM;
	}

	if ( stksz > 0 ) {
		/* Allocate user stack area */
		stack = IAmalloc((UINT)stksz, pk_ctsk->tskatr);
		if ( stack == NULL ) {
			IAfree(sstack, TA_RNG0);
			return E_NOMEM;
		}
	}

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_ctsk->tskatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_ctsk->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_ctsk->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_ctsk->tskatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_TSK,
						   tcb_table->oname,
						   sizeof(TCB),
						   domid,
						   pk_ctsk->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_ctsk->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_ctsk->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	tcb = (TCB*)QueRemoveNext(&free_tcb);
	if ( tcb == NULL ) {
		ercd = E_LIMIT;
		goto error_exit;
	}

	tcb->tskid = (tcb->tskid & ~DOMID_MASK) | MP_GEN_DOMIDPART(domid);

#ifdef NUM_PORID
	/* Domain ID part is updated. */
	tcb->wrdvno = (tcb->wrdvno & ~DOMID_MASK) | MP_GEN_DOMIDPART(domid);
#endif

	/* Set object name */
	strncpy((char*)tcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
	/* Registration */
	mp_register_object_in_domain(domid,
				     MP_OBJECT_TYPE_TSK,
				     INDEX_TSK(tcb->tskid),
				     oname);

	/* Initialize control block */
	tcb->exinf     = pk_ctsk->exinf;
	tcb->tskatr    = pk_ctsk->tskatr;
	tcb->task      = pk_ctsk->task;
	tcb->ipriority = (UB)int_priority(pk_ctsk->itskpri);
	tcb->resid     = resid;
	tcb->stksz     = stksz;
	tcb->sstksz    = sstksz;
	if ( (pk_ctsk->tskatr & TA_ASSPRC) != 0 ) {
		tcb->assprc    = pk_ctsk->assprc;
	}
	else{
		tcb->assprc    = ALL_CORE_BIT;
	}
#if TA_GP
	/* Set global pointer */
	if ( (pk_ctsk->tskatr & TA_GP) != 0 ) {
		gp = pk_ctsk->gp;
	}
	tcb->gp = gp;
#endif

	/* Set stack pointer */
	if ( stksz > 0 ) {
		tcb->istack = (VB*)stack + stksz;
	} else {
		tcb->istack = pk_ctsk->stkptr;
	}
	tcb->isstack = (VB*)sstack + sstksz - RESERVE_SSTACK(tcb->tskatr);

	/* Set initial value of task operation mode */
	tcb->isysmode = (B)sysmode;
	tcb->sysmode  = (H)sysmode;

	/* Set initial value of task space */
	if ( (pk_ctsk->tskatr & TA_TASKSPACE) != 0 ) {
		tcb->tskctxb.uatb = pk_ctsk->uatb;
		tcb->tskctxb.lsid = pk_ctsk->lsid;
	} else {
		tcb->tskctxb.uatb = NULL;
		tcb->tskctxb.lsid = 0;		/* Task Space */
	}

	/* make it to DORMANT state */
	make_dormant(tcb);
	setup_context(tcb);

	ercd = tcb->tskid;

    error_exit:
	END_CRITICAL_NO_DISPATCH;

	if ( ercd < E_OK ) {
		IAfree(sstack, TA_RNG0);
		if ( stksz > 0 ) {
			IAfree(stack, pk_ctsk->tskatr);
		}
	}

	return ercd;
}

/*
 * Task deletion
 *	Call from critical section
 * (*)EXPORT for delayed del_tsk()
 */
EXPORT void _del_tsk( TCB *tcb )
{
	VP	stack;

	/* Free system stack */
	stack = (VB*)tcb->isstack + RESERVE_SSTACK(tcb->tskatr) - tcb->sstksz;
	IAfree(stack, TA_RNG0);

	if ( tcb->stksz > 0 ) {
		/* Free user stack */
		stack = (VB*)tcb->istack - tcb->stksz;
		IAfree(stack, tcb->tskatr);
	}

	/* Unregistration */
	mp_unregister_object_in_domain(tcb->tskid,
				       MP_OBJECT_TYPE_TSK,
				       INDEX_TSK(tcb->tskid));

	/* Return control block to FreeQue */
	QueInsert(&tcb->tskque, &free_tcb);
	tcb->state = TS_NONEXIST;
}

/*
 * Delete task 
 */
SYSCALL ER _tk_del_tsk( ID tskid )
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
	if ( state != TS_DORMANT ) {
		ercd = E_OBJ;
	} else if ( is_ctxtsk(tcb) ) {
		/* if target task is still running (case of tk_del_tsk after tk_ter_tsk) */
		tcb->stkfree = TRUE;
		tcb->state = TS_NONEXIST;
	} else {
		_del_tsk(tcb);
	}

    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Start task
 */
SYSCALL ER _tk_sta_tsk( ID tskid, INT stacd )
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
	if ( state != TS_DORMANT ) {
		ercd = E_OBJ;
	} else if ( is_ctxtsk(tcb) ) {
		/* if target task is still running(case of tk_sta_tsk called after tk_ter_tsk) */
		ercd = E_OBJ;
	} else {
		setup_context(tcb);
		setup_stacd(tcb, stacd);
		make_ready(tcb);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Task finalization
 *	Call from critical section
 */
LOCAL void _ter_tsk( TCB *tcb )
{
	TSTAT	state;

	if ( tcb->svclocked != NULL ) {
		/* Unlock all extended SVC locks */
		AllUnlockSVC(tcb);
	}

	state = (TSTAT)tcb->state;
	if ( state == TS_READY ) {
		make_non_ready(tcb);

	} else if ( (state & TS_WAIT) != 0 ) {
		wait_cancel(tcb);
		if ( tcb->wspec->rel_wai_hook != NULL ) {
			(*tcb->wspec->rel_wai_hook)(tcb);
		}
	}

#ifdef NUM_MTXID
	/* signal mutex */
	signal_all_mutex(tcb);
#endif

	cleanup_context(tcb);
}

/*
 * End its own task
 */
SYSCALL void _tk_ext_tsk( void )
{
#ifdef DORMANT_STACK_SIZE
	/* To avoid destroying stack used in 'make_dormant', 
	   allocate the dummy area on the stack. */
	volatile VB _dummy[DORMANT_STACK_SIZE];
#endif
	UINT	prid;

	/* Check context error */
#if CHK_CTX2
	if ( in_indp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_ext_tsk was called in the task independent\n");
#endif
		tm_monitor(); /* To monitor */
	}
#endif
#if CHK_CTX1
	if ( in_ddsp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_ext_tsk was called in the dispatch disabled\n");
#endif
	}
#endif

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	dispatch_disabled[prid] = DDS_ENABLE;

	_ter_tsk(ctxtsk[prid]);
	make_dormant(ctxtsk[prid]);

	END_CRITICAL_FORCE;
	/* No return */

#ifdef DORMANT_STACK_SIZE
	/* for WARNING */
	_dummy[0] = 0;
#endif
}

/*
 * End and delete its own task
 */
SYSCALL void _tk_exd_tsk( void )
{
	UINT	prid;

	/* Check context error */
#if CHK_CTX2
	if ( in_indp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_exd_tsk was called in the task independent\n");
#endif
		tm_monitor(); /* To monitor */
	}
#endif
#if CHK_CTX1
	if ( in_ddsp() ) {
#if USE_KERNEL_MESSAGE
		tm_putstring((UB*)"tk_exd_tsk was called in the dispatch disabled\n");
#endif
	}
#endif

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	dispatch_disabled[prid] = DDS_ENABLE;

	_ter_tsk(ctxtsk[prid]);
	/* _del_tsk() called from dispatcher */

	ctxtsk[prid]->stkfree = TRUE;
	ctxtsk[prid]->state = TS_NONEXIST;

	END_CRITICAL_FORCE;
	/* No return */
}

/*
 * Termination of other task
 */
SYSCALL ER _tk_ter_tsk( ID tskid )
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
	} else if ( tcb->klocked ) {
		/* Normally, it does not become this state.
		 * When the state is page-in wait in the virtual memory
		 * system and when trying to terminate any task,
		 * it becomes this state.
		 */
		ercd = E_OBJ;
	} else {
		/* if terminate task is runninng make dispatch enable */
		if ( is_ctxtsk(tcb) ) {
			dispatch_disabled[get_prid_from_ctx(tcb)] = DDS_ENABLE;
		}

		_ter_tsk(tcb);
		make_dormant(tcb);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Change task priority
 */
SYSCALL ER _tk_chg_pri( ID tskid, PRI tskpri )
{
	TCB	*tcb;
	INT	priority;
	ER	ercd;

	CHECK_TSKID_SELF(tskid);
	CHECK_PRI_INI(tskpri);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Conversion priority to internal expression */
	if ( tskpri == TPRI_INI ) {
		priority = tcb->ipriority;
	} else {
		priority = int_priority(tskpri);
	}

#ifdef NUM_MTXID
	/* Mutex priority change limit */
	ercd = chg_pri_mutex(tcb, priority);
	if ( ercd < E_OK ) {
		goto error_exit;
	}

	tcb->bpriority = (UB)priority;
	priority = ercd;
#else
	tcb->bpriority = priority;
#endif

	/* Change priority */
	change_task_priority(tcb, priority);

	ercd = E_OK;
    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Rotate ready queue
 */
SYSCALL ER _tk_rot_rdq( PRI tskpri )
{
	UINT	prid;

	CHECK_PRI_RUN(tskpri);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( tskpri == TPRI_RUN ) {
		if ( in_indp() ) {
			rotate_ready_queue_run();
		} else {
			rotate_ready_queue_ctx();
		}
	} else {
		rotate_ready_queue(int_priority(tskpri));
	}
	END_CRITICAL_SECTION;

	return E_OK;
}

/*
 * Change slice time
 */
SYSCALL ER _tk_chg_slt( ID tskid, RELTIM slicetime )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			tcb->slicetime = slicetime;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Refer task ID at execution
 */
SYSCALL ID _tk_get_tid( void )
{
	ID	tid;
	UINT	prid;

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	tid = ( ctxtsk[prid] == NULL )? 0: ctxtsk[prid]->tskid;
	END_CRITICAL_NO_DISPATCH;

	return tid;
}

/*
 * Refer task state
 */
SYSCALL ER _tk_ref_tsk( ID tskid, T_RTSK *pk_rtsk )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	memset(pk_rtsk, 0, sizeof(*pk_rtsk));

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd != E_OK ) {
			goto error_exit;
		}

		if ( state == TS_READY && is_ctxtsk(tcb) ) {
			pk_rtsk->tskstat = TTS_RUN;
		} else {
			pk_rtsk->tskstat = (UINT)state << 1;
		}
		if ( (state & TS_WAIT) != 0 ) {
			pk_rtsk->tskwait = tcb->wspec->tskwait;
			pk_rtsk->wid     = tcb->wid;
			if ( tcb->nodiswai ) {
				pk_rtsk->tskstat |= TTS_NODISWAI;
			}
		}
		pk_rtsk->exinf     = tcb->exinf;
		pk_rtsk->tskpri    = ext_tskpri(tcb->priority);
		pk_rtsk->tskbpri   = ext_tskpri(tcb->bpriority);
		pk_rtsk->wupcnt    = tcb->wupcnt;
		pk_rtsk->suscnt    = tcb->suscnt;
		pk_rtsk->slicetime = tcb->slicetime;
		pk_rtsk->waitmask  = tcb->waitmask;
		pk_rtsk->texmask   = tcb->texmask;
		pk_rtsk->tskevent  = tcb->tskevt;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get task statistic information
 */
SYSCALL ER _tk_inf_tsk( ID tskid, T_ITSK *pk_itsk, BOOL clr )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			pk_itsk->stime = tcb->stime;
			pk_itsk->utime = tcb->utime;
			if ( clr ) {
				tcb->stime = 0;
				tcb->utime = 0;
			}
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Get task resource group 
 */
SYSCALL ID _tk_get_rid( ID tskid )
{
	TCB	*tcb;
	ER	ercd;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			ercd = tcb->resid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Set task resource group
 */
SYSCALL ID _tk_set_rid( ID tskid, ID resid )
{
	TCB	*tcb;
	ER	ercd;

	CHECK_TSKID_SELF(tskid);
	CHECK_RESID(resid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			ercd = tcb->resid;
			tcb->resid = resid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Release wait
 */
SYSCALL ER _tk_rel_wai( ID tskid )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	state = (TSTAT)tcb->state;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	if ( (state & TS_WAIT) == 0 ) {
		ercd = E_OBJ;
	} else {
		wait_release_ng(tcb, E_RLWAI);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Disable task wait
 */
#define WAIT_PATTERN	( TTW_SLP  | TTW_DLY  | TTW_SEM | TTW_FLG  \
					      | TTW_MBX | TTW_MTX  \
			| TTW_SMBF | TTW_RMBF | TTW_CAL | TTW_ACP  \
			| TTW_RDV  | TTW_MPF  | TTW_MPL            \
			| TTW_EV1  | TTW_EV2  | TTW_EV3 | TTW_EV4  \
			| TTW_EV5  | TTW_EV6  | TTW_EV7 | TTW_EV8  \
			| TTX_SVC )

SYSCALL INT _tk_dis_wai( ID tskid, UINT waitmask )
{
	TCB	*tcb;
	UINT	tskwait = 0;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);
	CHECK_PAR(((waitmask & ~WAIT_PATTERN) == 0)&&((waitmask & WAIT_PATTERN) != 0));

	BEGIN_CRITICAL_SECTION;

	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Update wait disable mask */
	tcb->waitmask = waitmask;

	if ( (tcb->state & TS_WAIT) != 0 ) {
		tskwait = tcb->wspec->tskwait;
		if ( ((tskwait & waitmask) != 0) && !tcb->nodiswai ) {
			/* Free wait */
			wait_release_ng(tcb, E_DISWAI);
			tskwait = 0;
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? (INT)ercd: (INT)tskwait;
}

/*
 * Enable task wait
 */
SYSCALL ER _tk_ena_wai( ID tskid )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			tcb->waitmask = 0;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */
/*
 *	Domain support function
 */

/*
 * Find task ID
 */
SYSCALL ER _tk_fnd_tsk( ID domid, UB *oname )
{
	ER	ercd;
	ID	tskid = 0;
	TCB	*tcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_TSK, (UB*)tcb_table, tcb_table->oname,
			       (UB*)&tcb_table->tskid, sizeof(TCB), domid, oname);
	if ( 0 < ercd ) {
		tskid = (ID)ercd;
		tcb = get_tcb(tskid);
		ercd = mp_check_protection_fnd(tskid, tcb->tskatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: tskid;
}

/*
 * Get domain information on task
 */
SYSCALL ER _tk_dmi_tsk( ID tskid, T_DMI *pk_dmi )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(tcb->tskatr);
			pk_dmi->domid = mp_get_domid_from_id(tcb->tskid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, tcb->oname, OBJECT_NAME_LENGTH);
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */
/*
 *	Debug support function
 */
#if USE_DBGSPT

/*
 * Get object name from control block
 */
EXPORT ER task_getname(ID tskid, UB **name)
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( (tskid != TSK_SELF) && !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else if ( (tcb->tskatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = tcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER task_setname(ID tskid, UB *name)
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( (tskid != TSK_SELF) && !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else if ( (tcb->tskatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_TSK, tcb_table->oname, sizeof(TCB), tcb->tskid, name, INDEX_TSK(tcb->tskid));
	}

	return ercd;
}

/*
 * Refer task usage state
 */
SYSCALL INT _td_lst_tsk( ID list[], INT nent )
{
	TCB	*tcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = tcb_table + NUM_TSKID;
	for ( tcb = tcb_table; tcb < end; tcb++ ) {
		if ( tcb->state == TS_NONEXIST ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = tcb->tskid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer task state
 */
SYSCALL ER _td_ref_tsk( ID tskid, TD_RTSK *pk_rtsk )
{
	TCB	*tcb;
	TSTAT	state;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	memset(pk_rtsk, 0, sizeof(*pk_rtsk));

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	state = (TSTAT)tcb->state;
	if ( state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( (tskid != TSK_SELF) && !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else {
		if ( state == TS_READY && is_ctxtsk(tcb) ) {
			pk_rtsk->tskstat = TTS_RUN;
		} else {
			pk_rtsk->tskstat = (UINT)state << 1;
		}
		if ( (state & TS_WAIT) != 0 ) {
			pk_rtsk->tskwait = tcb->wspec->tskwait;
			pk_rtsk->wid     = tcb->wid;
			if ( tcb->nodiswai ) {
				pk_rtsk->tskstat |= TTS_NODISWAI;
			}
		}
		pk_rtsk->exinf     = tcb->exinf;
		pk_rtsk->tskpri    = ext_tskpri(tcb->priority);
		pk_rtsk->tskbpri   = ext_tskpri(tcb->bpriority);
		pk_rtsk->wupcnt    = tcb->wupcnt;
		pk_rtsk->suscnt    = tcb->suscnt;
		pk_rtsk->slicetime = tcb->slicetime;
		pk_rtsk->waitmask  = tcb->waitmask;
		pk_rtsk->texmask   = tcb->texmask;
		pk_rtsk->tskevent  = tcb->tskevt;

		pk_rtsk->task      = tcb->task;
		pk_rtsk->stksz     = tcb->stksz;
		pk_rtsk->sstksz    = tcb->sstksz - RESERVE_SSTACK(tcb->tskatr);
		pk_rtsk->istack    = tcb->istack;
		pk_rtsk->isstack   = tcb->isstack;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get task statistic information
 */
SYSCALL ER _td_inf_tsk( ID tskid, TD_ITSK *pk_itsk, BOOL clr )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( (tskid != TSK_SELF) && !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else {
		pk_itsk->stime = tcb->stime;
		pk_itsk->utime = tcb->utime;
		if ( clr ) {
			tcb->stime = 0;
			tcb->utime = 0;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Find task ID
 */
SYSCALL ER _td_fnd_tsk( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_TSK, (UB*)tcb_table, tcb_table->oname,
			       (UB*)&tcb_table->tskid, sizeof(TCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on task
 */
SYSCALL ER _td_dmi_tsk( ID tskid, TD_DMI *pk_dmi )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( (tskid != TSK_SELF) && !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(tcb->tskatr);
		pk_dmi->domid  = mp_get_domid_from_id(tcb->tskid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, tcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
