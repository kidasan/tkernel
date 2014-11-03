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
 *	@(#)dbgspt.h (T-Kernel)
 *
 *	T-Kernel Debugger Support (Common parts)
 */

#ifndef __TK_DBGSPT_H__
#define __TK_DBGSPT_H__

#include <basic.h>
#include <tk/typedef.h>
#include <tk/syscall.h>

#include <tk/sysdepend/dbgspt_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Object name information		td_ref_dsname, td_set_dsname
 */
#define TN_TSK 0x01
#define TN_SEM 0x02
#define TN_FLG 0x03
#define TN_MBX 0x04
#define TN_MBF 0x05
#define TN_POR 0x06
#define TN_MTX 0x07
#define TN_MPL 0x08
#define TN_MPF 0x09
#define TN_CYC 0x0a
#define TN_ALM 0x0b
#define TN_DOM 0x0c

/*
 * Semaphore state information		td_ref_sem
 */
typedef	struct td_rsem {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	semcnt;		/* Current semaphore value */
} TD_RSEM;

/*
 * Event flag state information		td_ref_flg
 */
typedef	struct td_rflg {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	UINT	flgptn;		/* Current event flag pattern */
} TD_RFLG;

/*
 * Mail box state information		td_ref_mbx
 */
typedef	struct td_rmbx {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	T_MSG	*pk_msg;	/* Next received message */
} TD_RMBX;

/*
 * Mutex state information		td_ref_mtx
 */
typedef struct td_rmtx {
	VP	exinf;		/* Extended information */
	ID	htsk;		/* Locking task ID */
	ID	wtsk;		/* Lock wait task ID */
} TD_RMTX;

/*
 * Message buffer state information 	td_ref_mbf
 */
typedef struct td_rmbf {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Receive wait task ID */
	ID	stsk;		/* Send wait task ID */
	INT	msgsz;		/* Next received message size (byte) */
	INT	frbufsz;	/* Free buffer size (byte) */
	INT	maxmsz;		/* Maximum length of message (byte) */
} TD_RMBF;

/*
 * Rendezvous port state information	td_ref_por
 */
typedef struct td_rpor {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Call wait task ID */
	ID	atsk;		/* Receive wait task ID */
	INT	maxcmsz;	/* Maximum length of call message (byte) */
	INT	maxrmsz;	/* Maximum length of replay message (byte) */
} TD_RPOR;

/*
 * Fixed size memory pool state information	td_ref_mpf
 */
typedef struct td_rmpf {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	frbcnt;		/* Number of free blocks */
} TD_RMPF;

/*
 * Variable size memory pool state information	td_ref_mpl
 */
typedef struct td_rmpl {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	frsz;		/* Total size of free area (byte) */
	INT	maxsz;		/* Size of maximum continuous
				   free area (byte) */
} TD_RMPL;

/*
 * Cycle handler state information	td_ref_cyc
 */
typedef struct td_rcyc {
	VP	exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until next handler startup */
	UINT	cycstat;	/* Cycle handler status */
} TD_RCYC;

/*
 * Alarm handler state information	td_ref_alm
 */
typedef struct td_ralm {
	VP	exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until handler startup */
	UINT	almstat;	/* Alarm handler status */
} TD_RALM;

/*
 * Subsystem state information		td_ref_ssy
 */
typedef struct td_rssy {
	PRI	ssypri;		/* Subsystem priority */
	INT	resblksz;	/* Resource management block size (byte) */
} TD_RSSY;

/*
 * Domain state information		td_ref_dom
 */
typedef struct td_rdom {
	VP	exinf;		/* Extended information */
	ID	domid;		/* Domain ID of a domain belonging to */
	UINT	objcnt;		/* Number of objects belonging to a domain */
} TD_RDOM;

/*
 * Task state information		td_ref_tsk
 */
typedef	struct td_rtsk {
	VP	exinf;		/* Extended information */
	PRI	tskpri;		/* Current priority */
	PRI	tskbpri;	/* Base priority */
	UINT	tskstat;	/* Task state */
	UINT	tskwait;	/* Wait factor */
	ID	wid;		/* Wait object ID */
	INT	wupcnt;		/* Number of wakeup requests queuing */
	INT	suscnt;		/* Number of SUSPEND request nests */
	RELTIM	slicetime;	/* Maximum continuous execution time
				   (millisecond) */
	UINT	waitmask;	/* Disabled wait factor */
	UINT	texmask;	/* Enabled task exception */
	UINT	tskevent;	/* Occurring task event */
	FP	task;		/* Task startup address */
	INT	stksz;		/* User stack size (byte) */
	INT	sstksz;		/* System stack size (byte) */
	VP	istack;		/* User stack pointer initial value */
	VP	isstack;	/* System stack pointer initial value */
} TD_RTSK;

/*
 * Task exception state information		td_ref_tex
 */
typedef struct td_rtex {
	UINT	pendtex;	/* Occurring task exception */
	UINT	texmask;	/* Enabled task exception */
} TD_RTEX;

/*
 * Task statistics information		td_inf_tsk
 */
typedef struct td_itsk {
	RELTIM	stime;		/* Cumulative system execution time
				   (milliseconds) */
	RELTIM	utime;		/* Cumulative user execution time
				   (milliseconds) */
} TD_ITSK;

/*
 * System state information		td_ref_sys
 */
typedef struct td_rsys {
	INT	sysstat;	/* System state */
	ID	runtskid;	/* ID of task in execution state */
	ID	schedtskid;	/* ID of task that should be in
				   execution state */
} TD_RSYS;

/*
 * System call/extended SVC trace definition 	td_hok_svc
 */
typedef struct td_hsvc {
	FP	enter;		/* Hook routine before calling */
	FP	leave;		/* Hook routine after calling */
} TD_HSVC;

/*
 * Task dispatch trace definition		td_hok_dsp
 */
typedef struct td_hdsp {
	FP	exec;		/* Hook routine when starting execution */
	FP	stop;		/* Hook routine when stopping execution */
} TD_HDSP;

/*
 * Exception/Interrupt trace definition			td_hok_int
 */
typedef struct td_hint {
	FP	enter;		/* Hook routine before calling handler */
	FP	leave;		/* Hook routine after calling handler */
} TD_HINT;

/*
 * Domain information			td_dmi_xxx
 */
typedef struct td_dmi {
	ATR	domatr;		/* Domain attribute */
	ID	domid;		/* Domain ID */
	ID	kdmid;		/* Kernel domain ID */
	UB	oname[8];	/* Object name */
} TD_DMI;

/* ------------------------------------------------------------------------ */

/*
 * Definition for interface library automatic generation (mktdsvc)
 */
/*** DEFINE_TDSVC ***/

/* [BEGIN SYSCALLS] */

/* Refer each object usage state */
IMPORT INT td_lst_tsk( ID list[], INT nent );
IMPORT INT td_lst_sem( ID list[], INT nent );
IMPORT INT td_lst_flg( ID list[], INT nent );
IMPORT INT td_lst_mbx( ID list[], INT nent );
IMPORT INT td_lst_mtx( ID list[], INT nent );
IMPORT INT td_lst_mbf( ID list[], INT nent );
IMPORT INT td_lst_por( ID list[], INT nent );
IMPORT INT td_lst_mpf( ID list[], INT nent );
IMPORT INT td_lst_mpl( ID list[], INT nent );
IMPORT INT td_lst_cyc( ID list[], INT nent );
IMPORT INT td_lst_alm( ID list[], INT nent );
IMPORT INT td_lst_ssy( ID list[], INT nent );
IMPORT INT td_lst_dom( ID list[], INT nent );

/* Refer each object state */
IMPORT ER td_ref_sem( ID semid, TD_RSEM *rsem );
IMPORT ER td_ref_flg( ID flgid, TD_RFLG *rflg );
IMPORT ER td_ref_mbx( ID mbxid, TD_RMBX *rmbx );
IMPORT ER td_ref_mtx( ID mtxid, TD_RMTX *rmtx );
IMPORT ER td_ref_mbf( ID mbfid, TD_RMBF *rmbf );
IMPORT ER td_ref_por( ID porid, TD_RPOR *rpor );
IMPORT ER td_ref_mpf( ID mpfid, TD_RMPF *rmpf );
IMPORT ER td_ref_mpl( ID mplid, TD_RMPL *rmpl );
IMPORT ER td_ref_cyc( ID cycid, TD_RCYC *rcyc );
IMPORT ER td_ref_alm( ID almid, TD_RALM *ralm );
IMPORT ER td_ref_ssy( ID ssid, TD_RSSY *rssy );
IMPORT ER td_ref_dom( ID domid, TD_RDOM *rdom );

/* Refer task state */
IMPORT ER td_ref_tsk( ID tskid, TD_RTSK *rtsk );
IMPORT ER td_ref_tex( ID tskid, TD_RTEX *rtex );
IMPORT ER td_inf_tsk( ID tskid, TD_ITSK *itsk, BOOL clr );
IMPORT ER td_get_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );
IMPORT ER td_set_reg( ID tskid, T_REGS *regs, T_EIT *eit, T_CREGS *cregs );

