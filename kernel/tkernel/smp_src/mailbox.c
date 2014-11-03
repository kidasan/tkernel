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
 *	mailbox.c (T-Kernel/OS)
 *	Mailbox
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

#ifdef NUM_MBXID

EXPORT ID	max_mbxid;	/* Maximum mailbox ID */

/*
 * Mailbox control block
 *
 *	'mq_head' is the first message queue pointer that
 *	points a message.
 *	It is NULL if the message queue is empty.
 *	'mq_tail' is a pointer that points end of message
 *	queue that is not empty.
 *	The message queue value is not guaranteed if the
 *	message queue is empty.
 *	It is used only if the message queue is FIFO (TA_MFIFO).
 */
typedef struct mailbox_control_block {
	QUEUE	wait_queue;	/* Mailbox wait queue */
	ID	mbxid;		/* Mailbox ID */
	VP	exinf;		/* Extended information */
	ATR	mbxatr;		/* Mailbox attribute */
	T_MSG	mq_head;	/* Head of message queue */
	T_MSG	*mq_tail;	/* End of message queue */
	/* For SMP T-Kernel */
	UB	oname[OBJECT_NAME_LENGTH];	/* name */
} MBXCB;

LOCAL MBXCB	*mbxcb_table;	/* Mailbox control block */
LOCAL QUEUE	free_mbxcb;	/* FreeQue */

#define get_mbxcb(id)	( &mbxcb_table[INDEX_MBX(id)] )

/*
 * Initialization of mailbox control block 
 */
EXPORT ER mailbox_initialize( void )
{
	MBXCB	*mbxcb, *end;
	W	n;

	/* Get system information */
	n = _tk_get_cfn(SCTAG_TMAXMBXID, &max_mbxid, 1);
	if ( (n < 1) || (NUM_MBXID < 1) ) {
		return E_SYS;
	}

	/* Create mailbox control block */
	mbxcb_table = Imalloc((UINT)NUM_MBXID * sizeof(MBXCB));
	if ( mbxcb_table == NULL ) {
		return E_NOMEM;
	}

	/* Register all control blocks onto FreeQue */
	QueInit(&free_mbxcb);
	end = mbxcb_table + NUM_MBXID;
	for ( mbxcb = mbxcb_table; mbxcb < end; mbxcb++ ) {
		mbxcb->mbxid = 0;
		QueInsert(&mbxcb->wait_queue, &free_mbxcb);
	}

	return E_OK;
}


/*
 * Head message
 */
#define headmsg(mbxcb)	( (mbxcb)->mq_head.msgque[0] )

/*
 * Next message
 */
#define nextmsg(msg)	( ((T_MSG*)(msg))->msgque[0] )

/*
 * Insert a message queue following priority
 */
Inline void queue_insert_mpri( T_MSG_PRI *pk_msg, T_MSG *head )
{
	T_MSG_PRI	*msg;
	T_MSG		*prevmsg = head;

	while ( (msg = (T_MSG_PRI*)nextmsg(prevmsg)) != NULL ) {
		if ( msg->msgpri > pk_msg->msgpri ) {
			break;
		}
		prevmsg = (T_MSG*)msg;
	}
	nextmsg(pk_msg) = msg;
	nextmsg(prevmsg) = pk_msg;
}

/*
 * Processing if the priority of wait task changes
 */
LOCAL void mbx_chg_pri( TCB *tcb, INT oldpri )
{
	MBXCB	*mbxcb;

	mbxcb = get_mbxcb(tcb->wid);
	gcb_change_priority((GCB*)mbxcb, tcb);
}

/*
 * Definition of mailbox wait specification
 */
LOCAL WSPEC wspec_mbx_tfifo = { TTW_MBX, NULL, NULL };
LOCAL WSPEC wspec_mbx_tpri  = { TTW_MBX, mbx_chg_pri, NULL };


/*
 * Create mailbox
 */
