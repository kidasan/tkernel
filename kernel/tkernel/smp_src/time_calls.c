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
 *	time_calls.c (T-Kernel/OS)
 *	Time Management Function
 */

#include "kernel.h"
#include "timer.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include "tkdev_timer.h"
#include <sys/rominfo.h>

/* For SMP T-Kernel */
#include "smpkernel.h"
#include "mp_common.h"
#include "mp_domain.h"

/*
 * Set system clock
 */
SYSCALL ER _tk_set_tim( SYSTIM *pk_tim )
{
	CHECK_PAR(pk_tim->hi >= 0);

	BEGIN_CRITICAL_SECTION;
	real_time_ofs = ll_sub(toLSYSTIM(pk_tim), current_time);
	END_CRITICAL_NO_DISPATCH;

	return E_OK;
}

/*
 * Refer system clock
 */
SYSCALL ER _tk_get_tim( SYSTIM *pk_tim )
{
	BEGIN_CRITICAL_SECTION;
	*pk_tim = toSYSTIM(real_time());
	END_CRITICAL_NO_DISPATCH;

	return E_OK;
}

/*
 * Refer system operating time
 */
SYSCALL ER _tk_get_otm( SYSTIM *pk_tim )
{
	BEGIN_CRITICAL_SECTION;
	*pk_tim = toSYSTIM(current_time);
	END_CRITICAL_NO_DISPATCH;

	return E_OK;
}

#if USE_DBGSPT
/*
 * Refer system clock
 */
SYSCALL ER _td_get_tim( SYSTIM *tim, UINT *ofs )
{
	BEGIN_CRITICAL_SECTION;
	*ofs = get_hw_timer_nsec();
	*tim = toSYSTIM(real_time());
	END_CRITICAL_NO_DISPATCH;

	return E_OK;
}

/*
 * Refer system operating time
 */
SYSCALL ER _td_get_otm( SYSTIM *tim, UINT *ofs )
{
	BEGIN_CRITICAL_SECTION;
	*ofs = get_hw_timer_nsec();
	*tim = toSYSTIM(current_time);
	END_CRITICAL_NO_DISPATCH;

	return E_OK;
}
#endif /* USE_DBGSPT */

/* ------------------------------------------------------------------------ */

/*
 * Definition of task delay wait specification
 */
LOCAL WSPEC wspec_dly = { TTW_DLY, NULL, NULL };

/*
 * Task delay
 */
SYSCALL ER _tk_dly_tsk( RELTIM dlytim )
{
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_DISPATCH();

	if ( dlytim > 0 ) {
		BEGIN_CRITICAL_SECTION;
		prid = get_prid();
		/* Check wait disable */
		if ( (ctxtsk[prid]->waitmask & TTW_DLY) != 0 ) {
			ercd = E_DISWAI;
		} else {
			ctxtsk[prid]->wspec = &wspec_dly;
			ctxtsk[prid]->wid = 0;
			ctxtsk[prid]->wercd = &ercd;
			make_wait_reltim(dlytim, TA_NULL);
			QueInit(&ctxtsk[prid]->tskque);
		}
		END_CRITICAL_SECTION;
	}

	return ercd;
}

/* ------------------------------------------------------------------------ */
/*
 *	Handler support
 */

/*
 * Convert assprc to prid
 */
LOCAL const UW bit_tbl[] = {
	0, /* 0000 */	0, /* 0001 */	1, /* 0010 */	0, /* 0011 */
	2, /* 0100 */	0, /* 0101 */	1, /* 0110 */	0, /* 0111 */
	3, /* 1000 */	0, /* 1001 */	1, /* 1010 */	0, /* 1011 */
	2, /* 1100 */	0, /* 1101 */	1, /* 1110 */	0, /* 1111 */
};

Inline ID assprc_to_prid( UW assprc )
{
	ID	prid;
	INT	i;
	UW	bit;

	if ( assprc != 0 ){
		prid = 0;
		for ( i = 0; i < 8; i++ ){
			bit = assprc & 0xf;
			if ( bit != 0 ) {
				return prid + bit_tbl[bit];
			}
			assprc >>= 4;
			prid += 4;
		}
	}

	return 0;
}

/*
 * Check assign processor
 */
#if defined(NUM_CYCID)||defined(NUM_ALMID)
#if CHK_PAR
LOCAL BOOL chk_assprc( UW assprc )
{
	if ( (assprc == 0)
	  || ((assprc & ~ALL_CORE_BIT) != 0)
	  || ((assprc & (assprc-1)) != 0) ) {
		return FALSE;
	}
	return TRUE;
}
#endif
#endif

/*
 * Handler queue
 */
typedef struct {
	BOOL	upd:1;
	BOOL	act:1;
	UW	rsv:30;
	QUEUE	hdrque;
} HDRQUE;

EXPORT HDRQUE	handler_queue[MAX_PROC];

