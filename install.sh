#!/bin/sh

echo Obtaining required packages...
apt-get -qq update
apt-get -qq install build-essential
apt-get -qq install libusb-1.0-0-dev
echo Copying source code files...
mkdir -p /usr/local/src/gf2-morse/man
cp -f src/cp2130.cpp /usr/local/src/gf2-morse/.
cp -f src/cp2130.h /usr/local/src/gf2-morse/.
cp -f src/error.cpp /usr/local/src/gf2-morse/.
cp -f src/error.h /usr/local/src/gf2-morse/.
cp -f src/gf2device.cpp /usr/local/src/gf2-morse/.
cp -f src/gf2device.h /usr/local/src/gf2-morse/.
cp -f src/gf2-morse.cpp /usr/local/src/gf2-morse/.
cp -f src/GPL.txt /usr/local/src/gf2-morse/.
cp -f src/LGPL.txt /usr/local/src/gf2-morse/.
cp -f src/libusb-extra.c /usr/local/src/gf2-morse/.
cp -f src/libusb-extra.h /usr/local/src/gf2-morse/.
cp -f src/Makefile /usr/local/src/gf2-morse/.
cp -f src/man/gf2-morse.1 /usr/local/src/gf2-morse/man/.
cp -f src/README.txt /usr/local/src/gf2-morse/.
echo Building and installing binaries and man pages...
make -C /usr/local/src/gf2-morse install clean
echo Done!
