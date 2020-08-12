#ifndef CSOURCE_FILE_LIST_H
#define CSOURCE_FILE_LIST_H

#include <QString>
#include <QFile>
#include <QDirIterator>
#include <QDir>

#include <QDebug>
#include <QDateTime>

#include "../COutputItem.h"

typedef QList<COutputItem> T_OutputItemList;

class CSourceFileList
{
public:
	CSourceFileList();
	~CSourceFileList();

	static int loadFileList(const QString& fileListFilename, T_OutputItemList& resultFileList);
	static int saveFileList(const QString& fileListFilename, const QMap<long, COutputItem>& resultFileList); 

	static int generateFileList(const QString& resultFilename, const QString& srcDir, const QStringList& nameFilters, T_OutputItemList& resultFileList, bool bSaveToFile = true);

	static const char* kFILE_LIST;
};



#endif // CSOURCE_FILE_LIST_H


