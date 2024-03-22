#!/bin/sh

echo Removing binaries and man pages...
make -C /usr/local/src/gf2-morse uninstall
echo Removing source code files...
rm -rf /usr/local/src/gf2-morse
echo Done!
