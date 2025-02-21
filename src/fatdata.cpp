#include "fatdata.h"
#include "error.h"
#include "fattype.h"

#include <cstdlib>

void FATData::writeDirectory(const TreeItem *item, FATDiskImage &image, size_t occupiedBytes) const
{
	printf("Writing directory '%.8s' with %lu entries\n", item->direntry.fileName, item->children.size());

	for (TreeItem *child : item->children)
	{
		image.writeImgFile(&child->direntry, sizeof(struct direntry));
	}

	size_t bytesWritten = item->children.size() * sizeof(struct direntry);
	ssize_t bytesLeft = occupiedBytes - bytesWritten;
	image.writeImgFileZeros(bytesLeft);
}

void FATData::writeFile(const TreeItem *item, FATDiskImage &image, size_t occupiedBytes) const
{
	printf("Writing file '%.8s' '%.3s' of size %u\n", item->direntry.fileName, 
			item->direntry.fileExtension,  item->direntry.fileSize);

	uint8_t *fileData = (uint8_t *) calloc(item->direntry.fileSize, sizeof(uint8_t));

	if (!fileData)
	{
		mkfatError(1, "unable to allocate memory for copying file '%s'\n", item->path().c_str());
	}

	fileRead(item->path().c_str(), fileData, item->direntry.fileSize);

	image.writeImgFile(fileData, item->direntry.fileSize);

	free(fileData);

	ssize_t bytesLeft = occupiedBytes - item->direntry.fileSize;
	image.writeImgFileZeros(bytesLeft);
}

void FATData::write12Recursive(const TreeItem *item, FATDiskImage &image, size_t rootSectors) const
{
	size_t itemSize = item->size();
	size_t clusterCount = (itemSize + (this->bytesPerCluster - 1)) / this->bytesPerCluster;

	if (itemSize == 0) return;

	size_t occupiedBytes = this->bytesPerCluster * clusterCount;

	if (rootSectors != 0)
		occupiedBytes = rootSectors * this->bootSector.bytesPerSector;

	if (item->is_directory())
		this->writeDirectory(item, image, occupiedBytes);
	else
		this->writeFile(item, image, occupiedBytes);

	for (const TreeItem *child : item->children)
	{
		this->write12Recursive(child, image, 0);
	}

}

void FATData::write16Recursive(const TreeItem *item, FATDiskImage &image, size_t rootSectors) const
{
	size_t itemSize = item->size();
	size_t clusterCount = (itemSize + (this->bytesPerCluster - 1)) / this->bytesPerCluster;

	if (itemSize == 0) return;

	size_t occupiedBytes = this->bytesPerCluster * clusterCount;

	if (rootSectors != 0)
		occupiedBytes = rootSectors * this->bootSector.bytesPerSector;

	if (item->is_directory())
		this->writeDirectory(item, image, occupiedBytes);
	else
		this->writeFile(item, image, occupiedBytes);

	for (const TreeItem *child : item->children)
	{
		this->write16Recursive(child, image, 0);
	}

}

void FATData::write32Recursive(const TreeItem *item, FATDiskImage &image) const
{
	size_t itemSize = item->size();
	size_t clusterCount = (itemSize + (this->bytesPerCluster - 1)) / this->bytesPerCluster;

	if (itemSize == 0) return;

	if (item->is_directory())
		this->writeDirectory(item, image, this->bytesPerCluster * clusterCount);
	else
		this->writeFile(item, image, this->bytesPerCluster * clusterCount);

	for (const TreeItem *child : item->children)
	{
		this->write32Recursive(child, image);
	}
}

void FATData::write12(FATDiskImage &image) const
{
	size_t rootSectors = this->bootSector.rootEntryCount * sizeof(struct direntry);
	rootSectors = (rootSectors + this->bootSector.bytesPerSector - 1) / this->bootSector.bytesPerSector;
	this->write12Recursive(this->tree.root, image, rootSectors);
	this->padZeros(image, DISK_SIZE_12);
}

void FATData::write16(FATDiskImage &image) const
{
	size_t rootSectors = this->bootSector.rootEntryCount * sizeof(struct direntry);
	rootSectors = (rootSectors + this->bootSector.bytesPerSector - 1) / this->bootSector.bytesPerSector;
	this->write16Recursive(this->tree.root, image, rootSectors);
	this->padZeros(image, DISK_SIZE_16);
}

void FATData::write32(FATDiskImage &image) const
{
	this->write32Recursive(this->tree.root, image);
	this->padZeros(image, DISK_SIZE_32);
}

FATData::FATData(const FileTree &tree, const FATBootSector &bootSector, FatType fatType): 
	tree(tree), bootSector(bootSector), fatType(fatType) {

	this->bytesPerCluster = this->bootSector.bytesPerSector * this->bootSector.sectorsPerCluster;
}

void FATData::write(FATDiskImage &image) const
{
	switch (this->fatType) {
		case FatType::FAT12: write12(image); break;
		case FatType::FAT16: write16(image); break;
		case FatType::FAT32: write32(image); break;
	}
}

void FATData::padZeros(FATDiskImage &image, size_t diskSize) const
{
	size_t totalBytes = image.sizeImgFile();
	size_t bytesLeft = diskSize * this->bootSector.bytesPerSector - totalBytes;
	image.writeImgFileZeros(bytesLeft);
}
