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
 *	rendezvous.c (T-Kernel/OS)
 *	Rendezvous
 */

#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "check.h"
#include <sys/rominfo.h>

/* For SMP T-Kernel */
#include "smpkernel.h"
#include "mp_common.h"
#include "mp_domain.h"

#ifdef NUM_PORID

EXPORT ID	max_porid;	/* Maximum rendezvous port ID */

/*
 * Rendezvous port control block
 */
typedef struct port_control_block {
	QUEUE	call_queue;	/* Port call wait queue */
	ID	porid;		/* Port ID */
	VP	exinf;		/* Extended information */
	ATR	poratr;		/* Port attribute */
	QUEUE	accept_queue;	/* Port accept wait queue */
	INT	maxcmsz;	/* Maximum length of call message */
	INT	maxrmsz;	/* Maximum length of reply message */
	/* For SMP T-Kernel */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
} PORCB;

LOCAL PORCB	*porcb_table;	/* Rendezvous port control block */
LOCAL QUEUE	free_porcb;	/* FreeQue */

#define get_porcb(id)	( &porcb_table[INDEX_POR(id)] )


/* 
 * Initialization of port control block 
 */
EXPORT ER rendezvous_initialize( void )
{
	PORCB	*porcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXPORID, &max_porid, 1);
	if ( (n < 1) || (NUM_PORID < 1) ) {
		return E_SYS;
	}

	/* Create port control block */
	porcb_table = Imalloc((UINT)NUM_PORID * sizeof(PORCB));
	if ( porcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FeeQue */
	QueInit(&free_porcb);
	end = porcb_table + NUM_PORID;
	for ( porcb = porcb_table; porcb < end; porcb++ ) {
		porcb->porid = 0;
		QueInsert(&porcb->call_queue, &free_porcb);
	}

	return E_OK;
}

/*
 *  31    28      24              16      12                        0
 *  -----------------------------------------------------------------
 *  |      | PRID  |     DOMID     |  RNO  |         OBJID          |
 *  -----------------------------------------------------------------
 */
#define RDVNO_SHIFT	12
#define RDVNO_MASK	0xf000

/*
 * Create rendezvous number
 */
Inline RNO gen_rdvno( TCB *tcb )
{
	RNO	rdvno;

	rdvno = tcb->wrdvno;
	tcb->wrdvno = (tcb->wrdvno & ~RDVNO_MASK)
			| ((tcb->wrdvno + (1 << RDVNO_SHIFT)) & RDVNO_MASK);

	return rdvno;
}

/*
 * Get task ID from rendezvous number 
 */
Inline ID get_tskid_rdvno( RNO rdvno )
{
	return (rdvno & ~RDVNO_MASK);
}

/*
 * Check validity of rendezvous number
 */
#define CHECK_RDVNO(rdvno) {					\
	if ( !CHK_TSKID(get_tskid_rdvno(rdvno))			\
	  || !CHK_ID_DOM(get_tskid_rdvno(rdvno))		\
	  || !CHK_ID_PRC(get_tskid_rdvno(rdvno)) ) {		\
		return E_OBJ;					\
	}							\
}

/*
 * Processing if the priority of send wait task changes
 */
LOCAL void cal_chg_pri( TCB *tcb, INT oldpri )
{
	PORCB	*porcb;

	porcb = get_porcb(tcb->wid);
	gcb_change_priority((GCB*)porcb, tcb);
}

/*
 * Definition of rendezvous wait specification
 */
LOCAL WSPEC wspec_cal_tfifo = { TTW_CAL, NULL, NULL };
LOCAL WSPEC wspec_cal_tpri  = { TTW_CAL, cal_chg_pri, NULL };
LOCAL WSPEC wspec_acp       = { TTW_ACP, NULL, NULL };
LOCAL WSPEC wspec_rdv       = { TTW_RDV, NULL, NULL };


/*
 * Create rendezvous port
 */
