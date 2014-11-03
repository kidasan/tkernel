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
 *	svc.c		Boot
 *
 *	SVC processing
 */
#include "tmonitor.h"

/*
 * Branch routine to SVC
 */
EXPORT	W	procSVC(W fno, W p1, W p2, W p3, W p4)
{
	W	er;

	switch(fno) {
	case 0:		/* Enter Monitor */
		/* There is no processing */
		while ( 1 );
		break;

	case 1:		/* Get Character */
		er = getChar(p1);
		break;

	case 2:		/* Put Character */
		er = putChar(p1);
		break;

	case 3:		/* Get Line */
		er = getString((UB*)p1);
		break;

	case 4:		/* Put String */
		er = putString((UB*)p1);
		break;

	case 255:	/* Extension SVC */
		er = procExtSVC(p1, p2, p3, p4);
		break;

	default:
		er = E_PAR;
		break;
	}
	return er;
}
