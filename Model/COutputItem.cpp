#include "COutputItem.h"

COutputItem::COutputItem()
{
    m_fileId = 0;
	m_fileName = "";
    m_fileLastModified = ""; 
	m_fileSize = 0;
}

COutputItem::COutputItem(long fileId, const QString& fileName, const QString& fileLastModified, long fileSize)
{
	m_fileId = fileId;

    m_fileName = fileName;
    m_fileLastModified = fileLastModified;

	m_fileSize = fileSize; 
}




