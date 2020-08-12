#ifndef CGROUP_LIST_MODEL_H
#define CGROUP_LIST_MODEL_H

#include <QStandardItemModel>
#include "Model/CGroupItem.h"

class CGroupListModel: public QStandardItemModel
{
    Q_OBJECT

public:
    CGroupListModel(QObject *parent = 0);

	void addGroupItem(const CGroupItem& groupItem);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
	QStringList mimeTypes() const; 

    Qt::DropActions supportedDropActions() const;

	QWidget* m_parent;
};

#endif // CGROUP_LIST_MODEL_H
