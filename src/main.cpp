
#include "common.h"
#include "fattype.h"
#include "filetree.h"
#include "diskimage.h"
#include "reserved.h"
#include "fattable.h"
#include "fatdata.h"
#include "error.h"

#include <cstdlib>
#include <string>

void usage(void)
{
	fprintf(stderr, "Usage:\n./mkfat <12,16,32> <outputfile> [-Srootdir] [-Bbootfile] [-Rreservedfile] [-Vvolumelabel]\n");
	exit(EXIT_FAILURE);
}

const char *findArg(int argc, char **argv, const char *argName)
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

const char *findArgOrDefault(int argc, char **argv, const char *argName, const char *def) {
	const char *findArgResult = findArg(argc, argv, argName);

	return findArgResult ? findArgResult : def;
}

FatType fatTypeOrFail(const char *str) {
	std::string fatTypeString = str;

	if (fatTypeString == "32") return FatType::FAT32;
	else if (fatTypeString == "16") return FatType::FAT16;
	else if (fatTypeString == "12") return FatType::FAT12;
	else mkfatError(1, "invalid fat type '%s', must be one of: 12, 16, 32\n", fatTypeString.c_str());
}

int main(int argc, char **argv)
{
	if (argc < 3)
		usage();

	// TODO: testing - can use 'hdiutil attach <fs.img>' to attach and 'hdiutil detach <disk thing>' to mount and unmount
	// TODO: 'hdiutil imageinfo /dev/<whatever>' and 'diskutil'

	// TODO: make all of the output that mkfat generates an option which is disabled by default

	// TODO: namespace

	FatType fatType = fatTypeOrFail(argv[1]);
	std::string outputPath = std::string(argv[2]);

	std::string rootDirectory = findArgOrDefault(argc, argv, "-S", "");
	std::string fatBootSectorPath = findArgOrDefault(argc, argv, "-B", "");
	std::string reservedPath = findArgOrDefault(argc, argv, "-R", "");
	std::string volumeLabel = findArgOrDefault(argc, argv, "-V", VOLUMELABEL);

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
