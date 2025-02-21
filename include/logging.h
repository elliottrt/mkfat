#ifndef _MKFAT_ERROR
#define _MKFAT_ERROR

// should we show verbose logs
void mkfatSetVerbose(bool verbose);

void mkfatVerbose(const char *format, ...);

void mkfatWarn(const char *format, ...);

void mkfatError [[noreturn]] (int exitCode, const char *format, ...);

#endif
