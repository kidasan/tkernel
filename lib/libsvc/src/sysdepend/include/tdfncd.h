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
 *	T-Kernel/DS function code
 */

#define TDFN_LST_TSK	0x80010200
#define TDFN_LST_SEM	0x80020200
#define TDFN_LST_FLG	0x80030200
#define TDFN_LST_MBX	0x80040200
#define TDFN_LST_MTX	0x80050200
#define TDFN_LST_MBF	0x80060200
#define TDFN_LST_POR	0x80070200
#define TDFN_LST_MPF	0x80080200
#define TDFN_LST_MPL	0x80090200
#define TDFN_LST_CYC	0x800a0200
#define TDFN_LST_ALM	0x800b0200
#define TDFN_LST_SSY	0x800c0200
#define TDFN_LST_DOM	0x800d0200
#define TDFN_REF_SEM	0x800e0200
#define TDFN_REF_FLG	0x800f0200
#define TDFN_REF_MBX	0x80100200
#define TDFN_REF_MTX	0x80110200
#define TDFN_REF_MBF	0x80120200
#define TDFN_REF_POR	0x80130200
#define TDFN_REF_MPF	0x80140200
#define TDFN_REF_MPL	0x80150200
#define TDFN_REF_CYC	0x80160200
#define TDFN_REF_ALM	0x80170200
#define TDFN_REF_SSY	0x80180200
#define TDFN_REF_DOM	0x80190200
#define TDFN_REF_TSK	0x801a0200
#define TDFN_REF_TEX	0x801b0200
#define TDFN_INF_TSK	0x801c0300
#define TDFN_GET_REG	0x801d0400
#define TDFN_SET_REG	0x801e0400
#define TDFN_REF_SYS	0x801f0100
#define TDFN_GET_TIM	0x80200200
#define TDFN_GET_OTM	0x80210200
#define TDFN_RDY_QUE	0x80220300
#define TDFN_SEM_QUE	0x80230300
#define TDFN_FLG_QUE	0x80240300
#define TDFN_MBX_QUE	0x80250300
#define TDFN_MTX_QUE	0x80260300
#define TDFN_SMBF_QUE	0x80270300
#define TDFN_RMBF_QUE	0x80280300
#define TDFN_CAL_QUE	0x80290300
#define TDFN_ACP_QUE	0x802a0300
#define TDFN_MPF_QUE	0x802b0300
#define TDFN_MPL_QUE	0x802c0300
#define TDFN_HOK_SVC	0x802d0100
#define TDFN_HOK_DSP	0x802e0100
#define TDFN_HOK_INT	0x802f0100
#define TDFN_REF_DSNAME	0x80300300
#define TDFN_SET_DSNAME	0x80310300
#define TDFN_DMI_TSK	0x80320200
#define TDFN_DMI_SEM	0x80330200
#define TDFN_DMI_FLG	0x80340200
#define TDFN_DMI_MBX	0x80350200
#define TDFN_DMI_MTX	0x80360200
#define TDFN_DMI_MBF	0x80370200
#define TDFN_DMI_POR	0x80380200
#define TDFN_DMI_MPF	0x80390200
#define TDFN_DMI_MPL	0x803a0200
#define TDFN_DMI_CYC	0x803b0200
#define TDFN_DMI_ALM	0x803c0200
#define TDFN_DMI_DOM	0x803d0200
#define TDFN_FND_TSK	0x803e0200
#define TDFN_FND_SEM	0x803f0200
#define TDFN_FND_FLG	0x80400200
#define TDFN_FND_MBX	0x80410200
#define TDFN_FND_MTX	0x80420200
#define TDFN_FND_MBF	0x80430200
#define TDFN_FND_POR	0x80440200
#define TDFN_FND_MPF	0x80450200
#define TDFN_FND_MPL	0x80460200
#define TDFN_FND_CYC	0x80470200
#define TDFN_FND_ALM	0x80480200
#define TDFN_FND_DOM	0x80490200

