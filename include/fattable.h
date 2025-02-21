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

	void write_to(FATDiskImage &image) const;

};

#endif
