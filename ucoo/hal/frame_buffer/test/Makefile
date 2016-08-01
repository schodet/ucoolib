BASE = ../../../..

TARGETS = host
PROGS = test_frame_buffer

test_frame_buffer_SOURCES = test_frame_buffer.host.cc

MODULES = ucoo/hal/frame_buffer

include $(BASE)/build/top.mk

host_CFLAGS += $(shell sdl2-config --cflags)
host_LIBS += $(shell sdl2-config --libs)
