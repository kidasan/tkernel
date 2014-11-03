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
 *	isyscall.h (T-Kernel/OS)
 *	System Call Int. Format Definition
 */

#ifndef _ISYSCALL_
#define _ISYSCALL_

/* ------------------------------------------------------------------------ */

#if TA_GP
# define _CALL(p1, p2, p3, hdr, cb)	\
		CallUserHandler((INT)(p1), (INT)(p2), (INT)(p3), (FP)(hdr), (cb)->gp)
# define CallUserHandlerP1(   p1,         hdr, cb)	_CALL(p1, 0,  0,  hdr, cb)
# define CallUserHandlerP2(   p1, p2,     hdr, cb)	_CALL(p1, p2, 0,  hdr, cb)
# define CallUserHandlerP3(   p1, p2, p3, hdr, cb)	_CALL(p1, p2, p3, hdr, cb)
# define CallUserHandlerP2_GP(p1, p2,     hdr, cb)	_CALL(p1, p2, gp, hdr, cb)
#else
# define CallUserHandlerP1(   p1,         hdr, cb)	(*(hdr))(p1)
# define CallUserHandlerP2(   p1, p2,     hdr, cb)	(*(hdr))(p1, p2)
# define CallUserHandlerP3(   p1, p2, p3, hdr, cb)	(*(hdr))(p1, p2, p3)
# define CallUserHandlerP2_GP(p1, p2,     hdr, cb)	(*(hdr))(p1, p2)
#endif

/* ------------------------------------------------------------------------ */

#if TA_GP
#define P0(void)		( int _1,int _2,int _3,int _4,int _5, VP gp )
#define P1(p1)			( p1,    int _2,int _3,int _4,int _5, VP gp )
#define P2(p1, p2)		( p1, p2,       int _3,int _4,int _5, VP gp )
#define P3(p1, p2, p3)		( p1, p2, p3,          int _4,int _5, VP gp )
#define P4(p1, p2, p3, p4)	( p1, p2, p3, p4,             int _5, VP gp )
#define P5(p1, p2, p3, p4, p5)	( p1, p2, p3, p4, p5,                 VP gp )
#define P2GP(p1, p2)		( p1, p2,			      VP gp )
#else
#define P0(void)		( void )
#define P1(p1)			( p1 )
#define P2(p1, p2)		( p1, p2 )
#define P3(p1, p2, p3)		( p1, p2, p3 )
#define P4(p1, p2, p3, p4)	( p1, p2, p3, p4 )
#define P5(p1, p2, p3, p4, p5)	( p1, p2, p3, p4, p5 )
#define P2GP(p1, p2)		( p1, p2 )
#endif

/* ------------------------------------------------------------------------ */

/* T-Kernel/OS */
IMPORT ID _tk_cre_tsk P1( T_CTSK *pk_ctsk );
IMPORT ER _tk_del_tsk( ID tskid );
IMPORT ER _tk_sta_tsk( ID tskid, INT stacd );
IMPORT void _tk_ext_tsk( void );
IMPORT void _tk_exd_tsk( void );
IMPORT ER _tk_ter_tsk( ID tskid );
IMPORT ER _tk_dis_dsp( void );
IMPORT ER _tk_ena_dsp( void );
IMPORT ER _tk_chg_pri( ID tskid, PRI tskpri );
IMPORT ER _tk_chg_slt( ID tskid, RELTIM slicetime );
IMPORT ER _tk_rot_rdq( PRI tskpri );
IMPORT ER _tk_rel_wai( ID tskid );
IMPORT ID _tk_get_tid( void );
IMPORT ER _tk_get_tsp( ID tskid, T_TSKSPC *pk_tskspc );
IMPORT ER _tk_set_tsp( ID tskid, T_TSKSPC *pk_tskspc );
IMPORT ID _tk_get_rid( ID tskid );
IMPORT ID _tk_set_rid( ID tskid, ID resid );
IMPORT ER _tk_get_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );
IMPORT ER _tk_set_reg( ID tskid, T_REGS *pk_regs, T_EIT *pk_eit, T_CREGS *pk_cregs );
IMPORT ER _tk_get_cpr( ID tskid, INT copno, T_COPREGS *pk_copregs );
IMPORT ER _tk_set_cpr( ID tskid, INT copno, T_COPREGS *pk_copregs );
IMPORT ER _tk_inf_tsk( ID tskid, T_ITSK *pk_itsk, BOOL clr );
IMPORT ER _tk_ref_tsk( ID tskid, T_RTSK *pk_rtsk );
IMPORT ER _tk_def_tex( ID tskid, T_DTEX *pk_dtex );
IMPORT ER _tk_ena_tex( ID tskid, UINT texptn );
IMPORT ER _tk_dis_tex( ID tskid, UINT texptn );
IMPORT ER _tk_ras_tex( ID tskid, INT texcd );
IMPORT INT _tk_end_tex( BOOL enatex );
IMPORT ER _tk_ref_tex( ID tskid, T_RTEX *pk_rtex );
IMPORT ER _tk_sus_tsk( ID tskid );
IMPORT ER _tk_rsm_tsk( ID tskid );
IMPORT ER _tk_frsm_tsk( ID tskid );
IMPORT ER _tk_slp_tsk( TMO tmout );
IMPORT ER _tk_wup_tsk( ID tskid );
IMPORT INT _tk_can_wup( ID tskid );
IMPORT ER _tk_sig_tev( ID tskid, INT tskevt );
IMPORT INT _tk_wai_tev( INT waiptn, TMO tmout );
IMPORT INT _tk_dis_wai( ID tskid, UINT waitmask );
IMPORT ER _tk_ena_wai( ID tskid );

