#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QUrl>

#include <QDebug>

#include "COutputListWidget.h"

COutputListWidget::COutputListWidget(QWidget *parent)
: QTreeView(parent)
{
	QFont currentFont = static_cast <QWidget*> (this)->font(); 
	fileFontSize_ = currentFont.pointSize(); 
}

void COutputListWidget::setOutputListModel(COutputListModel *outputListModel)
{
	m_outputListModel = outputListModel;
} 

void COutputListWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_dragStartPosition = event->pos();
	}
	QTreeView::mousePressEvent(event);
}

void COutputListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit outputItemTriggered(); 
    QTreeView::mouseDoubleClickEvent(event);
}

void COutputListWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Return) {
		emit outputItemTriggered(); 
	} else if ((event->key() == Qt::Key_Equal) && (event->modifiers() == Qt::ControlModifier)) { 
		fileZoomIn();
	} else if ((event->key() == Qt::Key_Minus) && (event->modifiers() == Qt::ControlModifier)) { 
		fileZoomOut();
	} 
	QTreeView::keyPressEvent(event);
}

void COutputListWidget::updateOutputFont(const QFont& outputFont) 
{
	QFont currentFont = static_cast <QWidget*> (this)->font();

	if (currentFont != outputFont) { 
		static_cast <QWidget*> (this)->setFont(outputFont);

		fileFontSize_ = outputFont.pointSize();
		updateOutputListWidget();
	}
}


QStringList COutputListWidget::getSelectedItemNameList()
{
    QModelIndexList indexSelectedList;
	QModelIndex indexSelected, mappedIndex;
	int rowSelected;
	QStandardItem* itemSelected;
    
    QString outputItemName;
	QStringList outputItemNameList;

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
			itemSelected = m_outputListModel->item(rowSelected, 0);
			if (itemSelected != 0) {
				outputItemName = itemSelected->text();
			};
		}

		// as all items in the same row with differnt columns will also be returned in selectedIndexes
		if (!outputItemNameList.contains(outputItemName)) {
			// not add output name to the list if already added
			outputItemNameList += outputItemName;
		}
	}

    return outputItemNameList;
}

void COutputListWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton)) {
		return;
	}
	if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
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

void COutputListWidget::updateOutputListWidget()
{
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
}

void COutputListWidget::fileZoomIn()
{
	fileFontSize_++;
	QFont fnt = static_cast <QWidget*> (this)->font();
	fnt.setPointSize(fileFontSize_); 
	static_cast <QWidget*> (this)->setFont(fnt);

	updateOutputListWidget();
}

void COutputListWidget::fileZoomOut()
{
	if (fileFontSize_ > 1) {
		fileFontSize_--;
		QFont fnt = static_cast <QWidget*> (this)->font();
		fnt.setPointSize(fileFontSize_); 
		static_cast <QWidget*> (this)->setFont(fnt);
	}

	updateOutputListWidget(); 
}


void COutputListWidget::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        e->accept();
        if (e->delta() > 0) {
		   fileZoomIn();
		} else {
		   fileZoomOut();
		}
	}
	QTreeView::wheelEvent(e); 
}


