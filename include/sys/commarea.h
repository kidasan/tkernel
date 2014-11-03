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
 *	@(#)commarea.h (sys)
 *
 *	Kernel common data 
 */

#ifndef __SYS_COMMAREA_H__
#define __SYS_COMMAREA_H__

#include <basic.h>
#include <sys/queue.h>
#include <tk/util.h>
#include <tk/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __commarea__
#define __commarea__
typedef struct CommArea	CommArea;
#endif

/*
 * Kernel common data structure 
 */
struct CommArea {
	FastLock	*PinfoLock;
	ID		SysResID;
	FUNCP		GetPinfo;
	FUNCP		GetPidToPinfo;
	FUNCP		GetLSID;
	FUNCP		GetUATB;
#if TA_GP
	VP		gp;
#endif
        INT             tev_fflock;
};

/*
 * Kernel common data reference address 
 */
IMPORT CommArea	*__CommArea;

#ifdef __cplusplus
}
#endif
#endif /* __SYS_COMMAREA_H__ */
