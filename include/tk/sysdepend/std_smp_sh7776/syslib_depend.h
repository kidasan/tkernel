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
 *	@(#)syslib_depend.h (tk/SH7776)
 *
 *	T-Kernel/SM SH7776 Library
 */

#ifndef __TK_SYSLIB_DEPEND_H__
#define __TK_SYSLIB_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CPU interrupt control
 *	'intsts' is the value of SR register in CPU
 *	disint()  Set SR.I = 15 and return the original SR to the return value.
 *	enaint()  Set SR.I = intsts.I. Do not change except for SR.I.
 *		  Return the original SR to the return value.
 */
IMPORT UINT disint( void );
IMPORT UINT enaint( UINT intsts );
#define DI(intsts)	( (intsts) = disint() )
#define EI(intsts)	( enaint(intsts) )
#define isDI(intsts)	( ((intsts) & 0x00f0U) != 0 )

/*
 * Enable the higher level of interrupt than intlevel (0-15)
 *	Set 'intlevel' to SR.I in CPU.
 *	Return the original SR to the return value.
 *	Use when enabling a multiplexed interrupt in interrupt handler.
 */
#define SetIntLevel(intlevel)	( enaint((intlevel) << 4) )

/*
 * Request the interrupt level (1-15) by the value of INTEVT (0x200-0x3c0)
 */
#define IntLevel(intevt)	( 0x1f - ((intevt) >> 5) )

/*
 * Interrupt vector
 *	Interrupt vector is the value of the interrupt factor code INTVEC 2
 */
typedef UINT	INTVEC;

/* Convert to the interrupt definition number */
#define DINTNO(intvec)	(intvec)

/*
 * Interrupt vector(INTVEC) : Typical exception code
 *	Only these typical exception codes can be used with
 *	an INTC operation functions.
 */
#define IV_TMU0		0x400	/* Timer ch.0 */
#define IV_TMU1		0x420	/* Timer ch.1 */
#define IV_TMU2		0x440	/* Timer ch.2 */

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
IMPORT void EnableInt( INTVEC intvec, INT level );

/*
 * Disable interrupt
 *	Disable the external interrupts specified by 'intvec' by setting
 *	its priority level to '0'.
 *	Disable the peripheral module interrupts specified by 'intvec' by
 *	setting its priority level to '0' or '1'.
 */
IMPORT void DisableInt( INTVEC intvec );

/*
 * Issue EOI(End Of Interrupt)
 */
#define EndOfInt(intvec)

/* ------------------------------------------------------------------------ */

/*
 * I/O port access
 *	Only memory mapped I/O for SH
 */
Inline void out_w( INT port, UW data )
{
	Asm("mov.l %0, @%1":: "r"(data), "r"(port));
}
Inline void out_h( INT port, UH data )
{
	Asm("mov.w %0, @%1":: "r"(data), "r"(port));
}
Inline void out_b( INT port, UB data )
{
	Asm("mov.b %0, @%1":: "r"(data), "r"(port));
}

Inline UW in_w( INT port )
{
	UW	data;
	Asm("mov.l @%1, %0": "=r"(data): "r"(port));
	return data;
}
Inline UH in_h( INT port )
{
	UH	data;
	Asm("mov.w @%1, %0": "=r"(data): "r"(port));
	return data;
}
Inline UB in_b( INT port )
{
	UB	data;
	Asm("mov.b @%1, %0": "=r"(data): "r"(port));
	return data;
}

#ifdef __cplusplus
}
#endif
#endif /* __TK_SYSLIB_DEPEND_H__ */
