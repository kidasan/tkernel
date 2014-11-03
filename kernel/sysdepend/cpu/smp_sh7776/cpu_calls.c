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
 *	cpu_calls.c (SH7776)
 *	CPU-Dependent System Call
 */

#include "kernel.h"
#include "task.h"
#include "check.h"
#include "cpu_task.h"
#include "ready_queue.h"

#include <sys/sysinfo.h>
#include <tk/sysdef.h>
#include <tk/smp_util.h>

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

#define MAX_INTVEC	( N_INTVEC + 2 + N_TRAVEC )

/*
 * High level programming language supported
 */

/* High level programming language interrupt handler entry */
EXPORT FP hll_inthdr[MAX_INTVEC];

/* High level programming language-supported routine (General) */
IMPORT void inthdr_startup();

/* For default handler */
IMPORT void defaulthdr_startup();

/* Mask registrer table */
typedef struct {
	UW	maskaddr[MAX_PROC];
	UW	mask;
} INTMASKTBL;

#define clraddr(addr)		(addr + 0x00000010)

LOCAL INTMASKTBL intmasktable[N_INTVEC] = {
	{{NULL, NULL}, 0},			/* 0x000 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x020 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x040 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x060 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x080 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x0A0 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x0C0 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x0E0 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x100 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x120 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x140 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x160 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x180 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x1A0 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x1C0 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x1E0 : Reserved (CPU exception) */
	{{NULL, NULL}, 0},			/* 0x200 : Reserved (IRL[3:0] 15) */
	{{NULL, NULL}, 0},			/* 0x220 : Reserved (IRL[3:0] 14) */
	{{NULL, NULL}, 0},			/* 0x240 : Reserved (IRL[3:0] 13) */
	{{NULL, NULL}, 0},			/* 0x260 : Reserved (IRL[3:0] 12) */
	{{NULL, NULL}, 0},			/* 0x280 : Reserved (IRL[3:0] 11) */
	{{NULL, NULL}, 0},			/* 0x2A0 : Reserved (IRL[3:0] 10) */
	{{NULL, NULL}, 0},			/* 0x2C0 : Reserved (IRL[3:0] 9) */
	{{NULL, NULL}, 0},			/* 0x2E0 : Reserved (IRL[3:0] 8) */
	{{NULL, NULL}, 0},			/* 0x300 : Reserved (IRL[3:0] 7) */
	{{NULL, NULL}, 0},			/* 0x320 : Reserved (IRL[3:0] 6) */
	{{NULL, NULL}, 0},			/* 0x340 : Reserved (IRL[3:0] 5) */
	{{NULL, NULL}, 0},			/* 0x360 : Reserved (IRL[3:0] 4) */
	{{NULL, NULL}, 0},			/* 0x380 : Reserved (IRL[3:0] 3) */
	{{NULL, NULL}, 0},			/* 0x3A0 : Reserved (IRL[3:0] 2) */
	{{NULL, NULL}, 0},			/* 0x3C0 : Reserved (IRL[3:0] 1) */
	{{C0INT2MSK0, C1INT2MSK0}, 0x00000001},	/* 0x3E0 : DU */
	{{C0INT2MSK1, C1INT2MSK1}, 0x80000000},	/* 0x400 : TMU-ch0 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x40000000},	/* 0x420 : TMU-ch1 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x20000000},	/* 0x440 : TMU-ch2 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x10000000},	/* 0x460 : TMU-ch2 input capture */
	{{C0INT2MSK1, C1INT2MSK1}, 0x08000000},	/* 0x480 : TMU-ch3 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x04000000},	/* 0x4A0 : TMU-ch4 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x02000000},	/* 0x4C0 : TMU-ch5 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x01000000},	/* 0x4E0 : TMU-ch5 input capture */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00800000},	/* 0x500 : TMU-ch6 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00400000},	/* 0x520 : TMU-ch7 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00200000},	/* 0x540 : TMU-ch8, TMU-ch8 input capture */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00100000},	/* 0x560 : TMU-ch9, TMU-ch10 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00080000},	/* 0x580 : USB2.0 h/f */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00040000},	/* 0x5A0 : SIU2 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00020000},	/* 0x5C0 : FM multiplex decoder */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00010000},	/* 0x5E0 : SATA */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00008000},	/* 0x600 : H-UDI0, H-UDI1 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00004000},	/* 0x620 : SHwyDMAC */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00002000},	/* 0x640 : YUV */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00001000},	/* 0x660 : TSIF */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000800},	/* 0x680 : 2bit ADCIF */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000400},	/* 0x6A0 : Remocon */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000200},	/* 0x6C0 : SSI0,1,2,3 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000100},	/* 0x6E0 : PCIE0 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000080},	/* 0x700 : PCIE1 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000040},	/* 0x720 : PCIE2 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000020},	/* 0x740 : VIN0,1,2 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000010},	/* 0x760 : MIMSLB */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000008},	/* 0x780 : R-GP1 */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000004},	/* 0x7A0 : 3DG */
	{{NULL, NULL}, 0},			/* 0x7C0 : Reserved (Unallocation) */
	{{C0INT2MSK1, C1INT2MSK1}, 0x00000001},	/* 0x7E0 : HSPI0,1 */
	{{NULL, NULL}, 0},			/* 0x800 : Reserved (General FPU disable exception) */
	{{NULL, NULL}, 0},			/* 0x820 : Reserved (Slot FPU disable exception) */
	{{NULL, NULL}, 0},			/* 0x840 : Reserved (Unallocation) */
	{{C0INT2MSK2, C1INT2MSK2}, 0x10000000},	/* 0x860 : I2C0,1 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x08000000},	/* 0x880 : RCAN(Mailbox), RCAN */
	{{C0INT2MSK2, C1INT2MSK2}, 0x04000000},	/* 0x8A0 : RESET/WDT */
	{{C0INT2MSK2, C1INT2MSK2}, 0x02000000},	/* 0x8C0 : SCIF-ch0 (ERI5, RXI5, BRI5, TXI5) */
	{{C0INT2MSK2, C1INT2MSK2}, 0x01000000},	/* 0x8E0 : SCIF-ch1-4 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00800000},	/* 0x900 : SCIF-ch5-8 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00400000},	/* 0x920 : SDIF0,1 */
	{{NULL, NULL}, 0},			/* 0x940 : Reserved (Unallocation) */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00100000},	/* 0x960 : Temperature sensor */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00080000},	/* 0x980 : GPIO0,1,2,3,4 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00040000},	/* 0x9A0 : PAM */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00020000},	/* 0x9C0 : IMR */
	{{NULL, NULL}, 0},			/* 0x9E0 : Reserved (Unallocation) */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00008000},	/* 0xA00 : DTU0 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00004000},	/* 0xA20 : DTU1 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00002000},	/* 0xA40 : V-coreX */
	{{NULL, NULL}, 0},			/* 0xA60 : Reserved (Unallocation) */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00000800},	/* 0xA80 : AESOP */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00000400},	/* 0xAA0 : GPS */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00000200},	/* 0xAC0 : HPB-DMAC ch0-15 */
	{{C0INT2MSK2, C1INT2MSK2}, 0x00000100},	/* 0xAE0 : LBSC-DMAC ch0-5, LBSC-ATA, LBSC-WTOE, MTSB */
	{{NULL, NULL}, 0},			/* 0xB00 : Reserved 00(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xB20 : Reserved 01(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xB40 : Reserved 02(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xB60 : Reserved 03(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xB80 : Reserved 04(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xBA0 : Reserved 05(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xBC0 : Reserved 06(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xBE0 : Reserved 07(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xC00 : Reserved 08(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xC20 : Reserved 09(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xC40 : Reserved 10(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xC60 : Reserved 11(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xC80 : Reserved 12(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xCA0 : Reserved 13(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xCC0 : Reserved 14(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xCE0 : Reserved 15(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xD00 : Reserved 16(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xD20 : Reserved 17(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xD40 : Reserved 18(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xD60 : Reserved 19(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xD80 : Reserved 20(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xDA0 : Reserved 21(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xDC0 : Reserved 22(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xDE0 : Reserved 23(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xE00 : Reserved 24(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xE20 : Reserved 25(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xE40 : Reserved 26(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xE60 : Reserved 27(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xE80 : Reserved 28(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xEA0 : Reserved 29(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xEC0 : Reserved 30(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xEE0 : Reserved 31(IO-CHIP) */
	{{NULL, NULL}, 0},			/* 0xF00 : Reserved (INTICI0) */
	{{NULL, NULL}, 0},			/* 0xF20 : Reserved (INTICI1) */
	{{NULL, NULL}, 0},			/* 0xF40 : Reserved (INTICI2) */
	{{NULL, NULL}, 0},			/* 0xF60 : Reserved (INTICI3) */
	{{NULL, NULL}, 0},			/* 0xF80 : Reserved (INTICI4) */
	{{NULL, NULL}, 0},			/* 0xFA0 : Reserved (INTICI5) */
	{{NULL, NULL}, 0},			/* 0xFC0 : Reserved (INTICI6) */
	{{NULL, NULL}, 0},			/* 0xFE0 : Reserved (INTICI7) */
};

