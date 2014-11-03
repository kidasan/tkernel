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
 *	T-Kernel/DS system call branch table
 */

#include <machine.h>

#define _TDSC_ENTRY(name)	.int	Csym(_##name)

#define N_TDFN	73

	_TDSC_ENTRY(td_lst_tsk)
	_TDSC_ENTRY(td_lst_sem)
	_TDSC_ENTRY(td_lst_flg)
	_TDSC_ENTRY(td_lst_mbx)
	_TDSC_ENTRY(td_lst_mtx)
	_TDSC_ENTRY(td_lst_mbf)
	_TDSC_ENTRY(td_lst_por)
	_TDSC_ENTRY(td_lst_mpf)
	_TDSC_ENTRY(td_lst_mpl)
	_TDSC_ENTRY(td_lst_cyc)
	_TDSC_ENTRY(td_lst_alm)
	_TDSC_ENTRY(td_lst_ssy)
	_TDSC_ENTRY(td_lst_dom)
	_TDSC_ENTRY(td_ref_sem)
	_TDSC_ENTRY(td_ref_flg)
	_TDSC_ENTRY(td_ref_mbx)
	_TDSC_ENTRY(td_ref_mtx)
	_TDSC_ENTRY(td_ref_mbf)
	_TDSC_ENTRY(td_ref_por)
	_TDSC_ENTRY(td_ref_mpf)
	_TDSC_ENTRY(td_ref_mpl)
	_TDSC_ENTRY(td_ref_cyc)
	_TDSC_ENTRY(td_ref_alm)
	_TDSC_ENTRY(td_ref_ssy)
	_TDSC_ENTRY(td_ref_dom)
	_TDSC_ENTRY(td_ref_tsk)
	_TDSC_ENTRY(td_ref_tex)
	_TDSC_ENTRY(td_inf_tsk)
	_TDSC_ENTRY(td_get_reg)
	_TDSC_ENTRY(td_set_reg)
	_TDSC_ENTRY(td_ref_sys)
	_TDSC_ENTRY(td_get_tim)
	_TDSC_ENTRY(td_get_otm)
	_TDSC_ENTRY(td_rdy_que)
	_TDSC_ENTRY(td_sem_que)
	_TDSC_ENTRY(td_flg_que)
	_TDSC_ENTRY(td_mbx_que)
	_TDSC_ENTRY(td_mtx_que)
	_TDSC_ENTRY(td_smbf_que)
	_TDSC_ENTRY(td_rmbf_que)
	_TDSC_ENTRY(td_cal_que)
	_TDSC_ENTRY(td_acp_que)
	_TDSC_ENTRY(td_mpf_que)
	_TDSC_ENTRY(td_mpl_que)
	_TDSC_ENTRY(td_hok_svc)
	_TDSC_ENTRY(td_hok_dsp)
	_TDSC_ENTRY(td_hok_int)
	_TDSC_ENTRY(td_ref_dsname)
	_TDSC_ENTRY(td_set_dsname)
	_TDSC_ENTRY(td_dmi_tsk)
	_TDSC_ENTRY(td_dmi_sem)
	_TDSC_ENTRY(td_dmi_flg)
	_TDSC_ENTRY(td_dmi_mbx)
	_TDSC_ENTRY(td_dmi_mtx)
	_TDSC_ENTRY(td_dmi_mbf)
	_TDSC_ENTRY(td_dmi_por)
	_TDSC_ENTRY(td_dmi_mpf)
	_TDSC_ENTRY(td_dmi_mpl)
	_TDSC_ENTRY(td_dmi_cyc)
	_TDSC_ENTRY(td_dmi_alm)
	_TDSC_ENTRY(td_dmi_dom)
	_TDSC_ENTRY(td_fnd_tsk)
	_TDSC_ENTRY(td_fnd_sem)
	_TDSC_ENTRY(td_fnd_flg)
	_TDSC_ENTRY(td_fnd_mbx)
	_TDSC_ENTRY(td_fnd_mtx)
	_TDSC_ENTRY(td_fnd_mbf)
	_TDSC_ENTRY(td_fnd_por)
	_TDSC_ENTRY(td_fnd_mpf)
	_TDSC_ENTRY(td_fnd_mpl)
	_TDSC_ENTRY(td_fnd_cyc)
	_TDSC_ENTRY(td_fnd_alm)
	_TDSC_ENTRY(td_fnd_dom)
