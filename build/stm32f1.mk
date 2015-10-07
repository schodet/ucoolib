# ucoolib - Microcontroller object oriented library.
#
# Rules for STM32F1.

include $(UCOO_BASE)/build/stm32.mk

$(eval $(call stm32_libopencm3,stm32f1))

stm32f1_SUBTARGETS = stm32 arm newlib

stm32f1_CPU = cortex-m3
stm32f1_STM32_DEFS = -DSTM32F1

$(eval $(call stm32_flags,stm32f1))

stm32f1_PREFIX = arm-none-eabi-
$(eval $(call arch_cmds,stm32f1))

stm32f1_ELF_SUFFIX = .elf

# Rules.

$(eval $(call arch_build_rules,stm32f1))
$(eval $(call arch_lst_rules,stm32f1))
$(eval $(call arch_bin_rules,stm32f1))
$(eval $(call arch_size_rules,stm32f1))
$(eval $(call arch_test_rules,stm32f1))
$(eval $(call arch_misc_rules,stm32f1))
$(eval $(call stm32_program_rules,stm32f1))
