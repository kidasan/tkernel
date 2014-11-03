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
 *	tmonitor.h	Boot
 *
 *	common definition
 */

#include <basic.h>
#include <tk/errno.h>

/*
 * External function
 */
/* svc.c */
IMPORT	W	procSVC(W fno, W p1, W p2, W p3, W p4);

/* console.c */
IMPORT	W	putChar(W c);
IMPORT	W	putString(UB *str);
IMPORT	W	getChar(W wait);
IMPORT	W	getString(UB *str);

/* extsvc.c */
IMPORT	W	procExtSVC(W fno, W p1, W p2, W p3);

/* sio.c */
IMPORT	void	initSIO(void);
IMPORT	void	putSIO(UB ch);
IMPORT	W	getSIO(W tmo);
