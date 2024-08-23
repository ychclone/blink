#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QDateTime>

#include "CProjectListModel.h"
#include "Display/CProjectDlg.h"

CProjectListModel::CProjectListModel(QObject *parent)
	: QStandardItemModel(0, 4, parent)
{
	parent_ = static_cast<QWidget*> (parent);
	confManager_ = CConfigManager::getInstance();

    // view when list is empty, view with content in CMainWindow::loadProjectList()
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Tag Update Datatime"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Project Create Datetime"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Labels"));
}

void CProjectListModel::addProjectItem(const CProjectItem& projectItem)
{
	QDateTime tagUpdateDateTime, projectCreateDatetime;

    insertRow(0);

	setData(index(0, 0), projectItem.name_);

	if (projectItem.tagUpdateDateTime_ == "") {
		setData(index(0, 1), "");
	} else {
		tagUpdateDateTime = QDateTime::fromString(projectItem.tagUpdateDateTime_, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 1), tagUpdateDateTime);
	}

	if (projectItem.projectCreateDateTime_ == "") {
		setData(index(0, 2), "");
	} else {
		projectCreateDatetime = QDateTime::fromString(projectItem.projectCreateDateTime_, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 2), projectCreateDatetime);
	}

    setData(index(0, 3), projectItem.labels_);

}

bool CProjectListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent)
{
    QList <QUrl> urlList;

    QFileInfo info;
    QString fName;

    CProjectItem droppedItem;

    urlList = data->urls(); // retrieve list of urls

    foreach(QUrl url, urlList) // iterate over list
    {
        //qDebug("url = %s", url.toString().toLatin1().constData());
        fName = url.toLocalFile();
        //qDebug("fName = %s", fName.toLatin1().constData());
        info.setFile(fName);

        if (info.isDir()) {

            // fill default value according to item dropped
            droppedItem.name_ = info.fileName();
            droppedItem.srcDir_ = fName;

			QString defaultMaskForNewProject = confManager_->getAppSettingValue("defaultMaskForNewProject").toString();

			if (defaultMaskForNewProject == "") {
				droppedItem.srcMask_ = "*";
			} else {
				droppedItem.srcMask_ =  defaultMaskForNewProject;
			}

            QString defaultDirToExclude = confManager_->getAppSettingValue("defaultDirToExclude").toString();
            if (defaultDirToExclude == "") {
                droppedItem.dirToExclude_ = ".git";
            } else {
                droppedItem.dirToExclude_ = defaultDirToExclude;
            }

            QString defaultFileMaskToExclude = confManager_->getAppSettingValue("defaultFileMaskToExclude").toString();
            if (defaultFileMaskToExclude == "") {
                droppedItem.fileMaskToExclude_ = "*.tmp";
            } else {
                droppedItem.fileMaskToExclude_ = defaultFileMaskToExclude;
            }

            droppedItem.headerMask_ = "";
            droppedItem.labels_ = "";

            QDialog* dialog = new CProjectDlg(droppedItem.name_, droppedItem, parent_);
            dialog->exec();
        } else {
            QMessageBox::information(parent_, "Add project", "Only folder is supported!", QMessageBox::Ok);
        }
    }
    return true;
}

QStringList CProjectListModel::mimeTypes () const
{
    QStringList qstrList;
    // list of accepted mime types accepted
    qstrList.append("text/uri-list");
    return qstrList;
}

Qt::DropActions CProjectListModel::supportedDropActions () const
{
    // actions supported
    return Qt::CopyAction | Qt::MoveAction;
}



