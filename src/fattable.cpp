#include "fattable.h"
#include "fatentry.h"
#include "fattype.h"
#include "logging.h"
#include <cstdio>

#define CLN_SHUT_BITMASK 0x8000000
#define HRD_ERR_BITMAS 0x04000000

#define CLN_SHUT_BYTE 0x80
#define HRD_ERR_BYTE 0x40

// TODO: instead of writing each entry individually, and then repeating for each fat, we should write once to a buffer in memory then write that buffer the required number of times

// TODO: make rootEntryClusters a boolean to of whether the current dir is the root
void _FATEntry_write12(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster, uint16_t rootEntryClusters)
{
	size_t itemSize = item->size();

	// See https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
	ssize_t entryCount = (itemSize + (bytesPerCluster - 1)) / bytesPerCluster;
	entryCount--; // remove EOC, we'll do that manually

	mkfatVerbose("Writing fatentry '%.8s' '%.3s', size: %zd\n", item->direntry.fileName, item->direntry.fileExtension, entryCount + 1);

	if (itemSize == 0) 
		return;

	if (rootEntryClusters == 0)
	{
		item->direntry.firstClusterLo = *cluster & 0x0FFF;
		item->direntry.firstClusterHi = 0;

		if (item->dot && item->dotdot && item->parent)
		{
			item->dot->direntry.firstClusterLo = item->direntry.firstClusterLo;
			item->dot->direntry.firstClusterHi = 0;
			item->dotdot->direntry.firstClusterLo = item->parent->direntry.firstClusterLo;
			item->dotdot->direntry.firstClusterHi = 0;
		}

		// special case where .. entries in folders in the root have firstCluster = 0
		if (item->dotdot && item->parent && !item->parent->parent)
		{
			item->dotdot->direntry.firstClusterLo = 0;
			item->dotdot->direntry.firstClusterHi = 0;
		}

		while (entryCount--)
		{
			*cluster = *cluster + 1;
			FATEntry12(*cluster);
		}

		FATEntry12(ENDCLUSTER12);
		*cluster = *cluster + 1;
	}

	if (item->is_directory())
	{
		for (TreeItem *child : item->children)
			_FATEntry_write12(child, cluster, bytesPerCluster, 0);
	}

}

void FATTable::write12(FATDiskImage &image) const
{
	FATEntryDiskImage(image);

	for (size_t fat = 0; fat < this->bootSector.fatCount; fat++)
	{
		FATEntryResetCount();
		FATEntry12(this->bootSector.mediaDescriptor, 0xFF);
		FATEntry12(0xFF, 0xFF);

		uint32_t cluster = FATEntryGetCount();
		size_t bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;
		size_t rootEntryClusters = ((this->bootSector.rootEntryCount * sizeof(struct direntry)) + 
									(bytesPerCluster - 1)) / bytesPerCluster;

		_FATEntry_write12(this->tree.root, &cluster, bytesPerCluster, rootEntryClusters);

		FATEntry12Flush();

		size_t bytesWritten = (FATEntryGetCount() * 3) / 2;

		if (bytesWritten > fatSizeBytes()) {
			mkfatError(1, "FAT too large; takes up %zu bytes but only have %zu\n", bytesWritten, fatSizeBytes());
		}

		size_t bytesLeft = this->bootSector.fatSize16 * this->bootSector.bytesPerSector - bytesWritten;

		image.writeImgFileZeros(bytesLeft);
	}
}

