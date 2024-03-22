This directory contains the source code files needed to compile the GF2 Morse
Command. A list of relevant files follows:
– cp2130.cpp;
– cp2130.h;
– error.cpp;
– error.h;
– gf2-morse.cpp;
– gf2device.cpp;
– gf2device.h;
– libusb-extra.c;
– libusb-extra.h;
– Makefile.

In order to compile the above command successfully, you must have the packages
"build-essential" and "libusb-1.0-0-dev" installed. Given that, if you wish to
simply compile, change your working directory to the current one on a terminal
window, and simply invoke "make" or "make all". If you wish to install besides
compiling, run "sudo make install". Alternatively, if you wish to force a
rebuild, you should invoke "make clean all", or "sudo make clean install" if
you prefer to install after rebuilding.

It may be necessary to undo any previous operations. Invoking "make clean"
will delete all object code generated (binaries included) during earlier
compilations. You can also invoke "sudo make uninstall" to unistall the
binaries.

P.S.:
Notice that any make operation containing the targets "install" or "uninstall"
(e.g. "make all install" or "make uninstall") requires root permissions, or in
other words, must be run with sudo.
