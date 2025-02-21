#include "common.h"
#include "fatformat.h"
#include "error.h"

#include <cerrno>

direntry::direntry(void)
{
	memset(this, 0, sizeof(struct direntry));
	memset(this->fileName, ' ', DIRENTRY_NAMELEN);
	memset(this->fileExtension, ' ', DIRENTRY_EXTLEN);
}

void direntry::setFileName(const char *name)
{

	// if this is a dot or dotdot entry handle it
	if (strcmp(name, ".") == 0)
	{
		memcpy(this->fileName, name, 1);
		return;
	}
	else if (strcmp(name, "..") == 0)
	{
		memcpy(this->fileName, name, 2);
		return;
	}

	// try to find a . that begins the extension
	const char *dotLocation = strchr(name, '.');
	size_t nameLength = strlen(name);

	if (dotLocation)
	{
		// if there's a second . then this filename is bad and we don't want to deal with it
		if (strchr(dotLocation + 1, '.'))
			mkfatError(1, "bad filename '%s': contains multiple .\n", name);

		formatFATName(dotLocation + 1, this->fileExtension, DIRENTRY_EXTLEN);
		nameLength -= strlen(dotLocation);
	}
	else
	{
		memset(this->fileExtension, ' ', DIRENTRY_EXTLEN);
	}

	formatFATName(name, this->fileName, min(DIRENTRY_NAMELEN, nameLength));
}

void fileRead(const char *filename, void *out, size_t size)
{
	FILE *file = fopen(filename, "rb");

	if (!file)
	{
		mkfatError(1,
			"unable to open file '%s': %s\n",
			filename, strerror(errno)
		);
	}

	size_t bytesRead = fread(out, 1, size, file);

	if (bytesRead != size)
	{
		mkfatError(1,
			"unable to read file '%s': %s\n", filename,
			feof(file) ? "reached EOF" : "file error"
		);
	}

	// we don't care about the exit code, since we aren't writing
	fclose(file);
}

size_t fileSize(const char *filename)
{
	FILE *file = fopen(filename, "rb");

	if (!file)
	{
		mkfatError(1,
			"unable to open file '%s': %s\n",
			filename, strerror(errno)
		);
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);

	fclose(file);

	return size;
}
