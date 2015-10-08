EXEC = chain.out

OBJECTS = \
	main.o \

include $(MAKER_ROOT)/Makefile.clang

CFLAGS += \
	-I$(LIBCHAIN_ROOT)/src/include \
	-I$(WISP_BASE_ROOT) \

LFLAGS += \
	-L$(LIBCHAIN_ROOT)/bld/clang \
	-L$(WISP_BASE_ROOT)/gcc \

LIBS = \
    -lchain \
	-lwisp-base \

VPATH = ../../src
