#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QDateTime>

#include "CProfileListModel.h"
#include "Display/CProfileDlg.h"

CProfileListModel::CProfileListModel(QObject *parent)
	: QStandardItemModel(0, 4, parent)
{
	m_parent = static_cast<QWidget*> (parent);

    // view when list is empty, view with content in CMainWindow::loadProfileList()
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Tag Update Datatime"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Profile Create Datetime"));
    setHeaderData(3, Qt::Horizontal, QObject::tr("Labels"));
}

void CProfileListModel::addProfileItem(const CProfileItem& profileItem)
{
	QDateTime tagUpdateDateTime, profileCreateDatetime;

    insertRow(0);

	setData(index(0, 0), profileItem.m_name);

	if (profileItem.m_tagUpdateDateTime == "") {
		setData(index(0, 1), "");
	} else {
		tagUpdateDateTime = QDateTime::fromString(profileItem.m_tagUpdateDateTime, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 1), tagUpdateDateTime);
	}

	if (profileItem.m_profileCreateDateTime == "") {
		setData(index(0, 2), "");
	} else {
		profileCreateDatetime = QDateTime::fromString(profileItem.m_profileCreateDateTime, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 2), profileCreateDatetime);
	}

    setData(index(0, 3), profileItem.m_labels);

}

bool CProfileListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
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
            droppedItem.m_name = info.fileName();
            droppedItem.m_srcDir = fName;
            droppedItem.m_srcMask = "*.cpp *.c *.go *.js *.py *.ts";
            droppedItem.m_headerMask = "*.hpp *.h";
            droppedItem.m_labels = "";

            QDialog* dialog = new CProfileDlg(droppedItem.m_name, droppedItem, m_parent);
            dialog->exec();

        } else {
            QMessageBox::information(m_parent, "Add profile", "Only folder is supported!", QMessageBox::Ok);
        }
    }
    return true;
}

QStringList CProfileListModel::mimeTypes () const
{
    QStringList qstrList;
    // list of accepted mime types accepted
    qstrList.append("text/uri-list");
    return qstrList;
}

Qt::DropActions CProfileListModel::supportedDropActions () const
{
    // actions supported
    return Qt::CopyAction | Qt::MoveAction;
}



