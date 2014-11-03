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
 *	@(#)segment_depend.h (sys/NAVIENGINE)
 *
 *	Segment management system dependent definitions
 */

#ifndef __SYS_SEGMENT_DEPEND_H__
#define __SYS_SEGMENT_DEPEND_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Access level definitions */
typedef enum {
	MapUser		= 0x02,
	MapSystem	= 0x00,
	MapRead		= 0x00,
	MapWrite	= 0x01,
	MapExecute	= 0x04,
} MapModeLevel;

/*
 * MapMemory() attr
 */
#define MM_USER		0x20U	/* User */
#define MM_SYSTEM	0x00U	/* System */
#define MM_READ		0x00U	/* Read */
#define MM_WRITE	0x10U	/* Write */
#define MM_EXECUTE	0x01U	/* Execute */
#define MM_CDIS		0x0cU	/* Cache disabled */

#ifdef __cplusplus
}
#endif
#endif /* __SYS_SEGMENT_DEPEND_H__ */
