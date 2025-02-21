#include "fattable.h"
#include "logging.h"
#include "fatentry.h"

void FATTable::generateFAT16_recursive(TreeItem *item, size_t bytesPerCluster, bool isRoot)
{
	size_t itemSize = item->size();

	// See https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating
	ssize_t clusterCount = (itemSize + (bytesPerCluster - 1)) / bytesPerCluster;
	clusterCount--; // remove EOC, we'll do that manually

	mkfatVerbose("Writing fatentry '%.8s' '%.3s', size: %zd\n", item->direntry.fileName, item->direntry.fileExtension, clusterCount + 1);

	if (itemSize == 0) 
		return;

	if (!isRoot)
	{
		item->direntry.firstClusterLo = entryCount & 0xFFFF;
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

		while (clusterCount--)
		{
			FATEntry16(entryCount + 1);
		}

		FATEntry16(ENDCLUSTER16);
	}

	if (item->is_directory())
	{
		for (TreeItem *child : item->children)
			generateFAT16_recursive(child, bytesPerCluster, false);
	}

}

void FATTable::generateFAT16(void)
{
	FATEntry16(MEDIA_TYPE, 0xFF);
	FATEntry16(0xFF, 0xFF);

	size_t bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;

	generateFAT16_recursive(this->tree.root, bytesPerCluster, true);
}


void FATTable::FATEntry16(uint8_t b0, uint8_t b1)
{
	uint8_t bytes[2] = {b0, b1};

	tableWrite(bytes, 2);
	entryCount++;
}

void FATTable::FATEntry16(uint16_t entry)
{
	uint8_t *bytes = (uint8_t *) &entry;

	tableWrite(bytes, 2);
	entryCount++;
}
