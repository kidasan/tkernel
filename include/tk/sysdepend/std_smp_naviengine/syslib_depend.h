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
 *	@(#)syslib_depend.h (tk/NAVIENGINE)
 *
 *	T-Kernel/SM NAVIENGINE Library
 */

#ifndef __TK_SYSLIB_DEPEND_H__
#define __TK_SYSLIB_DEPEND_H__

#include <tk/errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CPU interrupt control
 *	'intsts' is the value of CPSR in CPU
 *	disint()  Set CPSR.I = 1 and return the original CPSR
 *		  to the return value.
 *	enaint()  Set CPSR.I = intsts.I. Do not change except
 *		  for CPSR.I.
 *		  Return the original CPSR to the return value.
 */
IMPORT UINT disint( void );
IMPORT UINT enaint( UINT intsts );
#define DI(intsts)	( (intsts) = disint() )
#define EI(intsts)	( enaint(intsts) )
#define isDI(intsts)	( ((intsts) & 0x00c0U) != 0 )

/*
 * Interrupt vector
 *	The interrupt vector is the index number of the vector table.
 */
typedef UINT	INTVEC;

/* Convert to interrupt definition number */
#define DINTNO(intvec)	(intvec)

/*
 * Interrupt vector value
 */
#define IV_FIQ		( 31 )		/* FIQ */
#define IV_IRQ(n)	( 32 + (n) )	/* INT 0-127 */
#define IV_MPSWI(n)	( IV_IRQ(n) )	/* MP SWINT 0-15 */
#define IV_GPIO(n)	( 160 + (n) )	/* GPIO 0-31 */
#define IV_MAX		( 255 )		/* Max Interrupt Vector */

#define IRQ_GPIO	( 94 )

/*
 * Interrupt controller operation
 *	Operation functions of interrupt controller  support IRQ, EB IRQ
 *	interrupts. Some functions treat part of those interrupts.
 *	FIQ interrupt is not supported.
 */

/*
 * Set interrupt enable level
 *	Disable All interrupts which level is less then or equal to specified
 *	'level'. 'level' must be '0'-'15'.
 *	If '15' is specified, all level interrupts are enable.
 *	If '0' is specified, all level interrupts are disable.
 *	'level' is set to the PRIMSK register of interrupt controller.
 *	Initial value is '15'.
 *	This function returns the interrupt enable level before being changed.
 */
IMPORT INT SetIntLevel( INT level );

/*
 * Set interrupt mode
 *	Set accept mode to 'mode' regarding interrupt specified by 'intvec'.
 *	If 'mode' is not correct value, operation is not guaranteed.
 */
IMPORT void SetIntMode( INTVEC intvec, UINT mode );

#define IM_LEVEL	0x00U	/* level trigger (high level Only) */
#define IM_EDGE		0x02U	/* edge  trigger (rising edge only) */
#define IM_HI		0x00U	/* high level trigger/rising  edge trigger */

#define IM_NN		0x00U	/* N-N Software Model */
#define IM_1N		0x01U	/* 1-N Software Model */

/*
 * Set interrupt assign processor
 */
IMPORT void SetIntAssprc( INTVEC intvec, UW assprc );

/*
 * Clear interrupt assign processor
 */
IMPORT void ClrIntAssprc( INTVEC intvec );

/*
 * Enable interrupt
 *	Enable interrupt specified by 'intvec'.
 *
 *	level := '0'-'15'.
 *	level=15 means lowest priority level, level=0 means highest.
 *	If 'level' is outside this range, operation is not guaranteed.
 *	Set the PRI register with the value 'level'.
 */
IMPORT void EnableInt( INTVEC intvec, INT level );

/*
 * Disable interrupt
 *	Disable interrupt specified by 'intvec'.
 */
IMPORT void DisableInt( INTVEC intvec );

/*
 * Clear interrupt request
 *	Clear interrupt request specified by 'intvec'.
 */
IMPORT void ClearInt( INTVEC intvec );

/*
 * Check for existence of interrupt request
 *	Check whether there is an interrupt request specified by 'intvec'.
 *	If there is an interrupt request, return TRUE (except 0).
 */
IMPORT BOOL CheckInt( INTVEC intvec );

/*
 * Issue EOI(End Of Interrupt)
 */
IMPORT void EndOfInt( INTVEC intvec );

/* ------------------------------------------------------------------------ */

/*
 * I/O port access
 *	Only memory mapped I/O for Arm
 */
Inline void out_w( INT port, UW data )
{
	*(_UW*)port = data;
}
Inline void out_h( INT port, UH data )
{
	*(_UH*)port = data;
}
Inline void out_b( INT port, UB data )
{
	*(_UB*)port = data;
}

Inline UW in_w( INT port )
{
	return *(_UW*)port;
}
Inline UH in_h( INT port )
{
	return *(_UH*)port;
}
Inline UB in_b( INT port )
{
	return *(_UB*)port;
}

#ifdef __cplusplus
}
#endif
#endif /* __TK_SYSLIB_DEPEND_H__ */
