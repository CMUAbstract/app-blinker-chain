EXEC = chain.out

OBJECTS = \
	main.o \

include ../../../Makefile.common.gcc

CFLAGS += \
	-I$(LIBCHAIN_ROOT)/src/include \
	-I$(WISP_BASE_ROOT)

LFLAGS = \
	-L$(WISP_BASE_ROOT)/gcc \
	-L$(LIBCHAIN_ROOT)/gcc \
	-L$(LIBMSPCONSOLE_ROOT)/gcc \

LIBS = \
    -lwisp-base \
    -lchain \
    -lmspconsole

VPATH = $(SRC_DIR)
