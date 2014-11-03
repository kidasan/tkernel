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
 *	@(#)vmalloc.c (libtk)
 *
 *	Non resident system memory allocation 
 */

#include "libtk.h"

#ifndef VMALLOCTEST

EXPORT void* Vmalloc( size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_malloc(size, &_Vmacb);
	MEMUNLOCK()

	return p;
}

EXPORT void* Vcalloc( size_t nmemb, size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_calloc(nmemb, size, &_Vmacb);
	MEMUNLOCK()

	return p;
}

EXPORT void* Vrealloc( void *ptr, size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_realloc(ptr, size, &_Vmacb);
	MEMUNLOCK()

	return p;
}

EXPORT void Vfree( void *ptr )
{
	MEMLOCK( return )
	_mem_free(ptr, &_Vmacb);
	MEMUNLOCK()
}

#else /* VMALLOCTEST */

EXPORT void Vmalloctest( int mode )
{
	MEMLOCK( return )
	_mem_malloctest(mode, &_Vmacb);
	MEMUNLOCK()
}

EXPORT BOOL Vmalloccheck( void *ptr )
{
	BOOL	v;

	MEMLOCK( return FALSE )
	v = _mem_malloccheck(ptr, &_Vmacb);
	MEMUNLOCK()

	return v;
}

#endif /* VMALLOCTEST */
