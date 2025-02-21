#ifndef _MKFAT_DISKIMAGE
#define _MKFAT_DISKIMAGE

#include <string>

class FATWriteable;

class FATDiskImage
{

	std::string imgPath = "";
	FILE *imgFile = NULL;
	size_t sectorSize;

public:

	FATDiskImage(const std::string &imgPath, size_t sectorSize);

	void createImgFile(void);
	void closeImgFile(void);

	void writeImgFile(const void *data, size_t count);
	void writeImgFile(const FATWriteable *fatData);

	void writeImgFileZeros(size_t count);

	size_t sizeImgFile(void);

};

#endif
