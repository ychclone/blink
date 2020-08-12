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

	long m_fileId;
	QString m_fileName;
	QString m_fileLastModified;

	long m_fileSize;
};

#endif // COUTPUT_ITEM_H




