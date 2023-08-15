#include "diskimage.h"
#include "writeable.h"
#include "common.h"

#include <stdio.h>


FATDiskImage::FATDiskImage(const std::string &imgPath, size_t sectorSize)
{
	this->imgPath = imgPath;
	this->sectorSize = sectorSize;
}


void FATDiskImage::createImgFile(void)
{

	this->imgFile = fopen(this->imgPath.c_str(), "wb");

	if (this->imgFile == NULL)
	{
		perror("Unable to open disk image file");
		exit(1);
	}

}


void FATDiskImage::closeImgFile(void)
{

	int status = fclose(this->imgFile);

	if (status != 0)
	{
		perror("Unable to close disk image file");
		exit(1);
	}

}


void FATDiskImage::writeImgFile(const void *data, size_t count)
{
	size_t written = fwrite(data, 1, count, this->imgFile);
	if (written != count)
	{
		fprintf(stderr, "Problem writing to disk image, only %lu/%lu byte(s) written.\n", written, count);
		if (ferror(this->imgFile))
		{
			perror("Problem");
		}
		exit(1);
	}
}


void FATDiskImage::writeImgFile(const FATWriteable *fatData)
{
	fatData->write(this);
}


// TODO: optimize this
void FATDiskImage::writeImgFileZeros(size_t count)
{
	uint8_t _zero[1024] = {0};
	ssize_t _count = count;
	for (; _count > 0; _count -= 1024)
		this->writeImgFile(_zero, min(_count, 1024));
}

size_t FATDiskImage::sizeImgFile(void)
{
	return ftell(this->imgFile);
}

