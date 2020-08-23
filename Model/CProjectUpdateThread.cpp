#include "CProjectUpdateThread.h"

CProjectUpdateThread::CProjectUpdateThread(QObject *parent)
: QThread(parent),
  bCancelUpdate_(false)
{

}

void CProjectUpdateThread::setCurrentProjectItem(const CProjectItem& projectItem)
{
	projectItem_ = projectItem;
}

void CProjectUpdateThread::setRebuildTag(bool bRebuildTag)
{
	bRebuildTag_ = bRebuildTag;
}

void CProjectUpdateThread::initStep(int totalStep)
{
    stepCompleted_ = 0;
	totalStep_ = totalStep;

	emit percentageCompleted(0); // emit 0 initially
}

void CProjectUpdateThread::finishOneStep()
{
	int endPercent;
	stepCompleted_++;
	endPercent = stepCompleted_ * (100 / totalStep_);
	emit percentageCompleted(endPercent);

	if (stepCompleted_ >= totalStep_) {
		emit percentageCompleted(100); // always 100% if finish all steps
	}
}

void CProjectUpdateThread::finishAllStep()
{
	emit percentageCompleted(100);
}

bool CProjectUpdateThread::runCommand(const QString& program, const QString& workDir, const QString& redirectFile)
{
	QString errStr;
	CRunCommand::ENUM_RunCommandErr cmdErr;

	cmdErr = (CRunCommand::ENUM_RunCommandErr) cmd_.startRun(program, workDir, redirectFile, errStr);

	switch (cmdErr) {
		case CRunCommand::E_RUNCMD_NO_ERROR:
			break;
		case CRunCommand::E_RUNCMD_CANCELLED:
			emit cancelledTagBuild();
			return false;
			break;
		case CRunCommand::E_RUNCMD_FAILSTART:
			emit errorDuringRun(errStr);
			return false;
			break;
		case CRunCommand::E_RUNCMD_ERRRUN:
			emit errorDuringRun(errStr);
			return false;
			break;
		case CRunCommand::E_RUNCMD_CRASHED:
			emit errorDuringRun(errStr);
			return false;
			break;
		default:
			break;
	}

	finishOneStep(); // for updating progress bar
	return true;
}

void CProjectUpdateThread::cancelUpdate()
{
	bCancelUpdate_ = true;
	cmd_.cancelCommand(true);
}

int CProjectUpdateThread::countTotalRunCmd()
{
	int i, totalCmd;
	QString cmdKey, cmdStr;

	CConfigManager* confManager;
	confManager	= CConfigManager::getInstance();

	totalCmd = 0;
	for (i = 1; i < MAX_SUPPORTED_RUN_COMMAND+1; i++) {
		cmdKey = QString("UpdateTagRunCmd") + QString::number(i);
        cmdStr = confManager->getAppSettingValue(cmdKey).toString();

		if (cmdStr.isEmpty()) { // break if no more command to run
			break;
		}
		totalCmd = i;
	}
	return totalCmd;
}

int CProjectUpdateThread::createTag(QTagger& tagger, const T_FileItemList& inputFileList)
{
	long i = 0;
	unsigned long fileId = 0;
	QString currentFilePath;

	long totalFile = inputFileList.size();
	tagger.initKeywordFileTokenMap();

	for (i = 0; i < totalFile; i++) {
		currentFilePath = inputFileList.at(i).fileName_; // index start from 0
		fileId = inputFileList.at(i).fileId_;

		qDebug() << fileId << ":" << currentFilePath;

		tagger.parseSourceFile(fileId, currentFilePath);

		emit percentageCompleted(static_cast<int> (static_cast<float> (i) / totalFile * 100));
	}
	return 0;
}

