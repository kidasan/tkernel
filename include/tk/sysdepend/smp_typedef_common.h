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
 *	@(#)smp_def_common.h (tk)
 *
 *	System dependencies definition.
 */

#ifndef __TK_SMP_TYPEDEF_COMMON_H__
#define __TK_SMP_TYPEDEF_COMMON_H__

#if STD_SMP_NAVIENGINE
#  include <tk/sysdepend/std_smp_naviengine/smp_typedef_depend.h>
#endif
#if STD_SMP_SH7776
#  include <tk/sysdepend/std_smp_sh7776/smp_typedef_depend.h>
#endif
#if STD_SMP_SH7786
#  include <tk/sysdepend/std_smp_sh7786/smp_typedef_depend.h>
#endif

#endif /* __TK_SMP_TYPEDEF_COMMON_H__ */
