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
 *	@(#)setspc.c (libtk/NAVIENGINE)
 *
 *	Address space control 
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <tk/sysdef.h>
#include "getsvcenv.h"

/*
 * Set task address space 
 */
EXPORT ER SetTaskSpace( ID taskid )
{
	UW	taskmode;
	ER	ercd;
	UW	imask, prid;

	if ( taskid == TSK_SELF ) {
		/* Set current CPL in RPL */
		DI(imask);
		prid = get_prid();
		taskmode = SCInfo.taskmode[prid];
		SCInfo.taskmode[prid] ^= (taskmode ^ (taskmode << 16)) & TMF_PPL(3);
		EI(imask);

	} else {
		T_EIT		r_eit;
		T_CREGS		r_cregs;
		T_TSKSPC	tskspc;

		/* Get logical space/taskmode for taskid tasks */
		ercd = tk_get_reg(taskid, NULL, &r_eit, &r_cregs);
		if ( ercd < E_OK ) {
			goto err_ret;
		}

		/* Change to logical space for nominated tasks */
		tskspc.uatb = r_cregs.uatb;
		tskspc.lsid = r_cregs.lsid;
		ercd = tk_set_tsp(TSK_SELF, &tskspc);
		if ( ercd < E_OK ) {
			goto err_ret;
		}
		/* Change to PPL for nominted tasks */
		DI(imask);
		prid = get_prid();
		taskmode = SCInfo.taskmode[prid];
		SCInfo.taskmode[prid] ^= (taskmode ^ r_eit.taskmode) & TMF_PPL(3);
		EI(imask);

	}

	return E_OK;

err_ret:
	return ercd;
}
