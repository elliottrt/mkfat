#ifndef _MKFAT_FSINFO
#define _MKFAT_FSINFO

struct FATFSInfo
{

	uint32_t leadSignature = 0x41615252;
	uint8_t zeroed[480] = {0};
	uint32_t structSignature = 0x61417272;
	uint32_t freeClusters = 0xFFFFFFFF;
	uint32_t nextCluster = 0xFFFFFFFF;
	uint8_t zeroed1[12] = {0};
	uint32_t trailSignature = 0xAA550000;

	FATFSInfo(void) = default;

} __attribute__ ((packed));

#endif