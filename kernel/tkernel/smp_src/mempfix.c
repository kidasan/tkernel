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
 *	mempfix.c (T-Kernel/OS)
 *	Fixed Size Memory Pool
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

#ifdef NUM_MPFID

EXPORT ID	max_mpfid;	/* Maximum fixed size memory pool ID */

/*
 * Fixed size memory pool control block
 */
typedef struct free_list {
	struct free_list *next;
} FREEL;

typedef struct fix_memorypool_control_block {
	QUEUE	wait_queue;	/* Memory pool wait queue */
	ID	mpfid;		/* Fixed size memory pool ID */
	VP	exinf;		/* Extended information */
	ATR	mpfatr;		/* Memory pool attribute */
	INT	mpfcnt;		/* Number of blocks in whole memory pool */
	INT	blfsz;		/* Fixed size memory block size */
	INT	mpfsz;		/* Whole memory pool size */
	INT	frbcnt;		/* Number of blocks in free area */
	VP	mempool;	/* Top address of memory pool */
	VP	unused;		/* Top address of unused area */
	FREEL	*freelist;	/* Free block list */
	OBJLOCK	lock;		/* Lock for object exclusive access */
	/* For SMP T-Kernel */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
} MPFCB;

LOCAL MPFCB	*mpfcb_table;	/* Fixed size memory pool control block */
LOCAL QUEUE	free_mpfcb;	/* FreeQue */

#define get_mpfcb(id)	( &mpfcb_table[INDEX_MPF(id)] )


/*
 * Initialization of fixed size memory pool control block
 */
EXPORT ER fix_memorypool_initialize( void )
{
	MPFCB	*mpfcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXMPFID, &max_mpfid, 1);
	if ( (n < 1) || (NUM_MPFID < 1) ) {
		return E_SYS;
	}

	/* Create fixed size memory pool control block */
	mpfcb_table = Imalloc((UINT)NUM_MPFID * sizeof(MPFCB));
	if ( mpfcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&free_mpfcb);
	end = mpfcb_table + NUM_MPFID;
	for ( mpfcb = mpfcb_table; mpfcb < end; mpfcb++ ) {
		mpfcb->mpfid = 0;
		InitOBJLOCK(&mpfcb->lock);
		QueInsert(&mpfcb->wait_queue, &free_mpfcb);
	}

	return E_OK;
}


#define MINSIZE		( sizeof(FREEL) )
#define MINSZ(sz)	( ((sz) + (MINSIZE-1)) & ~(MINSIZE-1) )

/*
 * Return end address in memory pool area
 */
Inline VP mempool_end( MPFCB *mpfcb )
{
	return (VB*)mpfcb->mempool + mpfcb->mpfsz;
}


/*
 * Processing if the priority of wait task changes
 */
LOCAL void mpf_chg_pri( TCB *tcb, INT oldpri )
{
	MPFCB	*mpfcb;

	mpfcb = get_mpfcb(tcb->wid);
	gcb_change_priority((GCB*)mpfcb, tcb);
}

/*
 * Definition of fixed size memory pool wait specification
 */
LOCAL WSPEC wspec_mpf_tfifo = { TTW_MPF, NULL, NULL };
LOCAL WSPEC wspec_mpf_tpri  = { TTW_MPF, mpf_chg_pri, NULL };


/*
 * Create fixed size memory pool
 */
