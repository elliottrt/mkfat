#ifndef _MKFAT_FATENTRY
#define _MKFAT_FATENTRY

#include <cstdint>

enum FATEntryType
{
	FREECLUSTER = 0,
	RESERVEDCLUSTER = 1,
	DATACLUSTER = 2,
	BADCLUSTER12 = 0x0FF7,
	ENDCLUSTER12 = 0x0FFF,
	BADCLUSTER16 = 0xFFF7,
	ENDCLUSTER16 = 0xFFFF,
	BADCLUSTER32 = 0x0FFFFFF7,
	ENDCLUSTER32 = 0x0FFFFFFF
};


enum FATEntryType FATEntryType32(uint32_t value);

enum FATEntryType FATEntryType16(uint16_t value);

enum FATEntryType FATEntryType12(uint16_t value);

#endif
