EXEC = blinker

OBJECTS = \
	main.o \

DEPS += \
	libchain \
	libmsp \
	libwispbase \
	libio \

CONFIG_EDB ?= 0
#CONFIG_PRINTF_LIB ?= libedb
#CONFIG_LIBEDB_PRINTF ?= eif
include $(MAKER_ROOT)/Makefile.console

export LIBMSP_DCO_FREQ = 8000000
