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
 *	mempool.c (T-Kernel/OS)
 *	Variable Size Memory Pool
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

#ifdef NUM_MPLID

EXPORT ID	max_mplid;	/* Maximum variable size memory pool ID */

/*
 * Variable size memory pool control block
 *	'areaque' connects memory blocks in address ascending order
 *	'freeque' connects memory blocks in size increasing order
 */
typedef struct memorypool_control_block {
	QUEUE	wait_queue;	/* Memory pool wait queue */
	ID	mplid;		/* Variable size memory pool ID */
	VP	exinf;		/* Extended information */
	ATR	mplatr;		/* Memory pool attribute */
	INT	mplsz;		/* Whole memory pool size */
	QUEUE	areaque;	/* Queue connecting all blocks */
	QUEUE	freeque;	/* Queue connecting free blocks */
	/* For SMP T-Kernel */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
} MPLCB;

LOCAL MPLCB	*mplcb_table;	/* Variable size memory pool control block */
LOCAL QUEUE	free_mplcb;	/* FreeQue */

#define get_mplcb(id)	( &mplcb_table[INDEX_MPL(id)] )


/*
 * Initialization of variable size memory pool control block
 */
EXPORT ER memorypool_initialize( void )
{
	MPLCB	*mplcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXMPLID, &max_mplid, 1);
	if ( (n < 1) || (NUM_MPLID < 1) ) {
		return E_SYS;
	}

	/* Create variable size memory pool control block */
	mplcb_table = Imalloc((UINT)NUM_MPLID * sizeof(MPLCB));
	if ( mplcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FeeQue */
	QueInit(&free_mplcb);
	end = mplcb_table + NUM_MPLID;
	for ( mplcb = mplcb_table; mplcb < end; mplcb++ ) {
		mplcb->mplid = 0;
		QueInsert(&mplcb->wait_queue, &free_mplcb);
	}

	return E_OK;
}

/* ------------------------------------------------------------------------ */

/*
 * Minimum unit of subdivision
 *	The lower 3 bits of the address is always 0
 *	because memory is allocated by ROUNDSZ.
 *	AreaQue uses the lowest bits for flag.
 */
#define ROUNDSZ		( sizeof(QUEUE) )	/* 8 bytes */

/*
 * Minimum fragment size (the smallest size to store FreeQue)
 */
#define MIN_FRAGMENT	( sizeof(QUEUE) * 2 )

/*
 * Adjusting the size which can be allocated 
 */
Inline INT roundSize( INT sz )
{
	if ( sz < (INT)MIN_FRAGMENT ) {
		sz = (INT)MIN_FRAGMENT;
	}
	return (INT)(((UINT)sz + (ROUNDSZ-1)) & ~(ROUNDSZ-1));
}

/*
 * Flag that uses the lower bits of AreaQue's 'prev'.
 */
#define AREA_USE	0x00000001U	/* In-use */
#define AREA_MASK	0x00000001U

#define setAreaFlag(q, f)   ( (q)->prev = (QUEUE*)((UINT)(q)->prev |  (UINT)(f)) )
#define clrAreaFlag(q, f)   ( (q)->prev = (QUEUE*)((UINT)(q)->prev & ~(UINT)(f)) )
#define chkAreaFlag(q, f)   ( ((UINT)(q)->prev & (UINT)(f)) != 0 )

#define Mask(x)		( (QUEUE*)((UINT)(x) & ~AREA_MASK) )
#define Assign(x, y)	( (x) = (QUEUE*)(((UINT)(x) & AREA_MASK) | (UINT)(y)) )

/*
 * Area size
 */
#define AreaSize(aq)	( (VB*)(aq)->next - (VB*)((aq) + 1) )
#define FreeSize(fq)	( (VB*)((fq) - 1)->next - (VB*)(fq) )

/*
 * Maximum free area size
 */
Inline INT MaxFreeSize( MPLCB *mplcb )
{
	if ( isQueEmpty(&mplcb->freeque) ) {
		return 0;
	}
	return FreeSize(mplcb->freeque.prev);
}

/*
 * FreeQue search
 *	Search the free area whose size is equal to 'blksz',
 *	or larger than
 *      'blksz' but closest.
 *	If it does not exist, return '&mplcb->freeque'.
 */
