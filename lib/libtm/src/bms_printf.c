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
 *	@(#)bms_printf.c (libdbg)
 *
 *	printf() using monitor
 */

#include <tm/tmonitor.h>
#include <stdarg.h>

extern int vsprintf( char *s, const char *format, va_list arg );

EXPORT int bms_printf( const char *format, ... )
{
	char	buf[1024];
	int	len;
	va_list	ap;

	va_start(ap, format);
	len = vsprintf(buf, format, ap);
	va_end(ap);

	tm_putstring((UB*)buf);

	return len;
}