void _FATEntry_write16(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster, uint16_t rootEntryClusters)
{
	size_t itemSize = item->size();

	// See https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
	ssize_t entryCount = (itemSize + (bytesPerCluster - 1)) / bytesPerCluster;

	entryCount--; // remove EOC, we'll do that manually

	mkfatVerbose("Writing fatentry '%.8s' '%.3s', size: %zd\n", item->direntry.fileName, item->direntry.fileExtension, entryCount + 1);

	if (itemSize == 0) 
		return;

	if (rootEntryClusters == 0)
	{
		item->direntry.firstClusterLo = *cluster & 0xFFFF;
		item->direntry.firstClusterHi = 0;

		if (item->dot && item->dotdot && item->parent)
		{
			item->dot->direntry.firstClusterLo = item->direntry.firstClusterLo;
			item->dot->direntry.firstClusterHi = 0;
			item->dotdot->direntry.firstClusterLo = item->parent->direntry.firstClusterLo;
			item->dotdot->direntry.firstClusterHi = 0;
		}

		if (item->dotdot && item->parent && !item->parent->parent)
		{
			item->dotdot->direntry.firstClusterLo = 0;
			item->dotdot->direntry.firstClusterHi = 0;
		}

		while (entryCount--)
		{
			*cluster = *cluster + 1;
			FATEntry16(*cluster);
		}

		FATEntry16(ENDCLUSTER16);
		*cluster = *cluster + 1;
	}

	if (item->is_directory())
	{
		for (TreeItem *child : item->children)
			_FATEntry_write16(child, cluster, bytesPerCluster, 0);
	}

}

void FATTable::write16(FATDiskImage &image) const
{
	FATEntryDiskImage(image);

	for (size_t fat = 0; fat < this->bootSector.fatCount; fat++)
	{
		FATEntryResetCount();
		FATEntry16(MEDIA_TYPE, 0xFF);
		FATEntry16(0xFF, 0xFF);

		uint32_t cluster = FATEntryGetCount();
		size_t bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;
		size_t rootEntryClusters = ((this->bootSector.rootEntryCount * sizeof(struct direntry)) + 
									(bytesPerCluster - 1)) / bytesPerCluster;
		_FATEntry_write16(this->tree.root, &cluster, bytesPerCluster, rootEntryClusters);

		size_t bytesWritten = FATEntryGetCount() * 2;

		if (bytesWritten > fatSizeBytes()) {
			mkfatError(1, "FAT too large; takes up %zu bytes but only have %zu\n", bytesWritten, fatSizeBytes());
		}

		size_t bytesLeft = this->bootSector.fatSize16 * this->bootSector.bytesPerSector - bytesWritten;

		image.writeImgFileZeros(bytesLeft);
	}
}

void _FATEntry_write32(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster)
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
			_FATEntry_write32(child, cluster, bytesPerCluster);
	}

}

void FATTable::write32(FATDiskImage &image) const
{
	FATEntryDiskImage(image);

	for (size_t fat = 0; fat < this->bootSector.fatCount; fat++)
	{
		FATEntryResetCount();
		FATEntry32(MEDIA_TYPE, 0xFF, 0xFF, 0xFF);
		FATEntry32NoAnd(0xFF, 0xFF, 0xFF, 0x0F | HRD_ERR_BYTE | CLN_SHUT_BYTE);

		uint32_t cluster = FATEntryGetCount();
		size_t bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;
		_FATEntry_write32(this->tree.root, &cluster, bytesPerCluster);

		size_t bytesWritten = FATEntryGetCount() * 4;

		if (bytesWritten > fatSizeBytes()) {
			mkfatError(1, "FAT too large; takes up %zu bytes but only have %zu\n", bytesWritten, fatSizeBytes());
		}

		size_t bytesLeft = this->bootSector.biosParamBlock.fat32.fatSize32 * this->bootSector.bytesPerSector - bytesWritten;

		image.writeImgFileZeros(bytesLeft);
	}
}

FATTable::FATTable(const FileTree &tree, const FATBootSector &bootSector, FatType fatType):
	tree(tree), fatType(fatType), bootSector(bootSector) {

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

void FATTable::write_to(FATDiskImage &image) const
{
	switch (this->fatType) {
		case FatType::FAT12: write12(image); break;
		case FatType::FAT16: write16(image); break;
		case FatType::FAT32: write32(image); break;
	}
}
