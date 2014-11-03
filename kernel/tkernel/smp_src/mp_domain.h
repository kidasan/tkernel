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
 *	mp_domain.h (T-Kernel/OS)
 *	Domain Definition
 */

#ifndef _MP_DOMAIN_
#define _MP_DOMAIN_

/* Object type */
typedef enum {
	MP_OBJECT_TYPE_TSK = 0,
#ifdef NUM_SEMID
	MP_OBJECT_TYPE_SEM,
#endif
#ifdef NUM_FLGID
	MP_OBJECT_TYPE_FLG,
#endif
#ifdef NUM_MBXID
	MP_OBJECT_TYPE_MBX,
#endif
#ifdef NUM_MTXID
	MP_OBJECT_TYPE_MTX,
#endif
#ifdef NUM_MBFID
	MP_OBJECT_TYPE_MBF,
#endif
#ifdef NUM_PORID
	MP_OBJECT_TYPE_POR,
#endif
#ifdef NUM_MPFID
	MP_OBJECT_TYPE_MPF,
#endif
#ifdef NUM_MPLID
	MP_OBJECT_TYPE_MPL,
#endif
#ifdef NUM_CYCID
	MP_OBJECT_TYPE_CYC,
#endif
#ifdef NUM_ALMID
	MP_OBJECT_TYPE_ALM,
#endif
	MP_OBJECT_TYPE_DOM,
	MP_HASHCB_TABLE_SIZE
} MP_OBJECT_TYPE;

/* Default object name */
IMPORT UB default_object_name[];

/* Kernel domain ID */
#define MP_KDMID_SELF		SET_PRID(MP_KDMID)
#define MP_IS_KDMID(domid)	((domid) == MP_KDMID_SELF)

/* Compare domain ID part of ID */
#define MP_CMP_DOMIDPART(id1, id2)	((((id1) ^ (id2)) & DOMID_MASK) == 0)

/* Generate domain ID part from domain ID */
#define MP_GEN_DOMIDPART(domid)		((GET_OBJID(domid) << DOMID_SHIFT) & DOMID_MASK)

/* Get domain related attribute */
#define MP_GET_DOMATR(atr)	((atr) & (TA_ONAME | TA_PRIVATE | TA_PROTECTED | TA_PUBLIC))

/* Get protection attribute */
#define MP_GET_PATR(atr)	((atr) & (TA_PRIVATE | TA_PROTECTED | TA_PUBLIC))

/* Domain function */
IMPORT ER mp_existence_check_of_domain( ID domid );
IMPORT ER mp_check_protection_cre( ID domid, ATR atr );
IMPORT ER mp_check_protection_fwd( ID objid, ATR objatr, TCB *tcb );
IMPORT ER mp_check_protection_fnd( ID objid, ATR objatr );
IMPORT ER mp_check_domain_and_protection( ID argid, ID objid, ATR objatr );
IMPORT ER mp_check_domain_and_protection_tsk( ID argid, ID objid, ATR objatr );
IMPORT ER mp_existence_check_of_oname( INT type, UB *onametop, INT cbsz, ID domid, UB *oname );
IMPORT ER mp_change_oname( INT type, UB *onametop, INT cbsz, ID objid, UB *oname, INT idx );
IMPORT ER mp_search_oname( INT type, UB *cbtop, UB *onametop, UB *idtop, INT cbsz, ID domid, UB *oname );
IMPORT void mp_register_object_in_domain( ID domid, INT type, INT objidx, UB *oname );
IMPORT void mp_unregister_object_in_domain( ID objid, INT type, INT objidx );
IMPORT ID mp_get_domid_from_id( ID id );

#endif /* _MP_DOMAIN_ */
