EXEC = chain.out

OBJECTS = \
	main.o \

include ../../../Makefile.common.clang

CFLAGS += \
	-I $(LIBCHAIN_ROOT)/src/include \

LFLAGS += \
	-L $(LIBCHAIN_ROOT)/clang \

LIBS = \
    -lchain \

VPATH = $(SRC_DIR)