SYSCALL ID _tk_cre_mbx( T_CMBX *pk_cmbx )
{
#if CHK_RSATR
	const ATR VALID_MBXATR = {
		 TA_MPRI
		|TA_TPRI
		|TA_NODISWAI
		|TA_DOMID
		|TA_ONAME
		|TA_PRIVATE
		|TA_PROTECTED
		|TA_PUBLIC
	};
#endif
	MBXCB	*mbxcb;
	ID	mbxid;
	ER	ercd;
	ID	domid;
	UB	*oname;

	CHECK_RSATR(pk_cmbx->mbxatr, VALID_MBXATR);
	CHECK_PATR(pk_cmbx->mbxatr);
#if CHK_ID
	if ( (pk_cmbx->mbxatr & TA_DOMID) != 0 ) {
		CHECK_DOMID(pk_cmbx->domid);
	}
#endif

	BEGIN_CRITICAL_SECTION;
	/* Check domid */
	if ( (pk_cmbx->mbxatr & TA_DOMID) != 0 ) {
		ercd = mp_existence_check_of_domain(pk_cmbx->domid);
		if ( ercd != E_OK ) {
			goto error_exit;
		}
		domid = pk_cmbx->domid;
	} else {
		domid = MP_KDMID;
	}
	/* Check object name */
	if ( (pk_cmbx->mbxatr & TA_ONAME) != 0 ) {
		ercd = mp_existence_check_of_oname(MP_OBJECT_TYPE_MBX,
						   mbxcb_table->oname,
						   sizeof(MBXCB),
						   domid,
						   pk_cmbx->oname);
		if ( ercd != E_OK ) {
			/* Already exists */
			goto error_exit;
		}
		oname = pk_cmbx->oname;
	} else {
		oname = default_object_name;
	}
	/* Check protection */
	ercd = mp_check_protection_cre(domid, pk_cmbx->mbxatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Get control block from FreeQue */
	mbxcb = (MBXCB*)QueRemoveNext(&free_mbxcb);
	if ( mbxcb == NULL ) {
		ercd = E_LIMIT;
	} else {
		mbxid = MP_GEN_DOMIDPART(domid) | ID_MBX(mbxcb - mbxcb_table);

		/* Initialize control block */
		QueInit(&mbxcb->wait_queue);
		mbxcb->mbxid  = mbxid;
		mbxcb->exinf  = pk_cmbx->exinf;
		mbxcb->mbxatr = pk_cmbx->mbxatr;
		mbxcb->mq_head.msgque[0] = NULL;

		/* Set object name */
		strncpy((char*)mbxcb->oname, (char*)oname, OBJECT_NAME_LENGTH);
		/* Registration */
		mp_register_object_in_domain(domid,
					     MP_OBJECT_TYPE_MBX,
					     INDEX_MBX(mbxid),
					     oname);

		ercd = mbxid;
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Delete mailbox
 */
SYSCALL ER _tk_del_mbx( ID mbxid )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mbxid, mbxcb->mbxid, mbxcb->mbxatr);
		if ( ercd == E_OK ) {
			/* Release wait state of task (E_DLT) */
			wait_delete(&mbxcb->wait_queue);

			/* Unregistration */
			mp_unregister_object_in_domain(mbxid,
						       MP_OBJECT_TYPE_MBX,
						       INDEX_MBX(mbxid));

			/* Return to FreeQue */
			QueInsert(&mbxcb->wait_queue, &free_mbxcb);
			mbxcb->mbxid = 0;
		}
	}
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Send to mailbox
 */
SYSCALL ER _tk_snd_mbx( ID mbxid, T_MSG *pk_msg )
{
	MBXCB	*mbxcb;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if (mbxcb->mbxid == 0) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(mbxid, mbxcb->mbxid, mbxcb->mbxatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	if ( (mbxcb->mbxatr & TA_MPRI) != 0 ) {
		if ( ((T_MSG_PRI*)pk_msg)->msgpri <= 0 ) {
			ercd = E_PAR;
			goto error_exit;
		}
	}

	if ( !isQueEmpty(&mbxcb->wait_queue) ) {
		/* Directly send to receive wait task */
		tcb = (TCB*)(mbxcb->wait_queue.next);
		*tcb->winfo.mbx.ppk_msg = pk_msg;
		wait_release_ok(tcb);

	} else {
		/* Connect message to queue */
		if ( (mbxcb->mbxatr & TA_MPRI) != 0 ) {
			/* Connect message to queue following priority */
			queue_insert_mpri((T_MSG_PRI*)pk_msg, &mbxcb->mq_head);
		} else {
			/* Connect to end of queue */
			nextmsg(pk_msg) = NULL;
			if ( headmsg(mbxcb) == NULL ) {
				headmsg(mbxcb) = pk_msg;
			} else {
				nextmsg(mbxcb->mq_tail) = pk_msg;
			}
			mbxcb->mq_tail = pk_msg;
		}
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Receive from mailbox
 */
SYSCALL ER _tk_rcv_mbx( ID mbxid, T_MSG **ppk_msg, TMO tmout )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;
	UINT	prid;

	CHECK_MBXID(mbxid);
	CHECK_TMOUT(tmout);
	CHECK_DISPATCH();

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
		goto error_exit;
	}
	ercd = mp_check_domain_and_protection(mbxid, mbxcb->mbxid, mbxcb->mbxatr);
	if ( ercd != E_OK ) {
		goto error_exit;
	}

	/* Check receive wait disable */
	if ( is_diswai((GCB*)mbxcb, ctxtsk[prid], TTW_MBX) ) {
		ercd = E_DISWAI;
		goto error_exit;
	}

	if ( headmsg(mbxcb) != NULL ) {
		/* Get message from head of queue */
		*ppk_msg = headmsg(mbxcb);
		headmsg(mbxcb) = nextmsg(*ppk_msg);
	} else {
		/* Ready for receive wait */
		ctxtsk[prid]->wspec = ( (mbxcb->mbxatr & TA_TPRI) != 0 )?
					&wspec_mbx_tpri: &wspec_mbx_tfifo;
		ctxtsk[prid]->wercd = &ercd;
		ctxtsk[prid]->winfo.mbx.ppk_msg = ppk_msg;
		gcb_make_wait_with_diswai((GCB*)mbxcb, tmout);
	}

    error_exit:
	END_CRITICAL_SECTION;

	return ercd;
}

/*
 * Refer mailbox state 
 */
SYSCALL ER _tk_ref_mbx( ID mbxid, T_RMBX *pk_rmbx )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mbxid, mbxcb->mbxid, mbxcb->mbxatr);
		if ( ercd == E_OK ) {
			pk_rmbx->exinf = mbxcb->exinf;
			pk_rmbx->wtsk = wait_tskid(&mbxcb->wait_queue);
			pk_rmbx->pk_msg = headmsg(mbxcb);
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
 * Find mailbox ID
 */
SYSCALL ER _tk_fnd_mbx( ID domid, UB *oname )
{
	ER	ercd;
	ID	mbxid = 0;
	MBXCB	*mbxcb;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_MBX, (UB*)mbxcb_table, mbxcb_table->oname,
			       (UB*)&mbxcb_table->mbxid, sizeof(MBXCB), domid, oname);
	if ( 0 < ercd ) {
		mbxid = (ID)ercd;
		mbxcb = get_mbxcb(mbxid);
		ercd = mp_check_protection_fnd(mbxid, mbxcb->mbxatr);
	}
	END_CRITICAL_NO_DISPATCH;

	return ( ercd < E_OK )? ercd: mbxid;
}

/*
 * Get domain information on mailbox
 */
SYSCALL ER _tk_dmi_mbx( ID mbxid, T_DMI *pk_dmi )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(mbxid, mbxcb->mbxid, mbxcb->mbxatr);
		if ( ercd == E_OK ) {
			pk_dmi->domatr  = MP_GET_DOMATR(mbxcb->mbxatr);
			pk_dmi->domid = mp_get_domid_from_id(mbxcb->mbxid);
			pk_dmi->kdmid = MP_KDMID_SELF;
			memcpy(pk_dmi->oname, mbxcb->oname, OBJECT_NAME_LENGTH);
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
EXPORT ER mailbox_getname(ID id, UB **name)
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(id);

	mbxcb = get_mbxcb(id);
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, mbxcb->mbxid) ) {
		ercd = E_NOEXS;
	} else if ( (mbxcb->mbxatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		*name = mbxcb->oname;
	}

	return ercd;
}

