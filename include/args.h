#ifndef _MKFAT_ARGS
#define _MKFAT_ARGS

bool hasArg(int argc, const char **argv, const char *argName);

const char *findArgVal(int argc, const char **argv, const char *argName);

const char *findArgValOrDefault(int argc, const char **argv, const char *argName, const char *def);

#endif
