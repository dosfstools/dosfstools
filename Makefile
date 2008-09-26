# Makefile

DESTDIR =
PREFIX = /usr/local
SBINDIR = $(PREFIX)/sbin
DOCDIR = $(PREFIX)/share/doc
MANDIR = $(PREFIX)/share/man

#OPTFLAGS = -O2 -fomit-frame-pointer -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
OPTFLAGS = -O2 -fomit-frame-pointer $(shell getconf LFS_CFLAGS)
#WARNFLAGS = -Wall -pedantic -std=c99
WARNFLAGS = -Wall
DEBUGFLAGS = -g
CFLAGS += $(OPTFLAGS) $(WARNFLAGS) $(DEBUGFLAGS)

VPATH = src

all: build

build: dosfsck dosfslabel mkdosfs

dosfsck: boot.o check.o common.o fat.o file.o io.o lfn.o dosfsck.o

dosfslabel: boot.o check.o common.o fat.o file.o io.o lfn.o dosfslabel.o

mkdosfs: mkdosfs.o

rebuild: distclean build

install: install-bin install-doc install-man

install-bin: build
	install -d -m 0755 $(DESTDIR)/$(SBINDIR)
	install -m 0755 dosfsck dosfslabel mkdosfs $(DESTDIR)/$(SBINDIR)

	ln -sf dosfsck $(DESTDIR)/$(SBINDIR)/fsck.msdos
	ln -sf dosfsck $(DESTDIR)/$(SBINDIR)/fsck.vfat
	ln -sf mkdosfs $(DESTDIR)/$(SBINDIR)/mkfs.msdos
	ln -sf mkdosfs $(DESTDIR)/$(SBINDIR)/mkfs.vfat

install-doc:
	install -d -m 0755 $(DESTDIR)/$(DOCDIR)/dosfstools
	install -m 0644 doc/* $(DESTDIR)/$(DOCDIR)/dosfstools

install-man:
	install -d -m 0755 $(DESTDIR)/$(MANDIR)/man8
	install -m 0644 man/*.8 $(DESTDIR)/$(MANDIR)/man8

	ln -sf dosfsck.8 $(DESTDIR)/$(MANDIR)/man8/fsck.msdos.8
	ln -sf dosfsck.8 $(DESTDIR)/$(MANDIR)/man8/fsck.vfat.8
	ln -sf mkdosfs.8 $(DESTDIR)/$(MANDIR)/man8/mkfs.msdos.8
	ln -sf mkdosfs.8 $(DESTDIR)/$(MANDIR)/man8/mkfs.vfat.8

uninstall: uninstall-bin uninstall-doc uninstall-man

uninstall-bin:
	rm -f $(DESTDIR)/$(SBINDIR)/dosfsck
	rm -f $(DESTDIR)/$(SBINDIR)/dosfslabel
	rm -f $(DESTDIR)/$(SBINDIR)/mkdosfs

	rm -f $(DESTDIR)/$(SBINDIR)/fsck.msdos
	rm -f $(DESTDIR)/$(SBINDIR)/fsck.vfat
	rm -f $(DESTDIR)/$(SBINDIR)/mkfs.msdos
	rm -f $(DESTDIR)/$(SBINDIR)/mkfs.vfat

	rmdir --ignore-fail-on-non-empty $(DESTDIR)/$(SBINDIR)

uninstall-doc:
	rm -rf $(DESTDIR)/$(DOCDIR)/dosfstools

	rmdir --ignore-fail-on-non-empty $(DESTDIR)/$(DOCDIR)

uninstall-man:
	rm -f $(DESTDIR)/$(MANDIR)/man8/dosfsck.8
	rm -f $(DESTDIR)/$(MANDIR)/man8/dosfslabel.8
	rm -f $(DESTDIR)/$(MANDIR)/man8/mkdosfs.8

	rm -f $(DESTDIR)/$(MANDIR)/man8/fsck.msdos.8
	rm -f $(DESTDIR)/$(MANDIR)/man8/fsck.vfat.8
	rm -f $(DESTDIR)/$(MANDIR)/man8/mkfs.msdos.8
	rm -f $(DESTDIR)/$(MANDIR)/man8/mkfs.vfat.8

	rmdir --ignore-fail-on-non-empty $(DESTDIR)/$(MANDIR)/man8
	rmdir --ignore-fail-on-non-empty $(DESTDIR)/$(MANDIR)

reinstall: distclean install

clean:
	rm -f *.o

distclean: clean
	rm -f dosfsck dosfslabel mkdosfs

.PHONY: build rebuild install install-bin install-doc install-man uninstall uninstall-bin uninstall-doc uninstall-man reinstall clean distclean
