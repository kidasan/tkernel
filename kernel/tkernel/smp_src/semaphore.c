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
 *	semaphore.c (T-Kernel/OS)
 *	Semaphore
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

#ifdef NUM_SEMID

EXPORT ID	max_semid;	/* Maximum semaphore ID */

/*
 * Semaphore control block
 */
typedef struct semaphore_control_block {
	QUEUE	wait_queue;	/* Semaphore wait queue */
	ID	semid;		/* Semaphore ID */
	VP	exinf;		/* Extended information */
	ATR	sematr;		/* Semaphore attribute */
	INT	semcnt;		/* Semaphore current count value */
	INT	maxsem;		/* Semaphore maximum count value */
	/* For SMP T-Kernel */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
} SEMCB;

LOCAL SEMCB	*semcb_table;	/* Semaphore control block */
LOCAL QUEUE	free_semcb;	/* FreeQue */

#define get_semcb(id)	( &semcb_table[INDEX_SEM(id)] )


/* 
 * Initialization of semaphore control block 
 */
EXPORT ER semaphore_initialize( void )
{
	SEMCB	*semcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXSEMID, &max_semid, 1);
	if ( (n < 1) || (NUM_SEMID < 1) ) {
		return E_SYS;
	}

	/* Semaphore control block */
	semcb_table = Imalloc((UINT)NUM_SEMID * sizeof(SEMCB));
	if ( semcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FeeQue */
	QueInit(&free_semcb);
	end = semcb_table + NUM_SEMID;
	for ( semcb = semcb_table; semcb < end; semcb++ ) {
		semcb->semid = 0;
		QueInsert(&semcb->wait_queue, &free_semcb);
	}

	return E_OK;
}


/*
 * Processing if the priority of wait task changes
 */
LOCAL void sem_chg_pri( TCB *tcb, INT oldpri )
{
	SEMCB	*semcb;
	QUEUE	*queue;
	TCB	*top;

	semcb = get_semcb(tcb->wid);
	if ( oldpri >= 0 ) {
		/* Reorder wait line */
		gcb_change_priority((GCB*)semcb, tcb);
	}

	if ( (semcb->sematr & TA_CNT) != 0 ) {
		return;
	}

	/* From the head task in a wait queue, allocate semaphore counts
	   and release wait state as much as possible */
	queue = semcb->wait_queue.next;
	while ( queue != &semcb->wait_queue ) {
		top = (TCB*)queue;
		queue = queue->next;

		/* Meet condition for releasing wait? */
		if ( semcb->semcnt < top->winfo.sem.cnt ) {
			break;
		}

		/* Release wait */
		wait_release_ok(top);

		semcb->semcnt -= top->winfo.sem.cnt;
	}
}

/*
 * Processing if the wait task is freed
 */
LOCAL void sem_rel_wai( TCB *tcb )
{
	sem_chg_pri(tcb, -1);
}

/*
 * Definition of semaphore wait specification
 */
LOCAL WSPEC wspec_sem_tfifo = { TTW_SEM, NULL,        sem_rel_wai };
LOCAL WSPEC wspec_sem_tpri  = { TTW_SEM, sem_chg_pri, sem_rel_wai };


/*
 * Create semaphore
 */
