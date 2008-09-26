
all: dosfsck dosfslabel mkdosfs

dosfsck: boot.o check.o common.o fat.o file.o io.o lfn.o dosfsck.o
	$(CC) -o $@ $(LDFLAGS) $^

dosfslabel : boot.o check.o common.o fat.o file.o io.o lfn.o dosfslabel.o
	$(CC) -o $@ $(LDFLAGS) $^

mkdosfs: mkdosfs.o
	$(CC) -o $@ $(LDFLAGS) $^

.c.o:
	$(CC) -c $(CFLAGS) $*.c

install: dosfsck
	mkdir -p $(SBINDIR) $(MANDIR)
	install -m 755 dosfsck $(SBINDIR)
	install -m 644 dosfsck.8 $(MANDIR)
	install -m 755 dosfslabel $(SBINDIR)
	rm -f $(SBINDIR)/fsck.msdos
	rm -f $(SBINDIR)/fsck.vfat
	ln -s dosfsck $(SBINDIR)/fsck.msdos
	ln -s dosfsck $(SBINDIR)/fsck.vfat
	rm -f $(MANDIR)/fsck.msdos.8
	ln -s dosfsck.8 $(MANDIR)/fsck.msdos.8
	ln -s dosfsck.8 $(MANDIR)/fsck.vfat.8

	mkdir -p $(SBINDIR) $(MANDIR)
	install -m 755 mkdosfs $(SBINDIR)
	install -m 644 mkdosfs.8 $(MANDIR)
	rm -f $(SBINDIR)/mkfs.msdos
	rm -f $(SBINDIR)/mkfs.vfat
	ln -s mkdosfs $(SBINDIR)/mkfs.msdos
	ln -s mkdosfs $(SBINDIR)/mkfs.vfat
	rm -f $(MANDIR)/mkfs.msdos.8
	ln -s mkdosfs.8 $(MANDIR)/mkfs.msdos.8
	ln -s mkdosfs.8 $(MANDIR)/mkfs.vfat.8

clean:
	rm -f *.o *.s *.i *~ \#*# tmp_make .#* .new*

distclean: clean
	rm -f *.a dosfsck dosfslabel mkdosfs

dep:
	sed '/\#\#\# Dependencies/q' <Makefile >tmp_make
	$(CPP) $(CFLAGS) -MM *.c >>tmp_make
	mv tmp_make Makefile

### Dependencies
boot.o: boot.c common.h dosfsck.h io.h boot.h
check.o: check.c common.h dosfsck.h io.h fat.h file.h lfn.h check.h
common.o: common.c common.h
dosfsck.o: dosfsck.c version.h common.h dosfsck.h io.h boot.h fat.h \
  file.h check.h
dosfslabel.o: dosfslabel.c version.h common.h dosfsck.h io.h boot.h fat.h \
  file.h check.h
fat.o: fat.c common.h dosfsck.h io.h check.h fat.h
file.o: file.c common.h file.h
io.o: io.c dosfsck.h common.h io.h
lfn.o: lfn.c common.h io.h dosfsck.h lfn.h file.h
mkdosfs.o: mkdosfs.c version.h
