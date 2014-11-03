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
 *	ready_queue.c (T-Kernel/OS)
 *	Ready Queue Operation Routine
 */

#include "kernel.h"
#include "task.h"
#include "wait.h"
#include "ready_queue.h"

Inline TCB *get_next_tcb( RDYQUE *rq, TCB *tcb );
IMPORT void single_core_assign(TCB *tcb[], TCB *top_tcb[]);
IMPORT void multi_core_assign( TCB *tcb[], TCB *top_tcb[], UW *top_bit, UW *bit_cnt, INT assign_num );

EXPORT UW	hist[1 << (MAX_PROC - 1)];

LOCAL const UW cnt_bits_table[] = {
	0,	/* 0000 */	1,	/* 0001 */	1,	/* 0010 */	2,	/* 0011 */
	1,	/* 0100 */	2,	/* 0101 */	2,	/* 0110 */	3,	/* 0111 */
	1,	/* 1000 */	2,	/* 1001 */	2,	/* 1010 */	3,	/* 1011 */
	2,	/* 1100 */	3,	/* 1101 */	3,	/* 1110 */	4,	/* 1111 */
};

LOCAL UW cnt_bits(UW bits)
{
	UW cnt = 0;
	INT i;

	for (i = 0; i < num_proc; i += 4, bits>>=4) {
		cnt += cnt_bits_table[bits&0xF];
	}
	return cnt;
}

LOCAL UW cnt_bits2(UW bits)
{
	UW cnt = 0;
	INT i;

	for (i = 0; i < num_proc; i++) {
		if ( bits & (1<<i) ) {
			cnt++;
		}
	}
	return cnt;
}

LOCAL void unsum_bits(UW *cnt, UW bit)
{
	INT k;
	for ( k = 0; k < num_proc; k++ ) {
		if ( bit & (1<<k) ) {
			cnt[k]--;
		}
	}
}

LOCAL void sum_bits(UW *cnt, UW bit)
{
	INT k;
	for ( k = 0; k < num_proc; k++ ) {
		if ( bit & (1<<k) ) {
			cnt[k]++;
		}
	}
}

LOCAL void drop_bit(UW *bits, UW no)
{
	INT k;
	for ( k = 0; k < num_proc; k++ ) {
		bits[k] &= ~(1<<no);
	}
}

LOCAL INT try_assign(UW *top_bit, TCB *top_tcb[], TCB *sch[], UW *bit_cnt, INT idx, INT pr, TCB *cmptcb)
{
	if ( (top_bit[idx] & (1<<pr)) && (sch[pr] == NULL) && (cmptcb == top_tcb[idx]) ) {
		unsum_bits(bit_cnt, top_bit[idx]);
		drop_bit(top_bit, pr);
		sch[pr] = top_tcb[idx];
		top_bit[idx] = 0;
		return 1;
	}
	return 0;
}

EXPORT void ready_queue_top( RDYQUE *rq, TCB *tcb[] )
{
	INT	i, j, k;
	TCB	*top_tcb[MAX_PROC];
	UW	top_bit[MAX_PROC];
	UW	bit_cnt[MAX_PROC];
	UW	dds_mask;
	INT	hist_num;
	UW	bit = 0;
	UW 	or_bit_num;
	INT	assign_num = 0;

	/* Initialzie first */
	for ( i = 0; i < num_proc; i++ ) {
		top_tcb[i] = rq->top[i];
		top_bit[i] = 0;
		bit_cnt[i] = 0;
	}

	/* make DDS_DISABLE_MASK */
	dds_mask = 0;
	for ( j = 0; j < num_proc; j++ ) {
		if ( dispatch_disabled[j] >= DDS_DISABLE ) {
			dds_mask |= (1<<j);
		}
	}
	dds_mask = ~dds_mask;

	/* get top[] */
	hist_num = 0;
	for ( j = 0; j < num_proc; j++ ) {
RETRY:
		if ( top_tcb[j] == NULL ) break;

		/* care for DDS_DISABLE */
		for ( i = 0; i < num_proc; i++ ) {
			if ( (top_tcb[j] == ctxtsk[i]) && (dispatch_disabled[i] >= DDS_DISABLE) ) {
				bit = (1<<i);		/* If dispatch disabled, it is not possible to move. */
				break;
			}
		}
		if ( i == num_proc ) {
			if ( (top_tcb[j]->tskatr & TA_ASSPRC) == 0 ) {
				bit = ALL_CORE_BIT;	/* if not specified processor, specified all processor */
			}
			else{
				bit = top_tcb[j]->assprc;
			}
			bit &= dds_mask;		/* The core prohibiting dispatching is excluded. */
			if ( bit == 0 ) {
				/* shift top[] */
				for ( k = j; k < num_proc-1; k++ ) {
					top_tcb[k] = top_tcb[k+1];
				}
				if ( top_tcb[k] != NULL ) {
					top_tcb[k] = get_next_tcb(rq, top_tcb[k]);
				}
				goto RETRY;
			}
		}

		if (bit != ALL_CORE_BIT) {
			/* check routine */
			for ( i = 0; i < hist_num; i++ ) {
				or_bit_num  = hist[i];
				or_bit_num |= bit;	/* lower 16bit is bit */
				or_bit_num += (1<<16);	/* upper 16bit is num */
				/* check assign */
				if ( cnt_bits(or_bit_num&0xFFFF) < (or_bit_num>>16) ) {
					/* shift top[] */
					for ( k = j; k < num_proc-1; k++ ) {
						top_tcb[k] = top_tcb[k+1];
					}
					if ( top_tcb[k] != NULL ) {
						top_tcb[k] = get_next_tcb(rq, top_tcb[k]);
					}
					goto RETRY;
				}
				/* not store at last loop */
				if ( j != (num_proc-1) ) {
					hist[hist_num+i] = or_bit_num;
				}
			}
			/* not store at last loop */
			if ( j != (num_proc-1) ) {
				hist[hist_num+i] = bit|(1<<16);
				hist_num += (i+1);
			}
		}
		/* save bit */
		top_bit[j] = bit;
		/* count bit */
		sum_bits(bit_cnt, bit);
		assign_num++;
	}

	for ( j = 0; j < assign_num; j++ ) {
		k = cnt_bits2(top_tcb[j]->assprc);
		if(k > 1 && k < num_proc){
			break;
		}
	}

	if(j < assign_num){
		multi_core_assign(tcb, top_tcb, top_bit, bit_cnt, assign_num);
	}
	else{
		single_core_assign(tcb, top_tcb);
	}

	rq->chg = FALSE;
}