SYSCALL ID _tk_cre_mpf( T_CMPF *pk_cmpf )
{
#if CHK_RSATR
	const ATR VALID_MPFATR = {
		 TA_TPRI
		|TA_RNG3
		|TA_NORESIDENT
		|TA_NODISWAI
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	MPFCB	*mpfcb;
	ID	mpfid;
	INT	blfsz, mpfsz;
	VP	mempool;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_cmpf->mpfatr, VALID_MPFATR);
	CHECK_PAR(pk_cmpf->mpfcnt > 0);
	CHECK_PAR(pk_cmpf->blfsz > 0);
	CHECK_DISPATCH();
	CHECK_PATR(pk_cmpf->mpfatr);
#if CHK_ID
	if ( (pk_cmpf->mpfatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_cmpf->domid);
	}
#endif

	blfsz = (INT)MINSZ(pk_cmpf->blfsz);
	mpfsz = blfsz * pk_cmpf->mpfcnt;

	/* Allocate memory for memory pool */
	mempool = IAmalloc((UINT)mpfsz, pk_cmpf->mpfatr);
	if ( mempool == NULL ) {
		return E_NOMEM;
	}

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_cmpf->mpfatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_cmpf->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_cmpf->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_cmpf->mpfatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_MPF,
						   mpfcb_table->oname,
						   sizeof(MPFCB),
						   domid,
						   pk_cmpf->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_cmpf->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_cmpf->mpfatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	mpfcb = (MPFCB*)QueRemoveNext(&free_mpfcb);
	if ( mpfcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mpfid = MP_GEN_DOMIDPART(domid) | ID_MPF(mpfcb - mpfcb_table);

		/* Initialize control block */
		QueInit(&mpfcb->wait_queue);
		mpfcb->exinf    = pk_cmpf->exinf;
		mpfcb->mpfatr   = pk_cmpf->mpfatr;
		mpfcb->mpfcnt   = mpfcb->frbcnt = pk_cmpf->mpfcnt;
		mpfcb->blfsz    = blfsz;
		mpfcb->mpfsz    = mpfsz;
		mpfcb->unused   = mpfcb->mempool = mempool;
		mpfcb->freelist = NULL;

		/* Set object name */
		strncpy((char*)mpfcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(domid,
					     MP_OBJECT_TYPE_MPF,
					     INDEX_MPF(mpfid),
					     oname);

		mpfcb->mpfid    = mpfid;  /* Set ID after completion */

		ercd = mpfid;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	if ( ercd < E_OK ) {
		IAfree(mempool, pk_cmpf->mpfatr);
	}

	return ercd;
}

/*
 * Delete fixed size memory pool 
 */
SYSCALL ER _tk_del_mpf( ID mpfid )
{
	MPFCB	*mpfcb;
	VP	mempool = NULL;
	ATR	memattr = 0;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mpfid, mpfcb->mpfid, mpfcb->mpfatr);
		if ( ercd == E_OK ) {
			mempool = mpfcb->mempool;
			memattr = mpfcb->mpfatr;

			/* Release wait state of task (E_DLT) */
			wait_delete(&mpfcb->wait_queue);

			/* Unregistration */
			mp_unregister_object_in_domain(mpfid,
						       MP_OBJECT_TYPE_MPF,
						       INDEX_MPF(mpfid));

			/* Return to FreeQue */
			QueInsert(&mpfcb->wait_queue, &free_mpfcb);
			mpfcb->mpfid = 0;
		}
	}
	END_CRITICAL_SECTION;

	if ( mempool != NULL ) {
		IAfree(mempool, memattr);
	}

	return ercd;
}

/*
 * Get fixed size memory block 
 */
