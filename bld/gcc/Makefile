EXEC = blinker.out

OBJECTS = \
	main.o \

BOARD ?= mspts430
CONFIG_EDB ?= 0
#CONFIG_PRINTF_LIB ?= libedb
#CONFIG_LIBEDB_PRINTF ?= eif

include $(MAKER_ROOT)/Makefile.gcc

include $(MAKER_ROOT)/Makefile.board
include $(MAKER_ROOT)/Makefile.console
include $(MAKER_ROOT)/Makefile.edb
include $(MAKER_ROOT)/Makefile.chain

VPATH = ../../src