SYSCALL ID _tk_cre_sem( T_CSEM *pk_csem )
{
#if CHK_RSATR
	const ATR VALID_SEMATR = {
		 TA_TPRI
		|TA_CNT
		|TA_NODISWAI
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	SEMCB	*semcb;
	ID	semid;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_csem->sematr, VALID_SEMATR);
	CHECK_PAR(pk_csem->isemcnt >= 0);
	CHECK_PAR(pk_csem->maxsem > 0);
	CHECK_PAR(pk_csem->maxsem >= pk_csem->isemcnt);
	CHECK_PATR(pk_csem->sematr);
#if CHK_ID
	if ( (pk_csem->sematr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_csem->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_csem->sematr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_csem->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_csem->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_csem->sematr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_SEM,
						   semcb_table->oname,
						   sizeof(SEMCB),
						   domid,
						   pk_csem->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_csem->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_csem->sematr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	semcb = (SEMCB*)QueRemoveNext(&free_semcb);
	if ( semcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		semid = MP_GEN_DOMIDPART(domid) | ID_SEM(semcb - semcb_table);

		/* Initialize control block */
		QueInit(&semcb->wait_queue);
		semcb->semid = semid;
		semcb->exinf = pk_csem->exinf;
		semcb->sematr = pk_csem->sematr;
		semcb->semcnt = pk_csem->isemcnt;
		semcb->maxsem = pk_csem->maxsem;

		/* Set object name */
		strncpy((char*)semcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(domid,
					     MP_OBJECT_TYPE_SEM,
					     INDEX_SEM(semid),
					     oname);

		ercd = semid;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Delete semaphore
 */
SYSCALL ER _tk_del_sem( ID semid )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(semid, semcb->semid, semcb->sematr);
		if ( ercd == E_OK ) {
			/* Release wait state of task (E_DLT) */
			wait_delete(&semcb->wait_queue);

			/* Unregistration */
			mp_unregister_object_in_domain(semid,
						       MP_OBJECT_TYPE_SEM,
						       INDEX_SEM(semid));

			/* Return to FreeQue */
			QueInsert(&semcb->wait_queue, &free_semcb);
			semcb->semid = 0;
		}
	}
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Signal semaphore
 */
SYSCALL ER _tk_sig_sem( ID semid, INT cnt )
{
	SEMCB	*semcb;
	TCB	*tcb;
	QUEUE	*queue;
	ER	ercd = E_OK;
    
	CHECK_SEMID(semid);
	CHECK_PAR(cnt > 0);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(semid, semcb->semid, semcb->sematr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
	if ( cnt > (semcb->maxsem - semcb->semcnt) ) {
		ercd = E_QOVR;
		goto error_exit;
	}

	/* Return semaphore counts */
	semcb->semcnt += cnt;

	/* Search task that frees wait */
	queue = semcb->wait_queue.next;
	while ( queue != &semcb->wait_queue ) {
		tcb = (TCB*)queue;
		queue = queue->next;

		/* Meet condition for Releasing wait? */
		if ( semcb->semcnt < tcb->winfo.sem.cnt ) {
			if ( (semcb->sematr & TA_CNT) == 0 ) {
				break;
			}
			continue;
		}

		/* Release wait */
		wait_release_ok(tcb);

		semcb->semcnt -= tcb->winfo.sem.cnt;
		if ( semcb->semcnt <= 0 ) {
			break;
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Wait on semaphore
 */
SYSCALL ER _tk_wai_sem( ID semid, INT cnt, TMO tmout )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;
	INT	prid;

	CHECK_SEMID(semid);
	CHECK_PAR(cnt > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(semid, semcb->semid, semcb->sematr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
#if CHK_PAR
	if ( cnt > semcb->maxsem ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Check wait disable */
	if ( is_diswai((GCB*)semcb, ctxtsk[prid], TTW_SEM) ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	if ( (((semcb->sematr & TA_CNT) != 0)
	      || (gcb_top_of_wait_queue((GCB*)semcb, ctxtsk[prid]) == ctxtsk[prid]))
	  && (semcb->semcnt >= cnt) ) {
		/* Get semaphore count */
		semcb->semcnt -= cnt;

	} else {
		/* Ready for wait */
		ctxtsk[prid]->wspec = ( (semcb->sematr & TA_TPRI) != 0 )?
					&wspec_sem_tpri: &wspec_sem_tfifo;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.sem.cnt = cnt;
		gcb_make_wait_with_diswai((GCB*)semcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Refer semaphore state
 */
SYSCALL ER _tk_ref_sem( ID semid, T_RSEM *pk_rsem )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(semid, semcb->semid, semcb->sematr);
		if ( ercd == E_OK ) {
			pk_rsem->exinf  = semcb->exinf;
			pk_rsem->wtsk   = wait_tskid(&semcb->wait_queue);
			pk_rsem->semcnt = semcb->semcnt;
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
 * Find semaphore ID
 */
SYSCALL ER _tk_fnd_sem( ID domid, UB *oname )
{
	ER	ercd;
	ID	semid = 0;
	SEMCB	*semcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_SEM, (UB*)semcb_table, semcb_table->oname,
			       (UB*)&semcb_table->semid, sizeof(SEMCB), domid, oname);
	if ( 0 < ercd ) {
		semid = (ID)ercd;
		semcb = get_semcb(semid);
		ercd = mp_check_protection_fnd(semid, semcb->sematr);
	}
	END_CRITICAL_SECTION;

	return ( ercd < E_OK )? ercd: semid;
}

/*
 * Get domain information on semaphore
 */
SYSCALL ER _tk_dmi_sem( ID semid, T_DMI *pk_dmi )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(semid, semcb->semid, semcb->sematr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(semcb->sematr);
			pk_dmi->domid = mp_get_domid_from_id(semcb->semid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, semcb->oname, OBJECT_NAME_LENGTH);
		}
	}
	END_CRITICAL_SECTION;

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
EXPORT ER semaphore_getname(ID id, UB **name)
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(id);

	semcb = get_semcb(id);
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, semcb->semid) ) {
		ercd = E_NOEXS;
	} else if ( (semcb->sematr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = semcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER semaphore_setname(ID id, UB *name)
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(id);

	semcb = get_semcb(id);
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, semcb->semid) ) {
		ercd = E_NOEXS;
	} else if ( (semcb->sematr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_SEM, semcb_table->oname, sizeof(SEMCB), id, name, INDEX_SEM(id));
	}

	return ercd;
}

/*
 * Refer object usage state
 */
SYSCALL INT _td_lst_sem( ID list[], INT nent )
{
	SEMCB	*semcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = semcb_table + NUM_SEMID;
	for ( semcb = semcb_table; semcb < end; semcb++ ) {
		if ( semcb->semid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = semcb->semid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer object state
 */
SYSCALL ER _td_ref_sem( ID semid, TD_RSEM *rsem )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(semid, semcb->semid) ) {
		ercd = E_NOEXS;
	} else {
		rsem->exinf  = semcb->exinf;
		rsem->wtsk   = wait_tskid(&semcb->wait_queue);
		rsem->semcnt = semcb->semcnt;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer wait queue
 */
SYSCALL INT _td_sem_que( ID semid, ID list[], INT nent )
{
	SEMCB	*semcb;
	QUEUE	*q;
	ER	ercd;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(semid, semcb->semid) ) {
		ercd = E_NOEXS;
	} else {
		INT	n = 0;
		for ( q = semcb->wait_queue.next; q != &semcb->wait_queue; q = q->next ) {
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
 * Find semaphore ID
 */
SYSCALL ER _td_fnd_sem( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_SEM, (UB*)semcb_table, semcb_table->oname,
			       (UB*)&semcb_table->semid, sizeof(SEMCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on semaphore
 */
SYSCALL ER _td_dmi_sem( ID semid, TD_DMI *pk_dmi )
{
	SEMCB	*semcb;
	ER	ercd = E_OK;

	CHECK_SEMID(semid);

	semcb = get_semcb(semid);

	BEGIN_CRITICAL_SECTION;
	if ( semcb->semid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(semid, semcb->semid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(semcb->sematr);
		pk_dmi->domid  = mp_get_domid_from_id(semcb->semid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, semcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_SEMID */
