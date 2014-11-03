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
 *	@(#)rominfo_common.h (sys)
 *
 *	ROM information 
 */

#ifndef __SYS_ROMINFO_COMMON_H__
#define __SYS_ROMINFO_COMMON_H__

#if STD_SH7727
#  include <sys/sysdepend/std_sh7727/rominfo_depend.h>
#endif
#if STD_SH7751R
#  include <sys/sysdepend/std_sh7751r/rominfo_depend.h>
#endif
#if MIC_M32104
#  include <sys/sysdepend/mic_m32104/rominfo_depend.h>
#endif
#if STD_S1C38K
#  include <sys/sysdepend/std_s1c38k/rominfo_depend.h>
#endif
#if STD_MC9328
#  include <sys/sysdepend/std_mc9328/rominfo_depend.h>
#endif
#if MIC_VR4131
#  include <sys/sysdepend/mic_vr4131/rominfo_depend.h>
#endif
#if STD_VR5500
#  include <sys/sysdepend/std_vr5500/rominfo_depend.h>
#endif
#if STD_MB87Q1100
#  include <sys/sysdepend/std_mb87q1100/rominfo_depend.h>
#endif
#if STD_SH7760
#  include <sys/sysdepend/std_sh7760/rominfo_depend.h>
#endif
#if STD_SMP_NAVIENGINE
#  include <sys/sysdepend/std_smp_naviengine/rominfo_depend.h>
#endif
#if STD_SMP_SH7776
#  include <sys/sysdepend/std_smp_sh7776/rominfo_depend.h>
#endif
#if STD_SMP_SH7786
#  include <sys/sysdepend/std_smp_sh7786/rominfo_depend.h>
#endif

#endif /* __SYS_ROMINFO_COMMON_H__ */
