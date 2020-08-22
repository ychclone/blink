#include "COutputListModel.h"

COutputListModel::COutputListModel(QObject *parent)
	: QStandardItemModel(0, 3, parent) // 3 column
{
	m_parent = static_cast<QWidget*> (parent);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Last Modified"));
	setHeaderData(2, Qt::Horizontal, QObject::tr("Size"));

	m_outputListProxyModel = new QSortFilterProxyModel;
	m_outputListProxyModel->setSourceModel(static_cast <QStandardItemModel*> (this));
    m_outputListProxyModel->setDynamicSortFilter(true);

	m_outputListSelectionModel = new QItemSelectionModel(m_outputListProxyModel);

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
	return m_outputListProxyModel;
}

QItemSelectionModel* COutputListModel::getSelectionModel()
{
	return m_outputListSelectionModel;
}

void COutputListModel::addItem(const COutputItem& outputItem)
{
	quint64 fileSize;
	QDateTime lastModifiedDateTime;

	insertRow(0);

	setData(index(0, 0), outputItem.m_fileName);

	setData(index(0, 0), QIcon(":/Icons/text-x-preview.ico"), Qt::DecorationRole);

	lastModifiedDateTime = QDateTime::fromString(outputItem.m_fileLastModified, "dd/MM/yyyy HH:mm:ss");
	setData(index(0, 1), lastModifiedDateTime);

	fileSize = outputItem.m_fileSize;

	setData(index(0, 2), fileSize);
}

QVariant COutputListModel::data(const QModelIndex& index, int role) const
{
	return QStandardItemModel::data(index,role);
}




