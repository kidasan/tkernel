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
 *	navienginedef.h	Boot
 *
 *	System-dependent definition (for NAVIENGINE)
 *
 *	Included also from assembler program.
 */

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
 * System control coprocessor (CP15): Control register (CR1)
 */
#define CR1_M		0x0001		/* MMU enable */
#define CR1_A		0x0002		/* Alignment check enable */
#define CR1_C		0x0004		/* Cache enable */

#define CR1_S		0x0100		/* System protection */
#define CR1_R		0x0200		/* ROM protection */

#define CR1_Z		0x0800		/* Program Flow Enable */
#define CR1_I		0x1000		/* Instruction cache enable */
#define CR1_V		0x2000		/* High vector */
#define CR1_RR		0x4000		/* Round robin cache */
#define CR1_L4		0x8000		/* ARMv4 compatible mode */

#define CR1_U		0x00400000	/* Unaligned Data Access */
#define CR1_XP		0x00800000	/* Extended Page Table Configuration */

#define CR1_EE		0x02000000	/* CPSR E bit Setting */

#define CR1_NM		0x08000000	/* NMFI */
#define CR1_TR		0x10000000	/* TEX Remap */
#define CR1_FA		0x20000000	/* Force AP */

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
 * Interrupt controller : Distributed Interrupt Controller (DIC)
 */
#define SCU_BASE	(0xC0000000)		/* = PERIPHBASE */

#define SCU_CTRL	(SCU_BASE + 0x0000)
#define SCU_CONFIG	(SCU_BASE + 0x0004)
#define SCU_CPUSTAT	(SCU_BASE + 0x0008)
#define SCU_INVALL	(SCU_BASE + 0x000c)

#define DIC_CPUCTRL	(SCU_BASE + 0x0100)
#define DIC_PRIMSK	(SCU_BASE + 0x0104)
#define DIC_BINPNT	(SCU_BASE + 0x0108)
#define DIC_INTACK	(SCU_BASE + 0x010C)
#define DIC_INTEOI	(SCU_BASE + 0x0110)
#define DIC_PRIRUN	(SCU_BASE + 0x0114)
#define DIC_HIPEND	(SCU_BASE + 0x0118)

#define DIC_CTRL	(SCU_BASE + 0x1000)
#define DIC_TYPE	(SCU_BASE + 0x1004)
#define DIC_ENBSET(n)	(SCU_BASE + 0x1100 + _OFS1(n))	/* n = 0 .. 255 */
#define DIC_ENBCLR(n)	(SCU_BASE + 0x1180 + _OFS1(n))	/* n = 0 .. 255 */
#define DIC_PENDSET(n)	(SCU_BASE + 0x1200 + _OFS1(n))	/* n = 0 .. 255 */
#define DIC_PENDCLR(n)	(SCU_BASE + 0x1280 + _OFS1(n))	/* n = 0 .. 255 */
#define DIC_ACTIVE(n)	(SCU_BASE + 0x1300 + _OFS1(n))	/* n = 0 .. 255 */
#define DIC_PRI(n)	(SCU_BASE + 0x1400 + _OFS8(n))	/* n = 0 .. 255 */
#define DIC_CPUTRG(n)	(SCU_BASE + 0x1800 + _OFS8(n))	/* n = 0 .. 255 */
#define DIC_CONFIG(n)	(SCU_BASE + 0x1C00 + _OFS2(n))	/* n = 0 .. 255 */
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
#define GPIO_IM0REG		(GPIO_BASE + 0x24)
#define GPIO_IM1REG		(GPIO_BASE + 0x28)
#define GPIO_IM2REG		(GPIO_BASE + 0x2C)
#define GPIO_IM3REG		(GPIO_BASE + 0x30)
#define GPIO_GPIPOLINVREG	(GPIO_BASE + 0x38)
#define GPIO_GPIPOLRSTREG	(GPIO_BASE + 0x3C)
#define GPIO_GPOPOLINVREG	(GPIO_BASE + 0x40)
#define GPIO_GPOPOLRSTREG	(GPIO_BASE + 0x44)

/*
 * Private Timer
 */
#define PTIMER_BASE		(0xC0000600)
#define PTIMER_LOAD		(PTIMER_BASE + 0x00)
#define PTIMER_COUNTER		(PTIMER_BASE + 0x04)
#define PTIMER_CONTROL		(PTIMER_BASE + 0x08)
#define PTIMER_STATUS		(PTIMER_BASE + 0x0C)

#define PTCR_ENABLE		(0x01)
#define PTCR_RELOAD		(0x02)
#define PTCR_INTEN		(0x04)
#define PTCR_PRESCALER		(8)

/*
 * System Control Unit
 */
#define SYSCTRL_BASE		(0x18037C00)
#define SYSCTRL_MEMO		(SYSCTRL_BASE + 0x0000)
#define SYSCTRL_RESET_STATUS	(SYSCTRL_BASE + 0x000C)

/*
 * UART
 */
#define UART_BASE(n)	(0x18034000 + 0x400*(n))
#define RBR_REG(n)	(UART_BASE(n) + 0x00)
#define THR_REG(n)	(UART_BASE(n) + 0x00)
#define DLL_REG(n)	(UART_BASE(n) + 0x00)
#define IER_REG(n)	(UART_BASE(n) + 0x04)
#define DLH_REG(n)	(UART_BASE(n) + 0x04)
#define IIR_REG(n)	(UART_BASE(n) + 0x08)
#define FCR_REG(n)	(UART_BASE(n) + 0x08)
#define LCR_REG(n)	(UART_BASE(n) + 0x0c)
#define MCR_REG(n)	(UART_BASE(n) + 0x10)
#define LSR_REG(n)	(UART_BASE(n) + 0x14)
#define MSR_REG(n)	(UART_BASE(n) + 0x18)
#define SCR_REG(n)	(UART_BASE(n) + 0x1c)
#define FDR_REG(n)	(UART_BASE(n) + 0x20)

#define LSR5		(1 << 5)
#define LSR0		(1 << 0)

