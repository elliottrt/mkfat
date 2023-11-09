#include "fatentry.h"

FATDiskImage *_diskImage = NULL;
size_t _FATEntryCount = 0;

uint16_t _FAT12DelayedEntry = 0x0000;
bool entryWaiting = false;

void FATEntry32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	uint8_t bytes[4] = {b0, b1, b2, (uint8_t) (b3 & 0x0F)};

	_diskImage->writeImgFile(bytes, 4);
	_FATEntryCount++;
}

void FATEntry32NoAnd(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
	uint8_t bytes[4] = {b0, b1, b2, b3};

	_diskImage->writeImgFile(bytes, 4);
	_FATEntryCount++;
}

void FATEntry32(uint32_t entry)
{
	uint8_t *bytes = (uint8_t *) &entry;
	bytes[3] &= 0x0F;

	_diskImage->writeImgFile(bytes, 4);
	_FATEntryCount++;
}

enum FATEntryType FATEntryType32(uint32_t value)
{
	switch (value)
	{
	case 0:
		return FREECLUSTER;
	case 1:
	case 0xFFFFFF0:
	case 0xFFFFFF1:
	case 0xFFFFFF2:
	case 0xFFFFFF3:
	case 0xFFFFFF4:
	case 0xFFFFFF5:
	case 0xFFFFFF6:
		return RESERVEDCLUSTER;
	case 0xFFFFFF7:
		return BADCLUSTER32;
	case 0xFFFFFF8:
	case 0xFFFFFF9:
	case 0xFFFFFFA:
	case 0xFFFFFFB:
	case 0xFFFFFFC:
	case 0xFFFFFFD:
	case 0xFFFFFFE:
	case 0xFFFFFFF:
		return ENDCLUSTER32;
	default:
		return DATACLUSTER;
	}

	return RESERVEDCLUSTER;
}

void FATEntry16(uint8_t b0, uint8_t b1)
{
	uint8_t bytes[2] = {b0, b1};

	_diskImage->writeImgFile(bytes, 2);
	_FATEntryCount++;
}

void FATEntry16(uint16_t entry)
{
	uint8_t *bytes = (uint8_t *) &entry;

	_diskImage->writeImgFile(bytes, 2);
	_FATEntryCount++;
}

enum FATEntryType FATEntryType16(uint16_t value)
{
	switch (value)
	{
	case 0:
		return FREECLUSTER;
	case 1:
	case 0xFFF0:
	case 0xFFF1:
	case 0xFFF2:
	case 0xFFF3:
	case 0xFFF4:
	case 0xFFF5:
	case 0xFFF6:
		return RESERVEDCLUSTER;
	case 0xFFF7:
		return BADCLUSTER16;
	case 0xFFF8:
	case 0xFFF9:
	case 0xFFFA:
	case 0xFFFB:
	case 0xFFFC:
	case 0xFFFD:
	case 0xFFFE:
	case 0xFFFF:
		return ENDCLUSTER16;
	default:
		return DATACLUSTER;
	}

	return RESERVEDCLUSTER;
}

void FATEntry12(uint8_t b0, uint8_t b1)
{
	if (entryWaiting)
	{
		uint8_t delayedBytes[2] = { (uint8_t)(_FAT12DelayedEntry & 0xFF), (uint8_t)((_FAT12DelayedEntry & 0xF00) >> 8) };
		uint8_t bytes[3] = { 
							 delayedBytes[0], 
							 (uint8_t)(delayedBytes[1] | ((b0 & 0xF) << 4)), 
							 (uint8_t)(((b0 & 0xF0) >> 4) | ((b1 & 0x0F) << 4)) 
						   };
		/*
		printf("delayed entry: %x\n", delayedBytes[0]);
		printf("delayed entry: %x\n", b0 & 0x0F);
		printf("delayed entry: %x\n", delayedBytes[1]);
		printf("delayed entry: %x\n", (b0 & 0xF0) >> 4);
		printf("delayed entry: %x\n", b1 & 0xF0);
		*/
		_diskImage->writeImgFile(bytes, 3);
		_FATEntryCount += 2;
	}
	else
	{
		_FAT12DelayedEntry = b0 | ((uint16_t)b1 << 8);
	}
	entryWaiting = !entryWaiting;
}

void FATEntry12(uint16_t entry)
{
	if (entryWaiting)
	{
		uint8_t delayedBytes[2] = { (uint8_t) (_FAT12DelayedEntry & 0xFF), (uint8_t)((_FAT12DelayedEntry & 0xF00) >> 8) };
		uint8_t bytes[3] = { 
							 delayedBytes[0], 
							 (uint8_t)(delayedBytes[1] | ((entry & 0xF) << 4)), 
							 (uint8_t)((entry & 0xFF0) >> 4) 
						   };
		_diskImage->writeImgFile(bytes, 3);
		_FATEntryCount += 2;
	}
	else
	{
		_FAT12DelayedEntry = entry;
	}
	entryWaiting = !entryWaiting;
}

void FATEntry12Flush(void)
{
	if (entryWaiting)
	{
		uint8_t bytes[3] = { (uint8_t)(_FAT12DelayedEntry & 0xFF), 
							 (uint8_t)((_FAT12DelayedEntry & 0xF00) >> 8),
							 0x00
							};
		_diskImage->writeImgFile(bytes, 3);
		_FATEntryCount += 2;
		entryWaiting = false;
	}
}

enum FATEntryType FATEntryType12(uint16_t value)
{
	switch (value & 0x0FFF)
	{
	case 0:
		return FREECLUSTER;
	case 1: 
	case 0xFF0:
	case 0xFF1:
	case 0xFF2:
	case 0xFF3:
	case 0xFF4:
	case 0xFF5:
	case 0xFF6:
		return RESERVEDCLUSTER;
	case 0xFF7:
		return BADCLUSTER12;
	case 0xFF8:
	case 0xFF9:
	case 0xFFA:
	case 0xFFB:
	case 0xFFC:
	case 0xFFD:
	case 0xFFE:
	case 0xFFF:
		return ENDCLUSTER12;
	default:
		return DATACLUSTER;
	}

	return RESERVEDCLUSTER;
}

void FATEntryDiskImage(FATDiskImage *image)
{
	_diskImage = image;
}

size_t FATEntryGetCount()
{
	return _FATEntryCount;
}

void FATEntryResetCount()
{
	_FATEntryCount = 0;
}

