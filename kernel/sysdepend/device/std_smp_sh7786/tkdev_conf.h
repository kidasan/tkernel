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
 *	tkdev_conf.h (SH7786)
 *	Target System Configuration 
 */

#ifndef _TKDEV_CONF_
#define _TKDEV_CONF_

/*
 * Timer TMU0
 */
#define VECNO_TMU0	( 0x400 >> 5 )	/* TMU0 interrupt vector number */

#define TSTR0		0xFFD80004	/* B:Timer start register 0 */
#define TCOR0		0xFFD80008	/* W:Timer constant register 0 */
#define TCNT0		0xFFD8000c	/* W:Timer counter 0 */
#define TCR0		0xFFD80010	/* H:Timer control register 0 */

#define TSTR_STR0	0x01U		/* TMU0 start (TSTR0) */
#define TSTR_STR1	0x02U		/* TMU1 start (TSTR0) */
#define TSTR_STR2	0x04U		/* TMU2 start (TSTR0) */

#define TCR_UNF		0x0100U		/* Underflow flag */
#define TCR_UNIE	0x0020U		/* Underflow interrupt enable */
#define TCR_CKEG	0x0018U		/* Clock edge */
#define TCR_TPSC	0x0007U		/* Timer prescaler */
#define TCR_TPSC_P4	0x0000U		/*	Peripheral clock/4 */
#define TCR_TPSC_P16	0x0001U		/*	Peripheral clock/16 */
#define TCR_TPSC_P64	0x0002U		/*	Peripheral clock/64 */
#define TCR_TPSC_P256	0x0003U		/*	Peripheral clock/256 */
#define TCR_TPSC_P1024	0x0004U		/*	Peripheral clock/1024 */
#define TCR_TPSC_EXT	0x0007U		/*	External clock */

/*
 * Timer interrupt
 */
#define TIMER_INTLEVEL		8			/* Interrupt mask register (0-15) */
#define TMU0_PRI		(TIMER_INTLEVEL << 1)	/* PRI LV * 2 */
#define TMU0_SFT		24			/* INT2PRI01 bit28-24 */

#define INT2MSKCLR1_TMU0	0x80000000

/*
 * Interrupt Controller(INTC)
 * Definition related to the Inter-processor interrupt
 */
#define INTICI0_VEC	0xF00		/* diapatch request interrupt */
#define INTICI2_VEC	0xF40		/* alarm & cyclic handoler request interrupt */
#define ICI0INTPRI	14		/* diapatch request interrupt level */
#define ICI2INTPRI	(14 << 8)	/* alarm & cyclic handoler request interrupt level */

#endif /* _TKDEV_CONF_ */
