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
 *	@(#)tmon.h (libtm)
 *
 *	T-Monitor library common definitions 
 *
 *	Also accessed from the assembler 
 */

#ifndef _in_asm_source_
#  include <basic.h>
#  include <tm/tmonitor.h>
#else
#  include <machine.h>
#  include "tmsvc.h"
#endif
#include <tk/errno.h>

/*
 * Function code 
 */
#define TM_MONITOR	0
#define TM_GETCHAR	1
#define TM_PUTCHAR	2
#define TM_GETLINE	3
#define TM_PUTSTRING	4
#define TM_COMMAND	5
#define TM_READDISK	6
#define TM_WRITEDISK	7
#define TM_INFODISK	8
#define TM_EXIT		9

#define TM_EXTSVC	255
