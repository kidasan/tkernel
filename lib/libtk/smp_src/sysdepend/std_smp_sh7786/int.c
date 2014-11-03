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
 *	@(#)int.c (libtk/SH7786)
 *
 *	Interrupt controller  
 */

#include <basic.h>
#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <sys/sysinfo.h>
#include <tk/smp_util.h>

typedef struct {
	UW	regaddr;			/* Interrupt priority register address */
	UINT	shift:24;
	UINT	mask:8;
} INTPRITBL;

LOCAL INTPRITBL intpritable[N_INTVEC] = {
	{ NULL, 0, 0x00 },			/* 0x000 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x020 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x040 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x060 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x080 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x0A0 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x0C0 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x0E0 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x100 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x120 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x140 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x160 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x180 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x1A0 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x1C0 : Reserved (CPU exception) */
	{ NULL, 0, 0x00 },			/* 0x1E0 : Reserved (CPU exception) */
	{ INTPRI, 28, 0x0f },			/* 0x200 : IRQ[0] */
	{ NULL,   0,  0x00 },			/* 0x220 : Reserved (IRL[3:0] 14) */
	{ INTPRI, 24, 0x0f },			/* 0x240 : IRQ[1] */
	{ NULL,   0,  0x00 },			/* 0x260 : Reserved (IRL[3:0] 12) */
	{ INTPRI, 20, 0x0f },			/* 0x280 : IRQ[2] */
	{ NULL,   0,  0x00 },			/* 0x2A0 : Reserved (IRL[3:0] 10) */
	{ INTPRI, 16, 0x0f },			/* 0x2C0 : IRQ[3] */
	{ NULL,   0,  0x00 },			/* 0x2E0 : Reserved (IRL[3:0] 8) */
	{ INTPRI, 12, 0x0f },			/* 0x300 : IRQ[4] */
	{ NULL,   0,  0x00 },			/* 0x320 : Reserved (IRL[3:0] 6) */
	{ INTPRI, 8,  0x0f },			/* 0x340 : IRQ[5] */
	{ NULL,   0,  0x00 },			/* 0x360 : Reserved (IRL[3:0] 4) */
	{ INTPRI, 4,  0x0f },			/* 0x380 : IRQ[6] */
	{ NULL,   0,  0x00 },			/* 0x3A0 : Reserved (IRL[3:0] 2) */
	{ INTPRI, 0,  0x0f },			/* 0x3C0 : IRQ[7] */
	{ INT2PRI0, 0,  0x1f },			/* 0x3E0 : WDT */
	{ INT2PRI1, 24, 0x1f },			/* 0x400 : TMU-ch0 */
	{ INT2PRI1, 16, 0x1f },			/* 0x420 : TMU-ch1 */
	{ INT2PRI1, 8,  0x1f },			/* 0x440 : TMU-ch2 */
	{ INT2PRI1, 0,  0x1f },			/* 0x460 : TMU-ch2 input capture */
	{ INT2PRI2, 24, 0x1f },			/* 0x480 : TMU-ch3 */
	{ INT2PRI2, 16, 0x1f },			/* 0x4A0 : TMU-ch4 */
	{ INT2PRI2, 8,  0x1f },			/* 0x4C0 : TMU-ch5 */
	{ NULL,     0,  0x00 },			/* 0x4E0 : Reserved (Unallocation) */
	{ INT2PRI3, 24, 0x1f },			/* 0x500 : DMAC-ch0 */
	{ INT2PRI3, 16, 0x1f },			/* 0x520 : DMAC-ch1 */
	{ INT2PRI3, 8,  0x1f },			/* 0x540 : DMAC-ch2 */
	{ INT2PRI3, 0,  0x1f },			/* 0x560 : DMAC-ch3 */
	{ INT2PRI4, 24, 0x1f },			/* 0x580 : DMAC-ch4 */
	{ INT2PRI4, 16, 0x1f },			/* 0x5A0 : DMAC-ch5 */
	{ INT2PRI4, 8,  0x1f },			/* 0x5C0 : DMAC address error */
	{ INT2PRI4, 0,  0x1f },			/* 0x5E0 : H-UDI 1 */
	{ INT2PRI5, 24, 0x1f },			/* 0x600 : H-UDI 0 */
	{ INT2PRI5, 16, 0x1f },			/* 0x620 : SHwyDMAC-ch0 */
	{ INT2PRI5, 8,  0x1f },			/* 0x640 : SHwyDMAC-ch1 */
	{ INT2PRI5, 0,  0x1f },			/* 0x660 : SHwyDMAC-ch2 */
	{ INT2PRI6, 24, 0x1f },			/* 0x680 : SHwyDMAC-ch3 */
	{ INT2PRI6, 16, 0x1f },			/* 0x6A0 : HPB-ch6-0 */
	{ INT2PRI6, 8,  0x1f },			/* 0x6C0 : HPB-ch11-7 */
	{ INT2PRI6, 0,  0x1f },			/* 0x6E0 : HPB-ch13-12 */
	{ INT2PRI7, 24, 0x1f },			/* 0x700 : SCIF-ch0 (ERI0) */
	{ INT2PRI7, 16, 0x1f },			/* 0x720 : SCIF-ch0 (RXI0) */
	{ INT2PRI7, 8,  0x1f },			/* 0x740 : SCIF-ch0 (BRI0) */
	{ INT2PRI7, 0,  0x1f },			/* 0x760 : SCIF-ch0 (TXI0) */
	{ INT2PRI8, 24, 0x1f },			/* 0x780 : SCIF-ch1 (ERI1, RXI1, BRI1, TXI1) */
	{ INT2PRI8, 16, 0x1f },			/* 0x7A0 : TMU-ch8-6 */
	{ INT2PRI8, 8,  0x1f },			/* 0x7C0 : TMU-ch11-9 */
	{ NULL,     0,  0x00 },			/* 0x7E0 : Reserved (Unallocation) */
	{ NULL,     0,  0x00 },			/* 0x800 : Reserved (General FPU disable exception) */
	{ NULL,     0,  0x00 },			/* 0x820 : Reserved (Slot FPU disable exception) */
	{ INT2PRI9, 8,  0x1f },			/* 0x840 : SCIF-ch2 (ERI2, RXI2, BRI2, TXI2) */
	{ INT2PRI9, 0,  0x1f },			/* 0x860 : SCIF-ch3 (ERI3, RXI3, BRI3, TXI3) */
	{ INT2PRI10, 24, 0x1f },		/* 0x880 : SCIF-ch4 (ERI4, RXI4, BRI4, TXI4) */
	{ INT2PRI10, 16, 0x1f },		/* 0x8A0 : SCIF-ch5 (ERI5, RXI5, BRI5, TXI5) */
	{ INT2PRI10, 8,  0x1f },		/* 0x8C0 : Ether */
	{ INT2PRI10, 0,  0x1f },		/* 0x8E0 : Ether */
	{ NULL,      0,  0x00 },		/* 0x900 : Reserved (IRL[7:4] 15) */
	{ NULL,      0,  0x00 },		/* 0x920 : Reserved (IRL[7:4] 14) */
	{ NULL,      0,  0x00 },		/* 0x940 : Reserved (IRL[7:4] 13) */
	{ NULL,      0,  0x00 },		/* 0x960 : Reserved (IRL[7:4] 12) */
	{ NULL,      0,  0x00 },		/* 0x980 : Reserved (IRL[7:4] 11) */
	{ NULL,      0,  0x00 },		/* 0x9A0 : Reserved (IRL[7:4] 10) */
	{ NULL,      0,  0x00 },		/* 0x9C0 : Reserved (IRL[7:4] 9) */
	{ NULL,      0,  0x00 },		/* 0x9E0 : Reserved (IRL[7:4] 8) */
	{ NULL,      0,  0x00 },		/* 0xA00 : Reserved (IRL[7:4] 7) */
	{ NULL,      0,  0x00 },		/* 0xA20 : Reserved (IRL[7:4] 6) */
	{ NULL,      0,  0x00 },		/* 0xA40 : Reserved (IRL[7:4] 5) */
	{ NULL,      0,  0x00 },		/* 0xA60 : Reserved (IRL[7:4] 4) */
	{ NULL,      0,  0x00 },		/* 0xA80 : Reserved (IRL[7:4] 3) */
	{ NULL,      0,  0x00 },		/* 0xAA0 : Reserved (IRL[7:4] 2) */
	{ NULL,      0,  0x00 },		/* 0xAC0 : Reserved (IRL[7:4] 1) */
	{ INT2PRI14, 0,  0x1f },		/* 0xAE0 : PCIeC0(1) */
	{ INT2PRI15, 24, 0x1f },		/* 0xB00 : PCIeC0(2) */
	{ INT2PRI15, 16, 0x1f },		/* 0xB20 : PCIeC0(3) */
	{ INT2PRI15, 8,  0x1f },		/* 0xB40 : PCIeC1(1) */
	{ INT2PRI15, 0,  0x1f },		/* 0xB60 : PCIeC1(2) */
	{ INT2PRI16, 24, 0x1f },		/* 0xB80 : PCIeC1(3) */
	{ INT2PRI16, 16, 0x1f },		/* 0xBA0 : USB */
	{ NULL,      0,  0x00 },		/* 0xBC0 : Reserved (Unallocation) */
	{ NULL,      0,  0x00 },		/* 0xBE0 : Reserved (Unallocation) */
	{ INT2PRI17, 24, 0x1f },		/* 0xC00 : DTU-ch0 (TEND) */
	{ INT2PRI17, 16, 0x1f },		/* 0xC20 : DTU-ch0 (AE) */
	{ INT2PRI17, 8,  0x1f },		/* 0xC40 : DTU-ch0 (TMISS) */
	{ INT2PRI17, 0,  0x1f },		/* 0xC60 : DTU-ch1 (TEND) */
	{ INT2PRI18, 24, 0x1f },		/* 0xC80 : DTU-ch1 (AE) */
	{ INT2PRI18, 16, 0x1f },		/* 0xCA0 : DTU-ch1 (TMISS) */
	{ INT2PRI18, 8,  0x1f },		/* 0xCC0 : I2C-ch0 */
	{ INT2PRI18, 0,  0x1f },		/* 0xCE0 : I2C-ch1 */
	{ INT2PRI19, 24, 0x1f },		/* 0xD00 : DU */
	{ INT2PRI19, 16, 0x1f },		/* 0xD20 : SSI-ch0 */
	{ INT2PRI19, 8,  0x1f },		/* 0xD40 : SSI-ch1 */
	{ INT2PRI19, 0,  0x1f },		/* 0xD60 : SSI-ch2 */
	{ INT2PRI20, 24, 0x1f },		/* 0xD80 : SSI-ch3 */
	{ INT2PRI20, 16, 0x1f },		/* 0xDA0 : PCIeC2(1) */
	{ INT2PRI20, 8,  0x1f },		/* 0xDC0 : PCIeC2(2) */
	{ INT2PRI20, 0,  0x1f },		/* 0xDE0 : PCIeC2(3) */
	{ INT2PRI21, 24, 0x1f },		/* 0xE00 : HAC-ch0 */
	{ INT2PRI21, 16, 0x1f },		/* 0xE20 : HAC-ch1 */
	{ INT2PRI21, 8,  0x1f },		/* 0xE40 : FLCTL */
	{ NULL,      0,  0x00 },		/* 0xE60 : Reserved (Unallocation) */
	{ INT2PRI22, 24, 0x1f },		/* 0xE80 : HSPI */
	{ INT2PRI22, 16, 0x1f },		/* 0xEA0 : GPIO0 */
	{ INT2PRI22, 8,  0x1f },		/* 0xEC0 : GPIO0 */
	{ INT2PRI22, 0,  0x1f },		/* 0xEE0 : Thermal Sensor */
	{ NULL,      0,  0x00 },		/* 0xF00 : Reserved (INTICI0) */
	{ NULL,      0,  0x00 },		/* 0xF20 : Reserved (INTICI1) */
	{ NULL,      0,  0x00 },		/* 0xF40 : Reserved (INTICI2) */
	{ NULL,      0,  0x00 },		/* 0xF60 : Reserved (INTICI3) */
	{ NULL,      0,  0x00 },		/* 0xF80 : Reserved (INTICI4) */
	{ NULL,      0,  0x00 },		/* 0xFA0 : Reserved (INTICI5) */
	{ NULL,      0,  0x00 },		/* 0xFC0 : Reserved (INTICI6) */
	{ NULL,      0,  0x00 },		/* 0xFE0 : Reserved (INTICI7) */
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
	UW	before, after;

	if( vecno < N_INTVEC ) {
		tbl = &intpritable[vecno];
		if ( tbl->regaddr != NULL ) {
			msk = ~((UINT)tbl->mask << tbl->shift);
			level = (level & tbl->mask) << tbl->shift;

		_DI(imask);
			while(1){
				before = in_w(tbl->regaddr);
				after = atomic_cmpxchg((UW*)tbl->regaddr, (UW)((before & msk) | level), before);
				if(before == after) break;
			}
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
