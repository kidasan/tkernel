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
 *	@(#)rominfo_depend.h (sys/SH7776)
 *
 *	ROM information 
 */

#ifndef __SYS_ROMINFO_DEPEND_H__
#define __SYS_ROMINFO_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ROM information 
 */
typedef struct {
	FP	kernel;		/* Kernel startup address */
	UB	*sysconf;	/* SYSCONF top */
	UB	*devconf;	/* DEVCONF top */
	VP	userarea;	/* RAM user area top */
	FP	userinit;	/* User initialization program address */
	FP      resetinit;      /* Reset initialization program address */
	UW	*ioconf;	/* IO configuration address */
	VW	rsv[9];		/* Reserved (always zero) */
	UW	rd_type;	/* ROM disk type */
	UW	rd_blksz;	/* ROM disk block size */
	UW	rd_saddr;	/* ROM disk start address */
	UW	rd_eaddr;	/* ROM disk end address */
	VW	rsv2[12];	/* Reserved (always zero) */
} RomInfo;

/* Start address of ROMInfo */
#define ROMInfo	( (RomInfo*)0x80010000 )


/* common tag in DEVCONF[] --> config/src/sysdepend/<TARGET>/DEVCONF */
#define DCTAG_DEBUGMODE	((UB*)"DEBUGMODE")	/* Debug mode (1:debug, 0:normal) */

/* common tag in SYSCONF[] --> config/src/sysdepend/<TARGET>/SYSCONF */
/* Product information */
#define SCTAG_TSYSNAME	((UB*)"TSysName")	/* System name */

#define SCTAG_MAKER	((UB*)"Maker")		/* Maker Code */
#define SCTAG_PRODUCTID	((UB*)"ProductID")	/* Kernel Identifier */
#define SCTAG_SPECVER	((UB*)"SpecVer")	/* Specification Version */
#define SCTAG_PRODUCTVER ((UB*)"ProductVer")	/* Product Version */
#define SCTAG_PRODUCTNO	((UB*)"ProductNo")	/* Product Number [0]-[3] */


/* T-Kernel/OS */
#define SCTAG_TMAXTSKID	((UB*)"TMaxTskId")	/* Maximum task ID */
#define SCTAG_TMAXSEMID	((UB*)"TMaxSemId")	/* Maximum semaphore ID */
#define SCTAG_TMAXFLGID	((UB*)"TMaxFlgId")	/* Maximum event flag ID */
#define SCTAG_TMAXMBXID	((UB*)"TMaxMbxId")	/* Maximum mail box ID */
#define SCTAG_TMAXMTXID	((UB*)"TMaxMtxId")	/* Maximum mutex ID */
#define SCTAG_TMAXMBFID	((UB*)"TMaxMbfId")	/* Maximum message buffer ID */
#define SCTAG_TMAXPORID	((UB*)"TMaxPorId")	/* Maximum rendezvous port ID */
#define SCTAG_TMAXMPFID	((UB*)"TMaxMpfId")	/* Maximum fixed size memory pool ID */
#define SCTAG_TMAXMPLID	((UB*)"TMaxMplId")	/* Maximum variable size memory pool ID */
#define SCTAG_TMAXCYCID	((UB*)"TMaxCycId")	/* Maximum cyclic handler ID */
#define SCTAG_TMAXALMID	((UB*)"TMaxAlmId")	/* Maximum alarm handler ID */
#define SCTAG_TMAXRESID	((UB*)"TMaxResId")	/* Maximum resource group ID */
#define SCTAG_TMAXSSYID	((UB*)"TMaxSsyId")	/* Maximum sub system ID */
#define SCTAG_TMAXSSYPRI ((UB*)"TMaxSsyPri")	/* Maximum sub system priority */

#define SCTAG_TSYSSTKSZ	((UB*)"TSysStkSz")	/* Default system stack size (byte) */
#define SCTAG_TSVCLIMIT	((UB*)"TSVCLimit")	/* SVC protection level */
#define SCTAG_TTIMPERIOD ((UB*)"TTimPeriod")	/* Timer interval (msec) */


/* SMP T-Kernel */
#define SCTAG_TMAXDOMID ((UB*)"TMaxDomId")	/* Maximum domain ID */
#define SCTAG_TOAPLIMIT ((UB*)"TOAPLimit")	/* The lowest protection level that protection is effective */
#define SCTAG_TSYSPRNUM ((UB*)"TSysPrNum")	/* Number of the processors of the whole system */
#define SCTAG_TKERPRNUM ((UB*)"TKerPrNum")	/* Number of the processors which a kernel controls */

/* T-Kernel/SM */				/* Maximum number of ... */
#define SCTAG_TMAXREGDEV ((UB*)"TMaxRegDev")	/* devices registration */
#define SCTAG_TMAXOPNDEV ((UB*)"TMaxOpnDev")	/* devices open */
#define SCTAG_TMAXREQDEV ((UB*)"TMaxReqDev")	/* device requests */

#define SCTAG_TDEVTMBFSZ ((UB*)"TDEvtMbfSz")	/* Event notification message */
						/* buffer size (byte), */
						/* Maximum length of message (byte) */


/* Task Event(1-8) */				/* Message management */
#define SCTAG_TEV_MSGEVT ((UB*)"TEV_MsgEvt")	/* Receive message */
#define SCTAG_TEV_MSGBRK ((UB*)"TEV_MsgBrk")	/* Release of an message waiting state */

#define SCTAG_TEV_GDI	 ((UB*)"TEV_GDI")	/* GDI interface */
#define SCTAG_TEV_FFLOCK ((UB*)"TEV_FFLock")	/* Release of an FIFO lock waiting state */


/* Segment manager */
#define SCTAG_REALMEMEND ((UB*)"RealMemEnd")	/* RAM bottom address (logical address) */


#ifdef __cplusplus
}
#endif
#endif /* __SYS_ROMINFO_DEPEND_H__ */
