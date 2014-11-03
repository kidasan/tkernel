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
 *	@(#)stddef.h
 *
 *	C language: common definitions 
 */

#ifndef __STDDEF_H__
#define __STDDEF_H__

#include <stdtype.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int		ptrdiff_t;

#ifdef	__size_t
typedef __size_t	size_t;
#undef	__size_t
#endif

#ifdef	__wchar_t
typedef __wchar_t	wchar_t;
#undef	__wchar_t
#endif

#define offsetof(type, member)	( (size_t)(&((type *)0)->member) )
#define NULL	0

#ifdef __cplusplus
}
#endif
#endif /* __STDDEF_H__ */
