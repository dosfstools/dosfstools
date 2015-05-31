# dosfstools src/Makefile.am
# Copyright (C) 2015  Andreas Bombe <aeb@debian.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AM_CFLAGS = -Wall -Wextra -Wno-sign-compare -Wno-missing-field-initializers \
	    -Wmissing-prototypes -Wstrict-prototypes -Wwrite-strings

sbin_PROGRAMS = fsck.fat mkfs.fat fatlabel

fsck_fat_SOURCES = boot.c check.c common.c fat.c file.c io.c lfn.c charconv.c \
		   fsck.fat.c
mkfs_fat_SOURCES = mkfs.fat.c
fatlabel_SOURCES = boot.c check.c common.c fat.c file.c io.c lfn.c charconv.c \
		   fatlabel.c

install-exec-hook:
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f fatlabel dosfslabel
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f fsck.fat dosfsck
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f fsck.fat fsck.msdos
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f fsck.fat fsck.vfat
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f mkfs.fat mkdosfs
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f mkfs.fat mkfs.msdos
	cd $(DESTDIR)$(sbindir) && $(LN_S) -f mkfs.fat mkfs.vfat

uninstall-hook:
	$(RM) $(DESTDIR)$(sbindir)/dosfslabel
	$(RM) $(DESTDIR)$(sbindir)/dosfsck
	$(RM) $(DESTDIR)$(sbindir)/fsck.msdos
	$(RM) $(DESTDIR)$(sbindir)/fsck.vfat
	$(RM) $(DESTDIR)$(sbindir)/mkdosfs
	$(RM) $(DESTDIR)$(sbindir)/mkfs.msdos
	$(RM) $(DESTDIR)$(sbindir)/mkfs.vfat
