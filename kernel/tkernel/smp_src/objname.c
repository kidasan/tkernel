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
 *	objname.c (T-Kernel/DS)
 *	Object name support
 */

#include "kernel.h"
#include "task.h"
#include <libstr.h>

#if USE_DBGSPT

LOCAL ER object_getname( UINT objtype, ID objid, UB **name)
{
	ER	ercd;

	switch (objtype) {
#ifdef NUM_TSKID
	  case TN_TSK:
		{
			IMPORT ER task_getname(ID id, UB **name);
			ercd = task_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_SEMID
	  case TN_SEM:
		{
			IMPORT ER semaphore_getname(ID id, UB **name);
			ercd = semaphore_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_FLGID
	  case TN_FLG:
		{
			IMPORT ER eventflag_getname(ID id, UB **name);
			ercd = eventflag_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_MBXID
	  case TN_MBX:
		{
			IMPORT ER mailbox_getname(ID id, UB **name);
			ercd = mailbox_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_MBFID
	  case TN_MBF:
		{
			IMPORT ER messagebuffer_getname(ID id, UB **name);
			ercd = messagebuffer_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_PORID
	  case TN_POR:
		{
			IMPORT ER rendezvous_getname(ID id, UB **name);
			ercd = rendezvous_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_MTXID
	  case TN_MTX:
		{
			IMPORT ER mutex_getname(ID id, UB **name);
			ercd = mutex_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_MPLID
	  case TN_MPL:
		{
			IMPORT ER memorypool_getname(ID id, UB **name);
			ercd = memorypool_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_MPFID
	  case TN_MPF:
		{
			IMPORT ER fix_memorypool_getname(ID id, UB **name);
			ercd = fix_memorypool_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_CYCID
	  case TN_CYC:
		{
			IMPORT ER cyclichandler_getname(ID id, UB **name);
			ercd = cyclichandler_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_ALMID
	  case TN_ALM:
		{
			IMPORT ER alarmhandler_getname(ID id, UB **name);
			ercd = alarmhandler_getname(objid, name);
			break;
		}
#endif

#ifdef NUM_DOMID
	  case TN_DOM:
		{
			IMPORT ER domain_getname(ID id, UB **name);
			ercd = domain_getname(objid, name);
			break;
		}
#endif

	  default:
		ercd = E_PAR;

	}

	return ercd;
}

LOCAL ER object_setname( UINT objtype, ID objid, UB *name)
{
	ER	ercd;

	switch (objtype) {
#ifdef NUM_TSKID
	  case TN_TSK:
		{
			IMPORT ER task_setname(ID id, UB *name);
			ercd = task_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_SEMID
	  case TN_SEM:
		{
			IMPORT ER semaphore_setname(ID id, UB *name);
			ercd = semaphore_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_FLGID
	  case TN_FLG:
		{
			IMPORT ER eventflag_setname(ID id, UB *name);
			ercd = eventflag_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_MBXID
	  case TN_MBX:
		{
			IMPORT ER mailbox_setname(ID id, UB *name);
			ercd = mailbox_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_MBFID
	  case TN_MBF:
		{
			IMPORT ER messagebuffer_setname(ID id, UB *name);
			ercd = messagebuffer_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_PORID
	  case TN_POR:
		{
			IMPORT ER rendezvous_setname(ID id, UB *name);
			ercd = rendezvous_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_MTXID
	  case TN_MTX:
		{
			IMPORT ER mutex_setname(ID id, UB *name);
			ercd = mutex_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_MPLID
	  case TN_MPL:
		{
			IMPORT ER memorypool_setname(ID id, UB *name);
			ercd = memorypool_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_MPFID
	  case TN_MPF:
		{
			IMPORT ER fix_memorypool_setname(ID id, UB *name);
			ercd = fix_memorypool_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_CYCID
	  case TN_CYC:
		{
			IMPORT ER cyclichandler_setname(ID id, UB *name);
			ercd = cyclichandler_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_ALMID
	  case TN_ALM:
		{
			IMPORT ER alarmhandler_setname(ID id, UB *name);
			ercd = alarmhandler_setname(objid, name);
			break;
		}
#endif

#ifdef NUM_DOMID
	  case TN_DOM:
		{
			IMPORT ER domain_setname(ID id, UB *name);
			ercd = domain_setname(objid, name);
			break;
		}
#endif

	  default:
		ercd = E_PAR;

	}

	return ercd;
}

SYSCALL ER _td_ref_dsname( UINT type, ID id, UB *dsname )
{
	UB	*name_cb;
	ER	ercd;

	BEGIN_CRITICAL_SECTION;
	ercd = object_getname(type, id, &name_cb);
	if (ercd == E_OK) {
		strncpy((char*)dsname, (char*)name_cb, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

SYSCALL ER _td_set_dsname( UINT type, ID id, UB *dsname )
{
	ER	ercd;

	BEGIN_CRITICAL_SECTION;
	ercd = object_setname(type, id, dsname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
