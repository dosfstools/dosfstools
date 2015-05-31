# configure.ac for dosfstools
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

AC_INIT([dosfstools], [3.99.0])
AC_SUBST([RELEASE_DATE], [2015-05-16])
AM_INIT_AUTOMAKE([foreign])


AC_PROG_CC
AC_PROG_LN_S

AC_SYS_LARGEFILE


AC_CONFIG_FILES([Makefile src/Makefile src/version.h
		 manpages/Makefile manpages/mkfs.fat.8
		 manpages/fsck.fat.8 manpages/fatlabel.8])
AC_OUTPUT
