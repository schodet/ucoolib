# ucoolib - Microcontroller object oriented library.
#
# Arch specific makefiles can use macros in this file to factorize build
# system code.


# Define standard commands.
define arch_cmds
$1_TARGET_DEFS = -DTARGET_$1=1 $$($1_SUBTARGETS:%=-DTARGET_%=1)
$1_CPPFLAGS += $$($1_TARGET_DEFS)
$1_CC = $$($1_PREFIX)gcc
$1_CXX = $$($1_PREFIX)g++
$1_OBJCOPY = $$($1_PREFIX)objcopy
$1_OBJDUMP = $$($1_PREFIX)objdump
$1_SIZE = $$($1_PREFIX)size
$1_COMPILE.c = $$($1_CC) $$($1_CFLAGS) $$($1_CPPFLAGS) -c
$1_COMPILE.cc = $$($1_CXX) $$($1_CXXFLAGS) $$($1_CPPFLAGS) -c
$1_COMPILE.S = $$($1_CC) $$($1_ASFLAGS) $$($1_CPPFLAGS) -c
$1_LINK.c = $$($1_CC) $$($1_CFLAGS) $$($1_LDFLAGS)
$1_LINK.cc = $$($1_CXX) $$($1_CXXFLAGS) $$($1_LDFLAGS)
endef


# Define build rules.
define arch_build_rules

$1_PROGS += $$(PROGS)
$1_ELFS := $$($1_PROGS:%=%.$1$$($1_ELF_SUFFIX))
$1_EXTRA_CLEAN :=

all: all.$1
elf: elf.$1

.PHONY: all.$1 elf.$1

all.$1: elf.$1

elf.$1: $$($1_ELFS)

define $1_PROG_template
$1_$$1_LINKFLAGS = $$$$($1_CFLAGS) $$$$($1_CXXFLAGS) $$$$($1_LDFLAGS)
$$(call cmddep,$1_$$1_LINKFLAGS,$$$$(OBJDIR)/$$1.$1.linkflags)
$1_$$1_OBJECTS = $$$$(patsubst %,$$(OBJDIR)/%.$1.o,\
	$$$$(call filter_sources,$1,$$$$($$1_SOURCES)))
$$1.$1$$$$($1_ELF_SUFFIX): $$$$($1_$$1_OBJECTS) $$$$(OBJDIR)/$$1.$1.linkflags
	@echo "LINK [$1] $$$$@"
	$$$$Q$$$$($1_LINK.$$(call iscxx,$$($$1_SOURCES))) $$$$($1_$$1_OBJECTS) \
		$$$$($1_LDLIBS) -o $$$$@
endef
$$(foreach prog,$$($1_PROGS),$$(eval $$(call $1_PROG_template,$$(prog))))

$$(OBJDIR)/%.$1.o: %.c $$(COMPILE_DEPS) | $$(COMPILE_ORDER_DEPS)
	@echo "CC   [$1] $$<"
	$$(call mkdir_if_needed,$$@)
	$$Q$$($1_COMPILE.c) -o $$@ $$<

$$(OBJDIR)/%.$1.o: %.cc $$(COMPILE_DEPS) | $$(COMPILE_ORDER_DEPS)
	@echo "CXX  [$1] $$<"
	$$(call mkdir_if_needed,$$@)
	$$Q$$($1_COMPILE.cc) -o $$@ $$<

$$(OBJDIR)/%.$1.o: %.S $$(COMPILE_DEPS) | $$(COMPILE_ORDER_DEPS)
	@echo "AS   [$1] $$<"
	$$(call mkdir_if_needed,$$@)
	$$Q$$($1_COMPILE.S) -o $$@ $$<

# Dependency checking.
-include $$(patsubst %,$$(OBJDIR)/%.$1.d,$$(call filter_sources,$1,$$(ALL_SOURCES)))

endef


# Define listing rules.
define arch_lst_rules

lst: lst.$1
all.$1: lst.$1

.PHONY: lst.$1

lst.$1: $$($1_PROGS:%=%.$1.lst)

%.$1.lst: %.$1.elf
	@echo "LST  [$1] $$@"
	$$Q$$($1_OBJDUMP) -h -S $$< > $$@

$1_EXTRA_CLEAN += $$($1_PROGS:%=%.$1.lst)

endef


# Define binary rules.
define arch_bin_rules

hex: hex.$1
srec: srec.$1
bin: bin.$1
crc: crc.$1
all.$1: hex.$1

ifneq ($$(wildcard *.$1.srec),)
all.$1: srec.$1
endif
ifneq ($$(wildcard *.$1.bin),)
all.$1: bin.$1 crc.$1
endif

.PHONY: hex.$1 srec.$1 bin.$1 crc.$1

hex.$1: $$($1_PROGS:%=%.$1.hex)
srec.$1: $$($1_PROGS:%=%.$1.srec)
bin.$1: $$($1_PROGS:%=%.$1.bin)

%.$1.hex: %.$1$$($1_ELF_SUFFIX)
	@echo "HEX  [$1] $$@"
	$$Q$$($1_OBJCOPY) -O ihex $$< $$@
%.$1.srec: %.$1$$($1_ELF_SUFFIX)
	@echo "SREC [$1] $$@"
	$$Q$$($1_OBJCOPY) -O srec $$< $$@
%.$1.bin: %.$1$$($1_ELF_SUFFIX)
	@echo "BIN  [$1] $$@"
	$$Q$$($1_OBJCOPY) -O binary $$< $$@

crc.$1: $$($1_PROGS:%=%.$1.bin)
	$$Q$$(UCOO_BASE)/build/tools/crc $$^

$1_EXTRA_CLEAN += $$($1_PROGS:%=%.$1.hex) \
	$$($1_PROGS:%=%.$1.srec) \
	$$($1_PROGS:%=%.$1.bin)

endef


# Define size report rules.
define arch_size_rules

size: size.$1
all.$1: size.$1

.PHONY: size.$1

size.$1: $$($1_ELFS)
	$Q$$($1_SIZE) $$^

endef


# Define test rules.
define arch_test_rules

.PHONY: test.$1

test: test.$1

ifeq ($$(origin $1_RUN),undefined)
test.$1: elf.$1
else
test.$1: $$($1_PROGS:%=%.$1.test)

%.$1.test: %.$1$$($1_ELF_SUFFIX)
	@echo "TEST [$1] $$<"
	$$Q$$($1_RUN) ./$$<
endif

endef


# Define miscellaneous rules.
define arch_misc_rules

.PHONY: clean.$1

clean: clean.$1

clean.$1:
	@echo rm -f '$$(OBJDIR)/.../*.$1.[od]' '$$(OBJDIR)/*.$1.linkflags' \
		$$($1_ELFS) $$($1_EXTRA_CLEAN)
	$$Qrm -f $$(patsubst %,$$(OBJDIR)/%.$1.[od],\
		$$(call filter_sources,$1,$$(ALL_SOURCES))) \
		$$($1_PROGS:%=$(OBJDIR)/%.$1.linkflags) \
		$$($1_ELFS) $$($1_EXTRA_CLEAN)

endef
