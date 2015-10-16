EXEC = blinker.out

OBJECTS = \
	main.o \

LIBCHAIN_ENABLE_DIAGNOSTICS ?= 0
CONFIG_PRINTF_LIB ?= libmspconsole

include $(MAKER_ROOT)/Makefile.gcc

CFLAGS += \
	-I$(LIBCHAIN_ROOT)/src/include \
	-I$(WISP_BASE_ROOT) \
	-I$(LIBEDB_ROOT)/src/include \

ifeq ($(LIBCHAIN_ENABLE_DIAGNOSTICS),1)
CFLAGS += -DLIBCHAIN_ENABLE_DIAGNOSTICS
endif

ifeq ($(CONFIG_PRINTF_LIB),libedb)
CFLAGS += -DCONFIG_LIBEDB_PRINTF
LIBS += -ledb
LFLAGS += -L$(LIBEDB_ROOT)/gcc
endif

ifeq ($(CONFIG_PRINTF_LIB),libmspconsole)
CFLAGS += -DCONFIG_LIBMSPCONSOLE_PRINTF
LIBS += -lmspconsole
LFLAGS += -L$(LIBMSPCONSOLE_ROOT)/bld/gcc
endif

LFLAGS += \
	-L$(WISP_BASE_ROOT)/gcc \
	-L$(LIBCHAIN_ROOT)/bld/gcc \

LIBS += \
    -lwisp-base \
    -lchain \

VPATH = ../../src
