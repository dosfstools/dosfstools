/* fatlabel.c - User interface

   Copyright (C) 1993 Werner Almesberger <werner.almesberger@lrc.di.epfl.ch>
   Copyright (C) 1998 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
   Copyright (C) 2007 Red Hat, Inc.
   Copyright (C) 2008-2014 Daniel Baumann <mail@daniel-baumann.ch>
   Copyright (C) 2015 Andreas Bombe <aeb@debian.org>

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

#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

#include "common.h"
#include "fsck.fat.h"
#include "io.h"
#include "boot.h"
#include "fat.h"
#include "file.h"
#include "check.h"

int rw = 0, list = 0, test = 0, verbose = 0;
unsigned n_files = 0;
void *mem_queue = NULL;

static void usage(int error)
{
    FILE *f = error ? stderr : stdout;
    int status = error ? 1 : 0;

    fprintf(f, "usage: fatlabel device [label]\n");
    exit(status);
}

int main(int argc, char *argv[])
{
    const struct option long_options[] = {
	    {"version", no_argument, NULL, 'V'},
	    {"help",    no_argument, NULL, 'h'},
	    {0,}
    };
    int change;

    DOS_FS fs = { 0 };
    rw = 0;

    int i, c;

    char *device = NULL;
    char label[12] = { 0 };

    off_t offset;
    DIR_ENT de;

    check_atari();

    while ((c = getopt_long(argc, argv, "Vh", long_options, NULL)) != -1) {
	switch (c) {
	    case 'V':
		printf("fatlabel " VERSION " (" VERSION_DATE ")\n");
		exit(0);
		break;

	    case 'h':
		usage(0);
		break;

	    default:
		fprintf(stderr,
			"Internal error: getopt_long() return unexpected value %d\n", c);
		exit(2);
	}
    }

    if (optind == argc - 2) {
	change = 1;
    } else if (optind == argc - 1) {
	change = 0;
    } else {
	usage(1);
    }

    device = argv[optind++];
    if (change) {
	strncpy(label, argv[optind], 11);
	if (strlen(argv[optind]) > 11) {
	    fprintf(stderr,
		    "fatlabel: labels can be no longer than 11 characters\n");
	    exit(1);
	}
	for (i = 0; label[i] && i < 11; i++)
	    /* don't know if here should be more strict !uppercase(label[i]) */
	    if (islower(label[i])) {
		fprintf(stderr,
			"fatlabel: warning - lowercase labels might not work properly with DOS or Windows\n");
		break;
	    }
	rw = 1;
    }

    fs_open(device, rw);
    read_boot(&fs);
    if (fs.fat_bits == 32)
	read_fat(&fs);
    if (!rw) {
	offset = find_volume_de(&fs, &de);
	if (offset == 0)
	    fprintf(stdout, "%.11s\n", fs.label);
	else
	    fprintf(stdout, "%.8s%.3s\n", de.name, de.name + 8);
	exit(0);
    }

    write_label(&fs, label);
    fs_close(rw);
    return 0;
}
