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
 *	T-Kernel function code
 */

#define TFN_CRE_TSK	0x80010100
#define TFN_DEL_TSK	0x80020100
#define TFN_STA_TSK	0x80030200
#define TFN_EXT_TSK	0x80040000
#define TFN_EXD_TSK	0x80050000
#define TFN_TER_TSK	0x80060100
#define TFN_DIS_DSP	0x80070000
#define TFN_ENA_DSP	0x80080000
#define TFN_CHG_PRI	0x80090200
#define TFN_CHG_SLT	0x800a0200
#define TFN_ROT_RDQ	0x800b0100
#define TFN_REL_WAI	0x800c0100
#define TFN_GET_TID	0x800d0000
#define TFN_GET_TSP	0x800e0200
#define TFN_SET_TSP	0x800f0200
#define TFN_GET_RID	0x80100100
#define TFN_SET_RID	0x80110200
#define TFN_GET_REG	0x80120400
#define TFN_SET_REG	0x80130400
#define TFN_GET_CPR	0x80140300
#define TFN_SET_CPR	0x80150300
#define TFN_INF_TSK	0x80160300
#define TFN_REF_TSK	0x80170200
#define TFN_DEF_TEX	0x80180200
#define TFN_ENA_TEX	0x80190200
#define TFN_DIS_TEX	0x801a0200
#define TFN_RAS_TEX	0x801b0200
#define TFN_END_TEX	0x801c0100
#define TFN_REF_TEX	0x801d0200
#define TFN_SUS_TSK	0x801e0100
#define TFN_RSM_TSK	0x801f0100
#define TFN_FRSM_TSK	0x80200100
#define TFN_SLP_TSK	0x80210100
#define TFN_WUP_TSK	0x80220100
#define TFN_CAN_WUP	0x80230100
#define TFN_SIG_TEV	0x80240200
#define TFN_WAI_TEV	0x80250200
#define TFN_DIS_WAI	0x80260200
#define TFN_ENA_WAI	0x80270100
#define TFN_CRE_SEM	0x80280100
#define TFN_DEL_SEM	0x80290100
#define TFN_SIG_SEM	0x802a0200
#define TFN_WAI_SEM	0x802b0300
#define TFN_REF_SEM	0x802c0200
#define TFN_CRE_MTX	0x802d0100
#define TFN_DEL_MTX	0x802e0100
#define TFN_LOC_MTX	0x802f0200
#define TFN_UNL_MTX	0x80300100
#define TFN_REF_MTX	0x80310200
#define TFN_CRE_FLG	0x80320100
#define TFN_DEL_FLG	0x80330100
#define TFN_SET_FLG	0x80340200
#define TFN_CLR_FLG	0x80350200
#define TFN_WAI_FLG	0x80360500
#define TFN_REF_FLG	0x80370200
#define TFN_CRE_MBX	0x80380100
#define TFN_DEL_MBX	0x80390100
#define TFN_SND_MBX	0x803a0200
#define TFN_RCV_MBX	0x803b0300
#define TFN_REF_MBX	0x803c0200
#define TFN_CRE_MBF	0x803d0100
#define TFN_DEL_MBF	0x803e0100
#define TFN_SND_MBF	0x803f0400
#define TFN_RCV_MBF	0x80400300
#define TFN_REF_MBF	0x80410200
#define TFN_CRE_POR	0x80420100
#define TFN_DEL_POR	0x80430100
#define TFN_CAL_POR	0x80440500
#define TFN_ACP_POR	0x80450500
#define TFN_FWD_POR	0x80460500
#define TFN_RPL_RDV	0x80470300
#define TFN_REF_POR	0x80480200
#define TFN_DEF_INT	0x80490200
#define TFN_RET_INT	0x804a0000
#define TFN_CRE_MPL	0x804b0100
#define TFN_DEL_MPL	0x804c0100
#define TFN_GET_MPL	0x804d0400
#define TFN_REL_MPL	0x804e0200
#define TFN_REF_MPL	0x804f0200
#define TFN_CRE_MPF	0x80500100
#define TFN_DEL_MPF	0x80510100
#define TFN_GET_MPF	0x80520300
#define TFN_REL_MPF	0x80530200
#define TFN_REF_MPF	0x80540200
#define TFN_SET_TIM	0x80550100
#define TFN_GET_TIM	0x80560100
#define TFN_GET_OTM	0x80570100
#define TFN_DLY_TSK	0x80580100
#define TFN_CRE_CYC	0x80590100
#define TFN_DEL_CYC	0x805a0100
#define TFN_STA_CYC	0x805b0100
#define TFN_STP_CYC	0x805c0100
#define TFN_REF_CYC	0x805d0200
#define TFN_CRE_ALM	0x805e0100
#define TFN_DEL_ALM	0x805f0100
#define TFN_STA_ALM	0x80600200
#define TFN_STP_ALM	0x80610100
#define TFN_REF_ALM	0x80620200
#define TFN_REF_VER	0x80630100
#define TFN_REF_SYS	0x80640100
#define TFN_DEF_SSY	0x80650200
#define TFN_STA_SSY	0x80660300
#define TFN_CLN_SSY	0x80670300
#define TFN_EVT_SSY	0x80680400
#define TFN_REF_SSY	0x80690200
#define TFN_CRE_RES	0x806a0000
#define TFN_DEL_RES	0x806b0100
#define TFN_GET_RES	0x806c0300
#define TFN_SET_POW	0x806d0100
#define TFN_CRE_DOM	0x806e0100
#define TFN_DEL_DOM	0x806f0100
#define TFN_FDEL_DOM	0x80700100
#define TFN_FND_DOM	0x80710200
#define TFN_REF_DOM	0x80720200
#define TFN_GET_KDM	0x80730100
#define TFN_DMI_DOM	0x80740200
#define TFN_FND_TSK	0x80750200
#define TFN_DMI_TSK	0x80760200
#define TFN_FND_SEM	0x80770200
#define TFN_DMI_SEM	0x80780200
#define TFN_FND_FLG	0x80790200
#define TFN_DMI_FLG	0x807a0200
#define TFN_FND_MBX	0x807b0200
#define TFN_DMI_MBX	0x807c0200
#define TFN_FND_MTX	0x807d0200
#define TFN_DMI_MTX	0x807e0200
#define TFN_FND_MBF	0x807f0200
#define TFN_DMI_MBF	0x80800200
#define TFN_FND_POR	0x80810200
#define TFN_DMI_POR	0x80820200
#define TFN_FND_MPF	0x80830200
#define TFN_DMI_MPF	0x80840200
#define TFN_FND_MPL	0x80850200
#define TFN_DMI_MPL	0x80860200
#define TFN_FND_ALM	0x80870200
#define TFN_DMI_ALM	0x80880200
#define TFN_FND_CYC	0x80890200
#define TFN_DMI_CYC	0x808a0200
#define TFN_GET_PRC	0x808b0000
