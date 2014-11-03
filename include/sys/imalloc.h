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
 *	@(#)imalloc.h (sys)
 *
 *	Kernel memory allocation 
 *
 *	A function for allocating memory used in parts of T-Kernel.
 *	Not for general use.
 */

#ifndef __SYS_IMALLOC_H__
#define __SYS_IMALLOC_H__

#include <basic.h>
#include <tk/typedef.h>
#include <tk/sysmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Allocates resident memory which has the lowest protection
 * level (TSCVLimit) in which level where a T-Kernel system call
 * can be issued.
 */
IMPORT void* Imalloc( size_t size );
IMPORT void* Icalloc( size_t nmemb, size_t size );
IMPORT void  Ifree( void *ptr );

/*
 * Allocates memory with the attributes specified in 'attr'.
 *	attr = TA_RNGn | TA_NORESIDENT
 */
IMPORT void* IAmalloc( size_t size, UINT attr );
IMPORT void* IAcalloc( size_t nmemb, size_t size, UINT attr );
IMPORT void  IAfree( void *ptr, UINT attr );

/*
 * In all cases, malloc and calloc cannot be called while interrupt or
 * dispatch are disabled. 
 * Only free can always be called while interrupt or dispatch are
 * disabled; however memory release may not complete. For this reason,
 * it is preferable to issue the call when interrupt and dispatch are
 * both enabled.
 * If release has not completed, the area is allocated in accordance
 * with the subsequent
 * malloc and calloc.
*/

/* ------------------------------------------------------------------------ */

/*
 * System memory management in block units
 */
IMPORT VP GetSysMemBlk( INT nblk, UINT attr );
IMPORT ER RelSysMemBlk( VP addr );
IMPORT ER RefSysMemInfo( T_RSMB *rsmb );

#ifdef __cplusplus
}
#endif
#endif /* __SYS_IMALLOC_H__ */
