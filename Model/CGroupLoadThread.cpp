#include "CGroupLoadThread.h"

CGroupLoadThread::CGroupLoadThread(QObject *parent)
    : QThread(parent)
{
	m_taggerPtr = NULL;
	m_outputItemListPtr = NULL;
}

void CGroupLoadThread::setTaggerPtr(QTagger* taggerPtr)
{
	m_taggerPtr = taggerPtr;
}

void CGroupLoadThread::setOutputItemListPtr(T_OutputItemList* outputItemListPtr)
{
	m_outputItemListPtr = outputItemListPtr;
}

void CGroupLoadThread::setCurrentGroupItem(const CGroupItem& groupItem)
{
    m_groupItem = groupItem;
}

CGroupItem CGroupLoadThread::getCurrentGroupItem()
{
    return m_groupItem;
}


bool CGroupLoadThread::runCommand(const QString& program, const QString& workDir, const QString& redirectFile) 
{
	QString errStr;
	CRunCommand::ENUM_RunCommandErr cmdErr;

	cmdErr = (CRunCommand::ENUM_RunCommandErr) m_cmd.startRun(program, workDir, redirectFile, errStr);

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
	qTagConfigFileOut << "groupLoad=" << m_groupItem.m_name << endl;
 
	qTagConfigFile.close(); 

	QString tagDbFileName = QString(QTagger::kQTAG_TAGS_DIR) + "/" + m_groupItem.m_name + "/" + QString(QTagger::kQTAG_DEFAULT_TAGDBNAME);
	
	if (m_taggerPtr != NULL) {
		m_taggerPtr->loadTagList(tagDbFileName);
	}
*/	

	if (m_outputItemListPtr != NULL) {
		m_outputItemListPtr->clear();
	}

	QString tagRootDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TagDir").toString()); 
	QStringList groupProfileList; 

	groupProfileList = m_groupItem.m_profileList.split(CProfileManager::kGROUP_PROFILE_SEPERATOR, QString::SkipEmptyParts);

	foreach (const QString& profileName, groupProfileList) { 
		QString outputFile;  
		int bListFileOpenResult; 

		outputFile = tagRootDir + "/" + profileName + "/" + QTagger::kQTAG_DEFAULT_INPUTLIST_FILE;

		if (m_outputItemListPtr != NULL) {
			bListFileOpenResult = CSourceFileList::loadFileList(outputFile, *m_outputItemListPtr);
		}
	} 

	emit groupLoadPercentageCompleted(100);
}






