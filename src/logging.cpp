#include "logging.h"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

bool _verbose = false;

void mkfatSetVerbose(bool verbose) {
	_verbose = verbose;
}

void mkfatVerbose(const char *format, ...) {
	if (!_verbose) return;

	va_list ap;
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
}

void mkfatError(int exitCode, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	fprintf(stderr, "mkfat: error: ");
	vfprintf(stderr, format, ap);
	va_end(ap);
	exit(exitCode);
}
