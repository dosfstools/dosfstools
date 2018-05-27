#include "charconv.h"
#include <iconv.h>
#include <langinfo.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
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
	int defcp = default_dos_codepage();
	if (codepage < 0)
	    codepage = defcp;
	setlocale(LC_CTYPE, "");	/* initialize locale for CODESET */
	if (!iconv_init_codepage(codepage, &dos_to_local, &local_to_dos)
	    && codepage != defcp) {
	    codepage = defcp;
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

int local_string_to_dos_string(char *out, char *in, unsigned int len)
{
    char *pin = in;
    char *pout = out;
    size_t bytes_in = strlen(in);
    size_t bytes_out = len-1;
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
    out[len-bytes_out] = 0;
    return 1;
}

struct code_mapping
{
    const char *lang;
    int code_page;
};

int default_dos_codepage(void)
{
    /*
     * Built from the list at:
     * https://web.archive.org/web/20171015144140/https://www.microsoft.com/resources/msdn/goglobal/default.mspx
     *
     * Charsets in $LC_ALL/$LC_CTYPE/$LANG are disregarded. The only time that
     * would ever matter is if the charset was ".cp<something>", which is
     * basically never the case.
     */

    static const struct code_mapping code_map[] = {
	{"ar", 720}, {"ba", 866}, {"be", 866}, {"bg", 866}, {"cs", 852},
	{"el", 737}, {"et", 775}, {"fa", 720}, {"fil", 437}, {"ha", 437},
	{"he", 862}, {"hr", 852}, {"hu", 852}, {"ig", 437}, {"iu", 437},
	{"ja", 932}, {"kk", 866}, {"ko", 949}, {"ky", 866}, {"lt", 775},
	{"lv", 775}, {"mk", 866}, {"mn", 866}, {"pl", 852}, {"prs", 720},
	{"ro", 852}, {"ru", 866}, {"rw", 437}, {"sah", 866}, {"sk", 852},
	{"sl", 852}, {"sq", 852}, {"sw", 437}, {"tg", 866}, {"th", 874},
	{"tk", 852}, {"tr", 857}, {"tt", 866}, {"ug", 720}, {"uk", 866},
	{"ur", 720}, {"vi", 1258}, {"yo", 437},
	{"en_au", 850}, {"en_bz", 850}, {"en_ca", 850}, {"en_gb", 850},
	{"en_ie", 850}, {"en_in", 437}, {"en_jm", 850}, {"en_my", 437},
	{"en_nz", 850}, {"en_ph", 437}, {"en_sg", 437}, {"en_tt", 850},
	{"en_us", 437}, {"en_za", 437}, {"en_zw", 437}, {"zh_cn", 936},
	{"zh_hk", 950}, {"zh_mo", 950}, {"zh_sg", 936}, {"zh_tw", 950},
	{"az@cyrillic", 866}, {"az@latin", 857}, {"bs@cyrillic", 855},
	{"bs@latin", 852}, {"sr@cyrillic", 855}, {"sr@latin", 852},
	{"uz@cyrillic", 866}, {"uz@latin", 857},
	{"az", 857}, {"en", 437}, {"sr", 855}, {"uz", 857},
    };

    const char *lang;
    unsigned i;

    lang = getenv("LC_ALL");
    if(!lang) {
	lang = getenv("LC_CTYPE");
	if(!lang) {
	    lang = getenv("LANG");
	    if (!lang)
		lang = "";
	}
    }

    static const char fields[] = "_@.";

    for(i = 0; i != sizeof(code_map) / sizeof(*code_map); i++) {
	const char *field = fields;
	const char *p_lang = lang, *p_code = code_map[i].lang;

	for(;;) {
	    const char *n_lang = strchr(field, *p_lang);
	    const char *n_code = strchr(field, *p_code);

	    if ((n_lang || !*p_lang) && (n_code || !*p_code)) {
		if (!*p_code)
		    return code_map[i].code_page;
		else if (!*p_lang)
		    break;
		else if (n_lang > n_code) {
		    break;
		} else if (n_lang < n_code) {
		    while (*p_lang && !strchr(n_code, *p_lang))
			p_lang++;
		} else {
		    p_code++;
		    p_lang++;
		}
	    } else if (!n_lang && !n_code) {
		char ch_lang = *p_lang;
		if (ch_lang >= 'A' && ch_lang <= 'Z')
		    ch_lang = ch_lang - 'A' + 'a';
		if (ch_lang != *p_code)
		    break;
		p_lang++;
		p_code++;
	    } else {
		break;
	    }
	}
    }

    return 850;
}
