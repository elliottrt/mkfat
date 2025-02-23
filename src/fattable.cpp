#include "fattable.h"
#include "fattype.h"
#include "logging.h"

#include <cstdlib>
#include <cstring>

void FATTable::tableWrite(const void *data, size_t n)
{
	if (fatTableBytesUsed + n > fatSizeBytes())
	{
		mkfatError(1, "fat (of %zu entries) not large enough\n", fatSizeEntries());
	}

	memcpy(
		(uint8_t *) fatTable + fatTableBytesUsed,
		data,
		n
	);

	fatTableBytesUsed += n;
}

FATTable::FATTable(const FileTree &tree, const FATBootSector &bootSector, FatType fatType):
	tree(tree), fatType(fatType), bootSector(bootSector), entryCount(0), fatTableBytesUsed(0), delayedEntry(0), entryWaiting(false)
{

	fatTable = calloc(fatSizeBytes(), 1);

	if (fatTable == NULL)
	{
		mkfatError(1, "unable to allocate memory for FAT\n");
	}

	switch (fatType)
	{
		case FatType::FAT32: generateFAT32(); break;
		case FatType::FAT16: generateFAT16(); break;
		case FatType::FAT12: generateFAT12(); break;
	}
}

FATTable::~FATTable(void)
{
	free(fatTable);
}

size_t FATTable::fatSize(void) const {
	if (fatType == FatType::FAT32) 
	{
		return bootSector.biosParamBlock.fat32.fatSize32;
	} 
	else 
	{
		return bootSector.fatSize16;
	}
}

size_t FATTable::fatSizeBytes(void) const {
	return fatSize() * bootSector.bytesPerSector;
}

size_t FATTable::fatSizeEntries(void) const {
	switch (fatType)
	{
		case FatType::FAT12: return (fatSizeBytes() * 3) / 2;
		case FatType::FAT16: return fatSizeBytes() / 2;
		case FatType::FAT32: return fatSizeBytes() / 4;
	}
}

void FATTable::write_to(FATDiskImage &image) const
{
	for (size_t i = 0; i < bootSector.fatCount; i++)
	{
		image.writeImgFile(fatTable, fatSizeBytes());
	}
}
