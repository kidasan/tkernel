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
 *	icrt0.S (SH7776)
 *	System Startup
 */

#include <machine.h>
#include <tk/sysdef.h>

	/* Low level memory manager information */
	.comm	Csym(lowmem_top), 4	// Head of area (Low address)
	.comm	Csym(lowmem_limit), 4	// End of area (High address)

	.lcomm	monitor_stacktop, 8	// Monitor stack pointer for re-startup( 4 * 2 Processors)

	.text
	.balign	2
	.globl	Csym(start)
	.type	Csym(start), @function
Csym(start):
	/* Use the stack set by the boot loader */

	mov.l	INIT_SR, r0		// SR initial setting
	ldc	r0, sr

	/* SR.MD=1, SR.BL=0, SR.RB=0, SR.I=15 */
	mov.l	CACHE_ON, r0		// Cache on
	jsr	@r0
	nop

	/* It jumps to the initialization of the monitor stack if I am not the first processor. */
	mov.l	L_CPIDR, r0		// Get CPU core ID
	mov.l	@r0, r0
	and	#3, r0
	cmp/pl	r0
	bt	l2

	mov.l	DATA_ORG, r1		// Initialization of 'data' area (ROM startup)
	mov.l	DATA_START, r2
	mov.l	EDATA, r3
	cmp/eq	r1, r2			// If r1 != r2, start ROM
	bt/s	nocopy_data
	movt	r8			// If start ROM, r8 = 0
  copy_data:
	cmp/hi	r2, r3
	bf	nocopy_data
	mov.l	@r1+, r0
	mov.l	r0, @r2
	bra	copy_data
	add	#4, r2
  nocopy_data:

	mov	#0, r0			// Clear 'bss' area
	mov.l	BSS_TOP, r1
	mov.l	BSS_END, r2
	cmp/hi	r1, r2
	bf	nobss
	add	#4, r1
  clrbss:
	cmp/hi	r1, r2
	bt/s	clrbss
	mov.l	r0, @-r2
  nobss:

	mov.l	RAM_TOP, r1		// Low level memory manager initial setting
	mov.l	@r1, r1
	mov.l	BSS_END, r0
	cmp/hi	r0, r1			// _end or RAM_TOP
	bt	l1			// Either of High addresses
	mov	r0, r1
  l1:
	mov.l	RAM_END, r3
	mov.l	@r3, r3
	mov.l	LOWMEM_TOP, r2
	mov.l	LOWMEM_LIM, r4
	mov.l	r1, @r2			// lowmem_top   = RAM_TOP or _end
	mov.l	r3, @r4			// lowmem_limit = RAM_END

	tst	r8, r8			// If r8 == 0, start Rom
	bf	l2
	mov.l	ROM_STARTUP, r0		// Initialization at ROM startup
	jsr	@r0
	nop
  l2:
	mov.l	L_CPIDR, r0		// Get CPU core ID
	mov.l	@r0, r0
	and	#3, r0
	shll2	r0			// cpuid * sizeof(INT)
	mov.l	MON_STACKTOP, r1	// Monitor stack pointer for re-startup
	add	r0, r1			// r1 = &monitor_stacktop[cpuid]
	mov.l	r15, @r1		// Save

	/* It jumps to the system startup if I am not the first processor. */
	mov.l	L_CPIDR, r0		// Get CPU core ID
	mov.l	@r0, r0
	and	#3, r0
	cmp/pl	r0
	bt	l3

	/* Initializetion Spin lock for system synchronization (executes it only with CPU0) */
	mov.l	L_waitlock, r0
	mov.l	L_NONCACHE, r1
	or	r1, r0				/* non cache area access */
	mov	#0, r1
	mov.b	r1, @r0

	/* other core start processing (executes it only with CPU0) */
	mov.l	L_STBCR_MSTP, r1
	mov.l	L_C1STBCR, r0
	mov.l	r1, @r0

  l3:
	mov.l	MAIN, r0		// System start up
	jsr	@r0
	nop

  l_end:				// Not suppose to return from 'main,'
	bra	l_end			// but, just in case, prepare for out of control.
	nop

		.balign	4
  INIT_SR:	.long	SR_MD | SR_I(15)
  DATA_ORG:	.long	Csym(__data_org)
  DATA_START:	.long	Csym(__data_start)
  EDATA:	.long	Csym(_edata)
  BSS_TOP:	.long	Csym(__bss_start)
  BSS_END:	.long	Csym(_end)
  LOWMEM_TOP:	.long	Csym(lowmem_top)
  LOWMEM_LIM:	.long	Csym(lowmem_limit)
  ROM_STARTUP:	.long	Csym(ROM_startup)
  MAIN:		.long	Csym(main)
  MON_STACKTOP:	.long	monitor_stacktop
  L_CPIDR:	.long	CPIDR

	/* System shared information */
  RAM_TOP:	.long	0x88000280
  RAM_END:	.long	0x88000284

  L_waitlock:	.long	_waitlock
  L_NONCACHE:	.long	0xa0000000
  L_C1STBCR:	.long	C1STBCR
  L_STBCR_MSTP:	.long	0x00000000

