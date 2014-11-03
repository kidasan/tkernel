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
 *	@(#)pstartup.c (crttk)
 *
 *	Manager startup 
 */

#include <basic.h>
#include <tk/typedef.h>
#include <tk/errno.h>

/*
 * Compiler-dependent initialization/finalization sequence  
 */
IMPORT void _init_compiler( void );
IMPORT void _fini_compiler( void );

/*
 * Library (libtk) initialization/termination sequence
 */
IMPORT	void	_InitLibtk( void );
IMPORT	void	_FinishLibtk( void );

/*
 * Manager entry  
 */
IMPORT ER main( INT ac, UB *av[] );

/*
 * Manager startup 
 */
EXPORT ER _P_startup( INT ac, UB *av[] )
{
	ER	ercd;

	if ( ac >= 0 ) {
		/* Library initialization */
		_InitLibtk();

		/* Compiler-dependent initialization/finalization sequence */
		_init_compiler();
	}

	/* main() execute */
	ercd = main(ac, av);

	/* Library function termination sequence is performed in the
	 * event of startup failure (main returns an error during startup)
	 * and successful termination (main does not return an error
	 * at termination). 
	 */
	if ( (ac >= 0 && ercd <  E_OK) || (ac <  0 && ercd >= E_OK) ) {
		/* Compiler-dependent finalization sequence */
		_fini_compiler();

		/* Library termination sequence */
		_FinishLibtk();
	}

	return ercd;
}
