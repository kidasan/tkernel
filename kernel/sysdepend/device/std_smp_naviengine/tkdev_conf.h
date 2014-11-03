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
 *	tkdev_conf.h (NAVIENGINE)
 *	Target System Configuration
 */

#ifndef _TKDEV_CONF_
#define _TKDEV_CONF_
/* Also included from assembler source */

/*
 * Inter-Processor Interrupt
 */
#define IPI_DSP_ID		(0)
#define VECNO_IPI_DSP		(EIT_MPSWI(IPI_DSP_ID))
#define IPI_DSP_INTLEVEL	(0)

#define IPI_HDR_ID		(2)
#define VECNO_IPI_HDR		(EIT_MPSWI(IPI_HDR_ID))
#define IPI_HDR_INTLEVEL	(0)

/*
 * Private Timer
 */
/* Reg */
#define PTIMER_BASE(cpu)	(0xC0000600 + 0x0100*(cpu))
#define PTIMER_LOAD		(PTIMER_BASE(0) + 0x00)	
#define PTIMER_COUNTER		(PTIMER_BASE(0) + 0x04)	
#define PTIMER_CONTROL		(PTIMER_BASE(0) + 0x08)	
#define PTIMER_STATUS		(PTIMER_BASE(0) + 0x0C)	
/* Control */
#define PTCR_ENABLE		(0x01)
#define PTCR_RELOAD		(0x02)
#define PTCR_INTEN		(0x04)
#define PTCR_PRESCALER		(8)
/* IRQ */
#define PTM_IRQ			(29)
#define VECNO_PTM		(EIT_IRQ(PTM_IRQ))

/* Timer interrupt level */
#define TIMER_INTLEVEL		(8)		/* 0 - 15 */

/* Clock */
#define CPU_CLK_KHZ		(400*1000)	/* CPU Clock 400 MHz */
#define TIMER_PRESCALER		(200)
#define TIMER_COUNTER		(((TIMER_PERIOD * (CPU_CLK_KHZ/TIMER_PRESCALER)) / 2) - 1)

#endif /* _TKDEV_CONF_ */