void CProjectUpdateThread::run()
{
	bool bRunResult;

	QString cmdKey, cmdStr;
	QString srcTagName, targetTagName;
	QString errStr;

	CConfigManager* confManager;
	confManager	= CConfigManager::getInstance();

	bCancelUpdate_ = false;

	// current directory
	QDir currentDir(QDir::currentPath());

	// using absoluteFilePath so relative and absolute path also possible
	QString tmpDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TmpDir").toString());

	// using absoluteFilePath so relative and absolute path also possible
	QString tagDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TagDir").toString() + "/" + projectItem_.name_);
	QString tagName = "tags";

	QString fileListFilename = tagDir + "/" + CSourceFileList::kFILE_LIST;

	initStep(countTotalRunCmd() + 2); // total run command + 2 (initial file list, tag update)

	if (!currentDir.exists(tagDir)) {
		currentDir.mkpath(tagDir);
	}

	if (!currentDir.exists(tmpDir)) {
		currentDir.mkpath(tmpDir);
	}

	/* first step, recursively list the source and header files to currentListFile */
	QStringList nameFilters;
	QStringList srcMaskList = projectItem_.srcMask_.split(" ");
	QStringList headerMaskList = projectItem_.headerMask_.split(" ");

	nameFilters = srcMaskList + headerMaskList;

	if (bRebuildTag_) {
		T_FileItemList resultFileList;

		CSourceFileList::generateFileList(fileListFilename, projectItem_.srcDir_, nameFilters, resultFileList);

		// create tag
		QTagger tagger;

		createTag(tagger, resultFileList);

		tagger.writeTagDb(tagDir + "/" + QTagger::kQTAG_DEFAULT_TAGDBNAME);

		finishOneStep(); // for updating progress bar

		if (bCancelUpdate_) {
			emit cancelledTagBuild();
			return;
		}

		finishOneStep(); // for updating progress bar
	} else { // update tag

		T_FileItemList newFileList;
		T_FileItemList existingFileList;

		QMap<long, long> fileIdDeletedMap;
		QMap<long, long> fileIdModifiedMap;
		QMap<long, long> fileIdCreatedMap;

		int bListFileOpenResult;
		int bListFileSaveResult;

		// key: filename
		QMap<QString, CFileItem> existingFileInfoMap;
		QMap<QString, CFileItem> newFileInfoMap;

		// key: file id
		QMap<long, CFileItem> fileMapIdxByFileId;

		// key: file id
		QMap<long, long> assignedFileId;

		CFileItem updatedFileItem;

		// existing file list i.e. before update tag
		bListFileOpenResult = CSourceFileList::loadFileList(fileListFilename, existingFileList);

		// put the existing file list into a map
		foreach (const CFileItem& fileItem, existingFileList) {
			existingFileInfoMap[fileItem.fileName_] = fileItem;

			assignedFileId[fileItem.fileId_] = fileItem.fileId_; // file id assigned
		}

		finishOneStep(); // for updating progress bar

		// new file list, for this update tag, not save the file list to file yet as need to update file id
		CSourceFileList::generateFileList(fileListFilename, projectItem_.srcDir_, nameFilters, newFileList, false);

		long newFileId = 0;

		// put the new file list into a map
		foreach (const CFileItem& fileItem, newFileList) {
			updatedFileItem = fileItem;

			// use previous file id if file exist before
			QMap<QString, CFileItem>::iterator it = existingFileInfoMap.find(fileItem.fileName_);

			if (it == existingFileInfoMap.end()) { // not exist before

				if (newFileId == 0) { // assign to largest key +1 if not exist yet
					QMapIterator<long, long> itMap(assignedFileId);
					itMap.toBack();
					itMap.previous();
					newFileId = itMap.key() + 1;
				}

				updatedFileItem.fileId_ = newFileId;
				newFileId++; // the next file id will be +1

			} else {  // exist before
				updatedFileItem.fileId_ = it.value().fileId_; // reuse previous file id for assignment
			}

			newFileInfoMap[updatedFileItem.fileName_] = updatedFileItem;
			fileMapIdxByFileId[updatedFileItem.fileId_] = updatedFileItem;

			qDebug() << "filename in newlist" << updatedFileItem.fileName_;
			qDebug() << "fileid in newlist" << updatedFileItem.fileId_;
		}

		bListFileSaveResult = CSourceFileList::saveFileList(fileListFilename, fileMapIdxByFileId);

		finishOneStep(); // for updating progress bar

		// deleted file, modified file

		foreach (const CFileItem& fileItem, existingFileList) {

			QMap<QString, CFileItem>::iterator it = newFileInfoMap.find(fileItem.fileName_);

			if (it == newFileInfoMap.end()) { // deleted file: in existingFileList, but not in newModifyTimeMap
				qDebug() << "Deleted file: " << fileItem.fileName_;
                fileIdDeletedMap[fileItem.fileId_] = fileItem.fileId_;
			} else {
				if (it.value().fileLastModified_ != fileItem.fileLastModified_) { // modified file: check by file modification date time
					qDebug() << "Modified file: " << fileItem.fileName_;
					fileIdModifiedMap[fileItem.fileId_] = fileItem.fileId_;
				} else {
                    qDebug() << "Same file: " << fileItem.fileName_;
				}

				newFileInfoMap.erase(it); // remove checked item so remaining would be new files
			}
		}

		// new file: the remaining files
		QMap<QString, CFileItem>::const_iterator mapIt = newFileInfoMap.constBegin();
		while (mapIt != newFileInfoMap.constEnd()) {
			qDebug() << "New file: " << mapIt.key();
			fileIdCreatedMap[mapIt.value().fileId_] = mapIt.value().fileId_;
			++mapIt;
		}

		qDebug() << "fileIdModifiedMap:" << fileIdModifiedMap;
		qDebug() << "fileIdDeletedMap:" << fileIdDeletedMap;
		qDebug() << "fileIdCreatedMap:" << fileIdCreatedMap;

		// update tag
		QTagger tagger;

		tagger.updateTag(fileMapIdxByFileId, tagDir + "/" + QTagger::kQTAG_DEFAULT_TAGDBNAME, fileIdCreatedMap, fileIdModifiedMap, fileIdDeletedMap);
		tagger.writeTagDb(tagDir + "/" + QTagger::kQTAG_DEFAULT_TAGDBNAME);

		finishOneStep(); // for updating progress bar

		if (bCancelUpdate_) {
			emit cancelledTagBuild();
			return;
		}

		finishOneStep(); // for updating progress bar
	}

	for (int i = 1; i < MAX_SUPPORTED_RUN_COMMAND+1; i++) { // from 1 to MAX_SUPPORTED_RUN_COMMAND
		cmdKey = QString("UpdateTagRunCmd") + QString::number(i);
        cmdStr = confManager->getAppSettingValue(cmdKey).toString();

		if (cmdStr.isEmpty()) { // break if no more command to run
			qDebug() << "cmdStr empty, key: " << cmdKey << "str: " << cmdStr;
			break;
		} else {
			cmdStr.replace("$tmpList", fileListFilename);
			cmdStr.replace("$tagDir", tagDir);
			bRunResult = runCommand(cmdStr, projectItem_.srcDir_);
			if (!bRunResult) { // also break if errors during run
				break;
			}
		}
	}

    // for ctag
	if (bRunResult) { // only continue if runCommand without problem
		srcTagName = projectItem_.srcDir_ + "/" + tagName;
		targetTagName = tagDir + "/" + tagName;
		QFile::rename(srcTagName, targetTagName);
		//QFile::remove(srcTagName);
		finishAllStep(); // finish all step as it's the last one
	}


}





