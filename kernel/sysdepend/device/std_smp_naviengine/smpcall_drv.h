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
 *	smpcall_drv.h (NAVIENGINE)
 *
 *	Inter-processor communication driver header file
 */

#ifndef __SMPCALL_DRV_H__
#define __SMPCALL_DRV_H__

/*
---------------------------------------------------------------------------------------------------
	System configuration(SYSCONF)
---------------------------------------------------------------------------------------------------
*/
#define SCTAG_SMPCALL_IPI_IRQ	((UB*)"SmpcallIpiIrq")
#define SCTAG_SMPCALL_IPI_LEVEL	((UB*)"SmpcallIpiLevel")

/*
---------------------------------------------------------------------------------------------------
	Definition of inter-processor interrupt
---------------------------------------------------------------------------------------------------
*/
#define IPI_SWI_CPUBIT		(16)

/*
---------------------------------------------------------------------------------------------------
	Definition of processor dependence information
---------------------------------------------------------------------------------------------------
*/
typedef struct _SMPC_PINFO_ {
	UW	proc_mask;
	UW	status;
	FP	func;
	VP	param;
} SMPC_PINFO;

#define ACP_MASK	(0x0000ffff)
#define EXE_MASK	(0xffff0000)
#define EXE_SHIFT	(16)
#define ACP_BIT(n)	(1 << (n))
#define EXE_BIT(n)	(1 << ((n)+EXE_SHIFT))

/*
---------------------------------------------------------------------------------------------------
	Definition of smp-call driver information
---------------------------------------------------------------------------------------------------
*/
#define MAX_PROC_NUM		(4)		/* Number of maximum processor */

typedef struct _SMPC_DRVCB_ {
	UW		valid;
	UW		ipi_irq;
	UW		ipi_level;
	T_SPLOCK	lock;
	UINT		intsts;
	UW		available_proc;
	SMPC_PINFO	proc_info[MAX_PROC_NUM];
} SMPC_DRVCB;

#define get_smpc_dcb()		(&smpc_dcb)
#define get_smpc_pinfo(dcb, id)	(&(dcb)->proc_info[(id)])

#define DCB_VALID		(1)
#define is_dcb_valid(dcb)	(dcb->valid == DCB_VALID)

#endif /* __SMPCALL_DRV_H__ */

