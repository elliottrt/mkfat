#include "fattable.h"
#include "logging.h"
#include "fatentry.h"

#define CLN_SHUT_BITMASK 0x8000000
#define HRD_ERR_BITMAS 0x04000000

#define CLN_SHUT_BYTE 0x80
#define HRD_ERR_BYTE 0x40

void FATTable::generateFAT32_recursive(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster)
{
	size_t itemSize = item->size();

	// See https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
	ssize_t entryCount = (itemSize + (bytesPerCluster - 1)) / bytesPerCluster;
	entryCount--; // remove EOC, we'll do that manually

	mkfatVerbose("Writing fatentry '%.8s' '%.3s', size: %zd\n", item->direntry.fileName, item->direntry.fileExtension, entryCount + 1);

	if (itemSize == 0) 
		return;

	item->direntry.firstClusterLo = *cluster & 0xFFFF;
	item->direntry.firstClusterHi = *cluster >> 16;

	if (item->dot && item->dotdot && item->parent)
	{
		item->dot->direntry.firstClusterLo = item->direntry.firstClusterLo;
		item->dot->direntry.firstClusterHi = item->direntry.firstClusterHi;
		item->dotdot->direntry.firstClusterLo = item->parent->direntry.firstClusterLo;
		item->dotdot->direntry.firstClusterHi = item->parent->direntry.firstClusterHi;
	}

	if (item->dotdot && item->parent && !item->parent->parent)
	{
		item->dotdot->direntry.firstClusterLo = 0;
		item->dotdot->direntry.firstClusterHi = 0;
	}

	while (entryCount--)
	{
		FATEntry32(*cluster + 1);
		*cluster = *cluster + 1;
	}

	FATEntry32(ENDCLUSTER32);
	*cluster = *cluster + 1;

	
	if (item->is_directory())
	{
		for (TreeItem *child : item->children)
			generateFAT32_recursive(child, cluster, bytesPerCluster);
	}

}

void FATTable::generateFAT32(void)
{
	FATEntry32(MEDIA_TYPE, 0xFF, 0xFF, 0xFF);
	FATEntry32NoAnd(0xFF, 0xFF, 0xFF, 0x0F | HRD_ERR_BYTE | CLN_SHUT_BYTE);

	uint32_t cluster = entryCount;
	size_t bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;
	generateFAT32_recursive(this->tree.root, &cluster, bytesPerCluster);
}

void FATTable::FATEntry32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	uint8_t bytes[4] = {b0, b1, b2, (uint8_t) (b3 & 0x0F)};

	tableWrite(bytes, 4);
	entryCount++;
}

void FATTable::FATEntry32NoAnd(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	uint8_t bytes[4] = {b0, b1, b2, b3};

	tableWrite(bytes, 4);
	entryCount++;
}

void FATTable::FATEntry32(uint32_t entry)
{
	uint8_t *bytes = (uint8_t *) &entry;
	bytes[3] &= 0x0F;

	tableWrite(bytes, 4);
	entryCount++;
}
