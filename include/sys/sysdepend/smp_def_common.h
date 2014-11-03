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
 *	@(#)smp_def_common.h (sys)
 *
 *	SymmetricMultiProcessor dependent definition
 */

#ifndef __SYS_SMP_DEF_COMMON_H__
#define __SYS_SMP_DEF_COMMON_H__

#if STD_SMP_NAVIENGINE
#  include <sys/sysdepend/std_smp_naviengine/smp_def_depend.h>
#endif
#if STD_SMP_SH7776
#  include <sys/sysdepend/std_smp_sh7776/smp_def_depend.h>
#endif
#if STD_SMP_SH7786
#  include <sys/sysdepend/std_smp_sh7786/smp_def_depend.h>
#endif

#endif /* __SYS_SMP_DEF_COMMON_H__ */
