#ifndef _MKFAT_RESERVED
#define _MKFAT_RESERVED

#include "diskimage.h"
#include "bootsector.h"
#include "fsinfo.h"
#include "fattype.h"

class FATReserved
{

	void init12(void);
	void init16(void);
	void init32(void);

public:

	FATBootSector bootSector;
	const FatType fatType;
	size_t reservedSectors;
	FATFSInfo fsinfo;
	uint8_t *reservedData;
	size_t reservedDataSize;

	void loadReservedData(const std::string &reservedDataPath);

	FATReserved(const std::string &bootSectorPath, 
					   const std::string &reservedDataPath, 
					   FatType fatType,
					   const std::string &volumeLabel);

	void write_to(FATDiskImage &image) const;

};

#endif
