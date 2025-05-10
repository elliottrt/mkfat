
#include "common.h"
#include "fattype.h"
#include "filetree.h"
#include "diskimage.h"
#include "reserved.h"
#include "fattable.h"
#include "fatdata.h"
#include "logging.h"
#include "args.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

void usage(void)
{
	puts("Usage:");
	puts("  mkfat <12,16,32> <output> [-v] [-s ROOT] [-b BOOT] [-r RESERVED] [-l LABEL]");
	puts("");
	puts("options:");
	puts("  -v/--verbose         display extra information");
	puts("  -s ROOT              specify path to folder to use as root");
	puts("  -b BOOT              specify path to file containing bootsector");
	puts("  -r RESERVED          specify path to file with reserved section, FAT32 only");
	puts("  -l LABEL             set volume label");
}

FatType fatTypeOrFail(const char *str)
{
	std::string fatTypeString = str;

	if (fatTypeString == "32") return FatType::FAT32;
	else if (fatTypeString == "16") return FatType::FAT16;
	else if (fatTypeString == "12") return FatType::FAT12;
	else mkfatError(1, "invalid fat type '%s', must be one of: 12, 16, 32\n", fatTypeString.c_str());
}

int main(int argc, const char **argv)
{

	// TODO: make output an option and set a default
	// TODO: for fat12 and fat16, check that root dir region is large enough
	// TODO: testing - can use 'hdiutil attach <fs.img>' to attach and 'hdiutil detach <disk thing>' to mount and unmount
	// TODO: 'hdiutil imageinfo /dev/<whatever>' and 'diskutil'
	// TODO: use file <image name> to test
	// TODO: namespace
	// TODO: error on unknown option
	// TODO: long filename support?

	// get command line arguments

	std::string rootDirectory = findArgValOrDefault(argc, argv, "-s", "");
	std::string fatBootSectorPath = findArgValOrDefault(argc, argv, "-b", "");
	std::string reservedPath = findArgValOrDefault(argc, argv, "-r", "");
	std::string volumeLabel = findArgValOrDefault(argc, argv, "-l", VOLUMELABEL);
	bool verbose = hasArg(argc, argv, "-v") || hasArg(argc, argv, "--verbose");
	bool help = hasArg(argc, argv, "-h") || hasArg(argc, argv, "--help");

	mkfatSetVerbose(verbose);

	if (help)
	{
        usage();
        return EXIT_SUCCESS;
	}
	else if (argc < 3)
	{
	    usage();
		return EXIT_FAILURE;
	}

	FatType fatType = fatTypeOrFail(argv[1]);
	std::string outputPath = std::string(argv[2]);

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
