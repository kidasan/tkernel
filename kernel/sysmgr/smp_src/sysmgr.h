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
 *	sysmgr.h (T-Kernel/SM)
 *	T-Kernel/System Manager
 */

#ifndef _SYSMGR_
#define _SYSMGR_

#include <basic.h>
#include <tk/tkernel.h>
#include <libstr.h>
#include <sys/queue.h>
#include <tk/util.h>
#include <sys/debug.h>

/* ------------------------------------------------------------------------ */
/*
 *	Device management function
 */

/*
 * Lock for device management exclusive control
 */
IMPORT FastMLock	DevMgrLock;
#define LockDM()	MLock(&DevMgrLock, 0)
#define UnlockDM()	MUnlock(&DevMgrLock, 0)

/*
 * Lock for device registration exclusive control
 */
#define LockREG()	MLock(&DevMgrLock, 1)
#define UnlockREG()	MUnlock(&DevMgrLock, 1)

/*
 * Device management synchronous control 
 *	Enter using semaphore, but free wait by 'tk_dis_wai()'
 *	because it frees wait by specifying the specific task.
 *	Do not execute 'tk_sig_sem().'
 */
IMPORT ID	DevMgrSync;
#define SyncWaitDM()	{			\
	tk_wai_sem(DevMgrSync, 1, TMO_FEVR);	\
	tk_ena_wai(TSK_SELF);			\
}
#define SyncSignalDM(tskid)	tk_dis_wai(tskid, TTW_SEM)

/*
 * Device registration information
 */
typedef struct DeviceControlBlock {
	QUEUE	q;
	UB	devnm[L_DEVNM+1];	/* Device name */
	T_DDEV	ddev;			/* Registration information */
	QUEUE	openq;			/* Open device management queue */
} DevCB;

IMPORT	DevCB		*DevCBtbl;	/* Device registration information
					   table */
IMPORT	QUEUE		UsedDevCB;	/* In-use queue */

#define DID(devcb)		( ((devcb) - DevCBtbl + 1) << 8 )
#define DEVID(devcb, unitno)	( DID(devcb) + (unitno) )
#define DEVCB(devid)		( DevCBtbl + (((devid) >> 8) - 1) )
#define UNITNO(devid)		( (devid) & 0xff )

/*
 * Open management information
 */
typedef struct OpenControlBlock {
	QUEUE		q;
	QUEUE		resq;		/* For connection from resource
					   management */
	ID		resid;		/* Section resource ID */
	DevCB		*devcb;		/* Target device */
	INT		unitno;		/* Subunit number
					   (0: Physical device) */
	UINT		omode;		/* Open mode */
	QUEUE		requestq;	/* Request management queue */
	UH		waitone;	/* Number of individual request
					   waits */
	T_DEVREQ	*waireqlst;	/* List of multiple request waits */
	INT		nwaireq;	/* Number of multiple request waits */
	ID		abort_tskid;	/* Abort completion wait task */
	INT		abort_cnt;	/* Number of abort completion wait
					   requests */
} OpnCB;

#define RESQ_OPNCB(rq)		( (OpnCB*)((B*)(rq) - offsetof(OpnCB, resq)) )

/*
 * Request management information
 */
typedef struct RequestControlBlock {
	QUEUE		q;
	OpnCB		*opncb;		/* Open device */
	ID		tskid;		/* Processing task */
	T_DEVREQ	req;		/* Request packet */
} ReqCB;

/*
 * Resource management information
 */
typedef struct ResourceControlBlock {
	QUEUE		openq;		/* Open device management queue */
	INT		dissus;		/* Suspend disable request count */
} ResCB;

/*
 * Request function types
 */

typedef ER  (*OPNFN)( ID devid, UINT omode, VP exinf );
typedef ER  (*ABTFN)( ID tskid, T_DEVREQ *devreq, INT nreq, VP exinf );
typedef INT (*WAIFN)( T_DEVREQ *devreq, INT nreq, TMO tmout, VP exinf );
typedef INT (*EVTFN)( INT evttyp, VP evtinf, VP exinf );
typedef ER  (*CLSFN)( ID devid, UINT option, VP exinf );
typedef ER  (*EXCFN)( T_DEVREQ *devreq, TMO tmout, VP exinf );

