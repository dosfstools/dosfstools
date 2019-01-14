#include "charconv.h"
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static int iconv_init_codepage(int codepage, iconv_t *to_local, iconv_t *from_local)
{
    char codepage_name[32];
    snprintf(codepage_name, sizeof(codepage_name), "CP%d//TRANSLIT", codepage);
    *to_local = iconv_open(nl_langinfo(CODESET), codepage_name);
    if (*to_local == (iconv_t) - 1)
	perror(codepage_name);
    snprintf(codepage_name, sizeof(codepage_name), "CP%d", codepage);
    *from_local = iconv_open(codepage_name, nl_langinfo(CODESET));
    if (*from_local == (iconv_t) - 1)
	perror(codepage_name);
    return (*to_local && *from_local) ? 1 : 0;
}

static iconv_t dos_to_local;
static iconv_t local_to_dos;
static int used_codepage;

/*
 * Initialize conversion from codepage.
 * codepage = -1 means default codepage.
 * Returns 0 on success, non-zero on failure
 */
static int init_conversion(int codepage)
{
    static int initialized = -1;
    if (initialized < 0) {
	initialized = 1;
	if (codepage < 0)
	    codepage = DEFAULT_DOS_CODEPAGE;
	setlocale(LC_CTYPE, "");	/* initialize locale for CODESET */
	if (!iconv_init_codepage(codepage, &dos_to_local, &local_to_dos)
	    && codepage != DEFAULT_DOS_CODEPAGE) {
	    codepage = DEFAULT_DOS_CODEPAGE;
	    printf("Trying to set fallback DOS codepage %d\n",
		   codepage);
	    if (!iconv_init_codepage(codepage, &dos_to_local, &local_to_dos))
		initialized = 0;	/* no conversion available */
	}
	if (initialized)
	    used_codepage = codepage;
    }
    return initialized;
}

int set_dos_codepage(int codepage)
{
    return init_conversion(codepage);
}

int dos_char_to_printable(char **p, unsigned char c)
{
    char in[1] = { c };
    char *pin = in;
    size_t bytes_in = 1;
    size_t bytes_out = 4;
    if (!init_conversion(-1))
	return 0;
    return iconv(dos_to_local, &pin, &bytes_in, p, &bytes_out) != -1;
}

int local_string_to_dos_string(char *out, char *in, unsigned int out_size)
{
    char *pin = in;
    char *pout = out;
    size_t bytes_in = strlen(in);
    size_t bytes_out = out_size-1;
    size_t ret;
    if (!init_conversion(-1))
        return 0;
    ret = iconv(local_to_dos, &pin, &bytes_in, &pout, &bytes_out);
    if (ret == (size_t)-1) {
        fprintf(stderr, "Cannot convert input sequence '\\x%.02hhX' from codeset '%s' to 'CP%d': %s\n",
                *pin, nl_langinfo(CODESET), used_codepage, strerror(errno));
        return 0;
    }
    if (bytes_in != 0) {
        fprintf(stderr, "Cannot convert input string '%s' to 'CP%d': String is too long\n",
                in, used_codepage);
        return 0;
    }
    out[out_size-1-bytes_out] = 0;
    return 1;
}
