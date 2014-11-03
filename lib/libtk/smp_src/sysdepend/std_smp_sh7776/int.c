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
 *	@(#)int.c (libtk/SH7776)
 *
 *	Interrupt controller  
 */

#include <basic.h>
#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <sys/sysinfo.h>
#include <tk/smp_util.h>
#include <sys/kernel_util.h>

typedef struct {
	UW	regaddr0;			/* Interrupt priority register address(CPU0) */
	UW	regaddr1;			/* Interrupt priority register address(CPU1) */
	UINT	shift:24;
	UINT	mask:8;
} INTPRITBL;

LOCAL INTPRITBL intpritable[N_INTVEC] = {
	{ NULL, NULL, 0, 0x00 },		/* 0x000 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x020 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x040 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x060 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x080 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x0A0 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x0C0 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x0E0 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x100 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x120 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x140 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x160 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x180 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x1A0 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x1C0 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x1E0 : Reserved (CPU exception) */
	{ NULL, NULL, 0, 0x00 },		/* 0x200 : Reserved (IRL[3:0] 15) */
	{ NULL, NULL, 0, 0x00 },		/* 0x220 : Reserved (IRL[3:0] 14) */
	{ C0INTPRI, C1INTPRI, 28, 0x0f },	/* 0x240 : IRQ[0] */
	{ NULL, NULL, 0, 0x00 },		/* 0x260 : Reserved (IRL[3:0] 12) */
	{ C0INTPRI, C1INTPRI, 24, 0x0f },	/* 0x280 : IRQ[1] */
	{ NULL, NULL, 0, 0x00 },		/* 0x2A0 : Reserved (IRL[3:0] 10) */
	{ C0INTPRI, C1INTPRI, 20, 0x0f },	/* 0x2C0 : IRQ[2] */
	{ NULL, NULL, 0, 0x00 },		/* 0x2E0 : Reserved (IRL[3:0] 8) */
	{ C0INTPRI, C1INTPRI, 16, 0x0f },	/* 0x300 : IRQ[3] */
	{ NULL, NULL, 0, 0x00 },		/* 0x320 : Reserved (IRL[3:0] 6) */
	{ C0INTPRI, C1INTPRI, 12, 0x0f },	/* 0x340 : IRQ[4] */
	{ NULL, NULL, 0, 0x00 },		/* 0x360 : Reserved (IRL[3:0] 4) */
	{ C0INTPRI, C1INTPRI, 8, 0x0f },	/* 0x380 : IRQ[5] */
	{ NULL, NULL, 0, 0x00 },		/* 0x3A0 : Reserved (IRL[3:0] 2) */
	{ C0INTPRI, C1INTPRI, 4, 0x0f },	/* 0x3C0 : IRQ[6] */
	{ C0INT2PRI0, C1INT2PRI0, 0,  0x1f },	/* 0x3E0 : DU */
	{ C0INT2PRI1, C1INT2PRI1, 24, 0x1f },	/* 0x400 : TMU-ch0 */
	{ C0INT2PRI1, C1INT2PRI1, 16, 0x1f },	/* 0x420 : TMU-ch1 */
	{ C0INT2PRI1, C1INT2PRI1, 8,  0x1f },	/* 0x440 : TMU-ch2 */
	{ C0INT2PRI1, C1INT2PRI1, 0,  0x1f },	/* 0x460 : TMU-ch2 input capture */
	{ C0INT2PRI2, C1INT2PRI2, 24, 0x1f },	/* 0x480 : TMU-ch3 */
	{ C0INT2PRI2, C1INT2PRI2, 16, 0x1f },	/* 0x4A0 : TMU-ch4 */
	{ C0INT2PRI2, C1INT2PRI2, 8,  0x1f },	/* 0x4C0 : TMU-ch5 */
	{ C0INT2PRI2, C1INT2PRI2, 0,  0x00 },	/* 0x4E0 : TMU-ch5 input capture */
	{ C0INT2PRI3, C1INT2PRI3, 24, 0x1f },	/* 0x500 : TMU-ch6 */
	{ C0INT2PRI3, C1INT2PRI3, 16, 0x1f },	/* 0x520 : TMU-ch7 */
	{ C0INT2PRI3, C1INT2PRI3, 8,  0x1f },	/* 0x540 : TMU-ch8, TMU-ch8 input capture */
	{ C0INT2PRI3, C1INT2PRI3, 0,  0x1f },	/* 0x560 : TMU-ch9, TMU-ch10 */
	{ C0INT2PRI4, C1INT2PRI4, 24, 0x1f },	/* 0x580 : USB2.0 h/f */
	{ C0INT2PRI4, C1INT2PRI4, 16, 0x1f },	/* 0x5A0 : SIU2 */
	{ C0INT2PRI4, C1INT2PRI4, 8,  0x1f },	/* 0x5C0 : FM multiplex decoder */
	{ C0INT2PRI4, C1INT2PRI4, 0,  0x1f },	/* 0x5E0 : SATA */
	{ C0INT2PRI5, C1INT2PRI5, 24, 0x1f },	/* 0x600 : H-UDI0, H-UDI1 */
	{ C0INT2PRI5, C1INT2PRI5, 16, 0x1f },	/* 0x620 : SHwyDMAC */
	{ C0INT2PRI5, C1INT2PRI5, 8,  0x1f },	/* 0x640 : YUV */
	{ C0INT2PRI5, C1INT2PRI5, 0,  0x1f },	/* 0x660 : TSIF */
	{ C0INT2PRI6, C1INT2PRI6, 24, 0x1f },	/* 0x680 : 2bit ADCIF */
	{ C0INT2PRI6, C1INT2PRI6, 16, 0x1f },	/* 0x6A0 : Remocon */
	{ C0INT2PRI6, C1INT2PRI6, 8,  0x1f },	/* 0x6C0 : SSI0,1,2,3 */
	{ C0INT2PRI6, C1INT2PRI6, 0,  0x1f },	/* 0x6E0 : PCIE0 */
	{ C0INT2PRI7, C1INT2PRI7, 24, 0x1f },	/* 0x700 : PCIE1 */
	{ C0INT2PRI7, C1INT2PRI7, 16, 0x1f },	/* 0x720 : PCIE2 */
	{ C0INT2PRI7, C1INT2PRI7, 8,  0x1f },	/* 0x740 : VIN0,1,2 */
	{ C0INT2PRI7, C1INT2PRI7, 0,  0x1f },	/* 0x760 : MIMSLB */
	{ C0INT2PRI8, C1INT2PRI8, 24, 0x1f },	/* 0x780 : R-GP1 */
	{ C0INT2PRI8, C1INT2PRI8, 16, 0x1f },	/* 0x7A0 : 3DG */
	{ NULL, NULL,     0,  0x00 },		/* 0x7C0 : Reserved */
	{ C0INT2PRI8, C1INT2PRI8, 0,  0x1f },	/* 0x7E0 : HSPI0,1 */
	{ NULL, NULL,     0,  0x00 },		/* 0x800 : Reserved (General FPU disable exception) */
	{ NULL, NULL,     0,  0x00 },		/* 0x820 : Reserved (Slot FPU disable exception) */
	{ NULL, NULL,     0,  0x00 },		/* 0x840 : Reserved */
	{ C0INT2PRI9, C1INT2PRI9, 0,  0x1f },	/* 0x860 : I2C0,1 */
	{ C0INT2PRI10, C1INT2PRI10, 24, 0x1f },	/* 0x880 : RCAN(Mailbox), RCAN */
	{ C0INT2PRI10, C1INT2PRI10, 16, 0x1f },	/* 0x8A0 : RESET/WDT */
	{ C0INT2PRI10, C1INT2PRI10, 8,  0x1f },	/* 0x8C0 : SCIF-ch0 (ERI5, RXI5, BRI5, TXI5) */
	{ C0INT2PRI10, C1INT2PRI10, 0,  0x1f },	/* 0x8E0 : SCIF-ch1-4 */
	{ C0INT2PRI11, C1INT2PRI11, 24, 0x1f },	/* 0x900 : SCIF-ch5-8 */
	{ C0INT2PRI11, C1INT2PRI11, 16, 0x1f },	/* 0x920 : SDIF0,1 */
	{ NULL, NULL,      0,  0x00 },		/* 0x940 : Reserved */
	{ C0INT2PRI11, C1INT2PRI11, 0,  0x1f },	/* 0x960 : Temperature sensor */
	{ C0INT2PRI12, C1INT2PRI12, 24, 0x1f },	/* 0x980 : GPIO0,1,2,3,4 */
	{ C0INT2PRI12, C1INT2PRI12, 16, 0x1f },	/* 0x9A0 : PAM */
	{ C0INT2PRI12, C1INT2PRI12, 8,  0x1f },	/* 0x9C0 : IMR */
	{ NULL, NULL,      0,  0x00 },		/* 0x9E0 : Reserved */
	{ C0INT2PRI13, C1INT2PRI13, 24, 0x1f },	/* 0xA00 : DTU0 */
	{ C0INT2PRI13, C1INT2PRI13, 16, 0x1f },	/* 0xA20 : DTU1 */
	{ C0INT2PRI13, C1INT2PRI13, 8,  0x1f },	/* 0xA40 : V-coreX */
	{ NULL, NULL,      0,  0x00 },		/* 0xA60 : Reserved */
	{ C0INT2PRI14, C1INT2PRI14, 24, 0x1f },	/* 0xA80 : AESOP */
	{ C0INT2PRI14, C1INT2PRI14, 16, 0x1f },	/* 0xAA0 : GPS */
	{ C0INT2PRI14, C1INT2PRI14, 8,  0x1f },	/* 0xAC0 : HPB-DMAC ch0-15 */
	{ C0INT2PRI14, C1INT2PRI14, 0,  0x1f },	/* 0xAE0 : LBSC-DMAC ch0-5, LBSC-ATA, LBSC-WTOE, MTSB */
	{ NULL, NULL,      0,  0x00 },		/* 0xB00 : Reserved 00(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xB20 : Reserved 01(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xB40 : Reserved 02(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xB60 : Reserved 03(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xB80 : Reserved 04(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xBA0 : Reserved 05(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xBC0 : Reserved 06(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xBE0 : Reserved 07(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xC00 : Reserved 08(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xC20 : Reserved 09(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xC40 : Reserved 10(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xC60 : Reserved 11(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xC80 : Reserved 12(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xCA0 : Reserved 13(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xCC0 : Reserved 14(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xCE0 : Reserved 15(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xD00 : Reserved 16(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xD20 : Reserved 17(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xD40 : Reserved 18(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xD60 : Reserved 19(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xD80 : Reserved 20(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xDA0 : Reserved 21(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xDC0 : Reserved 22(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xDE0 : Reserved 23(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xE00 : Reserved 24(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xE20 : Reserved 25(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xE40 : Reserved 26(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xE60 : Reserved 27(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xE80 : Reserved 28(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xEA0 : Reserved 29(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xEC0 : Reserved 30(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xEE0 : Reserved 31(IO-CHIP) */
	{ NULL, NULL,      0,  0x00 },		/* 0xF00 : Reserved (INTICI0) */
	{ NULL, NULL,      0,  0x00 },		/* 0xF20 : Reserved (INTICI1) */
	{ NULL, NULL,      0,  0x00 },		/* 0xF40 : Reserved (INTICI2) */
	{ NULL, NULL,      0,  0x00 },		/* 0xF60 : Reserved (INTICI3) */
	{ NULL, NULL,      0,  0x00 },		/* 0xF80 : Reserved (INTICI4) */
	{ NULL, NULL,      0,  0x00 },		/* 0xFA0 : Reserved (INTICI5) */
	{ NULL, NULL,      0,  0x00 },		/* 0xFC0 : Reserved (INTICI6) */
	{ NULL, NULL,      0,  0x00 },		/* 0xFE0 : Reserved (INTICI7) */
};

/*
 * Interrupt disable/enable
 */
Inline UINT _disint( void )
{
	UINT	imask;

	Asm("	stc	sr, r0	\n"
	"	mov	r0, %0	\n"
	"	or	%1, r0	\n"
	"	ldc	r0, sr	"
		: "=r"(imask)
		: "i"(SR_I(15))
		: "r0" );

	return imask;
}
Inline void _enaint( UINT imask )
{
	Asm("	ldc	%0, sr" :: "r"(imask));
}

#define _DI(imask)	( imask = _disint() )
#define _EI(imask)	( _enaint(imask) )

/*
 * Enable interrupt
 *	Enable the external interrupts specified by 'intvec' by setting
 *	priority level to 'level'. '1'-'15' is valid for 'level' and '15' is
 *	the highest priority. If 'level' is invalid value, operation is not
 *	guaranteed.
 *	Enable the peripheral module interrupts specified by 'intvec' by
 *	setting priority level to 'level'. '2'-'31' is valid for 'level' and
 *	'31' is the highest priority. If 'level' is invalid value, operation
 *	is not 	guaranteed.
 */
EXPORT void EnableInt( INTVEC intvec, INT level )
{
	INTPRITBL	*tbl;
	INT	vecno = intvec >> 5;
	UINT	msk, imask;
	UW	regdata;

	if( vecno < N_INTVEC ) {
		tbl = &intpritable[vecno];

		if ( tbl->regaddr0 != NULL ) {
			msk = ~((UINT)tbl->mask << tbl->shift);
			level = (level & tbl->mask) << tbl->shift;

			_DI(imask);
			regdata = in_w((INT)tbl->regaddr0);
			out_w((INT)tbl->regaddr0, (UW)((regdata & msk) | level));
			regdata = in_w((INT)tbl->regaddr1);
			out_w((INT)tbl->regaddr1, (UW)((regdata & msk) | level));
			_EI(imask);
		}
	}
}

/* Disable interrupt
 *	Disable the external interrupts specified by 'intvec' by setting
 *	its priority level to '0'.
 *	Disable the peripheral module interrupts specified by 'intvec' by
 *	setting its priority level to '0' or '1'.
 */
EXPORT void DisableInt( INTVEC intvec )
{
	return EnableInt(intvec, 0);
}
