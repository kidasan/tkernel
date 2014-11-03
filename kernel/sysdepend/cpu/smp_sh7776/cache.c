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
 *	cache.c (SH7776)
 *	Cache Operation
 */

#include <basic.h>
#include <tk/tkernel.h>
#include <tk/sysdef.h>
#include <sys/segment.h>

#define PAGESIZE	0x1000U		/* Page size */
#define CACHE_LINESZ	0x20U		/* Cache line size */

#define ICACHE_ADR_TOP	0xf0000000	/* Instruction cache address array */
#define ICACHE_DAT_TOP	0xf1000000	/* Instruction cache data array */
#define ICACHE_WAY_MSK	0x00006000
#define ICACHE_ENT_MSK	0x00001fe0

#define DCACHE_ADR_TOP	0xf4000000	/* Data cache address array */
#define DCACHE_DAT_TOP	0xf5000000	/* Data cache data array */
#define DCACHE_WAY_MSK	0x00006000
#define DCACHE_ENT_MSK	0x00001fe0

#define CACHE_V		0x00000001	/* Valid */
#define CACHE_U		0x00000002	/* Dirty */
#define CACHE_TAG	0xfffffc00	/* Tag */

#define CACHE_A		0x00000008	/* Associative specification */

/*
 *  One page cache flush
 *	Flush one page cache (4KB) corresponding to logical address 'laddr'.
 *	'laddr' must be top address of page.
 *	
 *	Cache control program must be located at non-cached area,
 *	so P2 area (as shadow area) is used to execute the program.
 *	For this purpose, call the program with address pointed by
 *	'_FlushCache_', and don't call '_flush_cache_' directly.
 */
LOCAL void _flush_cache_( UW laddr, UW mode )
{
	UW	ent;

	if ( (mode & TCM_ICACHE) != 0 ) {
		for ( ent = 0; ent < PAGESIZE; ent += CACHE_LINESZ ) {
			/* Instruction cache */
			Asm("icbi @%0":: "r"(laddr + ent));
		}
	}
	if ( (mode & TCM_DCACHE) != 0 ) {
		for ( ent = 0; ent < PAGESIZE; ent += CACHE_LINESZ ) {
			/* Data cache */
			Asm("ocbp @%0":: "r"(laddr + ent));
		}
	}
}

LOCAL FP _FlushCacheP2_ = (FP)((UW)&_flush_cache_ + 0x20000000);

#define FlushCacheP2(laddr, mode)	(*_FlushCacheP2_)(laddr, mode)

/*
 * Cache flush
 *	Flush cache between 'laddr' and 'len' bytes areas.
 *	Writeback and disable cache
 */
EXPORT void FlushCacheM( VP laddr, INT len, UINT mode )
{
	UW	top, end;
	UINT	imask;

	top = (UW)laddr & ~(PAGESIZE-1);
	end = (UW)laddr + len;

	DI(imask);
	while ( top < end ) {
		FlushCacheP2(top, mode);
		top += PAGESIZE;
	}
	EI(imask);
}

EXPORT void FlushCache( VP laddr, INT len )
{
	FlushCacheM(laddr, len, TCM_ICACHE|TCM_DCACHE);
}

/*
 *  One line cache flush
 *	Flush one line cache (32B) corresponding to logical address 'laddr'.
 *	'laddr' must be top address of cache line.
 *	
 *	Cache control program must be located at non-cached area,
 *	so P2 area (as shadow area) is used to execute the program.
 *	For this purpose, call the program with address pointed by
 *	'_FlushCacheLine_', and don't call '_flush_cache_line_' directly.
 */
LOCAL void _flush_cache_line_( UW laddr, INT size, UW mode )
{
	UW	ent;

	if ( (mode & (CC_FLUSH | CC_INVALIDATE)) == CC_FLUSH ) {
		for ( ent = 0; ent < size; ent += CACHE_LINESZ ) {
			/* Writeback data cache */
			Asm("ocbwb @%0":: "r"(laddr + ent));
		}
	} else if ( (mode & (CC_FLUSH | CC_INVALIDATE)) == CC_INVALIDATE ) {
		for ( ent = 0; ent < size; ent += CACHE_LINESZ ) {
			/* Invalidate instruction cache */
			Asm("icbi @%0":: "r"(laddr + ent));
			/* Invalidate data cache */
			Asm("ocbi @%0":: "r"(laddr + ent));
		}
	} else {
		for ( ent = 0; ent < size; ent += CACHE_LINESZ ) {
			/* Invalidate instruction cache */
			Asm("icbi @%0":: "r"(laddr + ent));
			/* Writeback & Invalidate data cache */
			Asm("ocbp @%0":: "r"(laddr + ent));
		}
	}
}

LOCAL FP _FlushCacheLineP2_ = (FP)((UW)&_flush_cache_line_ + 0x20000000);

#define FlushCacheLineP2(laddr, size, mode)	(*_FlushCacheLineP2_)(laddr, size, mode)

/*
 * Cache control
 *	Control cache between 'laddr' and 'len' bytes areas.
 *	Writeback and invalidate cache
 */
EXPORT ER ControlCacheM( VP laddr, INT len, UINT mode )
{
	UW	top, end;
	UINT	imask;
	INT	rlen;

	if ( (mode & CC_SELF) != 0 ) {
		return E_NOSPT;
	}
	if ( (mode & ~(CC_FLUSH|CC_INVALIDATE)) != 0 ) {
		return E_PAR;
	}

	top   = (UW)laddr & ~(CACHE_LINESZ-1);
	end   = ((UW)laddr + len + (CACHE_LINESZ-1)) & ~(CACHE_LINESZ-1);
	rlen  = end - top;

	DI(imask);
	FlushCacheLineP2(top, rlen, mode);
	EI(imask);

	return E_OK;
}

/*
 * Get cache line size
 */
EXPORT INT GetCacheLineSize( void )
{
	return CACHE_LINESZ;
}
