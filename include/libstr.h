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
 *	@(#)libstr.h
 *
 *	Standard library for kernel link
 *
 */

#ifndef	__LIBSTR_H__
#define __LIBSTR_H__

#include <stdtype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	__size_t
typedef __size_t	size_t;
#undef	__size_t
#endif

#ifdef	__wchar_t
typedef __wchar_t	wchar_t;
#undef	__wchar_t
#endif

#define NULL		0

extern void* memset( void *s, int c, size_t n );
extern int memcmp( const void *s1, const void *s2, size_t n );
extern void* memcpy( void *dst, const void *src, size_t n );
extern void* memmove( void *dst, const void *src, size_t n );
extern void bzero( void *s, size_t n );

extern size_t strlen( const char *s );
extern int strcmp( const char *s1, const char *s2 );
extern int strncmp( const char *s1, const char *s2, size_t n );
extern char* strcpy( char *dst, const char *src );
extern char* strncpy( char *dst, const char *src, size_t n );
extern char* strcat( char *dst, const char *src );
extern char* strncat( char *dst, const char *src, size_t n );

extern long int strtol( const char *nptr, char **endptr, int base );

#ifdef __cplusplus
}
#endif
#endif /* __LIBSTR_H__ */
