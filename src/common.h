/* common.h - Common functions

   Copyright (C) 1993 Werner Almesberger <werner.almesberger@lrc.di.epfl.ch>
   Copyright (C) 2008-2014 Daniel Baumann <mail@daniel-baumann.ch>

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

#ifndef _COMMON_H
#define _COMMON_H

/* from linux/types.h */
#if defined(__linux__)
    #include <linux/types.h>
#elif defined(__osx__)
    #include <stdint.h>

    typedef uint8_t __u8;
    typedef uint16_t __u16;
    typedef uint32_t __u32;
    typedef uint64_t __u64;

    typedef int16_t __le16;
    typedef int32_t __le32;

    /* from linux stdio.h */
    #ifndef loff_t
        typedef long long loff_t;
    #endif /* loff_t */

    #ifndef off64_t
        #ifdef _LP64
            typedef off_t off64_t;
        #else
            typedef __longlong_t off64_t;
        #endif /* _LP64 */
    #endif /* off64_t */

    /* from endian.h */
    #if defined(__APPLE__) && defined(__MACH__)
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

        #ifndef lseek64
            #define lseek64 lseek
        #endif /* lseek64 */
    #endif /* __APPLE__ && __MACH__ */
#endif

void die(const char *msg, ...) __attribute((noreturn));

/* Displays a prinf-style message and terminates the program. */

void pdie(const char *msg, ...) __attribute((noreturn));

/* Like die, but appends an error message according to the state of errno. */

void *alloc(int size);

/* mallocs SIZE bytes and returns a pointer to the data. Terminates the program
   if malloc fails. */

void *qalloc(void **root, int size);

/* Like alloc, but registers the data area in a list described by ROOT. */

void qfree(void **root);

/* Deallocates all qalloc'ed data areas described by ROOT. */

int min(int a, int b);

/* Returns the smaller integer value of a and b. */

char get_key(const char *valid, const char *prompt);

/* Displays PROMPT and waits for user input. Only characters in VALID are
   accepted. Terminates the program on EOF. Returns the character. */

#endif
