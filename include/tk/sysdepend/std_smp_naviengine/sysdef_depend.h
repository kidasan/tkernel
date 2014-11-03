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
 *	@(#)sysdef_depend.h (tk/NAVIENGINE)
 *
 *	Definition about NAVIENGINE
 *
 *	Included also from assembler program.
 */

#ifndef __TK_SYSDEF_DEPEND_H__
#define __TK_SYSDEF_DEPEND_H__

/*
 * Program status register (PSR)
 */
#define PSR_N		0x80000000	/* Condition flag Negative */
#define PSR_Z		0x40000000	/* Zero */
#define PSR_C		0x20000000	/* Carry */
#define PSR_V		0x10000000	/* Overflow */

#define PSR_Q		0x08000000	/* Sticky overflow */
#define PSR_J		0x01000000	/* Jazelle mode */

#define PSR_GE		0x000F0000	/* GE bits for SIMD Instruction */
#define PSR_E		0x00000200	/* Data Endian */
#define PSR_A		0x00000100	/* Unaccuracy Abort */

#define PSR_I		0x00000080	/* Interrupt (IRQ) disable */
#define PSR_F		0x00000040	/* Fast Interrupt (FIQ) disable */
#define PSR_T		0x00000020	/* Thumb mode */

#define PSR_DI		( PSR_I )	/* All Interrupts disable */

#define PSR_M(n)	( n )		/* Processor mode 0-31 */
#define PSR_USR		PSR_M(16)	/* User mode */
#define PSR_FIQ		PSR_M(17)	/* Fast Interrupt (FIQ) mode */
#define PSR_IRQ		PSR_M(18)	/* Interrupt (IRQ) mode */
#define PSR_SVC		PSR_M(19)	/* Supervisor mode */
#define PSR_ABT		PSR_M(23)	/* Abort mode */
#define PSR_UND		PSR_M(27)	/* Undefined order mode */
#define PSR_SYS		PSR_M(31)	/* System mode */

/*
 * Task mode flag
 *	System shared information 'taskmode' 
 */
#define TMF_CPL(n)	( (n) )		/* Current protection level(0-3) */
#define TMF_PPL(n)	( (n) << 16 )	/* Previous protection level(0-3) */

/*
 * System control coprocessor (CP15): Control register (CR1)
 */
#define CR1_M		0x00000001	/* MMU enable */
#define CR1_A		0x00000002	/* Alignment check enable */
#define CR1_C		0x00000004	/* Cache enable */
#define CR1_W		0x00000008	/* Write buffer enable */
#define CR1_S		0x00000100	/* System protection */
#define CR1_R		0x00000200	/* ROM protection */
#define CR1_Z		0x00000800	/* Program Flow Enable */
#define CR1_I		0x00001000	/* Instruction cache enable */
#define CR1_V		0x00002000	/* High vector */
#define CR1_L4		0x00008000	/* ARMv4 compatible mode */
#define CR1_U		0x00400000	/* Unaligned Data Access */
#define CR1_XP		0x00800000	/* Extended Page Table Configuration */
#define CR1_EE		0x02000000	/* CPSR E bit Setting */
#define CR1_NM		0x08000000	/* NMFI bit */
#define CR1_TR		0x10000000	/* TEX Remapping */
#define CR1_FA		0x20000000	/* Force AP bit */

/*
 * Fault status (CP15:CR5)
 */
#define FSR_SD		0x1000	/* External Abort Qualifier */
#define FSR_RW		0x800	/* Not Read/Write (DFSR Only) */
#define FSR_DomainNum	0x0F0	/* Domain Number */

#define FSR_X           0x400   /* Extented Status Flag (Bit 10) */
#define FSR_ExTypeMask  0x40f   /* Extented Status Type Mask */
#define FSR_TypeMask	0x00d	/* Type mask */

#define FSR_Alignment	0x001	/* 00001 : Miss-alignment access */
#define FSR_CacheFault  0x004   /* 00100 : Cache Mentenance Fault */
#define FSR_ExtAbortT1	0x00c	/* 01100 : Ext Abort when Converting address */
#define FSR_ExtAbortT2	0x00e	/* 01110 : Ext Abort when converting address */
#define FSR_Translation	0x005	/* 001x1 : No page when converting address */
#define FSR_AccessBitS	0x003	/* 00011 : Access Bit Section */
#define FSR_AccessBitP	0x006	/* 00110 : Access Bit Page */
#define FSR_Domain	0x009	/* 010x1 : Domain access violation */
#define FSR_Permission	0x00d	/* 011x1 : Access violation */
#define FSR_ExtAbort	0x008	/* 01000 : Precise Ext Abort */
#define FSR_ExtAbortIP  0x406   /* 10110 : Imprecise Ext Abort */
#define FSR_BusErrorC	0x002	/* 00010 : Instruction Debug Event */

#define FSR_Section	0x000	/* xx0x : Section */
#define FSR_Page	0x002	/* xx1x : Page */


/* ------------------------------------------------------------------------ */

/* N-Bits field Number to Word Offset */
#define _OFS1(n)	(((n) / 32) * 4)
#define _OFS2(n)	(((n) / 16) * 4)
#define _OFS4(n)	(((n) / 8) * 4)
#define _OFS8(n)	(((n) / 4) * 4)
#define _OFS32(n)	((n) * 4)

/* N-Bits field Mask */
#define _BIT1(v, n)	((UW)(v) << ((n) % 32))
#define _BIT2(v, n)	((UW)(v) << (((n) % 16) * 2))
#define _BIT4(v, n)	((UW)(v) << (((n) % 8) * 4))
#define _BIT8(v, n)	((UW)(v) << (((n) % 4) * 8))

/*
 * Interrupt controller (Distributed Interrupt Controller (DIC))
 */
#define SCU_BASE	(0xC0000000)		// = PERIPHBASE

#define DIC_CPUCTRL	(SCU_BASE + 0x0100)
#define DIC_PRIMSK	(SCU_BASE + 0x0104)
#define DIC_BINPNT	(SCU_BASE + 0x0108)
#define DIC_INTACK	(SCU_BASE + 0x010C)
#define DIC_INTEOI	(SCU_BASE + 0x0110)
#define DIC_PRIRUN	(SCU_BASE + 0x0114)
#define DIC_HIPEND	(SCU_BASE + 0x0118)

#define DIC_CTRL	(SCU_BASE + 0x1000)
#define DIC_TYPE	(SCU_BASE + 0x1004)
#define DIC_ENBSET(n)	(SCU_BASE + 0x1100 + _OFS1(n))	// n = 0 .. 255
#define DIC_ENBCLR(n)	(SCU_BASE + 0x1180 + _OFS1(n))	// n = 0 .. 255
#define DIC_PENDSET(n)	(SCU_BASE + 0x1200 + _OFS1(n))	// n = 0 .. 255
#define DIC_PENDCLR(n)	(SCU_BASE + 0x1280 + _OFS1(n))	// n = 0 .. 255
#define DIC_ACTIVE(n)	(SCU_BASE + 0x1300 + _OFS1(n))	// n = 0 .. 255
#define DIC_PRI(n)	(SCU_BASE + 0x1400 + _OFS8(n))	// n = 0 .. 255
#define DIC_CPUTRG(n)	(SCU_BASE + 0x1800 + _OFS8(n))	// n = 0 .. 255
#define DIC_CONFIG(n)	(SCU_BASE + 0x1C00 + _OFS2(n))	// n = 0 .. 255
#define DIC_SWI(n)	(SCU_BASE + 0x1F00)

/*
 * GPIO
 */
#define GPIO_BASE		(0x18038000)
#define GPIO_PCEREG		(GPIO_BASE + 0x00)
#define GPIO_PCDREG		(GPIO_BASE + 0x04)
#define GPIO_SCHREG		(GPIO_BASE + 0x08)
#define GPIO_SCLREG		(GPIO_BASE + 0x0C)
#define GPIO_PVREG		(GPIO_BASE + 0x10)
#define GPIO_INTREG		(GPIO_BASE + 0x14)
#define GPIO_INEREG		(GPIO_BASE + 0x18)
#define GPIO_INDREG		(GPIO_BASE + 0x1C)
#define GPIO_INHREG		(GPIO_BASE + 0x20)
#define GPIO_IMREG(n)		(GPIO_BASE + 0x24 + _OFS4(n))
#define GPIO_GPIPOLINVREG	(GPIO_BASE + 0x38)
#define GPIO_GPIPOLRSTREG	(GPIO_BASE + 0x3C)
#define GPIO_GPOPOLINVREG	(GPIO_BASE + 0x40)
#define GPIO_GPOPOLRSTREG	(GPIO_BASE + 0x44)


/* ------------------------------------------------------------------------ */

/*
 * Software interrupt number for T-Monitor
 */
#define SWI_MONITOR	4	/* T-Monitor service call */

/*
 * Software interrupt number for T-Kernel 
 */
#define SWI_SVC		6	/* T-Kernel system call/extension SVC */
#define SWI_RETINT	7	/* tk_ret_int() system call */
#define SWI_DISPATCH	8	/* Task dispatcher */
#define SWI_DEBUG	9	/* Debugger support function */

/*
 * Software interrupt number for Extension
 */
#define SWI_KILLPROC	11	/* Force process termination request */

/* ------------------------------------------------------------------------ */

/*
 * Location and size of task specific space
 *	Number of entries and index number of first level page table
 */
#define TOP_PDIR_ENTRIES	0x300	/* 0x30000000 */
#define NUM_PDIR_ENTRIES	0x100	/* 0x30000000 - 0x40000000 */

#endif /* __TK_SYSDEF_DEPEND_H__ */
