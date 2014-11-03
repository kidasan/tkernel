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
 *	@(#)syscall.h (T-Kernel)
 *
 *	T-Kernel/OS (Common parts)
 */

#ifndef __TK_SYSCALL_H__
#define __TK_SYSCALL_H__

#ifndef _in_asm_source_

#include <basic.h>
#include <tk/typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

#endif /* _in_asm_source_ */

/* Task creation */
#define TSK_SELF	0		/* Its own task specify */
#define TPRI_INI	0		/* Specify priority at task startup */
#define TPRI_RUN	0		/* Specify highest priority during
					   running */

#define TA_ASM		0x00000000U	/* Program by assembler */
#define TA_HLNG		0x00000001U	/* Program by high level programming
					   language */
#define TA_SSTKSZ	0x00000002U	/* Set system stack size */
#define TA_USERSTACK	0x00000004U	/* Set user stack size */
#define TA_TASKSPACE	0x00000008U	/* Specify task space */
#define TA_RESID	0x00000010U	/* Specify resource group */
#define TA_DSNAME	0x00000040U	/* Use object name */

#define TA_RNG0		0x00000000U	/* Execute by protection level 0 */
#define TA_RNG1		0x00000100U	/* Execute by protection level 1 */
#define TA_RNG2		0x00000200U	/* Execute by protection level 2 */
#define TA_RNG3		0x00000300U	/* Execute by protection level 3 */

#define TA_COP0		0x00001000U	/* Use coprocessor (ID=0) */
#define TA_COP1		0x00002000U	/* Use coprocessor (ID=1) */
#define TA_COP2		0x00004000U	/* Use coprocessor (ID=2) */
#define TA_COP3		0x00008000U	/* Use coprocessor (ID=3) */

#define TA_ASSPRC	0x00020000U	/* Assign processor */

/* Assign processor */
#define TP_PRC1		0x00000001U	/* It operates by processor 1 */
#define TP_PRC2		0x00000002U	/* It operates by processor 2 */
#define TP_PRC3		0x00000004U	/* It operates by processor 3 */
#define TP_PRC4		0x00000008U	/* It operates by processor 4 */
#define TP_PRC5		0x00000010U	/* It operates by processor 5 */
#define TP_PRC6		0x00000020U	/* It operates by processor 6 */
#define TP_PRC7		0x00000040U	/* It operates by processor 7 */
#define TP_PRC8		0x00000080U	/* It operates by processor 8 */
#define TP_PRC9		0x00000100U	/* It operates by processor 9 */
#define TP_PRC10	0x00000200U	/* It operates by processor 10 */
#define TP_PRC11	0x00000400U	/* It operates by processor 11 */
#define TP_PRC12	0x00000800U	/* It operates by processor 12 */
#define TP_PRC13	0x00001000U	/* It operates by processor 13 */
#define TP_PRC14	0x00002000U	/* It operates by processor 14 */
#define TP_PRC15	0x00004000U	/* It operates by processor 15 */
#define TP_PRC16	0x00008000U	/* It operates by processor 16 */
#define TP_PRC17	0x00010000U	/* It operates by processor 17 */
#define TP_PRC18	0x00020000U	/* It operates by processor 18 */
#define TP_PRC19	0x00040000U	/* It operates by processor 19 */
#define TP_PRC20	0x00080000U	/* It operates by processor 20 */
#define TP_PRC21	0x00100000U	/* It operates by processor 21 */
#define TP_PRC22	0x00200000U	/* It operates by processor 22 */
#define TP_PRC23	0x00400000U	/* It operates by processor 23 */
#define TP_PRC24	0x00800000U	/* It operates by processor 24 */
#define TP_PRC25	0x01000000U	/* It operates by processor 25 */
#define TP_PRC26	0x02000000U	/* It operates by processor 26 */
#define TP_PRC27	0x04000000U	/* It operates by processor 27 */
#define TP_PRC28	0x08000000U	/* It operates by processor 28 */
#define TP_PRC29	0x10000000U	/* It operates by processor 29 */
#define TP_PRC30	0x20000000U	/* It operates by processor 30 */
#define TP_PRC31	0x40000000U	/* It operates by processor 31 */
#define TP_PRC32	0x80000000U	/* It operates by processor 32 */

/* Task state tskstat */
#define TTS_RUN		0x00000001U	/* RUN */
#define TTS_RDY		0x00000002U	/* READY */
#define TTS_WAI		0x00000004U	/* WAIT */
#define TTS_SUS		0x00000008U	/* SUSPEND */
#define TTS_WAS		0x0000000cU	/* WAIT-SUSPEND */
#define TTS_DMT		0x00000010U	/* DORMANT */
#define TTS_NODISWAI	0x00000080U	/* Wait disable rejection state */

/* Wait factor tskwait */
#define TTW_SLP		0x00000001U	/* Wait caused by wakeup wait */
#define TTW_DLY		0x00000002U	/* Wait caused by task delay */
#define TTW_SEM		0x00000004U	/* Semaphore wait */
#define TTW_FLG		0x00000008U	/* Event flag wait */
#define TTW_MBX		0x00000040U	/* Mail box wait */
#define TTW_MTX		0x00000080U	/* Mutex wait */
#define TTW_SMBF	0x00000100U	/* Message buffer send wait */
#define TTW_RMBF	0x00000200U	/* Message buffer receive wait */
#define TTW_CAL		0x00000400U	/* Rendezvous call wait */
#define TTW_ACP		0x00000800U	/* Rendezvous accept wait */
#define TTW_RDV		0x00001000U	/* Rendezvous end wait */
#define TTW_MPF		0x00002000U	/* Fixed size memory pool wait */
#define TTW_MPL		0x00004000U	/* Variable size memory pool wait */
#define TTW_EV1		0x00010000U	/* Task event # 1 wait */
#define TTW_EV2		0x00020000U	/* Task event # 2 wait */
#define TTW_EV3		0x00040000U	/* Task event # 3 wait */
#define TTW_EV4		0x00080000U	/* Task event # 4 wait */
#define TTW_EV5		0x00100000U	/* Task event # 5 wait */
#define TTW_EV6		0x00200000U	/* Task event # 6 wait */
#define TTW_EV7		0x00400000U	/* Task event # 7 wait */
#define TTW_EV8		0x00800000U	/* Task event # 8 wait */

#define TTX_SVC		0x80000000U	/* Extended SVC call disable
					   (tk_dis_wai) */

/* Semaphore generation */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_FIRST	0x00000000U	/* Give priority to task at head of
					   wait queue */
#define TA_CNT		0x00000002U	/* Give priority to task whose
					   request counts is less */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */

/* Mutex */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_INHERIT	0x00000002U	/* Priority inherited protocol */
#define TA_CEILING	0x00000003U	/* Upper limit priority protocol */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */

/* Event flag */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_WSGL		0x00000000U	/* Disable multiple tasks wait */
#define TA_WMUL		0x00000008U	/* Enable multiple tasks wait */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */

/* Event flag wait mode */
#define TWF_ANDW	0x00000000U	/* AND wait */
#define TWF_ORW		0x00000001U	/* OR wait */
#define TWF_CLR		0x00000010U	/* All clear specify */
#define TWF_BITCLR	0x00000020U	/* Only condition bit clear specify */

/* Mail box */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_MFIFO	0x00000000U	/* Manage messages by FIFO */
#define TA_MPRI		0x00000002U	/* Manage messages by priority
					   order */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */

/* Message buffer */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */

/* Rendezvous */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */

/* Handler */
#define TA_ASM		0x00000000U	/* Program by assembler */
#define TA_HLNG		0x00000001U	/* Program by high level programming
					   language */
#define TA_ASSPRC	0x00020000U	/* Assign processor */

/* Variable size memory pool */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_NORESIDENT	0x00000010U	/* Non resident */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */
#define TA_RNG0		0x00000000U	/* Protection level 0 */
#define TA_RNG1		0x00000100U	/* Protection level 1 */
#define TA_RNG2		0x00000200U	/* Protection level 2 */
#define TA_RNG3		0x00000300U	/* Protection level 3 */

/* Fixed size memory pool */
#define TA_TFIFO	0x00000000U	/* Manage wait task by FIFO */
#define TA_TPRI		0x00000001U	/* Manage wait task by priority
					   order */
#define TA_NORESIDENT	0x00000010U	/* Non-resident */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_NODISWAI	0x00000080U	/* Wait disable rejection */
#define TA_RNG0		0x00000000U	/* Protection level 0 */
#define TA_RNG1		0x00000100U	/* Protection level 1 */
#define TA_RNG2		0x00000200U	/* Protection level 2 */
#define TA_RNG3		0x00000300U	/* Protection level 3 */

/* Cycle handler */
#define TA_ASM		0x00000000U	/* Program by assembler */
#define TA_HLNG		0x00000001U	/* Program by high level programming
					   language */
#define TA_STA		0x00000002U	/* Cycle handler startup */
#define TA_PHS		0x00000004U	/* Save cycle handler phase */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_ASSPRC	0x00020000U	/* Assign processor */

#define TCYC_STP	0x00U		/* Cycle handler is not operating */
#define TCYC_STA	0x01U		/* Cycle handler is operating */

/* Alarm handler address */
#define TA_ASM		0x00000000U	/* Program by assembler */
#define TA_HLNG		0x00000001U	/* Program by high level programming
					   language */
#define TA_DSNAME	0x00000040U	/* Use object name */
#define TA_ASSPRC	0x00020000U	/* Assign processor */

#define TALM_STP	0x00U		/* Alarm handler is not operating */
#define TALM_STA	0x01U		/* Alarm handler is operating */

/* System state */
#define TSS_TSK		0x00U	/* During execution of task part(context) */
#define TSS_DDSP	0x01U	/* During dispatch disable */
#define TSS_DINT	0x02U	/* During Interrupt disable */
#define TSS_INDP	0x04U	/* During execution of task independent part */
#define TSS_QTSK	0x08U	/* During execution of semi-task part */

/* Power-saving mode */
#define TPW_DOSUSPEND	1	/* Transit to suspend state */
#define TPW_DISLOWPOW	2	/* Power-saving mode switch disable */
#define TPW_ENALOWPOW	3	/* Power-saving mode switch enable */

/* Domain */
#define TA_DOMID		0x00010000U	/* Specify domain ID */
#define TA_ONAME		0x00000040U	/* Use object name */
#define OBJECT_NAME_LENGTH	8		/* Object name length in each control block */
#define PRC_SELF		0		/* Its own processor specify */

/* Protection */
#define TA_PRIVATE		0x00040000U	/* Protection attribute is private */
#define TA_PROTECTED		0x00080000U	/* Protection attribute is protected */
#define TA_PUBLIC		0x00000000U	/* Protection attribute is public */

#ifndef _in_asm_source_

#ifdef __cplusplus
}
#endif

