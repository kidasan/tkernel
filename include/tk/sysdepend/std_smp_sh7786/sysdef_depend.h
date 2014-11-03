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
 *	@(#)sysdef_depend.h (tk/SH7786)
 *
 *	Definition about SH7786
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
#define ICR0		0xFE410000	/* W:Interrupt control 0 */
#define ICR1		0xFE41001C	/* W:Interrupt control 1 */
#define INTPRI		0xFE410010	/* W:Interrupt priority */
#define INTREQ		0xFE410024	/* W:Interrupt request */
#define C0INTMSK0	0xFE410030	/* W:Interrupt mask 0(CPU0) */
#define C1INTMSK0	0xFE410034	/* W:Interrupt mask 0(CPU1) */
#define C0INTMSK1	0xFE410040	/* W:Interrupt mask 1(CPU0) */
#define C1INTMSK1	0xFE410044	/* W:Interrupt mask 1(CPU1) */
#define C0INTMSKCLR0	0xFE410050	/* W:Interrupt mask clear 0(CPU0) */
#define C1INTMSKCLR0	0xFE410054	/* W:Interrupt mask clear 0(CPU1) */
#define C0INTMSKCLR1	0xFE410060	/* W:Interrupt mask clear 1(CPU0) */
#define C1INTMSKCLR1	0xFE410064	/* W:Interrupt mask clear 1(CPU1) */
#define INTMSK2		0xFE410068	/* W:Interrupt mask 2 */
#define INTMSKCLR2	0xFE41006C	/* W:Interrupt mask clear 2 */
#define INT2PRI0	0xFE410800	/* W:Interrupt priority 0 */
#define INT2PRI1	0xFE410804	/* W:Interrupt priority 1 */
#define INT2PRI2	0xFE410808	/* W:Interrupt priority 2 */
#define INT2PRI3	0xFE41080C	/* W:Interrupt priority 3 */
#define INT2PRI4	0xFE410810	/* W:Interrupt priority 4 */
#define INT2PRI5	0xFE410814	/* W:Interrupt priority 5 */
#define INT2PRI6	0xFE410818	/* W:Interrupt priority 6 */
#define INT2PRI7	0xFE41081C	/* W:Interrupt priority 7 */
#define INT2PRI8	0xFE410820	/* W:Interrupt priority 8 */
#define INT2PRI9	0xFE410824	/* W:Interrupt priority 9 */
#define INT2PRI10	0xFE410828	/* W:Interrupt priority 10 */
#define INT2PRI11	0xFE41082C	/* W:Interrupt priority 11 */
#define INT2PRI12	0xFE410830	/* W:Interrupt priority 12 */
#define INT2PRI13	0xFE410834	/* W:Interrupt priority 13 */
#define INT2PRI14	0xFE410838	/* W:Interrupt priority 14 */
#define INT2PRI15	0xFE41083C	/* W:Interrupt priority 15 */
#define INT2PRI16	0xFE410840	/* W:Interrupt priority 16 */
#define INT2PRI17	0xFE410844	/* W:Interrupt priority 17 */
#define INT2PRI18	0xFE410848	/* W:Interrupt priority 18 */
#define INT2PRI19	0xFE41084C	/* W:Interrupt priority 19 */
#define INT2PRI20	0xFE410850	/* W:Interrupt priority 20 */
#define INT2PRI21	0xFE410854	/* W:Interrupt priority 21 */
#define INT2PRI22	0xFE410858	/* W:Interrupt priority 22 */
#define INT2PRI23	0xFE41085C	/* W:Interrupt priority 23 */
#define INT2PRI24	0xFE410860	/* W:Interrupt priority 24 */
#define C0INT2MSK0	0xFE410A20	/* W:Peripheral interrupt mask 0(CPU0) */
#define C0INT2MSK1	0xFE410A24	/* W:Peripheral interrupt mask 1(CPU0) */
#define C0INT2MSK2	0xFE410A28	/* W:Peripheral interrupt mask 2(CPU0) */
#define C0INT2MSK3	0xFE410A2C	/* W:Peripheral interrupt mask 3(CPU0) */
#define C0INT2MSKCLR0	0xFE410A30	/* W:Peripheral interrupt mask clear 0(CPU0) */
#define C0INT2MSKCLR1	0xFE410A34	/* W:Peripheral interrupt mask clear 1(CPU0) */
#define C0INT2MSKCLR2	0xFE410A38	/* W:Peripheral interrupt mask clear 2(CPU0) */
#define C0INT2MSKCLR3	0xFE410A3C	/* W:Peripheral interrupt mask clear 3(CPU0) */
#define C1INT2MSK0	0xFE410B20	/* W:Peripheral interrupt mask 0(CPU1) */
#define C1INT2MSK1	0xFE410B24	/* W:Peripheral interrupt mask 1(CPU1) */
#define C1INT2MSK2	0xFE410B28	/* W:Peripheral interrupt mask 2(CPU1) */
#define C1INT2MSK3	0xFE410B2C	/* W:Peripheral interrupt mask 3(CPU1) */
#define C1INT2MSKCLR0	0xFE410B30	/* W:Peripheral interrupt mask clear 0(CPU1) */
#define C1INT2MSKCLR1	0xFE410B34	/* W:Peripheral interrupt mask clear 1(CPU1) */
#define C1INT2MSKCLR2	0xFE410B38	/* W:Peripheral interrupt mask clear 2(CPU1) */
#define C1INT2MSKCLR3	0xFE410B3C	/* W:Peripheral interrupt mask clear 3(CPU1) */
#define C0INTICI	0xFE410070	/* W:Interrupt among CPU0 */
#define C1INTICI	0xFE410074	/* W:Interrupt among CPU1 */
#define C0INTICICLR	0xFE410080	/* W:Clearing Interrupt among CPU0 */
#define C1INTICICLR	0xFE410084	/* W:Clearing Interrupt among CPU1 */
#define C0ICIPRI	0xFE410090	/* W:Setting Interrupt Priority Order among CPU0 */
#define C1ICIPRI	0xFE410094	/* W:Setting Interrupt Priority Order among CPU1 */
#define C0ICIPRICLR	0xFE4100a0	/* W:Clearing Interrupt Priority among CPU0 */
#define C1ICIPRICLR	0xFE4100a4	/* W:Clearing Interrupt Priority among CPU1 */

#define INTLEVEL_CLR	0x1f

/* power down */
#define MSTPCR0		0xFFC40030	/* W:Standby Control Register 0 */
#define MSTPCR1		0xFFC40034	/* W:Standby Control Register 1 */
#define C0STBCR		0xFE400004	/* W:CPU Standby Control Register 0 */
#define C1STBCR		0xFE401004	/* W:CPU Standby Control Register 1 */

#define MSTPCR0_MSTP8	0x00000100	/* Module stop bit of TMU ch0-2 */

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
