#include "COutputItem.h"

COutputItem::COutputItem()
{
    fileId_ = 0;
	fileName_ = "";
    fileLastModified_ = ""; 
	fileSize_ = 0;
}

COutputItem::COutputItem(long fileId, const QString& fileName, const QString& fileLastModified, long fileSize)
{
	fileId_ = fileId;

    fileName_ = fileName;
    fileLastModified_ = fileLastModified;

	fileSize_ = fileSize; 
}