LOCAL QUEUE* searchFreeArea( MPLCB *mplcb, INT blksz )
{
	QUEUE	*q = &mplcb->freeque;

	/* For area whose memory pool size is less than 1/4,
	   search from smaller size.
	   Otherwise, search from larger size. */
	if ( blksz > (mplcb->mplsz / 4) ) {
		/* Search from larger size. */
		INT fsz = 0;
		while ( (q = q->prev) != &mplcb->freeque ) {
			fsz = FreeSize(q);
			if ( fsz <= blksz ) {
				return ( fsz < blksz )? q->next: q;
			}
		}
		return ( fsz >= blksz )? q->next: q;
	} else {
		/* Search from smaller size. */
		while ( (q = q->next) != &mplcb->freeque ) {
			if ( FreeSize(q) >= blksz ) {
				break;
			}
		}
		return q;
	}
}

/*
 * Registration of free area on FreeQue
 *	FreeQue is composed of 2 types: Queue that links the
 *	different size of areas by size and queue that links the
 *	same size of areas.
 *
 *	freeque
 *	|
 *	|   +-----------------------+	    +-----------------------+
 *	|   | AreaQue		    |	    | AreaQue		    |
 *	|   +-----------------------+	    +-----------------------+
 *	*---> FreeQue Size order    |  *----> FreeQue Same size   ----->
 *	|   | FreeQue Same size   -----*    | EmptyQue		    |
 *	|   |			    |	    |			    |
 *	|   |			    |	    |			    |
 *	|   +-----------------------+	    +-----------------------+
 *	|   | AreaQue		    |	    | AreaQue		    |
 *	v   +-----------------------+	    +-----------------------+
 */
LOCAL void appendFreeArea( MPLCB *mplcb, QUEUE *aq )
{
	QUEUE	*fq;
	INT	size = AreaSize(aq);

	/* Registration position search */
	/*  Search the free area whose size is equal to 'blksz',
	 *  or larger than 'blksz' but closest.
	 *  If it does not exist, return '&mplcb->freeque'.
	 */
	fq = searchFreeArea(mplcb, size);

	/* Register */
	clrAreaFlag(aq, AREA_USE);
	if ( (fq != &mplcb->freeque) && (FreeSize(fq) == size) ) {
		QueInsert(aq + 1, fq + 1);
	} else {
		QueInsert(aq + 1, fq);
	}
	QueInit(aq + 2);
}

/*
 * Delete from FreeQue
 */
LOCAL void removeFreeQue( QUEUE *fq )
{
	if ( !isQueEmpty(fq + 1) ) {
		QUEUE *nq = (fq + 1)->next;

		QueRemove(fq + 1);
		QueInsert(nq + 1, nq);
		QueRemove(nq);
		QueInsert(nq, fq);
	}

	QueRemove(fq);
}

/*
 * Register area
 *	Insert 'ent' just after 'que.'
 */
LOCAL void insertAreaQue( QUEUE *que, QUEUE *ent )
{
	ent->prev = que;
	ent->next = que->next;
	Assign(que->next->prev, ent);
	que->next = ent;
}

/*
 * Delete area
 */
LOCAL void removeAreaQue( QUEUE *aq )
{
	Mask(aq->prev)->next = aq->next;
	Assign(aq->next->prev, Mask(aq->prev));
}

/*
 * Get memory block 
 *	'blksz' must be larger than minimum fragment size
 *	and adjusted by ROUNDSZ unit.
 */
LOCAL VP get_blk( MPLCB *mplcb, INT blksz )
{
	QUEUE	*q, *aq;

	/* Search FreeQue */
	q = searchFreeArea(mplcb, blksz);
	if ( q == &mplcb->freeque ) {
		return NULL;
	}

	/* remove free area from FreeQue */
	removeFreeQue(q);
	aq = q - 1;

	/* If there is a fragment smaller than the minimum fragment size,
	   allocate them together */
	if ( (AreaSize(aq) - (UINT)blksz) >= (MIN_FRAGMENT + sizeof(QUEUE)) ) {

		/* Divide the area into 2. */
		q = (QUEUE*)((VB*)(aq + 1) + blksz);
		insertAreaQue(aq, q);

		/* Register the remaining area onto FreeQue */
		appendFreeArea(mplcb, q);
	}
	setAreaFlag(aq, AREA_USE);

	return (VP)(aq + 1);
}

/*
 * Free memory block 
 */
