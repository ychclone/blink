#ifndef CSOURCE_FILE_LIST_H
#define CSOURCE_FILE_LIST_H

#include <QString>
#include <QFile>
#include <QDirIterator>
#include <QDir>

#include <QDebug>
#include <QDateTime>

#include "../CFileItem.h"

typedef QList<CFileItem> T_FileItemList;

class CSourceFileList
{
public:
	CSourceFileList();
	~CSourceFileList();

	static int loadFileList(const QString& fileListFilename, T_FileItemList& resultFileList);
	static int saveFileList(const QString& fileListFilename, const QMap<long, CFileItem>& resultFileList); 

	static int generateFileList(const QString& resultFilename, const QString& srcDir, const QStringList& nameFilters, T_FileItemList& resultFileList, bool bSaveToFile = true);

	static const char* kFILE_LIST;
};



#endif // CSOURCE_FILE_LIST_H


