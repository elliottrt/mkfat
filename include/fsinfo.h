#ifndef _MKFAT_FSINFO
#define _MKFAT_FSINFO

#include "common.h"
#include <cstdint>

struct FATFSInfo
{

	uint32_t leadSignature = 0x41615252;
	uint8_t zeroed[480] = {0};
	uint32_t structSignature = 0x61417272;
	// TODO: set freeClusters and nextCluster
	// note: nextCluster = entryCount after FATTable has been generated
	uint32_t freeClusters = 0xFFFFFFFF;
	uint32_t nextCluster = 0xFFFFFFFF;
	uint8_t zeroed1[12] = {0};
	uint32_t trailSignature = 0xAA550000;

	FATFSInfo(void) = default;

} __attribute__ ((packed));

static_assert(sizeof(FATFSInfo) == DISK_SECTOR_SIZE, "FATFSInfo must be the same size as a disk sector");

#endif