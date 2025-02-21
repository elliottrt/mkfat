#ifndef _TOOLS_FILETREE
#define _TOOLS_FILETREE

#include <string>
#include <vector>
#include <filesystem>

#include "common.h"

struct TreeItem
{
	// TODO: does this have to be a pointer? or could it be just TreeItem
	std::vector<TreeItem *> children;
	std::string name;

	const TreeItem *parent; 
	TreeItem *dot;
	TreeItem *dotdot;

	struct direntry direntry;
	bool artificial = true;

	TreeItem(const TreeItem *parent, const std::filesystem::directory_entry &entry);
	TreeItem(const TreeItem *parent, const std::string &name, bool isDirectory);

	void createDots(void);

	void findChildren(const std::filesystem::directory_entry &entry);

	std::string path(void) const;
	bool is_directory() const;

};

class FileTree
{
	
	std::filesystem::path path;

public:

	TreeItem *root;

	FileTree(const std::string &path, const std::string &volumeLabel);
	FileTree(const std::string &volumeLabel);

	// TODO: filetree destructor that frees all memory

	void collect(void);

};

#endif
