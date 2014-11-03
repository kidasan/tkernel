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
 *	cpu_calls.c (NAVIENGINE)
 *	CPU-Dependent System Call
 */

#include "kernel.h"
#include "task.h"
#include "check.h"
#include "cpu_task.h"
#include "ready_queue.h"

#include <sys/sysinfo.h>
#include <tk/sysdef.h>
#include "cpu_insn.h"

/* For SMP T-Kernel */
#include <sys/smp_def.h>
#include <sys/kernel_util.h>
#include "smpkernel.h"
#include "mp_common.h"
#include "mp_domain.h"

/*
 * Dispatch enable/disable
 */
SYSCALL ER _tk_dis_dsp( void )
{
	BOOL	done = FALSE;
	UINT	prid;

	CHECK_CTX(!in_loc());

  retry:
	BEGIN_CRITICAL_SECTION;
	prid = get_prid();
	if ( (ctxtsk[prid]->state & TS_SUSPEND) == 0 ) {
		dispatch_disabled[prid] = DDS_DISABLE;
		done = TRUE;
	}
	END_CRITICAL_NO_DISPATCH;
	if ( !done ) {
		goto retry;
	}

	return E_OK;
}

/*
 * Dispatch enable
 */
SYSCALL ER _tk_ena_dsp( void )
{
	CHECK_CTX(!in_loc());

	BEGIN_CRITICAL_SECTION
	dispatch_disabled[get_prid()] = DDS_ENABLE;
	ready_queue_top(&ready_queue, schedtsk);
	END_CRITICAL_SECTION

	return E_OK;
}

/* ------------------------------------------------------------------------ */

/*
 * High level programming language
 */

/* High level programming language interrupt handler entry */
EXPORT FP hll_inthdr[N_INTVEC];

/* High level programming language routine (Interrupt) */
IMPORT void inthdr_startup();

/* High level programming language routine (Exception) */
IMPORT void exchdr_startup();

/* For default handler */
IMPORT void defaulthdr_startup();

#define IRQVEC_TOP	EIT_FIQ
#define IRQVEC_END	EIT_END

/*
 * Check assign processor
 */
#if CHK_PAR
LOCAL BOOL chk_assprc( UW assprc )
{
	if ( (assprc == 0)
	  || ((assprc & ~ALL_CORE_BIT) != 0)
	  || ((assprc & (assprc-1)) != 0) ) {
		return FALSE;
	}
	return TRUE;
}
#endif

/*
 * Interrupt handler definition
 */
SYSCALL ER _tk_def_int( UINT dintno, T_DINT *pk_dint )
{
	FP	inthdr;

	CHECK_PAR(dintno < N_INTVEC);

	if ( pk_dint != NULL ) {
		/* Set interrupt handler */
		CHECK_RSATR(pk_dint->intatr, TA_HLNG|TA_ASSPRC);
#if PSR_DI == PSR_I
		CHECK_PAR( !(dintno == EIT_FIQ
				&& (pk_dint->intatr & TA_HLNG) != 0) );
#endif
#if CHK_PAR
		if ( (pk_dint->intatr & TA_ASSPRC) != 0 ) {
			if ( !chk_assprc(pk_dint->assprc) ) {
				return E_PAR;
			}
		}
#endif

		inthdr = pk_dint->inthdr;

		BEGIN_CRITICAL_SECTION;
		if ( (pk_dint->intatr & TA_HLNG) != 0 ) {
			hll_inthdr[dintno] = inthdr;
			inthdr = ( dintno == EIT_DEFAULT )? defaulthdr_startup:
				 ( dintno <  IRQVEC_TOP  )? exchdr_startup:
				 ( dintno <= IRQVEC_END  )? inthdr_startup:
				                            exchdr_startup;
		}
		define_inthdr(dintno, inthdr);
		/* Set cpu target */
		if ( (pk_dint->intatr & TA_ASSPRC) != 0 ) {
			SetIntAssprc(dintno, pk_dint->assprc);
		} else {
			SetIntAssprc(dintno, 1 << get_prid());
		}
		END_CRITICAL_NO_DISPATCH;
	} else {
		/* Clear interrupt handler */
		switch ( dintno ) {
		  case VECNO_DEFAULT:	inthdr = SaveMonHdr.default_hdr; break;
		  case VECNO_BREAK:	inthdr = SaveMonHdr.break_hdr;	 break;
		  case VECNO_MONITOR:	inthdr = SaveMonHdr.monitor_hdr; break;
		  default:		inthdr = NULL;
		}

		BEGIN_CRITICAL_SECTION;
		/* Clear cpu target */
		ClrIntAssprc(dintno);
		define_inthdr(dintno, inthdr);
		hll_inthdr[dintno] = NULL;
		END_CRITICAL_NO_DISPATCH;
	}

	return E_OK;
}

