#ifndef _MKFAT_FATENTRY
#define _MKFAT_FATENTRY

#include "diskimage.h"

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

void FATEntry32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
void FATEntry32NoAnd(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
void FATEntry32(uint32_t entry);
enum FATEntryType FATEntryType32(uint32_t value);

void FATEntry16(uint8_t b0, uint8_t b1);
void FATEntry16(uint16_t entry);
enum FATEntryType FATEntryType16(uint16_t value);

void FATEntry12(uint8_t b0, uint8_t b1);
void FATEntry12(uint16_t entry);
void FATEntry12Flush(void);
enum FATEntryType FATEntryType12(uint16_t value);

void FATEntryDiskImage(FATDiskImage *image);

size_t FATEntryGetCount();
void FATEntryResetCount();

#endif
