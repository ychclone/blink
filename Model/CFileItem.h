#ifndef COUTPUT_ITEM_H
#define COUTPUT_ITEM_H

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

#endif // COUTPUT_ITEM_H




