#ifndef CGROUP_LOAD_THREAD_H
#define CGROUP_LOAD_THREAD_H

#include <QThread>
#include <QProcess>

#include "Model/qTagger/qTagger.h"
#include "Model/CProjectManager.h"

#include "Model/CGroupItem.h"
#include "Model/CRunCommand.h"
#include "Model/CConfigManager.h"  

class CGroupLoadThread: public QThread
{
	Q_OBJECT

public:
    CGroupLoadThread(QObject *parent = 0);
    
    void setCurrentGroupItem(const CGroupItem& groupItem);

	void setFileItemListPtr(T_FileItemList* fileItemListPtr);
	void setTaggerPtr(QTagger* taggerPtrPtr);

	CGroupItem getCurrentGroupItem(); 
    
    void run();

signals:
	void groupLoadPercentageCompleted(int percentage); 

private:
    bool runCommand(const QString& program, const QString& workDir, const QString& redirectFile = "");
    CGroupItem groupItem_;

	CRunCommand cmd_;
	QTagger* taggerPtr_;

	T_FileItemList* fileItemListPtr_;
    
};

#endif // CGROUP_LOAD_THREAD_H


