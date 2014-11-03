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
 *	tkdev_init.c (NAVIENGINE)
 *	T-Kernel Device-Dependent Initialization/Finalization
 */

#include "kernel.h"
#include <tk/sysdef.h>
#include <tk/syslib.h>
#include <tm/tmonitor.h>

/*
 * Target system-dependent initialization
 */
EXPORT ER tkdev_initialize( void )
{
	/* Disable GPIO PORT_OE */
	out_w(GPIO_PCDREG, 0xffffffff);

	/* Disable GPIO interrupt */
	out_w(GPIO_INDREG, 0xffffffff);

	/* Clear GPIO interrupt */
	out_w(GPIO_INTREG, 0xffffffff);

	/* Set interrupt hold register */
	out_w(GPIO_INHREG, 0xffffffff);

	/* Enable cascade connection */
	SetIntMode(IV_IRQ(IRQ_GPIO), IM_LEVEL | IM_1N);

	EnableInt(IV_IRQ(IRQ_GPIO), 4);

	return E_OK;
}

/*
 * Target system-dependent finalization
 *	Normally jump to ROM monitor.
 *	No return from this function.
 */
EXPORT void tkdev_exit( void )
{
	disint();
	tm_exit(0);	/* Turn off power and exit */

	/* Not suppose to return from 'tm_exit,' but just in case */
	for ( ;; ) {
		tm_monitor();  /* To T-Monitor */
	}
}
LOCAL void mp_interrupt( void )
{
	/* dispatch occured end of interrupt */
}
LOCAL void handler_interrupt( void )
{
IMPORT void handler_execution( void );

	ENTER_CRITICAL_SECTION;
	handler_execution();
	LEAVE_CRITICAL_SECTION;
}
EXPORT void mp_int_init( void )
{
	T_DINT	pk_dint;

	if ( get_prid() == 0 ) {
		/* Interrupt handler definition */
		pk_dint.intatr = TA_HLNG;
		pk_dint.inthdr = mp_interrupt;
		tk_def_int(VECNO_IPI_DSP, &pk_dint);

		/* Interrupt handler definition */
		pk_dint.intatr = TA_HLNG;
		pk_dint.inthdr = handler_interrupt;
		tk_def_int(VECNO_IPI_HDR, &pk_dint);
	}

	/* Set interrupt Mode */
	SetIntMode(VECNO_IPI_DSP, IM_EDGE | IM_NN);
	/* Inter-Processor Interrupt enable*/
	EnableInt(VECNO_IPI_DSP, IPI_DSP_INTLEVEL);

	/* Set interrupt Mode */
	SetIntMode(VECNO_IPI_HDR, IM_EDGE | IM_NN);
	/* Inter-Processor Interrupt enable*/
	EnableInt(VECNO_IPI_HDR, IPI_HDR_INTLEVEL);
}