SYSCALL ER _tk_get_mpf( ID mpfid, VP *p_blf, TMO tmout )
{
	MPFCB	*mpfcb;
	FREEL	*freeList;
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_MPFID(mpfid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(mpfid, mpfcb->mpfid, mpfcb->mpfatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Check wait disable */
	if ( is_diswai((GCB*)mpfcb, ctxtsk[prid], TTW_MPF)  ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	/* If there is no space, ready for wait */
	if ( mpfcb->frbcnt <= 0 ) {
		ctxtsk[prid]->wspec = ( (mpfcb->mpfatr & TA_TPRI) != 0 )?
					&wspec_mpf_tpri: &wspec_mpf_tfifo;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.mpf.p_blf = p_blf;
		gcb_make_wait_with_diswai((GCB*)mpfcb, tmout);

		goto error_exit;
	}

	/* Get memory block */
	if ( mpfcb->freelist != NULL ) {
		freeList = mpfcb->freelist;
		mpfcb->freelist = freeList->next;
		*p_blf = freeList;
	} else {
		*p_blf = mpfcb->unused;
		mpfcb->unused = (VB*)mpfcb->unused + mpfcb->blfsz;
	}
	mpfcb->frbcnt--;

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Return fixed size memory block 
 */
SYSCALL ER _tk_rel_mpf( ID mpfid, VP blf )
{
	MPFCB	*mpfcb;
	TCB	*tcb;
	FREEL	*freeList;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(mpfid, mpfcb->mpfid, mpfcb->mpfatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
#if CHK_PAR
	if ( (blf < mpfcb->mempool) || (blf >= mempool_end(mpfcb))
	  || ((((VB*)blf - (VB*)mpfcb->mempool) % mpfcb->blfsz) != 0) ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	if ( !isQueEmpty(&mpfcb->wait_queue) ) {
		/* Send memory block to waiting task,
		   and then release the task */
		tcb = (TCB*)mpfcb->wait_queue.next;
		*tcb->winfo.mpf.p_blf = blf;
		wait_release_ok(tcb);
	} else {
		/* Free memory block */
		freeList = (FREEL*)blf;
		freeList->next = mpfcb->freelist;
		mpfcb->freelist = freeList;
		mpfcb->frbcnt++;
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Check fixed size pool state
 */
SYSCALL ER _tk_ref_mpf( ID mpfid, T_RMPF *pk_rmpf )
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);
	CHECK_DISPATCH();

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mpfid, mpfcb->mpfid, mpfcb->mpfatr);
		if ( ercd == E_OK ) {
			pk_rmpf->wtsk = wait_tskid(&mpfcb->wait_queue);
			pk_rmpf->exinf = mpfcb->exinf;
			pk_rmpf->frbcnt = mpfcb->frbcnt;
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
 * Find fixed size memory pool ID
 */
SYSCALL ER _tk_fnd_mpf( ID domid, UB *oname )
{
	ER	ercd;
	ID	mpfid = 0;
	MPFCB	*mpfcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_MPF, (UB*)mpfcb_table, mpfcb_table->oname,
			       (UB*)&mpfcb_table->mpfid, sizeof(MPFCB), domid, oname);
	if ( 0 < ercd ) {
		mpfid = (ID)ercd;
		mpfcb = get_mpfcb(mpfid);
		ercd = mp_check_protection_fnd(mpfid, mpfcb->mpfatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: mpfid;
}

/*
 * Get domain information on fixed size memory pool
 */
SYSCALL ER _tk_dmi_mpf( ID mpfid, T_DMI *pk_dmi )
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mpfid, mpfcb->mpfid, mpfcb->mpfatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(mpfcb->mpfatr);
			pk_dmi->domid = mp_get_domid_from_id(mpfcb->mpfid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, mpfcb->oname, OBJECT_NAME_LENGTH);
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
EXPORT ER fix_memorypool_getname(ID id, UB **name)
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(id);

	mpfcb = get_mpfcb(id);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, mpfcb->mpfid) ) {
		ercd = E_NOEXS;
	} else if ( (mpfcb->mpfatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = mpfcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER fix_memorypool_setname(ID id, UB *name)
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(id);

	mpfcb = get_mpfcb(id);
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, mpfcb->mpfid) ) {
		ercd = E_NOEXS;
	} else if ( (mpfcb->mpfatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_MPF, mpfcb_table->oname, sizeof(MPFCB), id, name, INDEX_MPF(id));
	}

	return ercd;
}

/*
 * Refer fixed size memory pool usage state
 */
SYSCALL INT _td_lst_mpf( ID list[], INT nent )
{
	MPFCB	*mpfcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = mpfcb_table + NUM_MPFID;
	for ( mpfcb = mpfcb_table; mpfcb < end; mpfcb++ ) {
		if ( mpfcb->mpfid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = mpfcb->mpfid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer fixed size memory pool state 
 */
SYSCALL ER _td_ref_mpf( ID mpfid, TD_RMPF *pk_rmpf )
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mpfid, mpfcb->mpfid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rmpf->wtsk = wait_tskid(&mpfcb->wait_queue);
		pk_rmpf->exinf = mpfcb->exinf;
		pk_rmpf->frbcnt = mpfcb->frbcnt;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer fixed size memory wait queue 
 */
SYSCALL INT _td_mpf_que( ID mpfid, ID list[], INT nent )
{
	MPFCB	*mpfcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mpfid, mpfcb->mpfid) ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mpfcb->wait_queue.next; q != &mpfcb->wait_queue; q = q->next ) {
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
 * Find fixed size memory pool ID
 */
SYSCALL ER _td_fnd_mpf( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_MPF, (UB*)mpfcb_table, mpfcb_table->oname,
			       (UB*)&mpfcb_table->mpfid, sizeof(MPFCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on fixed size memory pool
 */
SYSCALL ER _td_dmi_mpf( ID mpfid, TD_DMI *pk_dmi )
{
	MPFCB	*mpfcb;
	ER	ercd = E_OK;

	CHECK_MPFID(mpfid);

	mpfcb = get_mpfcb(mpfid);

	BEGIN_CRITICAL_SECTION;
	if ( mpfcb->mpfid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mpfid, mpfcb->mpfid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(mpfcb->mpfatr);
		pk_dmi->domid  = mp_get_domid_from_id(mpfcb->mpfid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, mpfcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_MPFID */
