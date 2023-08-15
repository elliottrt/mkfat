#ifndef _MKFAT_FATDATA
#define _MKFAT_FATDATA

#include "writeable.h"
#include "filetree.h"
#include "bootsector.h"

class FATData : public FATWriteable
{
public:

	FileTree *tree;
	FATBootSector *bootSector;
	std::string fatType;
	size_t bytesPerCluster;

	void writeDirectory(TreeItem *item, FATDiskImage *image, size_t occupiedBytes) const;
	void writeFile(TreeItem *item, FATDiskImage *image, size_t occupiedBytes) const;

	void write16Recursive(TreeItem *item, FATDiskImage *image, size_t rootEntries) const;
	void write32Recursive(TreeItem *item, FATDiskImage *image) const;

	void write12(FATDiskImage *image) const;
	void write16(FATDiskImage *image) const;
	void write32(FATDiskImage *image) const;

	FATData(FileTree *tree, FATBootSector *bootSector, const std::string &fatType);

	void write(FATDiskImage *image) const;

	void padZeros(FATDiskImage *image, size_t diskSize) const;

};

#endif
