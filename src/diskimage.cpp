#include "diskimage.h"
#include "writeable.h"
#include "common.h"
#include "error.h"

#include <cstdio>


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
		mkfatError(1, "unable to open disk image file: %s\n", strerror(errno));
	}
}


void FATDiskImage::closeImgFile(void)
{
	int status = fclose(this->imgFile);
	this->imgFile = NULL;

	if (status != 0)
	{
		mkfatError(1, "unable to close disk image file");
	}
}


void FATDiskImage::writeImgFile(const void *data, size_t count)
{
	size_t written = fwrite(data, 1, count, this->imgFile);

	if (written != count)
	{
		mkfatError(1,
			"unable to write to image file, only %zu/%zu bytes written\n",
			written, count
		);
	}
}

void FATDiskImage::writeImgFileZeros(size_t count)
{
	uint8_t zero[1024] = {0};
	for (ssize_t remaining = count; remaining > 0; remaining -= 1024)
		this->writeImgFile(zero, min(remaining, 1024));
}

size_t FATDiskImage::sizeImgFile(void)
{
	return ftell(this->imgFile);
}

