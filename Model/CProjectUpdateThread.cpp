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

	emit percentageCompleted(0, ""); // emit 0 initially
}

void CProjectUpdateThread::finishOneStep(const QString& indexingFileName)
{
	int endPercent;
	stepCompleted_++;
	endPercent = stepCompleted_ * (100 / totalStep_);
	emit percentageCompleted(endPercent, indexingFileName);

	if (stepCompleted_ >= totalStep_) {
		emit percentageCompleted(100, ""); // always 100% if finish all steps
	}
}

void CProjectUpdateThread::finishAllStep()
{
	emit percentageCompleted(100, "");
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

	finishOneStep(""); // for updating progress bar
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
	qsizetype i = 0;
	unsigned long fileId = 0;
	QString currentFilePath;

	qsizetype totalFile = inputFileList.size();

	tagger.initKeywordFileTokenMap();

	unsigned long long int totalFileSize = 0;
	unsigned long long int processedFileSize = 0;

	for (i = 0; i < totalFile; i++) {
		totalFileSize += inputFileList.at(i).fileSize_;
	}

	for (i = 0; i < totalFile; i++) {
		qDebug() << "inputFileList.at(i).fileName_ = " << inputFileList.at(i).fileName_ << Qt::endl;

		currentFilePath = inputFileList.at(i).fileName_; // index start from 0
		fileId = inputFileList.at(i).fileId_;

		qDebug() << fileId << ":" << currentFilePath;

		emit percentageCompleted(static_cast<int> (static_cast<long double> (processedFileSize) / totalFileSize * 100), currentFilePath);

		qDebug() << "parseSourceFile IN" << Qt::endl;
		tagger.parseSourceFile(fileId, currentFilePath);
		qDebug() << "parseSourceFile OUT" << Qt::endl;

		processedFileSize += inputFileList.at(i).fileSize_;

		if (bCancelUpdate_) {
			emit cancelledTagBuild();
			return 0;
		}
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
	QString tagDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+ "/tags/" + projectItem_.name_;
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

	// create tag
	QTagger tagger;

	T_FileItemList resultFileList;

	QElapsedTimer timer;
	timer.start();

	CSourceFileList::generateFileList(fileListFilename, projectItem_.srcDir_, nameFilters, resultFileList);

	createTag(tagger, resultFileList);

	tagger.writeTagDb(tagDir + "/" + QTagger::kQTAG_DEFAULT_TAGDBNAME);

	qDebug() << "Tag creation took" << timer.elapsed() << "milliseconds";

	finishOneStep(""); // for updating progress bar

	if (bCancelUpdate_) {
		emit cancelledTagBuild();
		return;
	}

	finishOneStep(""); // for updating progress bar

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

	return;
}





