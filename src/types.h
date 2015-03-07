/* types.h - Missing types

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

#ifndef _TYPES_H
#define _TYPES_H

#if defined(__linux__)
	#include <fcntl.h>
#elif defined(__APPLE__)
	#ifndef loff_t
		typedef long long loff_t;
	#endif /* loff_t */

	#ifndef lseek64
		#define lseek64 lseek
	#endif /* lseek64 */

	#ifndef off64_t
		#ifdef _LP64
			typedef off_t off64_t;
		#else
			typedef __longlong_t off64_t;
		#endif /* _LP64 */
	#endif /* off64_t */
#elif defined(__FreeBSD__)
	#ifndef loff_t
		typedef long long loff_t;
	#endif /* loff_t */

	#ifndef lseek64
		#define lseek64 lseek
	#endif /* lseek64 */

	#ifndef off64_t
		typedef off_t off64_t;
	#endif /* off64_t */
#endif

#endif /* _TYPES_H */
