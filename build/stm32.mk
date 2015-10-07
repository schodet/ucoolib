# ucoolib - Microcontroller object oriented library.
#
# Common rules for STM32.

ifndef stm32_once
stm32_once := 1


# Check for libopencm3.
LIBOPENCM3_PATH ?= $(UCOO_BASE)/lib/libopencm3
define stm32_libopencm3
ifneq ($$(LIBOPENCM3_PATH),)
  $1_LIBOPENCM3_LIB := $$(LIBOPENCM3_PATH)/lib/libopencm3_$(if $2,$2,$1).a
  ifeq ($$(wildcard $$($1_LIBOPENCM3_LIB)),)
    $$(error Can not find libopencm3 library, please run "make lib" in \
	    $$(LIBOPENCM3_PATH) or change LIBOPENCM3_PATH (you can set it to \
	    empty string to use system library))
  endif
  $1_LIBOPENCM3_CPPFLAGS = -I$$(LIBOPENCM3_PATH)/include
  $1_LIBOPENCM3_LDFLAGS = -L$$(LIBOPENCM3_PATH)/lib
endif
endef


# Define flags.
stm32_cortex-m3_CPU_CFLAGS = \
	-mthumb -mcpu=cortex-m3
stm32_cortex-m4_CPU_CFLAGS = \
	-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
define stm32_flags
$1_CPPFLAGS = $$(CPPFLAGS) $$($1_DEFS) \
	$$($1_STM32_DEFS) $$($1_LIBOPENCM3_CPPFLAGS)
$1_CFLAGS = $$(CFLAGS) \
	$$(stm32_$$($1_CPU)_CPU_CFLAGS) \
	-Wl,--gc-sections -ffunction-sections
$1_CXXFLAGS = $$(sort $$($1_CFLAGS) $$(CXXFLAGS))
$1_ASFLAGS = $$(ASFLAGS)
$1_LDSCRIPT ?= $(if $2,$2,$1).ld
$1_LDSCRIPT_PATH ?= $$(UCOO_BASE)/ucoo/arch/$(if $2,$2,$1)
$1_LDFLAGS = $$(LDFLAGS) -T$$($1_LDSCRIPT) -L$$($1_LDSCRIPT_PATH) \
	$$($1_LIBOPENCM3_LDFLAGS)
$1_LDLIBS = -nostartfiles $$(LDLIBS) $$($1_LIBS) \
	-lopencm3_$(if $2,$2,$1)
endef


# Define programing rules.
define stm32_program_rules

.PHONY: program.$1
ifeq ($$(words $$($1_PROGS)),1)
program.$1: $$($1_PROGS:%=%.$1.program)
endif

%.$1.program: %.$1.bin
	@echo "PROG [$1] $$<"
	$$Qst-flash write $$< 0x8000000

endef


endif # stm32_once
