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
 *	@(#)startup_elf.c (libtk)
 *
 *	Compiler dependent startup sequence (for ELF)
 */

#include <basic.h>

EXPORT void _init_compiler( void )
{
IMPORT	void	_init( void );
	_init();
}

EXPORT void _fini_compiler( void )
{
IMPORT	void	_fini( void );
	_fini();
}
