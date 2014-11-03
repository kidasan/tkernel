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
 *	@(#)machine_common.h (sys)
 *
 *	Machine type definition
 */

#ifndef __SYS_MACHINE_COMMON_H__
#define __SYS_MACHINE_COMMON_H__

#ifdef _STD_SH7727_
#  include <sys/sysdepend/std_sh7727/machine_depend.h>
#endif
#ifdef _STD_SH7751R_
#  include <sys/sysdepend/std_sh7751r/machine_depend.h>
#endif
#ifdef _MIC_M32104_
#  include <sys/sysdepend/mic_m32104/machine_depend.h>
#endif
#ifdef _STD_S1C38K_
#  include <sys/sysdepend/std_s1c38k/machine_depend.h>
#endif
#ifdef _STD_MC9328_
#  include <sys/sysdepend/std_mc9328/machine_depend.h>
#endif
#ifdef _MIC_VR4131_
#  include <sys/sysdepend/mic_vr4131/machine_depend.h>
#endif
#ifdef _STD_VR5500_
#  include <sys/sysdepend/std_vr5500/machine_depend.h>
#endif
#ifdef _STD_MB87Q1100_
#  include <sys/sysdepend/std_mb87q1100/machine_depend.h>
#endif
#ifdef _STD_SH7760_
#  include <sys/sysdepend/std_sh7760/machine_depend.h>
#endif
#ifdef _STD_SMP_NAVIENGINE_
#  include <sys/sysdepend/std_smp_naviengine/machine_depend.h>
#endif
#ifdef _STD_SMP_SH7776_
#  include <sys/sysdepend/std_smp_sh7776/machine_depend.h>
#endif
#ifdef _STD_SMP_SH7786_
#  include <sys/sysdepend/std_smp_sh7786/machine_depend.h>
#endif

#endif /* __SYS_MACHINE_COMMON_H__ */
