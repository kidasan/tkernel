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
 *	@(#)dbgspt_depend.h (tk/NAVIENGINE)
 *
 *	T-Kernel Debugger Support System-Dependent Definition (NAVIENGINE)
 */

#ifndef __TK_DBGSPT_DEPEND_H__
#define __TK_DBGSPT_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * System call/extension SVC caller information 
 */
typedef struct td_calinf {
	VP	ssp;		/* System stack pointer */
	VP	r11;		/* Frame pointer when calling */
} TD_CALINF;

#ifdef __cplusplus
}
#endif
#endif /* __TK_DBGSPT_DEPEND_H__ */
