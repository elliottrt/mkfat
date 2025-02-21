#ifndef _MKFAT_RESERVED
#define _MKFAT_RESERVED

#include "writeable.h"
#include "bootsector.h"
#include "fsinfo.h"

class FATReserved : public FATWriteable
{

public:

	FATBootSector bootSector;
	std::string fatType;
	size_t reservedSectors;
	FATFSInfo fsinfo;
	uint8_t *reservedData;
	size_t reservedDataSize;

	void loadReservedData(const std::string &reservedDataPath);

	FATReserved(const std::string &bootSectorPath, 
					   const std::string &reservedDataPath, 
					   const std::string &fatType,
					   const std::string &volumeLabel);

	void write(FATDiskImage &image) const override;

};

#endif
