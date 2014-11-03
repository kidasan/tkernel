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
 *	tkdev_init.c (SH7776)
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
	/* Disable all interrupts */
	if(get_prid() == 0){
		out_w(C0INTPRI, 0);
		out_w(C0INTMSK0, 0xFE000000);
		out_w(C0INTMSK1, 0x80000000);
		out_w(C0INTMSK2, 0xFFFE0000);

		out_w(C0INT2PRI0, 0);
		out_w(C0INT2PRI1, 0);
		out_w(C0INT2PRI2, 0);
		out_w(C0INT2PRI3, 0);
		out_w(C0INT2PRI4, 0);
		out_w(C0INT2PRI5, 0);
		out_w(C0INT2PRI6, 0);
		out_w(C0INT2PRI7, 0);
		out_w(C0INT2PRI8, 0);
		out_w(C0INT2PRI9, 0);
		out_w(C0INT2PRI10, 0);
		out_w(C0INT2PRI11, 0);
		out_w(C0INT2PRI12, 0);
		out_w(C0INT2PRI13, 0);
		out_w(C0INT2PRI14, 0);
		out_w(C0INT2PRI15, 0);
		out_w(C0INT2PRI16, 0);
		out_w(C0INT2PRI17, 0);
		out_w(C0INT2PRI18, 0);
		out_w(C0INT2PRI19, 0);
		out_w(C0INT2PRI20, 0);
		out_w(C0INT2PRI21, 0);
		out_w(C0INT2PRI22, 0);
/*		out_w(C0INT2PRI23, 0);*/		/* Reserved */
/*		out_w(C0INT2PRI24, 0);*/		/* Reserved */
		out_w(C0INT2MSK0, 0xFFFFFFFF);
		out_w(C0INT2MSK1, 0xFFFFFFFF);
		out_w(C0INT2MSK2, 0xFFFFFFFF);
/*		out_w(C0INT2MSK3, 0xFFFFFFFF);*/	/* Reserved */
	}
	else{
		out_w(C1INTPRI, 0);
		out_w(C1INTMSK0, 0xFE000000);
		out_w(C1INTMSK1, 0x80000000);
		out_w(C1INTMSK2, 0xFFFE0000);

		out_w(C1INT2PRI0, 0);
		out_w(C1INT2PRI1, 0);
		out_w(C1INT2PRI2, 0);
		out_w(C1INT2PRI3, 0);
		out_w(C1INT2PRI4, 0);
		out_w(C1INT2PRI5, 0);
		out_w(C1INT2PRI6, 0);
		out_w(C1INT2PRI7, 0);
		out_w(C1INT2PRI8, 0);
		out_w(C1INT2PRI9, 0);
		out_w(C1INT2PRI10, 0);
		out_w(C1INT2PRI11, 0);
		out_w(C1INT2PRI12, 0);
		out_w(C1INT2PRI13, 0);
		out_w(C1INT2PRI14, 0);
		out_w(C1INT2PRI15, 0);
		out_w(C1INT2PRI16, 0);
		out_w(C1INT2PRI17, 0);
		out_w(C1INT2PRI18, 0);
		out_w(C1INT2PRI19, 0);
		out_w(C1INT2PRI20, 0);
		out_w(C1INT2PRI21, 0);
		out_w(C1INT2PRI22, 0);
/*		out_w(C1INT2PRI23, 0);*/		/* Reserved */
/*		out_w(C1INT2PRI24, 0);*/		/* Reserved */
		out_w(C1INT2MSK0, 0xFFFFFFFF);
		out_w(C1INT2MSK1, 0xFFFFFFFF);
		out_w(C1INT2MSK2, 0xFFFFFFFF);
/*		out_w(C1INT2MSK3, 0xFFFFFFFF);*/	/* Reserved */
	}

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
	/* only clear interrupt factor
	   dispatch occured end of interrupt */
	if(get_prid() == 0){
		out_w(C0INTICICLR, 0x0000000f);
	}
	else{
		out_w(C1INTICICLR, 0x0000000f);
	}
}

LOCAL void handler_interrupt( void )
{
IMPORT void handler_execution( void );

	/* only clear interrupt factor
	   dispatch occured end of interrupt */
	if(get_prid() == 0){
		out_w(C0INTICICLR, 0x00000f00);
	}
	else{
		out_w(C1INTICICLR, 0x00000f00);
	}

	ENTER_CRITICAL_SECTION;
	handler_execution();
	LEAVE_CRITICAL_SECTION;
}

EXPORT void mp_int_init( void )
{
	T_DINT	pk_dint;
	ER	ercd;

	/* Interrupt handler definition */
	if(get_prid() == 0){
		/* dispatch request */
		pk_dint.intatr = TA_HLNG;
		pk_dint.inthdr = mp_interrupt;
		ercd = tk_def_int(INTICI0_VEC, &pk_dint);

		/* alarm & cyclic handler request */
		pk_dint.intatr = TA_HLNG;
		pk_dint.inthdr = handler_interrupt;
		ercd = tk_def_int(INTICI2_VEC, &pk_dint);

		out_w(C0ICIPRI, ICI0INTPRI | ICI2INTPRI);	/* enable(CPU0) */
	}
	else{
		out_w(C1ICIPRI, ICI0INTPRI | ICI2INTPRI);	/* enable(CPU1) */
	}
}
