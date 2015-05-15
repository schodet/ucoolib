BASE = ../../../..

TARGETS = host stm32f4
PROGS = test_uart
stm32f4_PROGS = test_uart_disc
test_uart_SOURCES = test_uart.cc
test_uart_disc_SOURCES = test_uart_disc.cc

MODULES = ucoo/hal/uart
test_uart_disc_MODULES = $(MODULES) ucoo/base/test ucoo/hal/usb

include $(BASE)/build/top.mk
