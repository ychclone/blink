#include "CSourceFileList.h"

const char* CSourceFileList::kFILE_LIST= "fileList.txt";

CSourceFileList::CSourceFileList()
{


}

CSourceFileList::~CSourceFileList()
{

}

int CSourceFileList::loadFileList(const QString& fileListFilename, T_OutputItemList& resultFileList)
{
	bool bListFileOpenResult;
	QFile currentListFile(fileListFilename);
	COutputItem outputItem;
	QString lineItem;

	bListFileOpenResult = currentListFile.open(QIODevice::ReadOnly | QIODevice::Text);

	if (!bListFileOpenResult) {
		qDebug() << "Cannot open list file (" << fileListFilename << ") for reading!" << endl;
		return -1;
	}

	QTextStream listFileStream(&currentListFile);

	QString lastFilePath = "";
	int filePathIndex = 0;

	QString currentFileName = "";
	QString currentPath = "";

	QString currentEntry = "";

	if (bListFileOpenResult) {
	   	while (!listFileStream.atEnd()) {
		   	lineItem = listFileStream.readLine();
			lineItem = lineItem.trimmed();

			// start, end section
			outputItem.m_fileId = lineItem.section('\t', 0, 0).toLong();
			currentEntry = lineItem.section('\t', 1, 1);

            filePathIndex = currentEntry.lastIndexOf("/");
			if (filePathIndex >= 0) { // full path
				outputItem.m_fileName = currentEntry;
				lastFilePath = currentEntry.left(filePathIndex); // index start from 0, exclude separator
			} else {
                outputItem.m_fileName = lastFilePath + "/" + currentEntry;
			}

            outputItem.m_fileLastModified = lineItem.section('\t', 2, 2);
			outputItem.m_fileSize = lineItem.section('\t', 3, 3).toLong();

			resultFileList << outputItem;
        }
		currentListFile.close();
		return 0;
	}
	return 0;
}

int CSourceFileList::saveFileList(const QString& fileListFilename, const QMap<long, COutputItem>& resultFileList)
{
	QFile currentListFile(fileListFilename);

	if (!currentListFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Cannot open list file (" << fileListFilename << ") for writing!" << endl;
	}

	QTextStream listFileStream(&currentListFile);

	COutputItem outputItem;

	QString lastFilePath = "";
	int filePathIndex = 0;

	QString currentFileName = "";
	QString currentPath = "";

	foreach (const COutputItem& outputItem, resultFileList) {
		// file id
		listFileStream << QString::number(outputItem.m_fileId);
		listFileStream << "\t";

		// filename
        filePathIndex = outputItem.m_fileName.lastIndexOf(QDir::separator());
		if (filePathIndex >= 0) {
			currentFileName = outputItem.m_fileName.mid(filePathIndex + 1); // exclude separator
			currentPath = outputItem.m_fileName.left(filePathIndex);

			if (currentPath == lastFilePath) {
				listFileStream << currentFileName;
			} else {
				listFileStream << currentPath;
				lastFilePath = currentPath;
			}
		}

		listFileStream << "\t";

		// last modified datetime
		listFileStream << outputItem.m_fileLastModified;
		listFileStream << "\t";

		// file size
		listFileStream << QString::number(outputItem.m_fileSize);
		listFileStream << "\n";
	}

	currentListFile.flush();
	currentListFile.close();
	return 0;
}

int CSourceFileList::generateFileList(const QString& resultFilename, const QString& srcDir, const QStringList& nameFilters, T_OutputItemList& resultFileList, bool bSaveToFile)
{
	QFile currentListFile(resultFilename);

	QTextStream listFileStream;

	if (bSaveToFile) {
		if (!currentListFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << "Cannot open list file (" << resultFilename << ") for writing!" << endl;
		}

		listFileStream.setDevice(&currentListFile);
	}

	QDirIterator iter(srcDir, QDirIterator::Subdirectories|QDirIterator::FollowSymlinks);
	COutputItem outputItem;
	QString lastModifiedDateTime;
	QString fileSizeStr;

	long fileId = 0;

	QString lastFilePath = "";

	QString currentPath;
	QString currentFileName;

	while (iter.hasNext()) {
		if (QDir::match(nameFilters, iter.fileName())) {
			outputItem.m_fileName = iter.filePath();

			QFileInfo fileInfo(outputItem.m_fileName);
			fileInfo.setCaching(false);

            outputItem.m_fileId = fileId;

			lastModifiedDateTime = fileInfo.lastModified().toString("dd/MM/yyyy hh:mm:ss");
			outputItem.m_fileLastModified = lastModifiedDateTime; // update outputItem last modified datetime

			outputItem.m_fileSize = fileInfo.size(); // update outputItem file size

			if (bSaveToFile) {
				// file id
				listFileStream << QString::number(outputItem.m_fileId);
				listFileStream << "\t";

				// filename; write filename only if same path as previous one
				if (fileInfo.path() == lastFilePath) {
					listFileStream << fileInfo.fileName();
				} else {
					listFileStream << outputItem.m_fileName;
					lastFilePath = fileInfo.path(); // update last path
				}
				listFileStream << "\t";

				// last modified datetime
				listFileStream << lastModifiedDateTime;
				listFileStream << "\t";

				// file size
				listFileStream << QString::number(outputItem.m_fileSize);
				listFileStream << "\n";

				listFileStream.flush();
			}

			// append to result file list
			resultFileList << outputItem;

			fileId++;
		}
		iter.next();
	}

	if (bSaveToFile) {
		currentListFile.close();
	}

	return 0;
}





