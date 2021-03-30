#!/bin/sh

echo Obtaining required packages...
apt-get -qq update
apt-get -qq install build-essential
apt-get -qq install libusb-1.0-0-dev
echo Copying source code files...
mkdir -p /usr/local/src/gf2-morse
cp -f src/gf2-core.c /usr/local/src/gf2-morse/.
cp -f src/gf2-core.h /usr/local/src/gf2-morse/.
cp -f src/gf2-morse.c /usr/local/src/gf2-morse/.
cp -f src/GPL.txt /usr/local/src/gf2-morse/.
cp -f src/LGPL.txt /usr/local/src/gf2-morse/.
cp -f src/libusb-extra.c /usr/local/src/gf2-morse/.
cp -f src/libusb-extra.h /usr/local/src/gf2-morse/.
cp -f src/Makefile /usr/local/src/gf2-morse/.
cp -f src/README.txt /usr/local/src/gf2-morse/.
echo Building and installing binaries...
make -C /usr/local/src/gf2-morse all install clean
echo Installing man pages...
mkdir -p /usr/local/share/man/man1
cp -f man/gf2-morse.1.gz /usr/local/share/man/man1/.
echo Done!
