#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QDateTime>

#include "CProjectListModel.h"
#include "Display/CProfileDlg.h"

CProjectListModel::CProjectListModel(QObject *parent)
	: QStandardItemModel(0, 4, parent)
{
	parent_ = static_cast<QWidget*> (parent);

    // view when list is empty, view with content in CMainWindow::loadProjectList()
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Tag Update Datatime"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Profile Create Datetime"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Labels"));
}

void CProjectListModel::addProfileItem(const CProfileItem& profileItem)
{
	QDateTime tagUpdateDateTime, profileCreateDatetime;

    insertRow(0);

	setData(index(0, 0), profileItem.name_);

	if (profileItem.tagUpdateDateTime_ == "") {
		setData(index(0, 1), "");
	} else {
		tagUpdateDateTime = QDateTime::fromString(profileItem.tagUpdateDateTime_, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 1), tagUpdateDateTime);
	}

	if (profileItem.profileCreateDateTime_ == "") {
		setData(index(0, 2), "");
	} else {
		profileCreateDatetime = QDateTime::fromString(profileItem.profileCreateDateTime_, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 2), profileCreateDatetime);
	}

    setData(index(0, 3), profileItem.labels_);

}

bool CProjectListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent)
{
    QList <QUrl> urlList;

    QFileInfo info;
    QString fName;

    CProfileItem droppedItem;

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
            droppedItem.srcMask_ = "*.cpp *.c *.go *.java *.js *.py *.scala *.ts";
            droppedItem.headerMask_ = "*.hpp *.h";
            droppedItem.labels_ = "";

            QDialog* dialog = new CProfileDlg(droppedItem.name_, droppedItem, parent_);
            dialog->exec();

        } else {
            QMessageBox::information(parent_, "Add profile", "Only folder is supported!", QMessageBox::Ok);
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



