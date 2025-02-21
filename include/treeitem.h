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

	const TreeItem *parent; 
	TreeItem *dot;
	TreeItem *dotdot;

	struct direntry direntry;
	bool artificial = true;

	TreeItem(const TreeItem *parent, const std::filesystem::directory_entry &entry);
	TreeItem(const TreeItem *parent, const std::string &name, bool isDirectory);
	~TreeItem(void);

	void createDots(void);

	void findChildren(const std::filesystem::directory_entry &entry);

	std::string path(void) const;
	bool is_directory() const;

	size_t size(void) const;

};

#endif
