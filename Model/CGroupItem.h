#ifndef CGROUP_ITEM_H
#define CGROUP_ITEM_H

#include <QtXml>
#include <QTextStream>
#include <QString>

class CGroupItem
{
public:

    CGroupItem();
    CGroupItem(const QString& name, const QString& projectList, const QString& tagUpdateDateTime, 
				const QString& groupCreateDateTime, const QString& labels);

    QString name_,
			projectList_,
			
			tagUpdateDateTime_,
			groupCreateDateTime_,
			labels_;

    virtual ~CGroupItem() {};
};

#endif // CGROUP_ITEM_H




