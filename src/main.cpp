
#include "filetree.h"
#include "diskimage.h"
#include "reserved.h"
#include "fattable.h"
#include "fatdata.h"

#include <stdlib.h>

void usage(void)
{
	printf("Usage:\n./mkfat <12,16,32> <outputfile> [-Srootdir] [-Bbootfile] [-Rreservedfile] [-Vvolumelabel]\n");
	exit(1);
}

char *findArgument(int argc, char **argv, const char *argName)
{
	for (int arg = 0; arg < argc; arg++)
	{
		char *result = strstr(argv[arg], argName);
		if (result && result == argv[arg])
		{
			if (strlen(argv[arg]) != strlen(argName))
				return argv[arg] + strlen(argName);
			else if (arg + 1 < argc)
				return argv[arg + 1];
			else
				printf("Empty argument '%s'\n", argv[arg]);
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc < 3)
		usage();

	std::string fatType = std::string(argv[1]);
	std::string outputPath = std::string(argv[2]);

	char *rootDirectoryArg = findArgument(argc, argv, "-S");
	char *fatBootSectorPathArg = findArgument(argc, argv, "-B");
	char *reservedPathArg = findArgument(argc, argv, "-R");
	char *volumeLabelArg = findArgument(argc, argv, "-V");

	std::string rootDirectory = rootDirectoryArg ? rootDirectoryArg : "";
	std::string fatBootSectorPath = fatBootSectorPathArg ? fatBootSectorPathArg : "";
	std::string reservedPath = reservedPathArg ? reservedPathArg : "";
	std::string volumeLabel = volumeLabelArg ? volumeLabelArg : VOLUMELABEL;

	if (fatType != "12" && fatType != "16" && fatType != "32")
	{
		fprintf(stderr, "Invalid fat type '%s', must be 12, 16, or 32.\n", fatType.c_str());
		exit(1);
	}

	FATReserved fres = FATReserved(fatBootSectorPath, reservedPath, fatType, volumeLabel);
	FATDiskImage fdi = FATDiskImage(outputPath, fres.bootSector.bytesPerSector);

	FileTree ft = FileTree(rootDirectory, volumeLabel);
	ft.collect();

	FATTable fatTable = FATTable(&ft, &fres.bootSector, fatType);
	FATData fatData = FATData(&ft, &fres.bootSector, fatType);

	fdi.createImgFile();
	fdi.writeImgFile(&fres);
	fdi.writeImgFile(&fatTable);
	fdi.writeImgFile(&fatData);
	fdi.closeImgFile();

}
