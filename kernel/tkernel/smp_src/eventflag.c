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
 *	eventflag.c (T-Kernel/OS)
 *	Event Flag
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

#ifdef NUM_FLGID

EXPORT ID	max_flgid;	/* Maximum event flag ID */

/*
 * Event flag control block
 */
typedef struct eventflag_control_block {
	QUEUE	wait_queue;	/* Event flag wait queue */
	ID	flgid;		/* Event flag ID */
	VP	exinf;		/* Extended information */
	ATR	flgatr;		/* Event flag attribute */
	UINT	flgptn;		/* Event flag current pattern */
	/* For SMP T-Kernel */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
} FLGCB;

LOCAL FLGCB	*flgcb_table;	/* Event flag control block */
LOCAL QUEUE	free_flgcb;	/* FreeQue */

#define get_flgcb(id)	( &flgcb_table[INDEX_FLG(id)] )


/*
 * Initialization of event flag control block 
 */
EXPORT ER eventflag_initialize( void )
{
	FLGCB	*flgcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXFLGID, &max_flgid, 1);
	if ( (n < 1) || (NUM_FLGID < 1) ) {
		return E_SYS;
	}

	/* Create event flag control block */
	flgcb_table = Imalloc((UINT)NUM_FLGID * sizeof(FLGCB));
	if ( flgcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&free_flgcb);
	end = flgcb_table + NUM_FLGID;
	for ( flgcb = flgcb_table; flgcb < end; flgcb++ ) {
		flgcb->flgid = 0;
		QueInsert(&flgcb->wait_queue, &free_flgcb);
	}

	return E_OK;
}

/*
 * Check for event flag wait release condition
 */
Inline BOOL eventflag_cond( FLGCB *flgcb, UINT waiptn, UINT wfmode )
{
	if ( (wfmode & TWF_ORW) != 0 ) {
		return ( (flgcb->flgptn & waiptn) != 0 );
	} else {
		return ( (flgcb->flgptn & waiptn) == waiptn );
	}
}

/*
 * Processing if the priority of wait task changes
 */
LOCAL void flg_chg_pri( TCB *tcb, INT oldpri )
{
	FLGCB	*flgcb;

	flgcb = get_flgcb(tcb->wid);
	gcb_change_priority((GCB*)flgcb, tcb);
}

/*
 * Definition of event flag wait specification
 */
LOCAL WSPEC wspec_flg_tfifo = { TTW_FLG, NULL, NULL };
LOCAL WSPEC wspec_flg_tpri  = { TTW_FLG, flg_chg_pri, NULL };


/*
 * Create event flag
 */
