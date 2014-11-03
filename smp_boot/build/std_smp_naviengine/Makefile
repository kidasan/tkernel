#
#----------------------------------------------------------------------------
#    Boot Sample
#
#    Version: 1.00.00
#----------------------------------------------------------------------------
#    Description of license
#    Original Source:   Copyright (C) 2009 T-Engine Forum. 
#    This software is distributed under the T-Engine Public License.
#    Refer to T-Engine Public License at http://www.t-engine.org for use
#    conditions and redistribution conditions. 
#    Modification and redistribution are allowed to this software, in
#    accordance with T-Engine Public License. 
#    Therefore, the modification may be added to the program distributed by
#    T-Engine forum.
#    The original source is available for download from the download page at
#    http://www.t-engine.org.
#----------------------------------------------------------------------------
#


#
#	Makefile
#		for GNU make
#
#	Boot (NAVIENGINE)


# source file dependencies (generated automatically)
DEPS = Dependencies
DEPENDENCIES_OUTPUT := $(DEPS)

TETYPE = std
MACHINE = smp_naviengine

# default rules
include $(BD)/etc/makerules

# ----------------------------------------------------------------------------

# target object
TARGET = boot

S = ../../src
SD = ../../src/sysdepend/$(TETYPE)_$(MACHINE)

VPATH = $(S):$(SD)
HEADER := . $(SD) $(S) $(HEADER)

# common source files
COMSRC = svc.c console.c

# source files (system-dependent)
MSRC =	eitent_smp_naviengine.S reset.c extsvc.c sio.c

# for reset.c
CFLAGS += -fomit-frame-pointer

LNKDEF = boot_smp_naviengine.lnk

# ------------------------------

SRC = $(MSRC)  $(COMSRC)
OBJ = $(addsuffix .o, $(basename $(SRC)))

# gcc standard library
STDLIB = -lgcc

# additional library
LDLIBS +=

# linker options
LDFLAGS += -static -nostdlib -T $(SD)/$(LNKDEF)

# compiler options
CFLAGS += $(CFLAGS_WARNING)

# ----------------------------------------------------------------------------

.PHONY: all clean install

INST = $(TARGET).map $(TARGET).bin
ALL = $(TARGET) $(INST)

all: $(ALL)

$(TARGET): $(OBJ)
	$(CC) $(TARGET_ARCH) $(LDFLAGS) $^ $(LDLIBS) $(STDLIB) $(OUTPUT_OPTION)

$(TARGET).map: $(TARGET)
	$(NM) $(NMFLAGS) $< > $@

$(TARGET).bin: $(TARGET)
	$(OBJCOPY) -O binary $< $@

clean:
	$(RM) $(OBJ) $(ALL) $(DEPS)

install: $(addprefix $(EXE_INSTALLDIR)/, $(INST))


# generate dependencies
ifdef DEPENDENCIES_OUTPUT
  $(DEPS): ; touch $(DEPS)
else
  $(DEPS): $(SRC) ; $(MAKEDEPS) $@ $?
endif
include $(DEPS)
