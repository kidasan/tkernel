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
 *      gsyscnf.c (sysinit)
 *      SYSCONF access utilities
 */

#include "sysinit.h"
#include <sys/util.h>
#include <sys/sysinfo.h>
#include <libstr.h>
#define SysConfPtr	(SCInfo.sysconf)

LOCAL	UB	*SearchSysConf(UB *name)
{
	W	len;
	UB	*p;

	len = (W)strlen((char*)name);
	if (len > 0) {
		if (len > 16) {
			len = 16;
		}
		for (p = SysConfPtr; *p; ) {
			if (*p > ' ' && *p != '#' && memcmp(name, p, (size_t)len) == 0) {
				return p;
			}
			while (*p && *p++ != '\n') {
				;
			}
		}
	}
	return	(UB*)NULL;
}

LOCAL	UB	*SkipNext(UB *p)
{
	for (; *p > ' ' && *p != '#'; p++) {
		;
	}
	for ( ; ; p++) {
		if (*p == '\0' || *p == '\n' || *p == '#') {
			return (UB*)NULL;
		}
		if (*p > ' ') {
			break;
		}
	}
	return p;
}

EXPORT	W	GetSysConf(UB *name, W *val)
{
	W	cnt = 0;
	UB	*p;

	if ( (p = SearchSysConf(name)) != NULL ) {
		for ( ; (p = SkipNext(p)) && cnt < L_SYSCONF_VAL; cnt++) {
			*val++ = (W)strtol((char*)p, (char**)&p, 0);
		}
	}
	return cnt;
}

EXPORT	W	GetSysConfStr(UB *name, UB *str)
{
	UB	*p;
	UB	*d = str;

	if ( (p = SearchSysConf(name)) != NULL ) {
		if ( (p = SkipNext(p)) != NULL ) {
			while (*p && *p != '\n' && *p != '#') {
				*d++ = *p++;
				if (d - str >= L_SYSCONF_STR - 1) {
					break;
				}
			}
		}
	}
	*d = 0;
	return d - str;
}
