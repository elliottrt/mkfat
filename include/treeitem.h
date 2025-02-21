#ifndef _MKFAT_TREEITEM
#define _MKFAT_TREEITEM

#include <string>
#include <vector>
#include <filesystem>

#include "common.h"

struct TreeItem
{
	std::vector<TreeItem *> children;
	std::string name;

	// real path of this item, not relative to the new file system
	std::filesystem::path realPath;

	const TreeItem *parent; 
	TreeItem *dot;
	TreeItem *dotdot;

	struct direntry direntry;
	bool artificial;

	TreeItem(const TreeItem *parent, const std::filesystem::directory_entry &entry);
	TreeItem(const TreeItem *parent, const std::string &name, bool isDirectory);
	~TreeItem(void);

	// for directories, create the '.' and '..' entries
	void createDots(void);
	// for directories, find all files in the directory
	void findChildren(void);

	bool is_directory() const;

	size_t size(void) const;

};

#endif
