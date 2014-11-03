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
 *	eitent_smp_naviengine.S	Boot
 *
 *	EIT entry processing (for NAVIENGINE)
 */
#include	"navienginedef.h"

	.globl		_start
	.globl		_resetHdr
	.globl		_irqGPIOHdr
	.globl		_defaultHdr
	.globl		_svcHdr

	.extern		procReset
	.extern		procSVC
	.extern		svclock

// ----------------------------------------------------------------------------
// Various definitions
// ----------------------------------------------------------------------------

// Stack
#define CORE0_STACK	0x80007000
#define CORE1_STACK	0x80008000
#define CORE2_STACK	0x80009000
#define CORE3_STACK	0x8000A000

// Work area
#define WORK_TOP	0x80004460
#define WORK_END	0x80006000

// ROM information
#define ROMINFO		0x00020000

//
#define base(n)		( (n) & 0xfffff000 )
#define offs(n)		( (n) & 0x00000fff )

#define PAGETBL_BASE	0x80000000

// Vector table
#define EIT_VECTBL	0x80004000	// Vector table top
#define EITVEC(n)	( EIT_VECTBL + (n) * 4 )
#define EIT_DEFAULT	0		// Default handler
#define EIT_UNDEF	1		// Undefined
#define EIT_IABORT	2		// Prefetch abort
#define EIT_DABORT	3		// Data Abort
#define EIT_FIQ		31		// FIQ
#define EIT_IRQ(n)	(32 + (n))	// INT : 0 - 127
#define EIT_GPIO(n)	(160 + (n))	// GPIO : 0 - 31
#define N_INTVEC	256		// Vector table size

/*----------------------------------------------------------------------
	Exception vector (0x00000000)
----------------------------------------------------------------------*/
	.org	0x00000000
_start:
_resetHdr:
	.arm
	b	reset_entry	// 00 : Reset
	b	undef_entry	// 04 : Undefined
	b	svc_entry	// 08 : SWI
	b	iabort_entry	// 0C : Prefetch abort
	b	dabort_entry	// 10 : Data Abort
	nop			// 14 : (Reserved)
	b	irq_entry	// 18 : IRQ

/*----------------------------------------------------------------------
	FIQ entry
----------------------------------------------------------------------*/
	.org	0x0000001c	// 1C : FIQ

