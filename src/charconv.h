#ifndef _CHARCONV_H
#define _CHARCONV_H

#define DEFAULT_DOS_CODEPAGE 850

int set_dos_codepage(int codepage);
int dos_char_to_printable(char **p, unsigned char c);
int local_string_to_dos_string(char *out, char *in, unsigned int len);

#endif
