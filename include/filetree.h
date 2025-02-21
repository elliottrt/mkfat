#ifndef _TOOLS_FILETREE
#define _TOOLS_FILETREE

#include <string>
#include <vector>
#include <filesystem>

#include "common.h"

struct TreeItem
{
	std::vector<TreeItem *> children;
	std::string name;

	TreeItem *parent, *dot, *dotdot;

	struct direntry direntry;
	bool artificial = true;

	TreeItem(TreeItem *parent, const std::filesystem::directory_entry &entry);
	TreeItem(TreeItem *parent, const std::string &name, bool isDirectory);

	void createDots(void);

	void findChildren(const std::filesystem::directory_entry &entry);

	std::string path(void) const;
	bool directory() const;

};

class FileTree
{
	
	std::filesystem::path path;

public:

	TreeItem *root;

	FileTree(const std::string &path, const std::string &volumeLabel);
	FileTree(const std::string &volumeLabel);

	void collect(void);

};

#endif
