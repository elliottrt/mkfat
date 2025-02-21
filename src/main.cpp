
#include "common.h"
#include "fattype.h"
#include "filetree.h"
#include "diskimage.h"
#include "reserved.h"
#include "fattable.h"
#include "fatdata.h"
#include "logging.h"

#include <cstdlib>
#include <cstring>
#include <string>

void usage(void)
{
	fprintf(stderr, "Usage:\n./mkfat <12,16,32> <outputfile> [-Srootdir] [-Bbootfile] [-Rreservedfile] [-Vvolumelabel] [-v|--verbose]\n");
	exit(EXIT_FAILURE);
}

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

const char *findArgValOrDefault(int argc, const char **argv, const char *argName, const char *def) {
	const char *findArgValResult = findArgVal(argc, argv, argName);

	return findArgValResult ? findArgValResult : def;
}

FatType fatTypeOrFail(const char *str) {
	std::string fatTypeString = str;

	if (fatTypeString == "32") return FatType::FAT32;
	else if (fatTypeString == "16") return FatType::FAT16;
	else if (fatTypeString == "12") return FatType::FAT12;
	else mkfatError(1, "invalid fat type '%s', must be one of: 12, 16, 32\n", fatTypeString.c_str());
}

int main(int argc, const char **argv)
{
	if (argc < 3)
		usage();

	// TODO: testing - can use 'hdiutil attach <fs.img>' to attach and 'hdiutil detach <disk thing>' to mount and unmount
	// TODO: 'hdiutil imageinfo /dev/<whatever>' and 'diskutil'

	// TODO: namespace

	// TODO: after collecting FileTree, do a duplicate file check - two files in the same dir shouldn't have the same name

	// get command line arguments

	FatType fatType = fatTypeOrFail(argv[1]);
	std::string outputPath = std::string(argv[2]);

	std::string rootDirectory = findArgValOrDefault(argc, argv, "-S", "");
	std::string fatBootSectorPath = findArgValOrDefault(argc, argv, "-B", "");
	std::string reservedPath = findArgValOrDefault(argc, argv, "-R", "");
	std::string volumeLabel = findArgValOrDefault(argc, argv, "-V", VOLUMELABEL);
	bool verbose = hasArg(argc, argv, "-v") || hasArg(argc, argv, "--verbose");

	mkfatSetVerbose(verbose);

	// initialize FAT data structures

	FileTree fileTree = FileTree(rootDirectory, volumeLabel);
	fileTree.collect();

	FATReserved fatReserved = FATReserved(fatBootSectorPath, reservedPath, fatType, volumeLabel);
	FATDiskImage fatDiskImage = FATDiskImage(outputPath, fatReserved.bootSector.bytesPerSector);
	FATTable fatTable = FATTable(fileTree, fatReserved.bootSector, fatType);
	FATData fatData = FATData(fileTree, fatReserved.bootSector, fatType);

	// write out information to the file

	// TODO: i don't like this, because it might leave a partially constructed file if there is an error
	fatDiskImage.createImgFile();
	fatReserved.write_to(fatDiskImage);
	fatTable.write_to(fatDiskImage);
	fatData.write_to(fatDiskImage);
	fatDiskImage.closeImgFile();

	return EXIT_SUCCESS;
}