/* Refer system state */
IMPORT ER td_ref_sys( TD_RSYS *rsys );
IMPORT ER td_get_tim( SYSTIM *tim, UINT *ofs );
IMPORT ER td_get_otm( SYSTIM *tim, UINT *ofs );

/* Refer ready queue */
IMPORT INT td_rdy_que( PRI pri, ID list[], INT nent );

/* Refer wait queue */
IMPORT INT td_sem_que( ID semid, ID list[], INT nent );
IMPORT INT td_flg_que( ID flgid, ID list[], INT nent );
IMPORT INT td_mbx_que( ID mbxid, ID list[], INT nent );
IMPORT INT td_mtx_que( ID mtxid, ID list[], INT nent );
IMPORT INT td_smbf_que( ID mbfid, ID list[], INT nent );
IMPORT INT td_rmbf_que( ID mbfid, ID list[], INT nent );
IMPORT INT td_cal_que( ID porid, ID list[], INT nent );
IMPORT INT td_acp_que( ID porid, ID list[], INT nent );
IMPORT INT td_mpf_que( ID mpfid, ID list[], INT nent );
IMPORT INT td_mpl_que( ID mplid, ID list[], INT nent );

/* Execution trace */
IMPORT ER td_hok_svc( TD_HSVC *hsvc );
IMPORT ER td_hok_dsp( TD_HDSP *hdsp );
IMPORT ER td_hok_int( TD_HINT *hint );

