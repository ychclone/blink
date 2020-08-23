#include "CFileListModel.h"

CFileListModel::CFileListModel(QObject *parent)
	: QStandardItemModel(0, 3, parent) // 3 column
{
	parent_ = static_cast<QWidget*> (parent);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Last Modified"));
	setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));

	fileListProxyModel_ = new QSortFilterProxyModel;
	fileListProxyModel_->setSourceModel(static_cast <QStandardItemModel*> (this));
    fileListProxyModel_->setDynamicSortFilter(true);

	fileListSelectionModel_ = new QItemSelectionModel(fileListProxyModel_);

	qDebug() << "CFileListModel, this = " << this;
}

void CFileListModel::clearAndResetModel()
{
  	clear();  // header data will also be clear

	setColumnCount(3); // need to set back column count when QStandardItemModel clear otherwise setData will return false

    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Last Modified"));
	setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));
}

QSortFilterProxyModel* CFileListModel::getProxyModel()
{
	return fileListProxyModel_;
}

QItemSelectionModel* CFileListModel::getSelectionModel()
{
	return fileListSelectionModel_;
}

void CFileListModel::addItem(const CFileItem& fileItem)
{
	quint64 fileSize;
	QDateTime lastModifiedDateTime;

	insertRow(0);

	setData(index(0, 0), fileItem.fileName_);

	setData(index(0, 0), QIcon(":/Icons/text-x-preview.ico"), Qt::DecorationRole);

	lastModifiedDateTime = QDateTime::fromString(fileItem.fileLastModified_, "dd/MM/yyyy HH:mm:ss");
	setData(index(0, 1), lastModifiedDateTime);

	fileSize = fileItem.fileSize_;

	setData(index(0, 2), fileSize);
}

QVariant CFileListModel::data(const QModelIndex& index, int role) const
{
	return QStandardItemModel::data(index,role);
}