SYSCALL ID _tk_cre_por( T_CPOR *pk_cpor )
{
#if CHK_RSATR
	const ATR VALID_PORATR = {
		 TA_TPRI
		|TA_NODISWAI
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	PORCB	*porcb;
	ID	porid;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_cpor->poratr, VALID_PORATR);
	CHECK_PAR(pk_cpor->maxcmsz >= 0);
	CHECK_PAR(pk_cpor->maxrmsz >= 0);
	CHECK_INTSK();
	CHECK_PATR(pk_cpor->poratr);
#if CHK_ID
	if ( (pk_cpor->poratr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_cpor->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_cpor->poratr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_cpor->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_cpor->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_cpor->poratr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_POR,
						   porcb_table->oname,
						   sizeof(PORCB),
						   domid,
						   pk_cpor->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_cpor->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_cpor->poratr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	porcb = (PORCB*)QueRemoveNext(&free_porcb);
	if ( porcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		porid = MP_GEN_DOMIDPART(domid) | ID_POR(porcb - porcb_table);

		/* Initialize control block */
		QueInit(&porcb->call_queue);
		porcb->porid = porid;
		porcb->exinf = pk_cpor->exinf;
		porcb->poratr = pk_cpor->poratr;
		QueInit(&porcb->accept_queue);
		porcb->maxcmsz = pk_cpor->maxcmsz;
		porcb->maxrmsz = pk_cpor->maxrmsz;

		/* Set object name */
		strncpy((char*)porcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(domid,
					     MP_OBJECT_TYPE_POR,
					     INDEX_POR(porid),
					     oname);

		ercd = porid;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Delete rendezvous port
 */
SYSCALL ER _tk_del_por( ID porid )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);
	CHECK_INTSK();

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(porid, porcb->porid, porcb->poratr);
		if ( ercd == E_OK ) {
			/* Release wait state of task (E_DLT) */
			wait_delete(&porcb->call_queue);
			wait_delete(&porcb->accept_queue);

			/* Unregistration */
			mp_unregister_object_in_domain(porid,
						       MP_OBJECT_TYPE_POR,
						       INDEX_POR(porid));

			/* Return to FreeQue */
			QueInsert(&porcb->call_queue, &free_porcb);
			porcb->porid = 0;
		}
	}
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Call rendezvous
 */
SYSCALL INT _tk_cal_por( ID porid, UW calptn, VP msg, INT cmsgsz, TMO tmout )
{
	PORCB	*porcb;
	TCB	*tcb;
	QUEUE	*queue;
	RNO	rdvno;
	INT	rmsgsz;
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_PORID(porid);
	CHECK_PAR(calptn != 0);
	CHECK_PAR(cmsgsz >= 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(porid, porcb->porid, porcb->poratr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
#if CHK_PAR
	if ( cmsgsz > porcb->maxcmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Search accept wait task */
	queue = porcb->accept_queue.next;
	while ( queue != &porcb->accept_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;
		if ( (calptn & tcb->winfo.acp.acpptn) == 0 ) {
			continue;
		}

		/* Check rendezvous call wait disable */
		if ( is_diswai((GCB*)porcb, ctxtsk[prid], TTW_CAL) ) {
			ercd = E_DISWAI;
			goto error_exit;
		}

		/* Send message */
		rdvno = gen_rdvno(ctxtsk[prid]);
		if ( cmsgsz > 0 ) {
			memcpy(tcb->winfo.acp.msg, msg, (UINT)cmsgsz);
		}
		*tcb->winfo.acp.p_rdvno = rdvno;
		*tcb->winfo.acp.p_cmsgsz = cmsgsz;
		wait_release_ok(tcb);

		/* Check rendezvous end wait disable */
		if ( is_diswai((GCB*)porcb, ctxtsk[prid], TTW_RDV) ) {
			ercd = E_DISWAI;
			goto error_exit;
		}

		/* Ready for rendezvous end wait */
		ercd = E_TMOUT;
		ctxtsk[prid]->wspec = &wspec_rdv;
		ctxtsk[prid]->wid = 0;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.rdv.rdvno = rdvno;
		ctxtsk[prid]->winfo.rdv.msg = msg;
		ctxtsk[prid]->winfo.rdv.maxrmsz = porcb->maxrmsz;
		ctxtsk[prid]->winfo.rdv.p_rmsgsz = &rmsgsz;
		make_wait(TMO_FEVR, porcb->poratr);
		QueInit(&ctxtsk[prid]->tskque);

		goto error_exit;
	}

	/* Ready for rendezvous call wait */
	ctxtsk[prid]->wspec = ( (porcb->poratr & TA_TPRI) != 0 )?
					&wspec_cal_tpri: &wspec_cal_tfifo;
	ctxtsk[prid]->wercd = &ercd;
	ctxtsk[prid]->winfo.cal.calptn = calptn;
	ctxtsk[prid]->winfo.cal.msg = msg;
	ctxtsk[prid]->winfo.cal.cmsgsz = cmsgsz;
	ctxtsk[prid]->winfo.cal.p_rmsgsz = &rmsgsz;
	gcb_make_wait_with_diswai((GCB*)porcb, tmout);

    error_exit:
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? ercd: rmsgsz;
}

/*
 * Accept rendezvous
 */
SYSCALL INT _tk_acp_por( ID porid, UINT acpptn, RNO *p_rdvno, VP msg, TMO tmout )
{
	PORCB	*porcb;
	TCB	*tcb;
	QUEUE	*queue;
	RNO	rdvno;
	INT	cmsgsz;
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_PORID(porid);
	CHECK_PAR(acpptn != 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	porcb = get_porcb(porid);
    
	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(porid, porcb->porid, porcb->poratr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Search call wait task */
	queue = porcb->call_queue.next;
	while ( queue != &porcb->call_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;
		if ( (acpptn & tcb->winfo.cal.calptn) == 0 ) {
			continue;
		}

		/* Check rendezvous accept wait disable */
		if ( is_diswai((GCB*)porcb, ctxtsk[prid], TTW_ACP) ) {
			ercd = E_DISWAI;
			goto error_exit;
		}

		/* Receive message */
		*p_rdvno = rdvno = gen_rdvno(tcb);
		cmsgsz = tcb->winfo.cal.cmsgsz;
		if ( cmsgsz > 0 ) {
			memcpy(msg, tcb->winfo.cal.msg, (UINT)cmsgsz);
		}

		/* Check rendezvous end wait disable */
		if ( is_diswai((GCB*)porcb, tcb, TTW_RDV) ) {
			wait_release_ng(tcb, E_DISWAI);
			goto error_exit;
		}
		wait_cancel(tcb);

		/* Make the other task at rendezvous end wait state */
		tcb->wspec = &wspec_rdv;
		tcb->wid = 0;
		tcb->winfo.rdv.rdvno = rdvno;
		tcb->winfo.rdv.msg = tcb->winfo.cal.msg;
		tcb->winfo.rdv.maxrmsz = porcb->maxrmsz;
		tcb->winfo.rdv.p_rmsgsz = tcb->winfo.cal.p_rmsgsz;
		timer_insert(&tcb->wtmeb, TMO_FEVR,
					(CBACK)wait_release_tmout, tcb);
		QueInit(&tcb->tskque);

		goto error_exit;
	}

	/* Check rendezvous accept wait disable */
	if ( is_diswai((GCB*)porcb, ctxtsk[prid], TTW_ACP) ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	ercd = E_TMOUT;
	if ( tmout != TMO_POL ) {
		/* Ready for rendezvous accept wait */
		ctxtsk[prid]->wspec = &wspec_acp;
		ctxtsk[prid]->wid = porid;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.acp.acpptn = acpptn;
		ctxtsk[prid]->winfo.acp.msg = msg;
		ctxtsk[prid]->winfo.acp.p_rdvno = p_rdvno;
		ctxtsk[prid]->winfo.acp.p_cmsgsz = &cmsgsz;
		make_wait(tmout, porcb->poratr);
		QueInsert(&ctxtsk[prid]->tskque, &porcb->accept_queue);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? ercd: cmsgsz;
}

/*
 * Forward Rendezvous to Other Port
 */
SYSCALL ER _tk_fwd_por( ID porid, UINT calptn, RNO rdvno, VP msg, INT cmsgsz )
{
	PORCB	*porcb;
	ID	calid;
	TCB	*caltcb, *tcb;
	QUEUE	*queue;
	RNO	new_rdvno;
	ER	ercd = E_OK;

	CHECK_PORID(porid);
	CHECK_PAR(calptn != 0);
	CHECK_RDVNO(rdvno);
	CHECK_PAR(cmsgsz >= 0);
	CHECK_INTSK();

	porcb  = get_porcb(porid);
	calid  = get_tskid_rdvno(rdvno);
	caltcb = get_tcb(calid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(porid, porcb->porid, porcb->poratr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
	/* It makes an error when the call task is a violation
	   of the protection attribute. */
	ercd = mp_check_protection_fwd(porcb->porid, porcb->poratr, caltcb);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
#if CHK_PAR
	if ( cmsgsz > porcb->maxcmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif
	if ( ((caltcb->state & TS_WAIT) == 0)
	  || (caltcb->wspec != &wspec_rdv)
	  || (rdvno != caltcb->winfo.rdv.rdvno) ) {
		ercd = E_OBJ;
		goto error_exit;
	}
	if ( porcb->maxrmsz > caltcb->winfo.rdv.maxrmsz ) {
		ercd = E_OBJ;
		goto error_exit;
	}
#if CHK_PAR
	if ( cmsgsz > caltcb->winfo.rdv.maxrmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Search accept wait task */
	queue = porcb->accept_queue.next;
	while ( queue != &porcb->accept_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;
		if ( (calptn & tcb->winfo.acp.acpptn) == 0 ) {
			continue;
		}

		/* Check rendezvous accept wait disable */
		if ( is_diswai((GCB*)porcb, caltcb, TTW_CAL) ) {
			wait_release_ng(caltcb, E_DISWAI);
			ercd = E_DISWAI;
			goto error_exit;
		}

		/* Send message */
		new_rdvno = gen_rdvno(caltcb);
		if ( cmsgsz > 0 ) {
			memcpy(tcb->winfo.acp.msg, msg, (UINT)cmsgsz);
		}
		*tcb->winfo.acp.p_rdvno = new_rdvno;
		*tcb->winfo.acp.p_cmsgsz = cmsgsz;
		wait_release_ok(tcb);

		/* Check rendezvous end wait disable */
		if ( is_diswai((GCB*)porcb, caltcb, TTW_RDV) ) {
			wait_release_ng(caltcb, E_DISWAI);
			ercd = E_DISWAI;
			goto error_exit;
		}

		/* Change rendezvous end wait of the other task */
		caltcb->winfo.rdv.rdvno = new_rdvno;
		caltcb->winfo.rdv.msg = caltcb->winfo.cal.msg;
		caltcb->winfo.rdv.maxrmsz = porcb->maxrmsz;
		caltcb->winfo.rdv.p_rmsgsz = caltcb->winfo.cal.p_rmsgsz;
		caltcb->nodiswai = ( (porcb->poratr & TA_NODISWAI) != 0 )? TRUE: FALSE;
		goto error_exit;
	}

	/* Check rendezvous accept wait disable */
	if ( is_diswai((GCB*)porcb, caltcb, TTW_CAL) ) {
		wait_release_ng(caltcb, E_DISWAI);
		ercd = E_DISWAI;
		goto error_exit;
	}

	/* Change the other task to rendezvous call wait */
	caltcb->wspec = ( (porcb->poratr & TA_TPRI) != 0 )?
				&wspec_cal_tpri: &wspec_cal_tfifo;
	caltcb->wid = porid;
	caltcb->winfo.cal.calptn = calptn;
	caltcb->winfo.cal.msg = caltcb->winfo.rdv.msg;
	caltcb->winfo.cal.cmsgsz = cmsgsz;
	caltcb->winfo.cal.p_rmsgsz = caltcb->winfo.rdv.p_rmsgsz;
	caltcb->nodiswai = ( (porcb->poratr & TA_NODISWAI) != 0 )? TRUE: FALSE;
	timer_insert(&caltcb->wtmeb, TMO_FEVR,
			(CBACK)wait_release_tmout, caltcb);
	if ( (porcb->poratr & TA_TPRI) != 0 ) {
		queue_insert_tpri(caltcb, &porcb->call_queue);
	} else {
		QueInsert(&caltcb->tskque, &porcb->call_queue);
	}

	if ( cmsgsz > 0 ) {
		memcpy(caltcb->winfo.cal.msg, msg, (UINT)cmsgsz);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Reply rendezvous
 */
SYSCALL ER _tk_rpl_rdv( RNO rdvno, VP msg, INT rmsgsz )
{
	TCB	*caltcb;
	ER	ercd = E_OK;

	CHECK_RDVNO(rdvno);
	CHECK_PAR(rmsgsz >= 0);
	CHECK_INTSK();

	caltcb = get_tcb(get_tskid_rdvno(rdvno));

	BEGIN_CRITICAL_SECTION;
	if ( ((caltcb->state & TS_WAIT) == 0)
	  || (caltcb->wspec != &wspec_rdv)
	  || (rdvno != caltcb->winfo.rdv.rdvno) ) {
		ercd = E_OBJ;
		goto error_exit;
	}
#if CHK_PAR
	if ( rmsgsz > caltcb->winfo.rdv.maxrmsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Send message */
	if ( rmsgsz > 0 ) {
		memcpy(caltcb->winfo.rdv.msg, msg, (UINT)rmsgsz);
	}
	*caltcb->winfo.rdv.p_rmsgsz = rmsgsz;
	wait_release_ok(caltcb);

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Refer rendezvous port
 */
SYSCALL ER _tk_ref_por( ID porid, T_RPOR *pk_rpor )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(porid, porcb->porid, porcb->poratr);
		if ( ercd == E_OK ) {
			pk_rpor->exinf = porcb->exinf;
			pk_rpor->wtsk = wait_tskid(&porcb->call_queue);
			pk_rpor->atsk = wait_tskid(&porcb->accept_queue);
			pk_rpor->maxcmsz = porcb->maxcmsz;
			pk_rpor->maxrmsz = porcb->maxrmsz;
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
 * Find rendezvous ID
 */
SYSCALL ID _tk_fnd_por( ID domid, UB *oname )
{
	ER	ercd;
	ID	porid = 0;
	PORCB	*porcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_POR, (UB*)porcb_table, porcb_table->oname,
			       (UB*)&porcb_table->porid, sizeof(PORCB), domid, oname);
	if ( 0 < ercd ) {
		porid = (ID)ercd;
		porcb = get_porcb(porid);
		ercd = mp_check_protection_fnd(porid, porcb->poratr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: porid;
}

/*
 * Get domain information on rendezvous
 */
SYSCALL ER _tk_dmi_por( ID porid, T_DMI *pk_dmi )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(porid, porcb->porid, porcb->poratr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(porcb->poratr);
			pk_dmi->domid = mp_get_domid_from_id(porcb->porid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, porcb->oname, OBJECT_NAME_LENGTH);
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

/*
 * Get object name from control block
 */
EXPORT ER rendezvous_getname(ID id, UB **name)
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(id);

	porcb = get_porcb(id);
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, porcb->porid) ) {
		ercd = E_NOEXS;
	} else if ( (porcb->poratr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = porcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER rendezvous_setname(ID id, UB *name)
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(id);

	porcb = get_porcb(id);
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, porcb->porid) ) {
		ercd = E_NOEXS;
	} else if ( (porcb->poratr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_POR, porcb_table->oname, sizeof(PORCB), id, name, INDEX_POR(id));
	}

	return ercd;
}

/*
 * Refer rendezvous port usage state
 */
SYSCALL INT _td_lst_por( ID list[], INT nent )
{
	PORCB	*porcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = porcb_table + NUM_PORID;
	for ( porcb = porcb_table; porcb < end; porcb++ ) {
		if ( porcb->porid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = porcb->porid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer rendezvous port
 */
SYSCALL ER _td_ref_por( ID porid, TD_RPOR *pk_rpor )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(porid, porcb->porid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rpor->exinf = porcb->exinf;
		pk_rpor->wtsk = wait_tskid(&porcb->call_queue);
		pk_rpor->atsk = wait_tskid(&porcb->accept_queue);
		pk_rpor->maxcmsz = porcb->maxcmsz;
		pk_rpor->maxrmsz = porcb->maxrmsz;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer rendezvous call wait queue
 */
SYSCALL INT _td_cal_que( ID porid, ID list[], INT nent )
{
	PORCB	*porcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(porid, porcb->porid) ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = porcb->call_queue.next; q != &porcb->call_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer rendezvous accept wait queue
 */
SYSCALL INT _td_acp_que( ID porid, ID list[], INT nent )
{
	PORCB	*porcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(porid, porcb->porid) ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = porcb->accept_queue.next; q != &porcb->accept_queue; q = q->next ) {
			if ( n++ < nent ) {
				*list++ = ((TCB*)q)->tskid;
			}
		}
		ercd = n;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Find rendezvous ID
 */
SYSCALL ID _td_fnd_por( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_POR, (UB*)porcb_table, porcb_table->oname,
			       (UB*)&porcb_table->porid, sizeof(PORCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on rendezvous
 */
SYSCALL ER _td_dmi_por( ID porid, TD_DMI *pk_dmi )
{
	PORCB	*porcb;
	ER	ercd = E_OK;

	CHECK_PORID(porid);

	porcb = get_porcb(porid);

	BEGIN_CRITICAL_SECTION;
	if ( porcb->porid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(porid, porcb->porid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(porcb->poratr);
		pk_dmi->domid  = mp_get_domid_from_id(porcb->porid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, porcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_PORID */
