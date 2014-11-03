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
#	crt0 (system-dependent description)
#

# T-Engine type
MACHINE = smp_sh7786
TETYPE = std

# default rules
include $(BD)/etc/makerules

# target files
TARGET = crt0.o crt1f.o crt1s.o crti.o crtir.o crtn.o

# source files
SRC = $(TARGET:%.o=%.S)

# include main makefile (common description)
include ../../src/Makefile.common
