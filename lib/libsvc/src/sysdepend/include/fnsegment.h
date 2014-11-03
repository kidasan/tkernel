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
 *	Extended SVC function code
 */

#include <sys/ssid.h>

#define SEG_LOCKSPACE_FN	(0x01000200 | SEG_SVC)
#define SEG_UNLOCKSPACE_FN	(0x01010200 | SEG_SVC)
#define SEG_GETSPACEINFO_FN	(0x01020300 | SEG_SVC)
#define SEG_SETCACHEMODE_FN	(0x01030300 | SEG_SVC)
#define SEG_CONTROLCACHE_FN	(0x01040300 | SEG_SVC)
#define SEG_CNVPHYSICALADDR_FN	(0x01050300 | SEG_SVC)
#define SEG_CHKSPACE_FN	(0x01060400 | SEG_SVC)
#define SEG_CHKSPACETSTR_FN	(0x01070400 | SEG_SVC)
#define SEG_CHKSPACEBSTR_FN	(0x01080400 | SEG_SVC)
#define SEG_CHKSPACELEN_FN	(0x01090500 | SEG_SVC)
#define SEG_READMEMSPACE_FN	(0x010a0400 | SEG_SVC)
#define SEG_WRITEMEMSPACE_FN	(0x010b0400 | SEG_SVC)
#define SEG_SETMEMSPACEB_FN	(0x010c0400 | SEG_SVC)
#define SEG_FLUSHMEMCACHE_FN	(0x010d0300 | SEG_SVC)
#define SEG_MAPMEMORY_FN	(0x02000400 | SEG_SVC)
#define SEG_UNMAPMEMORY_FN	(0x02010100 | SEG_SVC)
#define SEG_MAKESPACE_FN	(0x10000400 | SEG_SVC)
#define SEG_UNMAKESPACE_FN	(0x10010300 | SEG_SVC)
#define SEG_CHANGESPACE_FN	(0x10020400 | SEG_SVC)