/* ------------------------------------------------------------------------ */

/*
 * Get task space
 */
SYSCALL ER _tk_get_tsp( ID tskid, T_TSKSPC *pk_tskspc )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			pk_tskspc->uatb = tcb->tskctxb.uatb;
			pk_tskspc->lsid = tcb->tskctxb.lsid;
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Set task space
 */
SYSCALL ER _tk_set_tsp( ID tskid, T_TSKSPC *pk_tskspc )
{
	TCB	*tcb;
	ER	ercd = E_OK;
	UINT	prid;
	INT	i;

	CHECK_TSKID_SELF(tskid);

	BEGIN_CRITICAL_SECTION;
	tcb = get_tcb_self(tskid);
	if ( tcb->state == TS_NONEXIST) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection_tsk(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd != E_OK ) {
			goto error_exit;
		}

		/* When it is the currently running task,
		   switch the current space */
		prid = get_prid();
		for ( i = 0; i < num_proc; i++ ) {
			if ( (i != prid) && (tcb == ctxtsk[i]) ) {
				ercd = E_OBJ;
				break;
			}
		}
		if ( ercd != E_OBJ ) {
			tcb->tskctxb.uatb = pk_tskspc->uatb;
			tcb->tskctxb.lsid = pk_tskspc->lsid;
			if ( ctxtsk[prid] == tcb ) {
				change_space(tcb->tskctxb.uatb, tcb->tskctxb.lsid);
			}
		}
	}
    error_exit:
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/* ------------------------------------------------------------------------ */

/*
 * Set task register contents
 */
LOCAL void set_reg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
	SStackFrame	*ssp;
	UW	cpsr;
	INT	i;

	ssp = tcb->tskctxb.ssp;
	cpsr = ssp->spsr_svc;

	if ( cregs != NULL ) {
		ssp = cregs->ssp;
	}

	if ( regs != NULL ) {
		for ( i = 0; i < 12; ++i ) {
			ssp->r[i] = regs->r[i];
		}
		ssp->ip = regs->r[12];
		if ( (cpsr & PSR_M(31)) == PSR_SVC ) {
			ssp->lr_svc = regs->lr;
		} else {
			ssp->lr_usr = regs->lr;
		}
	}

	if ( eit != NULL ) {
		ssp->pc       = eit->pc;
		ssp->spsr_svc = (eit->cpsr & 0xff000000) | (cpsr & 0x00ffffff);
		ssp->taskmode = eit->taskmode;
	}

	if ( cregs != NULL ) {
		tcb->tskctxb.ssp  = cregs->ssp;
		tcb->tskctxb.uatb = cregs->uatb;
		tcb->tskctxb.lsid = cregs->lsid;

		ssp->usp = cregs->usp;
	}
}

/*
 * Set task register contents
 */
