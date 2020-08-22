#include "CProfileUpdateThread.h"

CProfileUpdateThread::CProfileUpdateThread(QObject *parent)
: QThread(parent), 
  bCancelUpdate_(false)
{

}

void CProfileUpdateThread::setCurrentProfileItem(const CProfileItem& profileItem)
{
	profileItem_ = profileItem;
}

void CProfileUpdateThread::setRebuildTag(bool bRebuildTag)
{
	bRebuildTag_ = bRebuildTag;
}

void CProfileUpdateThread::initStep(int totalStep)
{
    stepCompleted_ = 0;
	totalStep_ = totalStep;
	
	emit percentageCompleted(0); // emit 0 initially 
}

void CProfileUpdateThread::finishOneStep()
{
	int endPercent;
	stepCompleted_++;    
	endPercent = stepCompleted_ * (100 / totalStep_);
	emit percentageCompleted(endPercent); 

	if (stepCompleted_ >= totalStep_) {
		emit percentageCompleted(100); // always 100% if finish all steps
	}
}

void CProfileUpdateThread::finishAllStep()
{
	emit percentageCompleted(100);
}

bool CProfileUpdateThread::runCommand(const QString& program, const QString& workDir, const QString& redirectFile)
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

void CProfileUpdateThread::cancelUpdate()
{
	bCancelUpdate_ = true;
	cmd_.cancelCommand(true);
}

int CProfileUpdateThread::countTotalRunCmd()
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

void CProfileUpdateThread::run()
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
	QString tagDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TagDir").toString() + "/" + profileItem_.name_);
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
	QStringList srcMaskList = profileItem_.srcMask_.split(" ");
	QStringList headerMaskList = profileItem_.headerMask_.split(" "); 

	nameFilters = srcMaskList + headerMaskList;

	if (bRebuildTag_) {
		T_OutputItemList resultFileList;

		CSourceFileList::generateFileList(fileListFilename, profileItem_.srcDir_, nameFilters, resultFileList);

		finishOneStep(); // for updating progress bar 
		
		// create tag
		QTagger tagger;

		tagger.createTag(resultFileList);

		tagger.writeTagDb(tagDir + "/" + QTagger::kQTAG_DEFAULT_TAGDBNAME);

		finishOneStep(); // for updating progress bar

		if (bCancelUpdate_) {
			emit cancelledTagBuild();
			return;
		}

		finishOneStep(); // for updating progress bar
	} else { // update tag

		T_OutputItemList newFileList;
		T_OutputItemList existingFileList;

		QMap<long, long> fileIdDeletedMap;
		QMap<long, long> fileIdModifiedMap; 
		QMap<long, long> fileIdCreatedMap;

		int bListFileOpenResult;
		int bListFileSaveResult;

		// key: filename
		QMap<QString, COutputItem> existingFileInfoMap;
		QMap<QString, COutputItem> newFileInfoMap; 

		// key: file id
		QMap<long, COutputItem> fileMapIdxByFileId;

		// key: file id
		QMap<long, long> assignedFileId;

		COutputItem updatedOutputItem; 

		// existing file list i.e. before update tag
		bListFileOpenResult = CSourceFileList::loadFileList(fileListFilename, existingFileList);

		// put the existing file list into a map
		foreach (const COutputItem& outputItem, existingFileList) {
			existingFileInfoMap[outputItem.fileName_] = outputItem;

			assignedFileId[outputItem.fileId_] = outputItem.fileId_; // file id assigned
		}

		finishOneStep(); // for updating progress bar  

		// new file list, for this update tag, not save the file list to file yet as need to update file id
		CSourceFileList::generateFileList(fileListFilename, profileItem_.srcDir_, nameFilters, newFileList, false); 

		long newFileId = 0;

		// put the new file list into a map 
		foreach (const COutputItem& outputItem, newFileList) { 
			updatedOutputItem = outputItem;

			// use previous file id if file exist before
			QMap<QString, COutputItem>::iterator it = existingFileInfoMap.find(outputItem.fileName_); 

			if (it == existingFileInfoMap.end()) { // not exist before
				
				if (newFileId == 0) { // assign to largest key +1 if not exist yet
					QMapIterator<long, long> itMap(assignedFileId);
					itMap.toBack();
					itMap.previous();
					newFileId = itMap.key() + 1;
				}

				updatedOutputItem.fileId_ = newFileId;
				newFileId++; // the next file id will be +1 

			} else {  // exist before
				updatedOutputItem.fileId_ = it.value().fileId_; // reuse previous file id for assignment
			}
			
			newFileInfoMap[updatedOutputItem.fileName_] = updatedOutputItem;
			fileMapIdxByFileId[updatedOutputItem.fileId_] = updatedOutputItem;  

			qDebug() << "filename in newlist" << updatedOutputItem.fileName_;
			qDebug() << "fileid in newlist" << updatedOutputItem.fileId_;
		}

		bListFileSaveResult = CSourceFileList::saveFileList(fileListFilename, fileMapIdxByFileId);

		finishOneStep(); // for updating progress bar

		// deleted file, modified file

		foreach (const COutputItem& outputItem, existingFileList) {  

			QMap<QString, COutputItem>::iterator it = newFileInfoMap.find(outputItem.fileName_);
			
			if (it == newFileInfoMap.end()) { // deleted file: in existingFileList, but not in newModifyTimeMap
				qDebug() << "Deleted file: " << outputItem.fileName_;
                fileIdDeletedMap[outputItem.fileId_] = outputItem.fileId_;
			} else {
				if (it.value().fileLastModified_ != outputItem.fileLastModified_) { // modified file: check by file modification date time
					qDebug() << "Modified file: " << outputItem.fileName_;
					fileIdModifiedMap[outputItem.fileId_] = outputItem.fileId_; 
				} else {
                    qDebug() << "Same file: " << outputItem.fileName_; 
				}

				newFileInfoMap.erase(it); // remove checked item so remaining would be new files
			}
		}

		// new file: the remaining files
		QMap<QString, COutputItem>::const_iterator mapIt = newFileInfoMap.constBegin();
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
			bRunResult = runCommand(cmdStr, profileItem_.srcDir_);
			if (!bRunResult) { // also break if errors during run
				break;
			}
		}
	}

    // for ctag
	if (bRunResult) { // only continue if runCommand without problem
		srcTagName = profileItem_.srcDir_ + "/" + tagName;
		targetTagName = tagDir + "/" + tagName;
		QFile::rename(srcTagName, targetTagName);
		//QFile::remove(srcTagName);
		finishAllStep(); // finish all step as it's the last one
	}


}





