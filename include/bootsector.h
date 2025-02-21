#ifndef _MKFAT_BOOTSECTOR
#define _MKFAT_BOOTSECTOR

#include <stdint.h>
#include <string>

struct FATBootSector
{

	uint8_t jmp[3];
	char oemName[8];

	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster; // need to set
	uint16_t reservedSectors;
	uint8_t fatCount;
	uint16_t rootEntryCount; // look at later
	uint16_t totalSectors16;
	uint8_t mediaDescriptor;
	uint16_t fatSize16; 	// need to set
	uint16_t sectorsPerTrack; // look at later
	uint16_t headCount; // look at later
	uint32_t hiddenSectors; // look at later
	uint32_t totalSectors32;

	union __attribute__((packed))
	{
		struct __attribute__((packed))
		{
			uint8_t driveNumber;
			uint8_t reserved;
			uint8_t bootSignature;
			uint32_t volumeID;
			char volumeLabel[11];
			char fileSystemType[8];
			uint8_t bootCode[448];
		} fat1216;
		struct __attribute__((packed))
		{
			uint32_t fatSize32; // need to set
			uint16_t extensionFlags;
			uint16_t fileSystemVersion;
			uint32_t rootCluster;
			uint16_t fsinfoSector;
			uint16_t backupBootSector;
			uint8_t reserved[12];
			uint8_t driveNumber;
			uint8_t reserved1;
			uint8_t bootSignature;
			uint32_t volumeID;
			char volumeLabel[11];
			char fileSystemType[8];
			uint8_t bootCode[420];
		} fat32;
	} biosParamBlock;

	uint8_t bootSignature[2];

	void defaultBootSector12(const std::string &volumeLabel);
	void defaultBootSector16(const std::string &volumeLabel);
	void defaultBootSector32(const std::string &volumeLabel);
	void bootSectorCommon(void);

	FATBootSector(const std::string &bootPath, const std::string &volumeLabel, const std::string &fatType);
	FATBootSector(void) = default;

} __attribute__ ((packed));

#endif
