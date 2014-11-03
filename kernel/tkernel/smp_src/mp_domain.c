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
 *	mp_domain.c (T-Kernel/OS)
 *	Domain Control
 */

#include "kernel.h"
#include "task.h"
#include "check.h"
#include <sys/rominfo.h>
#include "mp_common.h"
#include "mp_domain.h"

EXPORT ID	max_domid;		/* Maximum Domain ID */

/*
 * Domain control block
 */
typedef struct domain_control_block {
	QUEUE		freeque;			/* Queue connecting free blocks */
	ID		domid;				/* Domain ID */
	VP 		exinf;				/* Extended information */
	ATR		domatr;				/* Domain attribute */
	UB		oname[OBJECT_NAME_LENGTH];	/* Object name */
	UINT		objcnt;				/* Object count */
} DOMCB;

LOCAL DOMCB	*domcb_table;		/* Domain control block */
LOCAL QUEUE	free_domcb;		/* FreeQue */

#define get_domcb(domid)	( &domcb_table[INDEX_DOM(domid)] )

/*
 * [31] ENABLE
 *
 * [23:16] domain ID
 * [15:0] hash value
 */
#define HASH_ENABLE_BIT			(0x80000000)
#define MAKE_HASH_KEY(domid, hash)	(HASH_ENABLE_BIT | MP_GEN_DOMIDPART(domid) | (hash))

typedef struct hash_control_block {
	INT	size;
	UW	*key;
} HASHCB;

LOCAL HASHCB hashcb_table[MP_HASHCB_TABLE_SIZE];

#define get_hashcb(type)	( &hashcb_table[(type)] )

/* Default object name */
EXPORT UB default_object_name[] = "";

/* The lowest protection level that protection is effective */
LOCAL INT oaplimit;

/* Check 'oaplimit' */
#define is_oap_invalid(plevel)	((plevel) <= oaplimit)

/*
 * Calculate hash
 */
LOCAL UH calc_hash( UB *oname, UH *res )
{
	UH	hash;
	INT	i;
	UB	c;

	hash = 0;
	for ( i = 0; i < OBJECT_NAME_LENGTH; i++ ) {
		c = oname[i];
		if ( c == '\0' ) {
			break;
		}
		hash = hash * 17 + c;
	}

	*res = hash;

	return i;
}

/*
 * Search an object name of the hashcb 
 */
LOCAL INT search_hashcb( INT type, UB *onametop, INT cbsz, ID domid, UB *oname )
{
	UH	hash;
	HASHCB	*hashcb;
	UW	key;
	INT	i;
	UB	*pos;

	if ( calc_hash(oname, &hash) != 0 ) {
		hashcb = get_hashcb(type);
		key = MAKE_HASH_KEY(domid, hash);
		for ( i = 0; i < hashcb->size; i++ ) {
			if ( hashcb->key[i] == key ) {
				pos = onametop + cbsz * i;
				if ( strncmp((char*)pos, (char*)oname, OBJECT_NAME_LENGTH) == 0 ) {
					/* Found */
					return i;
				}
			}
		}
	}

	/* Not found */
	return -1;
}

/*
 * Check exception condition
 */
LOCAL BOOL check_exception_condition( ATR atr )
{
	if ( MP_GET_PATR(atr) == TA_PUBLIC ) {
		/* If it is PUBLIC, it is OK. */
		return TRUE;
	} else if ( in_indp() ) {
		/* The protection attribute is not checked
		   when called from the task independent part. */
		return TRUE;
	} else if ( is_oap_invalid(mp_get_calinf_plevel()) ) {
		/* Examine whether the protection level of the task is the level
		   that access protection becomes invalid */
		return TRUE;
	} else {
		/* Did not correspond */
		return FALSE;
	}
}

/*
 * Trace domain hierarchy for protection check
 */
LOCAL BOOL trace_domain_for_pchk( ID domid, ATR atr, ID tskid )
{
	ATR	patr;
	ID	rootid, tskdomid;

	patr = MP_GET_PATR(atr);
	rootid = MP_KDMID_SELF;
	tskdomid = tskid;
	do {
		tskdomid = mp_get_domid_from_id(tskdomid);
		if ( tskdomid == domid ) {
			return TRUE;
		}
	} while ( (patr == TA_PROTECTED) && (tskdomid != rootid) );
	/* Violation of protection attribute */
	return FALSE;
}

