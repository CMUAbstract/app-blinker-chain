EXEC = blinker.out

OBJECTS = \
	main.o \

include $(MAKER_ROOT)/Makefile.gcc

CFLAGS += \
	-I$(LIBCHAIN_ROOT)/src/include \
	-I$(WISP_BASE_ROOT)

LFLAGS += \
	-L$(WISP_BASE_ROOT)/gcc \
	-L$(LIBCHAIN_ROOT)/bld/gcc \
	-L$(LIBMSPCONSOLE_ROOT)/bld/gcc \

LIBS = \
    -lwisp-base \
    -lchain \
    -lmspconsole

VPATH = ../../src
