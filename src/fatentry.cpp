#include "fatentry.h"

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

