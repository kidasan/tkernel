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
 *	@(#)libinit.c (libtk)
 *
 *	libker library initialization 
 *
 *	_InitLibtk() is always linked as it is called from the
 *	startup part.
 *	Note that adding too many processing can make the program
 *	quite large.
 */

#include "libtk.h"
#include <sys/util.h>
#include <sys/memalloc.h>
#include <tk/util.h>

EXPORT MACB	_Kmacb;		/* Kmalloc management information */
EXPORT MACB	_Vmacb;		/* Vmalloc management information */
EXPORT MACB	_Smacb;		/* Smalloc management information */

LOCAL	BOOL	libtk_init_done = FALSE;

/*
 * Library initialization 
 */
EXPORT void _InitLibtk( void )
{
	INT	rng;

	if ( libtk_init_done ) {
		return;  /* Initialized */
	}

	/* Kernel utility initialization */
	KnlInit();

	/* Lowest protection level where system calls can be issued */
	if ( tk_get_cfn("TSVCLimit", &rng, 1) < 1 ) {
		rng = 2;
	}
	rng <<= 8;

	/* Create exclusive control lock for library sharing */
	_init_liblock();

	/* malloc initialization */
	_tkm_init((UINT)rng, &_Kmacb);			/* Kmalloc init */
	_tkm_init((UINT)rng|TA_NORESIDENT, &_Vmacb);	/* Vmalloc init */
	_tkm_init(TA_RNG3|TA_NORESIDENT, &_Smacb);	/* Smalloc init */

	libtk_init_done = TRUE;  /* Initialization complete */
}

/*
 * Library finalization 
 */
EXPORT void _FinishLibtk( void )
{
	if ( !libtk_init_done ) {
		return;
	}

	_delete_liblock();
}