LOCAL ER rel_blk( MPLCB *mplcb, VP blk )
{
	QUEUE	*aq;

	aq = (QUEUE*)blk - 1;

#if CHK_PAR
	if ( !chkAreaFlag(aq, AREA_USE) ) {
		return E_PAR;
	}
#endif
	clrAreaFlag(aq, AREA_USE);

	if ( !chkAreaFlag(aq->next, AREA_USE) ) {
		/* Merge to the next area */
		removeFreeQue(aq->next + 1);
		removeAreaQue(aq->next);
	}
	if ( !chkAreaFlag(aq->prev, AREA_USE) ) {
		/* Merge to the previous area */
		aq = aq->prev;
		removeFreeQue(aq + 1);
		removeAreaQue(aq->next);
	}

	/* Register free area onto FreeQue */
	appendFreeArea(mplcb, aq);

	return E_OK;
}

/*
 * Memory pool initial setting
 */
LOCAL void init_mempool( MPLCB *mplcb, VP mempool, INT mempsz )
{
	QUEUE	*tp, *ep;

	QueInit(&mplcb->areaque);
	QueInit(&mplcb->freeque);

	/* Register onto AreaQue */
	tp = (QUEUE*)mempool;
	ep = (QUEUE*)((VB*)mempool + mempsz) - 1;
	insertAreaQue(&mplcb->areaque, ep);
	insertAreaQue(&mplcb->areaque, tp);

	/* Set AREA_USE for locations that must not be free area */
	setAreaFlag(&mplcb->areaque, AREA_USE);
	setAreaFlag(ep, AREA_USE);

	/* Register onto FreeQue */
	appendFreeArea(mplcb, tp);
}

/* ------------------------------------------------------------------------ */

/*
 * Allocate memory and release wait task,
 * as long as there are enough free memory.
 */
LOCAL void mpl_wakeup( MPLCB *mplcb )
{
	TCB	*top;
	VP	blk;
	INT	blksz;

	while ( !isQueEmpty(&mplcb->wait_queue) ) {
		top = (TCB*)mplcb->wait_queue.next;
		blksz = top->winfo.mpl.blksz;

		/* Check free space */
		if ( blksz > MaxFreeSize(mplcb) ) {
			break;
		}

		/* Get memory block */
		blk = get_blk(mplcb, blksz);
		*top->winfo.mpl.p_blk = blk;

		/* Release wait task */
		wait_release_ok(top);
	}
}

/*
 * Processing if the priority of wait task changes
 *	Since you need to execute with interrupt disable,
 *	you cannot use it for the non-resident memory.
 */
LOCAL void mpl_chg_pri( TCB *tcb, INT oldpri )
{
	MPLCB	*mplcb;

	mplcb = get_mplcb(tcb->wid);
	if ( oldpri >= 0 ) {
		/* Reorder wait line */
		gcb_change_priority((GCB*)mplcb, tcb);
	}

	/* From the new top task of a wait queue, free the assign
	   wait of memory blocks as much as possible. */
	mpl_wakeup(mplcb);
}

/*
 * Processing if the wait task is freed
 */
LOCAL void mpl_rel_wai( TCB *tcb )
{
	mpl_chg_pri(tcb, -1);
}

/*
 * Definition of variable size memory pool wait specification
 */
LOCAL WSPEC wspec_mpl_tfifo = { TTW_MPL, NULL,        mpl_rel_wai };
LOCAL WSPEC wspec_mpl_tpri  = { TTW_MPL, mpl_chg_pri, mpl_rel_wai };


/*
 * Create variable size memory pool 
 */
