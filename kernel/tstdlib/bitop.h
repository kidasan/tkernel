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
 *	bitop.h (tstdlib)
 *
 *	T-Kernel common standard library
 *
 */

#ifndef	_BITOP_
#define _BITOP_

#include <stdtype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	__size_t
typedef __size_t	size_t;
#undef	__size_t
#endif

#ifdef	__wchar_t
typedef __wchar_t	wchar_t;
#undef	__wchar_t
#endif

#define NULL		0

extern void tstdlib_bitclr( VP base, W offset );
extern void tstdlib_bitset( VP base, W offset );
extern BOOL tstdlib_bittest( VP base, W offset );

extern W tstdlib_bitsearch0( VP base, W offset, W width );
extern W tstdlib_bitsearch1( VP base, W offset, W width );

#ifdef __cplusplus
}
#endif
#endif /* _BITOP_ */
