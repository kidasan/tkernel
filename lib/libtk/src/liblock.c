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
 *	@(#)liblock.c (libtk)
 *
 *	Shared exclusive control lock in library 
 */

#include "libtk.h"
#include <sys/util.h>
#include <tk/util.h>

LOCAL	FastMLock	LibLock;

/* Set Object Name in .exinf for DEBUG */
#define OBJNAME_LIBLOCK		"lltk"	/* multi-lock object name for liblock */

EXPORT ER _init_liblock( void )
{
	return CreateMLock(&LibLock, OBJNAME_LIBLOCK);
}

EXPORT ER _lib_locktmo( W lockno, W tmo, BOOL ignore_mintr )
{
	return MLockTmo(&LibLock, lockno, tmo);
}

EXPORT ER _lib_lock( W lockno, BOOL ignore_mintr )
{
	return MLock(&LibLock, lockno);
}

EXPORT void _lib_unlock( W lockno )
{
	MUnlock(&LibLock, lockno);
}

EXPORT void _delete_liblock( void )
{
	DeleteMLock(&LibLock);
}