/*
 * Initialization of handler
 */
EXPORT void handler_initialize( void )
{
	INT	i;
	HDRQUE	*hq;

	for ( i = 0; i < MAX_PROC; i++ ) {
		hq = &handler_queue[i];
		hq->upd = FALSE;
		hq->act = FALSE;
		QueInit(&hq->hdrque);
	}
}

/*
 * Insert handler to handler queue
 */
Inline void enqueue_handler( UINT prid, TMEB *event, CBACK cback, VP arg )
{
	HDRQUE	*hq;

	hq = &handler_queue[prid];
	event->callback = cback;
	event->arg = arg;
	QueInsert(&event->queue, &hq->hdrque);
	hq->upd = TRUE;
}

/*
 * Check update of the handler queue
 * Note : This function is called in a critical section
 */
EXPORT UW check_handler_queue( void )
{
	UW	assprc = 0;
	INT	i;
	HDRQUE	*hq;

	for ( i = 0; i < MAX_PROC; i++ ) {
		hq = &handler_queue[i];
		if ( hq->upd && !hq->act ) {
			assprc |= 1 << i;
			hq->act = TRUE;
		}
		hq->upd = FALSE;
	}

	return assprc;
}

/*
 * Handler execution
 * Note : This function is called in a critical section
 */
EXPORT void handler_execution( void )
{
	UINT	prid;
	HDRQUE	*hq;
	TMEB	*event;

	prid = get_prid();
	hq = &handler_queue[prid];
	while ( !isQueEmpty(&hq->hdrque) ) {
		event = (TMEB*)hq->hdrque.next;
		QueRemove(&event->queue);
		(*event->callback)(event->arg);
	}
	hq->act = FALSE;
}

/*
 * Handler update processing
 * Note : This function is called in a critical section
 */
EXPORT void handler_update( UW assprc )
{
	if ( assprc != 0 ) {
		if ( handler_request(assprc) ) {
			handler_execution();
		}
	}
}

/* ------------------------------------------------------------------------ */
/*
 *	Cyclic handler
 */

#ifdef NUM_CYCID

EXPORT ID	max_cycid;	/* Maximum interval start ID */

/*
 * Cyclic handler control block
 */
typedef struct cyclic_handler_control_block {
	union {
		struct {
			VP	exinf;		/* Extended information */
			ATR	cycatr;		/* Cyclic handler attribute */
		} cbinf;
		QUEUE	free_queue;		/* Connection with free queue */
	} extcb;
	FP	cychdr;		/* Cyclic handler address */
	UINT	cycstat;	/* Cyclic handler state */
	RELTIM	cyctim;		/* Cyclic time */
	TMEB	cyctmeb;	/* Timer event block */
#if TA_GP
	VP	gp;		/* Global pointer */
#endif
	ID	cycid;				/* Cyclic handler ID */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
	UW	assprc;
	UINT	cycno;
} CYCCB;

LOCAL CYCCB	*cyccb_table;	/* Cyclic handler control block */
LOCAL QUEUE	free_cyccb;	/* FreeQue */

#define get_cyccb(id)	( &cyccb_table[INDEX_CYC(id)] )

#define TCYC_RUN		0x02U
#define TCYC_IMM		0x04U
#define get_cycstat(cyccb)	( cyccb->cycstat & ~(TCYC_RUN | TCYC_IMM) )
#define is_cyc_assprc(cyccb)	( (cyccb->extcb.cbinf.cycatr & TA_ASSPRC) != 0 )
#define is_cyc_run(cyccb)	( (cyccb->cycstat & TCYC_RUN) != 0 )
#define is_cyc_imm(almcb)	( (cyccb->cycstat & TCYC_IMM) != 0 )

/*
 * Initialization of cyclic handler control block
 */
EXPORT ER cyclichandler_initialize( void )
{
	CYCCB	*cyccb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXCYCID, &max_cycid, 1);
	if ( (n < 1) || (NUM_CYCID < 1) ) {
		return E_SYS;
	}

	/* Create cyclic handler control block */
	cyccb_table = Imalloc((UINT)NUM_CYCID * sizeof(CYCCB));
	if ( cyccb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FeeQue */
	QueInit(&free_cyccb);
	end = cyccb_table + NUM_CYCID;
	for ( cyccb = cyccb_table; cyccb < end; cyccb++ ) {
		cyccb->cychdr = NULL; /* Unregistered handler */
		cyccb->cycid  = 0;
		cyccb->cycno = 0;
		QueInsert(&cyccb->extcb.free_queue, &free_cyccb);
	}

	return E_OK;
}

/*
 * Next startup time
 */
