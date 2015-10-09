# ucoolib - Microcontroller object oriented library.
#
# Build system top file. To be included with $(BASE) pointing to the project
# root.

ifeq ($(BASE),)
$(error BASE is not defined)
endif

UCOO_BASE := $(patsubst %/build/top.mk,%,$(lastword $(MAKEFILE_LIST)))

include $(UCOO_BASE)/build/macros.mk

# General parameters.

OBJDIR := obj

INCLUDES := $(INCLUDES) -I$(UCOO_BASE) -I$(BASE) -I$(OBJDIR)
CPPFLAGS = $(DEFS) $(INCLUDES) -MP -MMD
OPTIMIZE ?= -Os
CFLAGS = -g3 -Wall -W -Wundef -Wno-unused-parameter \
	-fno-exceptions $(OPTIMIZE)
CXXFLAGS = $(CFLAGS) -fno-rtti -fno-threadsafe-statics -std=gnu++11 -Wno-deprecated-declarations
LDFLAGS =
LDLIBS = $(LIBS)

# Quiet, unset to see executed lines.
Q = @

# Main rules.

all:
lst:
size:
hex:
srec:
bin:
crc:
test:

clean:
	$(if $(EXTRA_CLEAN),rm -f $(EXTRA_CLEAN))
	$(if $(EXTRA_CLEAN_DIRS),@echo rmdir $(EXTRA_CLEAN_DIRS))
	$(if $(EXTRA_CLEAN_DIRS),$(call rmdir_as_needed,$(EXTRA_CLEAN_DIRS),$(OBJDIR)))
	@echo rmdir '$(OBJDIR)/.../'
	$(call rmdir_as_needed,$(OBJDIRS),$(OBJDIR))

.PHONY: all lst size hex srec bin crc test clean

# Modules and sources setup.

include $(UCOO_BASE)/build/setup.mk

vpath %.cc $(UCOO_BASE) $(BASE)
vpath %.c $(UCOO_BASE) $(BASE)
vpath %.S $(UCOO_BASE) $(BASE)

# Configuration.

include $(UCOO_BASE)/build/config.mk

# Objects directory.

$(OBJDIR):
	$Qmkdir -p $@

# Arch specific.

include $(UCOO_BASE)/build/arch.mk
define TARGETS_template
include $$(firstword $$(wildcard $$(BASE)/build/$1.mk) $$(UCOO_BASE)/build/$1.mk)
endef
$(foreach target,$(TARGETS),$(eval $(call TARGETS_template,$(target))))

TARGETS_SUBTARGETS := $(foreach target,$(TARGETS),$(target):$(target) \
	$(foreach subtarget,$($(target)_SUBTARGETS),$(target):$(subtarget)))

.PHONY: program
ifeq ($(words $(filter-out host,$(TARGETS))),1)
program: $(patsubst %,program.%,$(filter-out host,$(TARGETS)))
endif
