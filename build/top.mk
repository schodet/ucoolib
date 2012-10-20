# ucoolib - Microcontroller object oriented library.
#
# Build system top file. To be included with $(BASE) pointing to the ucoolib
# root.

ifeq ($(BASE),)
$(error BASE is not defined)
endif

include $(BASE)/build/macros.mk

# General parameters.

OBJDIR := obj

INCLUDES := $(INCLUDES) -I$(BASE) -I$(OBJDIR)
CPPFLAGS := $(DEFS) $(INCLUDES) -MP -MMD
OPTIMIZE ?= -Os
CFLAGS := -g -Wall -W -Wundef -Wno-unused-parameter \
	-fno-exceptions $(OPTIMIZE)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-threadsafe-statics -fno-implicit-templates
LDFLAGS :=
LDLIBS := $(LIBS)

# Quiet, unset to see executed lines.
Q = @

# Main rules.

all:
lst:
size:

clean:
	rmdir $(OBJDIR) 2> /dev/null || true

.PHONY: all lst size clean

# Modules and sources setup.

include $(BASE)/build/setup.mk

vpath %.cc $(ALL_MODULES:%=$(BASE)/ucoolib/%)
vpath %.c $(ALL_MODULES:%=$(BASE)/ucoolib/%)
vpath %.S $(ALL_MODULES:%=$(BASE)/ucoolib/%)

# Configuration.

include $(BASE)/build/config.mk

# Objects directory.

$(OBJDIR):
	$Qmkdir -p $(OBJDIR)

# Arch specific.

include $(BASE)/build/arch.mk
define TARGETS_template
include $$(BASE)/build/$1.mk
endef
$(foreach target,$(TARGETS),$(eval $(call TARGETS_template,$(target))))
