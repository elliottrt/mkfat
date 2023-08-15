#include "fattable.h"
#include "fatentry.h"

#define CLN_SHUT_BITMASK 0x8000000
#define HRD_ERR_BITMAS 0x04000000

#define CLN_SHUT_BYTE 0x80
#define HRD_ERR_BYTE 0x40

void FATTable::write12(FATDiskImage *image) const
{
	fprintf(stderr, "FATTable::write12 not implemented\n");
	exit(1);
}

void _FATEntry_write16(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster, uint16_t rootEntryClusters)
{
	size_t itemSize;
	if (item->directory())
		itemSize = item->children.size() * sizeof(struct direntry);
	else
		itemSize = item->direntry.fileSize;

	printf("Writing fatentry for '%.8s' '%.3s', ", item->direntry.fileName, item->direntry.fileExtension);

	// See https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
	size_t entryCount = (itemSize + (bytesPerCluster - 1)) / bytesPerCluster;

	entryCount--; // remove EOC, we'll do that manually

	printf("entry size: %lu\n", entryCount + 1);

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

	if (!item->directory()) return;

	for (TreeItem *child : item->children)
	{
		_FATEntry_write16(child, cluster, bytesPerCluster, 0);
	}

}

void FATTable::write16(FATDiskImage *image) const
{
	FATEntryDiskImage(image);
	for (size_t fat = 0; fat < this->bootSector->fatCount; fat++)
	{
		FATEntryResetCount();
		FATEntry16(MEDIA_TYPE, 0xFF);
		FATEntry16(0xFF, 0xFF);

		uint32_t cluster = FATEntryGetCount();
		size_t bytesPerCluster = this->bootSector->bytesPerSector * this->bootSector->sectorsPerCluster;
		size_t rootEntryClusters = ((this->bootSector->rootEntryCount * sizeof(struct direntry)) + 
									(bytesPerCluster - 1)) / bytesPerCluster;
		_FATEntry_write16(this->tree->root, &cluster, bytesPerCluster, rootEntryClusters);

		size_t bytesWritten = FATEntryGetCount() * 2;
		size_t bytesLeft = this->bootSector->fatSize16 * this->bootSector->bytesPerSector - bytesWritten;

		image->writeImgFileZeros(bytesLeft);
	}
}

void _FATEntry_write32(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster)
{
	size_t itemSize;
	if (item->directory())
		itemSize = item->children.size() * sizeof(struct direntry);
	else
		itemSize = item->direntry.fileSize;

	printf("Writing fatentry for '%.8s' '%.3s', ", item->direntry.fileName, item->direntry.fileExtension);

	// See https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
	size_t entryCount = (itemSize + (bytesPerCluster - 1)) / bytesPerCluster;
	entryCount--; // remove EOC, we'll do that manually

	printf("entry size: %lu\n", entryCount + 1);

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

	if (!item->directory()) return;

	for (TreeItem *child : item->children)
	{
		_FATEntry_write32(child, cluster, bytesPerCluster);
	}

}

void FATTable::write32(FATDiskImage *image) const
{
	FATEntryDiskImage(image);
	for (size_t fat = 0; fat < this->bootSector->fatCount; fat++)
	{
		FATEntryResetCount();
		FATEntry32(MEDIA_TYPE, 0xFF, 0xFF, 0xFF);
		FATEntry32NoAnd(0xFF, 0xFF, 0xFF, 0x0F | HRD_ERR_BYTE | CLN_SHUT_BYTE);

		uint32_t cluster = FATEntryGetCount();
		size_t bytesPerCluster = this->bootSector->bytesPerSector * this->bootSector->sectorsPerCluster;
		_FATEntry_write32(this->tree->root, &cluster, bytesPerCluster);

		size_t bytesWritten = FATEntryGetCount() * 4;
		size_t bytesLeft = this->bootSector->biosParamBlock.fat32.fatSize32 * this->bootSector->bytesPerSector - bytesWritten;

		image->writeImgFileZeros(bytesLeft);
	}
}

FATTable::FATTable(FileTree *tree, FATBootSector *bootSector, const std::string &fatType)
{
	this->tree = tree;
	this->fatType = fatType;
	this->bootSector = bootSector;
}

void FATTable::write(FATDiskImage *image) const
{

	if (this->fatType == "32")
		this->write32(image);
	else if (this->fatType == "16")
		this->write16(image);
	else if (this->fatType == "12")
		this->write12(image);
	else
	{
		fprintf(stderr, "Invalid fat type '%s', must be 12,16,32\n", this->fatType.c_str());
		exit(1);
	}
}
