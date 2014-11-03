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
 *	@(#)tkminit.c (libtk)
 *
 *	Memory allocation library 
 *	T-Kernel initialization sequence 
 *
 *	_tkm_init is always linked as it is called from the startup
 *	sequence part. 
 *	Note that adding too many processing can make the program
 *	size quite large, irrespective of whether malloc is required
 *	or not.
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <sys/memalloc.h>

/*
 * Memory allocation 
 */
LOCAL VP getblk( INT nblk, UINT mematr )
{
	VP	ptr;
	ER	ercd;

	ercd = tk_get_smb(&ptr, nblk, mematr);
	if ( ercd < E_OK ) {
		return NULL;
	}

	return ptr;
}

/*
 * Memory release 
 */
LOCAL void relblk( VP ptr )
{
	tk_rel_smb(ptr);
}

/*
 * MACB initialization 
 */
EXPORT ER _tkm_init( UINT mematr, MACB *_macb )
{
	MACB	*macb = AlignMACB(_macb);
	T_RSMB	rsmb;
	ER	ercd;

	/* Initialize memory allocation management information */
	macb->pagesz   = 0;	/* 0 indicates not available for use */
	macb->mematr   = mematr;
	macb->testmode = 0;
	macb->getblk   = getblk;
	macb->relblk   = relblk;
	QueInit(&macb->areaque);
	QueInit(&macb->freeque);

	/* Get memory information */
	ercd = tk_ref_smb(&rsmb);
	if ( ercd < E_OK ) {
		return ercd;
	}
	macb->pagesz = (UINT)rsmb.blksz;

	return E_OK;
}
