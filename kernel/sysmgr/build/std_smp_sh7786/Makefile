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
#	Makefile for gmake
#	T-Kernel/System Manager (system-dependent description)
#

# T-Engine type
MACHINE = smp_sh7786
TETYPE = std

# default rules
include $(BD)/etc/makerules

# source files (system-dependent)
SRC = chkplv.c power.c cache.c

# Extension source files
VPATH += ../../../extension/memory/nommu
SRC += smpmemmgr.c segmgr.c

# include main makefile (common description)
include ../../smp_src/Makefile.common
