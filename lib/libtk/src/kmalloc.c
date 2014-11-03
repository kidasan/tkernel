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
 *	@(#)kmalloc.c (libtk)
 *
 *	Resident system memory allocation 
 */

#include "libtk.h"

#ifndef KMALLOCTEST

EXPORT void* Kmalloc( size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_malloc(size, &_Kmacb);
	MEMUNLOCK()

	return p;
}

EXPORT void* Kcalloc( size_t nmemb, size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_calloc(nmemb, size, &_Kmacb);
	MEMUNLOCK()

	return p;
}

EXPORT void* Krealloc( void *ptr, size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_realloc(ptr, size, &_Kmacb);
	MEMUNLOCK()

	return p;
}

EXPORT void Kfree( void *ptr )
{
	MEMLOCK( return )
	_mem_free(ptr, &_Kmacb);
	MEMUNLOCK()
}

#else /* KMALLOCTEST */

EXPORT void Kmalloctest( int mode )
{
	MEMLOCK( return )
	_mem_malloctest(mode, &_Kmacb);
	MEMUNLOCK()
}

EXPORT BOOL Kmalloccheck( void *ptr )
{
	BOOL	v;

	MEMLOCK( return FALSE )
	v = _mem_malloccheck(ptr, &_Kmacb);
	MEMUNLOCK()

	return v;
}

#endif /* KMALLOCTEST */
