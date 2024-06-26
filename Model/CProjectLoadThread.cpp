#include "CProjectLoadThread.h"

CProjectLoadThread::CProjectLoadThread(QObject *parent)
    : QThread(parent)
{
	taggerPtr_ = NULL;
	fileItemListPtr_ = NULL;
}

void CProjectLoadThread::setTaggerPtr(QTagger* taggerPtr)
{
	taggerPtr_ = taggerPtr;
}

void CProjectLoadThread::setFileItemListPtr(T_FileItemList* fileItemListPtr)
{
	fileItemListPtr_ = fileItemListPtr;
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
	QDir currentDir(QDir::currentPath());

	// using absolutePath so relative and absolute path also possible
	QString tagDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + projectItem_.name_;

	// write output to qtag config file
	QFile qTagConfigFile(QTagger::kQTAG_CONFIG_FILE);

	qTagConfigFile.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream qTagConfigFileOut(&qTagConfigFile);
	qTagConfigFileOut << "projectLoad=" << projectItem_.name_ << Qt::endl;

	qTagConfigFile.close();

	QString tagDbFileName = tagDir  + "/" + QString(QTagger::kQTAG_DEFAULT_TAGDBNAME);

	if (taggerPtr_ != NULL) {
		qDebug() << "loadTagList() for " << tagDbFileName << " IN";
		taggerPtr_->loadTagList(tagDbFileName);
		qDebug() << "loadTagList() for " << tagDbFileName << " OUT";
	}

	QString outputFile;
	int bListFileOpenResult = -1;

	outputFile = tagDir + "/" + QTagger::kQTAG_DEFAULT_INPUTLIST_FILE;

	if (fileItemListPtr_ != NULL) {
		fileItemListPtr_->clear();
		qDebug() << "loadFileList() for " << outputFile << " IN";
		bListFileOpenResult = CSourceFileList::loadFileList(outputFile, *fileItemListPtr_);
		qDebug() << "loadFileList() for " << outputFile << " OUT";
	}

	qDebug() << "outputFile = " << outputFile << Qt::endl;

	if (bListFileOpenResult == 0) {
		emit projectLoadPercentageCompleted(100);
	} else {
		emit projectLoadPercentageCompleted(0);
	}
}




