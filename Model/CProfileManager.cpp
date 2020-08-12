#include "CProfileManager.h"
#include "CConfigManager.h"

#include "Utils/CUtils.h" 

CProfileManager* CProfileManager::m_manager = 0;

const char* CProfileManager::kGROUP_PROFILE_SEPERATOR = ","; 

CProfileManager::CProfileManager()
{

}

CProfileManager* CProfileManager::getInstance()
{
    if (m_manager == 0) {
        m_manager = new CProfileManager();
    }
    return m_manager;
}

void CProfileManager::setProfileFile(const QString& profileFileName)  
{
    m_profileFile = profileFileName;
}

void CProfileManager::setStorageHandler(const CXmlStorageHandler& handler)
{
    m_handler = handler;
}

void CProfileManager::attachStorage()
{
    m_handler.loadFromFile(m_profileFile, m_profileMap, m_groupMap);
}

void CProfileManager::flushStorage()
{
    m_handler.saveToFile(m_profileFile, m_profileMap, m_groupMap);
}

void CProfileManager::detachStorage()
{
    m_handler.saveToFile(m_profileFile, m_profileMap, m_groupMap);
}

void CProfileManager::getProfileMap(QMap<QString, CProfileItem>& profileMap)
{
	profileMap = m_profileMap;
}

void CProfileManager::getGroupMap(QMap<QString, CGroupItem>& groupMap)
{
	groupMap = m_groupMap;
}

CProfileItem CProfileManager::getProfileItem(const QString& profileItemName) const
{
//    qDebug() << "getItem" << profileItemName << "called!\n";
//    qDebug() << "m_profileMap[profileItemName].m_name = " << m_profileMap[profileItemName].m_name << endl;
//    qDebug() << "m_profileMap[profileItemName].m_srcDir = " << m_profileMap[profileItemName].m_srcDir << endl;
    return m_profileMap[profileItemName];
}

CGroupItem CProfileManager::getGroupItem(const QString& groupItemName) const
{
    return m_groupMap[groupItemName];
}


void CProfileManager::addItem(const CProfileItem& newItem)
{
    m_profileMap[newItem.m_name] = newItem;
    emit profileMapUpdated();
    flushStorage();
}

void CProfileManager::updateProfileItem(const QString& profileItemName, const CProfileItem& newItem)
{
	QString tagDir; 

	// current directory
	QDir currentDir(QDir::currentPath());

	if (profileItemName != newItem.m_name) { // profile renamed
        m_profileMap.remove(profileItemName); // remove old one

		// current directory
		QDir currentDir(QDir::currentPath()); 

		// using absoluteFilePath so relative and absolute path also possible   
		tagDir = currentDir.absoluteFilePath(CConfigManager::getInstance()->getAppSettingValue("TagDir").toString() + "/" + profileItemName);
		QDir dir(tagDir);

		// remove tag directory
		CUtils::removeDirectory(dir);
	}

    m_profileMap[newItem.m_name] = newItem;
    emit profileMapUpdated();
    flushStorage();
}

void CProfileManager::updateGroupItem(const QString& groupItemName, const CGroupItem& newItem)
{
	if (groupItemName != newItem.m_name) { // group renamed
		m_groupMap.remove(groupItemName); // remove old one
	}

    m_groupMap[newItem.m_name] = newItem;
    emit groupMapUpdated();
    flushStorage();
}


void CProfileManager::removeProfileItem(const QString& profileItemName)
{
	QString tagDir; 

	// current directory
	QDir currentDir(QDir::currentPath()); 

	// using absoluteFilePath so relative and absolute path also possible   
	tagDir = currentDir.absoluteFilePath(CConfigManager::getInstance()->getAppSettingValue("TagDir").toString() + "/" + profileItemName);
	QDir dir(tagDir);

	// remove tag directory
	CUtils::removeDirectory(dir);

    // remove from map
    m_profileMap.remove(profileItemName);
    emit profileMapUpdated();
    flushStorage();

}

void CProfileManager::removeGroupItem(const QString& groupItemName)
{
    m_groupMap.remove(groupItemName);
    emit groupMapUpdated();
    flushStorage();
}

void CProfileManager::destroy()
{
    delete m_manager;
}



