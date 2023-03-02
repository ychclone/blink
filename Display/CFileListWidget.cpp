#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QUrl>

#include <QDebug>

#include "CFileListWidget.h"

CFileListWidget::CFileListWidget(QWidget *parent)
: QTreeView(parent)
{
	QFont currentFont = static_cast <QWidget*> (this)->font();
	fileFontSize_ = currentFont.pointSize();
}

void CFileListWidget::setFileListModel(CFileListModel *fileListModel)
{
	fileListModel_ = fileListModel;
}

void CFileListWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragStartPosition_ = event->pos();
	}
	QTreeView::mousePressEvent(event);
}

void CFileListWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		emit fileItemTriggered();
	}
    QTreeView::mouseReleaseEvent(event);
}

void CFileListWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Return) {
		emit fileItemTriggered();
	} else if ((event->key() == Qt::Key_Equal) && (event->modifiers() == Qt::ControlModifier)) {
		fileZoomIn();
	} else if ((event->key() == Qt::Key_Minus) && (event->modifiers() == Qt::ControlModifier)) {
		fileZoomOut();
	}
	QTreeView::keyPressEvent(event);
}

void CFileListWidget::updateOutputFont(const QFont& outputFont)
{
	QFont currentFont = static_cast <QWidget*> (this)->font();

	if (currentFont != outputFont) {
		static_cast <QWidget*> (this)->setFont(outputFont);

		fileFontSize_ = outputFont.pointSize();
		updateFileListWidget();
	}
}


QStringList CFileListWidget::getSelectedItemNameList()
{
    QModelIndexList indexSelectedList;
	QModelIndex indexSelected, mappedIndex;
	int rowSelected;
	QStandardItem* itemSelected;

    QString fileItemName;
	QStringList fileItemNameList;

	QSortFilterProxyModel* proxyModel;
	QItemSelectionModel* selectModel;

	proxyModel = static_cast <QSortFilterProxyModel *> (model());
	selectModel = static_cast <QItemSelectionModel* > (selectionModel());

    // get selected items index list
    indexSelectedList = selectModel->selectedIndexes();

	foreach (indexSelected, indexSelectedList) {
		// map back from proxy model
		mappedIndex = proxyModel->mapToSource(indexSelected);

		rowSelected = mappedIndex.row();

		if (indexSelected.isValid()) {
			itemSelected = fileListModel_->item(rowSelected, 0);
			if (itemSelected != 0) {
				fileItemName = itemSelected->text();
			};
		}

		// as all items in the same row with differnt columns will also be returned in selectedIndexes
		if (!fileItemNameList.contains(fileItemName)) {
			// not add output name to the list if already added
			fileItemNameList += fileItemName;
		}
	}

    return fileItemNameList;
}

void CFileListWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton)) {
		return;
	}
	if ((event->pos() - dragStartPosition_).manhattanLength() < QApplication::startDragDistance()) {
		return;
	}

	QList <QUrl> urlList;
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	QString filePath;
	QStringList filePathList;

	filePathList = getSelectedItemNameList();

	foreach (filePath, filePathList) {
		qDebug() << filePath;
		QUrl url = QUrl::fromLocalFile(filePath);
		urlList.append(url);
	}

	mimeData->setUrls(urlList);
	drag->setMimeData(mimeData);

	Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction);

	QTreeView::mouseMoveEvent(event);
}

void CFileListWidget::updateFileListWidget()
{
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
}

void CFileListWidget::fileZoomIn()
{
	fileFontSize_++;
	QFont fnt = static_cast <QWidget*> (this)->font();
	fnt.setPointSize(fileFontSize_);
	static_cast <QWidget*> (this)->setFont(fnt);

	updateFileListWidget();
}

void CFileListWidget::fileZoomOut()
{
	if (fileFontSize_ > 1) {
		fileFontSize_--;
		QFont fnt = static_cast <QWidget*> (this)->font();
		fnt.setPointSize(fileFontSize_);
		static_cast <QWidget*> (this)->setFont(fnt);
	}

	updateFileListWidget();
}


void CFileListWidget::wheelEvent(QWheelEvent *e)
{
	/* YCH modified (begin), commented temporary */
	/*
    if (e->modifiers() == Qt::ControlModifier) {
        e->accept();
        if (e->delta() > 0) {
		   fileZoomIn();
		} else {
		   fileZoomOut();
		}
	}
	*/
	/* YCH modified (end), commented temporary */
	QTreeView::wheelEvent(e);
}


