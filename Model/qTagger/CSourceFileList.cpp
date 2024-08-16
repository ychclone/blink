#include "CSourceFileList.h"

const char* CSourceFileList::kFILE_LIST= "fileList.txt";

CSourceFileList::CSourceFileList()
{


}

CSourceFileList::~CSourceFileList()
{

}

int CSourceFileList::loadFileList(const QString& fileListFilename, T_FileItemList& resultFileList)
{
	bool bListFileOpenResult;
	QFile currentListFile(fileListFilename);
	CFileItem fileItem;
	QString lineItem;

	bListFileOpenResult = currentListFile.open(QIODevice::ReadOnly | QIODevice::Text);

	if (!bListFileOpenResult) {
		qDebug() << "Cannot open list file (" << fileListFilename << ") for reading!" << Qt::endl;
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
			fileItem.fileId_ = lineItem.section('\t', 0, 0).toLong();
			currentEntry = lineItem.section('\t', 1, 1);

            filePathIndex = currentEntry.lastIndexOf("/");
			if (filePathIndex >= 0) { // full path
				fileItem.fileName_ = currentEntry;
				lastFilePath = currentEntry.left(filePathIndex); // index start from 0, exclude separator
			} else {
                fileItem.fileName_ = lastFilePath + "/" + currentEntry;
			}

            fileItem.fileLastModified_ = lineItem.section('\t', 2, 2);
			fileItem.fileSize_ = lineItem.section('\t', 3, 3).toULongLong();

			resultFileList << fileItem;
        }
		currentListFile.close();
		return 0;
	}
	return 0;
}

int CSourceFileList::saveFileList(const QString& fileListFilename, const QMap<long, CFileItem>& resultFileList)
{
	QFile currentListFile(fileListFilename);

	if (!currentListFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug() << "Cannot open list file (" << fileListFilename << ") for writing!" << Qt::endl;
	}

	QTextStream listFileStream(&currentListFile);

	CFileItem fileItem;

	QString lastFilePath = "";
	int filePathIndex = 0;

	QString currentFileName = "";
	QString currentPath = "";

	foreach (const CFileItem& fileItem, resultFileList) {
		// file id
		listFileStream << QString::number(fileItem.fileId_);
		listFileStream << "\t";

		// filename
        filePathIndex = fileItem.fileName_.lastIndexOf(QDir::separator());
		if (filePathIndex >= 0) {
			currentFileName = fileItem.fileName_.mid(filePathIndex + 1); // exclude separator
			currentPath = fileItem.fileName_.left(filePathIndex);

			if (currentPath == lastFilePath) {
				listFileStream << currentFileName;
			} else {
				listFileStream << currentPath;
				lastFilePath = currentPath;
			}
		}

		listFileStream << "\t";

		// last modified datetime
		listFileStream << fileItem.fileLastModified_;
		listFileStream << "\t";

		// file size
		listFileStream << QString::number(fileItem.fileSize_);
		listFileStream << "\n";
	}

	currentListFile.flush();
	currentListFile.close();
	return 0;
}

bool isBinaryFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Cannot open file (" << filePath << ") for reading!" << Qt::endl;
        return false;
    }

    QByteArray fileData = file.read(1024); // Read the first 1024 bytes
    file.close();

    // Check for Unicode BOM header
    if (fileData.startsWith("\xEF\xBB\xBF") || // UTF-8 BOM
        fileData.startsWith("\xFF\xFE") ||     // UTF-16 (LE) BOM
        fileData.startsWith("\xFE\xFF")) {     // UTF-16 (BE) BOM
        return false;
    }

    // Check for binary content
    for (char byte : fileData) {
        if (byte == '\0') {
            return true;
        }
    }

    return false;
}


int CSourceFileList::generateFileList(const QString& resultFilename, const QString& srcDir, const QStringList& nameFilters, T_FileItemList& resultFileList, const QStringList& defaultDirsToExclude, const QStringList& defaultFileMasksToExclude, bool bSaveToFile)
{
	QFile currentListFile(resultFilename);

	QTextStream listFileStream;

	if (bSaveToFile) {
		if (!currentListFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qDebug() << "Cannot open list file (" << resultFilename << ") for writing!" << Qt::endl;
		}

		listFileStream.setDevice(&currentListFile);
	}

	QDirIterator iter(srcDir, nameFilters, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories|QDirIterator::FollowSymlinks);
	CFileItem fileItem;
	QString lastModifiedDateTime;
	QString fileSizeStr;

	long fileId = 0;

	QString lastFilePath = "";

	QString filePath;
	QString currentFilePath;
	QString currentFileName;

	QStringList fileList;

	while (iter.hasNext()) {
		filePath = iter.next();
	
		currentFilePath = QFileInfo(filePath).path();
		currentFileName = QFileInfo(filePath).fileName();

		bool excludeDir = false, excludeFile = false;

		for (const QString& dirToExclude : defaultDirsToExclude) {
			if (currentFilePath.startsWith(srcDir + "/" + dirToExclude)) {
				excludeDir = true;
				break;
			}
		}

		if (!excludeDir) {
			for (const QString& fileMaskToExclude : defaultFileMasksToExclude) {
				QRegularExpression regExp(QRegularExpression::wildcardToRegularExpression(fileMaskToExclude), QRegularExpression::CaseInsensitiveOption);
				if (regExp.match(currentFileName).hasMatch()) {
					excludeFile = true;
					break;
				}
			}
		}

		if (excludeFile || excludeDir) {
			qDebug() << "Excluded file: " << currentFilePath << "/" << currentFileName;
			continue;
		}

		fileList << filePath;
	}

	for (const QString& filePath : fileList) {
		if (isBinaryFile(filePath)) {	// skip binary file
			qDebug() << "Binary file: " << filePath;
			continue;
		}

		QFileInfo fileInfo = QFileInfo(filePath);
		fileItem.fileName_ = filePath;

		fileInfo.setCaching(false);

		fileItem.fileId_ = fileId;

		lastModifiedDateTime = fileInfo.lastModified().toString("dd/MM/yyyy hh:mm:ss");
		fileItem.fileLastModified_ = lastModifiedDateTime; // update fileItem last modified datetime

		fileItem.fileSize_ = fileInfo.size(); // update fileItem file size

		if (bSaveToFile) {
			// file id
			listFileStream << QString::number(fileItem.fileId_);
			listFileStream << "\t";

			// filename; write filename only if same path as previous one
			if (fileInfo.path() == lastFilePath) {
				listFileStream << fileInfo.fileName();
			} else {
				listFileStream << fileItem.fileName_;
				lastFilePath = fileInfo.path(); // update last path
			}
			listFileStream << "\t";

			// last modified datetime
			listFileStream << lastModifiedDateTime;
			listFileStream << "\t";

			// file size
			listFileStream << QString::number(fileItem.fileSize_);
			listFileStream << "\n";

			listFileStream.flush();
		}

		// append to result file list
		resultFileList << fileItem;

		fileId++;
	}

	if (bSaveToFile) {
		currentListFile.close();
	}

	return 0;
}





