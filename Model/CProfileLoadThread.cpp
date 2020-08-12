#include "CProfileLoadThread.h"

CProfileLoadThread::CProfileLoadThread(QObject *parent)
    : QThread(parent)
{
	m_taggerPtr = NULL;
	m_outputItemListPtr = NULL;
}

void CProfileLoadThread::setTaggerPtr(QTagger* taggerPtr)
{
	m_taggerPtr = taggerPtr;
}

void CProfileLoadThread::setOutputItemListPtr(T_OutputItemList* outputItemListPtr)
{
	m_outputItemListPtr = outputItemListPtr;
}

void CProfileLoadThread::setCurrentProfileItem(const CProfileItem& profileItem)
{
    m_profileItem = profileItem;
}

CProfileItem CProfileLoadThread::getCurrentProfileItem()
{
    return m_profileItem;
}


bool CProfileLoadThread::runCommand(const QString& program, const QString& workDir, const QString& redirectFile) 
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

void CProfileLoadThread::run()
{
	CConfigManager* confManager;
	confManager	= CConfigManager::getInstance(); 

	QDir currentDir(QDir::currentPath());

	// using absolutePath so relative and absolute path also possible  
	QString tagDir = currentDir.absoluteFilePath(confManager->getAppSettingValue("TagDir").toString() + "/" + m_profileItem.m_name);
	
	QString tagSubstDrv = confManager->getAppSettingValue("TagSubstDrv").toString();
	QString srcSubstDrv = confManager->getAppSettingValue("SrcSubstDrv").toString(); 

    QString tagParam = tagSubstDrv + " " + tagDir;
    QString srcParam = srcSubstDrv + " " + m_profileItem.m_srcDir;

    runCommand("subst " + tagSubstDrv + " /d", m_profileItem.m_srcDir);
    runCommand("subst " + tagParam, m_profileItem.m_srcDir);
    runCommand("subst " + srcSubstDrv + " /d", m_profileItem.m_srcDir);
    runCommand("subst " + srcParam, m_profileItem.m_srcDir);

	// write output to qtag config file
	QFile qTagConfigFile(QTagger::kQTAG_CONFIG_FILE);

	qTagConfigFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream qTagConfigFileOut(&qTagConfigFile);
	qTagConfigFileOut << "profileLoad=" << m_profileItem.m_name << endl;
 
	qTagConfigFile.close(); 

	QString tagDbFileName = QString(QTagger::kQTAG_TAGS_DIR) + "/" + m_profileItem.m_name + "/" + QString(QTagger::kQTAG_DEFAULT_TAGDBNAME);
	
	if (m_taggerPtr != NULL) {
		m_taggerPtr->loadTagList(tagDbFileName);
	}

	QString outputFile;  
	int bListFileOpenResult = -1; 

	outputFile = tagDir + "/" + QTagger::kQTAG_DEFAULT_INPUTLIST_FILE;

	if (m_outputItemListPtr != NULL) {
		m_outputItemListPtr->clear();
		bListFileOpenResult = CSourceFileList::loadFileList(outputFile, *m_outputItemListPtr);
	}	

	qDebug() << "outputFile = " << outputFile << endl;

	if (bListFileOpenResult == 0) {
		emit profileLoadPercentageCompleted(100);
	} else {
		emit profileLoadPercentageCompleted(0);
	}
}




