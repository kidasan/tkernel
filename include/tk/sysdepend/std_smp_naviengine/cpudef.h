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
 *	@(#)cpudef.h (tk/NAVIENGINE)
 *
 *	NAVIENGINE dependent definition
 */

#ifndef __TK_CPUDEF_H__
#define __TK_CPUDEF_H__

#ifndef _in_asm_source_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * General purpose register			tk_get_reg tk_set_reg
 */
typedef struct t_regs {
	VW	r[13];		/* General purpose register R0-R12 */
	VP	lr;		/* Link register R14 */
} T_REGS;

/*
 * Exception-related register		tk_get_reg tk_set_reg
 */
typedef struct t_eit {
	VP	pc;		/* Program counter R15 */
	UW	cpsr;		/* Program status register */
	UW	taskmode;	/* Task mode flag */
} T_EIT;

/*
 * Control register			tk_get_reg tk_set_reg
 */
typedef struct t_cregs {
	VP	ssp;		/* System stack pointer R13_svc */
	VP	usp;		/* User stack pointer R13_ usr */
	VP	uatb;		/* Address of task specific space page table */
	UW	lsid;		/* Task logical space ID */
} T_CREGS;

/*
 * Coprocessor register	tk_get_cpr tk_set_cpr
 */
typedef union {
#if 0
	T_COP0REGS	cop0;
	T_COP1REGS	cop1;
	T_COP2REGS	cop2;
	T_COP3REGS	cop3;
#else
	UW		copDummy;
#endif
} T_COPREGS;

#ifdef __cplusplus
}
#endif

#endif /* _in_asm_source_ */

#endif /* __TK_CPUDEF_H__ */
