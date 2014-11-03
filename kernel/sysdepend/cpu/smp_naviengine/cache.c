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
 *	cache.c (NAVIENGINE)
 *	Cache Operation
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <sys/segment.h>
#include "smpcall.h"

#define CACHE_LINESZ	(32)	/* Cache line size */

/*
 * Flush the whole memory cache
 */
EXPORT void FlushCache( VP laddr, INT len )
{
	Asm(
	"	mov	r0, #0\n"
	"  	mcr	p15, 0, r0, cr7, c14, 0	\n"
	"	mcr	p15, 0, r0, cr7, c5, 0	\n"
	"	mcr	p15, 0, r0, cr7, c5, 4	\n"
	"	mcr	p15, 0, r0, cr7, c10, 4	\n"
		);
}


EXPORT void FlushCacheM( VP laddr, INT len, UINT mode )
{
	FlushCache(laddr, len);
}


/*
 * Clean data cache line
 */
LOCAL void CleanDCacheL( VP addr, INT blk )
{
	Asm("0:						\n"
	    "	subs	%1, %1, #1			\n"
	    "	mcr	p15, 0, %0, cr7, c10, 1		\n"	/* Clean data cache line */
	    "	add	%0, %0, %2			\n"
	    "	bne	0b				\n"
	    "	mcr	p15, 0, %3, c7, c10, 4		\n"	/* Data synchronization barrier */
		:
		: "r"(addr), "r"(blk), "r"(CACHE_LINESZ), "r"(0)
		: "cc");

}

/*
 * Invalidate data cache line
 */
LOCAL void InvalidateDCacheL( VP addr, INT blk )
{
	Asm("0:						\n"
	    "	subs	%1, %1, #1			\n"
	    "	mcr	p15, 0, %0, cr7, c6, 1		\n"	/* Invalidate data cache line */
	    "	add	%0, %0, %2			\n"
	    "	bne	0b				\n"
	    "	mcr	p15, 0, %3, c7, c10, 4		\n"	/* Data synchronization barrier */
		:
		: "r"(addr), "r"(blk), "r"(CACHE_LINESZ), "r"(0)
		: "cc");
}

/*
 * Clean and invalidate data cache line
 */
LOCAL void CleanInvalidateDCacheL( VP addr, INT blk )
{
	Asm("0:						\n"
	    "	subs	%1, %1, #1			\n"
	    "	mcr	p15, 0, %0, cr7, c14, 1		\n"	/* Clean and invalidate data cache line */
	    "	add	%0, %0, %2			\n"
	    "	bne	0b				\n"
	    "	mcr	p15, 0, %3, c7, c10, 4		\n"	/* Data synchronization barrier */
		:
		: "r"(addr), "r"(blk), "r"(CACHE_LINESZ), "r"(0)
		: "cc");
}

/*
 * Get cache line size
 */
EXPORT INT GetCacheLineSize( void )
{
	return CACHE_LINESZ;
}

/*
 * Data cache operation
 */
LOCAL void DCacheOp( VP top, INT blk, UINT mode )
{
	if ( (mode & (CC_FLUSH | CC_INVALIDATE)) == CC_FLUSH ) {
		CleanDCacheL(top, blk);

	} else if ( (mode & (CC_FLUSH | CC_INVALIDATE)) == CC_INVALIDATE ) {
		InvalidateDCacheL(top, blk);

	} else {
		CleanInvalidateDCacheL(top, blk);
	}
}

/* Smp-call param */
typedef struct {
	VP	top;
	INT	blk;
	UINT	mode;
} SMPC_CACHE_PARA;

/*
 * Smp-call callback function
 */
LOCAL void smpcb_cache( SMPC_CACHE_PARA *para )
{
	DCacheOp(para->top, para->blk, para->mode);
}

/*
 * Cache control
 *	Control cache between 'laddr' and 'len' bytes areas.
 *	Writeback and invalidate cache
 */
EXPORT ER ControlCacheM( VP laddr, INT len, UINT mode )
{
	VP		top, end;
	INT		rlen, blk;
	SMPC_CACHE_PARA	para;

	if ( ((mode & ~(CC_FLUSH|CC_INVALIDATE|CC_SELF)) != 0)
	  || ((mode & (CC_FLUSH|CC_INVALIDATE)) == 0) ) {
		return E_PAR;
	}

	top = (VP)((UW)laddr & ~(CACHE_LINESZ - 1));
	end = (VP)(((UW)laddr + len + CACHE_LINESZ - 1) & ~(CACHE_LINESZ - 1));
	rlen = (INT)(end - top);
	blk = rlen / CACHE_LINESZ;

	if ( (mode & CC_SELF) != 0 ) {
		DCacheOp(top, blk, mode);
	} else {
		smpc_lock();

		para.top = top;
		para.blk = blk;
		para.mode = mode;
		smpc_req(smpcb_cache, (VP)&para, TRUE, TRUE);

		smpc_unlock();
	}

	return E_OK;
}
