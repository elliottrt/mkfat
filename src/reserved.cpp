#include "reserved.h"
#include "common.h"
#include "error.h"
#include "fattype.h"

void FATReserved::loadReservedData(const std::string &reservedDataPath)
{
	this->reservedDataSize = fileSize(reservedDataPath.c_str());
	this->reservedData = (uint8_t *) malloc(sizeof(uint8_t) * this->reservedDataSize);

	if (!this->reservedData)
	{
		mkfatError(1, "unable to allocate memory for reserved data file\n");
	}

	fileRead(reservedDataPath.c_str(), this->reservedData, this->reservedDataSize);

	size_t sectors = (this->reservedDataSize + DISK_SECTOR_SIZE - 1) / DISK_SECTOR_SIZE;

	size_t systemSectors = this->fatType == FatType::FAT32 ? 8 : 1;

	// if we need more reserved sectors, get them
	this->reservedSectors = max(systemSectors + sectors, this->reservedSectors);

}

void FATReserved::init12(void)
{
	int32_t reservedDiff = this->reservedSectors - 1;
	int32_t spaceForFAT = DISK_SIZE_12 - reservedDiff;
	if (spaceForFAT <= 0)
	{
		mkfatError(1, "amount of reserved sectors (%zu) exceeds disk space (%zu)\n", this->reservedSectors, DISK_SIZE_16);
	}

	this->bootSector.totalSectors16 = DISK_SIZE_12;

	uint32_t rootDirectorySectors = ((this->bootSector.rootEntryCount * sizeof(struct direntry)) + (this->bootSector.bytesPerSector - 1)) / this->bootSector.bytesPerSector;

	uint32_t dataSectorCount = this->bootSector.totalSectors16 - 
						(rootDirectorySectors + 
						this->reservedSectors + 
						this->bootSector.fatCount * this->bootSector.fatSize16);

	if (this->bootSector.sectorsPerCluster == 0)
	{
		mkfatError(1, "disk too small (%zu)\n", DISK_SIZE_12);
	}

	uint32_t clusterCount = dataSectorCount / this->bootSector.sectorsPerCluster;

	if (clusterCount >= 4085)
	{
		mkfatError(1, "cluster count too large (%u), drive should be FAT16\n", clusterCount);
	}
	else if (clusterCount >= 65525)
	{
		mkfatError(1, "cluster count too large (%u), drive should be FAT32\n", clusterCount);
	}
}

void FATReserved::init16(void)
{
	int32_t reservedDiff = this->reservedSectors - 1;
	int32_t spaceForFAT = DISK_SIZE_16 - reservedDiff;
	if (spaceForFAT <= 0)
	{
		mkfatError(1, "amount of reserved sectors (%lu) exceeds disk space (%zu)\n", this->reservedSectors, DISK_SIZE_16);
	}

	this->bootSector.totalSectors16 = DISK_SIZE_16;

	uint32_t rootDirectorySectors = ((this->bootSector.rootEntryCount * sizeof(struct direntry)) + (this->bootSector.bytesPerSector - 1)) / this->bootSector.bytesPerSector;
	uint32_t tmp1 = DISK_SIZE_16 - (this->reservedSectors + rootDirectorySectors);
	uint32_t tmp2 = (256 * this->bootSector.sectorsPerCluster) + this->bootSector.fatCount;
	this->bootSector.fatSize16 = (tmp1 + tmp2 - 1) / tmp2;

	uint32_t dataSectorCount = this->bootSector.totalSectors16 - 
						(rootDirectorySectors + 
						this->reservedSectors + 
						this->bootSector.fatCount * this->bootSector.fatSize16);

	if (this->bootSector.sectorsPerCluster == 0)
	{
		// TODO: instead of using hardcoded values, reference the ds2spc table
		mkfatError(1, "disk too small (%zu), should be at least %zu\n", DISK_SIZE_16, 8400 + reservedDiff);
	}

	uint32_t clusterCount = dataSectorCount / this->bootSector.sectorsPerCluster;


	if (clusterCount < 4085)
	{
		mkfatError(1, "cluster count too small (%u), drive should be FAT12\n", clusterCount);
	}
	else if (clusterCount >= 65525)
	{
		mkfatError(1, "cluster count too large (%u), drive should be FAT32\n", clusterCount);
	}
}