#ifdef NUM_SEMID
IMPORT ID _tk_cre_sem( T_CSEM *pk_csem );
IMPORT ER _tk_del_sem( ID semid );
IMPORT ER _tk_sig_sem( ID semid, INT cnt );
IMPORT ER _tk_wai_sem( ID semid, INT cnt, TMO tmout );
IMPORT ER _tk_ref_sem( ID semid, T_RSEM *pk_rsem );
#endif

#ifdef NUM_MTXID
IMPORT ID _tk_cre_mtx( T_CMTX *pk_cmtx );
IMPORT ER _tk_del_mtx( ID mtxid );
IMPORT ER _tk_loc_mtx( ID mtxid, TMO tmout );
IMPORT ER _tk_unl_mtx( ID mtxid );
IMPORT ER _tk_ref_mtx( ID mtxid, T_RMTX *pk_rmtx );
#endif

#ifdef NUM_FLGID
IMPORT ID _tk_cre_flg( T_CFLG *pk_cflg );
IMPORT ER _tk_del_flg( ID flgid );
IMPORT ER _tk_set_flg( ID flgid, UINT setptn );
IMPORT ER _tk_clr_flg( ID flgid, UINT clrptn );
IMPORT ER _tk_wai_flg( ID flgid, UINT waiptn, UINT wfmode, UINT *p_flgptn, TMO tmout );
IMPORT ER _tk_ref_flg( ID flgid, T_RFLG *pk_rflg );
#endif

#ifdef NUM_MBXID
IMPORT ID _tk_cre_mbx( T_CMBX* pk_cmbx );
IMPORT ER _tk_del_mbx( ID mbxid );
IMPORT ER _tk_snd_mbx( ID mbxid, T_MSG *pk_msg );
IMPORT ER _tk_rcv_mbx( ID mbxid, T_MSG **ppk_msg, TMO tmout );
IMPORT ER _tk_ref_mbx( ID mbxid, T_RMBX *pk_rmbx );
#endif

#ifdef NUM_MBFID
IMPORT ID _tk_cre_mbf( T_CMBF *pk_cmbf );
IMPORT ER _tk_del_mbf( ID mbfid );
IMPORT ER _tk_snd_mbf( ID mbfid, VP msg, INT msgsz, TMO tmout );
IMPORT INT _tk_rcv_mbf( ID mbfid, VP msg, TMO tmout );
IMPORT ER _tk_ref_mbf( ID mbfid, T_RMBF *pk_rmbf );
#endif

#ifdef NUM_PORID
IMPORT ID _tk_cre_por( T_CPOR *pk_cpor );
IMPORT ER _tk_del_por( ID porid );
IMPORT INT _tk_cal_por( ID porid, UINT calptn, VP msg, INT cmsgsz, TMO tmout );
IMPORT INT _tk_acp_por( ID porid, UINT acpptn, RNO *p_rdvno, VP msg, TMO tmout );
IMPORT ER _tk_fwd_por( ID porid, UINT calptn, RNO rdvno, VP msg, INT cmsgsz );
IMPORT ER _tk_rpl_rdv( RNO rdvno, VP msg, INT rmsgsz );
IMPORT ER _tk_ref_por( ID porid, T_RPOR *pk_rpor );
#endif

IMPORT ER _tk_def_int P2( UINT dintno, T_DINT *pk_dint );
IMPORT void _tk_ret_int( void );

#ifdef NUM_MPLID
IMPORT ID _tk_cre_mpl( T_CMPL *pk_cmpl );
IMPORT ER _tk_del_mpl( ID mplid );
IMPORT ER _tk_get_mpl( ID mplid, INT blksz, VP *p_blk, TMO tmout );
IMPORT ER _tk_rel_mpl( ID mplid, VP blk );
IMPORT ER _tk_ref_mpl( ID mplid, T_RMPL *pk_rmpl );
#endif

#ifdef NUM_MPFID
IMPORT ID _tk_cre_mpf( T_CMPF *pk_cmpf );
IMPORT ER _tk_del_mpf( ID mpfid );
IMPORT ER _tk_get_mpf( ID mpfid, VP *p_blf, TMO tmout );
IMPORT ER _tk_rel_mpf( ID mpfid, VP blf );
IMPORT ER _tk_ref_mpf( ID mpfid, T_RMPF *pk_rmpf );
#endif

