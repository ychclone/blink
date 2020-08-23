#ifndef CPROJECT_LOAD_THREAD_H
#define CPROJECT_LOAD_THREAD_H

#include <QThread>
#include <QProcess>

#include "Model/qTagger/qTagger.h"

#include "Model/CProjectItem.h"
#include "Model/CRunCommand.h"
#include "Model/CConfigManager.h"  

class CProjectLoadThread: public QThread
{
	Q_OBJECT

public:
    CProjectLoadThread(QObject *parent = 0);
    
    void setCurrentProjectItem(const CProjectItem& projectItem);

	void setFileItemListPtr(T_FileItemList* fileItemListPtr);
	void setTaggerPtr(QTagger* taggerPtrPtr);

	CProjectItem getCurrentProjectItem(); 
    
    void run();

signals:
	void projectLoadPercentageCompleted(int percentage); 

private:
    bool runCommand(const QString& program, const QString& workDir, const QString& redirectFile = "");
    CProjectItem projectItem_;

	CRunCommand cmd_;
	QTagger* taggerPtr_;

	T_FileItemList* fileItemListPtr_;
    
};

#endif // CPROJECT_LOAD_THREAD_H