SYSCALL ER _tk_set_reg( ID tskid,
		T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs )
{
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_INTSK();
	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			if ( is_ctxtsk(tcb) ) {
				ercd = E_OBJ;
			} else {
				set_reg(tcb, pk_regs, pk_eit, pk_cregs);
			}
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get task register contents
 */
LOCAL void get_reg( TCB *tcb, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
	SStackFrame	*ssp;
	UW		cpsr;
	INT		i;

	ssp = tcb->tskctxb.ssp;
	cpsr = ssp->spsr_svc;

	if ( regs != NULL ) {
		for ( i = 0; i < 12; ++i ) {
			regs->r[i] = ssp->r[i];
		}
		regs->r[12] = ssp->ip;
		if ( (cpsr & PSR_M(31)) == PSR_SVC ) {
			regs->lr = ssp->lr_svc;
		} else {
			regs->lr = ssp->lr_usr;
		}
	}

	if ( eit != NULL ) {
		eit->pc       = ssp->pc;
		eit->cpsr     = ssp->spsr_svc;
		eit->taskmode = ssp->taskmode;
	}

	if ( cregs != NULL ) {
		cregs->ssp   = tcb->tskctxb.ssp;
		cregs->uatb  = tcb->tskctxb.uatb;
		cregs->lsid  = tcb->tskctxb.lsid;

		cregs->usp = ssp->usp;
	}
}

/*
 * Get task register contents
 */
SYSCALL ER _tk_get_reg( ID tskid,
		T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs )
{
	TCB		*tcb;
	ER		ercd = E_OK;

	CHECK_INTSK();
	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			if ( is_ctxtsk(tcb) ) {
				ercd = E_OBJ;
			} else {
				get_reg(tcb, pk_regs, pk_eit, pk_cregs);
			}
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Set task coprocessor register
 */
SYSCALL ER _tk_set_cpr( ID tskid, INT copno, T_COPREGS *pk_copregs )
{
	ATR	copatr = TA_COP0 << copno;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_INTSK();
	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);
	CHECK_PAR((copatr & available_cop) != 0);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			if ( (tcb->tskatr & copatr) == 0 ) {
				ercd = E_PAR;
			} else if ( is_ctxtsk(tcb) ) {
				ercd = E_OBJ;
			} else {
				/* No coprocessor */
			}
		}
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get task coprocessor register
 */
SYSCALL ER _tk_get_cpr( ID tskid, INT copno, T_COPREGS *pk_copregs )
{
	ATR	copatr = TA_COP0 << copno;
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_INTSK();
	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);
	CHECK_PAR((copatr & available_cop) != 0);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else {
		ercd = mp_check_domain_and_protection(tskid, tcb->tskid, tcb->tskatr);
		if ( ercd == E_OK ) {
			if ( (tcb->tskatr & copatr) == 0 ) {
				ercd = E_PAR;
			} else if ( is_ctxtsk(tcb) ) {
				ercd = E_OBJ;
			} else {
				/* No coprocessor */
			}
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
 * Set task register
 */
SYSCALL ER _td_set_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else if ( is_ctxtsk(tcb) ) {
		ercd = E_OBJ;
	} else {
		set_reg(tcb, regs, eit, cregs);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

/*
 * Get task register
 */
SYSCALL ER _td_get_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs )
{
	TCB	*tcb;
	ER	ercd = E_OK;

	CHECK_TSKID(tskid);
	CHECK_NONSELF(tskid);

	tcb = get_tcb(tskid);

	BEGIN_CRITICAL_SECTION;
	if ( tcb->state == TS_NONEXIST ) {
		ercd = E_NOEXS;
	} else if ( !MP_CMP_DOMIDPART(tskid, tcb->tskid) ) {
		ercd = E_NOEXS;
	} else if ( is_ctxtsk(tcb) ) {
		ercd = E_OBJ;
	} else {
		get_reg(tcb, regs, eit, cregs);
	}
	END_CRITICAL_NO_DISPATCH;

	return ercd;
}

#endif /* USE_DBGSPT */

