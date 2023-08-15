#include "common.h"
#include "fatformat.h"

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
		formatFATName(dotLocation + 1, this->fileExtension, DIRENTRY_EXTLEN);
		nameLength -= strlen(dotLocation);
	}
	else
	{
		memset(this->fileExtension, ' ', DIRENTRY_EXTLEN);
	}

	formatFATName(name, this->fileName, min(DIRENTRY_NAMELEN, nameLength));
	
	/*
	fputs("'", stdout);
	fwrite(this->filename, 1, 8, stdout);
	fputs("' '", stdout);
	fwrite(this->fileext, 1, 3, stdout);
	fputs("'\n", stdout);
	*/
}

void fileRead(FILE *file, void *out, size_t size)
{
	size_t read = fread(out, 1, size, file);
	if (read != size)
	{
		fprintf(stderr, "Problem reading bootsector, only %lu/%lu byte(s) read.\n", read, size);
		if (feof(file))
		{
			fprintf(stderr, "Problem: Reached EOF\n");
		}
		if (ferror(file))
		{
			perror("Problem");
		}
		exit(1);
	}
}

size_t fileSize(FILE *f)
{
	size_t pos = ftell(f);

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	fseek(f, pos, SEEK_SET);

	return size;
}
