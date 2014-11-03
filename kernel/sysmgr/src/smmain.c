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
 *	smmain.c (T-Kernel/SM)
 *	T-Kernel/System Manager
 */

#include "sysmgr.h"

/*
 * Get resource management block
 *	Get the resource management block of subsystem of the
 *	local task 'ssid'
 */
EXPORT VP GetResBlk( ID ssid, ID tskid )
{
	ID	resid;
	VP	resblk;
	ER	ercd;

	resid = tk_get_rid(tskid);
	if ( resid < E_OK ) {
		ercd = resid;
		goto err_ret;
	}

	ercd = tk_get_res(resid, ssid, &resblk);
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	return resblk;

err_ret:
	DEBUG_PRINT(("GetResBlk ercd = %d\n", ercd));
	return NULL;
}

/* ------------------------------------------------------------------------- */

/*
 * Initialization sequence
 */
LOCAL ER initialize( void )
{
	ER	ercd;

	/* system management */
	ercd = initialize_sysmgr();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	/* Device management */
	ercd = initialize_devmgr();
	if ( ercd < E_OK ) {
		goto err_ret;
	}

	return E_OK;

err_ret:
	DEBUG_PRINT(("initialize ercd = %d\n", ercd));
	return ercd;
}

/*
 * Finalization sequence
 */
LOCAL ER finish( void )
{
	ER	ercd;

	/* Device management */
	ercd = finish_devmgr();
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("1. finish -> finish_devmgr ercd = %d\n", ercd));
	}
#endif

	/* system management */
	ercd = finish_sysmgr();
#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("2. finish -> finish_sysmgr ercd = %d\n", ercd));
	}
#endif

	return ercd;
}

/*
 * T-Kernel/System Manager Entrance
 */
EXPORT ER SystemManager( INT ac, UB *av[] )
{
	ER	ercd;

	if ( ac >= 0 ) {
		/* Initialization sequence */
		ercd = initialize();
		if ( ercd < E_OK ) {
			finish();
		}
	} else {
		/* Finalization sequence */
		ercd = finish();
	}

#ifdef DEBUG
	if ( ercd < E_OK ) {
		DEBUG_PRINT(("SystemManager ercd = %d\n", ercd));
	}
#endif
	return ercd;
}
