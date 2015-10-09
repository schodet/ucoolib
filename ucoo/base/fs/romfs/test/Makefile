BASE = ../../../../..

TARGETS = host stm32f4
PROGS = test_romfs
test_romfs_SOURCES = test_romfs.cc

MODULES = ucoo/base/test ucoo/base/fs/romfs ucoo/hal/usb

COMPILE_DEPS = $(OBJDIR)/test_fs.h
EXTRA_CLEAN = $(OBJDIR)/test_fs.h

include $(BASE)/build/top.mk

$(OBJDIR)/test_fs.h: ../mkromfs.py hello.txt
	$< -o $@ -i test_fs $(wordlist 2,$(words $^),$^)
