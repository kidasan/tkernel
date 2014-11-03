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
 *	sio.c	Boot
 *
 *	Serial I/O processing (for NAVIENGINE)
 */
#include "tmonitor.h"
#include "sysdepend.h"

/*
 * Serial port initialization
 */
EXPORT void initSIO( void )
{
	INT	dll, dlh, ier, fcr, lcr;
	INT	d;

	dll = DLL_REG(DEBUG_PORT_NUM);
	dlh = DLH_REG(DEBUG_PORT_NUM);
	ier = IER_REG(DEBUG_PORT_NUM);
	fcr = FCR_REG(DEBUG_PORT_NUM);
	lcr = LCR_REG(DEBUG_PORT_NUM);
	d = 66500000 / 8 / DEBUG_PORT_BPS;
	if ( get_prid() == 0 ) {
		out_b(lcr, 0x83);
		out_b(dll, (d & 0xff));
		out_b(dlh, (d >> 8));
		out_b(lcr, 0x03);
		out_b(ier, 0x0f);
		out_b(fcr, 0x0f);
	}
}

/*
 * Serial output
 */
EXPORT void putSIO( UB c )
{
	INT	thr, lsr;

	thr = THR_REG(DEBUG_PORT_NUM);
	lsr = LSR_REG(DEBUG_PORT_NUM);
	while ( (in_b(lsr) & LSR5) == 0 ){
		;
	}
	out_b(thr, c);
}

/*
 * Serial input
 */
EXPORT W getSIO( W tmo )
{
	INT	rbr, lsr;

	rbr = RBR_REG(DEBUG_PORT_NUM);
	lsr = LSR_REG(DEBUG_PORT_NUM);
	if ( (in_b(lsr) & LSR0) == 0 ) {
		return -1;
	} else {
		return in_b(rbr);
	}
}
