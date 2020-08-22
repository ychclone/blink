#include <QVariant>
#include <QStandardItem>

#include <QDebug>

#include "CFindReplaceModel.h"

CFindReplaceModel::CFindReplaceModel()
{
	fileListModel_ = new QStandardItemModel();
}

CFindReplaceModel::~CFindReplaceModel()
{
	delete fileListModel_;
}

// fileList passed by value instead of reference as caller may use local variables in stack
void CFindReplaceModel::setFileList(QStringList fileList)
{
	fileList_ = fileList;

	fileListModel_->clear();

    foreach (const QString &filePath, fileList) {
		QStandardItem* item = new QStandardItem(filePath);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setData(QVariant(Qt::Checked), Qt::CheckStateRole);
		fileListModel_->appendRow(item);
    }
}

QStringList CFindReplaceModel::getFileList()
{
	QModelIndex parent;
	int i;
	QStringList selectedFileList;

    for (i = 0; i < fileListModel_->rowCount(parent); i++) {
        QStandardItem* item = fileListModel_->item(i, 0); // always column 0
		if (item->checkState() == Qt::Checked) {
			selectedFileList.append(item->text());
		}
    }

	return selectedFileList;
}

QStringList CFindReplaceModel::selectAllFiles()
{
	QModelIndex parent;
	int i;
	QStringList selectedFileList;

    for (i = 0; i < fileListModel_->rowCount(parent); i++) {
        QStandardItem* item = fileListModel_->item(i, 0); // always column 0
		item->setCheckState(Qt::Checked);
    }

	return selectedFileList;
}

QStringList CFindReplaceModel::clearSelectAllFiles()
{
	QModelIndex parent;
	int i;
	QStringList selectedFileList;

    for (i = 0; i < fileListModel_->rowCount(parent); i++) {
        QStandardItem* item = fileListModel_->item(i, 0); // always column 0
		item->setCheckState(Qt::Unchecked);
    }

	return selectedFileList;
}

QStandardItemModel* CFindReplaceModel::getFileListModel() {
	return fileListModel_;
}


