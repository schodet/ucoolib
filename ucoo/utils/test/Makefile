BASE = ../../..

TARGETS = host stm32f4
PROGS = test_fifo test_crc test_function test_pool
stm32f4_PROGS = test_delay
test_fifo_SOURCES = test_fifo.cc
test_delay_SOURCES = test_delay.cc
test_crc_SOURCES = test_crc.cc
test_function_SOURCES = test_function.cc
test_pool_SOURCES = test_pool.cc

MODULES = ucoo/utils ucoo/base/test ucoo/hal/usb

include $(BASE)/build/top.mk
