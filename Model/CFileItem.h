#ifndef CFILE_ITEM_H
#define CFILE_ITEM_H

#include <QTextStream>
#include <QString>

class CFileItem
{
public:

    CFileItem();
	CFileItem(long fileId, const QString& fileName, const QString& fileLastModified, long fileSize);

    virtual ~CFileItem() {};

	long fileId_;
	QString fileName_;
	QString fileLastModified_;

	long fileSize_;
};

#endif // CFILE_ITEM_H