IMPORT ER _tk_set_tim( SYSTIM *pk_tim );
IMPORT ER _tk_get_tim( SYSTIM *pk_tim );
IMPORT ER _tk_get_otm( SYSTIM *pk_tim );
IMPORT ER _tk_dly_tsk( RELTIM dlytim );

#ifdef NUM_CYCID
IMPORT ID _tk_cre_cyc P1( T_CCYC *pk_ccyc );
IMPORT ER _tk_del_cyc( ID cycid );
IMPORT ER _tk_sta_cyc( ID cycid );
IMPORT ER _tk_stp_cyc( ID cycid );
IMPORT ER _tk_ref_cyc( ID cycid, T_RCYC *pk_rcyc );
#endif

#ifdef NUM_ALMID
IMPORT ID _tk_cre_alm P1( T_CALM *pk_calm );
IMPORT ER _tk_del_alm( ID almid );
IMPORT ER _tk_sta_alm( ID almid, RELTIM almtim );
IMPORT ER _tk_stp_alm( ID almid );
IMPORT ER _tk_ref_alm( ID almid, T_RALM *pk_ralm );
#endif

IMPORT ER _tk_ref_ver( T_RVER *pk_rver );
IMPORT ER _tk_ref_sys( T_RSYS *pk_rsys );
IMPORT ER _tk_def_ssy P2( ID ssid, T_DSSY *pk_dssy );
IMPORT ER _tk_sta_ssy( ID ssid, ID resid, INT info );
IMPORT ER _tk_cln_ssy( ID ssid, ID resid, INT info );
IMPORT ER _tk_evt_ssy( ID ssid, INT evttyp, ID resid, INT info );
IMPORT ER _tk_ref_ssy( ID ssid, T_RSSY *pk_rssy );
IMPORT ID _tk_cre_res( void );
IMPORT ER _tk_del_res( ID resid );
IMPORT ER _tk_get_res( ID resid, ID ssid, VP *p_resblk );
IMPORT ER _tk_set_pow( UINT powmode );

/* T-Kernel/SM */
IMPORT INT _tk_get_cfn( UB *name, INT *val, INT max );
IMPORT INT _tk_get_cfs( UB *name, UB *buf, INT max );

/*** SMP additional ***/
/* T-Kernel/OS */
IMPORT ER _tk_fnd_tsk( ID domid, UB *oname );
IMPORT ER _tk_dmi_tsk( ID tskid, T_DMI *pk_dmi );

#ifdef NUM_SEMID
IMPORT ER _tk_fnd_sem( ID domid, UB *oname );
IMPORT ER _tk_dmi_sem( ID semid, T_DMI *pk_dmi );
#endif

#ifdef NUM_MTXID
IMPORT ER _tk_fnd_mtx( ID domid, UB *oname );
IMPORT ER _tk_dmi_mtx( ID mtxid, T_DMI *pk_dmi );
#endif

#ifdef NUM_FLGID
IMPORT ER _tk_fnd_flg( ID domid, UB *oname );
IMPORT ER _tk_dmi_flg( ID flgid, T_DMI *pk_dmi );
#endif

#ifdef NUM_MBXID
IMPORT ER _tk_fnd_mbx( ID domid, UB *oname );
IMPORT ER _tk_dmi_mbx( ID mbxid, T_DMI *pk_dmi );
#endif

#ifdef NUM_MBFID
IMPORT ID _tk_fnd_mbf( ID domid, UB *oname );
IMPORT ER _tk_dmi_mbf( ID mbfid, T_DMI *pk_dmi );
#endif

#ifdef NUM_PORID
IMPORT ID _tk_fnd_por( ID domid, UB *oname );
IMPORT ER _tk_dmi_por( ID porid, T_DMI *pk_dmi );
#endif

#ifdef NUM_MPLID
IMPORT ER _tk_fnd_mpl( ID domid, UB *oname );
IMPORT ER _tk_dmi_mpl( ID mplid, T_DMI *pk_dmi );
#endif

#ifdef NUM_MPFID
IMPORT ER _tk_fnd_mpf( ID domid, UB *oname );
IMPORT ER _tk_dmi_mpf( ID mpfid, T_DMI *pk_dmi );
#endif

#ifdef NUM_CYCID
IMPORT ER _tk_fnd_cyc( ID domid, UB *oname );
IMPORT ER _tk_dmi_cyc( ID cycid, T_DMI *pk_dmi );
#endif

#ifdef NUM_ALMID
IMPORT ER _tk_fnd_alm( ID domid, UB *oname );
IMPORT ER _tk_dmi_alm( ID almid, T_DMI *pk_dmi );
#endif

/* Domain */
IMPORT ID _tk_cre_dom( T_CDOM *pk_cdom );
IMPORT ER _tk_del_dom( ID domid );
IMPORT ID _tk_fnd_dom( ID domid, UB *oname );
IMPORT ER _tk_ref_dom( ID domid, T_RDOM *pk_rdom );
IMPORT ER _tk_dmi_dom( ID domid, T_DMI *pk_dmi );

#endif /* _ISYSCALL_ */
