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
 *	@(#)sysdef_depend.h (tk/SH7776)
 *
 *	Definition about SH7776
 *
 *	Included also from assembler program.
 */

#ifndef __TK_SYSDEF_DEPEND_H__
#define __TK_SYSDEF_DEPEND_H__

/*
 * Use specify register
 */
#define SP	r15	/* Stack pointer */

/*
 * BANK1 register
 *	R0	For work				With task
 *	R1	Reservation				With task
 *	R2 MDR	Operation mode register		  	With task
 *	R3 SST	System stack top		  	With task
 *	R4	For work				Task independent
 *	R5	Reservation				Task independent
 *	R6 ISP	EIT stack pointer	  		Task independent
 *	R7	For EIT work				Task independent
 *		(Available only at SR.BL=1)
 */
#define MDR	r2	/* Operation mode register */
#define SST	r3	/* System stack top */
#define ISP	r6	/* EIT stack pointer */

/*
 * MDR register
 */
#define MDR_DCT		0x80000000	/* Delayed context trap request */
#define MDR_PPL(n)	( (n) << 16 )	/* Previous protection level (0-3) */
#define MDR_CPL(n)	( (n) )		/* Current protection level (0-3) */

/*
 * SR register
 */
#define SR_MD		0x40000000	/* Privilege mode */
#define SR_RB		0x20000000	/* Register bank */
#define SR_BL		0x10000000	/* Interrupt block */
#define SR_FD		0x00008000	/* FPU disable (SH4) */
#define NOT_SR_FD	~0x00008000	/* FPU enable (SH4) */
#define SR_I(n)		( (n) << 4 )	/* Interrupt mask register (0-15) */

/*
 * FPU status/control register (SH4)
 */
#define FPSCR_FR	0x00200000	/* Register bank */
#define FPSCR_SZ	0x00100000	/* Transfer size mode */
#define FPSCR_PR	0x00080000	/* Precision mode */
#define FPSCR_DN	0x00040000	/* Denormalization mode */
#define FPSCR_C(n)	((n) << 12)	/* Exception cause  (6bit) */
#define FPSCR_E(n)	((n) << 7)	/* Exception enable (5bit) */
#define FPSCR_F(n)	((n) << 2)	/* Exception flag   (5bit) */
#define FPSCR_RM(n)	(n)		/* Rounding mode    (2bit) */

					/* FPU exception */
#define FPE_I		0x01		/*   Inexact */
#define FPE_U		0x02		/*   Underflow */
#define FPE_O		0x04		/*   Overflow */
#define FPE_Z		0x08		/*   Division by zero */
#define FPE_V		0x10		/*   Invalid */
#define FPE_E		0x20		/*   FPU error */

/*
 * EIT register
 */
#define TRA		0xFF000020	/* W:TRAPA exception */
#define EXPEVT		0xFF000024	/* W:Exception event */
#define INTEVT		0xFF000028	/* W:Interrupt event */

/*
 * MMU register
 */
#define PTEH		0xFF000000	/* W:Page table entry high */
#define PTEL		0xFF000004	/* W:Page table entry low */
#define TTB		0xFF000008	/* W:Page table base */
#define TEA		0xFF00000c	/* W:TLB exception address */
#define MMUCR		0xFF000010	/* W:MMU control */
#define PTEA		0xFF000034	/* W:Page table entry assistance */
#define PASCR		0xFF000070	/* W:Physical address space control register */

#define MMU_AT		0x00000001	/* MMU enable */
#define MMU_TI		0x00000004	/* TLB Invalidate */
#define MMU_SV		0x00000100	/* Single virtual memory mode */
#define MMU_SQMD	0x00000200	/* Store queue mode */
#define PASCR_SE	0x80000000	/* Address mode */

/*
 * Cache register
 */
#define CCR		0xFF00001c	/* W:Cache control */
#define RAMCR		0xff000074	/* W:RAM control */
#define L2CR		0xfbf00000	/* W:L2 Cache control */

#define CCR_OCE		0x00000001	/* OC enable */
#define CCR_WT		0x00000002	/* Write through enable (U0 P0 P3) */
#define CCR_CB		0x00000004	/* Copy back enable (P1) */
#define CCR_OCI		0x00000008	/* OC invalidation */
#define CCR_L2OE	0x00000010	/* L2 OC enable */
#define CCR_ICE		0x00000100	/* IC enable */
#define CCR_ICI		0x00000800	/* IC invalidation */
#define CCR_L2IE	0x00001000	/* L2 cache enable */
#define CCR_CCD		0x00010000	/* Cache Coherency Control */
#define CCR_MCP		0x00020000	/* Mixed Coherency Protocols */
#define CCR_SNM		0x00040000	/* Measures for Synonym Problems */
#define CCR_IBE		0x01000000	/* Broadcast as for the ICBI instruction */

#define L2CR_L2CI	0x00000008	/* L2 cache invalidation */

/*
 * Interrupt controller
 */
#define ICR0		0xFF802000	/* W:Interrupt control 0 */
#define ICR1		0xFF802060	/* W:Interrupt control 1 */
#define INTREQ		0xFF802068	/* W:Interrupt request */
#define INTICI0		0xFF802070	/* W:Interrupt among CPU0 */
#define INTICI1		0xFF802074	/* W:Interrupt among CPU1 */

#define C0INTMSK0	0xFF802100	/* W:Interrupt mask 0(CPU0) */
#define C0INTMSK1	0xFF802104	/* W:Interrupt mask 1(CPU0) */
#define C0INTMSKCLR0	0xFF802108	/* W:Interrupt mask clear 0(CPU0) */
#define C0INTMSKCLR1	0xFF80210C	/* W:Interrupt mask clear 1(CPU0) */
#define C0INTPRI	0xFF802110	/* W:Interrupt priority(CPU0) */
#define C0INTICICLR	0xFF802118	/* W:Clearing Interrupt among(CPU0) */
#define C0ICIPRI	0xFF80211C	/* W:Setting Interrupt Priority Order among(CPU0) */
#define C0ICIPRICLR	0xFF802120	/* W:Clearing Interrupt Priority among(CPU0) */
#define C0INTMSK2	0xFF802124	/* W:Interrupt mask 2(CPU0) */
#define C0INTMSKCLR2	0xFF802128	/* W:Interrupt mask clear 2(CPU0) */

#define C1INTMSK0	0xFF802180	/* W:Interrupt mask 0(CPU1) */
#define C1INTMSK1	0xFF802184	/* W:Interrupt mask 1(CPU1) */
#define C1INTMSKCLR0	0xFF802188	/* W:Interrupt mask clear 0(CPU1) */
#define C1INTMSKCLR1	0xFF80218C	/* W:Interrupt mask clear 1(CPU1) */
#define C1INTPRI	0xFF802190	/* W:Interrupt priority(CPU1) */
#define C1INTICICLR	0xFF802198	/* W:Clearing Interrupt among(CPU1) */
#define C1ICIPRI	0xFF80219C	/* W:Setting Interrupt Priority Order among(CPU1) */
#define C1ICIPRICLR	0xFF8021A0	/* W:Clearing Interrupt Priority among(CPU1) */
#define C1INTMSK2	0xFF8021A4	/* W:Interrupt mask 2(CPU1) */
#define C1INTMSKCLR2	0xFF8021A8	/* W:Interrupt mask clear 2(CPU1) */

#define C0INT2PRI0	0xFF802800	/* W:Interrupt priority 0(CPU0) */
#define C0INT2PRI1	0xFF802804	/* W:Interrupt priority 1(CPU0) */
#define C0INT2PRI2	0xFF802808	/* W:Interrupt priority 2(CPU0) */
#define C0INT2PRI3	0xFF80280C	/* W:Interrupt priority 3(CPU0) */
#define C0INT2PRI4	0xFF802810	/* W:Interrupt priority 4(CPU0) */
#define C0INT2PRI5	0xFF802814	/* W:Interrupt priority 5(CPU0) */
#define C0INT2PRI6	0xFF802818	/* W:Interrupt priority 6(CPU0) */
#define C0INT2PRI7	0xFF80281C	/* W:Interrupt priority 7(CPU0) */
#define C0INT2PRI8	0xFF802820	/* W:Interrupt priority 8(CPU0) */
#define C0INT2PRI9	0xFF802824	/* W:Interrupt priority 9(CPU0) */
#define C0INT2PRI10	0xFF802828	/* W:Interrupt priority 10(CPU0) */
#define C0INT2PRI11	0xFF80282C	/* W:Interrupt priority 11(CPU0) */
#define C0INT2PRI12	0xFF802830	/* W:Interrupt priority 12(CPU0) */
#define C0INT2PRI13	0xFF802834	/* W:Interrupt priority 13(CPU0) */
#define C0INT2PRI14	0xFF802838	/* W:Interrupt priority 14(CPU0) */
#define C0INT2PRI15	0xFF80283C	/* W:Interrupt priority 15(CPU0) */
#define C0INT2PRI16	0xFF802840	/* W:Interrupt priority 16(CPU0) */
#define C0INT2PRI17	0xFF802844	/* W:Interrupt priority 17(CPU0) */
#define C0INT2PRI18	0xFF802848	/* W:Interrupt priority 18(CPU0) */
#define C0INT2PRI19	0xFF80284C	/* W:Interrupt priority 19(CPU0) */
#define C0INT2PRI20	0xFF802850	/* W:Interrupt priority 20(CPU0) */
#define C0INT2PRI21	0xFF802854	/* W:Interrupt priority 21(CPU0) */
#define C0INT2PRI22	0xFF802858	/* W:Interrupt priority 22(CPU0) */
#define C0INT2PRI23	0xFF80285C	/* W:Interrupt priority 23(CPU0) */
#define C0INT2PRI24	0xFF802860	/* W:Interrupt priority 24(CPU0) */

#define C0INT2MSK0	0xFF8028A0	/* W:Peripheral interrupt mask 0(CPU0) */
#define C0INT2MSK1	0xFF8028A4	/* W:Peripheral interrupt mask 1(CPU0) */
#define C0INT2MSK2	0xFF8028A8	/* W:Peripheral interrupt mask 2(CPU0) */
#define C0INT2MSK3	0xFF8028AC	/* W:Peripheral interrupt mask 3(CPU0) */
#define C0INT2MSKCLR0	0xFF8028B0	/* W:Peripheral interrupt mask clear 0(CPU0) */
#define C0INT2MSKCLR1	0xFF8028B4	/* W:Peripheral interrupt mask clear 1(CPU0) */
#define C0INT2MSKCLR2	0xFF8028B8	/* W:Peripheral interrupt mask clear 2(CPU0) */
#define C0INT2MSKCLR3	0xFF8028BC	/* W:Peripheral interrupt mask clear 3(CPU0) */
#define C0INTSMSK0	0xFF8028C0	/* W:Peripheral interrupt sub-mask 0(CPU0) */
#define C0INTSMSK1	0xFF8028C4	/* W:Peripheral interrupt sub-mask 1(CPU0) */
#define C0INTSMSK2	0xFF8028C8	/* W:Peripheral interrupt sub-mask 2(CPU0) */
#define C0INTSMSK3	0xFF8028CC	/* W:Peripheral interrupt sub-mask 3(CPU0) */
#define C0INTSMSK4	0xFF8028D0	/* W:Peripheral interrupt sub-mask 4(CPU0) */
#define C0INTSMSK5	0xFF8028D4	/* W:Peripheral interrupt sub-mask 5(CPU0) */
#define C0INTSMSK6	0xFF8028D8	/* W:Peripheral interrupt sub-mask 6(CPU0) */
#define C0INTSMSK7	0xFF8028DC	/* W:Peripheral interrupt sub-mask 7(CPU0) */
#define C0INTSMSKCLR0	0xFF8028E0	/* W:Peripheral interrupt sub-mask clear 0(CPU0) */
#define C0INTSMSKCLR1	0xFF8028E4	/* W:Peripheral interrupt sub-mask clear 1(CPU0) */
#define C0INTSMSKCLR2	0xFF8028E8	/* W:Peripheral interrupt sub-mask clear 2(CPU0) */
#define C0INTSMSKCLR3	0xFF8028EC	/* W:Peripheral interrupt sub-mask clear 3(CPU0) */
#define C0INTSMSKCLR4	0xFF8028F0	/* W:Peripheral interrupt sub-mask clear 4(CPU0) */
#define C0INTSMSKCLR5	0xFF8028F4	/* W:Peripheral interrupt sub-mask clear 5(CPU0) */
#define C0INTSMSKCLR6	0xFF8028F8	/* W:Peripheral interrupt sub-mask clear 6(CPU0) */
#define C0INTSMSKCLR7	0xFF8028FC	/* W:Peripheral interrupt sub-mask clear 7(CPU0) */

#define C1INT2PRI0	0xFF802900	/* W:Interrupt priority 0(CPU1) */
#define C1INT2PRI1	0xFF802904	/* W:Interrupt priority 1(CPU1) */
#define C1INT2PRI2	0xFF802908	/* W:Interrupt priority 2(CPU1) */
#define C1INT2PRI3	0xFF80290C	/* W:Interrupt priority 3(CPU1) */
#define C1INT2PRI4	0xFF802910	/* W:Interrupt priority 4(CPU1) */
#define C1INT2PRI5	0xFF802914	/* W:Interrupt priority 5(CPU1) */
#define C1INT2PRI6	0xFF802918	/* W:Interrupt priority 6(CPU1) */
#define C1INT2PRI7	0xFF80291C	/* W:Interrupt priority 7(CPU1) */
#define C1INT2PRI8	0xFF802920	/* W:Interrupt priority 8(CPU1) */
#define C1INT2PRI9	0xFF802924	/* W:Interrupt priority 9(CPU1) */
#define C1INT2PRI10	0xFF802928	/* W:Interrupt priority 10(CPU1) */
#define C1INT2PRI11	0xFF80292C	/* W:Interrupt priority 11(CPU1) */
#define C1INT2PRI12	0xFF802930	/* W:Interrupt priority 12(CPU1) */
#define C1INT2PRI13	0xFF802934	/* W:Interrupt priority 13(CPU1) */
#define C1INT2PRI14	0xFF802938	/* W:Interrupt priority 14(CPU1) */
#define C1INT2PRI15	0xFF80293C	/* W:Interrupt priority 15(CPU1) */
#define C1INT2PRI16	0xFF802940	/* W:Interrupt priority 16(CPU1) */
#define C1INT2PRI17	0xFF802944	/* W:Interrupt priority 17(CPU1) */
#define C1INT2PRI18	0xFF802948	/* W:Interrupt priority 18(CPU1) */
#define C1INT2PRI19	0xFF80294C	/* W:Interrupt priority 19(CPU1) */
#define C1INT2PRI20	0xFF802950	/* W:Interrupt priority 20(CPU1) */
#define C1INT2PRI21	0xFF802954	/* W:Interrupt priority 21(CPU1) */
#define C1INT2PRI22	0xFF802958	/* W:Interrupt priority 22(CPU1) */
#define C1INT2PRI23	0xFF80295C	/* W:Interrupt priority 23(CPU1) */
#define C1INT2PRI24	0xFF802960	/* W:Interrupt priority 24(CPU1) */

#define C1INT2MSK0	0xFF8029A0	/* W:Peripheral interrupt mask 0(CPU1) */
#define C1INT2MSK1	0xFF8029A4	/* W:Peripheral interrupt mask 1(CPU1) */
#define C1INT2MSK2	0xFF8029A8	/* W:Peripheral interrupt mask 2(CPU1) */
#define C1INT2MSK3	0xFF8029AC	/* W:Peripheral interrupt mask 3(CPU1) */
#define C1INT2MSKCLR0	0xFF8029B0	/* W:Peripheral interrupt mask clear 0(CPU1) */
#define C1INT2MSKCLR1	0xFF8029B4	/* W:Peripheral interrupt mask clear 1(CPU1) */
#define C1INT2MSKCLR2	0xFF8029B8	/* W:Peripheral interrupt mask clear 2(CPU1) */
#define C1INT2MSKCLR3	0xFF8029BC	/* W:Peripheral interrupt mask clear 3(CPU1) */
#define C1INTSMSK0	0xFF8029C1	/* W:Peripheral interrupt sub-mask 0(CPU1) */
#define C1INTSMSK1	0xFF8029C4	/* W:Peripheral interrupt sub-mask 1(CPU1) */
#define C1INTSMSK2	0xFF8029C8	/* W:Peripheral interrupt sub-mask 2(CPU1) */
#define C1INTSMSK3	0xFF8029CC	/* W:Peripheral interrupt sub-mask 3(CPU1) */
#define C1INTSMSK4	0xFF8029D0	/* W:Peripheral interrupt sub-mask 4(CPU1) */
#define C1INTSMSK5	0xFF8029D4	/* W:Peripheral interrupt sub-mask 5(CPU1) */
#define C1INTSMSK6	0xFF8029D8	/* W:Peripheral interrupt sub-mask 6(CPU1) */
#define C1INTSMSK7	0xFF8029DC	/* W:Peripheral interrupt sub-mask 7(CPU1) */
#define C1INTSMSKCLR0	0xFF8029E0	/* W:Peripheral interrupt sub-mask clear 0(CPU1) */
#define C1INTSMSKCLR1	0xFF8029E4	/* W:Peripheral interrupt sub-mask clear 1(CPU1) */
#define C1INTSMSKCLR2	0xFF8029E8	/* W:Peripheral interrupt sub-mask clear 2(CPU1) */
#define C1INTSMSKCLR3	0xFF8029EC	/* W:Peripheral interrupt sub-mask clear 3(CPU1) */
#define C1INTSMSKCLR4	0xFF8029F0	/* W:Peripheral interrupt sub-mask clear 4(CPU1) */
#define C1INTSMSKCLR5	0xFF8029F4	/* W:Peripheral interrupt sub-mask clear 5(CPU1) */
#define C1INTSMSKCLR6	0xFF8029F8	/* W:Peripheral interrupt sub-mask clear 6(CPU1) */
#define C1INTSMSKCLR7	0xFF8029FC	/* W:Peripheral interrupt sub-mask clear 7(CPU1) */

#define INTLEVEL_CLR	0x1f

/* power down */
#define MSTPCR0		0xFFC80030	/* W:Standby Control Register 0 */
#define MSTPCR1		0xFFC80034	/* W:Standby Control Register 1 */
#define MSTPCR3		0xFFC8003C	/* W:Standby Control Register 3 */
#define C0STBCR		0xFE400004	/* W:CPU0 Standby Control Register */
#define C1STBCR		0xFE401004	/* W:CPU1 Standby Control Register */

#define	STBCR_MSTP	0x00000001	/* Module stop bit of CPU core */
#define MSTPCR0_MSTP26	0x04000000	/* Module stop bit of SCIF ch0 */
#define MSTPCR0_MSTP16	0x00010000	/* Module stop bit of TMU ch0 */

/*
 * Interrupt vector number for T-Monitor
 */
#define TRAP_MONITOR	0x90	/* Monitor service call */

/*
 * Interrupt vector number for T-Kernel
 */
#define TRAP_SVC	0x91	/* System call, Extension SVC */
#define TRAP_RETINT	0x92	/* tk_ret_int() system call */
#define TRAP_DISPATCH	0x93	/* Task dispatcher call */
#define TRAP_LOADSR	0x95	/* SR register load function */
#define TRAP_DEBUG	0x96	/* Debugger support function */

/*
 * Interrupt vector number for Extension
 */
#define TRAP_KILLPROC	0x97	/* Force process termination request */

/*
 * CPU core ID register
 */
#define CPIDR		0xFF000048

#endif /* __TK_SYSDEF_DEPEND_H__ */
