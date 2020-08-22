#ifndef CCONFIG_MANAGER_H
#define CCONFIG_MANAGER_H

#include <QObject>
#include <QSettings>
#include <QVariant>

class CConfigManager: public QObject
{
    Q_OBJECT
public:
	CConfigManager();

    virtual ~CConfigManager() {};

    static CConfigManager* getInstance();

	QVariant getAppSettingValue(const QString& key) const;
	QVariant getAppSettingValue(const QString& key, const QVariant &defaultValue) const;

	void setAppSettingValue(const QString& key, const QVariant& val);

	QVariant getValue(const QString& section, const QString& key) const;
	QVariant getValue(const QString& section, const QString& key, const QVariant &defaultValue) const;

	void setValue(const QString& section, const QString& key, const QVariant& val);

	void updateConfig();

private:
	static CConfigManager* manager_;
	QSettings* confSetting;
};


#endif // CCONFIG_MANAGER_H
