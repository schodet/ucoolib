# ucoolib - Microcontroller object oriented library.
#
# Rules for STM32F4.

include $(UCOO_BASE)/build/stm32.mk

$(eval $(call stm32_libopencm3,stm32f4))

stm32f4_SUBTARGETS := stm32 arm newlib

stm32f4_CPU := cortex-m4
stm32f4_STM32_DEFS := -DSTM32F4

$(eval $(call stm32_flags,stm32f4))

stm32f4_PREFIX := arm-none-eabi-
$(eval $(call arch_cmds,stm32f4))

stm32f4_ELF_SUFFIX = .elf

# Rules.

$(eval $(call arch_build_rules,stm32f4))
$(eval $(call arch_lst_rules,stm32f4))
$(eval $(call arch_bin_rules,stm32f4))
$(eval $(call arch_size_rules,stm32f4))
$(eval $(call arch_misc_rules,stm32f4))
$(eval $(call stm32_program_rules,stm32f4))
