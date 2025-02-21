#ifndef _MKFAT_FATDATA
#define _MKFAT_FATDATA

#include "writeable.h"
#include "filetree.h"
#include "bootsector.h"

class FATData : public FATWriteable
{
public:

	const FileTree &tree;
	const FATBootSector &bootSector;
	std::string fatType;
	size_t bytesPerCluster;

	void writeDirectory(const TreeItem *item, FATDiskImage &image, size_t occupiedBytes) const;
	void writeFile(const TreeItem *item, FATDiskImage &image, size_t occupiedBytes) const;

	void write12Recursive(const TreeItem *item, FATDiskImage &image, size_t rootEntries) const;
	void write16Recursive(const TreeItem *item, FATDiskImage &image, size_t rootEntries) const;
	void write32Recursive(const TreeItem *item, FATDiskImage &image) const;

	void write12(FATDiskImage &image) const;
	void write16(FATDiskImage &image) const;
	void write32(FATDiskImage &image) const;

	FATData(const FileTree &tree, const FATBootSector &bootSector, const std::string &fatType);

	void write(FATDiskImage &image) const override;

	void padZeros(FATDiskImage &image, size_t diskSize) const;

};

#endif
