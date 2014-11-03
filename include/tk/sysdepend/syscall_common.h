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
 *	@(#)syscall_common.h (tk)
 *
 *	T-Kernel/OS
 */

#ifndef __TK_SYSCALL_COMMON_H__
#define __TK_SYSCALL_COMMON_H__

#if STD_SH7727
#  include <tk/sysdepend/std_sh7727/cpuattr.h>
#  include <tk/sysdepend/std_sh7727/cpudef.h>
#endif
#if STD_SH7751R
#  include <tk/sysdepend/std_sh7751r/cpuattr.h>
#  include <tk/sysdepend/std_sh7751r/cpudef.h>
#endif
#if MIC_M32104
#  include <tk/sysdepend/mic_m32104/cpuattr.h>
#  include <tk/sysdepend/mic_m32104/cpudef.h>
#endif
#if STD_S1C38K
#  include <tk/sysdepend/std_s1c38k/cpuattr.h>
#  include <tk/sysdepend/std_s1c38k/cpudef.h>
#endif
#if STD_MC9328
#  include <tk/sysdepend/std_mc9328/cpuattr.h>
#  include <tk/sysdepend/std_mc9328/cpudef.h>
#endif
#if MIC_VR4131
#  include <tk/sysdepend/mic_vr4131/cpuattr.h>
#  include <tk/sysdepend/mic_vr4131/cpudef.h>
#endif
#if STD_VR5500
#  include <tk/sysdepend/std_vr5500/cpuattr.h>
#  include <tk/sysdepend/std_vr5500/cpudef.h>
#endif
#if STD_MB87Q1100
#  include <tk/sysdepend/std_mb87q1100/cpuattr.h>
#  include <tk/sysdepend/std_mb87q1100/cpudef.h>
#endif
#if STD_SH7760
#  include <tk/sysdepend/std_sh7760/cpuattr.h>
#  include <tk/sysdepend/std_sh7760/cpudef.h>
#endif
#if STD_SMP_NAVIENGINE
#  include <tk/sysdepend/std_smp_naviengine/cpuattr.h>
#  include <tk/sysdepend/std_smp_naviengine/cpudef.h>
#endif
#if STD_SMP_SH7776
#  include <tk/sysdepend/std_smp_sh7776/cpuattr.h>
#  include <tk/sysdepend/std_smp_sh7776/cpudef.h>
#endif
#if STD_SMP_SH7786
#  include <tk/sysdepend/std_smp_sh7786/cpuattr.h>
#  include <tk/sysdepend/std_smp_sh7786/cpudef.h>
#endif

#endif /* __TK_SYSCALL_COMMON_H__ */
