#ifndef CPROJECT_UPDATE_THREAD_H
#define CPROJECT_UPDATE_THREAD_H

#include <QThread>
#include <QDir>

#include "Utils/commonType.h"
#include "Utils/CUtils.h"
#include "Model/qTagger/qTagger.h"

#include "Model/CProjectItem.h"
#include "Model/CRunCommand.h"
#include "Model/CConfigManager.h"
#include "Model/qTagger/CSourceFileList.h"

class CProjectUpdateThread: public QThread
{
    Q_OBJECT

public:
    CProjectUpdateThread(QObject *parent = 0);

    void setCurrentProjectItem(const CProjectItem& projectItem);
    void setRebuildTag(bool bRebuildTag);

	void run();
	void cancelUpdate();

signals:
    void percentageCompleted(int percentage, QString indexingFileName);
	void cancelledTagBuild();
	void errorDuringRun(const QString& errStr);

private:

    bool runCommand(const QString& program, const QString& workDir, const QString& redirectFile = "");

	void initStep(int totalStep);
	void finishOneStep(const QString& indexingFileName);
	void finishAllStep();

	int countTotalRunCmd();

	int createTag(QTagger& tagger, const T_FileItemList& inputFileList);

    CProjectItem projectItem_;

	CRunCommand cmd_;

    int stepCompleted_;
    int totalStep_;

	bool bCancelUpdate_;
	bool bRebuildTag_;

	static const int MAX_SUPPORTED_RUN_COMMAND = 999;

};

#endif // CPROJECT_UPDATE_THREAD_H