EXPORT void single_core_assign(TCB *tcb[], TCB *top_tcb[])
{
	INT	i, j;
	TCB	*sch[MAX_PROC];

	/* Initialzie first */
	for ( i = 0; i < num_proc; i++ ) {
		sch[i] = NULL;
	}

	/* assign TA_ASSPROC attribute task */
	for ( j = 0; j < num_proc; j++ ) {
		if ( top_tcb[j] == NULL ) break;
		if ( (top_tcb[j]->tskatr & TA_ASSPRC) == 0 || top_tcb[j]->assprc == ALL_CORE_BIT) continue;
		for (i = 0; i < num_proc; i++) {
			if ( top_tcb[j]->assprc & (1<<i) ) {
				if ( sch[i] == NULL ) {
					sch[i] = top_tcb[j];
					top_tcb[j] = NULL;
					break;
				}
			}
		}
	}

	/* assign schedule task same as ctxtsk */
	for ( j = 0; j < num_proc; j++ ) {
		if ( top_tcb[j] == NULL ) continue;
		for ( i = 0; i < num_proc; i++ ) {
			if ( (sch[i] == NULL) && (ctxtsk[i] == top_tcb[j]) ) {
				sch[i] = top_tcb[j];
				top_tcb[j] = NULL;
				break;
			}
		}
	}

	/* assign rest */
	for ( j = 0; j < num_proc; j++ ) {
		if ( top_tcb[j] == NULL ) continue;
		for ( i = 0; i < num_proc; i++ ) {
			if ( dispatch_disabled[i] >= DDS_DISABLE ) continue;
			if ( sch[i] == NULL ) {
				sch[i] = top_tcb[j];
				top_tcb[j] = NULL;
				break;
			}
		}
	}

	/* copy */
	for ( i = 0; i < num_proc; i++ ) {
		tcb[i] = sch[i];
	}
}

EXPORT void multi_core_assign( TCB *tcb[], TCB *top_tcb[], UW *top_bit, UW *bit_cnt, INT assign_num )
{
	INT	i, j, c;
	TCB	*sch[MAX_PROC];

	/* Initialzie first */
	for ( i = 0; i < num_proc; i++ ) {
		sch[i] = NULL;
	}

	/* assign */
	/* search count 1,2,...num_proc */
RE_ASSIGN:
	for ( c = 1; c <= num_proc; c++ ) {
		/* assign same as ctxtsk */
		for ( j = 0; j < num_proc; j++ ) {
			/* try column */
			if ( bit_cnt[j] == c ) {
				for ( i = 0; i < num_proc; i++ ) {
					if ( try_assign(top_bit, top_tcb, sch, bit_cnt, i, j, ctxtsk[j]) ) {
						if (--assign_num == 0) goto EXIT;
						goto RE_ASSIGN;
					}
				}
			}
			/* try row */
			if ( cnt_bits(top_bit[j]) == c ) {
				for ( i = 0; i < num_proc; i++ ) {
					if ( try_assign(top_bit, top_tcb, sch, bit_cnt, j, i, ctxtsk[i]) ) {
						if (--assign_num == 0) goto EXIT;
						goto RE_ASSIGN;
					}
				}
			}
		}
		/* assign rest */
		for ( j = 0; j < num_proc; j++ ) {
			/* try column */
			if ( bit_cnt[j] == c ) {
				for ( i = 0; i < num_proc; i++ ) {
					if ( try_assign(top_bit, top_tcb, sch, bit_cnt, i, j, top_tcb[i]) ) {
						if (--assign_num == 0) goto EXIT;
						goto RE_ASSIGN;
					}
				}
			}
			/* try row */
			if ( cnt_bits(top_bit[j]) == c ) {
				for ( i = 0; i < num_proc; i++ ) {
					if ( try_assign(top_bit, top_tcb, sch, bit_cnt, j, i, top_tcb[j]) ) {
						if (--assign_num == 0) goto EXIT;
						goto RE_ASSIGN;
					}
				}
			}
		}
	}
EXIT:
	/* copy */
	for ( i = 0; i < num_proc; i++ ) {
		tcb[i] = sch[i];
	}
}