/*
 * Existence check of domain
 */
EXPORT ER mp_existence_check_of_domain( ID domid )
{
	DOMCB	*domcb;

	/* Check object ID */
	domcb = get_domcb(domid);
	if ( domcb->domid == 0 ) {
		return E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		return E_NOEXS;
	} else {
		return E_OK;
	}
}

/*
 * Protection attribute check at object creation
 */
EXPORT ER mp_check_protection_cre( ID domid, ATR atr )
{
	ID	tskid;

	if ( check_exception_condition(atr) ) {
		return E_OK;
	} else {
		domid = SET_PRID(domid);
		tskid = mp_get_calinf_tskid();
		if ( trace_domain_for_pchk(domid, atr, tskid) ) {
			return E_OK;
		} else {
			/* Violation of protection attribute */
			return E_PAR;
		}
	}
}

/*
 * Protection attribute check for foward rendezvous
 */
EXPORT ER mp_check_protection_fwd( ID objid, ATR objatr, TCB *tcb )
{
	ID	domid, tskid;

	if ( MP_GET_PATR(objatr) == TA_PUBLIC ) {
		/* If it is PUBLIC, it is OK. */
		return E_OK;
	} else if ( is_oap_invalid(mp_get_plevel(tcb)) ) {
		/* Examine whether the protection level of the task is the level
		   that access protection becomes invalid */
		return E_OK;
	} else {
		domid = mp_get_domid_from_id(objid);
		tskid = tcb->tskid;
		if ( trace_domain_for_pchk(domid, objatr, tskid) ) {
			/* OK */
			return E_OK;
		} else {
			/* Violation of protection attribute */
			return E_DACV;
		}
	}
}

/*
 * Protection attribute check for find object
 */
EXPORT ER mp_check_protection_fnd( ID objid, ATR objatr )
{
	ID	domid, tskid;

	if ( check_exception_condition(objatr) ) {
		return E_OK;
	} else {
		domid = mp_get_domid_from_id(objid);
		tskid = mp_get_calinf_tskid();
		if ( trace_domain_for_pchk(domid, objatr, tskid) ) {
			return E_OK;
		} else {
			/* Violation of protection attribute */
			return E_NOEXS;
		}
	}
}

/*
 * Check domain and protection
 */
EXPORT ER mp_check_domain_and_protection( ID argid, ID objid, ATR objatr )
{
	ID	domid, tskid;

	/* Check domain ID part */
	if ( !MP_CMP_DOMIDPART(argid, objid) ) {
		/* Invalid ID */
		return E_NOEXS;
	} else if ( check_exception_condition(objatr) ) {
		return E_OK;
	} else {
		domid = mp_get_domid_from_id(objid);
		tskid = mp_get_calinf_tskid();
		if ( trace_domain_for_pchk(domid, objatr, tskid) ) {
			return E_OK;
		} else {
			/* Violation of protection attribute */
			return E_DACV;
		}
	}
}

/*
 * Check domain and protection for task
 */
EXPORT ER mp_check_domain_and_protection_tsk( ID argid, ID objid, ATR objatr )
{
	/* self doesn't check */
	if ( argid == TSK_SELF ) {
		return E_OK;
	} else {
		return mp_check_domain_and_protection(argid, objid, objatr);
	}
}

/*
 * Existence check of oname
 */
EXPORT ER mp_existence_check_of_oname( INT type, UB *onametop, INT cbsz, ID domid, UB *oname )
{
	if ( search_hashcb(type, onametop, cbsz, domid, oname) < 0 ) {
		/* Not found */
		return E_OK;
	} else {
		/* Found */
		return E_ONAME;
	}
}

/*
 * Change oname
 */