#endif /* _in_asm_source_ */

/* System dependencies */
#include <tk/sysdepend/syscall_common.h>

#ifndef _in_asm_source_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Task creation information 		tk_cre_tsk
 */
typedef struct t_ctsk {
	VP	exinf;		/* Extended information */
	ATR	tskatr;		/* Task attribute */
	FP	task;		/* Task startup address */
	PRI	itskpri;	/* Priority at task startup */
	INT	stksz;		/* User stack size (byte) */
	INT	sstksz;		/* System stack size (byte) */
	VP	stkptr;		/* User stack pointer */
	VP	uatb;		/* Task space page table */
	INT	lsid;		/* Logical space ID */
	ID	resid;		/* Resource ID */
	ID	domid;		/* Domain ID */
	UW	assprc;		/* Assign processor */
	UB	oname[8];	/* Object name */
#if TA_GP
	VP	gp;		/* Global pointer (gp) */
#endif
} T_CTSK;

/*
 * Task space information		tk_get_tsp, tk_set_tsp
 */
typedef struct t_tskspc {
	VP	uatb;		/* Address of task space page table */
	INT	lsid;		/* Logical space ID */
} T_TSKSPC;

/*
 * Task statistics information		tk_inf_tsk
 */
typedef struct t_itsk {
	RELTIM	stime;		/* Cumulative system execution time
				   (milliseconds) */
	RELTIM	utime;		/* Cumulative user execution time
				   (milliseconds) */
} T_ITSK;

