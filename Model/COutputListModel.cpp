#include "COutputListModel.h"

COutputListModel::COutputListModel(QObject *parent)
	: QStandardItemModel(0, 3, parent) // 3 column
{
	parent_ = static_cast<QWidget*> (parent);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Last Modified"));
	setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));

	outputListProxyModel_ = new QSortFilterProxyModel;
	outputListProxyModel_->setSourceModel(static_cast <QStandardItemModel*> (this));
    outputListProxyModel_->setDynamicSortFilter(true);

	outputListSelectionModel_ = new QItemSelectionModel(outputListProxyModel_);

	qDebug() << "COutputListModel, this = " << this;
}

void COutputListModel::clearAndResetModel()
{
  	clear();  // header data will also be clear

	setColumnCount(3); // need to set back column count when QStandardItemModel clear otherwise setData will return false

    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Last Modified"));
	setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));
}

QSortFilterProxyModel* COutputListModel::getProxyModel()
{
	return outputListProxyModel_;
}

QItemSelectionModel* COutputListModel::getSelectionModel()
{
	return outputListSelectionModel_;
}

void COutputListModel::addItem(const COutputItem& outputItem)
{
	quint64 fileSize;
	QDateTime lastModifiedDateTime;

	insertRow(0);

	setData(index(0, 0), outputItem.fileName_);

	setData(index(0, 0), QIcon(":/Icons/text-x-preview.ico"), Qt::DecorationRole);

	lastModifiedDateTime = QDateTime::fromString(outputItem.fileLastModified_, "dd/MM/yyyy HH:mm:ss");
	setData(index(0, 1), lastModifiedDateTime);

	fileSize = outputItem.fileSize_;

	setData(index(0, 2), fileSize);
}

QVariant COutputListModel::data(const QModelIndex& index, int role) const
{
	return QStandardItemModel::data(index,role);
}




