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
 *	Extended SVC function code
 */

#include <sys/ssid.h>

#define SYSTEM_TK_GET_CFN_FN	(0x00010300 | SYSTEM_SVC)
#define SYSTEM_TK_GET_CFS_FN	(0x00020300 | SYSTEM_SVC)
#define SYSTEM_TK_GET_SMB_FN	(0x01000300 | SYSTEM_SVC)
#define SYSTEM_TK_REL_SMB_FN	(0x01010100 | SYSTEM_SVC)
#define SYSTEM_TK_REF_SMB_FN	(0x01020100 | SYSTEM_SVC)
#define SYSTEM__GETKERNELCOMMONAREA_FN	(0x10000100 | SYSTEM_SVC)
#define SYSTEM__SYSLOG_SEND_FN	(0x10010200 | SYSTEM_SVC)

