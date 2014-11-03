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
 *	@(#)malloc.c (libtk)
 *
 *	Non resident system memory allocation 
 */

#include "libtk.h"
#include <sys/util.h>

EXPORT void* malloc( size_t size )
{
	return Vmalloc(size);
}

EXPORT void* calloc( size_t nmemb, size_t size )
{
	return Vcalloc(nmemb, size);
}

EXPORT void* realloc( void *ptr, size_t size )
{
	return Vrealloc(ptr, size);
}

EXPORT void free( void *ptr )
{
	Vfree(ptr);
}
