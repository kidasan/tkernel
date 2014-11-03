#! /usr/local/bin/perl
#
# ----------------------------------------------------------------------
#     SMP T-Kernel
#
#     Copyright (C) 2007-2010 Ken Sakamura. All Rights Reserved.
#     SMP T-Kernel is distributed under the T-License for SMP T-Kernel.
# ----------------------------------------------------------------------
#
#     Version:   1.00.01
#     Released by T-Engine Forum(http://www.t-engine.org) at 2010/02/19.
#
# ----------------------------------------------------------------------
#

#
#	makeiftd.pl
#
#	generate interface library for SH7776
#

sub makelib
{
	print LIB <<EndOfIfLibBody;
#include <machine.h>
#include <tk/sysdef.h>
#include <sys/svc/$fn_h>

	.text
	.balign	2
	.globl	Csym(${func})
	.type	Csym(${func}), \@function
Csym(${func}):
	mov.l	fno, r0
	trapa	#TRAP_DEBUG
	rts
	nop

	.balign	4
fno:	.long	TDFN_${Func}

EndOfIfLibBody
}

1;
