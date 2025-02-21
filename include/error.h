#ifndef _MKFAT_ERROR
#define _MKFAT_ERROR

void mkfatError [[noreturn]] (int exitCode, const char *format, ...);

#endif
