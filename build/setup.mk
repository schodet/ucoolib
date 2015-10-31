# ucoolib - Microcontroller object oriented library.
#
# Setup source files according to requested products and modules configuration.

TARGETS ?= host

DEFAULT_MODULES ?= ucoo/intf ucoo/arch ucoo/arch/host ucoo/arch/host/mex

ALL_PROGS := $(PROGS) $(foreach target,$(TARGETS),$($(target)_PROGS))

ALL_MODULES :=
NEW_MODULES := $(sort $(DEFAULT_MODULES) $(MODULES) \
	$(call foreachsub,$(ALL_PROGS),MODULES))

define MODULES_deps_template
ALL_MODULES += $$(NEW_MODULES)
NEW_UCOO_MODULES := $$(filter ucoo/%,$$(NEW_MODULES))
NEW_EXT_MODULES := $$(filter-out ucoo/%,$$(NEW_MODULES))
include $$(NEW_UCOO_MODULES:%=$$(UCOO_BASE)/%/Module)
include $$(NEW_EXT_MODULES:%=$$(BASE)/%/Module)
NEW_MODULES := $$(filter-out $$(ALL_MODULES),\
	$$(call foreachsub,$$(ALL_MODULES),MODULES))
MODULES := $$(sort $$(MODULES) $$(call foreachsub,$$(MODULES),MODULES))
$$(foreach prog,$$(ALL_PROGS),$$(eval $$(call ifdefined $$(prog)_MODULES,\
$$(prog)_MODULES := $$(sort $$($$(prog)_MODULES) \
	$$(call foreachsub,$$($$(prog)_MODULES),MODULES)))))
$$(if $$(NEW_MODULES),$$(eval $$(MODULES_deps_template)))
endef
$(eval $(MODULES_deps_template))

ALL_UCOO_MODULES := $(filter ucoo/%,$(ALL_MODULES))
ALL_EXT_MODULES := $(filter-out ucoo/%,$(ALL_MODULES))

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