/*
 * Task state information 		tk_ref_tsk
 */
typedef	struct t_rtsk {
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
} T_RTSK;

/*
 * Task exception handler definition information	tk_def_tex
 */
typedef struct t_dtex {
	ATR	texatr;		/* Task exception handler attribute */
	FP	texhdr;		/* Task exception handler address */
} T_DTEX;

/*
 * Task exception state information		tk_ref_tex
 */
typedef struct t_rtex {
	UINT	pendtex;	/* Occurring task exception */
	UINT	texmask;	/* Enabled task exception */
} T_RTEX;

/*
 * Semaphore creation information		tk_cre_sem
 */
typedef	struct t_csem {
	VP	exinf;		/* Extended information */
	ATR	sematr;		/* Semaphore attribute */
	INT	isemcnt;	/* Semaphore initial count value */
	INT	maxsem;		/* Semaphore maximum count value */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CSEM;

/*
 * Semaphore state information		tk_ref_sem
 */
typedef	struct t_rsem {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	semcnt;		/* Current semaphore value */
} T_RSEM;

/*
 * Mutex creation information		tk_cre_mtx
 */
typedef struct t_cmtx {
	VP	exinf;		/* Extended information */
	ATR	mtxatr;		/* Mutex attribute */
	PRI	ceilpri;	/* Upper limit priority of mutex */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CMTX;

/*
 * Mutex state information		tk_ref_mtx
 */
typedef struct t_rmtx {
	VP	exinf;		/* Extended information */
	ID	htsk;		/* Locking task ID */
	ID	wtsk;		/* Lock wait task ID */
} T_RMTX;

/*
 * Event flag creation information	tk_cre_flg
 */
typedef	struct t_cflg {
	VP	exinf;		/* Extended information */
	ATR	flgatr;		/* Event flag attribute */
	UINT	iflgptn;	/* Event flag initial value */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CFLG;

/*
 * Event flag state information		tk_ref_flg
 */
typedef	struct t_rflg {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	UINT	flgptn;		/* Current event flag pattern */
} T_RFLG;

/*
 * Mail box creation information	tk_cre_mbx
 */
typedef	struct t_cmbx {
	VP	exinf;		/* Extended information */
	ATR	mbxatr;		/* Mail box attribute */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CMBX;

/*
 * Mail box message header
 */
typedef struct t_msg {
	VP	msgque[1];	/* Area for message queue */
} T_MSG;

typedef struct t_msg_pri {
	T_MSG	msgque;		/* Area for message queue */
	PRI	msgpri;		/* Message priority */
} T_MSG_PRI;

/*
 * Mail box state information		tk_ref_mbx
 */
typedef	struct t_rmbx {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	T_MSG	*pk_msg;	/* Next received message */
} T_RMBX;

/*
 * Message buffer creation information	tk_cre_mbf
 */
typedef	struct t_cmbf {
	VP	exinf;		/* Extended information */
	ATR	mbfatr;		/* Message buffer attribute */
	INT	bufsz;		/* Message buffer size (byte) */
	INT	maxmsz;		/* Maximum length of message (byte) */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CMBF;

/*
 * Message buffer state information 	tk_ref_mbf
 */
typedef struct t_rmbf {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Receive wait task ID */
	ID	stsk;		/* Send wait task ID */
	INT	msgsz;		/* Next received message size (byte) */
	INT	frbufsz;	/* Free buffer size (byte) */
	INT	maxmsz;		/* Maximum length of message (byte) */
} T_RMBF;

/*
 * Rendezvous port creation information	tk_cre_por
 */
typedef	struct t_cpor {
	VP	exinf;		/* Extended information */
	ATR	poratr;		/* Port attribute */
	INT	maxcmsz;	/* Maximum length of call message (byte) */
	INT	maxrmsz;	/* Maximum length of replay message (byte) */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CPOR;

/*
 * Rendezvous port state information	tk_ref_por
 */
typedef struct t_rpor {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Call wait task ID */
	ID	atsk;		/* Receive wait task ID */
	INT	maxcmsz;	/* Maximum length of call message (byte) */
	INT	maxrmsz;	/* Maximum length of replay message (byte) */
} T_RPOR;

/*
 * Interrupt handler definition information	tk_def_int
 */
typedef struct t_dint {
	ATR	intatr;		/* Interrupt handler attribute */
	FP	inthdr;		/* Interrupt handler address */
	UW      assprc;		/* AssignProcessor */
#if TA_GP
	VP	gp;		/* Global pointer (gp) */
#endif
} T_DINT;

/*
 * Variable size memory pool creation information	tk_cre_mpl
 */
typedef	struct t_cmpl {
	VP	exinf;		/* Extended information */
	ATR	mplatr;		/* Memory pool attribute */
	INT	mplsz;		/* Size of whole memory pool (byte) */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CMPL;

/*
 * Variable size memory pool state information	tk_ref_mpl
 */
typedef struct t_rmpl {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	frsz;		/* Total size of free area (byte) */
	INT	maxsz;		/* Size of maximum continuous free area
				   (byte) */
} T_RMPL;

/*
 * Fixed size memory pool state information	tk_cre_mpf
 */
typedef	struct t_cmpf {
	VP	exinf;		/* Extended information */
	ATR	mpfatr;		/* Memory pool attribute */
	INT	mpfcnt;		/* Number of blocks in whole memory pool */
	INT	blfsz;		/* Fixed size memory block size (byte) */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CMPF;

/*
 * Fixed size memory pool state information	tk_ref_mpf
 */
typedef struct t_rmpf {
	VP	exinf;		/* Extended information */
	ID	wtsk;		/* Wait task ID */
	INT	frbcnt;		/* Number of free area blocks */
} T_RMPF;

/*
 * Cycle handler creation information 	tk_cre_cyc
 */
typedef struct t_ccyc {
	VP	exinf;		/* Extended information */
	ATR	cycatr;		/* Cycle handler attribute */
	FP	cychdr;		/* Cycle handler address */
	RELTIM	cyctim;		/* Cycle interval */
	RELTIM	cycphs;		/* Cycle phase */
	ID	domid;		/* Domain ID */
	UW      assprc;		/* AssignProcessor */
	UB	oname[8];	/* Object name */
#if TA_GP
	VP	gp;		/* Global pointer (gp) */
#endif
} T_CCYC;

/*
 * Cycle handler state information	tk_ref_cyc
 */
typedef struct t_rcyc {
	VP	exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until next handler startup */
	UINT	cycstat;	/* Cycle handler status */
} T_RCYC;

/*
 * Alarm handler creation information		tk_cre_alm
 */
typedef struct t_calm {
	VP	exinf;		/* Extended information */
	ATR	almatr;		/* Alarm handler attribute */
	FP	almhdr;		/* Alarm handler address */
	ID	domid;		/* Domain ID */
	UW      assprc;		/* AssignProcessor */
	UB	oname[8];	/* Object name */
#if TA_GP
	VP	gp;		/* Global pointer (gp) */
#endif
} T_CALM;

/*
 * Alarm handler state information	tk_ref_alm
 */
typedef struct t_ralm {
	VP	exinf;		/* Extended information */
	RELTIM	lfttim;		/* Remaining time until handler startup */
	UINT	almstat;	/* Alarm handler state */
} T_RALM;

/*
 * Version information		tk_ref_ver
 */
typedef struct t_rver {
	UH	maker;		/* OS manufacturer */
	UH	prid;		/* OS identification number */
	UH	spver;		/* Specification version */
	UH	prver;		/* OS product version */
	UH	prno[4];	/* Product number, Product management
				   information */
} T_RVER;

/*
 * System state information		tk_ref_sys
 */
typedef struct t_rsys {
	INT	sysstat;	/* System state */
	ID	runtskid;	/* ID of task in execution state */
	ID	schedtskid;	/* ID of the task that should be the
				   execution state */
} T_RSYS;

/*
 * Subsystem definition information 		tk_def_ssy
 */
typedef struct t_dssy {
	ATR	ssyatr;		/* Subsystem attribute */
	PRI	ssypri;		/* Subsystem priority */
	FP	svchdr;		/* Extended SVC handler address */
	FP	breakfn;	/* Break function address */
	FP	startupfn;	/* Startup function address */
	FP	cleanupfn;	/* Cleanup function address */
	FP	eventfn;	/* Event function address */
	INT	resblksz;	/* Resource management block size (byte) */
#if TA_GP
	VP	gp;		/* Global pointer (gp) */
#endif
} T_DSSY;

/*
 * Subsystem state information		tk_ref_ssy
 */
typedef struct t_rssy {
	PRI	ssypri;		/* Subsystem priority */
	INT	resblksz;	/* Resource management block size (byte) */
} T_RSSY;

/* For SMP T-Kernel */
/*
 * Domain creation information	tk_cre_dom
 */
typedef struct t_cdom {
	VP	exinf;		/* Extended information */
	ATR	domatr;		/* Domain attribute */
	ID	domid;		/* Domain ID */
	UB	oname[8];	/* Object name */
} T_CDOM;

/*
 * Domain state information	tk_ref_dom
 */
typedef struct t_rdom {
	VP	exinf;		/* Extended information */
	ID	domid;		/* Domain ID */
	UINT	objcnt;		/* Object count */
} T_RDOM;

/*
 * Domain information		tk_dmi_xxx
 */
typedef struct t_dmi {
	ATR	domatr;		/* Domain attribute */
	ID	domid;		/* Domain ID */
	ID	kdmid;		/* Kernel domain ID */
	UB	oname[8];	/* Object name */
} T_DMI;

/* ------------------------------------------------------------------------ */
/*
 * Definition for interface library automatic generation (mktksvc)
 */
/*** DEFINE_TKSVC ***/

/* [BEGIN SYSCALLS] */
IMPORT ID tk_cre_tsk( T_CTSK *pk_ctsk );
IMPORT ER tk_del_tsk( ID tskid );
IMPORT ER tk_sta_tsk( ID tskid, INT stacd );
IMPORT void tk_ext_tsk( void );
IMPORT void tk_exd_tsk( void );
IMPORT ER tk_ter_tsk( ID tskid );
IMPORT ER tk_dis_dsp( void );
IMPORT ER tk_ena_dsp( void );
IMPORT ER tk_chg_pri( ID tskid, PRI tskpri );
IMPORT ER tk_chg_slt( ID tskid, RELTIM slicetime );
IMPORT ER tk_rot_rdq( PRI tskpri );
IMPORT ER tk_rel_wai( ID tskid );
IMPORT ID tk_get_tid( void );
IMPORT ER tk_get_tsp( ID tskid, T_TSKSPC *pk_tskspc );
IMPORT ER tk_set_tsp( ID tskid, T_TSKSPC *pk_tskspc );
IMPORT ID tk_get_rid( ID tskid );
IMPORT ID tk_set_rid( ID tskid, ID resid );
IMPORT ER tk_get_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );
IMPORT ER tk_set_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );
IMPORT ER tk_get_cpr( ID tskid, INT copno, T_COPREGS *pk_copregs );
IMPORT ER tk_set_cpr( ID tskid, INT copno, T_COPREGS *pk_copregs );
IMPORT ER tk_inf_tsk( ID tskid, T_ITSK *pk_itsk, BOOL clr );
IMPORT ER tk_ref_tsk( ID tskid, T_RTSK *pk_rtsk );
IMPORT ER tk_def_tex( ID tskid, T_DTEX *pk_dtex );
IMPORT ER tk_ena_tex( ID tskid, UINT texptn );
IMPORT ER tk_dis_tex( ID tskid, UINT texptn );
IMPORT ER tk_ras_tex( ID tskid, INT texcd );
IMPORT INT tk_end_tex( BOOL enatex );
IMPORT ER tk_ref_tex( ID tskid, T_RTEX *pk_rtex );
IMPORT ER tk_sus_tsk( ID tskid );
IMPORT ER tk_rsm_tsk( ID tskid );
IMPORT ER tk_frsm_tsk( ID tskid );
IMPORT ER tk_slp_tsk( TMO tmout );
IMPORT ER tk_wup_tsk( ID tskid );
IMPORT INT tk_can_wup( ID tskid );
IMPORT ER tk_sig_tev( ID tskid, INT tskevt );
IMPORT INT tk_wai_tev( INT waiptn, TMO tmout );
IMPORT INT tk_dis_wai( ID tskid, UINT waitmask );
IMPORT ER tk_ena_wai( ID tskid );
IMPORT ID tk_cre_sem( T_CSEM *pk_csem );
IMPORT ER tk_del_sem( ID semid );
IMPORT ER tk_sig_sem( ID semid, INT cnt );
IMPORT ER tk_wai_sem( ID semid, INT cnt, TMO tmout );
IMPORT ER tk_ref_sem( ID semid, T_RSEM *pk_rsem );
IMPORT ID tk_cre_mtx( T_CMTX *pk_cmtx );
IMPORT ER tk_del_mtx( ID mtxid );
IMPORT ER tk_loc_mtx( ID mtxid, TMO tmout );
IMPORT ER tk_unl_mtx( ID mtxid );
IMPORT ER tk_ref_mtx( ID mtxid, T_RMTX *pk_rmtx );
IMPORT ID tk_cre_flg( T_CFLG *pk_cflg );
IMPORT ER tk_del_flg( ID flgid );
IMPORT ER tk_set_flg( ID flgid, UINT setptn );
IMPORT ER tk_clr_flg( ID flgid, UINT clrptn );
IMPORT ER tk_wai_flg( ID flgid, UINT waiptn, UINT wfmode, UINT *p_flgptn, TMO tmout );
IMPORT ER tk_ref_flg( ID flgid, T_RFLG *pk_rflg );
IMPORT ID tk_cre_mbx( T_CMBX* pk_cmbx );
IMPORT ER tk_del_mbx( ID mbxid );
IMPORT ER tk_snd_mbx( ID mbxid, T_MSG *pk_msg );
IMPORT ER tk_rcv_mbx( ID mbxid, T_MSG **ppk_msg, TMO tmout );
IMPORT ER tk_ref_mbx( ID mbxid, T_RMBX *pk_rmbx );
IMPORT ID tk_cre_mbf( T_CMBF *pk_cmbf );
IMPORT ER tk_del_mbf( ID mbfid );
IMPORT ER tk_snd_mbf( ID mbfid, VP msg, INT msgsz, TMO tmout );
IMPORT INT tk_rcv_mbf( ID mbfid, VP msg, TMO tmout );
IMPORT ER tk_ref_mbf( ID mbfid, T_RMBF *pk_rmbf );
IMPORT ID tk_cre_por( T_CPOR *pk_cpor );
IMPORT ER tk_del_por( ID porid );
IMPORT INT tk_cal_por( ID porid, UINT calptn, VP msg, INT cmsgsz, TMO tmout );
IMPORT INT tk_acp_por( ID porid, UINT acpptn, RNO *p_rdvno, VP msg, TMO tmout );
IMPORT ER tk_fwd_por( ID porid, UINT calptn, RNO rdvno, VP msg, INT cmsgsz );
IMPORT ER tk_rpl_rdv( RNO rdvno, VP msg, INT rmsgsz );
IMPORT ER tk_ref_por( ID porid, T_RPOR *pk_rpor );
IMPORT ER tk_def_int( UINT dintno, T_DINT *pk_dint );
IMPORT void tk_ret_int( void );
IMPORT ID tk_cre_mpl( T_CMPL *pk_cmpl );
IMPORT ER tk_del_mpl( ID mplid );
IMPORT ER tk_get_mpl( ID mplid, INT blksz, VP *p_blk, TMO tmout );
IMPORT ER tk_rel_mpl( ID mplid, VP blk );
IMPORT ER tk_ref_mpl( ID mplid, T_RMPL *pk_rmpl );
IMPORT ID tk_cre_mpf( T_CMPF *pk_cmpf );
IMPORT ER tk_del_mpf( ID mpfid );
IMPORT ER tk_get_mpf( ID mpfid, VP *p_blf, TMO tmout );
IMPORT ER tk_rel_mpf( ID mpfid, VP blf );
IMPORT ER tk_ref_mpf( ID mpfid, T_RMPF *pk_rmpf );
IMPORT ER tk_set_tim( SYSTIM *pk_tim );
IMPORT ER tk_get_tim( SYSTIM *pk_tim );
IMPORT ER tk_get_otm( SYSTIM *pk_tim );
IMPORT ER tk_dly_tsk( RELTIM dlytim );
IMPORT ID tk_cre_cyc( T_CCYC *pk_ccyc );
IMPORT ER tk_del_cyc( ID cycid );
IMPORT ER tk_sta_cyc( ID cycid );
IMPORT ER tk_stp_cyc( ID cycid );
IMPORT ER tk_ref_cyc( ID cycid, T_RCYC *pk_rcyc );
IMPORT ID tk_cre_alm( T_CALM *pk_calm );
IMPORT ER tk_del_alm( ID almid );
IMPORT ER tk_sta_alm( ID almid, RELTIM almtim );
IMPORT ER tk_stp_alm( ID almid );
IMPORT ER tk_ref_alm( ID almid, T_RALM *pk_ralm );
IMPORT ER tk_ref_ver( T_RVER *pk_rver );
IMPORT ER tk_ref_sys( T_RSYS *pk_rsys );
IMPORT ER tk_def_ssy( ID ssid, T_DSSY *pk_dssy );
IMPORT ER tk_sta_ssy( ID ssid, ID resid, INT info );
IMPORT ER tk_cln_ssy( ID ssid, ID resid, INT info );
IMPORT ER tk_evt_ssy( ID ssid, INT evttyp, ID resid, INT info );
IMPORT ER tk_ref_ssy( ID ssid, T_RSSY *pk_rssy );
IMPORT ID tk_cre_res( void );
IMPORT ER tk_del_res( ID resid );
IMPORT ER tk_get_res( ID resid, ID ssid, VP *p_resblk );
IMPORT ER tk_set_pow( UINT powmode );
/* For SMP T-Kernel */
IMPORT ER tk_cre_dom( T_CDOM *pk_cdom );
IMPORT ER tk_del_dom( ID domid );
IMPORT ER tk_fdel_dom( ID domid );
IMPORT ID tk_fnd_dom( ID domid, UB *oname );
IMPORT ER tk_ref_dom( ID domid, T_RDOM *pk_rdom );
IMPORT ID tk_get_kdm( ID prcid );
IMPORT ER tk_dmi_dom( ID domid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_tsk( ID domid, UB *oname );
IMPORT ER tk_dmi_tsk( ID tskid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_sem( ID domid, UB *oname );
IMPORT ER tk_dmi_sem( ID semid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_flg( ID domid, UB *oname );
IMPORT ER tk_dmi_flg( ID flgid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_mbx( ID domid, UB *oname );
IMPORT ER tk_dmi_mbx( ID mbxid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_mtx( ID domid, UB *oname );
IMPORT ER tk_dmi_mtx( ID mtxid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_mbf( ID domid, UB *oname );
IMPORT ER tk_dmi_mbf( ID mbfid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_por( ID domid, UB *oname );
IMPORT ER tk_dmi_por( ID porid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_mpf( ID domid, UB *oname );
IMPORT ER tk_dmi_mpf( ID mpfid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_mpl( ID domid, UB *oname );
IMPORT ER tk_dmi_mpl( ID mplid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_alm( ID domid, UB *oname );
IMPORT ER tk_dmi_alm( ID mplid, T_DMI *pk_dmi );
IMPORT ID tk_fnd_cyc( ID domid, UB *oname );
IMPORT ER tk_dmi_cyc( ID mplid, T_DMI *pk_dmi );
IMPORT ID tk_get_prc( void );
/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif

#endif /* _in_asm_source_ */

#endif /* __TK_SYSCALL_H__ */
