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
 *	Extended SVC parameter packet
 */

#include <basic.h>
#include <sys/segment.h>
#include <sys/str_align.h>
#include <sys/svc/fnsegment.h>

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
} SEG_LOCKSPACE_PARA;

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
} SEG_UNLOCKSPACE_PARA;

typedef struct {
	VP addr;	_align64
	INT len;	_align64
	T_SPINFO *pk_spinfo;	_align64
} SEG_GETSPACEINFO_PARA;

typedef struct {
	VP addr;	_align64
	INT len;	_align64
	UINT mode;	_align64
} SEG_SETCACHEMODE_PARA;

typedef struct {
	VP addr;	_align64
	INT len;	_align64
	UINT mode;	_align64
} SEG_CONTROLCACHE_PARA;

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
	VP *paddr;	_align64
} SEG_CNVPHYSICALADDR_PARA;

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
	UINT mode;	_align64
	UINT env;	_align64
} SEG_CHKSPACE_PARA;

typedef struct {
	TC *str;	_align64
	INT max;	_align64
	UINT mode;	_align64
	UINT env;	_align64
} SEG_CHKSPACETSTR_PARA;

typedef struct {
	UB *str;	_align64
	INT max;	_align64
	UINT mode;	_align64
	UINT env;	_align64
} SEG_CHKSPACEBSTR_PARA;

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
	UINT mode;	_align64
	UINT env;	_align64
	INT lsid;	_align64
} SEG_CHKSPACELEN_PARA;

typedef struct {
	VP laddr;	_align64
	VP buf;	_align64
	INT len;	_align64
	INT lsid;	_align64
} SEG_READMEMSPACE_PARA;

typedef struct {
	VP laddr;	_align64
	VP buf;	_align64
	INT len;	_align64
	INT lsid;	_align64
} SEG_WRITEMEMSPACE_PARA;

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
	_pad_b(24)
	UB data;	_align64
	_pad_l(24)
	INT lsid;	_align64
} SEG_SETMEMSPACEB_PARA;

typedef struct {
	VP laddr;	_align64
	INT len;	_align64
	UINT mode;	_align64
} SEG_FLUSHMEMCACHE_PARA;

typedef struct {
	VP paddr;	_align64
	INT len;	_align64
	UINT attr;	_align64
	VP *laddr;	_align64
} SEG_MAPMEMORY_PARA;

typedef struct {
	VP laddr;	_align64
} SEG_UNMAPMEMORY_PARA;

typedef struct {
	VP laddr;	_align64
	INT npage;	_align64
	INT lsid;	_align64
	UINT pte;	_align64
} SEG_MAKESPACE_PARA;

typedef struct {
	VP laddr;	_align64
	INT npage;	_align64
	INT lsid;	_align64
} SEG_UNMAKESPACE_PARA;

typedef struct {
	VP laddr;	_align64
	INT npage;	_align64
	INT lsid;	_align64
	UINT pte;	_align64
} SEG_CHANGESPACE_PARA;

