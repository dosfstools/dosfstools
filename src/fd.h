/* fd.h - The FD constants/structures

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.

   The complete text of the GNU General Public License
   can be found in /usr/share/common-licenses/GPL-3 file.
*/

#ifndef _FD_H
#define _FD_H

#include <sys/ioctl.h>

/*
 * Geometry
 */
struct floppy_struct {
	unsigned int size;	/* nr of sectors total */
	unsigned int sect;	/* sectors per track */
	unsigned int head;	/* nr of heads */
	unsigned int track;	/* nr of tracks */
	unsigned int stretch;	/* bit 0 !=0 means double track steps */
				/* bit 1 != 0 means swap sides */
				/* bits 2..9 give the first sector */
				/*  number (the LSB is flipped) */
	unsigned char gap;	/* gap1 size */
	unsigned char rate;	/* data rate. |= 0x40 for perpendicular */
	unsigned char spec1;	/* stepping rate, head unload time */
	unsigned char fmt_gap;	/* gap2 size */
	const char * name;	/* used only for predefined formats */
};

#define FDGETPRM _IOR(2, 0x04, struct floppy_struct)
/* set/get disk parameters */

#endif /* _FD_H */
