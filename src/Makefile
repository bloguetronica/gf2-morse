# Makefile for GF2 Morse Command, Debian Linux (GCC)
# Copyright (c) 2020-2024 Samuel Lourenço

# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <https://www.gnu.org/licenses/>.


# Please feel free to contact me via e-mail: samuel.fmlourenco@gmail.com


prefix = /usr/local

CC = gcc
CFLAGS = -O2 -std=c11 -Wall -pedantic
CXX = g++
CXXFLAGS = -O2 -std=c++11 -Wall -pedantic
LDFLAGS = -s
LDLIBS = -lusb-1.0
MANPAGES = gf2-morse.1
MANPAGESGZ = $(MANPAGES:=.gz)
MKDIR = mkdir -p
MV = mv -f
OBJECTS = cp2130.o error.o gf2device.o libusb-extra.o
RMDIR = rmdir --ignore-fail-on-non-empty
TARGETS = gf2-morse

.PHONY: all clean install uninstall

all: $(TARGETS)

$(TARGETS): % : %.o $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) *.o $(TARGETS)

install: all install-bin install-man

install-bin:
	$(MKDIR) $(DESTDIR)$(prefix)/bin && $(MV) $(TARGETS) $(DESTDIR)$(prefix)/bin/.

install-man:
	cd man && gzip -fknv9 $(MANPAGES) && $(MKDIR) $(DESTDIR)$(prefix)/share/man/man1 && $(MV) $(MANPAGESGZ) $(DESTDIR)$(prefix)/share/man/man1/.

uninstall: uninstall-man uninstall-bin clean

uninstall-bin:
	cd $(DESTDIR)$(prefix)/bin && $(RM) $(TARGETS)

uninstall-man:
	if [ -d $(DESTDIR)$(prefix)/share/man/man1 ]; then cd $(DESTDIR)$(prefix)/share/man/man1 && $(RM) $(MANPAGESGZ) && $(RMDIR) $(DESTDIR)$(prefix)/share/man/man1; fi
