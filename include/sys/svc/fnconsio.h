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

#define CONSIO_CONSOLE_GET_FN	(0x00100400 | CONSIO_SVC)
#define CONSIO_CONSOLE_PUT_FN	(0x00110400 | CONSIO_SVC)
#define CONSIO_CONSOLE_CONF_FN	(0x00120200 | CONSIO_SVC)
#define CONSIO_CONSOLE_IN_FN	(0x00200300 | CONSIO_SVC)
#define CONSIO_CONSOLE_OUT_FN	(0x00210300 | CONSIO_SVC)
#define CONSIO_CONSOLE_CTL_FN	(0x00220300 | CONSIO_SVC)

