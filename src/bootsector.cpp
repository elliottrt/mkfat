#include "bootsector.h"
#include "common.h"
#include "fatformat.h"
#include "fattype.h"

// this is found on pages 22-23 of fatgen103.pages
struct spctable
{
	uint32_t diskSize;
	uint8_t sectorsPerCluster;
}
FAT32_SPC_TABLE[] =
{
	{ 66600, 0 },
	{ 532480, 1 },
	{ 16777216, 8 },
	{ 33554432, 16 },
	{ 67108864, 32 },
	{ 0xFFFFFFFF, 64 }
},
FAT16_SPC_TABLE[] =
{
	{ 8400,   0},
    { 32680,   2},
    { 262144,   4},
    { 524288,    8},
    { 1048576,  16},
    { 2097152,  32},
    { 4194304,  64},
    { 0xFFFFFFFF, 0}
};

// From https://android.googlesource.com/platform/system/core/+/kitkat-release/toolbox/newfs_msdos.c
static const uint8_t defaultBootCode[] =
{
	0xfa,			/* cli		    */
    0x31, 0xc0, 		/* xor	   ax,ax    */
    0x8e, 0xd0, 		/* mov	   ss,ax    */
    0xbc, 0x00, 0x7c,		/* mov	   sp,7c00h */
    0xfb,			/* sti		    */
    0x8e, 0xd8, 		/* mov	   ds,ax    */
    0xe8, 0x00, 0x00,		/* call    $ + 3    */
    0x5e,			/* pop	   si	    */
    0x83, 0xc6, 0x19,		/* add	   si,+19h  */
    0xbb, 0x07, 0x00,		/* mov	   bx,0007h */
    0xfc,			/* cld		    */
    0xac,			/* lodsb	    */
    0x84, 0xc0, 		/* test    al,al    */
    0x74, 0x06, 		/* jz	   $ + 8    */
    0xb4, 0x0e, 		/* mov	   ah,0eh   */
    0xcd, 0x10, 		/* int	   10h	    */
    0xeb, 0xf5, 		/* jmp	   $ - 9    */
    0x30, 0xe4, 		/* xor	   ah,ah    */
    0xcd, 0x16, 		/* int	   16h	    */
    0xcd, 0x19, 		/* int	   19h	    */
    0x0d, 0x0a,
    'N', 'o', 'n', '-', 's', 'y', 's', 't',
    'e', 'm', ' ', 'd', 'i', 's', 'k',
    0x0d, 0x0a,
    'P', 'r', 'e', 's', 's', ' ', 'a', 'n',
    'y', ' ', 'k', 'e', 'y', ' ', 't', 'o',
    ' ', 'r', 'e', 'b', 'o', 'o', 't',
    0x0d, 0x0a,
    0
};

uint8_t getSectorsPerCluster(struct spctable *table, size_t tableSize, size_t diskSize)
{
	for (size_t i = 0; i < tableSize; i++)
	{
		if (diskSize <= table[i].diskSize)
			return table[i].sectorsPerCluster;
	}
	return 0;
}

// TODO: for all of the strncpy, remove the magic number sizes

void FATBootSector::defaultBootSector12(const std::string &volumeLabel)
{
	this->bootSectorCommon();
	this->jmp[1] = 0x3C;
	this->reservedSectors = 1;
	this->totalSectors16 = DISK_SIZE_12;
	this->totalSectors32 = 0;
	this->sectorsPerCluster = 2;
	this->biosParamBlock.fat1216.driveNumber = DRIVE_NUM;
	this->biosParamBlock.fat1216.bootSignature = BOOT_SIGNATURE;
	strncpy(this->biosParamBlock.fat1216.volumeLabel, volumeLabel.c_str(), 11);
	strncpy(this->biosParamBlock.fat1216.fileSystemType, "FAT12   ", 8);
	memcpy(this->biosParamBlock.fat1216.bootCode, defaultBootCode, sizeof(defaultBootCode));

	switch(this->totalSectors16)
	{
	case 160:
		this->sectorsPerCluster = 1;
		this->rootEntryCount = 64;
		this->mediaDescriptor = 0xFE;
		this->fatSize16 = 1;
		break;
	case 180:
		this->sectorsPerCluster = 1;
		this->rootEntryCount = 64;
		this->mediaDescriptor = 0xFC;
		this->fatSize16 = 2;
		break;
	case 320:
		this->rootEntryCount = 112;
		this->mediaDescriptor = 0xFF;
		this->fatSize16 = 1;
		break;
	case 360:
		this->rootEntryCount = 112;
		this->mediaDescriptor = 0xFD;
		this->fatSize16 = 2;
		break;
	case 640:
		this->rootEntryCount = 112;
		this->mediaDescriptor = 0xFB;
		this->fatSize16 = 2;
		break;
	case 720:
		this->rootEntryCount = 112;
		this->mediaDescriptor = 0xF9;
		this->fatSize16 = 3;
		break;
	case 1200:
		this->sectorsPerCluster = 1;
		this->rootEntryCount = 224;
		this->mediaDescriptor = 0xF9;
		this->fatSize16 = 7;
		break;
	case 1232:
		this->bytesPerSector = 1024;
		this->sectorsPerCluster = 1;
		this->rootEntryCount = 192;
		this->mediaDescriptor = 0xFE;
		this->fatSize16 = 2;
		break;
	case 1440:
		this->sectorsPerCluster = 1;
		this->rootEntryCount = 224;
		this->mediaDescriptor = 0xF0;
		this->fatSize16 = 9;
		break;
	case 2880:
		this->rootEntryCount = 240;
		this->mediaDescriptor = 0xF0;
		this->fatSize16 = 9;
		break;
	default:
		this->rootEntryCount = 256;
		break;
	}

}

