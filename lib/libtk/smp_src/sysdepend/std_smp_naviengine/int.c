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
 *	@(#)int.c (libtk/NAVIENGINE)
 *
 *	Interrupt controller operation
 */

#include <basic.h>
#include <tk/syslib.h>
#include <tk/sysdef.h>
#include <sys/smp_def.h>

/*
 * Interrupt disable/enable
 */
Inline UW _disint( void )
{
	UW	imask;
	Asm("	mrs	%0, cpsr	\n"
	"	orr	ip, %0, %1	\n"
	"	msr	cpsr_c, ip	"
		: "=r"(imask)
		: "i"(PSR_DI)
		: "ip" );
	return imask;
}
Inline void _enaint( UW imask )
{
	Asm("msr cpsr_c, %0":: "r"(imask));
}
#define _DI(imask)	( imask = _disint() )
#define _EI(imask)	( _enaint(imask) )

/*
 * My CPU ID (0 - 3)
 */
Inline W myCPUid( void )
{
	UW	id;

	Asm("mrc p15, 0, %0, cr0, c0, 5": "=r"(id));
	return id & 0x03;
}

/*
 * Set interrupt enable leve
 *	Disable All interrupts which level is less then or equal to specified
 *	'level'. 'level' must be '0'-'15'.
 *	If '15' is specified, all level interrupts are enable.
 *	If '0' is specified, all level interrupts are disable.
 *	'level' is set to the PRIMSK register of interrupt controller.
 *	Initial value is '15'.
 *	This function returns the interrupt enable level before being changed.
 */
EXPORT INT SetIntLevel( INT level )
{
        INT     oldlevel;
        UW      imask;

        _DI(imask);
        oldlevel = (*(_UW*)DIC_PRIMSK & 0x0000000f0U) >> 4;
        *(_UW*)DIC_PRIMSK = (level & 0x0f) << 4;
        _EI(imask);

        return oldlevel;
}

/*
 * Set interrupt mode
 *	Set accept mode to 'mode' regarding interrupt specified by 'intvec'.
 *	If 'mode' is not correct value, operation is not guaranteed.
 *   
 *	mode := (IM_LEVEL || IM_EDGE) | [IM_HI] | (IM_NN || IM_1N)
 */
EXPORT void SetIntMode( INTVEC intvec, UINT mode )
{
	INT	i;
	UW	imask;

	i = (INT)intvec;
	mode &= 0x03;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		_DI(imask);
		*(_UW*)DIC_CONFIG(i) = (*(_UW*)DIC_CONFIG(i) &
					~_BIT2(3, i)) | _BIT2(mode, i);
		_EI(imask);
	} else if ( i < IV_GPIO(32) ) {
		i -= IV_GPIO(0);
		_DI(imask);
		*(_UW*)GPIO_IMREG(i) = (*(_UW*)GPIO_IMREG(i) &
					~_BIT4(0xf, i)) | _BIT4(mode, i);
		_EI(imask);
	}
}

/*
 * Set interrupt assign processor
 */
EXPORT void SetIntAssprc( INTVEC intvec, UW assprc )
{
	INT	i, n;
	UW	imask, v;

	i = (INT)intvec;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		if ( i >= 32 ) {
			assprc &= (1 << MAX_PROC) - 1;
			_DI(imask);
			v = *(_UW*)DIC_CPUTRG(i) & ~_BIT8(0xf, i);
			if ( assprc == 0 ) {
				n = _BIT1(1,  myCPUid());
				*(_UW*)DIC_CPUTRG(i) = v | _BIT8(n, i);
			} else {
				*(_UW*)DIC_CPUTRG(i) = v | _BIT8(assprc, i);
			}
			_EI(imask);
		}
	} else if ( i < IV_GPIO(32) ) {
		;
	}
}

/*
 * Clear interrupt assign processor
 */
EXPORT void ClrIntAssprc( INTVEC intvec )
{
	INT	i;
	UW	imask;

	i = (INT)intvec;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		if ( i >= 32 ) {
			_DI(imask);
			*(_UW*)DIC_CPUTRG(i) &= ~_BIT8(0xf, i);
			_EI(imask);
		}
	} else if ( i < IV_GPIO(32) ) {
		;
	}
}

/*
 * Enable interrupt
 *	Enable interrupt specified by 'intvec'.
 */
EXPORT void EnableInt( INTVEC intvec, INT level )
{
	INT	i;
	UW	imask;

	i = (INT)intvec;
	level = (level & 0x0f) << 4;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		_DI(imask);
		*(_UW*)DIC_PRI(i) = (*(_UW*)DIC_PRI(i) &
					~ _BIT8(0xff, i)) | _BIT8(level, i);
		if ( i >= 16 ) {
			*(_UW*)DIC_ENBSET(i) = _BIT1(1, i);
		}
		_EI(imask);
	} else if ( i < IV_GPIO(32) ) {
		i -= IV_GPIO(0);
		*(_UW*)GPIO_INEREG = _BIT1(1, i);
	}
}

/*
 * Disable interrupt
 *	Disable interrupt specified by 'intvec'.
 */
EXPORT void DisableInt( INTVEC intvec )
{
	INT	i;

	i = (INT)intvec;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		if ( i >= 16 ) {
			*(_UW*)DIC_ENBCLR(i) = _BIT1(1, i);
		}
	} else if ( i < IV_GPIO(32) ) {
		i -= IV_GPIO(0);
		*(_UW*)GPIO_INDREG = _BIT1(1, i);
	}
}

/*
 * Clear interrupt request
 *	Clear interrupt request specified by 'intvec'.
 */
EXPORT void ClearInt( INTVEC intvec )
{
	INT	i;

	i = (INT)intvec;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		*(_UW*)DIC_PENDCLR(i) = _BIT1(1, i);
	} else if ( i < IV_GPIO(32) ) {
		i -= IV_GPIO(0);
		*(_UW*)GPIO_INTREG = _BIT1(1, i);
	}
}

/*
 * End of interrupt
 */
EXPORT void EndOfInt( INTVEC intvec )
{
	INT	i;

	i = (INT)intvec;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		if (i == IV_IRQ(0)) {
			i = 1023;		/* spurious interrupt */
		} else {
			i -= IV_IRQ(0);
		}

		*(_UW*)DIC_INTEOI = i;
	} else if ( i < IV_GPIO(32) ) {
		i -= IV_GPIO(0);
		*(_UW*)GPIO_INTREG = _BIT1(1, i);
		*(_UW*)DIC_INTEOI = IRQ_GPIO;	/* GPIO Cascade */
	}
}

/*
 * Check interrupt request
 *	Check interrupt request specified by 'intvec'.
 *	If interrupt request occurs, returns TRUE (nonzero).
 */
EXPORT BOOL CheckInt( INTVEC intvec )
{
	INT	i;
	UW	stat;

	i = (INT)intvec;
	stat = 0U;

	if ( i < IV_IRQ(0) ) {
		;
	} else if ( i < IV_GPIO(0) ) {
		i -= IV_IRQ(0);
		stat = *(_UW*)DIC_ACTIVE(i);
	} else if ( i < IV_GPIO(32) ) {
		i -= IV_GPIO(0);
		stat = *(_UW*)GPIO_INTREG;
	}

	return (stat & _BIT1(1, i)) ? TRUE : FALSE;
}
