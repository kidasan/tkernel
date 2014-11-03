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
#include <tk/sysmgr.h>
#include <sys/str_align.h>
#include <sys/svc/fnsysmgr.h>

typedef struct {
	UB *name;	_align64
	INT *val;	_align64
	INT max;	_align64
} SYSTEM_TK_GET_CFN_PARA;

typedef struct {
	UB *name;	_align64
	UB *buf;	_align64
	INT max;	_align64
} SYSTEM_TK_GET_CFS_PARA;

typedef struct {
	VP *addr;	_align64
	INT nblk;	_align64
	UINT attr;	_align64
} SYSTEM_TK_GET_SMB_PARA;

typedef struct {
	VP addr;	_align64
} SYSTEM_TK_REL_SMB_PARA;

typedef struct {
	T_RSMB *pk_rsmb;	_align64
} SYSTEM_TK_REF_SMB_PARA;

typedef struct {
	CommArea **area;	_align64
} SYSTEM__GETKERNELCOMMONAREA_PARA;

typedef struct {
	const char *string;	_align64
	int len;	_align64
} SYSTEM__SYSLOG_SEND_PARA;

