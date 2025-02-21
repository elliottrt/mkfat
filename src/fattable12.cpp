#include "fattable.h"
#include "logging.h"
#include "fatentry.h"

// TODO: make rootEntryClusters a boolean to of whether the current dir is the root
void FATTable::generateFAT12_recursive(TreeItem *item, uint32_t *cluster, size_t bytesPerCluster, uint16_t rootEntryClusters)
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
			generateFAT12_recursive(child, cluster, bytesPerCluster, 0);
	}

}

void FATTable::generateFAT12(void)
{
	FATEntry12(this->bootSector.mediaDescriptor, 0xFF);
	FATEntry12(0xFF, 0xFF);

	uint32_t cluster = entryCount;
	size_t bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;
	size_t rootEntryClusters = ((this->bootSector.rootEntryCount * sizeof(struct direntry)) + 
								(bytesPerCluster - 1)) / bytesPerCluster;

	generateFAT12_recursive(this->tree.root, &cluster, bytesPerCluster, rootEntryClusters);

	FATEntry12Flush();
}

void FATTable::FATEntry12(uint8_t b0, uint8_t b1)
{
	if (entryWaiting)
	{
		uint8_t delayedBytes[2] =
		{ 
			(uint8_t)(delayedEntry & 0xFF),
			(uint8_t)((delayedEntry & 0xF00) >> 8) 
		};
		uint8_t bytes[3] =
		{ 
			delayedBytes[0], 
			(uint8_t)(delayedBytes[1] | ((b0 & 0xF) << 4)), 
			(uint8_t)(((b0 & 0xF0) >> 4) | ((b1 & 0x0F) << 4)) 
		};

		tableWrite(bytes, 3);
		entryCount += 2;
	}
	else
	{
		delayedEntry = b0 | ((uint16_t)b1 << 8);
	}
	entryWaiting = !entryWaiting;
}

void FATTable::FATEntry12(uint16_t entry)
{
	if (entryWaiting)
	{
		uint8_t delayedBytes[2] =
		{
			(uint8_t) (delayedEntry & 0xFF),
			(uint8_t)((delayedEntry & 0xF00) >> 8) 
		};
		uint8_t bytes[3] =
		{ 
			delayedBytes[0], 
			(uint8_t)(delayedBytes[1] | ((entry & 0xF) << 4)), 
			(uint8_t)((entry & 0xFF0) >> 4) 
		};

		tableWrite(bytes, 3);
		entryCount += 2;
	}
	else
	{
		delayedEntry = entry;
	}
	entryWaiting = !entryWaiting;
}

void FATTable::FATEntry12Flush(void)
{
	if (entryWaiting)
	{
		uint8_t bytes[3] =
		{
			(uint8_t)(delayedEntry & 0xFF), 
			(uint8_t)((delayedEntry & 0xF00) >> 8),
			0x00
		};

		tableWrite(bytes, 3);
		entryCount += 2;
		entryWaiting = false;
	}
}
