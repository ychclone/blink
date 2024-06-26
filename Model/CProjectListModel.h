#ifndef CPROJECT_LIST_MODEL_H
#define CPROJECT_LIST_MODEL_H

#include <QStandardItemModel>
#include "Model/CProjectItem.h"
#include "Model/CConfigManager.h"

class CProjectListModel: public QStandardItemModel
{
    Q_OBJECT

public:
    CProjectListModel(QObject *parent = 0);

	void addProjectItem(const CProjectItem& projectItem);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
	QStringList mimeTypes() const;

    Qt::DropActions supportedDropActions() const;

	QWidget* parent_;
	CConfigManager* confManager_;
};

#endif // CPROJECT_LIST_MODEL_H
