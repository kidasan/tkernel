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
 *	@(#)segment.h (sys)
 *
 *	Segment management
 */

#ifndef __SYS_SEGMENT_H__
#define __SYS_SEGMENT_H__

#include <sys/sysdepend/segment_common.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INVADR			((VP)-1)	/* Invalid address */

#ifndef __pinfo__
typedef struct pinfo		PINFO;		/* Defined in sys/pinfo.h */
#define __pinfo__
#endif
#ifndef __diskinfo__
typedef struct diskinfo		DiskInfo;	/* Defined in device/disk.h */
#define __diskinfo__
#endif

struct phyblk {
	UW	blk;		/* Physical block number */
	UW	len;		/* Number of physical blocks */
};
#ifndef __phyblk__
typedef struct phyblk		PhyBlk;
#define __phyblk__
#endif

/*
 * CheckSpace(),CheckStrSpace() mode
 */
#define MA_READ		0x04U	/* Read */
#define MA_WRITE	0x02U	/* Write */
#define MA_EXECUTE	0x01U	/* Execute */

/*
 * GetSpaceInfo()
 */
typedef struct t_spinfo {
	VP	paddr;
	VP	page;
	INT	pagesz;
	INT	cachesz;
	INT	cont;
} T_SPINFO;

/*
 * SetCacheMode() mode
 */
#define CM_OFF		0x01
#define CM_WB		0x02
#define CM_WT		0x03
#define CM_CONT		0x10

/*
 * ControlCache() mode
 */
#define CC_FLUSH	0x01			/* Cache Fluch (write back) */
#define CC_INVALIDATE	0x02			/* Cache Invalidate */
#define CC_SELF		0x04			/* Only my processor specifies */


/*
 * Definitions for interface library auto generate (mkiflib)
 */
/*** DEFINE_IFLIB
[INCLUDE FILE]
<sys/segment.h>

[PREFIX]
SEG
***/

/* [BEGIN SYSCALLS] */

/* ALIGN_NO 0x100 */
#ifndef LockSpace
IMPORT ER  LockSpace( VP laddr, INT len );
IMPORT ER  UnlockSpace( VP laddr, INT len );
#endif
IMPORT ER GetSpaceInfo( VP addr, INT len, T_SPINFO *pk_spinfo );
IMPORT INT SetCacheMode( VP addr, INT len, UINT mode );
IMPORT INT ControlCache( VP addr, INT len, UINT mode );
IMPORT INT CnvPhysicalAddr( VP laddr, INT len, VP *paddr );
IMPORT ER  ChkSpace( VP laddr, INT len, UINT mode, UINT env );
IMPORT INT ChkSpaceTstr( TC *str, INT max, UINT mode, UINT env );
IMPORT INT ChkSpaceBstr( UB *str, INT max, UINT mode, UINT env );
IMPORT INT ChkSpaceLen( VP laddr, INT len, UINT mode, UINT env, INT lsid );
IMPORT INT ReadMemSpace( VP laddr, VP buf, INT len, INT lsid );
IMPORT INT WriteMemSpace( VP laddr, VP buf, INT len, INT lsid );
IMPORT INT SetMemSpaceB( VP laddr, INT len, UB data, INT lsid );
IMPORT ER  FlushMemCache( VP laddr, INT len, UINT mode );

/* ALIGN_NO 0x100 */
IMPORT ER MapMemory( VP paddr, INT len, UINT attr, VP *laddr );
IMPORT ER UnmapMemory( VP laddr );

/* ALIGN_NO 0x1000 */
IMPORT ER MakeSpace( VP laddr, INT npage, INT lsid, UINT pte );
IMPORT ER UnmakeSpace( VP laddr, INT npage, INT lsid );
IMPORT ER ChangeSpace( VP laddr, INT npage, INT lsid, UINT pte );
/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif
#endif /* __SYS_SEGMENT_H__ */
