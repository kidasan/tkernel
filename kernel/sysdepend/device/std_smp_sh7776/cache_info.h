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
 *	cache_info.h (SH7776)
 *	Cache Information
 */

#ifndef _CACHE_INFO_
#define _CACHE_INFO_

/*
 * Set non-cache area memory
 *	When using the control table for non-cache area memory 
 *	by memory manager routines, define the address of non-cache area.
 *	
 *	When not using, specify 0 for UseNoCacheMemoryTable.
 */
#define USE_NOCACHE_MEMTBL	(0)	/* Do not use */

#define NoCacheMemoryTop	(0)	/* Top address of non-cache area  */
#define NoCacheMemoryEnd	(0)	/* End address of non-cache area  */

/*
 * Conversion between page number and address
 *	When switching ON/Off of cache by an address, 
 *	define the conversion formula for the following Macro:
 */
#define CachingAddr(p)		( (VP)((UW)(p) & ~0x20000000U) )
#define NoCachingAddr(p)	( (VP)((UW)(p) | 0x20000000U) )

/*
 * Conversion between physical address and logical address of
 * real memory area (physical space)
 */
#define toLogicalAddress(paddr)		(VP)(paddr)
#define toPhysicalAddress(laddr)	(VP)(laddr)

/*
 * Convert logical address into logical address of the cache off area
 */
#define toNoCacheLogicalAddress(laddr)	\
	(VP)(((UW)(laddr) & 0x1fffffffU) | 0xa0000000U)

#endif /* _CACHE_INFO_ */