Inline LSYSTIM cyc_next_time( CYCCB *cyccb )
{
	LSYSTIM		tm;
	longlong	n;

	tm = ll_add(cyccb->cyctmeb.time, uitoll(cyccb->cyctim));

	if ( ll_cmp(tm, current_time) <= 0 ) {

		/* Adjust time to be later than current time */
		tm = ll_sub(current_time, cyccb->cyctmeb.time);
		n  = lui_div(tm, cyccb->cyctim);
		ll_inc(&n);
		tm = lui_mul(n, cyccb->cyctim);
		tm = ll_add(cyccb->cyctmeb.time, tm);
	}

	return tm;
}

LOCAL void call_cychdr( CYCCB* cyccb );

/*
 * Cyclic handler distribution processing
 */
LOCAL void distribute_cychdr( CYCCB *cyccb )
{
	UINT	prid;

	if ( is_cyc_assprc(cyccb) ) {
		prid = assprc_to_prid(cyccb->assprc);
	} else {
		prid = get_prid();
	}
	enqueue_handler(prid, &cyccb->cyctmeb, (CBACK)call_cychdr, (VP)cyccb);
}

/*
 * Register timer event queue
 */
Inline void cyc_timer_insert( CYCCB *cyccb, LSYSTIM tm )
{
	timer_insert_abs(&cyccb->cyctmeb, tm, (CBACK)distribute_cychdr, cyccb);
}

/*
 * Copy cyclic handler control block
 */
Inline void copy_cyccb( CYCCB *dst, CYCCB *src )
{
	dst->extcb.cbinf.exinf = src->extcb.cbinf.exinf;
	dst->cychdr = src->cychdr;
#if TA_GP
	dst->gp = src->gp;
#endif
	dst->cycno = src->cycno;
}

/*
 * Cyclic handler routine
 */
LOCAL void call_cychdr( CYCCB *cyccb )
{
	CYCCB	para;

	/* Set next startup time */
	cyc_timer_insert(cyccb, cyc_next_time(cyccb));

	cyccb->cycstat |= TCYC_RUN;
	copy_cyccb(&para, cyccb);

	LEAVE_CRITICAL_SECTION;

	/* Execute cyclic handler / Enable interrupt nest */
	ENABLE_INTERRUPT_UPTO(TIMER_INTLEVEL);
	CallUserHandlerP1(para.extcb.cbinf.exinf, para.cychdr, &para);
	DISABLE_INTERRUPT;

	ENTER_CRITICAL_SECTION;

	if ( para.cycno == cyccb->cycno ) {
		cyccb->cycstat &= ~TCYC_RUN;
	}
}

/*
 * Immediate call of cyclic handler 
 */
LOCAL void immediate_call_cychdr( CYCCB *cyccb )
{
	UW	assprc;
	CYCCB	para;

	if ( is_cyc_assprc(cyccb) ) {
		distribute_cychdr(cyccb);
		assprc = check_handler_queue();
		handler_update(assprc);

	} else {
		/* Set next startup time */
		cyc_timer_insert(cyccb, cyc_next_time(cyccb));

		cyccb->cycstat |= (TCYC_RUN | TCYC_IMM);
		copy_cyccb(&para, cyccb);

		LEAVE_CRITICAL_SECTION;

		/* Execute cyclic handler in task-independent part
		   (Keep interrupt disabled) */
		ENTER_TASK_INDEPENDENT;
		CallUserHandlerP1(para.extcb.cbinf.exinf, para.cychdr, &para);
		LEAVE_TASK_INDEPENDENT;

		ENTER_CRITICAL_SECTION;

		if ( para.cycno == cyccb->cycno ) {
			cyccb->cycstat &= ~(TCYC_RUN | TCYC_IMM);
		}
	}
}

/*
 * Create cyclic handler 
 */
