#export BOARD ?= mspts430
export BOARD ?= msp-exp430fr5994

TOOLS = \

TOOLCHAINS = \
	gcc \
	clang \

include ext/maker/Makefile
