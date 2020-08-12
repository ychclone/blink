#ifndef CPROFILE_MANAGER_H
#define CPROFILE_MANAGER_H

#include "Model/CProfileItem.h"
#include "Model/CGroupItem.h"
#include "Storage/IStorageHandler.h"
#include "Storage/CXmlStorageHandler.h"

class CProfileManager: public QObject
{
    Q_OBJECT
    
public:
    virtual ~CProfileManager() {};

    static CProfileManager* getInstance();
    
    void setProfileFile(const QString& profileFileName); 
    void setStorageHandler(const CXmlStorageHandler& handler);
    void attachStorage();
    void flushStorage();
    void detachStorage();
    
	void getProfileMap(QMap<QString, CProfileItem>& profileMap);
	void getGroupMap(QMap<QString, CGroupItem>& groupMap);
    
    void addItem(const CProfileItem& newItem);

    CProfileItem getProfileItem(const QString& profileItemName) const;
	CGroupItem getGroupItem(const QString& groupItemName) const; 

    void updateProfileItem(const QString& profileItemName, const CProfileItem& newItem);
	void updateGroupItem(const QString& groupItemName, const CGroupItem& newItem); 

    void removeProfileItem(const QString& profileItemName);
	void removeGroupItem(const QString& groupItemName); 

    void destroy();

	static const char* kGROUP_PROFILE_SEPERATOR; 

protected:
    CProfileManager();

signals:
    void profileMapUpdated();
	void groupMapUpdated(); 
    
private:
    CXmlStorageHandler m_handler;

    QMap<QString, CProfileItem> m_profileMap;
	QMap<QString, CGroupItem> m_groupMap; 

    QString m_profileFile;

    static CProfileManager* m_manager;

};

#endif // CPROFILE_MANAGER_H


