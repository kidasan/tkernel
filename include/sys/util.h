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
 *	@(#)util.h (sys)
 *
 *	Manager utilities 
 */

#ifndef __SYS_UTIL_H__
#define __SYS_UTIL_H__

#include <basic.h>
#include <tk/typedef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __pinfo__
typedef struct pinfo	PINFO;	/* Defined in sys/pinfo.h */
#define __pinfo__
#endif

/*
 * SYSCONF definition
 */
#define L_SYSCONF_VAL		16	/* Maximum number of elements */
#define L_SYSCONF_STR		256	/* Maximum string length */

/* Device related */
IMPORT ID	GetDevEvtMbf(void);

/* Memory allocation */
IMPORT void*	Smalloc(size_t size);
IMPORT void*	Scalloc(size_t nmemb, size_t size);
IMPORT void*	Srealloc(void *ptr, size_t size);
IMPORT void	Sfree(void *ptr);
IMPORT void	Smalloctest(int mode);
IMPORT BOOL	Smalloccheck(void *ptr);
IMPORT void	Kmalloctest(int mode);
IMPORT BOOL	Kmalloccheck(void *ptr);
IMPORT void	Vmalloctest(int mode);
IMPORT BOOL	Vmalloccheck(void *ptr);

/* Address check */
IMPORT ER	CheckSpaceR( VP address, W len );
IMPORT ER	CheckSpaceRW( VP address, W len );
IMPORT ER	CheckSpaceRE( VP address, W len );
IMPORT ER	CheckStrSpaceR( TC *str, W max );
IMPORT ER	CheckStrSpaceRW( TC *str, W max );
IMPORT ER	CheckBStrSpaceR( UB *str, W max );
IMPORT ER	CheckBStrSpaceRW( UB *str, W max );

/* Error code conversion */
IMPORT BOOL	_isDebugMode( void );
IMPORT BOOL	_isFsrcvMode( void );

/* Other */
IMPORT void _InitLibtk(void);
IMPORT void KnlInit(void);

#ifdef __cplusplus
}
#endif
#endif /*
 * Library (libtk) initialization 
 *	The library is normally initialized automatically,
 *	so these functions do not need to be called explicitly.
 */
