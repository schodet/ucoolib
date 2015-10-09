# ucoolib - Microcontroller object oriented library.
#
# Setup source files according to requested products and modules configuration.

TARGETS ?= host

DEFAULT_MODULES ?= ucoo/intf ucoo/arch ucoo/arch/host ucoo/arch/host/mex

ALL_PROGS := $(PROGS) $(foreach target,$(TARGETS),$($(target)_PROGS))

ALL_MODULES := $(sort $(DEFAULT_MODULES) $(MODULES) \
	$(foreach prog,$(ALL_PROGS),$($(prog)_MODULES)))
ALL_UCOO_MODULES := $(filter ucoo/%,$(ALL_MODULES))
ALL_EXT_MODULES := $(filter-out ucoo/%,$(ALL_MODULES))

include $(ALL_UCOO_MODULES:%=$(UCOO_BASE)/%/Module)
include $(ALL_EXT_MODULES:%=$(BASE)/%/Module)

define MODULES_template
$1_SOURCES += $$(foreach module,\
	$$(DEFAULT_MODULES) $$(call defval,$1_MODULES,$$(MODULES)),\
	$$(addprefix $$(module)/,$$($$(subst /,_,$$(module))_SOURCES))\
)
endef
$(foreach prog,$(ALL_PROGS),$(eval $(call MODULES_template,$(prog))))

ALL_SOURCES := $(foreach prog,$(ALL_PROGS),$($(prog)_SOURCES))

OBJDIRS := $(sort $(dir $(ALL_SOURCES:%=$(OBJDIR)/%)))

.SECONDARY: $(COMPILE_DEPS) $(COMPILE_ORDER_DEPS)
