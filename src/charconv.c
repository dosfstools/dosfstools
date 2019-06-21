#include "charconv.h"
#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

/* CP850 table for 0x80-0xFF range from:
 * http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/PC/CP850.TXT
 */
static const wchar_t cp850_table[128] = {
    0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
    0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
    0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
    0x00ff, 0x00d6, 0x00dc, 0x00f8, 0x00a3, 0x00d8, 0x00d7, 0x0192,
    0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
    0x00bf, 0x00ae, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00c1, 0x00c2, 0x00c0,
    0x00a9, 0x2563, 0x2551, 0x2557, 0x255d, 0x00a2, 0x00a5, 0x2510,
    0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x00e3, 0x00c3,
    0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x00a4,
    0x00f0, 0x00d0, 0x00ca, 0x00cb, 0x00c8, 0x0131, 0x00cd, 0x00ce,
    0x00cf, 0x2518, 0x250c, 0x2588, 0x2584, 0x00a6, 0x00cc, 0x2580,
    0x00d3, 0x00df, 0x00d4, 0x00d2, 0x00f5, 0x00d5, 0x00b5, 0x00fe,
    0x00de, 0x00da, 0x00db, 0x00d9, 0x00fd, 0x00dd, 0x00af, 0x00b4,
    0x00ad, 0x00b1, 0x2017, 0x00be, 0x00b6, 0x00a7, 0x00f7, 0x00b8,
    0x00b0, 0x00a8, 0x00b7, 0x00b9, 0x00b3, 0x00b2, 0x25a0, 0x00a0,
};

static int wchar_string_to_cp850_string(char *out, wchar_t *in, unsigned int out_size)
{
    unsigned i, j;
    for (i = 0; i < out_size-1 && in[i]; ++i) {
        if (in[i] > 0 && in[i] < 0x80) {
            out[i] = in[i];
            continue;
        }
        for (j = 0; j < 0x80; ++j) {
            if (in[i] == cp850_table[j]) {
                out[i] = (0x80 | j);
                break;
            }
        }
        if (j == 0x80) {
            fprintf(stderr, "Cannot convert input character '%lc' to 'CP850': %s\n", (wint_t)in[i], strerror(EILSEQ));
            return 0;
        }
    }
    out[i] = 0;
    return 1;
}

static int cp850_char_to_printable(char **p, unsigned char c)
{
    wchar_t wc = (c & 0x80) ? cp850_table[c & 0x7F] : c;
    int ret = wctomb(*p, wc);
    if (ret != -1)
        *p += ret;
    return ret != -1;
}

static int local_string_to_cp850_string(char *out, char *in, unsigned int out_size)
{
    int ret;
    wchar_t *wcs;
    if (strlen(in) >= out_size) {
        fprintf(stderr, "Cannot convert input string '%s' to 'CP850': String is too long\n", in);
        return 0;
    }
    wcs = calloc(out_size, sizeof(wchar_t));
    if (!wcs) {
        fprintf(stderr, "Cannot convert input string '%s' to 'CP850': %s\n", in, strerror(ENOMEM));
        return 0;
    }
    if (mbstowcs(wcs, in, out_size) == (size_t)-1) {
        fprintf(stderr, "Cannot convert input string '%s' to 'CP850': %s\n", in, strerror(errno));
        free(wcs);
        return 0;
    }
    ret = wchar_string_to_cp850_string(out, wcs, out_size);
    free(wcs);
    return ret;
}

#ifdef HAVE_ICONV

static int iconv_init_codepage(int codepage, iconv_t *to_local, iconv_t *from_local)
{
    char codepage_name[32];
    snprintf(codepage_name, sizeof(codepage_name), "CP%d//TRANSLIT", codepage);
    *to_local = iconv_open(nl_langinfo(CODESET), codepage_name);
    if (*to_local == (iconv_t) - 1)
        fprintf(stderr, "Cannot initialize conversion from codepage %d to %s: %s\n", codepage, nl_langinfo(CODESET), strerror(errno));
    snprintf(codepage_name, sizeof(codepage_name), "CP%d", codepage);
    *from_local = iconv_open(codepage_name, nl_langinfo(CODESET));
    if (*from_local == (iconv_t) - 1)
        fprintf(stderr, "Cannot initialize conversion from %s to codepage %d: %s\n", nl_langinfo(CODESET), codepage, strerror(errno));
    return (*to_local != (iconv_t)-1 && *from_local != (iconv_t)-1) ? 1 : 0;
}

static iconv_t dos_to_local;
static iconv_t local_to_dos;
static int used_codepage;
static int internal_cp850;

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
	if (!iconv_init_codepage(codepage, &dos_to_local, &local_to_dos))
	    initialized = 0;
	if (!initialized && codepage == 850) {
	    fprintf(stderr, "Using internal CP850 conversion table\n");
	    internal_cp850 = 1;	/* use internal CP850 conversion table */
	    initialized = 1;
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
    if (internal_cp850)
        return cp850_char_to_printable(p, c);
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
    if (internal_cp850)
        return local_string_to_cp850_string(out, in, out_size);
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

#else

int set_dos_codepage(int codepage)
{
    static int initialized = -1;
    if (initialized < 0) {
        setlocale(LC_CTYPE, ""); /* initialize locale for wide character functions */
        if (codepage < 0)
            codepage = DEFAULT_DOS_CODEPAGE;
        initialized = (codepage == 850) ? 1 : 0;
        if (!initialized)
            fprintf(stderr, "Cannot initialize unsupported codepage %d, only codepage 850 is supported\n", codepage);
    }
    return initialized;
}

int dos_char_to_printable(char **p, unsigned char c)
{
    return cp850_char_to_printable(p, c);
}

int local_string_to_dos_string(char *out, char *in, unsigned int len)
{
    return local_string_to_cp850_string(out, in, len);
}

#endif
