#include "filetree.h"
#include "fatformat.h"
#include "error.h"

FileTree::FileTree(const std::string &path, const std::string &volumeLabel)
{
	// if there is no root directory provided, we make an artificial one
	if (path == "")
	{
		this->root = new TreeItem(NULL, "root", true);
	}
	else
	{
		char last = path.at(path.length() - 1);
		std::string rootPath = path;
		if (last == '/')
			rootPath.erase(path.length() - 1);
		this->path = std::filesystem::path(rootPath);

		this->root = new TreeItem(NULL, std::filesystem::directory_entry(this->path));
	}

	TreeItem *volumeLabelItem = new TreeItem(this->root, volumeLabel, false);
	formatFATName(volumeLabel.c_str(), volumeLabelItem->direntry.fileName, 11);
	volumeLabelItem->direntry.attributes |= VOLUME_ID;
	volumeLabelItem->direntry.attributes |= SYSTEM;
	this->root->children.push_back(volumeLabelItem);
}

FileTree::FileTree(const std::string &volumeLabel)
{
	this->root = new TreeItem(NULL, "root", true);

	TreeItem *volumeLabelItem = new TreeItem(this->root, volumeLabel, false);
	formatFATName(volumeLabel.c_str(), volumeLabelItem->direntry.fileName, 11);
	volumeLabelItem->direntry.attributes |= VOLUME_ID;
	volumeLabelItem->direntry.attributes |= SYSTEM;
	this->root->children.push_back(volumeLabelItem);
}

FileTree::~FileTree(void) {
	if (root) {
		delete root;
	}
}

void FileTree::collect(void)
{
	if (this->root && !this->root->artificial)
		this->root->findChildren(std::filesystem::directory_entry(this->path));
}

