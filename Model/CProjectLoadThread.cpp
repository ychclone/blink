#include "CProjectLoadThread.h"

CProjectLoadThread::CProjectLoadThread(QObject *parent)
    : QThread(parent)
{
	taggerPtr_ = NULL;
	outputItemListPtr_ = NULL;
}

void CProjectLoadThread::setTaggerPtr(QTagger* taggerPtr)
{
	taggerPtr_ = taggerPtr;
}

void CProjectLoadThread::setOutputItemListPtr(T_OutputItemList* outputItemListPtr)
{
	outputItemListPtr_ = outputItemListPtr;
}

void CProjectLoadThread::setCurrentProjectItem(const CProjectItem& projectItem)
{
    projectItem_ = projectItem;
}

CProjectItem CProjectLoadThread::getCurrentProjectItem()
{
    return projectItem_;
}


bool CProjectLoadThread::runCommand(const QString& program, const QString& workDir, const QString& redirectFile)
{
	QString errStr;
	CRunCommand::ENUM_RunCommandErr cmdErr;

	cmdErr = (CRunCommand::ENUM_RunCommandErr) cmd_.startRun(program, workDir, redirectFile, errStr);

	switch (cmdErr) {
		case CRunCommand::E_RUNCMD_NO_ERROR:
			break;
		case CRunCommand::E_RUNCMD_CANCELLED:
			return false;
			break;
		case CRunCommand::E_RUNCMD_FAILSTART:
			return false;
			break;
		case CRunCommand::E_RUNCMD_ERRRUN:
			return false;
			break;
		case CRunCommand::E_RUNCMD_CRASHED:
			return false;
			break;
		default:
			break;
	}

	return true;
}

void CProjectLoadThread::run()
{
	CConfigManager* confManager;
	confManager	= CConfigManager::getInstance();

	QDir currentDir(QDir::currentPath());

	// using absolutePath so relative and absolute path also possible
	QString tagDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TagDir").toString() + "/" + projectItem_.name_);

	// write output to qtag config file
	QFile qTagConfigFile(QTagger::kQTAG_CONFIG_FILE);

	qTagConfigFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream qTagConfigFileOut(&qTagConfigFile);
	qTagConfigFileOut << "projectLoad=" << projectItem_.name_ << endl;

	qTagConfigFile.close();

	QString tagDbFileName = QString(QTagger::kQTAG_TAGS_DIR) + "/" + projectItem_.name_ + "/" + QString(QTagger::kQTAG_DEFAULT_TAGDBNAME);

	if (taggerPtr_ != NULL) {
		taggerPtr_->loadTagList(tagDbFileName);
	}

	QString outputFile;
	int bListFileOpenResult = -1;

	outputFile = tagDir + "/" + QTagger::kQTAG_DEFAULT_INPUTLIST_FILE;

	if (outputItemListPtr_ != NULL) {
		outputItemListPtr_->clear();
		bListFileOpenResult = CSourceFileList::loadFileList(outputFile, *outputItemListPtr_);
	}

	qDebug() << "outputFile = " << outputFile << endl;

	if (bListFileOpenResult == 0) {
		emit projectLoadPercentageCompleted(100);
	} else {
		emit projectLoadPercentageCompleted(0);
	}
}




