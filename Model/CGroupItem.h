#ifndef CGROUP_ITEM_H
#define CGROUP_ITEM_H

#include <QtXml>
#include <QTextStream>
#include <QString>

class CGroupItem
{
public:

    CGroupItem();
    CGroupItem(const QString& name, const QString& profileList, const QString& tagUpdateDateTime, 
				const QString& groupCreateDateTime, const QString& labels);

    QString m_name,
			m_profileList,
			
			m_tagUpdateDateTime,
			m_groupCreateDateTime,
			m_labels;

    virtual ~CGroupItem() {};
};

#endif // CGROUP_ITEM_H




