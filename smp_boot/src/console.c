/*
 *----------------------------------------------------------------------------
 *    Boot Sample
 *
 *    Version: 1.00.00
 *----------------------------------------------------------------------------
 *    Description of license
 *    Original Source:   Copyright (C) 2009 T-Engine Forum. 
 *    This software is distributed under the T-Engine Public License.
 *    Refer to T-Engine Public License at http://www.t-engine.org for use
 *    conditions and redistribution conditions. 
 *    Modification and redistribution are allowed to this software, in
 *    accordance with T-Engine Public License. 
 *    Therefore, the modification may be added to the program distributed by
 *    T-Engine forum.
 *    The original source is available for download from the download page at
 *    http://www.t-engine.org.
 *----------------------------------------------------------------------------
 */

/*
 *	console.c	Boot
 *
 *	Console I/O processing
 */
#include	"tmonitor.h"

/*
 * Single character output 
 */
EXPORT	W	putChar(W c)
{
	if ( c == '\n' ) {
		putSIO('\r');
	}
	putSIO(c);

	return 0;
}

/*
 * Character string output
 */
EXPORT	W	putString(UB *str)
{
	UB	c;

	while ((c = *str++)) {
		putChar(c);
	}

	return 0;
}

/*
 * Single character input
 */
EXPORT	W	getChar(W wait)
{
	W	c;

	do {
		c = getSIO(0);
	} while ( (wait != 0) && (c == -1) );

	return c;
}

/*
 * Single line input
 */
EXPORT	W	getString(UB *str)
{
	UB	*p;
	W	c;

	p = str;
	while ( 1 ) {
		c = getSIO(0);
		if ( c == '\n' ) {
			break;
		}
		if ( 0 < c ) {
			*p++ = c;
		}
	}
	*p = 0;

	return (p - str);
}
