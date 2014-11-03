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
 *	@(#)sysmgr.h (T-Kernel)
 *
 *	T-Kernel/SM
 */

#ifndef __TK_SYSMGR_H__
#define __TK_SYSMGR_H__

#include <basic.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Memory Protection Level
 */
#define TPL_KERNEL	(0)		/* OS, Subsystem, DeviceDriver, ... */
#define TPL_SYSAPPL	(1)		/* System Application */
#define TPL_RESERVED	(2)		/* (Reserved) */
#define TPL_USRAPPL	(3)		/* User Application */
#define TPL_BIT		((UW)(TPL_KERNEL|TPL_SYSAPPL|TPL_RESERVED|TPL_USRAPPL))
#define isTPL(no)	((TPL_KERNEL<=(x))&&((x)<=TPL_USRAPPL))

/*
 * Memory attribute		tk_get_smb
 */
#define TA_NORESIDENT	0x00000010U	/* Non-resident */
#define TA_RNG0		0x00000000U	/* Protection level 0 */
#define TA_RNG1		0x00000100U	/* Protection level 1 */
#define TA_RNG2		0x00000200U	/* Protection level 2 */
#define TA_RNG3		0x00000300U	/* Protection level 3 */
#define TA_NOCACHE	0x00010000U	/* Non-cache area specify */

/*
 * System memory state information
 */
typedef struct t_rsmb {
	INT	blksz;		/* Block size (byte) */
	INT	total;		/* Number of all blocks */
	INT	free;		/* Number of remaining blocks */
} T_RSMB;

/*
 * Subsystem event
 */
#define TSEVT_SUSPEND_BEGIN	1	/* Before device suspend starts */
#define TSEVT_SUSPEND_DONE	2	/* After device suspend completes */
#define TSEVT_RESUME_BEGIN	3	/* Before device resume starts */
#define TSEVT_RESUME_DONE	4	/* After device resume completes */
#define TSEVT_DEVICE_REGIST	5	/* Device registration notification */
#define TSEVT_DEVICE_DELETE	6	/* Device unregistration notification */

/*
 * FlushMemCache() mode
 */
#define	TCM_ICACHE	0x0001	/* Disable instruction cache */
#define	TCM_DCACHE	0x0002	/* Writeback and disable data cache */

/* ------------------------------------------------------------------------ */

/*
 * Non-resident system memory allocation
 */
IMPORT void* Vmalloc( size_t size );
IMPORT void* Vcalloc( size_t nmemb, size_t size );
IMPORT void* Vrealloc( void *ptr, size_t size );
IMPORT void  Vfree( void *ptr );

/*
 * Resident system memory allocation
 */
IMPORT void* Kmalloc( size_t size );
IMPORT void* Kcalloc( size_t nmemb, size_t size );
IMPORT void* Krealloc( void *ptr, size_t size );
IMPORT void  Kfree( void *ptr );

/*
 * Map memory
 */
IMPORT ER MapMemory( VP paddr, INT len, UINT attr, VP *laddr );
IMPORT ER UnmapMemory( VP laddr );

/* ------------------------------------------------------------------------ */

#ifndef __commarea__
#define __commarea__
typedef struct CommArea	CommArea;	/* sys/commarea.h */
#endif

/*
 * Definition for interface library automatic generation (mkiflib)
 */
/*** DEFINE_IFLIB
[INCLUDE FILE]
<tk/sysmgr.h>

[PREFIX]
SYSTEM
***/

/* [BEGIN SYSCALLS] */
/* System information management function */
/* ALIGN_NO 1 */
IMPORT INT tk_get_cfn( UB *name, INT *val, INT max );
IMPORT INT tk_get_cfs( UB *name, UB *buf, INT max );

/* System memory management function */
/* ALIGN_NO 0x100 */
IMPORT ER tk_get_smb( VP *addr, INT nblk, UINT attr );
IMPORT ER tk_rel_smb( VP addr );
IMPORT ER tk_ref_smb( T_RSMB *pk_rsmb );

/* System internal use */
/* ALIGN_NO 0x1000 */
IMPORT ER  _GetKernelCommonArea( CommArea **area );
IMPORT int _syslog_send( const char *string, int len );
/* [END SYSCALLS] */

#ifdef __cplusplus
}
#endif
#endif /* __TK_SYSMGR_H__ */
