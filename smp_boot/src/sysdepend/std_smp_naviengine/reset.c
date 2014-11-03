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
 *	reset.c		Boot
 *
 *	Reset processing
 */
#include "tmonitor.h"
#include "sysdepend.h"

/* Page table */
#define SSEC(n)		((((n) << 20) & 0xFF000000) | (1 << 18))
#define NSEC(n)		((((n) << 20) & 0xFFF00000))
#define AP(a,b,c)	(((a) << 15) | ((b) << 11) | ((c) << 10) | 0x2)
#define _X		(1 << 4)
#define _C		(1 << 3)
#define _B		(1 << 2)
#define _S		(1 << 16)
#define _TEX(a,b,c)	(((a) << 14) | ((b) << 13) | ((c) << 12))

/* SVC lock */
EXPORT UW svclock;

/*
 * System initialization processing
 */
LOCAL void resetSystem( void )
{
	UINT	prid = get_prid();
	UW	dat;
	INT	i;
	UW	n, *p;

	/* Initialize lock */
	if ( prid == 0 ) {
		svclock = 0;
	}

	/* Initialize SCU */
	if ( prid == 0 ) {
		out_w(SCU_CTRL,  in_w(SCU_CTRL) | 0x01);
		out_w(SCU_INVALL, 0x0000ffff);
	}

	/* Initialize DIC */
	/* Disable CPU interface */
	out_w(DIC_CPUCTRL, in_w(DIC_CPUCTRL) & ~1);
	if ( prid == 0 ) {
		/* Disable DIC */
		out_w(DIC_CTRL, in_w(DIC_CTRL) & ~1);

		/* Disable interrupt */
		/* Clear pending state */
		for ( i = 0; i < 256; i += 32 ) {
			out_w(DIC_ENBCLR(i), 0xffffffff);
			out_w(DIC_PENDCLR(i), 0xffffffff);
		}
		/* Interrupt configuration */
		for ( i = 0; i < 256; i += 16 ) {
			out_w(DIC_CONFIG(i), 0xaaaaaaaa);	/* Edge , 1-N */
		}
		/* CPU targets */
		/* Interrupt priority */
		for ( i = 0; i < 32  ; i += 4 ) {
			out_w(DIC_PRI(i), 0xf0f0f0f0);		/* Lowest priority */
		}
		for ( i = 32; i < 256  ; i += 4 ) {
			out_w(DIC_CPUTRG(i), 0x00000000);	/* No target */
			out_w(DIC_PRI(i), 0xf0f0f0f0);		/* Lowest priority */
		}
	} else {
		/* Disable interrupt */
		/* Clear pending state */
		for ( i = 0; i < 32; i += 32 ) {
			out_w(DIC_ENBCLR(i), 0xffffffff);
			out_w(DIC_PENDCLR(i), 0xffffffff);
		}
		/* Interrupt configuration */
		for ( i = 0; i < 32; i += 16 ) {
			out_w(DIC_CONFIG(i), 0xaaaaaaaa);	/* Edge , 1-N */
		}
		/* CPU targets */
		/* Interrupt priority */
		for ( i = 0; i < 32  ; i += 4 ) {
			out_w(DIC_PRI(i), 0xf0f0f0f0);		/* Lowest priority */
		}
	}

	/* Initialize CPU interface */
	/* Priority mask */
	out_w(DIC_PRIMSK, 0xF0);	/* Not masked */
	/* Binary Point */
	out_w(DIC_BINPNT, 0x03);	/* All priority bits are compared */
	/* Enable CPU interface */
	out_w(DIC_CPUCTRL, in_w(DIC_CPUCTRL) | 0x01);
	if ( prid == 0 ) {
		/* Enable DIC */
		out_w(DIC_CTRL, in_w(DIC_CTRL) | 0x01);
	}

	while ( 1 ) {
		dat = in_w(DIC_INTACK);
		if ( (dat & 0x3ff) == 0x3ff ) {
			break;
		}
		out_w(DIC_INTEOI, dat);
	}
	for ( i = 16; i < 256; i++ ) {
		dat = in_w(DIC_ACTIVE(i));
		if ( (dat & (1<<(i%32))) != 0 ) {
			out_w(DIC_INTEOI, i);
		}
	}

	/* Initialize sysInfo */
	if ( prid == 0 ) {
		/* 'sysInfo->work' is already cleared */
		for ( p = (UW*)sysInfo; p < (UW*)sysInfo->work; ) {
			*p++ = 0;
		}

		/* Set EIT vector */
		sysInfo->eitvec[0] = _defaultHdr;	/* Default handler */
		sysInfo->eitvec[4] = _svcHdr;		/* SWI - SVC */
		sysInfo->eitvec[32+94] = _irqGPIOHdr;	/* GPIO handler */

		/* RAM */
		sysInfo->ramtop = (VP)RAM_TOP;
		n = (UW)romInfo->userarea;
		if ( n <= MIN_RAM_END || n >= RAM_END ) {
			n = DEF_RAM_END;
		}
		sysInfo->ramend = (VP)n;

		/* SYSCONF / DEVCONF */
		sysInfo->sysconf = romInfo->sysconf;
		sysInfo->devconf = romInfo->devconf;

		/* Clock - None */

		/* Initialize page table */
		/* ROM				AP=1 01, X=0, C=1, B=0 */
		for ( n = 0x000; n < 0x040; n++ ) {
			sysInfo->pagetbl[n] = NSEC(n) | AP(1,0,1) | _C;
		}
		/* FPGA				AP=0 01, X=1, C=0, B=0 */
		for ( n = 0x040; n < 0x060; n++ ) {
			sysInfo->pagetbl[n] = NSEC(n) | AP(0,0,1) | _X;
		}
		/* IO (1)			AP=0 01, X=1, C=0, B=0 */
		for ( n = 0x180; n < 0x181; n++ ) {
			sysInfo->pagetbl[n] = NSEC(n) | AP(0,0,1) | _X;
		}
		/* RAM 				AP=0 01, X=0, C=1, B=1 */
		for ( n = 0x800; n < 0x900; n++ ) {
			sysInfo->pagetbl[n] = NSEC(n) | AP(0,0,1) | _C | _B | _S | _TEX(0,0,1);
		}
		/* MPCore Private		AP=0 01, X=1, C=0, B=0 */
		for ( n = 0xC00; n < 0xC01; n++ ) {
			sysInfo->pagetbl[n] = NSEC(n) | AP(0,0,1) | _X;
		}
	}
}

/*
 * Reset processing
 */
EXPORT void procReset( void )
{
	/* System initialization */
	resetSystem();

	/* Serial initialization */
	if ( get_prid() == 0 ) {
		initSIO();
	}
}