SYSCALL ID _tk_cre_mpl( T_CMPL *pk_cmpl )
{
#if CHK_RSATR
	const ATR VALID_MPLATR = {
		 TA_TPRI
		|TA_RNG3
		|TA_NODISWAI
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	MPLCB	*mplcb;
	ID	mplid;
	INT	mplsz;
	VP	mempool;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_cmpl->mplatr, VALID_MPLATR);
	CHECK_PAR(pk_cmpl->mplsz > 0);
	CHECK_DISPATCH();
	CHECK_PATR(pk_cmpl->mplatr);
#if CHK_ID
	if ( (pk_cmpl->mplatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_cmpl->domid);
	}
#endif

	mplsz = roundSize(pk_cmpl->mplsz);

	/* Allocate memory for memory pool */
	mempool = IAmalloc((UINT)mplsz + (sizeof(QUEUE)*2), pk_cmpl->mplatr);
	if ( mempool == NULL ) {
		return E_NOMEM;
	}

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_cmpl->mplatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_cmpl->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_cmpl->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_cmpl->mplatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_MPL,
						   mplcb_table->oname,
						   sizeof(MPLCB),
						   domid,
						   pk_cmpl->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_cmpl->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_cmpl->mplatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	mplcb = (MPLCB*)QueRemoveNext(&free_mplcb);
	if ( mplcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mplid = MP_GEN_DOMIDPART(domid) | ID_MPL(mplcb - mplcb_table);

		/* Initialize control block */
		QueInit(&mplcb->wait_queue);
		mplcb->mplid  = mplid;
		mplcb->exinf  = pk_cmpl->exinf;
		mplcb->mplatr = pk_cmpl->mplatr;
		mplcb->mplsz  = mplsz;

		/* Initialize memory pool */
		init_mempool(mplcb, mempool, mplsz + ((INT)sizeof(QUEUE)*2));

		/* Set object name */
		strncpy((char*)mplcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(domid,
					     MP_OBJECT_TYPE_MPL,
					     INDEX_MPL(mplid),
					     oname);

		ercd = mplid;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	if ( ercd < E_OK ) {
		IAfree(mempool, pk_cmpl->mplatr);
	}

	return ercd;
}

/*
 * Delete variable size memory pool 
 */
SYSCALL ER _tk_del_mpl( ID mplid )
{
	MPLCB	*mplcb;
	VP	mempool = NULL;
	ATR	memattr = 0;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mplid, mplcb->mplid, mplcb->mplatr);
		if ( ercd == E_OK ) {
			mempool = mplcb->areaque.next;
			memattr = mplcb->mplatr;

			/* Free wait state of task (E_DLT) */
			wait_delete(&mplcb->wait_queue);

			/* Unregistration */
			mp_unregister_object_in_domain(mplid,
						       MP_OBJECT_TYPE_MPL,
						       INDEX_MPL(mplid));

			/* Return to FreeQue */
			QueInsert(&mplcb->wait_queue, &free_mplcb);
			mplcb->mplid = 0;
		}
	}
	END_CRITICAL_SECTION;

	if ( ercd == E_OK ) {
		IAfree(mempool, memattr);
	}

	return ercd;
}

/*
 * Get variable size memory block 
 */
