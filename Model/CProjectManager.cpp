#include "CProjectManager.h"
#include "CConfigManager.h"

#include "Utils/CUtils.h"

CProjectManager* CProjectManager::manager_ = 0;

CProjectManager::CProjectManager()
{

}

CProjectManager* CProjectManager::getInstance()
{
    if (manager_ == 0) {
        manager_ = new CProjectManager();
    }
    return manager_;
}

void CProjectManager::setProjectFile(const QString& projectFileName)
{
    projectFile_ = projectFileName;
}

void CProjectManager::setStorageHandler(const CXmlStorageHandler& handler)
{
    handler_ = handler;
}

void CProjectManager::attachStorage()
{
    handler_.loadFromFile(projectFile_, projectMap_);
}

void CProjectManager::flushStorage()
{
    handler_.saveToFile(projectFile_, projectMap_);
}

void CProjectManager::detachStorage()
{
    handler_.saveToFile(projectFile_, projectMap_);
}

void CProjectManager::getProjectMap(QMap<QString, CProjectItem>& projectMap)
{
	projectMap = projectMap_;
}

CProjectItem CProjectManager::getProjectItem(const QString& projectItemName) const
{
//    qDebug() << "getItem" << projectItemName << "called!\n";
//    qDebug() << "projectMap_[projectItemName].name_ = " << projectMap_[projectItemName].name_ << Qt::endl;
//    qDebug() << "projectMap_[projectItemName].srcDir_ = " << projectMap_[projectItemName].srcDir_ << Qt::endl;
    return projectMap_[projectItemName];
}

void CProjectManager::addItem(const CProjectItem& newItem)
{
    projectMap_[newItem.name_] = newItem;
    emit projectMapUpdated();
    flushStorage();
}

void CProjectManager::updateProjectItem(bool newProject, const QString& projectItemName, const CProjectItem& newItem)
{
	QString tagDir;

	// current directory
	QDir currentDir(QDir::currentPath());

	if (projectItemName != newItem.name_) { // project renamed
		qDebug() << "newItem.name_ = " << newItem.name_;
		qDebug() << "projectItemName = " << projectItemName;

        projectMap_.remove(projectItemName); // remove old one

		// using absoluteFilePath so relative and absolute path also possible
		tagDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + projectItemName;
		QDir dir(tagDir);

		// remove tag directory
		CUtils::removeDirectory(dir);
	}

    projectMap_[newItem.name_] = newItem;
    emit projectMapUpdated();
    flushStorage();

	if (newProject) {
		emit newProjectAdded(projectItemName);
	}
}

void CProjectManager::removeProjectItem(const QString& projectItemName)
{
	QString tagDir;

	// current directory
	QDir currentDir(QDir::currentPath());

	// using absoluteFilePath so relative and absolute path also possible
	tagDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tags/" + projectItemName;
	QDir dir(tagDir);

	// remove tag directory
	CUtils::removeDirectory(dir);

    // remove from map
    projectMap_.remove(projectItemName);
    emit projectMapUpdated();
    flushStorage();

}

void CProjectManager::destroy()
{
    delete manager_;
}



