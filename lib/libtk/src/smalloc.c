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
 *	@(#)smalloc.c (libtk)
 *
 *	Non-resident shared memory allocation 
 */

#include "libtk.h"

#ifndef SMALLOCTEST

EXPORT void* Smalloc( size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_malloc(size, &_Smacb);
	MEMUNLOCK()

	return p;
}

EXPORT void* Scalloc( size_t nmemb, size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_calloc(nmemb, size, &_Smacb);
	MEMUNLOCK()

	return p;
}

EXPORT void* Srealloc( void *ptr, size_t size )
{
	void	*p;

	MEMLOCK( return NULL )
	p = _mem_realloc(ptr, size, &_Smacb);
	MEMUNLOCK()

	return p;
}

EXPORT void Sfree( void *ptr )
{
	MEMLOCK( return )
	_mem_free(ptr, &_Smacb);
	MEMUNLOCK()
}

#else /* SMALLOCTEST */

EXPORT void Smalloctest( int mode )
{
	MEMLOCK( return )
	_mem_malloctest(mode, &_Smacb);
	MEMUNLOCK()
}

EXPORT BOOL Smalloccheck( void *ptr )
{
	BOOL	v;

	MEMLOCK( return FALSE )
	v = _mem_malloccheck(ptr, &_Smacb);
	MEMUNLOCK()

	return v;
}

#endif /* SMALLOCTEST */
