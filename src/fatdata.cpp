#include "fatdata.h"

void FATData::writeDirectory(TreeItem *item, FATDiskImage *image, size_t occupiedBytes) const
{
	printf("Writing directory '%.8s' with %lu entries\n", item->direntry.fileName, item->children.size());

	// since vector items are contiguous? in memory, maybe try doing a single write
	for (TreeItem *child : item->children)
	{
		image->writeImgFile(&child->direntry, sizeof(struct direntry));
	}

	size_t bytesWritten = item->children.size() * sizeof(struct direntry);
	ssize_t bytesLeft = occupiedBytes - bytesWritten;
	image->writeImgFileZeros(bytesLeft);
}

void FATData::writeFile(TreeItem *item, FATDiskImage *image, size_t occupiedBytes) const
{
	printf("Writing file '%.8s' '%.3s' of size %u\n", item->direntry.fileName, 
													   item->direntry.fileExtension, 
													   item->direntry.fileSize);

	FILE *fFile = fopen(item->path().c_str(), "rb");
	uint8_t *fileData = (uint8_t *) malloc(sizeof(uint8_t) * item->direntry.fileSize);

	if (!fFile)
	{
		fprintf(stderr, "Unable to open file '%s' to copy to disk image\n", item->path().c_str());
		exit(1);
	}
	if (!fileData)
	{
		fprintf(stderr, "Unable to allocate memory for copying file '%s'\n", item->path().c_str());
		exit(1);
	}

	fileRead(fFile, fileData, item->direntry.fileSize);

	image->writeImgFile(fileData, item->direntry.fileSize);

	fclose(fFile);
	free(fileData);

	ssize_t bytesLeft = occupiedBytes - item->direntry.fileSize;
	image->writeImgFileZeros(bytesLeft);
}

void FATData::write16Recursive(TreeItem *item, FATDiskImage *image, size_t rootClusters) const
{
	size_t itemSize;
	if (item->directory())
		itemSize = item->children.size() * sizeof(struct direntry);
	else
		itemSize = item->direntry.fileSize;
	size_t clusterCount = (itemSize + (this->bytesPerCluster - 1)) / this->bytesPerCluster;

	if (itemSize == 0) return;

	size_t occupiedBytes = this->bytesPerCluster * clusterCount;

	if (rootClusters != 0)
		occupiedBytes = rootClusters * this->bytesPerCluster;

	if (item->directory())
		this->writeDirectory(item, image, occupiedBytes);
	else
		this->writeFile(item, image, occupiedBytes);

	for (TreeItem *child : item->children)
	{
		this->write16Recursive(child, image, 0);
	}

}

void FATData::write32Recursive(TreeItem *item, FATDiskImage *image) const
{
	size_t itemSize;
	if (item->directory())
		itemSize = item->children.size() * sizeof(struct direntry);
	else
		itemSize = item->direntry.fileSize;
	size_t clusterCount = (itemSize + (this->bytesPerCluster - 1)) / this->bytesPerCluster;

	if (itemSize == 0) return;

	if (item->directory())
		this->writeDirectory(item, image, this->bytesPerCluster * clusterCount);
	else
		this->writeFile(item, image, this->bytesPerCluster * clusterCount);

	for (TreeItem *child : item->children)
	{
		this->write32Recursive(child, image);
	}
}

void FATData::write12(FATDiskImage *image) const
{
	fprintf(stderr, "FATData::write12 not implemented\n");
	exit(1);
}

void FATData::write16(FATDiskImage *image) const
{
	size_t rootClusters = this->bootSector->rootEntryCount * sizeof(struct direntry);
	rootClusters = (rootClusters + this->bootSector->bytesPerSector - 1) / this->bootSector->bytesPerSector;
	rootClusters = (rootClusters + this->bootSector->sectorsPerCluster - 1) / this->bootSector->sectorsPerCluster;
	this->write16Recursive(this->tree->root, image, rootClusters);
	this->padZeros(image, DISK_SIZE_16);
}

void FATData::write32(FATDiskImage *image) const
{
	this->write32Recursive(this->tree->root, image);
	this->padZeros(image, DISK_SIZE_32);
}

FATData::FATData(FileTree *tree, FATBootSector *bootSector, const std::string &fatType)
{
	this->tree = tree;
	this->bootSector = bootSector;
	this->fatType = fatType;
	this->bytesPerCluster = this->bootSector->bytesPerSector * this->bootSector->sectorsPerCluster;
}

void FATData::write(FATDiskImage *image) const
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

void FATData::padZeros(FATDiskImage *image, size_t diskSize) const
{
	size_t totalBytes = image->sizeImgFile();
	size_t bytesLeft = diskSize * this->bootSector->bytesPerSector - totalBytes;
	image->writeImgFileZeros(bytesLeft);
}
