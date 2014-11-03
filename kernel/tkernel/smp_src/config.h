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
 *	config.h (T-Kernel/OS)
 *	System Configuration Definition
 */

#ifndef _CONFIG_
#define _CONFIG_

/* Macro to operate object ID */
#define GET_PRID(id)	(((id) & MP_PRID_MASK) >> MP_PRID_SHIFT)
#define SET_PRID(id)	((id) | (MP_SFT_PRID))
#define DOMID_MASK	(0x00ff0000U)
#define DOMID_SHIFT	(16)
#define GET_DOMID(id)	(((id) & DOMID_MASK) >> DOMID_SHIFT)
#define OBJID_MASK	(0x0000ffff)
#define GET_OBJID(id)	((id) & OBJID_MASK)

/* Task configuration */
#define MIN_TSKID	(1)
#define MAX_TSKID	(max_tskid)
#define NUM_TSKID	(MAX_TSKID)
#define CHK_TSKID(id)	((MIN_TSKID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_TSKID))
#define INDEX_TSK(id)	(GET_OBJID(id)-(MIN_TSKID))
#define ID_TSK(index)	SET_PRID((index)+(MIN_TSKID))

/* Semaphore configuration */
#define MIN_SEMID	(1)
#define MAX_SEMID	(max_semid)
#define NUM_SEMID	(MAX_SEMID)
#define CHK_SEMID(id)	((MIN_SEMID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_SEMID))
#define INDEX_SEM(id)	(GET_OBJID(id)-(MIN_SEMID))
#define ID_SEM(index)	SET_PRID((index)+(MIN_SEMID))

/* Mutex configuration */
#define MIN_MTXID	(1)
#define MAX_MTXID	(max_mtxid)
#define NUM_MTXID	(MAX_MTXID)
#define CHK_MTXID(id)	((MIN_MTXID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_MTXID))
#define INDEX_MTX(id)	(GET_OBJID(id)-(MIN_MTXID))
#define ID_MTX(index)	SET_PRID((index)+(MIN_MTXID))

/* Event flag configuration */
#define MIN_FLGID	(1)
#define MAX_FLGID	(max_flgid)
#define NUM_FLGID	(MAX_FLGID)
#define CHK_FLGID(id)	((MIN_FLGID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_FLGID))
#define INDEX_FLG(id)	(GET_OBJID(id)-(MIN_FLGID))
#define ID_FLG(index)	SET_PRID((index)+(MIN_FLGID))

/* Mailbox configuration */
#define MIN_MBXID	(1)
#define MAX_MBXID	(max_mbxid)
#define NUM_MBXID	(MAX_MBXID)
#define CHK_MBXID(id)	((MIN_MBXID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_MBXID))
#define INDEX_MBX(id)	(GET_OBJID(id)-(MIN_MBXID))
#define ID_MBX(index)	SET_PRID((index)+(MIN_MBXID))

/* Message buffer configuration */
#define MIN_MBFID	(1)
#define MAX_MBFID	(max_mbfid)
#define NUM_MBFID	(MAX_MBFID)
#define CHK_MBFID(id)	((MIN_MBFID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_MBFID))
#define INDEX_MBF(id)	(GET_OBJID(id)-(MIN_MBFID))
#define ID_MBF(index)	SET_PRID((index)+(MIN_MBFID))

/* Rendezvous configuration */
#define MIN_PORID	(1)
#define MAX_PORID	(max_porid)
#define NUM_PORID	(MAX_PORID)
#define CHK_PORID(id)	((MIN_PORID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_PORID))
#define INDEX_POR(id)	(GET_OBJID(id)-(MIN_PORID))
#define ID_POR(index)	SET_PRID((index)+(MIN_PORID))

/* Memory pool configuration */
#define MIN_MPLID	(1)
#define MAX_MPLID	(max_mplid)
#define NUM_MPLID	(MAX_MPLID)
#define CHK_MPLID(id)	((MIN_MPLID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_MPLID))
#define INDEX_MPL(id)	(GET_OBJID(id)-(MIN_MPLID))
#define ID_MPL(index)	SET_PRID((index)+(MIN_MPLID))

/* Fixed size memory pool configuration */
#define MIN_MPFID	(1)
#define MAX_MPFID	(max_mpfid)
#define NUM_MPFID	(MAX_MPFID)
#define CHK_MPFID(id)	((MIN_MPFID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_MPFID))
#define INDEX_MPF(id)	(GET_OBJID(id)-(MIN_MPFID))
#define ID_MPF(index)	SET_PRID((index)+(MIN_MPFID))

