#ifndef CPROJECT_MANAGER_H
#define CPROJECT_MANAGER_H

#include "Model/CProjectItem.h"
#include "Model/CGroupItem.h"
#include "Storage/IStorageHandler.h"
#include "Storage/CXmlStorageHandler.h"

class CProjectManager: public QObject
{
    Q_OBJECT

public:
    virtual ~CProjectManager() {};

    static CProjectManager* getInstance();

    void setProjectFile(const QString& projectFileName);
    void setStorageHandler(const CXmlStorageHandler& handler);
    void attachStorage();
    void flushStorage();
    void detachStorage();

	void getProjectMap(QMap<QString, CProjectItem>& projectMap);
	void getGroupMap(QMap<QString, CGroupItem>& groupMap);

    void addItem(const CProjectItem& newItem);

    CProjectItem getProjectItem(const QString& projectItemName) const;
	CGroupItem getGroupItem(const QString& groupItemName) const;

    void updateProjectItem(bool newProject, const QString& projectItemName, const CProjectItem& newItem);
	void updateGroupItem(const QString& groupItemName, const CGroupItem& newItem);

    void removeProjectItem(const QString& projectItemName);
	void removeGroupItem(const QString& groupItemName);

    void destroy();

	static const char* kGROUP_PROFILE_SEPERATOR;

protected:
    CProjectManager();

signals:
    void projectMapUpdated();
	void groupMapUpdated();
    void newProjectAdded(QString projectItemName);

private:
    CXmlStorageHandler handler_;

    QMap<QString, CProjectItem> projectMap_;
	QMap<QString, CGroupItem> groupMap_;

    QString projectFile_;

    static CProjectManager* manager_;

};

#endif // CPROJECT_MANAGER_H


