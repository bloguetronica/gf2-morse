#!/bin/sh

echo Removing man pages...
rm -f /usr/local/share/man/man1/gf2-morse.1.gz
rmdir --ignore-fail-on-non-empty /usr/local/share/man/man1
echo Removing binaries...
make -C /usr/local/src/gf2-morse uninstall
echo Removing source code files...
rm -rf /usr/local/src/gf2-morse
echo Done!