fiq_entry:
	ldr	ip, =base(EIT_VECTBL)
	ldr	ip, [ip, #offs(EITVEC(EIT_FIQ))]
	bx	ip

/*----------------------------------------------------------------------
	IRQ entry
----------------------------------------------------------------------*/
/*
 *	Called by interrupt entry routine on the state saved in the interrupt 
 *	stack as shown below.
 *		+---------------+
 *	sp  ->	|R3		|
 *		|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		| Return address
 *		+---------------+
 *
 *	ip = Vector table address
 *	r3 = CPU source ID
 */
irq_entry:
	sub	lr, lr, #4		// Return address adjustment
	stmfd	sp!, {ip, lr}		// Register save
	mrs	ip, spsr
	stmfd	sp!, {r3, ip}

	ldr	r3, =base(DIC_INTACK)
	ldr	r3, [r3, #offs(DIC_INTACK)]
	mov	lr, r3, lsl #22		// Interrupt ID (& 0x3ff)
        mov     lr, lr, lsr #22
	cmp	lr, #15			// <= 15 : MPSWI Vector
	bgt	0f
	ldr	ip, =base(DIC_INTEOI)
	str	r3, [ip, #offs(DIC_INTEOI)]
0:
	cmp	lr, #127
	movgt	lr, #0			// > 127  : 0

	and	r3, r3, #0xc00		// CPU source ID
	mov	r3, r3, lsr #10

	ldr	ip, =EITVEC(EIT_IRQ(0))
	ldr	lr, [ip, lr, lsl #2]!
	cmp	lr, #0
	bxne	lr
	b	default_entry

/*
 * GPIO interrupt (Cascade connection to IRQ)
 *		+---------------+
 *	sp  ->	|R3		|
 *		|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		| <- Return address
 *		+---------------+
 *	ip = Vector table address
 */
_irqGPIOHdr:
	ldr	lr, =base(GPIO_INTREG)
	ldr	lr, [lr, #offs(GPIO_INTREG)]
	cmp	lr, #0
	beq	irqGPIOret
	sub	ip, lr, #1
	eor	lr, lr, ip
	clz	ip, lr
	rsb	lr, ip, #31
	ldr	ip, =EITVEC(EIT_GPIO(0))
	ldr	lr, [ip, lr, lsl #2]!
	cmp	lr, #0
	bxne	lr
	b	default_entry

irqGPIOret:
	ldr	ip, =base(DIC_INTEOI)
	mov	r3, #94
	str	r3, [ip, #offs(DIC_INTEOI)]
	ldmfd	sp!, {r3, ip}
	msr	spsr_fsxc, ip
	ldmfd	sp!, {ip, pc}^

/*----------------------------------------------------------------------
	Undef entry
----------------------------------------------------------------------*/
/*
 *		+---------------+
 *	sp  ->	|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		| <- Return address
 *		+---------------+
 *	ip = Vector table address
 */
undef_entry:
	stmfd	sp!, {ip, lr}		// Register save
	mrs	ip, spsr
	stmfd	sp!, {ip}

	ldr	ip, =base(EITVEC(EIT_UNDEF))
	ldr	lr, [ip, #offs(EITVEC(EIT_UNDEF))]!
	cmp	lr, #0
	bxne	lr
	b	default_entry

/*----------------------------------------------------------------------
	SWI entry
----------------------------------------------------------------------*/
/*
 *		+---------------+
 *	sp  ->	|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		| <- Return address
 *		+---------------+
 *	ip = Vector table address
 */
svc_entry:
	stmfd	sp!, {ip, lr}		// Register save
	mrs	ip, spsr
	stmfd	sp!, {ip}

	tst	ip, #PSR_T
	ldrneh	ip, [lr, #-2]		// Thumb state
	ldreq	ip, [lr, #-4]		// ARM state
	bicne	lr, ip, #0xff00
	biceq	lr, ip, #0xff000000
	cmp	lr, #N_INTVEC		// lr = Vector number
	movge	lr, #0

	ldr	ip, =EIT_VECTBL
	ldr	lr, [ip, lr, lsl #2]!
	cmp	lr, #0
	bxne	lr
	b	default_entry

/*----------------------------------------------------------------------
	Prefetch abort
----------------------------------------------------------------------*/
/*
 *		+---------------+
 *	sp  ->	|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		| <- Return address
 *		+---------------+
 *	ip = Vector table address
 */
iabort_entry:
	sub	lr, lr, #4		// Return address adjustment
	stmfd	sp!, {ip, lr}		// Register save
	mrs	ip, spsr
	stmfd	sp!, {ip}

	ldr	ip, =base(EITVEC(EIT_IABORT))
	ldr	lr, [ip, #offs(EITVEC(EIT_IABORT))]!
	cmp	lr, #0
	bxne	lr
	b	default_entry

/*----------------------------------------------------------------------
	Data abort
----------------------------------------------------------------------*/
/*
 *		+---------------+
 *	sp  ->	|SPSR		|
 *		|R12=ip		|
 *		|R14=lr		| <- Return address
 *		+---------------+
 *	ip = Vector table address
 */
dabort_entry:
	sub	lr, lr, #8		// Return address adjustment
	stmfd	sp!, {ip, lr}		// Register save
	mrs	ip, spsr
	stmfd	sp!, {ip}

	ldr	ip, =base(EITVEC(EIT_DABORT))
	ldr	lr, [ip, #offs(EITVEC(EIT_DABORT))]!
	cmp	lr, #0
	bxne	lr
	b	default_entry

/*----------------------------------------------------------------------
	Default entry
----------------------------------------------------------------------*/
default_entry:
	ldr	lr, =base(EITVEC(EIT_DEFAULT))
	ldr	lr, [lr, #offs(EITVEC(EIT_DEFAULT))]
	bx	lr

/*----------------------------------------------------------------------
	Default handler
----------------------------------------------------------------------*/
_defaultHdr:
	b	_defaultHdr

/*----------------------------------------------------------------------
	SVC entry
----------------------------------------------------------------------*/
_svcHdr:
// Acquire lock
	stmfd	sp!, {r1}
	ldr	ip, =svclock
	mov	lr, #1
0:
	ldrex	r1, [ip]
	cmp	r1, lr
	strexne	r1, lr, [ip]
	cmpne	r1, lr
	beq	0b
	mcr	p15, 0, r1, c7, c10, 5		// DMB
	ldmfd	sp!, {r1}

// ip = Function code
	ldr	ip, [sp, #4]
// Call SVC
	stmfd	sp!, {r3}
	mov	r3, r2
	mov	r2, r1
	mov	r1, r0
	mov	r0, ip
	bl	procSVC
	add	sp, sp, #4

// Release lock
	ldr	ip, =svclock
	mov	lr, #0
	mcr	p15, 0, lr, c7, c10, 5		// DMB
	str	lr, [ip]
	mcr	p15, 0, lr, c7, c10, 5		// DMB

	ldmfd	sp!, {ip}
	msr	spsr_fsxc, ip
	ldmfd	sp!, {ip, pc}^

/*----------------------------------------------------------------------
	Reset processing
----------------------------------------------------------------------*/
reset_entry:
	mov	r0, #(PSR_SVC | PSR_I | PSR_F)
	msr	cpsr_fsxc, r0

	mrc	p15, 0, r0, c0, c0, 5	// cpuid(*)
	cmp	r0, #0
	ldreq	sp, = CORE0_STACK
	cmp	r0, #1
	ldreq	sp, = CORE1_STACK
	cmp	r0, #2
	ldreq	sp, = CORE2_STACK
	cmp	r0, #3
	ldreq	sp, = CORE3_STACK

// Disable MMU and cache
	mrc	p15, 0, r0, cr1, c0, 0
	ldr	r1, =~(CR1_M | CR1_C | CR1_Z | CR1_I)
	and	r0, r0, r1
	mcr	p15, 0, r0, cr1, c0, 0
	nop
	nop

// CPU distinction
	mrc	p15, 0, r0, c0, c0, 5	// cpuid(*)
	cmp	r0, #0
	beq	cpu0_process

// CPU1-3 waits here
	ldr	r1, =SYSCTRL_RESET_STATUS
0:
	ldr	r2, [r1]
	tst	r2, #0x00010000			// HOTRST_STAT
	bne	0b

	ldr	r1, =SYSCTRL_MEMO		// MEMO
0:
	ldr	r2, [r1]
	cmp	r2, #0
	beq	0b
	b	_skip_001

// CPU0 processing - start
cpu0_process:
	ldr	r0, =init_data
0:
	ldr	r1, [r0], #4
	ldr	r2, [r0], #4
	cmp	r1, #0
	beq	1f
	str	r2, [r1]
	b	0b
1:

// Clear work area
	mov	r0, #0
	ldr	r1, =WORK_TOP
	ldr	r2, =WORK_END
1:
	cmp	r1, r2
	strcc	r0, [r1], #4
	bcc	1b

// data
	ldr	r0, =__data_start	//RAM Start address
	ldr	r1, =_edata		//RAM End address
	ldr	r3, =__data_org		//ROM Start address
1:
	cmp	r0, r1
	ldrcc	r2, [r3], #4		// --> LDRCC r2, [r3] + ADD r3, r3, #4
	strcc	r2, [r0], #4		// --> STRCC r2, [r0] + ADD r0, r0, #4
	bcc	1b

// bss
	ldr	r0, =__bss_start
	ldr	r1, =_end
	mov	r2, #0
1:
	cmp	r0, r1
	strcc	r2, [r0], #4
	bcc	1b

// SCU
	ldr	r0, =SCU_CPUSTAT
	mov	r1, #0
	str	r1, [r0]

// CPU0 processing - end

_skip_001:

// MMU configuration
	ldr	r1, =PAGETBL_BASE
	mcr	p15, 0, r1, cr2, cr0, 0		// Translation Table Base 0 (top 1GB)
	mcr	p15, 0, r1, cr2, cr0, 1		// Translation Table Base 1 (bot 3GB)
	mov	r1, #2				// 31-30
	mcr	p15, 0, r1, cr2, cr0, 2		// Translation Table Base Control
	ldr	r1, =0x55555555			// All Client
	mcr	p15, 0, r1, cr3, cr0, 0		// Domain Access

	mov	r1, #0
	mcr	p15, 0, r1, cr13, cr0, 0	// FCSE PID
	mcr	p15, 0, r1, cr13, cr0, 1	// ASID
	nop

// Reset processing
	bl	procReset

// Cache configuration
	mov	r1, #0

// Invalidate Entire Data Cache
	mcr	p15, 0, r1, cr7, c6, 0
	nop
	nop

// Invalidate Entire Instruction Cache. Also flushes the branch target cache
	mcr	p15, 0, r1, cr7, c5, 0
	nop
	nop

// Flush Prefetch Buffera
	mcr	p15, 0, r1, cr7, c5, 4
	nop
	nop

// Invalidate TLB
	mcr	p15, 0, r1, cr8, c7, 0
	nop
	nop

// Drain Synchronization Barriera
	mcr	p15, 0, r1, cr7, c10, 4
	nop
	nop

// Enable MMU and cache
	mrc	p15, 0, r0, cr1, c0, 0
	ldr	r1, =(CR1_M | CR1_A | CR1_C | CR1_Z | CR1_I | CR1_XP)
	orr	r0, r0, r1
	mcr	p15, 0, r0, cr1, c0, 0
	nop
	nop

// SMP mode
	mrc	p15, 0, r0, c1, c0, 1
	orr	r1, r0, #(1<<5)
	mcr	p15, 0, r1, c1, c0, 1

// CPU distinction
	mrc	p15, 0, r0, c0, c0, 5		// cpuid(*)
	cmp	r0, #0
	beq	_skip_002

	mov	r1, #0x00010000
0:
	subs	r1, r1, #1
	bne	0b

// CPU1-3 processing
	ldr	r1, =SYSCTRL_MEMO
	ldr	r0, [r1]
	b	_skip_003

_skip_002:

// CPU0 processing
	ldr	r0, =ROMINFO
	ldr	r0, [r0]

	ldr	r1, =SYSCTRL_MEMO
	str	r0, [r1]

// Release CPU1-3
	ldr	r1, =SYSCTRL_RESET_STATUS
	mov	r2, #0x00010000
	ldr	r3, [r1]
	tst	r3, r2				// HOTRST_STAT
	strne	r2, [r1]

_skip_003:
	mov	r1, #0
	mcr	p15, 0, r1, cr7, c14, 0
	mcr	p15, 0, r1, cr7, c5, 0
	mcr	p15, 0, r1, cr7, c10, 4
	mcr	p15, 0, r1, cr7, c5, 4

// To T-Kernel (No return)
	bx	r0
	nop

// ----------------------------------------------------------------------------
// Table of initialization data
// ----------------------------------------------------------------------------
init_data:
@;*DDR2 Init
    .word 0x18021044,0x30022123
    .word 0x18021058,0x00000001
    .word 0x18021008,0x00000020
@;*delay(Memo Register dummy write)
    .word 0x18037C00,0x00000000
    .word 0x18021008,0x10000004
    .word 0x18021008,0x00010002
    .word 0x18021008,0x00018002
    .word 0x18021008,0x00008002
    .word 0x18021008,0X1D480002
    .word 0x18021008,0x10000004
    .word 0x18021008,0x00000001
    .word 0x18021008,0x00000001
@;*delay(Memo Register dummy write)
    .word 0x18037C00,0x00000000
    .word 0x18037C00,0x00000000
    .word 0x18037C00,0x00000000
    .word 0x18021008,0x19480002
    .word 0x18021008,0x01308002
    .word 0x18021008,0x00000100
    .word 0x18021040,0x1485A912
    .word 0x18021034,0x00000121
@;*SysCon Init
@;*    .word 0x18037C80,0x007F0103
    .word 0x18037C80,0x00000000
    .word 0x18037C14,0x00000000
@;*ExBus Init
    .word 0x1801A000,0x0000004A
    .word 0x1801A004,0x08000049
    .word 0x1801A008,0x0600004E
    .word 0x1801A00C,0x0400004B
    .word 0x1801A010,0x1000004A
    .word 0x1801A014,0x1400000A
    .word 0x1801A020,0x10388E7F
    .word 0x1801A024,0x10388E7E
    .word 0x1801A028,0x10388E7E
    .word 0x1801A02C,0x10388E7F
    .word 0x1801A030,0x10388E7E
    .word 0x1801A034,0x10388E7E
    .word 0x0,       0x0
