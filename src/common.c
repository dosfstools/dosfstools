/* common.c - Common functions

   Copyright (C) 1993 Werner Almesberger <werner.almesberger@lrc.di.epfl.ch>
   Copyright (C) 1998 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
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

/* FAT32, VFAT, Atari format support, and various fixes additions May 1998
 * by Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de> */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <termios.h>

#include "common.h"

typedef struct _link {
    void *data;
    struct _link *next;
} LINK;

void die(const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

void pdie(const char *msg, ...)
{
    va_list args;

    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, ":%s\n", strerror(errno));
    exit(1);
}

void *alloc(int size)
{
    void *this;

    if ((this = malloc(size)))
	return this;
    pdie("malloc");
    return NULL;		/* for GCC */
}

void *qalloc(void **root, int size)
{
    LINK *link;

    link = alloc(sizeof(LINK));
    link->next = *root;
    *root = link;
    return link->data = alloc(size);
}

void qfree(void **root)
{
    LINK *this;

    while (*root) {
	this = (LINK *) * root;
	*root = this->next;
	free(this->data);
	free(this);
    }
}

int min(int a, int b)
{
    return a < b ? a : b;
}


#ifndef HAVE_VASPRINTF
static int vasprintf(char **strp, const char *fmt, va_list va)
{
    int length;
    va_list vacopy;

    va_copy(vacopy, va);

    length = vsnprintf(NULL, 0, fmt, vacopy);
    if (length < 0)
	return length;

    *strp = malloc(length + 1);
    if (!*strp) {
	errno = ENOMEM;
	return -1;
    }

    return vsnprintf(*strp, length + 1, fmt, va);
}
#endif

int xasprintf(char **strp, const char *fmt, ...)
{
    va_list va;
    int retval;

    va_start(va, fmt);
    retval = vasprintf(strp, fmt, va);
    va_end(va);

    if (retval < 0)
	pdie("asprintf");

    return retval;
}

char get_key(const char *valid, const char *prompt)
{
    int ch, okay;

    while (1) {
	if (prompt)
	    printf("%s ", prompt);
	fflush(stdout);
	while (ch = getchar(), ch == ' ' || ch == '\t') ;
	if (ch == EOF)
	    exit(1);
	if (!strchr(valid, okay = ch))
	    okay = 0;
	while (ch = getchar(), ch != '\n' && ch != EOF) ;
	if (ch == EOF)
	    exit(1);
	if (okay)
	    return okay;
	printf("Invalid input.\n");
    }
}


char *get_line(const char *prompt, char *dest, size_t length)
{
    struct termios tio, tio_orig;
    int tio_fail;
    char *retval;

    tio_fail = tcgetattr(0, &tio_orig);
    if (!tio_fail) {
	tio = tio_orig;
	tio.c_lflag |= ICANON | ECHO;
	tcsetattr(0, TCSAFLUSH, &tio);
    }

    printf("%s: ", prompt);
    fflush(stdout);

    retval = fgets(dest, length, stdin);

    if (!tio_fail)
	tcsetattr(0, TCSAFLUSH, &tio_orig);
    return retval;
}