SYSCALL ER _tk_get_mpl( ID mplid, INT blksz, VP* p_blk, TMO tmout )
{
	MPLCB	*mplcb;
	VP	blk = NULL;
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_MPLID(mplid);
	CHECK_PAR(blksz > 0);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);
	blksz = roundSize(blksz);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(mplid, mplcb->mplid, mplcb->mplatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

#if CHK_PAR
	if ( blksz > mplcb->mplsz ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Check wait disable */
	if ( is_diswai((GCB*)mplcb, ctxtsk[prid], TTW_MPL) ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	if ( (gcb_top_of_wait_queue((GCB*)mplcb, ctxtsk[prid]) == ctxtsk[prid])
	  && ((blk = get_blk(mplcb, blksz)) != NULL) ) {
		/* Get memory block */
		*p_blk = blk;
	} else {
		/* Ready for wait */
		ctxtsk[prid]->wspec = ( (mplcb->mplatr & TA_TPRI) != 0 )?
					&wspec_mpl_tpri: &wspec_mpl_tfifo;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.mpl.blksz = blksz;
		ctxtsk[prid]->winfo.mpl.p_blk = p_blk;
		gcb_make_wait_with_diswai((GCB*)mplcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Return variable size memory block 
 */
SYSCALL ER _tk_rel_mpl( ID mplid, VP blk )
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(mplid, mplcb->mplid, mplcb->mplatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}
#if CHK_PAR
	if ( ((B*)blk < (B*)mplcb->areaque.next) || ((B*)blk > (B*)mplcb->areaque.prev) ) {
		ercd = E_PAR;
		goto error_exit;
	}
#endif

	/* Free memory block */
	ercd = rel_blk(mplcb, blk);
	if ( ercd < E_OK ) {
		goto error_exit;
	}

	/* Assign memory block to waiting task */
	mpl_wakeup(mplcb);

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Refer variable size memory pool state
 */
SYSCALL ER _tk_ref_mpl( ID mplid, T_RMPL *pk_rmpl )
{
	MPLCB	*mplcb;
	QUEUE	*fq, *q;
	INT	frsz, blksz;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);
	CHECK_DISPATCH();

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mplid, mplcb->mplid, mplcb->mplatr);
		if ( ercd == E_OK ) {
			pk_rmpl->exinf = mplcb->exinf;
			pk_rmpl->wtsk  = wait_tskid(&mplcb->wait_queue);
			frsz = 0;
			for ( fq = mplcb->freeque.next; fq != &mplcb->freeque; fq = fq->next ) {
				blksz = FreeSize(fq);
				frsz += blksz;
				for ( q = (fq+1)->next; q != (fq+1); q = q->next ) {
					frsz += blksz;
				}
			}
			pk_rmpl->frsz  = frsz;
			pk_rmpl->maxsz = MaxFreeSize(mplcb);
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
 * Find variable size memory pool ID
 */
SYSCALL ER _tk_fnd_mpl( ID domid, UB *oname )
{
	ER	ercd;
	ID	mplid = 0;
	MPLCB	*mplcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_MPL, (UB*)mplcb_table, mplcb_table->oname,
			       (UB*)&mplcb_table->mplid, sizeof(MPLCB), domid, oname);
	if ( 0 < ercd ) {
		mplid = (ID)ercd;
		mplcb = get_mplcb(mplid);
		ercd = mp_check_protection_fnd(mplid, mplcb->mplatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: mplid;
}

/*
 * Get domain information on variable size memory pool
 */
SYSCALL ER _tk_dmi_mpl( ID mplid, T_DMI *pk_dmi )
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mplid, mplcb->mplid, mplcb->mplatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(mplcb->mplatr);
			pk_dmi->domid = mp_get_domid_from_id(mplcb->mplid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, mplcb->oname, OBJECT_NAME_LENGTH);
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
EXPORT ER memorypool_getname(ID id, UB **name)
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(id);

	mplcb = get_mplcb(id);
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, mplcb->mplid) ) {
		ercd = E_NOEXS;
	} else if ( (mplcb->mplatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = mplcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER memorypool_setname(ID id, UB *name)
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(id);

	mplcb = get_mplcb(id);
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, mplcb->mplid) ) {
		ercd = E_NOEXS;
	} else if ( (mplcb->mplatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_MPL, mplcb_table->oname, sizeof(MPLCB), id, name, INDEX_MPL(id));
	}

	return ercd;
}

/*
 * Refer variable size memory pool usage state
 */
SYSCALL INT _td_lst_mpl( ID list[], INT nent )
{
	MPLCB	*mplcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = mplcb_table + NUM_MPLID;
	for ( mplcb = mplcb_table; mplcb < end; mplcb++ ) {
		if ( mplcb->mplid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = mplcb->mplid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer variable size memory pool state
 */
SYSCALL ER _td_ref_mpl( ID mplid, TD_RMPL *pk_rmpl )
{
	MPLCB	*mplcb;
	QUEUE	*fq, *q;
	INT	frsz, blksz;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mplid, mplcb->mplid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rmpl->exinf = mplcb->exinf;
		pk_rmpl->wtsk  = wait_tskid(&mplcb->wait_queue);
		frsz = 0;
		for ( fq = mplcb->freeque.next; fq != &mplcb->freeque; fq = fq->next ) {
			blksz = FreeSize(fq);
			frsz += blksz;
			for ( q = (fq+1)->next; q != (fq+1); q = q->next ) {
				frsz += blksz;
			}
		}
		pk_rmpl->frsz  = frsz;
		pk_rmpl->maxsz = MaxFreeSize(mplcb);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer variable size memory pool wait queue 
 */
SYSCALL INT _td_mpl_que( ID mplid, ID list[], INT nent )
{
	MPLCB	*mplcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mplid, mplcb->mplid) ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mplcb->wait_queue.next; q != &mplcb->wait_queue; q = q->next ) {
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
 * Find variable size memory pool ID
 */
SYSCALL ER _td_fnd_mpl( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_MPL, (UB*)mplcb_table, mplcb_table->oname,
			       (UB*)&mplcb_table->mplid, sizeof(MPLCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on variable size memory pool
 */
SYSCALL ER _td_dmi_mpl( ID mplid, TD_DMI *pk_dmi )
{
	MPLCB	*mplcb;
	ER	ercd = E_OK;

	CHECK_MPLID(mplid);

	mplcb = get_mplcb(mplid);

	BEGIN_CRITICAL_SECTION;
	if ( mplcb->mplid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mplid, mplcb->mplid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(mplcb->mplatr);
		pk_dmi->domid  = mp_get_domid_from_id(mplcb->mplid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, mplcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_MPLID */
