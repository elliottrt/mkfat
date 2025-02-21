#ifndef _MKFAT_WRITEABLE
#define _MKFAT_WRITEABLE

#include <stdio.h>
#include "diskimage.h"

class FATWriteable
{
public:
	virtual void write(FATDiskImage *image) const = 0;
};

#endif
