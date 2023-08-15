#ifndef _TOOLS_FAT_COMMON
#define _TOOLS_FAT_COMMON

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISK_SECTOR_SIZE 512
#define FAT_COUNT 2
#define DRIVE_NUM 0x80
#define BOOT_SIGNATURE 0x29
#define ROOT_CLUSTER 2
#define FSINFO_SECTOR 1
#define BACKUP_BOOT_SECTOR 6
#define VOLUMELABEL "MKFATDSK"
#define MEDIA_TYPE 0xF8
#define DIRENTRY_NAMELEN 8
#define DIRENTRY_EXTLEN 3
// about 64 mb
#define DISK_SIZE_32 81920
#define TOTAL_SECTORS_32 DISK_SIZE_32
#define DISK_SIZE_16 49152 // or maybe 0xFFFF
#define TOTAL_SECTORS_16 DISK_SIZE_16
#define DISK_SIZE_12 32768
#define TOTAL_SECTORS_12 DISK_SIZE_12

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

enum dirattr
{
	READ_ONLY = 0x01,
	HIDDEN = 0x02,
	SYSTEM = 0x04,
	VOLUME_ID = 0x08,
	DIRECTORY = 0x10,
	ARCHIVE = 0x20,
	DEVICE = 0x40,
	UNUSED = 0x80,
	LFN = READ_ONLY | HIDDEN | SYSTEM | VOLUME_ID 
};

struct direntry
{
	char fileName[DIRENTRY_NAMELEN];
	char fileExtension[DIRENTRY_EXTLEN];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creationTenths;
	uint16_t creation2Seconds;
	uint16_t creationDate;
	uint16_t lastAccessDate;
	uint16_t firstClusterHi;
	uint16_t writeTime;
	uint16_t writeDate;
	uint16_t firstClusterLo;
	uint32_t fileSize;

	direntry(void);
	void setFileName(const char *name);
} __attribute__ ((packed));

void fileRead(FILE *file, void *out, size_t size);
size_t fileSize(FILE *f);

#endif
