#include "error.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

void mkfatError(int exitCode, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	fprintf(stderr, "mkfat: error: ");
	vfprintf(stderr, format, ap);
	va_end(ap);
	exit(exitCode);
}
