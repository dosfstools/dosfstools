/* dosfslabel.c  -  User interface */

/* Copyright 2007 Red Hat, Inc.
 * Portions copyright 1998 Roman Hodek.
 * Portions copyright 1993 Werner Almesberger.
 */

#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "common.h"
#include "dosfsck.h"
#include "io.h"
#include "boot.h"
#include "fat.h"
#include "file.h"
#include "check.h"


int interactive = 0,list = 0,test = 0,verbose = 0,write_immed = 0;
int atari_format = 0;
unsigned n_files = 0;
void *mem_queue = NULL;


static void usage(int error)
{
    FILE *f = error ? stderr : stdout;
    int status = error ? 1 : 0;

    fprintf(f,"usage: dosfslabel device [label]\n");
    exit(status);
}

/*
 * ++roman: On m68k, check if this is an Atari; if yes, turn on Atari variant
 * of MS-DOS filesystem by default.
 */
static void check_atari( void )
{
#ifdef __mc68000__
    FILE *f;
    char line[128], *p;

    if (!(f = fopen( "/proc/hardware", "r" ))) {
	perror( "/proc/hardware" );
	return;
    }

    while( fgets( line, sizeof(line), f ) ) {
	if (strncmp( line, "Model:", 6 ) == 0) {
	    p = line + 6;
	    p += strspn( p, " \t" );
	    if (strncmp( p, "Atari ", 6 ) == 0)
		atari_format = 1;
	    break;
	}
    }
    fclose( f );
#endif
}


int main(int argc, char *argv[])
{
    DOS_FS fs;
    int rw = 0;

    char *device = NULL;
    char *label = NULL;

    check_atari();

    if (argc < 2 || argc > 3)
        usage(1);

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
        usage(0);
    else if (!strcmp(argv[1], "-V") || !strcmp(argv[1], "--version")) {
        printf( "dosfslabel " VERSION ", " VERSION_DATE ", FAT32, LFN\n" );
        exit(0);
    }

    device = argv[1];
    if (argc == 3) {
        label = argv[2];
        if (strlen(label) > 11) {
            fprintf(stderr,
                    "dosfslabel: labels can be no longer than 11 characters\n");
            exit(1);
        }
        rw = 1;
    }

    fs_open(device, rw);
    read_boot(&fs);
    if (!rw) {
        fprintf(stdout, "%s\n", fs.label);
        exit(0);
    }

    write_label(&fs, label);
    return fs_close(rw) ? 1 : 0;
}
