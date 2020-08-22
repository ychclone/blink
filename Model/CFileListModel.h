#ifndef COUTPUT_LIST_MODEL_H
#define COUTPUT_LIST_MODEL_H

#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QDateTime>

#include <QDebug>

#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include "Model/COutputItem.h"

class CFileListModel: public QStandardItemModel
{
    Q_OBJECT

public:
    CFileListModel(QObject *parent = 0);
	void clearAndResetModel();

	QSortFilterProxyModel* getProxyModel();
	QItemSelectionModel* getSelectionModel();

	void addItem(const COutputItem& outputItem);

	QVariant data(const QModelIndex& index, int role) const;

private:
	QWidget* parent_;

	QSortFilterProxyModel* fileListProxyModel_;
	QItemSelectionModel* fileListSelectionModel_; 

};

#endif // COUTPUT_LIST_MODEL_H