void FATReserved::init32(void)
{
	int32_t reservedDiff = this->reservedSectors - 32;
	int32_t spaceForFAT = DISK_SIZE_32 - reservedDiff;
	if (spaceForFAT <= 0)
	{
		mkfatError(1, "Amount of reserved sectors (%lu) exceeds disk space (%i)\n", this->reservedSectors, DISK_SIZE_32);
	}

	this->bootSector.totalSectors32 = DISK_SIZE_32;

	uint32_t tmp1 = DISK_SIZE_32 - this->reservedSectors;
	uint32_t tmp2 = (256 * this->bootSector.sectorsPerCluster) + this->bootSector.fatCount;
	tmp2 /= 2;
	this->bootSector.biosParamBlock.fat32.fatSize32 = (tmp1 + tmp2 - 1) / tmp2;

	uint32_t dataSectorCount = this->bootSector.totalSectors32 - 
						(this->reservedSectors + this->bootSector.fatCount * this->bootSector.biosParamBlock.fat32.fatSize32);

	if (this->bootSector.sectorsPerCluster == 0)
	{
		// TODO: instead of using hardcoded values, reference the ds2spc table
		mkfatError(1, "disk size too small (%zu), should be at least %zu\n", DISK_SIZE_32, 66600 + reservedDiff);
	}

	uint32_t clusterCount = dataSectorCount / this->bootSector.sectorsPerCluster;

	if (clusterCount < 4085)
	{
		mkfatError(1, "cluster count too small (%u), drive should be FAT12\n", clusterCount);
	}
	else if (clusterCount < 65525)
	{
		mkfatError(1, "cluster count too small (%u), drive should be FAT16\n", clusterCount);
	}
}

FATReserved::FATReserved(const std::string &bootSectorPath, 
						 const std::string &reservedDataPath, 
					   	 FatType fatType,
					   	 const std::string &volumeLabel): fatType(fatType)
{
	this->bootSector = FATBootSector(bootSectorPath, volumeLabel, fatType);

	this->reservedData = NULL;
	this->reservedDataSize = 0;
	// 32 sectors if fat32 else 1 sector
	this->reservedSectors = this->fatType == FatType::FAT32 ? 32 : 1;

	if (reservedDataPath != "")
		this->loadReservedData(reservedDataPath);

	this->bootSector.reservedSectors = this->reservedSectors;

	if (fatType == FatType::FAT32)
	{
		init32();
	}
	else if (fatType == FatType::FAT16)
	{
		init16();
	}
	else
	{
		init12();
	}

}

void FATReserved::write_to(FATDiskImage &image) const
{

	ssize_t bytesLeft = this->reservedSectors * DISK_SECTOR_SIZE;

	image.writeImgFile(&this->bootSector, DISK_SECTOR_SIZE);
	bytesLeft -= DISK_SECTOR_SIZE;

	if (this->fatType == FatType::FAT32)
	{
		image.writeImgFile(&this->fsinfo, sizeof(this->fsinfo));
		image.writeImgFileZeros(DISK_SECTOR_SIZE * (BACKUP_BOOT_SECTOR - 2));
		
		// backup bootsector and fsinfo
		image.writeImgFile(&this->bootSector, sizeof(this->bootSector));
		image.writeImgFile(&this->fsinfo, DISK_SECTOR_SIZE);
		bytesLeft -= DISK_SECTOR_SIZE * 7;
	}

	if (this->reservedData)
	{
		image.writeImgFile(this->reservedData, this->reservedDataSize);
		bytesLeft -= this->reservedDataSize;
	}

	if (bytesLeft < 0)
	{
		mkfatError(1, "reserved section is too small\n");
	}

	image.writeImgFileZeros(bytesLeft);

}
