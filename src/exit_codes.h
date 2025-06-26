#ifndef _EXIT_CODES_H
#define _EXIT_CODES_H

/* Codes as defined by fsck.
   For more information, see fsck manpage. */
#define NO_ERRORS                   0
#define FS_ERRORS_CORRECTED         1
#define SYSTEM_SHOULD_BE_REBOOTED   2
#define FS_ERRORS_LEFT_UNCORRECTED  4
#define OPERATIONAL_ERROR           8
#define USAGE_OR_SYNTAX_ERROR       16
#define CHECKING_CANCELED_BY_USER   32
#define SHARED_LIB_ERROR            128

#endif
