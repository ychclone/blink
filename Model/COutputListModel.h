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

class COutputListModel: public QStandardItemModel
{
    Q_OBJECT

public:
    COutputListModel(QObject *parent = 0);
	void clearAndResetModel();

	QSortFilterProxyModel* getProxyModel();
	QItemSelectionModel* getSelectionModel();

	void addItem(const COutputItem& outputItem);

	QVariant data(const QModelIndex& index, int role) const;

private:
	QWidget* m_parent;

	QSortFilterProxyModel* m_outputListProxyModel;
	QItemSelectionModel* m_outputListSelectionModel; 

};

#endif // COUTPUT_LIST_MODEL_H
