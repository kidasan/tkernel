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
 *	check.h (T-Kernel/OS)
 *	Macro for Error Check 
 */

#ifndef _CHECK_
#define _CHECK_

/*
 * Check object ID range (E_ID)
 */
#if CHK_ID
#define CHECK_TSKID(tskid) {					\
	if (!in_indp() && ((tskid) == TSK_SELF)) {		\
		return E_OBJ;					\
	} else if (!CHK_TSKID(tskid)				\
		   || !CHK_ID_DOM(tskid)			\
		   || !CHK_ID_PRC(tskid) ) {			\
		return E_ID;					\
	}							\
}
#define CHECK_TSKID_SELF(tskid) {				\
	if ( !( (!in_indp() && (tskid == TSK_SELF))		\
		|| (CHK_TSKID(tskid)				\
		    && CHK_ID_DOM(tskid)			\
		    && CHK_ID_PRC(tskid)) ) ) {			\
		return E_ID;					\
	}							\
}
#define CHECK_SEMID(semid) {					\
	if (!CHK_SEMID(semid)					\
	 || !CHK_ID_DOM(semid)					\
	 || !CHK_ID_PRC(semid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_FLGID(flgid) {					\
	if (!CHK_FLGID(flgid)					\
	 || !CHK_ID_DOM(flgid)					\
	 || !CHK_ID_PRC(flgid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MBXID(mbxid) {					\
	if (!CHK_MBXID(mbxid)					\
	 || !CHK_ID_DOM(mbxid)					\
	 || !CHK_ID_PRC(mbxid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MBFID(mbfid) {					\
	if (!CHK_MBFID(mbfid)					\
	 || !CHK_ID_DOM(mbfid)					\
	 || !CHK_ID_PRC(mbfid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_PORID(porid) {					\
	if (!CHK_PORID(porid)					\
	 || !CHK_ID_DOM(porid)					\
	 || !CHK_ID_PRC(porid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MTXID(pisid) {					\
	if (!CHK_MTXID(pisid)					\
	 || !CHK_ID_DOM(pisid)					\
	 || !CHK_ID_PRC(pisid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MPLID(mplid) {					\
	if (!CHK_MPLID(mplid)					\
	 || !CHK_ID_DOM(mplid)					\
	 || !CHK_ID_PRC(mplid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_MPFID(mpfid) {					\
	if (!CHK_MPFID(mpfid)					\
	 || !CHK_ID_DOM(mpfid)					\
	 || !CHK_ID_PRC(mpfid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_CYCID(cycid) {					\
	if (!CHK_CYCID(cycid)					\
	 || !CHK_ID_DOM(cycid)					\
	 || !CHK_ID_PRC(cycid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_ALMID(almid) {					\
	if (!CHK_ALMID(almid)					\
	 || !CHK_ID_DOM(almid)					\
	 || !CHK_ID_PRC(almid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_RESID(resid) {					\
	if (!CHK_RESID(resid)) {				\
		return E_ID;					\
	}							\
}
#define CHECK_RESID_ANY(resid) {				\
	if (!(CHK_RESID(resid) || (resid == 0))) {		\
		return E_ID;					\
	}							\
}
#define CHECK_SSYID(ssid) {					\
	if (!CHK_SSYID(ssid)) {					\
		return E_ID;					\
	}							\
}
#define CHECK_SSYID_ALL(ssid) {					\
	if (!(CHK_SSYID(ssid) || (ssid == 0))) {		\
		return E_ID;					\
	}							\
}
#define CHECK_DOMID(domid) {					\
	if (!CHK_DOMID(domid)					\
	 || !CHK_ID_DOM(domid)					\
	 || !CHK_ID_PRC(domid) ) {				\
		return E_ID;					\
	}							\
}
#define CHECK_PRCID(prcid) {					\
	if (!CHK_PRCID(prcid)) {				\
		return E_ID;					\
	}							\
}
#else /* CHK_ID */
#define CHECK_TSKID(tskid)
#define CHECK_TSKID_SELF(tskid)
#define CHECK_SEMID(semid)
#define CHECK_FLGID(flgid)
#define CHECK_MBXID(mbxid)
#define CHECK_MBFID(mbfid)
#define CHECK_PORID(porid)
#define CHECK_MTXID(pisid)
#define CHECK_MPLID(mplid)
#define CHECK_MPFID(mpfid)
#define CHECK_CYCID(cycid)
#define CHECK_ALMID(almid)
#define CHECK_RESID(resid)
#define CHECK_RESID_ANY(resid)
#define CHECK_SSYID(ssid)
#define CHECK_SSYID_ALL(ssid)
#define CHECK_DOMID(domid)
#define CHECK_PRCID(prcid)
#endif /* CHK_ID */

/*
 * Check whether its own task is specified (E_OBJ)
 */
#if CHK_SELF
Inline ID get_ctxid( void )
{
	ID	id;

	BEGIN_DISABLE_INTERRUPT
	id = ctxtsk[get_prid()]->tskid;
	END_DISABLE_INTERRUPT

	return id;
}

#define CHECK_NONSELF(tskid) {					\
	if (!in_indp() && (tskid) == get_ctxid()) {		\
		return E_OBJ;					\
	}							\
}
#else /* CHK_SELF */
#define CHECK_NONSELF(tskid)
#endif /* CHK_SELF */

/*
 * Check task priority value (E_PAR)
 */
#if CHK_PAR
#define CHECK_PRI(pri) {					\
	if (!CHK_PRI(pri)) {					\
		return E_PAR;					\
	}							\
}
#define CHECK_PRI_INI(pri) {					\
	if ((pri != TPRI_INI) && !CHK_PRI(pri)) {		\
		return E_PAR;					\
	}							\
}
#define CHECK_PRI_RUN(pri) {					\
	if ((pri != TPRI_RUN) && !CHK_PRI(pri)) {		\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_PRI(pri)
#define CHECK_PRI_INI(pri)
#define CHECK_PRI_RUN(pri)
#endif /* CHK_PAR */

/*
 * Check subsystem priority value (E_PAR)
 */
#if CHK_PAR
#define CHECK_SSYPRI(ssypri) {					\
	if (!CHK_SSYPRI(ssypri)) {				\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_SSYPRI(ssypri)
#endif /* CHK_PAR */

/*
 * Check timeout specification value (E_PAR)
 */
#if CHK_PAR
#define CHECK_TMOUT(tmout) {					\
	if (!((tmout) >= TMO_FEVR)) {				\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_TMOUT(tmout)
#endif /* CHK_PAR */

/*
 * Check other parameter errors (E_PAR)
 */
#if CHK_PAR
#define CHECK_PAR(exp) {					\
	if (!(exp)) {						\
		return E_PAR;					\
	}							\
}
#else /* CHK_PAR */
#define CHECK_PAR(exp)
#endif /* CHK_PAR */

/*
 * Check reservation attribute error (E_RSATR)
 */
#if CHK_RSATR
#define CHECK_RSATR(atr, maxatr) {				\
	if ((atr & ~(maxatr)) != 0) {				\
	        return E_RSATR;					\
	}							\
}
#else /* CHK_RSATR */
#define CHECK_RSATR(atr, maxatr)
#endif /* CHK_RSATR */

/*
 * Check protection attribute value (E_RSATR)
 */
#if CHK_RSATR
#define CHECK_PATR(atr) {					\
	if ( (MP_GET_PATR(atr) != TA_PUBLIC)			\
	  && (MP_GET_PATR(atr) != TA_PROTECTED)			\
	  && (MP_GET_PATR(atr) != TA_PRIVATE) ) {		\
		return E_RSATR;					\
	}							\
}
#else /* CHK_RSATR */
#define CHECK_PATR(atr)
#endif /* CHK_RSATR */

/*
 * Check unsupported function (E_NOSPT)
 */
#if CHK_NOSPT
#define CHECK_NOSPT(exp) {					\
	if (!(exp)) {						\
		return E_NOSPT;					\
	}							\
}
#else /* CHK_NOSPT */
#define CHECK_NOSPT(exp)
#endif /* CHK_NOSPT */

/*
 * Check whether task-independent part is running (E_CTX)
 */
#if CHK_CTX
#define CHECK_INTSK() {						\
	if (in_indp()) {					\
		return E_CTX;					\
	}							\
}
#else /* CHK_CTX */
#define CHECK_INTSK()
#endif /* CHK_CTX */

/*
 * Check whether dispatch is in disabled state (E_CTX)
 */
#if CHK_CTX
#define CHECK_DISPATCH() {					\
	if (in_ddsp()) {					\
		return E_CTX;					\
	}							\
}
#define CHECK_DISPATCH_POL(tmout) {				\
	if ((tmout != TMO_POL) && in_ddsp()) {			\
		return E_CTX;					\
	}							\
}
#else /* CHK_CTX */
#define CHECK_DISPATCH()
#define CHECK_DISPATCH_POL(tmout)
#endif /* CHK_CTX */

/*
 * Check other context errors (E_CTX)
 */
#if CHK_CTX
#define CHECK_CTX(exp) {					\
	if (!(exp)) {						\
		return E_CTX;					\
	}							\
}
#else /* CHK_CTX */
#define CHECK_CTX(exp)
#endif /* CHK_CTX */

/*
 * Check non-existence co-processor error (E_NOCOP)
 */
#if CHK_NOCOP
#define CHECK_NOCOP(atr) {					\
	if ( ((atr & (TA_COP0|TA_COP1|TA_COP2|TA_COP3)) != 0)	\
	  && ((atr & available_cop) == 0) ) {			\
		return E_NOCOP;					\
	}							\
}
#else
#define CHECK_NOCOP(atr)
#endif

#endif /* _CHECK_ */
