# ucoolib - Microcontroller object oriented library.
#
# Common rules for STM32.

ifndef stm32_once
stm32_once := 1


# Check for cmsis.
CMSIS_PATH ?= $(UCOO_BASE)/lib/cmsis
define stm32_cmsis
$1_CMSIS_LIB := $$(CMSIS_PATH)/Include/cmsis_gcc.h
ifeq ($$(wildcard $$($1_CMSIS_LIB)),)
  $$(error Can not find CMSIS library, please put it in $$(CMSIS_PATH) or \
	  change CMSIS_PATH)
endif
$1_CMSIS_CPPFLAGS = -I$$(CMSIS_PATH)/Include -I$$(CMSIS_PATH)/Device
endef


# Define flags.
stm32_cortex-m3_CPU_CFLAGS = \
	-mthumb -mcpu=cortex-m3
stm32_cortex-m4_CPU_CFLAGS = \
	-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
define stm32_flags
$1_CPPFLAGS = $$(CPPFLAGS) $$($1_DEFS) \
	$$($1_STM32_DEFS) $$($1_CMSIS_CPPFLAGS)
$1_CFLAGS = $$(CFLAGS) \
	$$(stm32_$$($1_CPU)_CPU_CFLAGS) \
	-Wl,--gc-sections -ffunction-sections
$1_CXXFLAGS = $$(sort $$($1_CFLAGS) $$(CXXFLAGS))
$1_ASFLAGS = $$(ASFLAGS)
$1_LDSCRIPT ?= $(if $2,$2,$1).ld
$1_LDSCRIPT_PATH ?= $$(UCOO_BASE)/ucoo/arch/ld
$1_START_ADDRESS ?= $$(if $$(filter y,$$($1_BOOTLOADED)),0x8010000)
$1_SECTION_START = -Wl,--section-start=.text=$$($1_START_ADDRESS)
$1_LDFLAGS = $$(LDFLAGS) -T$$($1_LDSCRIPT) -L$$($1_LDSCRIPT_PATH) \
	$$(if $$($1_START_ADDRESS),$$($1_SECTION_START))
$1_LDLIBS = -nostartfiles $$(LDLIBS) $$($1_LIBS)
endef


# Define programing rules.
define stm32_program_rules

.PHONY: program.$1
ifeq ($$(words $$($1_PROGS)),1)
program.$1: $$($1_PROGS:%=%.$1.program)
endif

$1_PROGRAM_TOOL ?= stflash
$1_PROGRAM_FILE_stflash = bin
$1_PROGRAM_stflash = st-flash $(STFLASH_FLAGS) write $$< \
	$$(if $$($1_START_ADDRESS),$$($1_START_ADDRESS),0x8000000)
$1_PROGRAM_FILE_bmflash = hex
$1_PROGRAM_bmflash = bm-flash $(BMFLASH_FLAGS) $$<

%.$1.program: %.$1.$$($1_PROGRAM_FILE_$$($1_PROGRAM_TOOL))
	@echo "PROG [$1] $$<"
	$$Q$$($1_PROGRAM_$$($1_PROGRAM_TOOL))

endef


endif # stm32_once
