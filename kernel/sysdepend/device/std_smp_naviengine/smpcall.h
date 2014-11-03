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
 *	smpcall.h (NAVIENGINE)
 *
 *	Inter-processor communication function header
 */

#ifndef __SMPCALL_H__
#define __SMPCALL_H__

#include <basic.h>

IMPORT void smpc_lock( void );
IMPORT void smpc_unlock( void );
IMPORT void smpc_req( FP func, VP param, BOOL sync, BOOL self );
IMPORT ER smpc_regist( ID proc_id );
IMPORT ER smpc_init( void );

#endif /* __SMPCALL_H__ */
