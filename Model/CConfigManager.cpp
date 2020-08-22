#include "CConfigManager.h"

CConfigManager* CConfigManager::manager_ = 0;

CConfigManager* CConfigManager::getInstance()
{
    if (manager_ == 0) {
        manager_ = new CConfigManager();
    }
    return manager_;
}

CConfigManager::CConfigManager()
{
	confSetting = new QSettings("blink.ini", QSettings::IniFormat);
}

QVariant CConfigManager::getAppSettingValue(const QString& key) const
{
	if (key == "TagDir") { // default directory for storing tags
		return confSetting->value("Setting/" + key, "tags");
	} else{
		return confSetting->value("Setting/" + key);
	}
}

QVariant CConfigManager::getAppSettingValue(const QString& key, const QVariant &defaultValue) const
{
	if (key == "TagDir") { // default directory for storing tags
        return confSetting->value("Setting/" + key, "tags");
	} else {
		return confSetting->value("Setting/" + key, defaultValue);
	}
}

void CConfigManager::setAppSettingValue(const QString& key, const QVariant& val)
{
	confSetting->setValue("Setting/" + key, val);
}

QVariant CConfigManager::getValue(const QString& section, const QString& key) const
{
	return confSetting->value(section + "/" + key);
}

QVariant CConfigManager::getValue(const QString& section, const QString& key, const QVariant &defaultValue) const
{
	return confSetting->value(section + "/" + key, defaultValue);
}

void CConfigManager::setValue(const QString& section, const QString& key, const QVariant& val)
{
	confSetting->setValue(section + "/" + key, val);
}

void CConfigManager::updateConfig()
{
	confSetting->sync();
}







