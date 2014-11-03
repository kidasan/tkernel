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
#include <tk/devmgr.h>
#include <sys/str_align.h>
#include <sys/svc/fndevmgr.h>

typedef struct {
	UB *devnm;	_align64
	UINT omode;	_align64
} DEVICE_TK_OPN_DEV_PARA;

typedef struct {
	ID dd;	_align64
	UINT option;	_align64
} DEVICE_TK_CLS_DEV_PARA;

typedef struct {
	ID dd;	_align64
	INT start;	_align64
	VP buf;	_align64
	INT size;	_align64
	TMO tmout;	_align64
} DEVICE_TK_REA_DEV_PARA;

typedef struct {
	ID dd;	_align64
	INT start;	_align64
	VP buf;	_align64
	INT size;	_align64
	INT *asize;	_align64
} DEVICE_TK_SREA_DEV_PARA;

typedef struct {
	ID dd;	_align64
	INT start;	_align64
	VP buf;	_align64
	INT size;	_align64
	TMO tmout;	_align64
} DEVICE_TK_WRI_DEV_PARA;

typedef struct {
	ID dd;	_align64
	INT start;	_align64
	VP buf;	_align64
	INT size;	_align64
	INT *asize;	_align64
} DEVICE_TK_SWRI_DEV_PARA;

typedef struct {
	ID dd;	_align64
	ID reqid;	_align64
	INT *asize;	_align64
	ER *ioer;	_align64
	TMO tmout;	_align64
} DEVICE_TK_WAI_DEV_PARA;

typedef struct {
	UINT mode;	_align64
} DEVICE_TK_SUS_DEV_PARA;

typedef struct {
	ID devid;	_align64
	UB *devnm;	_align64
} DEVICE_TK_GET_DEV_PARA;

typedef struct {
	UB *devnm;	_align64
	T_RDEV *rdev;	_align64
} DEVICE_TK_REF_DEV_PARA;

typedef struct {
	ID dd;	_align64
	T_RDEV *rdev;	_align64
} DEVICE_TK_OREF_DEV_PARA;

typedef struct {
	T_LDEV *ldev;	_align64
	INT start;	_align64
	INT ndev;	_align64
} DEVICE_TK_LST_DEV_PARA;

typedef struct {
	ID devid;	_align64
	INT evttyp;	_align64
	VP evtinf;	_align64
} DEVICE_TK_EVT_DEV_PARA;

typedef struct {
	UB *devnm;	_align64
	T_DDEV *ddev;	_align64
	T_IDEV *idev;	_align64
} DEVICE_TK_DEF_DEV_PARA;

typedef struct {
	T_IDEV *idev;	_align64
} DEVICE_TK_REF_IDV_PARA;

