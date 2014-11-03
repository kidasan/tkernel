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

.macro FUNC_END
	.arm

	ldmdb	fp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, lr}
	bx	lr
.endm

	.section	.init
	FUNC_END

	.section	.fini
	FUNC_END
