#include "CGroupListWidget.h"

CGroupListWidget::CGroupListWidget(QWidget *parent)
: QTreeView(parent)
{
	QFont currentFont = static_cast <QWidget*> (this)->font(); 
	groupFontSize_ = currentFont.pointSize();
}

void CGroupListWidget::updateGroupListWidget()
{
	resizeColumnToContents(0);
	resizeColumnToContents(1);
	resizeColumnToContents(2);
	resizeColumnToContents(3);
}                  

void CGroupListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit groupItemTriggered(); 
    QTreeView::mouseDoubleClickEvent(event);
}

void CGroupListWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Return) {
		emit groupItemTriggered(); 
	} else if ((event->key() == Qt::Key_Equal) && (event->modifiers() == Qt::ControlModifier)) { 
		groupZoomIn();
	} else if ((event->key() == Qt::Key_Minus) && (event->modifiers() == Qt::ControlModifier)) { 
		groupZoomOut();
	} 
	
	QTreeView::keyPressEvent(event);
}

void CGroupListWidget::updateGroupFont(const QFont& outputFont) 
{
	QFont currentFont = static_cast <QWidget*> (this)->font();

	if (currentFont != outputFont) { 
		static_cast <QWidget*> (this)->setFont(outputFont);

		groupFontSize_ = outputFont.pointSize();
		updateGroupListWidget();
	}
}

void CGroupListWidget::groupZoomIn()
{
	groupFontSize_++;
	QFont fnt = static_cast <QWidget*> (this)->font();;
	fnt.setPointSize(groupFontSize_); 
	static_cast <QWidget*> (this)->setFont(fnt);

	updateGroupListWidget();
}

void CGroupListWidget::groupZoomOut()
{
	if (groupFontSize_ > 1) {
		groupFontSize_--;
		QFont fnt = static_cast <QWidget*> (this)->font();
		fnt.setPointSize(groupFontSize_);  
		static_cast <QWidget*> (this)->setFont(fnt);

		updateGroupListWidget(); 
	}
}

void CGroupListWidget::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() == Qt::ControlModifier) {
        e->accept();
        if (e->delta() > 0) {
		   groupZoomIn();
		} else {
		   groupZoomOut();
		}
	}
	QTreeView::wheelEvent(e); 
}




