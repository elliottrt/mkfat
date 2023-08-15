#include "fatformat.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char FAT_VALID_FILENAME_CHARS[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!#$%&'()-@^_`{}~";
#define FAT_DEFAULT_FILENAME_CHAR '_'
#define FAT_DEFAULT_FILENAME_PADDING ' '

// See https://en.wikipedia.org/wiki/8.3_filename

int formatFATChar(int c)
{
	char *valid = strchr(FAT_VALID_FILENAME_CHARS, c);

	// if it's valid, return it
	if (valid || c >= 128)
		return c;

	if (islower(c))
		return toupper(c);

	if (c == 0xE5)
		return 0x05;

	return FAT_DEFAULT_FILENAME_CHAR;

}

void formatFATName(const char *name, char *dest, size_t max)
{

	if (dest == NULL)
	{
		fprintf(stderr, "Null destination string in formatting\n");
		//exit(1);
		return;
	}

	memset(dest, FAT_DEFAULT_FILENAME_PADDING, max);

	if (name == NULL)
	{
		fprintf(stderr, "Null source string in formatting\n");
		//exit(1);
		return;
	}

	while (name[0] == ' ') 
		name++;

	while(*name && max--)
	{
		*dest = formatFATChar(*name);

		dest++;
		name++;
	}

}