EXPORT ER mp_change_oname( INT type, UB *onametop, INT cbsz, ID objid, UB *oname, INT idx )
{
	HASHCB	*hashcb;
	ID	domid;
	UH	hash;
	UW	key;
	INT	i;
	UB	*pos;

	hashcb = get_hashcb(type);
	if ( calc_hash(oname, &hash) != 0 ) {
		domid = mp_get_domid_from_id(objid);
		key = MAKE_HASH_KEY(domid, hash);
		for ( i = 0; i < hashcb->size; i++ ) {
			if ( hashcb->key[i] == key ) {
				pos = onametop + cbsz * i;
				if ( strncmp((char*)pos, (char*)oname, OBJECT_NAME_LENGTH) == 0 ) {
					/* Found */
					return E_ONAME;
				}
			}
		}
	} else {
		key = 0;
	}

	/* Change */
	pos = onametop + cbsz * idx;
	strncpy((char*)pos, (char*)oname, OBJECT_NAME_LENGTH);
	hashcb->key[idx] = key;

	return E_OK;
}

/*
 * Search oname
 */
EXPORT ER mp_search_oname( INT type, UB *cbtop, UB *onametop, UB *idtop,
			   INT cbsz, ID domid, UB *oname )
{
	ER	ercd;
	INT	idx;
	UB	*pos;

	/* Check domain ID */
	ercd = mp_existence_check_of_domain(domid);
	if ( ercd == E_OK ) {
		idx = search_hashcb(type, onametop, cbsz, domid, oname);
		if ( idx < 0 ) {
			/* Not found */
			ercd = E_NOEXS;
		} else {
			/* Found */
			pos = idtop + cbsz * idx;
			ercd = *((ID*)(pos));
		}
	}

	return ercd;
}

/*
 * Register object in the domain
 */
EXPORT void mp_register_object_in_domain( ID domid, INT type, INT objidx, UB *oname )
{
	UH	hash;
	UW	key;
	DOMCB	*domcb;

	/* Set key */
	if ( calc_hash(oname, &hash) == 0 ) {
		key = 0;
	} else {
		key = MAKE_HASH_KEY(domid, hash);
	}
	get_hashcb(type)->key[objidx] = key;

	/* Increment object count */
	domcb = get_domcb(domid);
	domcb->objcnt++;
}

/*
 * Unregister object in the domain
 */
EXPORT void mp_unregister_object_in_domain( ID objid, INT type, INT objidx )
{
	ID	domid;
	DOMCB	*domcb;

	/* Clear key */
	get_hashcb(type)->key[objidx] = 0;

	/* Decrement object count */
	domid = mp_get_domid_from_id(objid);
	domcb = get_domcb(domid);
	domcb->objcnt--;
}

/*
 * Get the ID of the domain that an object belongs to
 */
EXPORT ID mp_get_domid_from_id( ID id )
{
	ID	domid;
	DOMCB	*domcb;

	domid = (id & DOMID_MASK) >> DOMID_SHIFT;
	domcb = get_domcb(domid);

	return domcb->domid;
}

/*
 * Initialization of domian control block 
 */