/*
 * When accessing the cache control register, the program itself
 * must be in the non-cache area. Therefore convert to the address
 * of the shadow image in P2 area and call.
 */
CACHE_ON:	.long	cache_on + 0x20000000

/* ------------------------------------------------------------------------ */
/*
 *	Cache control
 */

#define	CACHE_LINESZ	0x20		/* Cache line size */

/*
 * Cache on
 */
	.text
	.balign	2
cache_on:
	sts.l	pr, @-SP

	bsr	cache_off		// Disable cache once.
	nop

	mov.l	C_PASCR, r0
	mov.l	C_PASCR_SE, r1
	and	r1, r0
	tst	r0, r0
	bt	on1			// 29-bit address mode

	/*** 32-bit address extended mode,  P1 area:Write-through ***/
	mov.l	C_PMB_DAT_ENT0, r0
	mov.l	C_PMB_P1, r1
	mov.l	r1, @r0

	mov.l	C_PMB_DAT_ENT1, r0
	mov.l	C_PMB_P2, r1
	mov.l	r1, @r0

  on1:
	mov.l	C_CCR, r0
	mov.l	C_CACHE_ON, r1
	mov.l	r1, @r0			// Cache on (Writeback is enabled)

	nop;nop;nop;nop;nop;nop;nop

	lds.l	@SP+, pr
	rts
	nop

/*
 * Cache off
 */
	.text
	.balign	2
cache_off:
	mov	#0, r0			// Flush cache
	mov.l	C_CACHE_TAGTOP, r1	// (Writeback and disable)
	mov.l	C_CACHE_TAGEND, r2
  l_flush:
	mov.l	r0, @r1
	add	#CACHE_LINESZ, r1
	cmp/hi	r1, r2
	bt	l_flush

	mov.l	C_PASCR, r0
	mov.l	C_PASCR_SE, r1
	and	r1, r0
	tst	r0, r0
	bt	off1			// 29-bit address mode

	/*** 32-bit address extended mode ***/
	mov.l	C_PMB_DAT_ENT0, r1
	mov.l	C_PMB_DAT_C_MASK, r0
	mov.l	@r1, r2
	and	r0, r2
	mov.l	r2, @r1

	mov.l	C_PMB_DAT_ENT1, r1
	mov.l	@r1, r2
	and	r0, r2
	mov.l	r2, @r1

  off1:
	mov.l	C_CCR, r1		// Disable cache and
	mov.l	C_CCR_MASK, r0		// invalidate instruction cache

	mov.l	@r1, r2
	and	r0, r2
	mov.l	C_CACHE_OFF, r0
	or	r0, r2
	mov.l	r2, @r1

	mov.l	C_L2CR, r1
	mov.l	C_L2CR_L2CI, r0		// invalidate L2 cache
	mov.l	r0, @r1

	rts
	nop

			.balign	4
  C_CCR:		.long	CCR
  C_L2CR:		.long	L2CR
  C_CACHE_ON:		.long	CCR_OCE|CCR_CB|CCR_OCI|CCR_L2OE|CCR_ICE|CCR_ICI|CCR_L2IE
  C_CACHE_OFF:		.long	CCR_CCD|CCR_ICI		// Invalidate instruction cache
  C_L2CR_L2CI:		.long	L2CR_L2CI

  C_CCR_MASK:		.long	CCR_MCP|CCR_SNM
  C_CACHE_TAGTOP:	.long	0xf4000000	// Data cache only
  C_CACHE_TAGEND:	.long	0xf4008000

  C_PASCR:		.long	PASCR
  C_PASCR_SE:		.long	PASCR_SE
  C_PMB_ADR_TOP:	.long	0xf6100000	// PMB address array
  C_PMB_DAT_TOP:	.long	0xf7100000	// PMB data array
  C_PMB_DAT_ENT0:	.long	0xf7100000	// PMB data array entry=0(P1 area)
  C_PMB_DAT_ENT1:	.long	0xf7100100	// PMB data array entry=1(P2 area)
  C_PMB_P1:		.long	0x00000198	// V=1, SZ:512M, C=1, WT=0
  C_PMB_P2:		.long	0x00000190	// V=1, SZ:512M, C=0, WT=0
  C_PMB_DAT_C_MASK:	.long	~0x00000008