/* Object name */
IMPORT ER td_ref_dsname( UINT type, ID id, UB *dsname );
IMPORT ER td_set_dsname( UINT type, ID id, UB *dsname );

/* Refer domain information of each object */
IMPORT ER td_dmi_tsk( ID tskid, TD_DMI *rdmi );
IMPORT ER td_dmi_sem( ID semid, TD_DMI *rdmi );
IMPORT ER td_dmi_flg( ID flgid, TD_DMI *rdmi );
IMPORT ER td_dmi_mbx( ID mbxid, TD_DMI *rdmi );
IMPORT ER td_dmi_mtx( ID mtxid, TD_DMI *rdmi );
IMPORT ER td_dmi_mbf( ID mbfid, TD_DMI *rdmi );
IMPORT ER td_dmi_por( ID porid, TD_DMI *rdmi );
IMPORT ER td_dmi_mpf( ID mpfid, TD_DMI *rdmi );
IMPORT ER td_dmi_mpl( ID mplid, TD_DMI *rdmi );
IMPORT ER td_dmi_cyc( ID cycid, TD_DMI *rdmi );
IMPORT ER td_dmi_alm( ID almid, TD_DMI *rdmi );
IMPORT ER td_dmi_dom( ID domid, TD_DMI *rdmi );

/* Find ID of each object */
IMPORT ER td_fnd_tsk( ID domid, UB *oname );
IMPORT ER td_fnd_sem( ID domid, UB *oname );
IMPORT ER td_fnd_flg( ID domid, UB *oname );
IMPORT ER td_fnd_mbx( ID domid, UB *oname );
IMPORT ER td_fnd_mtx( ID domid, UB *oname );
IMPORT ER td_fnd_mbf( ID domid, UB *oname );
IMPORT ER td_fnd_por( ID domid, UB *oname );
IMPORT ER td_fnd_mpf( ID domid, UB *oname );
IMPORT ER td_fnd_mpl( ID domid, UB *oname );
IMPORT ER td_fnd_cyc( ID domid, UB *oname );
IMPORT ER td_fnd_alm( ID domid, UB *oname );
IMPORT ER td_fnd_dom( ID domid, UB *oname );

/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif
#endif /* __TK_DBGSPT_H__ */
