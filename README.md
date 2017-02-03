An LED Blinker Application
==========================

An application that blinks two LEDs, alternating between them, implemented
using the tasks-and-channels abstraction of
[Chain](https://github.com/CMUAbstract/libchain).

Toolchains
==========

The following toolchains must be installed on the system.

## MSP430 GCC distributed by TI

TI MSP430 GCC Toolchain is the cross-compiler used to build executables for the
MSP430 platform.

*Version*: Only v3.05 is supported; changes in v4.00 appear to not be backward compatible.

*Upstream*: http://www.ti.com/tool/msp430-gcc-opensource

Arch Linux package (from AUR): `mspgcc-ti`, installed to `/opt/ti/mspgcc`

## *Optional*: LLVM/Clang with MSP430 backend

**TODO: building this app with Clang is not yet supported**

Build and Run
=============

This app uses the [Maker](https://github.com/CMUAbstract/maker) build system.
Maker manages two aspects: (1) builds of dependency libraries, and (2)
co-existing builds using multiple toolchains.

Dependency libraries are managed using git submodules, in `ext` subdirectory --
this way the app can control the exact version/commit of each library, while
the library upstream is free to evolve. Dependencies are rebuilt along with the
app on purpose so that compiler flags are consistent. Dependencies are
specified in `bld/Makefile`.

Clone
-----

To clone this repository along with the dependency libraries:

    git clone --recursive https://github.com/CMUAbstract/app-blinker-chain


Configure
---------

Set `BOARD` environment variable, or edit it in `Makefile`, (see `Makefile.board`
in [Maker](https://github.com/CMUAbstract/maker) for a list of supported boards):

    export BOARD=mspts430

Configure library parameters in `bld/Makefile` as desired, e.g. MCU frequency,
console output mechanism, etc.

Set paths to toolchains (see `Makefile.env` in
[Maker](https://github.com/CMUAbstract/maker) for more details):

    export TOOLCHAIN_ROOT=/path/to/mspgcc
    export DEV_ROOT=/path/to/llvm/parent/dir


Compile
-------

The general make target convention is `bld/<toolchain>/<target>`.  This app
supports two toolchains: `gcc` and `clang` (**TODO**: clang, not yet).

To build the app along with each dependency library:

    make bld/gcc/all

To clean the app build

    make bld/gcc/clean

To clean the app build and the build of each dependency library:

    make bld/gcc/depclean

To build each dependency library but not the app:

    make bld/gcc/dep

Flash
-----

To program the MSP430 board:

    make bld/gcc/prog

By default, the FET is assumed to be at /dev/ttyACM0, unless specified:

    make FET_DEVICE=/dev/ttyACM1 bld/gcc/prog
