# Sternhalma

Also known as Chinese Chequers.

Portability of the application between different platforms is a particular goal, and specifically
portability to the third generation Kindle (aka Kindle Keyboard) which appears as an ideal
platform for playing interactive fiction.

The application includes a terminal emulator and a basic curses style front-end menu. So when
running on the Kindle a third-party terminal emulator and launcher is not necessary.

## Status

[![Build Status](https://travis-ci.org/AnotherJohnH/Sternhalma.svg?branch=master)](https://travis-ci.org/AnotherJohnH/Sternhalma)

## How to run

## How to build

Type scons in the top level directory to run the SConstruct file.

The build files will work out whether the host system is Linux or macOS and configure the
build environment for the host system as the target. The automatic target selection can be
overriden by setting the PROJ\_TARGET environment variable. e.g.

   PROJ\_TARGET=macOS

or

   PROJ\_TARGET=Linux

Cross targets are also selected via the PROJ\_TARGET environment variable. e.g.

   PROJ\_TARGET=Kindle3

The BUILD\_... .sh scripts are provided to initialise PROJ\_TARGET and set other
environment variables required by each specific build. These scripts should
be sourced and then the top level SConstruct invoked in the normal way.

### Linux and macOS

Depend on SDL2, so a development install of SDL2 is required.

### Kindle3

Although this is also a Linux build, it does not depend on SDL2.

Requires gcc built for arm-linux-gnueabihf and a set of headers and static runtime libraries
that are compatible with the Linux installed on the Kindle3. The original ARMv6 Raspberry Pi
running a Debian based Linux has been found to be suitable platform to build the Kindle3 version.

### Browser (via Emscripten)

Uses the SDL2 library supplied with Emscripten. Some re-work/conditonal compilation will
be required to make it work.

### iOS

Requires access to a source installation of SDL2 which is built into a linSDL2.a as part of
the application build process.

Build of an actual app has not been achieved yet, and may not be easily achievable without
an Xcode project file.

### Android

Requires an NDK installation. Currently builds as a console only app as an integration with
an Android build of SDL2 has not been achieved yet.

## Coding style

The source is C++ but has the following non-typical for modern C++ features ...
* Memory is statically or stack allocated i.e. (almost) no new/delete
* Use of C style stdio API
* In some places have re-invented the wheel avoiding functionality that is in standard libraries
* 3 space indent
