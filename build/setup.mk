# ucoolib - Microcontroller object oriented library.
#
# Setup source files according to requested products and modules configuration.

TARGETS ?= host

DEFAULT_MODULES ?= intf arch arch/host/mex

ALL_PROGS := $(PROGS) $(foreach target,$(TARGETS),$($(target)_PROGS))

ALL_MODULES := $(sort $(DEFAULT_MODULES) $(MODULES) \
	$(foreach prog,$(ALL_PROGS),$($(prog)_MODULES)))

include $(ALL_MODULES:%=$(BASE)/ucoolib/%/Module)

define MODULES_template
$1_SOURCES += $$(foreach module,\
	$$(DEFAULT_MODULES) $$(call defval,$1_MODULES,$$(MODULES)),\
	$$($$(subst /,_,$$(module))_SOURCES)\
)
endef
$(foreach prog,$(ALL_PROGS),$(eval $(call MODULES_template,$(prog))))

ALL_SOURCES := $(foreach prog,$(ALL_PROGS),$($(prog)_SOURCES))

COMPILE_DEPS :=