SYSCALL ID _tk_cre_flg( T_CFLG *pk_cflg )
{
#if CHK_RSATR
	const ATR VALID_FLGATR = {
		 TA_TPRI
		|TA_WMUL
		|TA_NODISWAI
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	FLGCB	*flgcb;
	ID	flgid;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_cflg->flgatr, VALID_FLGATR);
	CHECK_PATR(pk_cflg->flgatr);
#if CHK_ID
	if ( (pk_cflg->flgatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_cflg->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_cflg->flgatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_cflg->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_cflg->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_cflg->flgatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_FLG,
						   flgcb_table->oname,
						   sizeof(FLGCB),
						   domid,
						   pk_cflg->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_cflg->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_cflg->flgatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	flgcb = (FLGCB*)QueRemoveNext(&free_flgcb);
	if ( flgcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		flgid = MP_GEN_DOMIDPART(domid) | ID_FLG(flgcb - flgcb_table);

		/* Initialize control block */
		QueInit(&flgcb->wait_queue);
		flgcb->flgid = flgid;
		flgcb->exinf = pk_cflg->exinf;
		flgcb->flgatr = pk_cflg->flgatr;
		flgcb->flgptn = pk_cflg->iflgptn;

		/* Set object name */
		strncpy((char*)flgcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(domid,
					     MP_OBJECT_TYPE_FLG,
					     INDEX_FLG(flgid),
					     oname);

		ercd = flgid;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Delete event flag
 */
SYSCALL ER _tk_del_flg( ID flgid )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(flgid, flgcb->flgid, flgcb->flgatr);
		if ( ercd == E_OK ) {
			/* Release wait state of task (E_DLT) */
			wait_delete(&flgcb->wait_queue);

			/* Unregistration */
			mp_unregister_object_in_domain(flgid,
						       MP_OBJECT_TYPE_FLG,
						       INDEX_FLG(flgid));

			/* Return to FreeQue */
			QueInsert(&flgcb->wait_queue, &free_flgcb);
			flgcb->flgid = 0;
		}
	}
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Event flag set
 */
SYSCALL ER _tk_set_flg( ID flgid, UINT setptn )
{
	FLGCB	*flgcb;
	TCB	*tcb;
	QUEUE	*queue;
	UINT	wfmode, waiptn;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(flgid, flgcb->flgid, flgcb->flgatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Set event flag */
	flgcb->flgptn |= setptn;

	/* Search task which should be released */
	queue = flgcb->wait_queue.next;
	while ( queue != &flgcb->wait_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;

		/* Meet condition for release wait? */
		waiptn = tcb->winfo.flg.waiptn;
		wfmode = tcb->winfo.flg.wfmode;
		if ( eventflag_cond(flgcb, waiptn, wfmode) ) {

			/* Release wait */
			*tcb->winfo.flg.p_flgptn = flgcb->flgptn;
			wait_release_ok(tcb);

			/* Clear event flag */
			if ( (wfmode & TWF_BITCLR) != 0 ) {
				if ( (flgcb->flgptn &= ~waiptn) == 0 ) {
					break;
				}
			}
			if ( (wfmode & TWF_CLR) != 0 ) {
				flgcb->flgptn = 0;
				break;
			}
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Clear event flag 
 */
SYSCALL ER _tk_clr_flg( ID flgid, UINT clrptn )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(flgid, flgcb->flgid, flgcb->flgatr);
		if ( ercd == E_OK ) {
			flgcb->flgptn &= clrptn;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Event flag wait
 */
SYSCALL ER _tk_wai_flg( ID flgid, UINT waiptn, UINT wfmode, UINT *p_flgptn, TMO tmout )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_FLGID(flgid);
	CHECK_PAR(waiptn != 0);
	CHECK_PAR((wfmode & ~(TWF_ORW|TWF_CLR|TWF_BITCLR)) == 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(flgid, flgcb->flgid, flgcb->flgatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	if ( ((flgcb->flgatr & TA_WMUL) == 0) && !isQueEmpty(&flgcb->wait_queue) ) {
		/* Disable multiple tasks wait */
		ercd = E_OBJ;
		goto error_exit;
	}

	/* Check wait disable */
	if ( is_diswai((GCB*)flgcb, ctxtsk[prid], TTW_FLG) ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	/* Meet condition for release wait? */
	if ( eventflag_cond(flgcb, waiptn, wfmode) ) {
		*p_flgptn = flgcb->flgptn;

		/* Clear event flag */
		if ( (wfmode & TWF_BITCLR) != 0 ) {
			flgcb->flgptn &= ~waiptn;
		}
		if ( (wfmode & TWF_CLR) != 0 ) {
			flgcb->flgptn = 0;
		}
	} else {
		/* Ready for wait */
		ctxtsk[prid]->wspec = ( (flgcb->flgatr & TA_TPRI) != 0 )?
					&wspec_flg_tpri: &wspec_flg_tfifo;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.flg.waiptn = waiptn;
		ctxtsk[prid]->winfo.flg.wfmode = wfmode;
		ctxtsk[prid]->winfo.flg.p_flgptn = p_flgptn;
		gcb_make_wait_with_diswai((GCB*)flgcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Check event flag state
 */
SYSCALL ER _tk_ref_flg( ID flgid, T_RFLG *pk_rflg )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(flgid, flgcb->flgid, flgcb->flgatr);
		if ( ercd == E_OK ) {
			pk_rflg->exinf = flgcb->exinf;
			pk_rflg->wtsk = wait_tskid(&flgcb->wait_queue);
			pk_rflg->flgptn = flgcb->flgptn;
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
 * Find eventflag ID
 */
SYSCALL ER _tk_fnd_flg( ID domid, UB *oname )
{
	ER	ercd;
	ID	flgid = 0;
	FLGCB	*flgcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_FLG, (UB*)flgcb_table, flgcb_table->oname,
			       (UB*)&flgcb_table->flgid, sizeof(FLGCB), domid, oname);
	if ( 0 < ercd ) {
		flgid = (ID)ercd;
		flgcb = get_flgcb(flgid);
		ercd = mp_check_protection_fnd(flgid, flgcb->flgatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: flgid;
}


/*
 * Get domain information on event flag
 */
SYSCALL ER _tk_dmi_flg( ID flgid, T_DMI *pk_dmi )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(flgid, flgcb->flgid, flgcb->flgatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(flgcb->flgatr);
			pk_dmi->domid = mp_get_domid_from_id(flgcb->flgid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, flgcb->oname, OBJECT_NAME_LENGTH);
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
EXPORT ER eventflag_getname(ID id, UB **name)
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(id);

	flgcb = get_flgcb(id);
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, flgcb->flgid) ) {
		ercd = E_NOEXS;
	} else if ( (flgcb->flgatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = flgcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER eventflag_setname(ID id, UB *name)
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(id);

	flgcb = get_flgcb(id);
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, flgcb->flgid) ) {
		ercd = E_NOEXS;
	} else if ( (flgcb->flgatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_FLG, flgcb_table->oname, sizeof(FLGCB), id, name, INDEX_FLG(id));
	}

	return ercd;
}

/*
 * Refer event flag usage state
 */
SYSCALL INT _td_lst_flg( ID list[], INT nent )
{
	FLGCB	*flgcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = flgcb_table + NUM_FLGID;
	for ( flgcb = flgcb_table; flgcb < end; flgcb++ ) {
		if ( flgcb->flgid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = flgcb->flgid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer event flag state
 */
SYSCALL ER _td_ref_flg( ID flgid, TD_RFLG *pk_rflg )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(flgid, flgcb->flgid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rflg->exinf = flgcb->exinf;
		pk_rflg->wtsk = wait_tskid(&flgcb->wait_queue);
		pk_rflg->flgptn = flgcb->flgptn;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer event flag wait queue
 */
SYSCALL INT _td_flg_que( ID flgid, ID list[], INT nent )
{
	FLGCB	*flgcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(flgid, flgcb->flgid) ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = flgcb->wait_queue.next; q != &flgcb->wait_queue; q = q->next ) {
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
 * Find eventflag ID
 */
SYSCALL ER _td_fnd_flg( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_FLG, (UB*)flgcb_table, flgcb_table->oname,
			       (UB*)&flgcb_table->flgid, sizeof(FLGCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on event flag
 */
SYSCALL ER _td_dmi_flg( ID flgid, TD_DMI *pk_dmi )
{
	FLGCB	*flgcb;
	ER	ercd = E_OK;

	CHECK_FLGID(flgid);

	flgcb = get_flgcb(flgid);

	BEGIN_CRITICAL_SECTION;
	if ( flgcb->flgid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(flgid, flgcb->flgid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(flgcb->flgatr);
		pk_dmi->domid  = mp_get_domid_from_id(flgcb->flgid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, flgcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_FLGID */
