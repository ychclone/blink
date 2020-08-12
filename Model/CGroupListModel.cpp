#include <QUrl>
#include <QFileInfo>
#include <QMimeData>
#include <QMessageBox>
#include <QDateTime>

#include "CGroupListModel.h" 
#include "Display/CGroupDlg.h"

CGroupListModel::CGroupListModel(QObject *parent)
	: QStandardItemModel(0, 4, parent)
{
	m_parent = static_cast<QWidget*> (parent);
    
	// view when list is empty, view with content in CMainWindow::loadGroupList() 
	setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Tag Update Datetime"));
    setHeaderData(2, Qt::Horizontal, QObject::tr("Group Create Datetime"));
	setHeaderData(3, Qt::Horizontal, QObject::tr("Labels")); 
}

void CGroupListModel::addGroupItem(const CGroupItem& groupItem)
{
	QDateTime tagUpdateDateTime, groupCreateDatetime;

    insertRow(0);

	setData(index(0, 0), groupItem.m_name);

	if (groupItem.m_tagUpdateDateTime == "") {
		setData(index(0, 1), "");
	} else {
		tagUpdateDateTime = QDateTime::fromString(groupItem.m_tagUpdateDateTime, "dd/MM/yyyy HH:mm:ss");  
		setData(index(0, 1), tagUpdateDateTime); 
	}

	if (groupItem.m_groupCreateDateTime == "") {
		setData(index(0, 2), ""); 
	} else {
		groupCreateDatetime = QDateTime::fromString(groupItem.m_groupCreateDateTime, "dd/MM/yyyy HH:mm:ss");
		setData(index(0, 2), groupCreateDatetime);
	}

	setData(index(0, 3), groupItem.m_labels); 
	
}

bool CGroupListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent)
{
    QList <QUrl> urlList;
    
    QFileInfo info;
    QString fName;

    CGroupItem droppedItem;

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

            QDialog* dialog = new CGroupDlg(droppedItem.m_name, droppedItem, m_parent);
            dialog->exec();

        } else {
            QMessageBox::information(m_parent, "Add group", "Only folder is supported!", QMessageBox::Ok);
        }
    }
    return true;
}

QStringList CGroupListModel::mimeTypes () const
{
    QStringList qstrList;
    // list of accepted mime types accepted
    qstrList.append("text/uri-list");
    return qstrList;
}

Qt::DropActions CGroupListModel::supportedDropActions () const
{
    // actions supported
    return Qt::CopyAction | Qt::MoveAction;
}



