#ifndef _MKFAT_FAT
#define _MKFAT_FAT

#include "writeable.h"
#include "filetree.h"
#include "bootsector.h"

class FATTable : public FATWriteable
{

	const FileTree &tree;
	std::string fatType;
	const FATBootSector &bootSector;

	void write12(FATDiskImage &image) const;
	void write16(FATDiskImage &image) const;
	void write32(FATDiskImage &image) const;

public:

	FATTable(const FileTree &tree, const FATBootSector &bootSector, const std::string &fatType);

	void write(FATDiskImage &image) const override;

};

#endif
