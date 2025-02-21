#ifndef _MKFAT_FAT
#define _MKFAT_FAT

#include "diskimage.h"
#include "filetree.h"
#include "bootsector.h"
#include "fattype.h"

#include <cstdint>

class FATTable
{

	const FileTree &tree;
	const FatType fatType;
	const FATBootSector &bootSector;

	// pointer to FAT table in memory
	void *fatTable;
	// number of entries put into the FAT
	size_t entryCount;
	// number of bytes put into the FAT
	size_t fatTableBytesUsed;

	// write n bytes from data into fatTable
	void tableWrite(const void *data, size_t n);

	void write12(FATDiskImage &image) const;
	void generateFAT12(void);
	void generateFAT12_recursive(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster, uint16_t rootEntryClusters);

	void write16(FATDiskImage &image) const;
	void generateFAT16(void);
	void generateFAT16_recursive(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster, uint16_t rootEntryClusters);

	void write32(FATDiskImage &image) const;
	void generateFAT32(void);
	void generateFAT32_recursive(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster);

	// write FAT32 entry into FAT
	void FATEntry32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
	void FATEntry32NoAnd(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
	void FATEntry32(uint32_t entry);

	// write FAT16 entry into FAT
	void FATEntry16(uint8_t b0, uint8_t b1);
	void FATEntry16(uint16_t entry);

	// write FAT12 entry into FAT
	void FATEntry12(uint8_t b0, uint8_t b1);
	void FATEntry12(uint16_t entry);
	void FATEntry12Flush(void);

	// because FAT12 has interleaved entries,
	// we need to write two at a time
	uint16_t delayedEntry;
	bool entryWaiting;

public:

	FATTable(const FileTree &tree, const FATBootSector &bootSector, FatType fatType);
	~FATTable(void);

	// returns the size of the FAT in sectors
	size_t fatSize(void) const;
	// returns the size of the FAT in bytes
	size_t fatSizeBytes(void) const;

	void write_to(FATDiskImage &image) const;

};

#endif