/* ------------------------------------------------------------------------ */

#if TA_GP
/*
 * Device driver call 
 */
IMPORT INT _CallDeviceDriver( INT p1, INT p2, INT p3, INT p4, FP drv, VP gp );
#define CallDeviceDriver(p1, p2, p3, p4, drv, gp ) \
		_CallDeviceDriver((INT)(p1), (INT)(p2), (INT)(p3), (INT)(p4), \
							(FP)(drv), (gp))
#endif

IMPORT void FlushCache( VP laddr, INT len );
IMPORT void FlushCacheM( VP laddr, INT len, UINT mode );
IMPORT ER ControlCacheM( VP laddr, INT len, UINT mode );
IMPORT INT GetCacheLineSize( void );

#define IMPORT_DEFINE	1
#if IMPORT_DEFINE
/* main.c */
IMPORT VP GetResBlk( ID ssid, ID tskid );
IMPORT ER SystemManager( INT ac, UB *av[] );
/* system.c */
IMPORT INT _tk_get_cfn( UB *name, INT *val, INT max );
IMPORT INT _tk_get_cfs( UB *name, UB *buf, INT max );
IMPORT ER initialize_sysmgr( void );
IMPORT ER finish_sysmgr( void );
/* imalloc.c */
IMPORT void* IAmalloc( size_t size, UINT attr );
IMPORT void* IAcalloc( size_t nmemb, size_t size, UINT attr );
IMPORT void  IAfree( void *ptr, UINT attr );
IMPORT void* Imalloc( size_t size );
IMPORT void* Icalloc( size_t nmemb, size_t size );
IMPORT void  Ifree( void *ptr );
IMPORT ER init_Imalloc( void );
/* syslog.c */
IMPORT	W	logtask_alive ;
IMPORT ER __syslog_send( const char *string, int len );
IMPORT ER initialize_syslog( void );
IMPORT ER finish_syslog( void );
/* device.c */
IMPORT	FastMLock	DevMgrLock;
IMPORT	ID		DevMgrSync;
IMPORT	DevCB		*DevCBtbl;
IMPORT	QUEUE		UsedDevCB;
IMPORT DevCB* searchDevCB( UB *devnm );
IMPORT INT phydevnm( UB *pdevnm, UB *ldevnm );
IMPORT ER initialize_devmgr( void );
IMPORT ER finish_devmgr( void );
/* deviceio.c */
IMPORT ER check_devdesc( ID dd, UINT mode, OpnCB **p_opncb );
IMPORT ID _tk_opn_dev( UB *devnm, UINT omode );
IMPORT ER _tk_cls_dev( ID dd, UINT option );
IMPORT ID _tk_rea_dev( ID dd, INT start, VP buf, INT size, TMO tmout );
IMPORT ER _tk_srea_dev( ID dd, INT start, VP buf, INT size, INT *asize );
IMPORT ID _tk_wri_dev( ID dd, INT start, VP buf, INT size, TMO tmout );
IMPORT ER _tk_swri_dev( ID dd, INT start, VP buf, INT size, INT *asize );
IMPORT ID _tk_wai_dev( ID dd, ID reqid, INT *asize, ER *ioer, TMO tmout );
IMPORT INT _tk_sus_dev( UINT mode );
IMPORT void devmgr_break( ID tskid );
IMPORT void devmgr_startup( ID resid, INT info );
IMPORT void devmgr_cleanup( ID resid, INT info );
IMPORT ER initDevIO( void );
IMPORT ER finishDevIO( void );
/* chkplv.c */
IMPORT ER ChkCallPLevel( void );
/* power.c */
IMPORT void low_pow( void );
IMPORT void off_pow( void );
/* memmgr.c */
IMPORT UW smPageCount( UW byte );
IMPORT VP GetSysMemBlk( INT nblk, UINT attr);
IMPORT ER RelSysMemBlk( VP addr );
IMPORT ER RefSysMemInfo( T_RSMB *pk_rsmb );
IMPORT ER init_memmgr( void );
IMPORT ER start_memmgr( void );
/* segmgr.c */
IMPORT ER initialize_segmgr( void );
#endif

#endif /* _SYSMGR_ */