/*
 * Check assign processor
 */
LOCAL BOOL chk_assprc( UW assprc )
{
	if ( (assprc == 0)
	  || ((assprc & ~ALL_CORE_BIT) != 0)
	  || ((assprc & (assprc-1)) != 0) ) {
		return FALSE;
	}
	return TRUE;
}

/*
 * Interrupt handler definition
 */
SYSCALL ER _tk_def_int( UINT dintno, T_DINT *pk_dint )
{
	FP	inthdr;
	INT	vecno = dintno >> 5;
	INTMASKTBL	*ptr;

	CHECK_PAR(vecno < MAX_INTVEC);

	if ( pk_dint != NULL ) {
		/* Set interrupt handler */
		if ( vecno == VECNO_TLBMISS ) {
			CHECK_RSATR(pk_dint->intatr, 0);
		} else {
			CHECK_RSATR(pk_dint->intatr, TA_HLNG | TA_ASSPRC);
		}
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
			hll_inthdr[vecno] = inthdr;
			inthdr = ( vecno == VECNO_DEFAULT )?
					defaulthdr_startup: inthdr_startup;
		}
		define_inthdr(vecno, inthdr);

		if ( (pk_dint->intatr & TA_ASSPRC) != 0 ) {
			if ( vecno < N_INTVEC) {
				ptr = &intmasktable[vecno];
				if ( ptr->mask != 0) {
					if ( pk_dint->assprc & TP_PRC1 ) {
						out_w(ptr->maskaddr[1], in_w(ptr->maskaddr[1]) | ptr->mask);
						out_w(clraddr(ptr->maskaddr[0]), ptr->mask);
					}
					else if ( pk_dint->assprc & TP_PRC2 ) {
						out_w(ptr->maskaddr[0], in_w(ptr->maskaddr[0]) | ptr->mask);
						out_w(clraddr(ptr->maskaddr[1]), ptr->mask);
					}
				}
			}
		}
		END_CRITICAL_NO_DISPATCH;
	} else {
		/* Free interrupt handler */
		switch ( vecno ) {
		  case VECNO_TRAPA:	inthdr = SaveMonHdr.trapa_hdr;	 break;
		  case VECNO_BREAK:	inthdr = SaveMonHdr.break_hdr;	 break;
		  case VECNO_MONITOR:	inthdr = SaveMonHdr.monitor_hdr; break;
		  case VECNO_DEFAULT:	inthdr = SaveMonHdr.default_hdr; break;
		  case VECNO_TLBMISS:	inthdr = SaveMonHdr.tlbmiss_hdr; break;
		  default:		inthdr = NULL;
		}

		BEGIN_CRITICAL_SECTION;
		define_inthdr(vecno, inthdr);
		if ( vecno < MAX_INTVEC ) {
			hll_inthdr[vecno] = NULL;
		}
		if ( vecno < N_INTVEC) {
			ptr = &intmasktable[vecno];
			if ( ptr->mask != 0) {
				out_w(ptr->maskaddr[0], in_w(ptr->maskaddr[0]) | ptr->mask);
				out_w(ptr->maskaddr[1], in_w(ptr->maskaddr[1]) | ptr->mask);
			}
		}
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
	const UW	SR_Mask		/* SR unchangeable bit */
		= 0x8fff7c0c | SR_MD | SR_RB | SR_BL | SR_FD | SR_I(15);
	SStackFrame	*ssp;
	UW		sr;
	INT		i;

	ssp = tcb->tskctxb.ssp;
	sr = ssp->rng0_3.ssr;

	if ( cregs != NULL ) {
		ssp = cregs->ssp;
	}

	if ( regs != NULL ) {
		for ( i = 0; i < 15; ++i ) {
			ssp->rng0_3.r[i] = regs->r[i];
		}
		ssp->rng0_3.mach = regs->mach;
		ssp->rng0_3.macl = regs->macl;
		ssp->rng0_3.gbr  = regs->gbr;
		ssp->rng0_3.pr   = regs->pr;
	}

	if ( eit != NULL ) {
		ssp->rng0_3.spc = eit->pc;
		ssp->rng0_3.ssr = (sr & SR_Mask) | (eit->sr & ~SR_Mask);
		ssp->rng0_3.mdr = eit->mdr;
	}

	if ( cregs != NULL ) {
		tcb->tskctxb.ssp     = cregs->ssp;
		tcb->tskctxb.uatb    = cregs->uatb;
		tcb->tskctxb.lsid    = cregs->lsid;
		ssp->rng0_3.bank1_r0 = cregs->r0;
		ssp->rng0_3.bank1_r1 = cregs->r1;

		if ( (tcb->tskatr & TA_RNG3) != 0 ) {
			/* RNG 1-3 */
			((VP*)tcb->isstack)[-1] = cregs->usp;
		}
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
	INT		i;

	ssp = tcb->tskctxb.ssp;

	if ( regs != NULL ) {
		for ( i = 0; i < 15; ++i ) {
			regs->r[i] = ssp->rng0_3.r[i];
		}
		regs->mach = ssp->rng0_3.mach;
		regs->macl = ssp->rng0_3.macl;
		regs->gbr  = ssp->rng0_3.gbr;
		regs->pr   = ssp->rng0_3.pr;
	}

	if ( eit != NULL ) {
		eit->pc  = ssp->rng0_3.spc;
		eit->sr  = ssp->rng0_3.ssr;
		eit->mdr = ssp->rng0_3.mdr;
	}

	if ( cregs != NULL ) {
		cregs->sstop = tcb->isstack;
		cregs->ssp   = tcb->tskctxb.ssp;
		cregs->usp   = ((VP*)tcb->isstack)[-1];
		cregs->uatb  = tcb->tskctxb.uatb;
		cregs->lsid  = tcb->tskctxb.lsid;
		cregs->r0    = ssp->rng0_3.bank1_r0;
		cregs->r1    = ssp->rng0_3.bank1_r1;
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
#if USE_SH3_DSP
				/* Set DSP register */
				memcpy(tcb->isstack, pk_copregs, sizeof(T_COP0REGS));
#endif
#if TA_FPU
				/* Set FPU register */
				memcpy(tcb->isstack, pk_copregs, sizeof(T_COP0REGS));
#endif
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

	BEGIN_CRITICAL_NO_DISPATCH;	/* Because the state of the task doesn't change. */
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
#if USE_SH3_DSP
				/* Get DSP register */
				memcpy(pk_copregs, tcb->isstack, sizeof(T_COP0REGS));
#endif
#if TA_FPU
				/* Get FPU register */
				memcpy(pk_copregs, tcb->isstack, sizeof(T_COP0REGS));
#endif
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
#if 0 /* (030206) This is not an error. Please ignore. */
	if ( eit != NULL ) {
		if ( (eit->sr & (SR_BL|SR_I(15))) != 0 ) {
			return E_PAR;
		}
	}
#endif

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
