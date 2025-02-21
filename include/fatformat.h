#ifndef _MKFAT_FATFORMAT
#define _MKFAT_FATFORMAT

#include <stdint.h>
#include <string.h>

int formatFATChar(int c);

void formatFATName(const char *name, char *dest, size_t max);

#endif
