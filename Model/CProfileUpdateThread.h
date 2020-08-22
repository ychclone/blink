#ifndef CPROFILE_UPDATE_THREAD_H
#define CPROFILE_UPDATE_THREAD_H

#include <QThread>
#include <QDir>

#include "Utils/commonType.h"
#include "Utils/CUtils.h"
#include "Model/qTagger/qTagger.h"

#include "Model/CProfileItem.h"
#include "Model/CRunCommand.h"
#include "Model/CConfigManager.h"   
#include "Model/qTagger/CSourceFileList.h" 

class CProfileUpdateThread: public QThread
{
    Q_OBJECT

public:
    CProfileUpdateThread(QObject *parent = 0);
    
    void setCurrentProfileItem(const CProfileItem& profileItem);
    void setRebuildTag(bool bRebuildTag);

	void run();
	void cancelUpdate();
    
signals:
    void percentageCompleted(int percentage);
	void cancelledTagBuild();
	void errorDuringRun(const QString& errStr);

private:	

    bool runCommand(const QString& program, const QString& workDir, const QString& redirectFile = "");

	void initStep(int totalStep);
	void finishOneStep();
	void finishAllStep();

	int countTotalRunCmd();
	
    CProfileItem profileItem_;

	CRunCommand cmd_;
    
    int stepCompleted_;
    int totalStep_;

	bool bCancelUpdate_;
	bool bRebuildTag_;

	static const int MAX_SUPPORTED_RUN_COMMAND = 999;
    
};

#endif // CPROFILE_UPDATE_THREAD_H
