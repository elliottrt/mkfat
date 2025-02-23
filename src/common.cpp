#include "common.h"
#include "fatformat.h"
#include "logging.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>

direntry::direntry(void)
{
	memset(this, 0, sizeof(struct direntry));
	memset(this->fileName, ' ', DIRENTRY_NAMELEN);
	memset(this->fileExtension, ' ', DIRENTRY_EXTLEN);
}

void direntry::setFileName(const std::string &name)
{

	// if this is a dot or dotdot entry handle it
	if (name == ".")
	{
		this->fileName[0] = '.';
	}
	else if (name == "..")
	{
		this->fileName[0] = '.';
		this->fileName[1] = '.';
	}
	else
	{
		// try to find a . that begins the extension
		size_t dotLocation = name.find('.');

		// if the filename has an extension, format it
		if (dotLocation != std::string::npos)
		{
			// if there's a second . then this filename is bad and we don't want to deal with it
			if (name.find('.', dotLocation + 1) != std::string::npos)
			{
				mkfatError(1, "bad filename '%s': contains multiple .\n", name.c_str());
			}

			const std::string ext_substr = name.substr(dotLocation + 1);

			formatFATName(
				ext_substr.c_str(), 
				this->fileExtension, 
				std::min(size_t(DIRENTRY_EXTLEN), ext_substr.length())
			);
		}

		// format the name of the file without the extension
		const std::string name_substr = name.substr(0, dotLocation);

		formatFATName(
			name_substr.c_str(),
			this->fileName,
			std::min(size_t(DIRENTRY_NAMELEN), name_substr.length())
		);
	}
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
