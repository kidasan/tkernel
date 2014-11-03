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
 *	extsvc.c	Boot
 *
 *	Extended SVC processing (for NAVIENGINE)
 */
#include "tmonitor.h"
#include "sysdepend.h"

/*
 * Branch routine to extended SVC
 */
EXPORT	W	procExtSVC(W fno, W p1, W p2, W p3)
{
	W	er;

	switch(fno) {
	case 0x00:	/* Debug Port Speed */
		er = DEBUG_PORT_BPS;
		break;

	case 0x04:	/* Debug Port Number */
		er = DEBUG_PORT_NUM;
		break;

	default:
		er = E_PAR;
		break;
	}
	return er;
}
