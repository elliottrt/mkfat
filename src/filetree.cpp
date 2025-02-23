#include "filetree.h"
#include "fatformat.h"

FileTree::FileTree(const std::string &path, const std::string &volumeLabel)
{
	// if there is no root directory provided, we make an artificial one
	if (path == "")
	{
		this->root = new TreeItem(NULL, "root", true);
	}
	else
	{
		this->path = std::filesystem::path(path);
		this->root = new TreeItem(NULL, std::filesystem::directory_entry(this->path));
	}

	TreeItem *volumeLabelItem = new TreeItem(this->root, volumeLabel, false);
	// TODO: replace magic number 11 here and other places
	formatFATName(volumeLabel.c_str(), volumeLabelItem->direntry.fileName, 11);
	volumeLabelItem->direntry.attributes |= VOLUME_ID;
	volumeLabelItem->direntry.attributes |= SYSTEM;
	this->root->children.push_back(volumeLabelItem);
}

FileTree::FileTree(const std::string &volumeLabel): FileTree("", volumeLabel)
{

}

FileTree::~FileTree(void)
{
	if (root)
	{
		delete root;
	}
}

void FileTree::collect(void)
{
	if (this->root && !this->root->artificial)
		this->root->findChildren();
}

