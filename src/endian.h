/* endian.h - Endian functions

   Copyright (C) 2015 Álvaro Fernández Rojas <noltari@gmail.com>

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

#ifndef _ENDIAN_H
#define _ENDIAN_H

#if defined(__linux__)
	#include <endian.h>
#elif defined(__APPLE__)
	#include <libkern/OSByteOrder.h>

	#define htobe16(x) OSSwapHostToBigInt16(x)
	#define htole16(x) OSSwapHostToLittleInt16(x)
	#define be16toh(x) OSSwapBigToHostInt16(x)
	#define le16toh(x) OSSwapLittleToHostInt16(x)

	#define htobe32(x) OSSwapHostToBigInt32(x)
	#define htole32(x) OSSwapHostToLittleInt32(x)
	#define be32toh(x) OSSwapBigToHostInt32(x)
	#define le32toh(x) OSSwapLittleToHostInt32(x)

	#define htobe64(x) OSSwapHostToBigInt64(x)
	#define htole64(x) OSSwapHostToLittleInt64(x)
	#define be64toh(x) OSSwapBigToHostInt64(x)
	#define le64toh(x) OSSwapLittleToHostInt64(x)
#elif defined(__FreeBSD__)
	#include <sys/endian.h>

	#define be16toh(x) betoh16(x)
	#define le16toh(x) letoh16(x)

	#define be32toh(x) betoh32(x)
	#define le32toh(x) letoh32(x)

	#define be64toh(x) betoh64(x)
	#define le64toh(x) letoh64(x)
#endif

#endif /* _ENDIAN_H */