/* Cyclic handler configuration */
#define MIN_CYCID	(1)
#define MAX_CYCID	(max_cycid)
#define NUM_CYCID	(MAX_CYCID)
#define CHK_CYCID(id)	((MIN_CYCID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_CYCID))
#define INDEX_CYC(id)	(GET_OBJID(id)-(MIN_CYCID))
#define ID_CYC(index)	SET_PRID((index)+(MIN_CYCID))

/* Alarm handler configuration */
#define MIN_ALMID	(1)
#define MAX_ALMID	(max_almid)
#define NUM_ALMID	(MAX_ALMID)
#define CHK_ALMID(id)	((MIN_ALMID) <= GET_OBJID(id) && GET_OBJID(id) <= (MAX_ALMID))
#define INDEX_ALM(id)	(GET_OBJID(id)-(MIN_ALMID))
#define ID_ALM(index)	SET_PRID((index)+(MIN_ALMID))

/* Subsystem manager configuration */
#define MIN_SSYID	(1)
#define MAX_SSYID	(max_ssyid)
#define NUM_SSYID	(MAX_SSYID)
#define CHK_SSYID(id)	((MIN_SSYID <= (id)) && ((id) <= MAX_SSYID))
#define INDEX_SSY(id)	((id)-MIN_SSYID)
#define ID_SSY(index)	((index)+MIN_SSYID)

/* Resource group configuration */
#define MIN_RESID	(1)
#define MAX_RESID	(max_resid)
#define NUM_RESID	(MAX_RESID)
#define CHK_RESID(id)	((MIN_RESID <= (id)) && ((id) <= MAX_RESID))
#define INDEX_RES(id)	((id)-MIN_RESID)
#define ID_RES(index)	((index)+MIN_RESID)

/* Task priority configuration */
#define MIN_PRI		(1)	/* Minimum priority number = highest priority */
#define MAX_PRI		(140)	/* Maximum priority number = lowest priority */
#define NUM_PRI		(140)	/* Number of priority levels */
#define CHK_PRI(pri)	((MIN_PRI <= (pri)) && ((pri) <= MAX_PRI))

/* Subsystem manager configuration */
#define MIN_SSYPRI	(1)
#define MAX_SSYPRI	(max_ssypri)
#define NUM_SSYPRI	(MAX_SSYPRI)
#define CHK_SSYPRI(pri)	((MIN_SSYPRI <= (pri)) && ((pri) <= MAX_SSYPRI))

/* Domain configuration */
#define MP_KDMID	(0)	/* Kernel domain ID */
#define MIN_DOMID	(0)
#define MAX_DOMID	(max_domid)
#define NUM_DOMID	(MAX_DOMID+1)
#define CHK_DOMID(id)	(MIN_DOMID <= GET_OBJID(id) && GET_OBJID(id) <= MAX_DOMID)
#define INDEX_DOM(id)	(GET_OBJID(id)-MIN_DOMID)
#define ID_DOM(index)	SET_PRID((index)+MIN_DOMID)
#define CHK_ID_DOM(id)	(MIN_DOMID <= GET_DOMID(id) && GET_DOMID(id) <= MAX_DOMID)
#define CHK_ID_PRC(id)	((1) == GET_PRID(id))
#define MIN_PRCID	(0)	/* PRC_SELF is contained.  */
#define MAX_PRCID	(num_proc)
#define CHK_PRCID(id)	(MIN_PRCID <= (id) && (id) <= MAX_PRCID)

/*
 * Check parameter
 *   0: Do not check parameter
 *   1: Check parameter
 */
#define CHK_NOSPT	(1)	/* Check unsupported function (E_NOSPT) */
#define CHK_RSATR	(1)	/* Check reservation attribute error (E_RSATR) */
#define CHK_PAR		(1)	/* Check parameter (E_PAR) */
#define CHK_ID		(1)	/* Check object ID range (E_ID) */
#define CHK_OACV	(1)	/* Check Object Access Violation (E_OACV) */
#define CHK_CTX		(1)	/* Check whether task-independent part is running (E_CTX) */
#define CHK_CTX1	(1)	/* Check dispatch disable part */
#define CHK_CTX2	(1)	/* Check task independent part */
#define CHK_SELF	(1)	/* Check if its own task is specified (E_OBJ) */
#define CHK_NOCOP	(1)	/* Check non-existence co-processor error (E_NOCOP) */

/*
 * Debugger support function
 *   0: Invalid
 *   1: Valid
 */
#define USE_DBGSPT		(1)


/*
 * Output (error) messages from T-Kernel
 *   0: Do not output message
 *   1: Output message
 */
#define USE_KERNEL_MESSAGE	(1)

#endif /* _CONFIG_ */
