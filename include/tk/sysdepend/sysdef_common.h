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
 *	@(#)sysdef_common.h (tk)
 *
 *	System dependencies definition.
 *	Included also from assembler program.
 */

#ifndef __TK_SYSDEF_COMMON_H__
#define __TK_SYSDEF_COMMON_H__

#if STD_SH7727
#  include <tk/sysdepend/std_sh7727/sysdef_depend.h>
#endif
#if STD_SH7751R
#  include <tk/sysdepend/std_sh7751r/sysdef_depend.h>
#endif
#if MIC_M32104
#  include <tk/sysdepend/mic_m32104/sysdef_depend.h>
#endif
#if STD_S1C38K
#  include <tk/sysdepend/std_s1c38k/sysdef_depend.h>
#endif
#if STD_MC9328
#  include <tk/sysdepend/std_mc9328/sysdef_depend.h>
#endif
#if MIC_VR4131
#  include <tk/sysdepend/mic_vr4131/sysdef_depend.h>
#endif
#if STD_VR5500
#  include <tk/sysdepend/std_vr5500/sysdef_depend.h>
#endif
#if STD_MB87Q1100
#  include <tk/sysdepend/std_mb87q1100/sysdef_depend.h>
#endif
#if STD_SH7760
#  include <tk/sysdepend/std_sh7760/sysdef_depend.h>
#endif
#if STD_SMP_NAVIENGINE
#  include <tk/sysdepend/std_smp_naviengine/sysdef_depend.h>
#endif
#if STD_SMP_SH7776
#  include <tk/sysdepend/std_smp_sh7776/sysdef_depend.h>
#endif
#if STD_SMP_SH7786
#  include <tk/sysdepend/std_smp_sh7786/sysdef_depend.h>
#endif

#endif /* __TK_SYSDEF_COMMON_H__ */