EXPORT ER mp_domain_initialize( void )
{
	DOMCB	*domcb, *end;
	W	n;
	INT	i;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXDOMID, &max_domid, 1);
	if ( n < 1 || NUM_DOMID < 1 ) {
		return E_SYS;
	}
	n = _tk_get_cfn(SCTAG_TOAPLIMIT, &oaplimit, 1);
	if ( n < 1 || oaplimit < 0 || oaplimit > 3 ) {
		return E_SYS;
	}

	/* Domain control block */
	domcb_table = Imalloc((UINT)NUM_DOMID * sizeof(DOMCB));
	if ( domcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Initialize processor domain */
	domcb = domcb_table;
	QueInit(&(domcb->freeque));
	domcb->domid = SET_PRID(MP_KDMID);
	domcb->exinf = 0;
	domcb->domatr = TA_PUBLIC;
	memset(domcb->oname, 0, OBJECT_NAME_LENGTH);
	domcb->objcnt = 0;

	/* Register domain control blocks onto FreeQue */
	QueInit(&free_domcb);
	domcb++;
	end = domcb_table + NUM_DOMID;
	for ( ; domcb < end; domcb++ ) {
		domcb->domid = 0;
		QueInsert(&domcb->freeque, &free_domcb);
	}

	/* Initialize hashcb_table */
	get_hashcb(MP_OBJECT_TYPE_TSK)->size = NUM_TSKID;
#ifdef NUM_SEMID
	get_hashcb(MP_OBJECT_TYPE_SEM)->size = NUM_SEMID;
#endif
#ifdef NUM_FLGID
	get_hashcb(MP_OBJECT_TYPE_FLG)->size = NUM_FLGID;
#endif
#ifdef NUM_MBXID
	get_hashcb(MP_OBJECT_TYPE_MBX)->size = NUM_MBXID;
#endif
#ifdef NUM_MTXID
	get_hashcb(MP_OBJECT_TYPE_MTX)->size = NUM_MTXID;
#endif
#ifdef NUM_MBFID
	get_hashcb(MP_OBJECT_TYPE_MBF)->size = NUM_MBFID;
#endif
#ifdef NUM_PORID
	get_hashcb(MP_OBJECT_TYPE_POR)->size = NUM_PORID;
#endif
#ifdef NUM_MPFID
	get_hashcb(MP_OBJECT_TYPE_MPF)->size = NUM_MPFID;
#endif
#ifdef NUM_MPLID
	get_hashcb(MP_OBJECT_TYPE_MPL)->size = NUM_MPLID;
#endif
#ifdef NUM_CYCID
	get_hashcb(MP_OBJECT_TYPE_CYC)->size = NUM_CYCID;
#endif
#ifdef NUM_ALMID
	get_hashcb(MP_OBJECT_TYPE_ALM)->size = NUM_ALMID;
#endif
	get_hashcb(MP_OBJECT_TYPE_DOM)->size = NUM_DOMID;

	for ( i = 0; i < MP_HASHCB_TABLE_SIZE; i++ ) {
		hashcb_table[i].key = Icalloc(hashcb_table[i].size, sizeof(UW));
		if ( hashcb_table[i].key == NULL ) {
			return E_NOMEM;
		}
	}

	return E_OK;
}

/*
 * Create domain
 */
