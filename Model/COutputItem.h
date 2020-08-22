#ifndef COUTPUT_ITEM_H
#define COUTPUT_ITEM_H

#include <QTextStream>
#include <QString>

class COutputItem
{
public:

    COutputItem();
	COutputItem(long fileId, const QString& fileName, const QString& fileLastModified, long fileSize);

    virtual ~COutputItem() {};

	long fileId_;
	QString fileName_;
	QString fileLastModified_;

	long fileSize_;
};

#endif // COUTPUT_ITEM_H




