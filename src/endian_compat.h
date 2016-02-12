#ifndef ENDIAN_COMPAT_H
#define ENDIAN_COMPAT_H

#if defined(HAVE_ENDIAN_H)
#include <endian.h>
#elif defined(HAVE_SYS_ENDIAN_H)
#include <sys/endian.h>
#else
#error No endian.h available and no fallback code
#endif

#endif