SYSCALL ID _tk_cre_dom( T_CDOM *pk_cdom )
{
#if CHK_RSATR
	const ATR VALID_DOMATR = {
		 TA_DOMID
		|TA_ONAME
	};
#endif
	DOMCB	*domcb;
	ID	bdomid, domid;
	UB	*oname;
	ER	ercd = E_OK;

	CHECK_RSATR(pk_cdom->domatr, VALID_DOMATR);
#if CHK_ID
	if ( (pk_cdom->domatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_cdom->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_cdom->domatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_cdom->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		bdomid = pk_cdom->domid;
	} else {
		bdomid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_cdom->domatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_DOM,
						   domcb_table->oname,
						   sizeof(DOMCB),
						   bdomid,
						   pk_cdom->oname);
		if ( ercd != E_OK ){
			/* Already exists */
			goto error_exit;
		}
		oname = pk_cdom->oname;
	} else {
		oname = default_object_name;
	}

	/* Get control block from FreeQue */
	domcb = (DOMCB*)QueRemoveNext(&free_domcb);
	if ( domcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		domid = MP_GEN_DOMIDPART(bdomid) | ID_DOM(domcb - domcb_table);
		/* Initialize control block */
		domcb->domid = domid;
		domcb->exinf = pk_cdom->exinf;
		domcb->domatr = pk_cdom->domatr;

		/* Set object name */
		strncpy((char*)domcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(bdomid,
					     MP_OBJECT_TYPE_DOM,
					     INDEX_DOM(domid),
					     oname);

		domcb->objcnt = 0;

		ercd = domid;
	}

    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Delete domain
 */
SYSCALL ER _tk_del_dom( ID domid )
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(domid);

	/* Kernel domain cannot be deleted */
	if ( MP_IS_KDMID(domid) ) {
		return E_OBJ;
	}

	domcb = get_domcb(domid);

	BEGIN_CRITICAL_SECTION;
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		ercd = E_NOEXS;
	} else if ( domcb->objcnt != 0 ) {
		ercd = E_OBJ;
	} else {
		/* Unregistration */
		mp_unregister_object_in_domain(domid,
					       MP_OBJECT_TYPE_DOM,
					       INDEX_DOM(domid));

		/* Return to FreeQue */
		QueInsert(&domcb->freeque, &free_domcb);
		domcb->domid = 0;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Force delete domain
 */
SYSCALL ER _tk_fdel_dom( ID domid )
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(domid);

	/* Kernel domain cannot be deleted */
	if ( MP_IS_KDMID(domid) ) {
		return E_OBJ;
	}

	domcb = get_domcb(domid);

	BEGIN_CRITICAL_SECTION;
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		ercd = E_NOEXS;
	} else {
		/* Unregistration */
		mp_unregister_object_in_domain(domid,
					       MP_OBJECT_TYPE_DOM,
					       INDEX_DOM(domid));

		/* Return to FreeQue */
		QueInsert(&domcb->freeque, &free_domcb);
		domcb->domid = 0;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer domain state
 */
SYSCALL ER _tk_ref_dom( ID domid, T_RDOM *pk_rdom )
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(domid);

	domcb = get_domcb(domid);

	BEGIN_CRITICAL_SECTION;
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rdom->exinf  = domcb->exinf;
		pk_rdom->domid  = mp_get_domid_from_id(domcb->domid);
		pk_rdom->objcnt = domcb->objcnt;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Find domain ID
 */
SYSCALL ID _tk_fnd_dom( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_DOM, (UB*)domcb_table, domcb_table->oname,
			       (UB*)&domcb_table->domid, sizeof(DOMCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on domain
 */
SYSCALL ER _tk_dmi_dom( ID domid, T_DMI *pk_dmi )
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(domid);

	domcb = get_domcb(domid);

	BEGIN_CRITICAL_SECTION;
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(domcb->domatr);
		pk_dmi->domid = mp_get_domid_from_id(domcb->domid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, domcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get kernel domain ID
 */
SYSCALL ID _tk_get_kdm( ID prcid )
{
	ID	kdmid;

	CHECK_PRCID(prcid);

	if ( prcid == PRC_SELF ) {
		kdmid = MP_KDMID_SELF;
	}
	else {
		kdmid = (ID)(MP_KDMID | (MP_KERNELID << MP_PRID_SHIFT));
	}

	return kdmid;
}


/* ------------------------------------------------------------------------ */
/*
 *	Debugger support function
 */
#if USE_DBGSPT

/*
 * Get object name from control block
 */
EXPORT ER domain_getname(ID id, UB **name)
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(id);

	domcb = get_domcb(id);
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, domcb->domid) ) {
		ercd = E_NOEXS;
	} else if ( (domcb->domatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = domcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER domain_setname(ID id, UB *name)
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(id);

	domcb = get_domcb(id);
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, domcb->domid) ) {
		ercd = E_NOEXS;
	} else if ( (domcb->domatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_DOM, domcb_table->oname, sizeof(DOMCB), id, name, INDEX_DOM(id));
	}

	return ercd;
}

/*
 * Refer domain usage state
 */
SYSCALL INT _td_lst_dom( ID list[], INT nent )
{
	DOMCB	*domcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = domcb_table + NUM_DOMID;
	for ( domcb = (domcb_table + 1); domcb < end; domcb++ ) {
		if ( domcb->domid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = domcb->domid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer domain state
 */
SYSCALL ER _td_ref_dom( ID domid, TD_RDOM *pk_rdom )
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(domid);

	domcb = get_domcb(domid);

	BEGIN_CRITICAL_SECTION;
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rdom->exinf  = domcb->exinf;
		pk_rdom->domid  = mp_get_domid_from_id(domcb->domid);
		pk_rdom->objcnt = domcb->objcnt;
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Find domain ID
 */
SYSCALL ID _td_fnd_dom( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_DOM, (UB*)domcb_table, domcb_table->oname,
			       (UB*)&domcb_table->domid, sizeof(DOMCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on domain
 */
SYSCALL ER _td_dmi_dom( ID domid, TD_DMI *pk_dmi )
{
	DOMCB	*domcb;
	ER	ercd = E_OK;

	CHECK_DOMID(domid);

	domcb = get_domcb(domid);

	BEGIN_CRITICAL_SECTION;
	if ( domcb->domid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(domcb->domid, domid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(domcb->domatr);
		pk_dmi->domid = mp_get_domid_from_id(domcb->domid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, domcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
