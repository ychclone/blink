#ifndef CPROFILE_LIST_MODEL_H
#define CPROFILE_LIST_MODEL_H

#include <QStandardItemModel>
#include "Model/CProfileItem.h"

class CProfileListModel: public QStandardItemModel
{
    Q_OBJECT

public:
    CProfileListModel(QObject *parent = 0);

	void addProfileItem(const CProfileItem& profileItem);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
	QStringList mimeTypes() const; 

    Qt::DropActions supportedDropActions() const;

	QWidget* m_parent;
};

#endif // CPROFILE_LIST_MODEL_H
