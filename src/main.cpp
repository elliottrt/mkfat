
#include "filetree.h"
#include "diskimage.h"
#include "reserved.h"
#include "fattable.h"
#include "fatdata.h"
#include "error.h"

#include <cstdlib>

void usage(void)
{
	fprintf(stderr, "Usage:\n./mkfat <12,16,32> <outputfile> [-Srootdir] [-Bbootfile] [-Rreservedfile] [-Vvolumelabel]\n");
	exit(EXIT_FAILURE);
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

	// TODO: testing - can use 'hdiutil attach <fs.img>' to attach and 'hdiutil detach <disk thing>' to mount and unmount
	// TODO: 'hdiutil imageinfo /dev/<whatever>' and 'diskutil'

	// TODO: make all of the output that mkfat generates an option which is disabled by default

	// TODO: namespace

	// TODO: use enumeration for fatType instead of string

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
		mkfatError(1, "invalid fat type '%s', must be one of: 12, 16, 32\n", fatType.c_str());
	}

	FATReserved fatReserved = FATReserved(fatBootSectorPath, reservedPath, fatType, volumeLabel);
	FATDiskImage fatDiskImage = FATDiskImage(outputPath, fatReserved.bootSector.bytesPerSector);

	FileTree fileTree = FileTree(rootDirectory, volumeLabel);
	fileTree.collect();

	FATTable fatTable = FATTable(fileTree, fatReserved.bootSector, fatType);
	FATData fatData = FATData(fileTree, fatReserved.bootSector, fatType);

	// TODO: i don't like this, because it might leave a partially constructed file if there is an error
	fatDiskImage.createImgFile();
	fatReserved.write(fatDiskImage);
	fatTable.write(fatDiskImage);
	fatData.write(fatDiskImage);
	fatDiskImage.closeImgFile();

	return EXIT_SUCCESS;
}
