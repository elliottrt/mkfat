#ifndef _MKFAT_FAT
#define _MKFAT_FAT

#include "diskimage.h"
#include "filetree.h"
#include "bootsector.h"
#include "fattype.h"

class FATTable
{

	const FileTree &tree;
	const FatType fatType;
	const FATBootSector &bootSector;

	void write12(FATDiskImage &image) const;
	void write16(FATDiskImage &image) const;
	void write32(FATDiskImage &image) const;

public:

	FATTable(const FileTree &tree, const FATBootSector &bootSector, FatType fatType);

	// returns the size of the FAT in sectors
	size_t fatSize(void) const;
	// returns the size of the FAT in bytes
	size_t fatSizeBytes(void) const;

	void write_to(FATDiskImage &image) const;

};

#endif
