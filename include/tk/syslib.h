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
 *	@(#)syslib.h (T-Kernel)
 *
 *	System Library
 */

#ifndef __TK_SYSLIB_H__
#define __TK_SYSLIB_H__

#include <basic.h>
#include <tk/typedef.h>

#include <tk/sysdepend/syslib_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Task address space setting
 */
IMPORT ER SetTaskSpace( ID tskid );

/*
 * Address space check
 */
IMPORT ER ChkSpaceR( VP addr, INT len );
IMPORT ER ChkSpaceRW( VP addr, INT len );
IMPORT ER ChkSpaceRE( VP addr, INT len );
IMPORT INT ChkSpaceBstrR( UB *str, INT max );
IMPORT INT ChkSpaceBstrRW( UB *str, INT max );
IMPORT INT ChkSpaceTstrR( TC *str, INT max );
IMPORT INT ChkSpaceTstrRW( TC *str, INT max );

/*
 * Address space lock
 */
IMPORT ER LockSpace( VP addr, INT len );
IMPORT ER UnlockSpace( VP addr, INT len );

/*
 * Get physical address
 */
IMPORT INT CnvPhysicalAddr( VP vaddr, INT len, VP *paddr );

/*
 * Wait
 */
IMPORT void WaitUsec( UINT usec );	/* micro second wait */
IMPORT void WaitNsec( UINT nsec );	/* nano second wait */

#ifdef __cplusplus
}
#endif
#endif /* __TK_SYSLIB_H__ */
