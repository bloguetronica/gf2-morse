This package contains all the necessary files and scripts to install the
additional GF2 Morse Command. The scripts included here are compatible with
most 32-bit and 64-bit Debian based operating systems (e.g. Linux Mint, Ubuntu
and variants, etc). Prior to installation, you must certify that your system
is Debian based (or at least uses apt-get) and that you have an active
Internet connection.

A list of the included scripts follows:
– install.sh;
– uninstall.sh.

In order to compile and install the command for the first time, you have to
run "install.sh" by typing "sudo ./install.sh" on a terminal window, after
changing your working directory to the current one. This script will first
obtain and install the required "build-essential" and "libusb-1.0-0-dev"
packages (if they are not installed yet). Then it will compile the binary and
move it to "/usr/local/bin/". It will also copy the corresponding man page to
"/usr/local/share/man/man1/". Note that the installation script won't create
the necessary access rules, in the assumption that the main commands are, or
will be installed.

To uninstall, you should run "uninstall.sh" by typing "sudo ./uninstall.sh",
again on a terminal window after making sure that your working directory is
this one. This will delete the previously installed binary, the corresponding
man page and the source code. However it won't remove the "build-essential"
and "libusb-1.0-0-dev" packages, since they could be already installed prior
to the first installation.
