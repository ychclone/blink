#include "CProfileManager.h"
#include "CConfigManager.h"

#include "Utils/CUtils.h" 

CProfileManager* CProfileManager::manager_ = 0;

const char* CProfileManager::kGROUP_PROFILE_SEPERATOR = ","; 

CProfileManager::CProfileManager()
{

}

CProfileManager* CProfileManager::getInstance()
{
    if (manager_ == 0) {
        manager_ = new CProfileManager();
    }
    return manager_;
}

void CProfileManager::setProfileFile(const QString& profileFileName)  
{
    profileFile_ = profileFileName;
}

void CProfileManager::setStorageHandler(const CXmlStorageHandler& handler)
{
    handler_ = handler;
}

void CProfileManager::attachStorage()
{
    handler_.loadFromFile(profileFile_, profileMap_, groupMap_);
}

void CProfileManager::flushStorage()
{
    handler_.saveToFile(profileFile_, profileMap_, groupMap_);
}

void CProfileManager::detachStorage()
{
    handler_.saveToFile(profileFile_, profileMap_, groupMap_);
}

void CProfileManager::getProfileMap(QMap<QString, CProfileItem>& profileMap)
{
	profileMap = profileMap_;
}

void CProfileManager::getGroupMap(QMap<QString, CGroupItem>& groupMap)
{
	groupMap = groupMap_;
}

CProfileItem CProfileManager::getProfileItem(const QString& profileItemName) const
{
//    qDebug() << "getItem" << profileItemName << "called!\n";
//    qDebug() << "profileMap_[profileItemName].name_ = " << profileMap_[profileItemName].name_ << endl;
//    qDebug() << "profileMap_[profileItemName].srcDir_ = " << profileMap_[profileItemName].srcDir_ << endl;
    return profileMap_[profileItemName];
}

CGroupItem CProfileManager::getGroupItem(const QString& groupItemName) const
{
    return groupMap_[groupItemName];
}


void CProfileManager::addItem(const CProfileItem& newItem)
{
    profileMap_[newItem.name_] = newItem;
    emit profileMapUpdated();
    flushStorage();
}

void CProfileManager::updateProfileItem(const QString& profileItemName, const CProfileItem& newItem)
{
	QString tagDir; 

	// current directory
	QDir currentDir(QDir::currentPath());

	if (profileItemName != newItem.name_) { // profile renamed
        profileMap_.remove(profileItemName); // remove old one

		// current directory
		QDir currentDir(QDir::currentPath()); 

		// using absoluteFilePath so relative and absolute path also possible   
		tagDir = currentDir.absoluteFilePath(CConfigManager::getInstance()->getAppSettingValue("TagDir").toString() + "/" + profileItemName);
		QDir dir(tagDir);

		// remove tag directory
		CUtils::removeDirectory(dir);
	}

    profileMap_[newItem.name_] = newItem;
    emit profileMapUpdated();
    flushStorage();
}

void CProfileManager::updateGroupItem(const QString& groupItemName, const CGroupItem& newItem)
{
	if (groupItemName != newItem.name_) { // group renamed
		groupMap_.remove(groupItemName); // remove old one
	}

    groupMap_[newItem.name_] = newItem;
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
    profileMap_.remove(profileItemName);
    emit profileMapUpdated();
    flushStorage();

}

void CProfileManager::removeGroupItem(const QString& groupItemName)
{
    groupMap_.remove(groupItemName);
    emit groupMapUpdated();
    flushStorage();
}

void CProfileManager::destroy()
{
    delete manager_;
}



