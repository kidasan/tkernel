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
#include <sys/consio.h>
#include <sys/str_align.h>
#include <sys/svc/fnconsio.h>

typedef struct {
	W port;	_align64
	B *buf;	_align64
	UW len;	_align64
	W tmout;	_align64
} CONSIO_CONSOLE_GET_PARA;

typedef struct {
	W port;	_align64
	B *buf;	_align64
	UW len;	_align64
	W tmout;	_align64
} CONSIO_CONSOLE_PUT_PARA;

typedef struct {
	W req;	_align64
	UW *arg;	_align64
} CONSIO_CONSOLE_CONF_PARA;

typedef struct {
	W port;	_align64
	B *buf;	_align64
	UW len;	_align64
} CONSIO_CONSOLE_IN_PARA;

typedef struct {
	W port;	_align64
	B *buf;	_align64
	UW len;	_align64
} CONSIO_CONSOLE_OUT_PARA;

typedef struct {
	W port;	_align64
	W req;	_align64
	W arg;	_align64
} CONSIO_CONSOLE_CTL_PARA;

