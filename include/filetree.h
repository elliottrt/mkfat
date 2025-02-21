#ifndef _TOOLS_FILETREE
#define _TOOLS_FILETREE

#include <filesystem>

#include "treeitem.h"

class FileTree
{
	
	std::filesystem::path path;

public:

	TreeItem *root;

	FileTree(const std::string &path, const std::string &volumeLabel);
	FileTree(const std::string &volumeLabel);

	~FileTree(void);

	// iterates through the children of the root directory,
	// collecting directories and files to include in the tree
	void collect(void);

};

#endif
