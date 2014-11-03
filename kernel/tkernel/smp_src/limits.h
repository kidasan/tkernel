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
 *	limits.h
 *
 *	C language: integer size 
 */

#ifndef _LIMITS_
#define _LIMITS_

#define CHAR_BIT	(8)
#define SCHAR_MIN	(-128)
#define SCHAR_MAX	(+127)
#define UCHAR_MAX	(255)
#define CHAR_MIN	SCHAR_MIN
#define CHAR_MAX	SCHAR_MAX
#define MB_LEN_MAX	(2)

#define SHRT_MIN	(-32768)
#define SHRT_MAX	(+32767)
#define USHRT_MAX	(65535)

#define LONG_MIN	(-2147483648L)
#define LONG_MAX	(+2147483647L)
#define ULONG_MAX	(4294967295L)

#define INT_MIN		LONG_MIN
#define INT_MAX		LONG_MAX
#define UINT_MAX	ULONG_MAX

#endif /* _LIMITS_ */
