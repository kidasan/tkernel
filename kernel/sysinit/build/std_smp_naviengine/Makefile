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
#	sysinit (system-dependent description)
#

# T-Engine type
MACHINE = smp_naviengine
TETYPE = std

# default rules
include ../../../../etc/makerules

# source files (system-dependent)
SRC = devinit.c cntwus.c smpcall_drv.c smpcall_startup.S

# Extension source files
VPATH += ../../../extension/startup/noext
SRC += subsystem.c

# include main makefile (common description)
include ../../smp_src/Makefile.common