void FATBootSector::defaultBootSector16(const std::string &volumeLabel)
{
	this->bootSectorCommon();

	this->jmp[1] = 0x3C;
	this->reservedSectors = 1;
	this->rootEntryCount = 512;
	this->totalSectors16 = DISK_SIZE_16;
	this->totalSectors32 = 0;
	this->biosParamBlock.fat1216.driveNumber = DRIVE_NUM;
	this->biosParamBlock.fat1216.bootSignature = BOOT_SIGNATURE;
	strncpy(this->biosParamBlock.fat1216.volumeLabel, volumeLabel.c_str(), 11);
	strncpy(this->biosParamBlock.fat1216.fileSystemType, "FAT16   ", 8);
	memcpy(this->biosParamBlock.fat1216.bootCode, defaultBootCode, sizeof(defaultBootCode));
	this->sectorsPerCluster = getSectorsPerCluster(FAT16_SPC_TABLE, sizeof(FAT16_SPC_TABLE), this->totalSectors16);
}

void FATBootSector::defaultBootSector32(const std::string &volumeLabel)
{
	this->bootSectorCommon();

	this->jmp[1] = 0x58;
	this->reservedSectors = 32;
	this->rootEntryCount = 0;
	this->totalSectors16 = 0;
	this->fatSize16 = 0;
	this->totalSectors32 = DISK_SIZE_32;
	this->biosParamBlock.fat32.rootCluster = ROOT_CLUSTER;
	this->biosParamBlock.fat32.fsinfoSector = FSINFO_SECTOR;
	this->biosParamBlock.fat32.backupBootSector = BACKUP_BOOT_SECTOR;
	this->biosParamBlock.fat32.driveNumber = DRIVE_NUM;
	this->biosParamBlock.fat32.bootSignature = BOOT_SIGNATURE;
	formatFATName(volumeLabel.c_str(), this->biosParamBlock.fat32.volumeLabel, 11);
	strncpy(this->biosParamBlock.fat32.fileSystemType, "FAT32   ", 8);
	memcpy(this->biosParamBlock.fat32.bootCode, defaultBootCode, sizeof(defaultBootCode));
	this->sectorsPerCluster = getSectorsPerCluster(FAT32_SPC_TABLE, sizeof(FAT32_SPC_TABLE), this->totalSectors32);
}

void FATBootSector::bootSectorCommon(void)
{
	this->jmp[0] = 0xEB;
	this->jmp[2] = 0x90;

	strncpy(this->oemName, OEM_NAME, sizeof(this->oemName));

	this->bytesPerSector = DISK_SECTOR_SIZE;
	this->fatCount = FAT_COUNT;
	this->mediaDescriptor = MEDIA_TYPE;
	this->bootSignature[0] = 0x55;
	this->bootSignature[1] = 0xAA;

	this->sectorsPerTrack = 0;
	this->headCount = 0;
	this->hiddenSectors = 0;
}

FATBootSector::FATBootSector(const std::string &bootPath, const std::string &volumeLabel, FatType fatType)
{
	memset(this, 0, sizeof(struct FATBootSector));

	if (bootPath != "")
		fileRead(bootPath.c_str(), this, DISK_SECTOR_SIZE);
	else switch (fatType)
	{
		case FatType::FAT32: defaultBootSector32(volumeLabel); break;
		case FatType::FAT16: defaultBootSector16(volumeLabel); break;
		case FatType::FAT12: defaultBootSector12(volumeLabel); break;
	}
}
