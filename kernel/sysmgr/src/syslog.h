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
 *	syslog.h (T-Kernel/SM)
 *	System Log Task Definition
 */

#ifndef _SYSLOG_
#define _SYSLOG_

#define USE_SYSLOG_CONSIO 1		/* Use console IO for system log */

#define MBF_LOG_BUFSZ	(1024*6)	/* Size of message buffer for log */
#define MBF_LOG_MAXMSZ	256		/* Maximum length of log message */

#endif /* _SYSLOG_ */
