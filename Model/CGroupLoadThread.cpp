#include "CGroupLoadThread.h"

CGroupLoadThread::CGroupLoadThread(QObject *parent)
    : QThread(parent)
{
	taggerPtr_ = NULL;
	outputItemListPtr_ = NULL;
}

void CGroupLoadThread::setTaggerPtr(QTagger* taggerPtr)
{
	taggerPtr_ = taggerPtr;
}

void CGroupLoadThread::setOutputItemListPtr(T_OutputItemList* outputItemListPtr)
{
	outputItemListPtr_ = outputItemListPtr;
}

void CGroupLoadThread::setCurrentGroupItem(const CGroupItem& groupItem)
{
    groupItem_ = groupItem;
}

CGroupItem CGroupLoadThread::getCurrentGroupItem()
{
    return groupItem_;
}


bool CGroupLoadThread::runCommand(const QString& program, const QString& workDir, const QString& redirectFile) 
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

void CGroupLoadThread::run()
{
	CConfigManager* confManager;
	confManager	= CConfigManager::getInstance(); 

	QDir currentDir(QDir::currentPath());

/*
	// write output to qtag config file
	QFile qTagConfigFile(QTagger::kQTAG_CONFIG_FILE);

	qTagConfigFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream qTagConfigFileOut(&qTagConfigFile);
	qTagConfigFileOut << "groupLoad=" << groupItem_.name_ << endl;
 
	qTagConfigFile.close(); 

	QString tagDbFileName = QString(QTagger::kQTAG_TAGS_DIR) + "/" + groupItem_.name_ + "/" + QString(QTagger::kQTAG_DEFAULT_TAGDBNAME);
	
	if (taggerPtr_ != NULL) {
		taggerPtr_->loadTagList(tagDbFileName);
	}
*/	

	if (outputItemListPtr_ != NULL) {
		outputItemListPtr_->clear();
	}

	QString tagRootDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TagDir").toString()); 
	QStringList groupProfileList; 

	groupProfileList = groupItem_.profileList_.split(CProfileManager::kGROUP_PROFILE_SEPERATOR, QString::SkipEmptyParts);

	foreach (const QString& profileName, groupProfileList) { 
		QString outputFile;  
		int bListFileOpenResult; 

		outputFile = tagRootDir + "/" + profileName + "/" + QTagger::kQTAG_DEFAULT_INPUTLIST_FILE;

		if (outputItemListPtr_ != NULL) {
			bListFileOpenResult = CSourceFileList::loadFileList(outputFile, *outputItemListPtr_);
		}
	} 

	emit groupLoadPercentageCompleted(100);
}






