#ifndef CGROUP_LOAD_THREAD_H
#define CGROUP_LOAD_THREAD_H

#include <QThread>
#include <QProcess>

#include "Model/qTagger/qTagger.h"
#include "Model/CProfileManager.h"

#include "Model/CGroupItem.h"
#include "Model/CRunCommand.h"
#include "Model/CConfigManager.h"  

class CGroupLoadThread: public QThread
{
	Q_OBJECT

public:
    CGroupLoadThread(QObject *parent = 0);
    
    void setCurrentGroupItem(const CGroupItem& groupItem);

	void setOutputItemListPtr(T_OutputItemList* outputItemListPtr);
	void setTaggerPtr(QTagger* taggerPtrPtr);

	CGroupItem getCurrentGroupItem(); 
    
    void run();

signals:
	void groupLoadPercentageCompleted(int percentage); 

private:
    bool runCommand(const QString& program, const QString& workDir, const QString& redirectFile = "");
    CGroupItem m_groupItem;

	CRunCommand m_cmd;
	QTagger* m_taggerPtr;

	T_OutputItemList* m_outputItemListPtr;
    
};

#endif // CGROUP_LOAD_THREAD_H