SYSCALL ID _tk_cre_cyc P1( T_CCYC *pk_ccyc )
{
#if CHK_RSATR
	const ATR VALID_CYCATR = {
		 TA_HLNG
		|TA_STA
		|TA_PHS
		|TA_GP
		|TA_DOMID
		|TA_ONAME
		|TA_ASSPRC
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;
	ID	domid, cycid;
	UB	*oname;

	CHECK_RSATR(pk_ccyc->cycatr, VALID_CYCATR);
	CHECK_PAR(pk_ccyc->cychdr != NULL);
	CHECK_PAR(pk_ccyc->cyctim > 0);
	CHECK_PATR(pk_ccyc->cycatr);
#if CHK_PAR
	if ( (pk_ccyc->cycatr & TA_ASSPRC) != 0 ) {
		if ( !chk_assprc(pk_ccyc->assprc) ) {
			return E_PAR;
		}
	}
#endif
#if CHK_ID
	if ( (pk_ccyc->cycatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_ccyc->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_ccyc->cycatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_ccyc->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_ccyc->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_ccyc->cycatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_CYC,
						   cyccb_table->oname,
						   sizeof(CYCCB),
						   domid,
						   pk_ccyc->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_ccyc->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_ccyc->cycatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	cyccb = (CYCCB*)QueRemoveNext(&free_cyccb);
	if ( cyccb == NULL ) {
		ercd = E_LIMIT;
		goto error_exit;
	}

	/* Initialize control block */
	cyccb->extcb.cbinf.exinf   = pk_ccyc->exinf;
	cyccb->extcb.cbinf.cycatr  = pk_ccyc->cycatr;
	cyccb->cychdr  = pk_ccyc->cychdr;
	cyccb->cyctim  = pk_ccyc->cyctim;
#if TA_GP
	if ( (pk_ccyc->cycatr & TA_GP) != 0 ) {
		gp = pk_ccyc->gp;
	}
	cyccb->gp = gp;
#endif
	if ( (pk_ccyc->cycatr & TA_ASSPRC) != 0 ) {
		cyccb->assprc = pk_ccyc->assprc;
	} else {
		cyccb->assprc = 0;
	}

	cycid = MP_GEN_DOMIDPART(domid) | ID_CYC(cyccb - cyccb_table);
	cyccb->cycid = cycid;

	/* Set object name */
	strncpy((char*)cyccb->oname, (char*)oname, OBJECT_NAME_LENGTH);
	/* Registration */
	mp_register_object_in_domain(domid,
				     MP_OBJECT_TYPE_CYC,
				     INDEX_CYC(cycid),
				     oname);

	/* First startup time
	 *	To guarantee the start of handler after the specified time,
	 *	add TIMER_PERIOD. 
	 */
	tm = ll_add(current_time, uitoll(pk_ccyc->cycphs));
	tm = ll_add(tm, uitoll(TIMER_PERIOD));

	if ( (pk_ccyc->cycatr & TA_STA) != 0 ) {
		/* Start cyclic handler */
		cyccb->cycstat = TCYC_STA;

		if ( pk_ccyc->cycphs == 0 ) {
			/* Immediate execution */
			cyccb->cyctmeb.time = tm;
			immediate_call_cychdr(cyccb);
		} else {
			/* Register onto timer event queue */
			cyc_timer_insert(cyccb, tm);
		}
	} else {
		/* Initialize only counter */
		cyccb->cycstat = TCYC_STP;
		cyccb->cyctmeb.time = tm;
	}

	ercd = cycid;

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Delete cyclic handler 
 */
SYSCALL ER _tk_del_cyc( ID cycid )
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(cycid, cyccb->cycid, cyccb->extcb.cbinf.cycatr);
		if ( ercd == E_OK ) {
			if ( (cyccb->cycstat & TCYC_STA) != 0 ) {
				/* Delete timer event queue */
				timer_delete(&cyccb->cyctmeb);
			}

			/* Unregistration */
			mp_unregister_object_in_domain(cycid,
						       MP_OBJECT_TYPE_CYC,
						       INDEX_CYC(cycid));

			/* Return to FreeQue */
			QueInsert(&cyccb->extcb.free_queue, &free_cyccb);
			cyccb->cychdr = NULL; /* Unregistered handler */
			cyccb->cycid  = 0;
			cyccb->cycno++;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}


/*
 * Start cyclic handler 
 */
SYSCALL ER _tk_sta_cyc( ID cycid )
{
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(cycid, cyccb->cycid, cyccb->extcb.cbinf.cycatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	if ( (cyccb->extcb.cbinf.cycatr & TA_PHS) != 0 ) {
		/* Continue cyclic phase */
		if ( (cyccb->cycstat & TCYC_STA) == 0 ) {
			/* Start cyclic handler */
			tm = cyccb->cyctmeb.time;
			if ( ll_cmp(tm, current_time) <= 0 ) {
				tm = cyc_next_time(cyccb);
			}
			cyc_timer_insert(cyccb, tm);
		}
	} else {
		/* Reset cyclic interval */
		if ( (cyccb->cycstat & TCYC_STA) != 0 ) {
			/* Stop once */
			timer_delete(&cyccb->cyctmeb);
		}

		/* FIRST ACTIVATION TIME
		 *	Adjust the first activation time with TIMER_PERIOD.
		 *	TIMER_PERIOD is Timer interrupt interval (millisecond).
		 */
		tm = ll_add(current_time, uitoll(cyccb->cyctim));
		tm = ll_add(tm, uitoll(TIMER_PERIOD));

		/* Start cyclic handler */
		cyc_timer_insert(cyccb, tm);
	}
	cyccb->cycstat |= TCYC_STA;

    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Stop cyclic handler 
 */
SYSCALL ER _tk_stp_cyc( ID cycid )
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(cycid, cyccb->cycid, cyccb->extcb.cbinf.cycatr);
		if ( ercd == E_OK ) {
			if ( (cyccb->cycstat & TCYC_STA) != 0 ) {
				/* Stop cyclic handler */
				timer_delete(&cyccb->cyctmeb);
			}
			cyccb->cycstat &= ~TCYC_STA;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer cyclic handler state
 */
SYSCALL ER _tk_ref_cyc( ID cycid, T_RCYC* pk_rcyc )
{
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(cycid, cyccb->cycid, cyccb->extcb.cbinf.cycatr);
		if ( ercd == E_OK ) {
			tm = cyccb->cyctmeb.time;
			if ( (cyccb->cycstat & TCYC_STA) == 0 ) {
				if ( ll_cmp(tm, current_time) <= 0 ) {
					tm = cyc_next_time(cyccb);
				}
			}
			tm = ll_sub(tm, current_time);
			tm = ll_sub(tm, uitoll(TIMER_PERIOD));
			if ( ll_sign(tm) < 0 ) {
				tm = ltoll(0);
			}

			pk_rcyc->exinf   = cyccb->extcb.cbinf.exinf;
			pk_rcyc->lfttim  = (RELTIM)tm;
			pk_rcyc->cycstat = get_cycstat(cyccb);
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
 * Find cyclic handler ID
 */
SYSCALL ER _tk_fnd_cyc( ID domid, UB *oname )
{
	ER	ercd;
	ID	cycid = 0;
	CYCCB	*cyccb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_CYC, (UB*)cyccb_table, cyccb_table->oname,
			       (UB*)&cyccb_table->cycid, sizeof(CYCCB), domid, oname);
	if ( 0 < ercd ) {
		cycid = (ID)ercd;
		cyccb = get_cyccb(cycid);
		ercd = mp_check_protection_fnd(cycid, cyccb->extcb.cbinf.cycatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: cycid;
}

/*
 * Get domain information on cyclic handler
 */
SYSCALL ER _tk_dmi_cyc( ID cycid, T_DMI *pk_dmi )
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(cycid, cyccb->cycid, cyccb->extcb.cbinf.cycatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(cyccb->extcb.cbinf.cycatr);
			pk_dmi->domid = mp_get_domid_from_id(cyccb->cycid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, cyccb->oname, OBJECT_NAME_LENGTH);
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#if USE_DBGSPT

/*
 * Get object name from control block
 */
EXPORT ER cyclichandler_getname(ID id, UB **name)
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(id);

	cyccb = get_cyccb(id);
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, cyccb->cycid) ) {
		ercd = E_NOEXS;
	} else if ( (cyccb->extcb.cbinf.cycatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = cyccb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER cyclichandler_setname(ID id, UB *name)
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(id);

	cyccb = get_cyccb(id);
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, cyccb->cycid) ) {
		ercd = E_NOEXS;
	} else if ( (cyccb->extcb.cbinf.cycatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_CYC, cyccb_table->oname, sizeof(CYCCB), id, name, INDEX_CYC(id));
	}

	return ercd;
}

/*
 * Refer cyclic handler usage state
 */
SYSCALL INT _td_lst_cyc( ID list[], INT nent )
{
	CYCCB	*cyccb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = cyccb_table + NUM_CYCID;
	for ( cyccb = cyccb_table; cyccb < end; cyccb++ ) {
		/* Unregistered handler */
		if ( cyccb->cychdr == NULL ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = cyccb->cycid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer cyclic handler state
 */
SYSCALL ER _td_ref_cyc( ID cycid, TD_RCYC* pk_rcyc )
{
	CYCCB	*cyccb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(cycid, cyccb->cycid) ) {
		ercd = E_NOEXS;
	} else {
		tm = cyccb->cyctmeb.time;
		if ( (cyccb->cycstat & TCYC_STA) == 0 ) {
			if ( ll_cmp(tm, current_time) <= 0 ) {
				tm = cyc_next_time(cyccb);
			}
		}
		tm = ll_sub(tm, current_time);
		tm = ll_sub(tm, uitoll(TIMER_PERIOD));
		if ( ll_sign(tm) < 0 ) {
			tm = ltoll(0);
		}

		pk_rcyc->exinf   = cyccb->extcb.cbinf.exinf;
		pk_rcyc->lfttim  = (RELTIM)tm;
		pk_rcyc->cycstat = get_cycstat(cyccb);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Find cyclic handler ID
 */
SYSCALL ER _td_fnd_cyc( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_CYC, (UB*)cyccb_table, cyccb_table->oname,
			       (UB*)&cyccb_table->cycid, sizeof(CYCCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on cyclic handler
 */
SYSCALL ER _td_dmi_cyc( ID cycid, TD_DMI *pk_dmi )
{
	CYCCB	*cyccb;
	ER	ercd = E_OK;

	CHECK_CYCID(cycid);

	cyccb = get_cyccb(cycid);

	BEGIN_CRITICAL_SECTION;
	if ( cyccb->cychdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(cycid, cyccb->cycid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(cyccb->extcb.cbinf.cycatr);
		pk_dmi->domid  = mp_get_domid_from_id(cyccb->cycid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, cyccb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_CYCID */

/* ------------------------------------------------------------------------ */
/*
 *	Alarm handler
 */

#ifdef NUM_ALMID

EXPORT ID	max_almid;	/* Maximum alarm handler ID */

/*
 * Alarm handler control block
 */
typedef struct alarm_handler_control_block {
	union {
		struct {
			VP	exinf;		/* Extended information */
			ATR	almatr;		/* Alarm handler attribute */
		} cbinf;
		QUEUE	free_queue;		/* Connection with free queue */
	} extcb;
	FP	almhdr;		/* Alarm handler address */
	UINT	almstat;	/* Alarm handler state */
	TMEB	almtmeb;	/* Timer event block */
#if TA_GP
	VP	gp;		/* Global pointer */
#endif
	ID	almid;				/* Alarm handler ID */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
	UW	assprc;
	UINT	almno;
} ALMCB;

LOCAL ALMCB	*almcb_table;	/* Alarm handler control block */
LOCAL QUEUE	free_almcb;	/* FreeQue */

#define get_almcb(id)	( &almcb_table[INDEX_ALM(id)] )

#define TALM_RUN		0x02U
#define TALM_IMM		0x04U
#define get_almstat(almcb)	( almcb->almstat & ~(TALM_RUN | TALM_IMM) )
#define is_alm_assprc(almcb)	( (almcb->extcb.cbinf.almatr & TA_ASSPRC) != 0 )
#define is_alm_run(almcb)	( (almcb->almstat & TALM_RUN) != 0 )
#define is_alm_imm(almcb)	( (almcb->almstat & TALM_IMM) != 0 )

/*
 * Initialization of alarm handler control block 
 */
EXPORT ER alarmhandler_initialize( void )
{
	ALMCB	*almcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXALMID, &max_almid, 1);
	if ( (n < 1) || (NUM_ALMID < 1) ) {
		return E_SYS;
	}

	/* Create alarm handler control block */
	almcb_table = Imalloc((UINT)NUM_ALMID * sizeof(ALMCB));
	if ( almcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FeeQue */
	QueInit(&free_almcb);
	end = almcb_table + NUM_ALMID;
	for ( almcb = almcb_table; almcb < end; almcb++ ) {
		almcb->almhdr = NULL; /* Unregistered handler */
		almcb->almid  = 0;
		almcb->almno = 0;
		QueInsert(&almcb->extcb.free_queue, &free_almcb);
	}

	return E_OK;
}

/*
 * Copy alarm handler control block
 */
Inline void copy_almcb( ALMCB *dst, ALMCB *src )
{
	dst->extcb.cbinf.exinf = src->extcb.cbinf.exinf;
	dst->almhdr = src->almhdr;
#if TA_GP
	dst->gp = src->gp;
#endif
	dst->almno = src->almno;
}

/*
 * Alarm handler start routine
 */
LOCAL void call_almhdr( ALMCB *almcb )
{
	ALMCB	para;


	almcb->almstat &= ~TALM_STA;
	almcb->almstat |= TALM_RUN;
	copy_almcb(&para, almcb);

	LEAVE_CRITICAL_SECTION;

	/* Execute alarm handler/ Enable interrupt nesting */
	ENABLE_INTERRUPT_UPTO(TIMER_INTLEVEL);
	CallUserHandlerP1(para.extcb.cbinf.exinf, para.almhdr, &para);
	DISABLE_INTERRUPT;

	ENTER_CRITICAL_SECTION;

	if ( para.almno == almcb->almno ) {
		almcb->almstat &= ~TALM_RUN;
	}
}

/*
 * Alarm handler distribution processing
 */
LOCAL void distribute_almhdr( ALMCB *almcb )
{
	UINT	prid;

	if ( is_alm_assprc(almcb) ) {
		prid = assprc_to_prid(almcb->assprc);
	} else {
		prid = get_prid();
	}
	enqueue_handler(prid, &almcb->almtmeb, (CBACK)call_almhdr, (VP)almcb);
}

/*
 * Alarm handler immediate call
 */
LOCAL void immediate_call_almhdr( ALMCB *almcb )
{
	UW	assprc;
	ALMCB	para;

	if ( is_alm_assprc(almcb) ) {
		almcb->almstat |= TALM_STA;
		distribute_almhdr(almcb);
		assprc = check_handler_queue();
		handler_update(assprc);
	} else {
		almcb->almstat &= ~TALM_STA;
		almcb->almstat |= (TALM_RUN | TALM_IMM);
		copy_almcb(&para, almcb);

		LEAVE_CRITICAL_SECTION;

		/* Execute alarm handler in task-independent part
		   (Keep interrupt disabled) */
		ENTER_TASK_INDEPENDENT;
		CallUserHandlerP1(para.extcb.cbinf.exinf, para.almhdr, &para);
		LEAVE_TASK_INDEPENDENT;

		ENTER_CRITICAL_SECTION;

		if ( para.almno == almcb->almno ) {
			almcb->almstat &= ~(TALM_RUN | TALM_IMM);
		}
	}
}

/*
 * Register onto timer event queue
 */
Inline void alm_timer_insert( ALMCB *almcb, RELTIM reltim )
{
	LSYSTIM	tm;

	/* To guarantee to start the handler after the specified time, 
	   add TIMER_PERIOD */
	tm = ll_add(current_time, uitoll(reltim));
	tm = ll_add(tm, uitoll(TIMER_PERIOD));

	timer_insert_abs(&almcb->almtmeb, tm, (CBACK)distribute_almhdr, almcb);
}


/*
 * Create alarm handler
 */
SYSCALL ID _tk_cre_alm P1( T_CALM *pk_calm )
{
#if CHK_RSATR
	const ATR VALID_ALMATR = {
		 TA_HLNG
		|TA_GP
		|TA_DOMID
		|TA_ONAME
		|TA_ASSPRC
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	ALMCB	*almcb;
	ER	ercd = E_OK;
	ID	domid, almid;
	UB	*oname;

	CHECK_RSATR(pk_calm->almatr, VALID_ALMATR);
	CHECK_PAR(pk_calm->almhdr != NULL);
	CHECK_PATR(pk_calm->almatr);
#if CHK_PAR
	if ( (pk_calm->almatr & TA_ASSPRC) != 0 ) {
		if ( !chk_assprc(pk_calm->assprc) ) {
			return E_PAR;
		}
	}
#endif
#if CHK_ID
	if ( (pk_calm->almatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_calm->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_calm->almatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_calm->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_calm->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_calm->almatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_ALM,
						   almcb_table->oname,
						   sizeof(ALMCB),
						   domid,
						   pk_calm->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_calm->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_calm->almatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from free queue */
	almcb = (ALMCB*)QueRemoveNext(&free_almcb);
	if ( almcb == NULL ) {
		ercd = E_LIMIT;
		goto error_exit;
	}

	/* Initialize control block */
	almcb->extcb.cbinf.exinf   = pk_calm->exinf;
	almcb->extcb.cbinf.almatr  = pk_calm->almatr;
	almcb->almhdr  = pk_calm->almhdr;
	almcb->almstat = TALM_STP;
#if TA_GP
	if ( (pk_calm->almatr & TA_GP) != 0 ) {
		gp = pk_calm->gp;
	}
	almcb->gp = gp;
#endif
	if ( (pk_calm->almatr & TA_ASSPRC) != 0 ) {
		almcb->assprc = pk_calm->assprc;
	} else {
		almcb->assprc = 0;
	}

	almid = MP_GEN_DOMIDPART(domid) | ID_ALM(almcb - almcb_table);
	almcb->almid = almid;

	/* Set object name */
	strncpy((char*)almcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
	/* Registration */
	mp_register_object_in_domain(domid,
				     MP_OBJECT_TYPE_ALM,
				     INDEX_ALM(almid),
				     oname);

	ercd = almid;

    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Delete alarm handler
 */
SYSCALL ER _tk_del_alm( ID almid )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(almid, almcb->almid, almcb->extcb.cbinf.almatr);
		if ( ercd == E_OK ) {
			if ( (almcb->almstat & TALM_STA) != 0 ) {
				/* Delete from timer event queue */
				timer_delete(&almcb->almtmeb);
			}

			/* Unregistration */
			mp_unregister_object_in_domain(almid,
						       MP_OBJECT_TYPE_ALM,
						       INDEX_ALM(almid));

			/* Return to FreeQue */
			QueInsert(&almcb->extcb.free_queue, &free_almcb);
			almcb->almhdr = NULL; /* Unregistered handler */
			almcb->almid  = 0;
			almcb->almno++;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}


/*
 * Start alarm handler
 */
SYSCALL ER _tk_sta_alm( ID almid, RELTIM almtim )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
		goto error_exit;
	}

	ercd = mp_check_domain_and_protection(almid, almcb->almid, almcb->extcb.cbinf.almatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	if ( is_alm_run(almcb) ) {
		if ( ((almtim == 0) && is_alm_imm(almcb))
		  || (!is_alm_assprc(almcb) && ((almtim == 0) || is_alm_imm(almcb))) ) {
			ercd = E_OBJ;
			goto error_exit;
		}
	}

	if ( (almcb->almstat & TALM_STA) != 0 ) {
		/* Cancel current settings */
		timer_delete(&almcb->almtmeb);
	}

	if ( almtim > 0 ) {
		/* Register onto timer event queue */
		alm_timer_insert(almcb, almtim);
		almcb->almstat |= TALM_STA;
	} else {
		/* Immediate execution */
		immediate_call_almhdr(almcb);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Stop alarm handler
 */
SYSCALL ER _tk_stp_alm( ID almid )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(almid, almcb->almid, almcb->extcb.cbinf.almatr);
		if ( ercd == E_OK ) {
			if ( (almcb->almstat & TALM_STA) != 0 ) {
				/* Stop alarm handler address */
				timer_delete(&almcb->almtmeb);
				almcb->almstat &= ~TALM_STA;
			}
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer alarm handler state
 */
SYSCALL ER _tk_ref_alm( ID almid, T_RALM *pk_ralm )
{
	ALMCB	*almcb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(almid, almcb->almid, almcb->extcb.cbinf.almatr);
		if ( ercd == E_OK ) {
			if ( (almcb->almstat & TALM_STA) != 0 ) {
				tm = almcb->almtmeb.time;
				tm = ll_sub(tm, current_time);
				tm = ll_sub(tm, uitoll(TIMER_PERIOD));
				if ( ll_sign(tm) < 0 ) {
					tm = ltoll(0);
				}
			} else {
				tm = ltoll(0);
			}

			pk_ralm->exinf   = almcb->extcb.cbinf.exinf;
			pk_ralm->lfttim  = (RELTIM)tm;
			pk_ralm->almstat = get_almstat(almcb);
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
 * Find alarm handler ID
 */
SYSCALL ER _tk_fnd_alm( ID domid, UB *oname )
{
	ER	ercd;
	ID	almid = 0;
	ALMCB	*almcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_ALM, (UB*)almcb_table, almcb_table->oname,
			       (UB*)&almcb_table->almid, sizeof(ALMCB), domid, oname);
	if ( 0 < ercd ) {
		almid = (ID)ercd;
		almcb = get_almcb(almid);
		ercd = mp_check_protection_fnd(almid, almcb->extcb.cbinf.almatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: almid;
}

/*
 * Get domain information on alarm handler
 */
SYSCALL ER _tk_dmi_alm( ID almid, T_DMI *pk_dmi )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(almid, almcb->almid, almcb->extcb.cbinf.almatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(almcb->extcb.cbinf.almatr);
			pk_dmi->domid = mp_get_domid_from_id(almcb->almid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, almcb->oname, OBJECT_NAME_LENGTH);
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#if USE_DBGSPT

/*
 * Get object name from control block
 */
EXPORT ER alarmhandler_getname(ID id, UB **name)
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(id);

	almcb = get_almcb(id);
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, almcb->almid) ) {
		ercd = E_NOEXS;
	} else if ( (almcb->extcb.cbinf.almatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = almcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER alarmhandler_setname(ID id, UB *name)
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(id);

	almcb = get_almcb(id);
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, almcb->almid) ) {
		ercd = E_NOEXS;
	} else if ( (almcb->extcb.cbinf.almatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_ALM, almcb_table->oname, sizeof(ALMCB), id, name, INDEX_ALM(id));
	}

	return ercd;
}

/*
 * Refer alarm handler usage state
 */
SYSCALL INT _td_lst_alm( ID list[], INT nent )
{
	ALMCB	*almcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = almcb_table + NUM_ALMID;
	for ( almcb = almcb_table; almcb < end; almcb++ ) {
		/* Unregistered handler */
		if ( almcb->almhdr == NULL ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = almcb->almid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer alarm handler state
 */
SYSCALL ER _td_ref_alm( ID almid, TD_RALM *pk_ralm )
{
	ALMCB	*almcb;
	LSYSTIM	tm;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(almid, almcb->almid) ) {
		ercd = E_NOEXS;
	} else {
		if ( (almcb->almstat & TALM_STA) != 0 ) {
			tm = almcb->almtmeb.time;
			tm = ll_sub(tm, current_time);
			tm = ll_sub(tm, uitoll(TIMER_PERIOD));
			if ( ll_sign(tm) < 0 ) {
				tm = ltoll(0);
			}
		} else {
			tm = ltoll(0);
		}

		pk_ralm->exinf   = almcb->extcb.cbinf.exinf;
		pk_ralm->lfttim  = (RELTIM)tm;
		pk_ralm->almstat = get_almstat(almcb);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Find alarm handler ID
 */
SYSCALL ER _td_fnd_alm( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_ALM, (UB*)almcb_table, almcb_table->oname,
			       (UB*)&almcb_table->almid, sizeof(ALMCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on alarm handler
 */
SYSCALL ER _td_dmi_alm( ID almid, TD_DMI *pk_dmi )
{
	ALMCB	*almcb;
	ER	ercd = E_OK;

	CHECK_ALMID(almid);

	almcb = get_almcb(almid);

	BEGIN_CRITICAL_SECTION;
	if ( almcb->almhdr == NULL ) { /* Unregistered handler */
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(almid, almcb->almid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(almcb->extcb.cbinf.almatr);
		pk_dmi->domid  = mp_get_domid_from_id(almcb->almid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, almcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_ALMID */
