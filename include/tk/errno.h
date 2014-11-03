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
 *	@(#)errno.h (T-Kernel)
 *
 *	T-Kernel Error Code
 */

#ifndef __TK_ERRNO_H__
#define __TK_ERRNO_H__

#ifndef _in_asm_source_
#include <tk/typedef.h>

#define MERCD(er)	( (ER)(er) >> 16 )	/* Main error code */
#define SERCD(er)	( (H)(er) )		/* Sub-error code */
#define ERCD(mer, ser)	( (ER)(((UW)(mer) << 16) | ((UW)(ser) & 0x0000FFFF)) )
#else
#define ERCD(mer, ser)	( ((mer) << 16) | ((ser) & 0xffff) )
#endif /* _in_asm_source_ */

#define E_OK		0		/* Completed successfully */

#define E_SYS		ERCD(-5, 0)	/* System error */
#define E_NOCOP		ERCD(-6, 0)	/* Coprocessor disable */
#define E_NOSPT		ERCD(-9, 0)	/* Unsupported function */
#define E_RSFN		ERCD(-10, 0)	/* Reserved function code number */
#define E_RSATR		ERCD(-11, 0)	/* Reserved attribute */
#define E_PAR		ERCD(-17, 0)	/* Parameter error */
#define E_ID		ERCD(-18, 0)	/* Incorrect ID number */
#define E_CTX		ERCD(-25, 0)	/* Context error */
#define E_MACV		ERCD(-26, 0)	/* Inaccessible memory/access violation */
#define E_OACV		ERCD(-27, 0)	/* Object access violation */
#define E_ILUSE		ERCD(-28, 0)	/* Incorrect system call use */
#define E_NOMEM		ERCD(-33, 0)	/* Insufficient memory */
#define E_LIMIT		ERCD(-34, 0)	/* Exceed system limits */
#define E_OBJ		ERCD(-41, 0)	/* Incorrect object state */
#define E_NOEXS		ERCD(-42, 0)	/* Object does not exist */
#define E_QOVR		ERCD(-43, 0)	/* Queuing overflow */
#define E_RLWAI		ERCD(-49, 0)	/* Forcibly release wait state */
#define E_TMOUT		ERCD(-50, 0)	/* Polling fail/time out */
#define E_DLT		ERCD(-51, 0)	/* Waited object was deleted */
#define E_DISWAI	ERCD(-52, 0)	/* Release wait caused by wait disable */

#define E_IO		ERCD(-57, 0)	/* Output/input error */
#define E_NOMDA		ERCD(-58, 0)	/* No media */
#define E_BUSY		ERCD(-65, 0)	/* Busy state */
#define E_ABORT		ERCD(-66, 0)	/* Aborted */
#define E_RONLY		ERCD(-67, 0)	/* write protected */

/* For SMP T-Kernel */
#define E_DOMAIN	ERCD(-68, 0)	/* Domain error */
#define E_ONAME		ERCD(-69, 0)	/* Object name error */
#define E_DACV		ERCD(-70, 0)	/* Inaccessible domain/access violation */

#endif /* __TK_ERRNO_H__ */
