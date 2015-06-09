# ucoolib - Microcontroller object oriented library.
#
# Rules for Host.

host_CPPFLAGS := $(CPPFLAGS) $(host_DEFS)
host_CFLAGS := $(CFLAGS) -Wl,--gc-sections -ffunction-sections
host_CXXFLAGS := $(sort $(host_CFLAGS) $(CXXFLAGS))
host_ASFLAGS := $(ASFLAGS)
host_LDFLAGS := $(LDFLAGS)
host_LDLIBS := $(LDLIBS) $(host_LIBS)
host_RUN :=

$(eval $(call arch_cmds,host))

# Rules.

$(eval $(call arch_build_rules,host))
$(eval $(call arch_test_rules,host))
$(eval $(call arch_misc_rules,host))
