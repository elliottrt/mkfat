#include "treeitem.h"
#include "error.h"

#include <sys/stat.h>

TreeItem::TreeItem(const TreeItem *parent, const std::filesystem::directory_entry &entry): parent(parent)
{
	
	this->artificial = false;

	if (entry.is_directory())
		this->direntry.attributes |= DIRECTORY;

	this->name = this->is_directory() ? entry.path().stem() : entry.path().filename();

	if (!entry.exists())
	{
		mkfatError(1, "file or directory '%s' does not exist\n", name.c_str());
	}

	if (!entry.is_regular_file() && !entry.is_directory())
	{
		mkfatError(1, "'%s' is not a regular file or directory\n", name.c_str());
	}

	this->direntry.setFileName(this->name.c_str());

	this->direntry.fileSize = this->is_directory() ? 0 : entry.file_size();

	struct stat sbuf;
	struct tm access, modify;
	stat(entry.path().c_str(), &sbuf);
	
	gmtime_r(&sbuf.st_atime, &access);
	gmtime_r(&sbuf.st_mtime, &modify);

	uint16_t accessdate = 0;
	accessdate |= (access.tm_mday & 0x1F); // 5 bits of day
	accessdate |= ((access.tm_mon + 1) & 0x0F) << 5; // 4 bits of month
	accessdate |= ((access.tm_year - 80) & 0x7F) << 9; // 7 bits of year
	this->direntry.lastAccessDate = accessdate;

	uint16_t modifydate = 0;
	modifydate |= (modify.tm_mday & 0x1F); // 5 bits of day
	modifydate |= ((modify.tm_mon + 1) & 0x0F) << 5; // 4 bits of month
	modifydate |= ((modify.tm_year - 80) & 0x7F) << 9; // 7 bits of year
	this->direntry.writeDate = modifydate;

	uint16_t modifytime = 0;
	modifytime |= ((modify.tm_sec / 2) & 0x1F); // 5 bits of 2sec
	modifytime |= ((modify.tm_min + 1) & 0x3F) << 5; // 6 bits of minute
	modifytime |= (modify.tm_hour & 0x1F) << 11; // 5 bits of hour
	this->direntry.writeTime = modifytime;

	// we don't support this
	this->direntry.creationTenths = 0;
	this->direntry.creation2Seconds = 0;
	this->direntry.creationDate = 0;
}

TreeItem::TreeItem(const TreeItem *parent, const std::string &name, bool isDirectory): parent(parent)
{
	this->name = name;
	this->direntry.setFileName(name.c_str());
	this->artificial = true;
	this->direntry.fileSize = 0;

	if (isDirectory)
	{
		this->direntry.attributes |= DIRECTORY;
		this->createDots();
	}
		
}

TreeItem::~TreeItem(void) {
	for (TreeItem *item : children) {
		delete item;
	}
}

void TreeItem::createDots(void)
{

	if (this->name == "." || this->name == "..")
		return;

	this->dot = new TreeItem(this, ".", true);
	this->dot->direntry.lastAccessDate = this->direntry.lastAccessDate;
	this->dot->direntry.writeDate = this->direntry.writeDate;
	this->dot->direntry.writeTime = this->direntry.writeTime;
	this->dot->direntry.creationTenths = 0;
	this->dot->direntry.creation2Seconds = 0;
	this->dot->direntry.creationDate = 0;
	this->children.push_back(this->dot);

	this->dotdot = new TreeItem(this, "..", true);
	this->dotdot->direntry.lastAccessDate = this->direntry.lastAccessDate;
	this->dotdot->direntry.writeDate = this->direntry.writeDate;
	this->dotdot->direntry.writeTime = this->direntry.writeTime;
	this->dotdot->direntry.creationTenths = 0;
	this->dotdot->direntry.creation2Seconds = 0;
	this->dotdot->direntry.creationDate = 0;
	this->children.push_back(this->dotdot);

}

void TreeItem::findChildren(const std::filesystem::directory_entry &entry)
{
	for (const auto &childEntry : std::filesystem::directory_iterator(entry.path())) 
	{
		// TODO: find some way to do this without manually checking filename - note that filenames starting with . aren't valid fat filenames, could be skipped?
		if (childEntry.path().filename() == ".DS_Store") continue;

		TreeItem *child = new TreeItem(this, childEntry);
		if (child->is_directory()) 
		{
			child->createDots();
			child->findChildren(childEntry);
		}
        this->children.push_back(child);
	}
}

std::string TreeItem::path(void) const
{
	std::string _path = this->name;
	const TreeItem *ancestor = this->parent;
	while (ancestor != NULL)
	{
		_path.insert(0, ancestor->name + "/");
		ancestor = ancestor->parent;
	}
	return _path;
}

bool TreeItem::is_directory(void) const
{
	return (this->direntry.attributes & DIRECTORY) != 0;
}

size_t TreeItem::size(void) const {
	if (is_directory())
		return children.size() * sizeof(struct direntry);
	else
		return direntry.fileSize;
}
