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
 *	version.c (T-Kernel/OS)
 *	Version Information
 */

#include <basic.h>
#include <tk/tkernel.h>
#include "version.h"

EXPORT const T_RVER kernel_version = {
	RV_MAKER,		/* Kernel manufacturing code */
	RV_PRODUCT_ID,		/* Kernel ID number */
	RV_SPEC_VER,		/* Specification version number */
	RV_PRODUCT_VER,		/* Kernel version number */

	{
		/* Product management information */
		RV_PRODUCT_NO1,
		RV_PRODUCT_NO2,
		RV_PRODUCT_NO3,
		RV_PRODUCT_NO4
	}
};
