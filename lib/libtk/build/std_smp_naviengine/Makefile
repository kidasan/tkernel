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
#	libtk (system-dependent description)
#

# T-Engine type
MACHINE = smp_naviengine
TETYPE = std

# default rules
include ../../../../etc/makerules

# source files (system-dependent)
SRC =	startup_elf.c \
	setspc.c waitusec.c waitnsec.c disint.S int.c

# compiler options
CFLAGS += -fomit-frame-pointer

# include main makefile (common description)
include ../../smp_src/Makefile.common