/*
 * Set object name to control block
 */
EXPORT ER mailbox_setname(ID id, UB *name)
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(id);

	mbxcb = get_mbxcb(id);
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(id, mbxcb->mbxid) ) {
		ercd = E_NOEXS;
	} else if ( (mbxcb->mbxatr & TA_ONAME) == 0 ) {
		ercd = E_OBJ;
	} else {
		ercd = mp_change_oname(MP_OBJECT_TYPE_MBX, mbxcb_table->oname, sizeof(MBXCB), id, name, INDEX_MBX(id));
	}

	return ercd;
}

/*
 * Refer mailbox usage state
 */
SYSCALL INT _td_lst_mbx( ID list[], INT nent )
{
	MBXCB	*mbxcb, *end;
	INT	n = 0;

	BEGIN_CRITICAL_SECTION;
	end = mbxcb_table + NUM_MBXID;
	for ( mbxcb = mbxcb_table; mbxcb < end; mbxcb++ ) {
		if ( mbxcb->mbxid == 0 ) {
			continue;
		}

		if ( n++ < nent ) {
			*list++ = mbxcb->mbxid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return n;
}

/*
 * Refer mailbox state
 */
SYSCALL ER _td_ref_mbx( ID mbxid, TD_RMBX *pk_rmbx )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mbxid, mbxcb->mbxid) ) {
		ercd = E_NOEXS;
	} else {
		pk_rmbx->exinf = mbxcb->exinf;
		pk_rmbx->wtsk = wait_tskid(&mbxcb->wait_queue);
		pk_rmbx->pk_msg = headmsg(mbxcb);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Refer mailbox wait queue
 */
SYSCALL INT _td_mbx_que( ID mbxid, ID list[], INT nent )
{
	MBXCB	*mbxcb;
	QUEUE	*q;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mbxid, mbxcb->mbxid) ) {
		ercd = E_NOEXS;
	} else {
		INT n = 0;
		for ( q = mbxcb->wait_queue.next; q != &mbxcb->wait_queue; q = q->next ) {
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
 * Find mailbox ID
 */
SYSCALL ER _td_fnd_mbx( ID domid, UB *oname )
{
	ER	ercd;

	CHECK_DOMID(domid);

	BEGIN_CRITICAL_SECTION;
	ercd = mp_search_oname(MP_OBJECT_TYPE_MBX, (UB*)mbxcb_table, mbxcb_table->oname,
			       (UB*)&mbxcb_table->mbxid, sizeof(MBXCB), domid, oname);
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get domain information on mailbox
 */
SYSCALL ER _td_dmi_mbx( ID mbxid, TD_DMI *pk_dmi )
{
	MBXCB	*mbxcb;
	ER	ercd = E_OK;

	CHECK_MBXID(mbxid);

	mbxcb = get_mbxcb(mbxid);

	BEGIN_CRITICAL_SECTION;
	if ( mbxcb->mbxid == 0 ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(mbxid, mbxcb->mbxid) ) {
		ercd = E_NOEXS;
	} else {
		pk_dmi->domatr = MP_GET_DOMATR(mbxcb->mbxatr);
		pk_dmi->domid  = mp_get_domid_from_id(mbxcb->mbxid);
		pk_dmi->kdmid = MP_KDMID_SELF;
		memcpy(pk_dmi->oname, mbxcb->oname, OBJECT_NAME_LENGTH);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */
#endif /* NUM_MBXID */
