#include "args.h"
#include "logging.h"

#include <cstring>

bool hasArg(int argc, const char **argv, const char *argName)
{
	for (int arg = 0; arg < argc; arg++)
	{
		if (strcmp(argName, argv[arg]) == 0)
			return true;
	}
	return false;
}

const char *findArgVal(int argc, const char **argv, const char *argName)
{
	for (int arg = 0; arg < argc; arg++)
	{
		const char *result = strstr(argv[arg], argName);
		if (result && result == argv[arg])
		{
			if (strlen(argv[arg]) != strlen(argName))
				return argv[arg] + strlen(argName);
			else if (arg + 1 < argc)
				return argv[arg + 1];
			else
				mkfatError(1, "argument '%s' has no value\n", argName);
		}
	}
	return NULL;
}

const char *findArgValOrDefault(int argc, const char **argv, const char *argName, const char *def)
{
	const char *findArgValResult = findArgVal(argc, argv, argName);

	return findArgValResult ? findArgValResult : def;
}
